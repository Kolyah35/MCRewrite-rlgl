#pragma once
#include <AABB.hpp>
#include <Level.hpp>
#include <utils.hpp>
#include <vector>
#include <rlgl.h>

#define CHUNK_SIZE 16

class Chunk {
public:
    inline static int updates = 0;
    inline static int buildThisFrame = 0;

    Chunk(Level& level, const Vector3i& min, const Vector3i& max);

    void render(int layer);
    void build();
    void renderTile(Vector3i pos, int tileID);

    inline void rebuild() { m_built = false; }
    inline AABB getBounds() const { return AABB((Vector3)m_min, (Vector3)m_max); }

private:
    Level& m_level;

    Vector3i m_min;
    Vector3i m_max;

    bool m_built;

    std::vector<ChunkVertex> m_vertices;
};