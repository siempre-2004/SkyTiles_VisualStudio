#include "particle.h"

#define PARTICLE_POSITION_BASE 300
#define PARTICLE_POSITION_OFFSET 150

std::vector<Vector2> particle_spawn_points = {
    {100.0f, 200.0f},
    {300.0f, 400.0f},
    {500.0f, 200.0f},
    {500.0f, 600.0f},
    {700.0f, 800.0f},
    {700.0f, 400.0f},
    {900.0f, 100.0f},
    {900.0f, 400.0f},
    {1200.0f, 500.0f},
    {1200.0f, 300.0f }
};
Particle particle_pool[MAX_PARTICLES];

void spawn_particle(Vector2 velocity, float life) {
    //Select a random spawn point from spawn points
    int randomIndex = rand() % particle_spawn_points.size();
    Vector2 basePosition = particle_spawn_points[randomIndex];

    float offsetX = static_cast<float>(rand() % (2 * PARTICLE_POSITION_OFFSET) - PARTICLE_POSITION_OFFSET);
    float offsetY = static_cast<float>(rand() % (2 * PARTICLE_POSITION_OFFSET) - PARTICLE_POSITION_OFFSET);

    Vector2 randomPosition = { basePosition.x + offsetX, basePosition.y + offsetY };

    // Find an idle particle in the particle pool
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particle_pool[i].active) {
            particle_pool[i] = {
                randomPosition,  
                velocity,        
                life,            
                {
                    static_cast<unsigned char>(rand() % 256), 
                    static_cast<unsigned char>(rand() % 256), 
                    static_cast<unsigned char>(rand() % 256), 
                    50
                },
                true 
            };
            break; 
        }
    }
}

void update_particles(float deltaTime){
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = particle_pool[i];

        if (!p.active){
            continue;
        }
        p.position.x += p.velocity.x * deltaTime; // update particle position
        p.position.y += p.velocity.y * deltaTime;
        p.life -= deltaTime;  // declining lifetime 
        p.color.a = static_cast<unsigned char>(255 * (p.life / 100.0f));// Dynamically adjust alpha
        // remove the particles
        if (p.life <= 0 || p.position.x < 0 || p.position.y < 0 ||
            p.position.x > GetScreenWidth() || p.position.y > GetScreenHeight()) {
            p.active = false;
        }
    }
}

void render_particles(Camera2D& camera) 
{
    for (int i = 0; i < MAX_PARTICLES; i++){
        Particle& p = particle_pool[i];
        if (!p.active){
            continue;
        }
        Vector2 screen_position = { p.position.x + camera.target.x - camera.offset.x / camera.zoom,
            p.position.y + camera.target.y - camera.offset.y / camera.zoom
        };
        float particleSize = 10.0f * (p.life / 100.0f);
        DrawCircleV(screen_position, particleSize, p.color);
    }
}