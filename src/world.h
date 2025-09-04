#pragma once
#include "terrain.h"

struct Prop
{
    Vector3 pos;
    float radius;
    int kind;
};
struct Zone
{
    Rectangle rect;
    int type;
}; // 0=none,1=forest,2=rockfield,3=camp

static std::vector<Zone> BuildZones(const Terrain &t)
{
    // Divide the map into 3x3 zones with different types
    std::vector<Zone> z;
    float W = t.gridW * t.cellSize, H = t.gridH * t.cellSize;
    int idx = 0;
    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < 3; i++)
        {
            Rectangle r{-W / 2 + i * (W / 3.0f), -H / 2 + j * (H / 3.0f), W / 3.0f, H / 3.0f};
            int type = (idx++ % 3) + 1; // rotate 1..3
            z.push_back({r, type});
        }
    }
    return z;
}

static std::vector<Prop> SpawnProps(Terrain &t, const std::vector<Zone> &zones, int count, unsigned seed)
{
    std::vector<Prop> props;
    props.reserve(count);
    std::mt19937 rr(seed);
    std::uniform_real_distribution<float> ux(-t.gridW * t.cellSize * 0.5f, t.gridW * t.cellSize * 0.5f);
    std::uniform_real_distribution<float> uz(-t.gridH * t.cellSize * 0.5f, t.gridH * t.cellSize * 0.5f);
    for (int i = 0; i < count; i++)
    {
        Vector3 p{ux(rr), 0, uz(rr)};
        // Find zone type at this point
        int zt = 0;
        for (auto &z : zones)
        {
            if (CheckCollisionPointRec({p.x, p.z}, z.rect))
            {
                zt = z.type;
                break;
            }
        }
        float r = (zt == 1 ? 1.5f : (zt == 2 ? 2.0f : 1.2f));
        p.y = t.terrainHeightAt(p.x, p.z);
        props.push_back({p, r, zt});
    }
    return props;
}

static void DrawProp(const Prop &pr)
{
    Color c = pr.kind == 1 ? Color{60, 120, 60, 255} : (pr.kind == 2 ? Color{110, 110, 110, 255} : Color{180, 160, 120, 255});
    if (pr.kind == 1)
        DrawCylinder(pr.pos, 0.6f, 1.2f, 5.0f, 8, DARKGREEN), DrawSphere(Vector3{pr.pos.x, pr.pos.y + 4.0f, pr.pos.z}, 2.5f, c);
    else if (pr.kind == 2)
        DrawCubeV(Vector3{pr.pos.x, pr.pos.y + pr.radius * 0.5f, pr.pos.z}, Vector3{pr.radius, pr.radius, pr.radius}, c);
    else
        DrawCubeV(Vector3{pr.pos.x, pr.pos.y + 0.5f, pr.pos.z}, Vector3{1.0f, 1.0f, 1.0f}, c);
}
