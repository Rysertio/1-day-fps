#include "noise.h"

Perlin::Perlin(unsigned s = 1337u)
{
    std::vector<int> perm(256);
    std::iota(perm.begin(), perm.end(), 0);
    std::mt19937 r(s);
    std::shuffle(perm.begin(), perm.end(), r);
    for (int i = 0; i < 512; i++)
        p[i] = perm[i & 255];
}

float Perlin::noise3(float x, float y, float z)
{
    int X = (int)floorf(x) & 255;
    int Y = (int)floorf(y) & 255;
    int Z = (int)floorf(z) & 255;
    x -= floorf(x);
    y -= floorf(y);
    z -= floorf(z);
    float u = Fade(x), v = Fade(y), w = Fade(z);
    int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
    int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;
    float res = Lerp(
        Lerp(
            Lerp(grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z), u),
            Lerp(grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z), u), v),
        Lerp(
            Lerp(grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1), u),
            Lerp(grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1), u), v),
        w);
    return (res + 1.0f) * 0.5f; // [0,1]
}
