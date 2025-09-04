// raylib FPS demo: procedural noise terrain, zoned spawns, Dijkstra enemy pathfinding,
// simple weapon system (hitscan + projectile), and a basic vehicle.
//
// Build (Linux):
//   g++ main.cpp -o game -std=c++17 -O2 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
// Build (Windows, MSYS2/MinGW):
//   g++ main.cpp -o game.exe -std=c++17 -O2 -lraylib -lopengl32 -lgdi32 -lwinmm
//
// Controls:
//   WASD: Move on foot
//   Mouse: Look
//   Space / Left Shift: Jump / Sprint
//   Left Click: Fire primary (hitscan)
//   Right Click: Fire secondary (projectile)
//   E: Enter/Exit vehicle (when near)
//   F: Toggle wireframe
//   R: Reload procedural world
//   ESC: Quit
//
// Notes:
// - Terrain generated via Perlin noise; a navigability grid (walkable) is built for Dijkstra.
// - Enemies pick a shortest path to the player periodically. Obstacles = steep cells + objects.
// - Vehicle is a simple car-like pawn with faster speed and different camera offset.
// - This is an educational, compact example; tweak map size if you need more performance.

#include "raylib.h"
#include "rlgl.h"
#include <vector>

#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <random>
#include <algorithm>
#include <string>
#include "raymath.h"
#include "noise.h"
#include "terrain.h"
#include "game.h"
int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "raylib FPS: Procedural Terrain + Dijkstra AI + Vehicle");
    DisableCursor();

    Game G;
    G.T.gridW = 128;
    G.T.gridH = 128;
    G.T.cellSize = 2.0f;
    G.T.maxHeight = 18.0f;
    G.regenerate(seed32);

    Camera3D cam{};
    cam.up = {0, 1, 0};
    cam.fovy = 75;
    cam.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(90);
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (IsKeyPressed(KEY_F))
            G.wire = !G.wire;
        if (IsKeyPressed(KEY_R))
        {
            seed32 = (uint32_t)GetTime() * 1000u + GetRandomValue(0, 1000000);
            G.regenerate(seed32);
        }

        handlePlayer(G, dt);
        handleCamera(G, cam, dt);
        handleWeapons(G);
        updateProjectiles(G, dt);
        updateEnemies(G, dt);

        BeginDrawing();
        ClearBackground(Color{30, 40, 50, 255});
        drawWorld(G, cam);
        drawUI(G);
        DrawFPS(GetScreenWidth() - 100, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}