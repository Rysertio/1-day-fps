#pragma once
#include "world.h"
#include "GridPathfinder.h"
#include "actors.h"
#include "util.h"

class Game
{
    std::vector<Zone> zones;
    std::vector<Prop> props;
    GridPathfinder nav;
    Player P;
    std::vector<Enemy> enemies;
    std::vector<Projectile> projs;
    Vehicle car;



friend void handlePlayer(Game &G, float dt);
friend void handleCamera(Game &G, Camera3D &cam, float dt);

friend void handleWeapons(Game &G);

friend void updateProjectiles(Game &G, float dt);

friend void updateEnemies(Game &G, float dt);

friend void drawUI(const Game &G);

friend void drawWorld(Game &G, Camera3D &cam);

public:
    void regenerate(unsigned s);
    void spawnActors(unsigned s);
    bool wire = false;
        Terrain T;

};
 void handlePlayer(Game &G, float dt);
 void handleCamera(Game &G, Camera3D &cam, float dt);

 void handleWeapons(Game &G);

 void updateProjectiles(Game &G, float dt);

 void updateEnemies(Game &G, float dt);

 void drawUI(const Game &G);

 void drawWorld(Game &G, Camera3D &cam);