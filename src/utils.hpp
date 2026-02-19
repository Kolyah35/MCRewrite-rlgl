#pragma once
#include <raymath.h>

struct ChunkVertex {
    float x, y, z;
    float brightness;
    float u, v;
};

enum class Faces {
    Up,    // y+
    Down,  // y-
    Back,  // z+
    Front, // z-
    Left,  // x+
    Right  // x-
};

struct HitResult {
    bool hit = false;

    Vector3 pos;
    Faces face;
};

struct Vector3i {
    int x;
    int y;
    int z;

    Vector3i() = default;
    Vector3i(int x, int y, int z) { this->x = x; this->y = y; this->z = z; }
    Vector3i(const Vector3& vecf) { x = (int)vecf.x; y = (int)vecf.y; z = (int)vecf.z; }

    explicit operator Vector3() const { return {(float)x, (float)y, (float)z}; }

    Vector3i operator+(const Vector3i& rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
    Vector3i operator-(const Vector3i& rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
    
    Vector3i operator+(const int& value) const { return {x + value, y + value, z + value}; }
    Vector3i operator-(const int& value) const { return {x - value, y - value, z - value}; }

    Vector3i& operator+=(const Vector3i& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    Vector3i& operator-=(const Vector3i& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }

    Vector3i operator*(int scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    Vector3i operator/(int scalar) const { return {x / scalar, y / scalar, z / scalar}; }

    Vector3i& operator*=(int scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vector3i& operator/=(int scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    bool operator==(const Vector3i& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    bool operator!=(const Vector3i& rhs) const { return !(*this == rhs); }
};
