#pragma once
#include<cstdint>
#include<random>
#include <algorithm>
#include"raymath.h"

static uint32_t seed32 = 1337u;
static std::mt19937 rng(seed32);


class Perlin
{
static float Fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
    int p[512];
    static float grad(int h, float x, float y, float z)
    {
        int g = h & 15;
        float u = g < 8 ? x : y;
        float v = g < 4 ? y : (g == 12 || g == 14 ? x : z);
        return ((g & 1) ? -u : u) + ((g & 2) ? -v : v);
    }
    public:
        Perlin(unsigned s);
        float noise3(float x, float y, float z);
};
