
static void drawUI(const Game &G)
{
    DrawRectangle(10, 10, 260, 60, Fade(BLACK, 0.4f));
    DrawText(TextFormat("HP: %.0f", G.P.health), 20, 20, 18, RAYWHITE);
    DrawText(TextFormat("Enemies: %d", (int)std::count_if(G.enemies.begin(), G.enemies.end(), [](auto &e)
                                                          { return e.alive; })),
             20, 40, 18, RAYWHITE);
    DrawText(G.P.inVehicle ? "[E] Exit vehicle" : "[E] Enter vehicle (near car)", 20, 80, 18, RAYWHITE);
}

static void drawWorld(Game &G, Camera3D &cam)
{
    BeginMode3D(cam);
    if (G.wire)
    {
        rlEnableWireMode();
    }
    DrawModel(G.T.model, {0, 0, 0}, 1.0f, WHITE);
    if (G.wire)
        rlDisableWireMode();

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

