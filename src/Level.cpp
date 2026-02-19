#include <raylib.h>
#include "Level.hpp"
#include <Frustum.hpp>
#include <Chunk.hpp>
#include <memory>
#include <Tile.hpp>
#include <fstream>
#include <iostream>
#include <rlgl.h>

Level::Level(int width, int height, int depth) : m_width(width), m_height(height), m_depth(depth), m_lightDepths(width * height) {
    if (!load()) {
        m_blocks = new uint8_t[width * height * depth];

        // Fill level with tiles
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < depth; y++) {
                for (int z = 0; z < height; z++) {
                    // Calculate index from x, y and z
                    int index = (y * m_height + z) * m_width + x;

                    // Fill level with tiles
                    m_blocks[index] = (uint8_t)((y <= depth * 2 / 3) ? 1 : 0);
                }
            }
        }
    }

    Vector3i chunkCount = {m_width / CHUNK_SIZE, m_depth / CHUNK_SIZE, m_height / CHUNK_SIZE};

    m_chunks.resize(chunkCount.x * chunkCount.y * chunkCount.z);

    for (int x = 0; x < chunkCount.x; x++) {
        for (int y = 0; y < chunkCount.y; y++) {
            for (int z = 0; z < chunkCount.z; z++) {
                Vector3i min = {x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE};

                Vector3i max = {std::min(m_width, (x + 1) * CHUNK_SIZE), std::min(m_depth, (y + 1) * CHUNK_SIZE), std::min(m_height, (z + 1) * CHUNK_SIZE)};

                m_chunks[(x + y * chunkCount.x) * chunkCount.z + z] = std::make_shared<Chunk>(*this, min, max);
            }
        }
    }

    calcLightDepths(0, 0, width, height);
}

bool Level::isSolidTile(Vector3i pos) {
    if (pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x >= m_width || pos.y >= m_depth || pos.z >= m_height) {
        return false;
    }

    int index = (pos.y * m_height + pos.z) * m_width + pos.x;

    return m_blocks[index] != 0;
}

void Level::render(int layer) {
    Frustum& frustum = Frustum::get();
    frustum.calculateFrustum();

    Chunk::buildThisFrame = 0;

    for (const auto& chunk : m_chunks) {
        if (frustum.cubeInFrustum(chunk->getBounds())) {
            chunk->render(layer);
        }
    }
}

void Level::renderHit(const HitResult& hit) {
    rlSetBlendMode(RL_BLEND_ADDITIVE);
    rlBegin(RL_QUADS);
    rlColor4f(1.f, 1.f, 1.f, (float)(std::sin(GetTime() * 10.f) * 0.2f) + 0.4f);

    static std::vector<ChunkVertex> hitTile(4);
    hitTile.clear();

    Tile::renderFace(hitTile, *this, 1, hit.pos, hit.face);

    for (auto& vertex : hitTile) {
        rlTexCoord2f(vertex.u, vertex.v);
        rlVertex3f(vertex.x, vertex.y, vertex.z);
    }
    rlEnd();
    rlSetBlendMode(RL_BLEND_ALPHA);
}

void Level::calcLightDepths(int minX, int minZ, int maxX, int maxZ) {
    for (int x = minX; x < minX + maxX; x++) {
        for (int z = minZ; z < minZ + maxZ; z++) {
            int prevDepth = m_lightDepths[x + z * m_width];

            int depth = m_depth - 1;
            while (depth > 0 && !isSolidTile(Vector3i(x, depth, z))) {
                depth--;
            }

            m_lightDepths[x + z * m_width] = depth;

            if (prevDepth != depth) {
                int minTileChangeY = std::min(prevDepth, depth);
                int maxTileChangeY = std::max(prevDepth, depth);

                rebuildChunks(Vector3i(x - 1, minTileChangeY - 1, z - 1), Vector3i(x + 1, maxTileChangeY + 1, z + 1));
            }
        }
    }
}

void Level::rebuildChunks(Vector3i min, Vector3i max) {
    min /= CHUNK_SIZE;
    max /= CHUNK_SIZE;

    min.x = std::max(min.x, 0);
    min.y = std::max(min.y, 0);
    min.z = std::max(min.z, 0);

    max.x = std::min(max.x, m_width / CHUNK_SIZE - 1);
    max.y = std::min(max.y, m_depth / CHUNK_SIZE - 1);
    max.z = std::min(max.z, m_height / CHUNK_SIZE - 1);

    for (int x = min.x; x <= max.x; x++) {
        for (int y = min.y; y <= max.y; y++) {
            for (int z = min.z; z <= max.z; z++) {
                auto chunk = m_chunks[(x + y * (m_width / CHUNK_SIZE)) * (m_height / CHUNK_SIZE) + z];
                chunk->rebuild();
            }
        }
    }
}

void Level::setTile(Vector3i pos, int id) {
    if (pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x >= m_width || pos.y >= m_depth || pos.z >= m_height) {
        return;
    }

    m_blocks[(pos.y * m_height + pos.z) * m_width + pos.x] = (uint8_t)id;

    calcLightDepths(pos.x, pos.z, 1, 1);
    rebuildChunks(pos - 1, pos + 1);
}

float Level::getBrightness(Vector3i pos) {
    float dark = 0.8f;
    float light = 1.0f;

    if (pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x >= m_width || pos.y >= m_depth || pos.z >= m_height) {
        return light;
    }

    if (pos.y < m_lightDepths[pos.x + pos.z * m_width]) {
        return dark;
    }

    return light;
}

std::vector<AABB> Level::getCubes(const AABB& other) {
    std::vector<AABB> aabbs;

    auto pos0 = other.min();
    auto pos1 = Vector3AddValue(other.max(), 1.f);

    if (pos0.x < 0) {
        pos0.x = 0;
    }

    if (pos0.y < 0) {
        pos0.y = 0;
    }

    if (pos0.z < 0) {
        pos0.z = 0;
    }

    if (pos1.x > m_width) {
        pos1.x = m_width;
    }

    if (pos1.y > m_depth) {
        pos1.y = m_depth;
    }

    if (pos1.z > m_height) {
        pos1.z = m_height;
    }

    for (int x = pos0.x; x < pos1.x; x++) {
        for (int y = pos0.y; y < pos1.y; y++) {
            for (int z = pos0.z; z < pos1.z; z++) {
                if (isSolidTile({x, y, z})) {
                    aabbs.push_back(AABB(Vector3(x, y, z), Vector3AddValue(Vector3(x, y, z), 1.f)));
                }
            }
        }
    }

    return aabbs;
}

void Level::save() {
    auto file = std::ofstream("level.dat", std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file level.dat" << std::endl;
        return;
    }

    int outSize;
    unsigned char* compressed = CompressData((const unsigned char*)m_blocks, m_width * m_height * m_depth, &outSize);

    file.write("\x1f\x8b\x08\0\0\0\0\0\0\xff", 10);
    file.write((const char*)compressed, outSize);
    file.close();
}

bool Level::load() {
    int size;
    auto data = LoadFileData("level.dat", &size);

    if (!data) return false;

    m_blocks = DecompressData(data + 10, size - 10, &size);
    MemFree(data);

    return true;
}