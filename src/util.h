#include"raymath.h"

static bool sphereRayHit(Vector3 ro, Vector3 rd, Vector3 c, float r, float &t)
{
    Vector3 oc = Vector3Subtract(ro, c);
    float b = Vector3DotProduct(oc, rd);
    float c2 = Vector3DotProduct(oc, oc) - r * r;
    float h = b * b - c2;
    if (h < 0)
        return false;
    h = sqrtf(h);
    t = -b - h;
    if (t < 0)
        t = -b + h;
    return t >= 0;
}

static Vector3 getCameraDir(float yaw, float pitch)
{
    float cy = cosf(yaw), sy = sinf(yaw), cp = cosf(pitch), sp = sinf(pitch);
    return {sy * cp, -sp, cy * cp};
}