#pragma once
#include <AABB.hpp>

#include <vector>
#include <memory>

#include <utils.hpp>

class Chunk;

class Level {
public:
    Level(int width, int height, int depth);

    void render(int layer);
    void renderHit(const HitResult& hit);

    bool isSolidTile(Vector3i pos);
    void calcLightDepths(int minX, int minZ, int maxX, int maxZ);
    void rebuildChunks(Vector3i min, Vector3i max);

    void setTile(Vector3i pos, int id);
    float getBrightness(Vector3i pos);
    std::vector<AABB> getCubes(const AABB& other);

    inline auto getWidth() const { return m_width; }
    inline auto getHeight() const { return m_height; }
    inline auto getDepth() const { return m_depth; }

    void save();
    bool load();

private:
    uint8_t* m_blocks;
    std::vector<int> m_lightDepths;
    std::vector<std::shared_ptr<Chunk>> m_chunks;

    int m_width;
    int m_height;
    int m_depth;
};