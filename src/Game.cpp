#include <raylib.h>
#include "Game.hpp"
#include <cmath>
#include <ctime>
#include <Timer.hpp>
#include <Level.hpp>
#include <Player.hpp>
#include <iostream>
#include <Textures.hpp>
#include <Chunk.hpp>
#include <Icon.hpp>
#include <rlgl.h>
#include <raymath.h>

#if defined(GRAPHICS_API_OPENGL_ES2)
#include "fog/glsl100.hpp"
#elif defined(GRAPHICS_API_OPENGL_21)
#include "fog/glsl120.hpp"
#elif defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_43)
#include "fog/glsl330.hpp"
#endif

Game::Game() {}

int Game::run() {
    static bool hasStarted = false;

    if (hasStarted) {
        return 0;
    }

    hasStarted = true;

    const int width = 1024;
    const int height = 768;

    InitWindow(width, height, "Game");
    Image icon = {
        .data = (void*)LWJGL_ICON_DATA_16x16,
        .width = 16,
        .height = 16,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    SetWindowIcon(icon);
    SetTargetFPS(0);

    rlClearColor(128, 204, 255, 255);
    rlEnableDepthTest();
    rlEnableBackfaceCulling();

    int texture = Textures::loadTexture("terrain.png", RL_TEXTURE_FILTER_NEAREST);

    if (!texture) {
        return -1;
    }

    Shader fogShader = LoadShaderFromMemory(fog_vs.begin(), fog_fs.begin());
    fogShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(fogShader, "matModel");
    fogShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(fogShader, "viewPos");

    DisableCursor();

    Timer timer(60);
    Level level(256, 256, 64);
    Player player(level);

    double lastTime = 0.0;
    HitResult hitResult;

    Matrix projection = MatrixPerspective(70.f * DEG2RAD, (float)width / (float)height, 0.05f, 1000.0f);
    rlSetMatrixProjection(projection);

    while (!WindowShouldClose()) {
        timer.advanceTime();

        for (uint32_t i = 0; i < timer.getTicks(); ++i) {
            player.tick();
        }

        player.turn(GetMouseDelta());

        if (hitResult.hit && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            level.setTile(Vector3i(hitResult.pos), 0);
        }

        if (hitResult.hit && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            auto pos = hitResult.pos;

            // Get position of the tile using face direction
            if (hitResult.face == Faces::Down) {
                pos.y--;
            }
            if (hitResult.face == Faces::Up) {
                pos.y++;
            }
            if (hitResult.face == Faces::Back) {
                pos.z--;
            }
            if (hitResult.face == Faces::Front) {
                pos.z++;
            }
            if (hitResult.face == Faces::Left) {
                pos.x--;
            }
            if (hitResult.face == Faces::Right) {
                pos.x++;
            }

            // Set the tile
            level.setTile(pos, 1);
        }

        if (IsKeyPressed(KEY_ENTER)) {
            level.save();
        }

        // begin render

        BeginDrawing();

        rlClearScreenBuffers();

        const auto& rot = player.getRot() * DEG2RAD;
        const auto& pos = player.getPos();

        Vector3 cameraPos = player.getPrevPos() + (pos - player.getPrevPos()) * timer.getPartialTicks();

        rlMatrixMode(RL_MODELVIEW);
        rlLoadIdentity();
        
        rlTranslatef(0, 0, -0.3f);

        rlRotatef(rot.y * RAD2DEG, 1, 0, 0);
        rlRotatef(rot.x * RAD2DEG + 90, 0, 1, 0);
        
        rlTranslatef(-cameraPos.x, -cameraPos.y, -cameraPos.z);

        SetShaderValue(fogShader, fogShader.locs[SHADER_LOC_VECTOR_VIEW], &cameraPos.x, SHADER_UNIFORM_VEC3);

        rlSetTexture(texture);
        level.render(1);
        rlSetTexture(0);

        BeginShaderMode(fogShader);
        level.render(0);
        EndShaderMode();
        
        Vector3 cameraPosition(pos.x, pos.y, pos.z);
        Vector3 direction = Vector3(cos(rot.x) * cos(-rot.y), sin(-rot.y), sin(rot.x) * cos(-rot.y));

        hitResult = pick(cameraPosition, direction, level);

        if (hitResult.hit) {
            rlSetTexture(texture);
            level.renderHit(hitResult);
            rlSetTexture(0);
        }

        EndDrawing();

        auto currentTime = GetTime();
        if (GetTime() >= lastTime + 1.0) {
            std::cout << GetFPS() << " fps, " << Chunk::updates << std::endl;

            Chunk::updates = 0;
            lastTime = currentTime;
        }
    }

    level.save();

    CloseWindow();

    return 0;
}

HitResult Game::pick(const Vector3& start, const Vector3& direction, Level& level) {
    HitResult result;

    const Vector3 dir = Vector3Normalize(direction);
    Vector3 currentPos = start;

    // DDA (Digital Differential Analyzer)
    Vector3i mapPos = Vector3i(floor(currentPos.x), floor(currentPos.y), floor(currentPos.z));
    Vector3 deltaDist = Vector3(std::abs(1.0f / dir.x), std::abs(1.0f / dir.y), std::abs(1.0f / dir.z));

    Vector3i step;
    Vector3 sideDist;

    if (dir.x < 0) {
        step.x = -1;
        sideDist.x = (currentPos.x - mapPos.x) * deltaDist.x;
    } else {
        step.x = 1;
        sideDist.x = (mapPos.x + 1.0f - currentPos.x) * deltaDist.x;
    }
    if (dir.y < 0) {
        step.y = -1;
        sideDist.y = (currentPos.y - mapPos.y) * deltaDist.y;
    } else {
        step.y = 1;
        sideDist.y = (mapPos.y + 1.0f - currentPos.y) * deltaDist.y;
    }
    if (dir.z < 0) {
        step.z = -1;
        sideDist.z = (currentPos.z - mapPos.z) * deltaDist.z;
    } else {
        step.z = 1;
        sideDist.z = (mapPos.z + 1.0f - currentPos.z) * deltaDist.z;
    }

    // DDA cycle
    float traveled = 0.0f;
    while (traveled < 3.0f) {
        if (sideDist.x < sideDist.y && sideDist.x < sideDist.z) {
            traveled = sideDist.x;
            sideDist.x += deltaDist.x;
            mapPos.x += step.x;
            result.face = step.x < 0 ? Faces::Right : Faces::Left;
        } else if (sideDist.y < sideDist.z) {
            traveled = sideDist.y;
            sideDist.y += deltaDist.y;
            mapPos.y += step.y;
            result.face = step.y < 0 ? Faces::Up : Faces::Down;
        } else {
            traveled = sideDist.z;
            sideDist.z += deltaDist.z;
            mapPos.z += step.z;
            result.face = step.z < 0 ? Faces::Front : Faces::Back;
        }

        if (level.isSolidTile(mapPos)) {
            result.hit = true;
            result.pos = (Vector3)mapPos;
            break;
        }
    }

    return result;
}