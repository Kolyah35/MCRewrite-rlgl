#pragma once
#include <AABB.hpp>
#include <vector>

/*
 * Author: Ben Humphrey (DigiBen)
 * E-mail: digiben@gametutorials.com
 */

class Frustum {
private:
    float m_Frustum[6][4];

public:
    Frustum();

    static Frustum& get() {
        static Frustum frustum;
        return frustum;
    }

    void normalizePlane(int side);
    void calculateFrustum();

    bool pointInFrustum(float x, float y, float z);

    bool sphereInFrustum(float x, float y, float z, float radius);

    bool cubeInFrustum(Vector3 min, Vector3 max);
    bool cubeInFrustum(const AABB& aabb);
};