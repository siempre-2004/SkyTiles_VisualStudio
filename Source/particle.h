#ifndef PARTICLE_H 
#define PARTICLE_H

#include <vector>
#include <set>
#include <iostream>
#include "raylib.h"

#define MAX_PARTICLES 100

struct Particle {
    Vector2 position;
    Vector2 velocity;
    float life;
    Color color;
    bool active;
};

extern std::vector<Vector2> particle_spawn_points;
extern Particle particle_pool[MAX_PARTICLES];

void update_particles(float deltaTime);
void render_particles(Camera2D& camera);
void spawn_particle(Vector2 velocity, float life);

#endif   