#include "Frustum.hpp"
#include <AABB.hpp>
#include <rlgl.h>

#define RIGHT   0 // The RIGHT side of the frustum
#define LEFT    1 // The LEFT      side of the frustum
#define BOTTOM  2 // The BOTTOM side of the frustum
#define TOP     3 // The TOP side of the frustum
#define BACK    4 // The BACK     side of the frustum
#define FRONT   5 // The FRONT side of the frustum

#define A 0 // The X value of the plane's normal
#define B 1 // The Y value of the plane's normal
#define C 2 // The Z value of the plane's normal
#define D 3 // The distance the plane is from the origin

Frustum::Frustum() : m_Frustum{0.f} {}

///////////////////////////////// NORMALIZE PLANE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This normalizes a plane (A side) from a given frustum.
/////
///////////////////////////////// NORMALIZE PLANE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Frustum::normalizePlane(int side) {
    // Here we calculate the magnitude of the normal to the plane (point A B C)
    // Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
    // To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
    float magnitude = (float) sqrt(m_Frustum[side][A] * m_Frustum[side][A] +
            m_Frustum[side][B] * m_Frustum[side][B] + m_Frustum[side][C] * m_Frustum[side][C]);

    // Then we divide the plane's values by it's magnitude.
    // This makes it easier to work with.
    m_Frustum[side][A] /= magnitude;
    m_Frustum[side][B] /= magnitude;
    m_Frustum[side][C] /= magnitude;
    m_Frustum[side][D] /= magnitude;
}


///////////////////////////////// CALCULATE FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This extracts our frustum from the projection and modelview matrix.
/////
///////////////////////////////// CALCULATE FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Frustum::calculateFrustum() {
    Matrix proj = rlGetMatrixProjection();           // This will hold our projection matrix
    Matrix modl = rlGetMatrixModelview();           // This will hold our modelview matrix
    Matrix clip;           // This will hold the clipping planes

    // Now that we have our modelview and projection matrix, if we combine these 2 matrices,
    // it will give us our clipping planes.  To combine 2 matrices, we multiply them.

    clip.m0 = modl.m0 * proj.m0 + modl.m1 * proj.m4 + modl.m2 * proj.m8 + modl.m3 * proj.m12;
    clip.m1 = modl.m0 * proj.m1 + modl.m1 * proj.m5 + modl.m2 * proj.m9 + modl.m3 * proj.m13;
    clip.m2 = modl.m0 * proj.m2 + modl.m1 * proj.m6 + modl.m2 * proj.m10 + modl.m3 * proj.m14;
    clip.m3 = modl.m0 * proj.m3 + modl.m1 * proj.m7 + modl.m2 * proj.m11 + modl.m3 * proj.m15;

    clip.m4 = modl.m4 * proj.m0 + modl.m5 * proj.m4 + modl.m6 * proj.m8 + modl.m7 * proj.m12;
    clip.m5 = modl.m4 * proj.m1 + modl.m5 * proj.m5 + modl.m6 * proj.m9 + modl.m7 * proj.m13;
    clip.m6 = modl.m4 * proj.m2 + modl.m5 * proj.m6 + modl.m6 * proj.m10 + modl.m7 * proj.m14;
    clip.m7 = modl.m4 * proj.m3 + modl.m5 * proj.m7 + modl.m6 * proj.m11 + modl.m7 * proj.m15;

    clip.m8 = modl.m8 * proj.m0 + modl.m9 * proj.m4 + modl.m10 * proj.m8 + modl.m11 * proj.m12;
    clip.m9 = modl.m8 * proj.m1 + modl.m9 * proj.m5 + modl.m10 * proj.m9 + modl.m11 * proj.m13;
    clip.m10 = modl.m8 * proj.m2 + modl.m9 * proj.m6 + modl.m10 * proj.m10 + modl.m11 * proj.m14;
    clip.m11 = modl.m8 * proj.m3 + modl.m9 * proj.m7 + modl.m10 * proj.m11 + modl.m11 * proj.m15;

    clip.m12 = modl.m12 * proj.m0 + modl.m13 * proj.m4 + modl.m14 * proj.m8 + modl.m15 * proj.m12;
    clip.m13 = modl.m12 * proj.m1 + modl.m13 * proj.m5 + modl.m14 * proj.m9 + modl.m15 * proj.m13;
    clip.m14 = modl.m12 * proj.m2 + modl.m13 * proj.m6 + modl.m14 * proj.m10 + modl.m15 * proj.m14;
    clip.m15 = modl.m12 * proj.m3 + modl.m13 * proj.m7 + modl.m14 * proj.m11 + modl.m15 * proj.m15;

    // Now we actually want to get the sides of the frustum.  To do this we take
    // the clipping planes we received above and extract the sides from them.

    // This will extract the RIGHT side of the frustum
    m_Frustum[RIGHT][A] = clip.m3 - clip.m0;
    m_Frustum[RIGHT][B] = clip.m7 - clip.m4;
    m_Frustum[RIGHT][C] = clip.m11 - clip.m8;
    m_Frustum[RIGHT][D] = clip.m15 - clip.m12;

    // Now that we have a normal (A,B,C) and a distance (D) to the plane,
    // we want to normalize that normal and distance.

    // Normalize the RIGHT side
    normalizePlane(RIGHT);

    // This will extract the LEFT side of the frustum
    m_Frustum[LEFT][A] = clip.m3 + clip.m0;
    m_Frustum[LEFT][B] = clip.m7 + clip.m4;
    m_Frustum[LEFT][C] = clip.m11 + clip.m8;
    m_Frustum[LEFT][D] = clip.m15 + clip.m12;

    // Normalize the LEFT side
    normalizePlane(LEFT);

    // This will extract the BOTTOM side of the frustum
    m_Frustum[BOTTOM][A] = clip.m3 + clip.m1;
    m_Frustum[BOTTOM][B] = clip.m7 + clip.m5;
    m_Frustum[BOTTOM][C] = clip.m11 + clip.m9;
    m_Frustum[BOTTOM][D] = clip.m15 + clip.m13;

    // Normalize the BOTTOM side
    normalizePlane(BOTTOM);

    // This will extract the TOP side of the frustum
    m_Frustum[TOP][A] = clip.m3 - clip.m1;
    m_Frustum[TOP][B] = clip.m7 - clip.m5;
    m_Frustum[TOP][C] = clip.m11 - clip.m9;
    m_Frustum[TOP][D] = clip.m15 - clip.m13;

    // Normalize the TOP side
    normalizePlane(TOP);

    // This will extract the BACK side of the frustum
    m_Frustum[BACK][A] = clip.m3 - clip.m2;
    m_Frustum[BACK][B] = clip.m7 - clip.m6;
    m_Frustum[BACK][C] = clip.m11 - clip.m10;
    m_Frustum[BACK][D] = clip.m15 - clip.m14;

    // Normalize the BACK side
    normalizePlane(BACK);

    // This will extract the FRONT side of the frustum
    m_Frustum[FRONT][A] = clip.m3 + clip.m2;
    m_Frustum[FRONT][B] = clip.m7 + clip.m6;
    m_Frustum[FRONT][C] = clip.m11 + clip.m10;
    m_Frustum[FRONT][D] = clip.m15 + clip.m14;

    // Normalize the FRONT side
    normalizePlane(FRONT);
}

// The code below will allow us to make checks within the frustum.  For example,
// if we want to see if a point, a sphere, or a cube lies inside of the frustum.
// Because all of our planes point INWARDS (The normals are all pointing inside the frustum)
// we then can assume that if a point is in FRONT of all of the planes, it's inside.

///////////////////////////////// POINT IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This determines if a point is inside of the frustum
/////
///////////////////////////////// POINT IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Frustum::pointInFrustum(float x, float y, float z) {
    // Go through all the sides of the frustum
    for (int i = 0; i < 6; i++) {
        // Calculate the plane equation and check if the point is behind a side of the frustum
        if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= 0) {
            // The point was behind a side, so it ISN'T in the frustum
            return false;
        }
    }

    // The point was inside of the frustum (In front of ALL the sides of the frustum)
    return true;
}


///////////////////////////////// SPHERE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This determines if a sphere is inside of our frustum by it's center and radius.
/////
///////////////////////////////// SPHERE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Frustum::sphereInFrustum(float x, float y, float z, float radius) {
    // Go through all the sides of the frustum
    for (int i = 0; i < 6; i++) {
        // If the center of the sphere is farther away from the plane than the radius
        if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= -radius) {
            // The distance was greater than the radius so the sphere is outside of the frustum
            return false;
        }
    }

    // The sphere was inside of the frustum!
    return true;
}

///////////////////////////////// CUBE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This determines if a cube is in or around our frustum by it's center and 1/2 it's length
/////
///////////////////////////////// CUBE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Frustum::cubeInFrustum(Vector3 min, Vector3 max) {
    // This test is a bit more work, but not too much more complicated.
    // Basically, what is going on is, that we are given the center of the cube,
    // and half the length.  Think of it like a radius.  Then we checking each point
    // in the cube and seeing if it is inside the frustum.  If a point is found in front
    // of a side, then we skip to the next side.  If we get to a plane that does NOT have
    // a point in front of it, then it will return false.

    // *Note* - This will sometimes say that a cube is inside the frustum when it isn't.
    // This happens when all the corners of the bounding box are not behind any one plane.
    // This is rare and shouldn't effect the overall rendering speed.

    for (int i = 0; i < 6; i++) {
        if (m_Frustum[i][A] * min.x + m_Frustum[i][B] * min.y + m_Frustum[i][C] * min.z + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * max.x + m_Frustum[i][B] * min.y + m_Frustum[i][C] * min.z + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * min.x + m_Frustum[i][B] * max.y + m_Frustum[i][C] * min.z + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * max.x + m_Frustum[i][B] * max.y + m_Frustum[i][C] * min.z + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * min.x + m_Frustum[i][B] * min.y + m_Frustum[i][C] * max.z + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * max.x + m_Frustum[i][B] * min.y + m_Frustum[i][C] * max.z + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * min.x + m_Frustum[i][B] * max.y + m_Frustum[i][C] * max.z + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * max.x + m_Frustum[i][B] * max.y + m_Frustum[i][C] * max.z + m_Frustum[i][D] > 0)
            continue;

        // If we get here, it isn't in the frustum
        return false;
    }

    return true;
}

bool Frustum::cubeInFrustum(const AABB& aabb) {
    return cubeInFrustum(aabb.min(), aabb.max());
}