#include "GridPathfinder.h"

float GridPathfinder::moveCost(int x0, int y0, int x1, int y1)
{
    Vector3 a = terr->cellToWorld(x0, y0), b = terr->cellToWorld(x1, y1);
    float hdiff = fabsf(b.y - a.y);
    float base = Vector3Distance(a, b);
    return base + hdiff * 4.0f + (blocked[idx(x1, y1)] ? 1e4f : 0.0f);
}

void GridPathfinder::addPropBlockers()
{
    if (!terr || !props)
        return;
    for (const auto &pr : *props)
    {
        Vector2 c = terr->worldToCell(pr.pos.x, pr.pos.z);
        int cx = (int)c.x, cy = (int)c.y;
        int rad = (int)std::ceil(pr.radius / terr->cellSize);
        for (int dy = -rad; dy <= rad; ++dy)
            for (int dx = -rad; dx <= rad; ++dx)
            {
                int x = cx + dx, y = cy + dy;
                if (in(x, y))
                    blocked[idx(x, y)] = 1;
            }
    }
}
std::vector<Vector3> GridPathfinder::path(Vector3 start, Vector3 goal)
{
    Vector2 cs = terr->worldToCell(start.x, start.z);
    Vector2 cg = terr->worldToCell(goal.x, goal.z);
    int sx = (int)std::clamp((int)cs.x, 0, W - 1), sy = (int)std::clamp((int)cs.y, 0, H - 1);
    int gx = (int)std::clamp((int)cg.x, 0, W - 1), gy = (int)std::clamp((int)cg.y, 0, H - 1);

    std::fill(prev.begin(), prev.end(), -1);
    std::fill(dist.begin(), dist.end(), 1e9f);
    using P = std::pair<float, int>; // (dist, idx)
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    int s = idx(sx, sy), g = idx(gx, gy);
    dist[s] = 0;
    pq.push({0.0f, s});
    const int NX[8] = {1, -1, 0, 0, 1, 1, -1, -1};
    const int NY[8] = {0, 0, 1, -1, 1, -1, 1, -1};

    while (!pq.empty())
    {
        auto [d, u] = pq.top();
        pq.pop();
        if (d != dist[u])
            continue;
        if (u == g)
            break;
        int ux = u % W, uy = u / W;
        for (int k = 0; k < 8; k++)
        {
            int vx = ux + NX[k], vy = uy + NY[k];
            if (!in(vx, vy))
                continue;
            float w = moveCost(ux, uy, vx, vy);
            float nd = d + w;
            int v = idx(vx, vy);
            if (nd < dist[v])
            {
                dist[v] = nd;
                prev[v] = u;
                pq.push({nd, v});
            }
        }
    }
    std::vector<Vector3> out;
    if (prev[g] == -1)
        return out;
    for (int u = g; u != -1; u = prev[u])
    {
        int ux = u % W, uy = u / W;
        out.push_back(terr->cellToWorld(ux, uy));
    }
    std::reverse(out.begin(), out.end());
    return out;
}
void GridPathfinder::reset(Terrain *t, const std::vector<Prop> *p)
{
    terr = t;
    props = p;
    if (!t)
        return;
    W = t->gridW;
    H = t->gridH;
    blocked = t->blocked;
    prev.assign(W * H, -1);
    dist.assign(W * H, 1e9f);
    addPropBlockers();
}