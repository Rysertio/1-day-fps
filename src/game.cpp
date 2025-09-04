#include "game.h"

void Game::regenerate(unsigned s)
{
    rng.seed(s);
    T.BuildTerrain(s);
    zones = BuildZones(T);
    props = SpawnProps(T, zones, 220, s + 42);
    nav.reset(&T, &props);
    spawnActors(s + 99);
}

void Game::spawnActors(unsigned s)
{
    enemies.clear();
    std::mt19937 rr(s);
    std::uniform_int_distribution<int> dx(8, T.gridW - 8), dy(8, T.gridH - 8);
    for (int i = 0; i < 20; i++)
    {
        int x = dx(rr), y = dy(rr);
        Vector3 p = T.cellToWorld(x, y);
        if (T.blocked[y * T.gridW + x])
        {
            i--;
            continue;
        }
        enemies.push_back({p});
    }
    // Player & car
    P.pos = T.cellToWorld(T.gridW / 2, T.gridH / 2);
    P.pos.y += 1.6f;
    car.pos = T.cellToWorld(T.gridW / 2 + 6, T.gridH / 2 + 2);
    car.pos.y += 1.0f;
    car.vel = {0, 0, 0};
    car.occupied = false;
    car.yaw = 0;
}
  void handlePlayer(Game &G, float dt)
{
    if (!G.P.inVehicle)
    {
        // FPS movement
        Vector3 forward = getCameraDir(G.P.yaw, 0);
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, {0, 1, 0}));
        Vector3 wish = {0, 0, 0};
        if (IsKeyDown(KEY_W))
            wish = Vector3Add(wish, forward);
        if (IsKeyDown(KEY_S))
            wish = Vector3Subtract(wish, forward);
        if (IsKeyDown(KEY_A))
            wish = Vector3Subtract(wish, right);
        if (IsKeyDown(KEY_D))
            wish = Vector3Add(wish, right);
        if (Vector3Length(wish) > 0)
            wish = Vector3Normalize(wish);
        float speed = (IsKeyDown(KEY_LEFT_SHIFT) ? 10.0f : 6.0f);
        G.P.vel.x = wish.x * speed;
        G.P.vel.z = wish.z * speed;
        // gravity
        G.P.vel.y -= 30.0f * dt;
        // jump
        if (G.P.onGround && IsKeyPressed(KEY_SPACE))
        {
            G.P.vel.y = 9.0f;
            G.P.onGround = false;
        }
        // integrate
        G.P.pos = Vector3Add(G.P.pos, Vector3Scale(G.P.vel, dt));
        // clamp to terrain
        float ty = G.T.terrainHeightAt(G.P.pos.x, G.P.pos.z) + 1.6f;
        if (G.P.pos.y <= ty)
        {
            G.P.pos.y = ty;
            G.P.vel.y = 0;
            G.P.onGround = true;
        }

        // Enter vehicle
        if (IsKeyPressed(KEY_E))
        {
            if (Vector3Distance(G.P.pos, G.car.pos) < 3.0f)
            {
                G.P.inVehicle = true;
                G.car.occupied = true;
            }
        }
    }
    else
    {
        // Vehicle controls
        float accel = 0.0f;
        if (IsKeyDown(KEY_W))
            accel += 30.0f;
        if (IsKeyDown(KEY_S))
            accel -= 20.0f;
        if (IsKeyDown(KEY_A))
            G.car.yaw -= 1.4f * dt;
        if (IsKeyDown(KEY_D))
            G.car.yaw += 1.4f * dt;
        Vector3 fwd = {sinf(G.car.yaw), 0, cosf(G.car.yaw)};
        G.car.vel = Vector3Add(G.car.vel, Vector3Scale(fwd, accel * dt));
        // friction
        G.car.vel = Vector3Scale(G.car.vel, 1.0f - std::min(1.0f, 1.5f * dt));
        G.car.pos = Vector3Add(G.car.pos, G.car.vel);
        // stick to ground
        G.car.pos.y = G.T.terrainHeightAt(G.car.pos.x, G.car.pos.z) + 0.8f;
        // Exit
        if (IsKeyPressed(KEY_E))
        {
            G.P.inVehicle = false;
            G.car.occupied = false;
            G.P.pos = Vector3Add(G.car.pos, Vector3{2.0f, 1.0f, 0});
        }
    }
}

  void handleCamera(Game &G, Camera3D &cam, float dt)
{
    Vector2 md = GetMouseDelta();
    G.P.yaw += md.x * 0.0025f;
    G.P.pitch += md.y * 0.0020f;
    G.P.pitch = std::clamp(G.P.pitch, -1.2f, 1.2f);

    Vector3 dir = getCameraDir(G.P.yaw, G.P.pitch);
    if (!G.P.inVehicle)
    {
        cam.position = G.P.pos;
        cam.target = Vector3Add(cam.position, dir);
    }
    else
    {
        // third-person over-hood cam
        Vector3 fwd = {sinf(G.car.yaw), 0, cosf(G.car.yaw)};
        Vector3 camPos = Vector3Add(G.car.pos, Vector3Scale(fwd, -6.0f));
        camPos.y += 3.0f;
        cam.position = camPos;
        cam.target = Vector3Add(G.car.pos, Vector3{0, 1.0f, 0});
    }
}

  void handleWeapons(Game &G)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        // hitscan
        Vector3 ro, rd;
        if (!G.P.inVehicle)
        {
            ro = G.P.pos;
            rd = getCameraDir(G.P.yaw, G.P.pitch);
        }
        else
        {
            ro = Vector3Add(G.car.pos, Vector3{0, 1.0f, 0});
            rd = {sinf(G.car.yaw), 0, cosf(G.car.yaw)};
        }
        Enemy *best = nullptr;
        float bestT = 1e9f;
        for (auto &e : G.enemies)
        {
            if (!e.alive)
                continue;
            float t;
            if (sphereRayHit(ro, rd, e.pos, e.radius * 1.2f, t))
            {
                if (t < bestT)
                {
                    bestT = t;
                    best = &e;
                }
            }
        }
        if (best)
        {
            best->hp -= 50.0f;
            if (best->hp <= 0)
            {
                best->alive = false;
            }
        }
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        Vector3 ro, rd;
        if (!G.P.inVehicle)
        {
            ro = Vector3Add(G.P.pos, Vector3{0, 0.6f, 0});
            rd = getCameraDir(G.P.yaw, G.P.pitch);
        }
        else
        {
            ro = Vector3Add(G.car.pos, Vector3{0, 1.0f, 0});
            rd = {sinf(G.car.yaw), 0, cosf(G.car.yaw)};
        }
        G.projs.push_back({ro, Vector3Scale(rd, 26.0f), 4.0f});
    }
}

  void updateProjectiles(Game &G, float dt)
{
    for (auto &p : G.projs)
    {
        p.vel.y -= 9.0f * dt;
        p.pos = Vector3Add(p.pos, Vector3Scale(p.vel, dt));
        p.ttl -= dt;
    }
    // collide with ground or enemies
    for (auto &p : G.projs)
    {
        if (p.ttl <= 0)
            continue;
        float ground = G.T.terrainHeightAt(p.pos.x, p.pos.z) + 0.1f;
        if (p.pos.y <= ground)
        {
            p.ttl = 0;
        }
        for (auto &e : G.enemies)
        {
            if (!e.alive)
                continue;
            if (Vector3Distance(p.pos, e.pos) < e.radius + 0.6f)
            {
                e.hp -= 40.0f;
                if (e.hp <= 0)
                    e.alive = false;
                p.ttl = 0;
                break;
            }
        }
    }
    G.projs.erase(std::remove_if(G.projs.begin(), G.projs.end(), [](const Projectile &pr)
                                 { return pr.ttl <= 0; }),
                  G.projs.end());
}

  void updateEnemies(Game &G, float dt)
{
    for (auto &e : G.enemies)
    {
        if (!e.alive)
            continue;
        e.repathTimer -= dt;
        if (e.repathTimer <= 0)
        {
            e.repathTimer = 1.0f + (rand() % 1000) / 1000.0f;
            Vector3 target = G.P.inVehicle ? G.car.pos : G.P.pos;
            e.way = G.nav.path(e.pos, target);
            e.wp = (e.way.size() > 0 ? 1 : 0);
        }
        if (e.way.size() > 1 && e.wp < (int)e.way.size())
        {
            Vector3 dst = e.way[e.wp];
            Vector3 d = Vector3Subtract(dst, e.pos);
            d.y = 0;
            float L = Vector3Length(d);
            if (L < 0.4f)
                e.wp++;
            else
            {
                d = Vector3Scale(Vector3Normalize(d), e.speed * dt);
                e.pos = Vector3Add(e.pos, d);
                e.pos.y = G.T.terrainHeightAt(e.pos.x, e.pos.z) + 0.4f;
            }
        }
        else
        {
            // direct chase as fallback
            Vector3 tar = (G.P.inVehicle ? G.car.pos : G.P.pos);
            Vector3 d = Vector3Subtract(tar, e.pos);
            d.y = 0;
            float L = Vector3Length(d);
            if (L > 0.01f)
            {
                d = Vector3Scale(Vector3Normalize(d), e.speed * dt * 0.6f);
                e.pos = Vector3Add(e.pos, d);
                e.pos.y = G.T.terrainHeightAt(e.pos.x, e.pos.z) + 0.4f;
            }
        }
    }
}

  void drawUI(const Game &G)
{
    DrawRectangle(10, 10, 260, 60, Fade(BLACK, 0.4f));
    DrawText(TextFormat("HP: %.0f", G.P.health), 20, 20, 18, RAYWHITE);
    DrawText(TextFormat("Enemies: %d", (int)std::count_if(G.enemies.begin(), G.enemies.end(), [](auto &e)
                                                          { return e.alive; })),
             20, 40, 18, RAYWHITE);
    DrawText(G.P.inVehicle ? "[E] Exit vehicle" : "[E] Enter vehicle (near car)", 20, 80, 18, RAYWHITE);
}

  void drawWorld(Game &G, Camera3D &cam)
{
    BeginMode3D(cam);
    DrawModel(G.T.model, {0, 0, 0}, 1.0f, WHITE);


    // Draw props
    for (auto &p : G.props)
        DrawProp(p);

    // Draw vehicle
    DrawCubeV(Vector3{G.car.pos.x, G.car.pos.y + 0.5f, G.car.pos.z}, Vector3{2.0f, 1.0f, 3.0f}, SKYBLUE);
    // Enemies
    for (const auto &e : G.enemies)
    {
        if (!e.alive)
            continue;
        DrawSphere(e.pos, e.radius, RED);
    }
    // Projectiles
    for (const auto &p : G.projs)
    {
        DrawSphere(p.pos, 0.2f, YELLOW);
    }

    // Crosshair
    EndMode3D();
    DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 3, RAYWHITE);
}