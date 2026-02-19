#pragma once
#include <raymath.h>

class AABB {
public:
    AABB(Vector3 min = Vector3(0.0f), Vector3 max = Vector3(0.0f));

    AABB expand(Vector3 size);
    AABB grow(Vector3 size);

    float clipXCollide(AABB& other, float xa);
    float clipYCollide(AABB& other, float ya);
    float clipZCollide(AABB& other, float za);
    bool intersects(AABB& other);

    void move(Vector3 delta);

    inline const Vector3 min() const { return m_min; }
    inline const Vector3 max() const { return m_max; }

private:
    Vector3 m_min;
    Vector3 m_max;
};