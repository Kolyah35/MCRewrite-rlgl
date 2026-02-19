#pragma once
#include <Level.hpp>
#include <AABB.hpp>

class Player {
public:
    Player(Level& level);

    void tick();

    void resetPos();
    void setPos(Vector3 pos);
    void turn(Vector2 delta);
    void move(Vector3 delta);
    void moveRelative(float xa, float za, float speed);
    inline const Vector2& getRot() const { return m_rot; }
    inline const Vector3& getPos() const { return m_pos; }
    inline const Vector3& getPrevPos() const { return m_prevPos; }
    inline const AABB& getAABB() const { return m_aabb; }

private:
    Level& m_level;
    AABB m_aabb;
    Vector3 m_pos;
    Vector3 m_prevPos;
    Vector2 m_rot;
    Vector3 m_motion;
    bool m_onGround;
};