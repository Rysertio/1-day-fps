#pragma once
#include <cstdint>
#include <random>
#include <algorithm>
#include <cstring>
#include "raylib.h"
#include "raymath.h"
#include "noise.h"

class Terrain
{
    // world units per cell
    float steepLimit = 0.9f;    // max slope to be walkable (cosine of angle between normals)
    std::vector<float> heights; // size gridW*gridH
    Mesh mesh{};

public:
    float maxHeight = 15.0f;    // vertical scale
    Model model{};                // renderable
    std::vector<uint8_t> blocked; // 0 walkable, 1 blocked
    Vector3 cellToWorld(int x, int y) const
    {
        float wx = (x - gridW / 2) * cellSize;
        float wz = (y - gridH / 2) * cellSize;
        float wy = heights[y * gridW + x];
        return {wx, wy, wz};
    }
    Vector2 worldToCell(float wx, float wz) const
    {
        int x = (int)roundf(wx / cellSize + gridW / 2.0f);
        int y = (int)roundf(wz / cellSize + gridH / 2.0f);
        return {(float)x, (float)y};
    }



    float terrainHeightAt(float x, float z);
    int gridW = 128, gridH = 128; // navigability grid resolution
    float cellSize = 2.0f;
    float heightSample(Perlin &perlin, float x, float y);
    void BuildTerrain(unsigned seed);
};
