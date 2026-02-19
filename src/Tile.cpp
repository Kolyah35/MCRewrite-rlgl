#include <Tile.hpp>

void Tile::renderFace(std::vector<ChunkVertex>& vertices, Level& level, int tileID, Vector3 pos, Faces face) {
    int tileX = tileID % 16;
    int tileY = tileID / 16;

    const float tileSize = 1.0f / 16.0f;

    float minU = tileX * tileSize;
    float maxU = minU + tileSize;
    float minV = tileY * tileSize;
    float maxV = minV + tileSize;

    const Vector3 shade = {0.6f, 1.f, 0.8f};
    float brightness;
    bool shadow;

    switch (face) {
    case Faces::Front:
        brightness = level.getBrightness(Vector3i(pos.x, pos.y, pos.z + 1)) * shade.z;
        shadow = !(brightness == shade.z);

        // clang-format off
        vertices.insert(vertices.end(), {
            {pos.x,     pos.y,     pos.z + 1, brightness, minU, maxV, shadow},
            {pos.x + 1, pos.y,     pos.z + 1, brightness, maxU, maxV, shadow},
            {pos.x + 1, pos.y + 1, pos.z + 1, brightness, maxU, minV, shadow},
            {pos.x,     pos.y + 1, pos.z + 1, brightness, minU, minV, shadow}
        });
        // clang-format on

        break;
    case Faces::Back:
        brightness = level.getBrightness(Vector3i(pos.x, pos.y, pos.z - 1)) * shade.z;
        shadow = !(brightness == shade.z);

        // clang-format off
        vertices.insert(vertices.end(), {
            {pos.x,     pos.y,     pos.z, brightness, maxU, maxV, shadow},
            {pos.x,     pos.y + 1, pos.z, brightness, maxU, minV, shadow},
            {pos.x + 1, pos.y + 1, pos.z, brightness, minU, minV, shadow},
            {pos.x + 1, pos.y,     pos.z, brightness, minU, maxV, shadow}
        });
        // clang-format on

        break;
    case Faces::Left:
        brightness = level.getBrightness(Vector3i(pos.x - 1, pos.y, pos.z)) * shade.x;
        shadow = !(brightness == shade.x);

        // clang-format off
        vertices.insert(vertices.end(), {
            {pos.x, pos.y,     pos.z,     brightness, minU, maxV, shadow},
            {pos.x, pos.y,     pos.z + 1, brightness, maxU, maxV, shadow},
            {pos.x, pos.y + 1, pos.z + 1, brightness, maxU, minV, shadow},
            {pos.x, pos.y + 1, pos.z,     brightness, minU, minV, shadow}
        });
        // clang-format on

        break;
    case Faces::Right:
        brightness = level.getBrightness(Vector3i(pos.x + 1, pos.y, pos.z)) * shade.x;
        shadow = !(brightness == shade.x);

        // clang-format off
        vertices.insert(vertices.end(), {
            {pos.x + 1, pos.y,     pos.z,     brightness, maxU, maxV, shadow},
            {pos.x + 1, pos.y + 1, pos.z,     brightness, maxU, minV, shadow},
            {pos.x + 1, pos.y + 1, pos.z + 1, brightness, minU, minV, shadow},
            {pos.x + 1, pos.y,     pos.z + 1, brightness, minU, maxV, shadow}
        });
        // clang-format on

        break;
    case Faces::Up:
        brightness = level.getBrightness(Vector3i(pos.x, pos.y + 1, pos.z)) * shade.y;
        shadow = !(brightness == shade.y);

        // clang-format off
        vertices.insert(vertices.end(), {
            {pos.x,     pos.y + 1, pos.z + 1, brightness, minU, maxV, shadow},
            {pos.x + 1, pos.y + 1, pos.z + 1, brightness, maxU, maxV, shadow},
            {pos.x + 1, pos.y + 1, pos.z,     brightness, maxU, minV, shadow},
            {pos.x,     pos.y + 1, pos.z,     brightness, minU, minV, shadow}
        });
        // clang-format on

        break;
    case Faces::Down:
        brightness = level.getBrightness(Vector3i(pos.x, pos.y - 1, pos.z)) * shade.y;
        shadow = !(brightness == shade.y);

        // clang-format off
        vertices.insert(vertices.end(), {
            {pos.x,     pos.y, pos.z,     brightness, minU, minV, shadow},
            {pos.x + 1, pos.y, pos.z,     brightness, maxU, minV, shadow},
            {pos.x + 1, pos.y, pos.z + 1, brightness, maxU, maxV, shadow},
            {pos.x,     pos.y, pos.z + 1, brightness, minU, maxV, shadow}
        });
        // clang-format on

        break;
    }
}
