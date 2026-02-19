#include "Chunk.hpp"
#include <utils.hpp>
#include <Tile.hpp>
#include <Textures.hpp>

Chunk::Chunk(Level& level, const Vector3i& min, const Vector3i& max) : m_level(level), m_min(min), m_max(max), m_built(false) {
    m_vertices.clear();
}

void Chunk::render(int layer) {
    if (!m_built) {
        build();
    }

    if (m_vertices.empty()) {
        return;
    }
    
    rlBegin(RL_QUADS);
        rlColor4ub(0, 0, 0, 255);

        for (const auto& vertex : m_vertices) {
            if (layer != 0) {
                unsigned char brightness = static_cast<unsigned char>(vertex.brightness * 255);

                rlColor4ub(brightness, brightness, brightness, 255);
                rlTexCoord2f(vertex.u, vertex.v);
            }

            rlVertex3f(vertex.x, vertex.y, vertex.z);
        }
    rlEnd();
}

void Chunk::build() {
    if (buildThisFrame == 2) {
        return;
    }

    m_built = true;
    updates++;
    buildThisFrame++;
    m_vertices.clear();

    for (int x = m_min.x; x < m_max.x; x++) {
        for (int y = m_min.y; y < m_max.y; y++) {
            for (int z = m_min.z; z < m_max.z; z++) {
                if (m_level.isSolidTile({x, y, z})) {
                    int id = (y != m_level.getDepth() * 2 / 3) ? 1 : 0;

                    renderTile(Vector3i(x, y, z), id);
                }
            }
        }
    }
}

void Chunk::renderTile(Vector3i pos, int tileID) {
    if (!m_level.isSolidTile(Vector3i(pos.x + 1, pos.y, pos.z))) {
        Tile::renderFace(m_vertices, m_level, tileID, (Vector3)pos, Faces::Right);
    }

    if (!m_level.isSolidTile(Vector3i(pos.x - 1, pos.y, pos.z))) {
        Tile::renderFace(m_vertices, m_level, tileID, (Vector3)pos, Faces::Left);
    }

    if (!m_level.isSolidTile(Vector3i(pos.x, pos.y + 1, pos.z))) {
        Tile::renderFace(m_vertices, m_level, tileID, (Vector3)pos, Faces::Up);
    }

    if (!m_level.isSolidTile(Vector3i(pos.x, pos.y - 1, pos.z))) {
        Tile::renderFace(m_vertices, m_level, tileID, (Vector3)pos, Faces::Down);
    }

    if (!m_level.isSolidTile(Vector3i(pos.x, pos.y, pos.z + 1))) {
        Tile::renderFace(m_vertices, m_level, tileID, (Vector3)pos, Faces::Front);
    }

    if (!m_level.isSolidTile(Vector3i(pos.x, pos.y, pos.z - 1))) {
        Tile::renderFace(m_vertices, m_level, tileID, (Vector3)pos, Faces::Back);
    }
}