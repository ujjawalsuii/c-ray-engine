#ifndef SPHERES_H
#define SPHERES_H

#include "vector.h"

// --- SPHERE DEFINITION ---
typedef struct {
    float r;
    Vec3 pos;
    Vec3 color;
} Sphere;

// --- CUBE DEFINITION (New!) ---
typedef struct {
    Vec3 min;   // Bottom-Left-Back corner (Smallest x,y,z)
    Vec3 max;   // Top-Right-Front corner (Largest x,y,z)
    Vec3 color;
} Cube;

// --- WORLD CONTAINER ---
typedef struct {
    // List of Spheres
    Sphere **spheres;
    int sphere_size;
    int sphere_capacity;

    // List of Cubes (New!)
    Cube **cubes;
    int cube_size;
    int cube_capacity;
} World;

// --- FUNCTIONS ---
void worldInit(World *world);
void freeWorld(World *world);

// Sphere Functions
void addSphere(World *world, Sphere *sphere);
Sphere *createSphere(float radius, Vec3 position, Vec3 color);
int doesIntersect(const Sphere *sphere, Vec3 rayPos, Vec3 rayDir, float *t);

// Cube Functions (New!)
void addCube(World *world, Cube *cube);
Cube *createCube(Vec3 min, Vec3 max, Vec3 color);
int doesCubeIntersect(const Cube *cube, Vec3 rayPos, Vec3 rayDir, float *t);

#endif // SPHERES_H