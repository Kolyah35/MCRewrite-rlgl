#pragma once
#include <string_view>
#include <utils.hpp>
#include <Level.hpp>

class Game {
public:
    Game();

    static Game& get() {
        static Game inst;
        return inst;
    }

    int run();
    
    HitResult pick(const Vector3& start, const Vector3& direction, Level& level);
};