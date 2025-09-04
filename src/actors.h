#include <vector>
#include "raymath.h"

struct Projectile
{
    Vector3 pos, vel;
    float ttl;
};

struct Enemy
{
    Vector3 pos;
    float speed = 4.0f;
    float radius = 0.6f;
    std::vector<Vector3> way;
    int wp = 0;
    float repathTimer = 0.0f;
    bool alive = true;
    float hp = 100;
};

struct Vehicle
{
    Vector3 pos;
    Vector3 vel;
    bool occupied = false;
    float yaw = 0;
};

struct Player
{
    Vector3 pos{0, 0, 0};
    Vector3 vel{0, 0, 0};
    float yaw = 0, pitch = 0;
    bool onGround = false;
    bool inVehicle = false;
    float health = 100;
};
