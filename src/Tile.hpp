#pragma once
#include <utils.hpp>
#include <vector>
#include <Level.hpp>

namespace Tile {
    void renderFace(std::vector<ChunkVertex>& vertices, Level& level, int tileID, Vector3 pos, Faces face);
}