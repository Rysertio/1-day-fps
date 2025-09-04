#pragma once
#include "world.h"
#include <queue>

class GridPathfinder
{
    int W, H;
    Terrain *terr;
    const std::vector<Prop> *props;
    std::vector<uint8_t> blocked; // copy base + props
    std::vector<int> prev;
    std::vector<float> dist; // reusable buffers


    inline int idx(int x, int y) const { return y * W + x; }
    bool in(int x, int y) const { return x >= 0 && y >= 0 && x < W && y < H; }

    void addPropBlockers();

    float moveCost(int x0, int y0, int x1, int y1);

    // Dijkstra from start to goal, returns list of world positions
    public:
    void reset(Terrain *t, const std::vector<Prop> *p);
    std::vector<Vector3> path(Vector3 start, Vector3 goal);
        GridPathfinder(Terrain *t = nullptr, const std::vector<Prop> *p = nullptr) { reset(t, p); }
};
