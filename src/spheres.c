#include "spheres.h"
#include <stdlib.h>
#include <math.h>

// --- WORLD MANAGEMENT ---

void worldInit(World *world) {
    // Init Spheres
    world->sphere_size = 0;
    world->sphere_capacity = 1;
    world->spheres = malloc(world->sphere_capacity * sizeof(Sphere*));

    // Init Cubes
    world->cube_size = 0;
    world->cube_capacity = 1;
    world->cubes = malloc(world->cube_capacity * sizeof(Cube*));
}

void freeWorld(World *world) {
    // Free Spheres
    for (int i = 0; i < world->sphere_size; i++) {
        free(world->spheres[i]);
    }
    free(world->spheres);

    // Free Cubes
    for (int i = 0; i < world->cube_size; i++) {
        free(world->cubes[i]);
    }
    free(world->cubes);
}

// --- SPHERE FUNCTIONS ---

void addSphere(World *world, Sphere *sphere) {
    if (world->sphere_size >= world->sphere_capacity) {
        world->sphere_capacity *= 2;
        world->spheres = realloc(world->spheres, world->sphere_capacity * sizeof(Sphere*));
    }
    world->spheres[world->sphere_size++] = sphere;
}

Sphere *createSphere(float radius, Vec3 position, Vec3 color) {
    Sphere *s = malloc(sizeof(Sphere));
    s->r = radius;
    s->pos = position;
    s->color = color;
    return s;
}

int doesIntersect(const Sphere *sphere, Vec3 rayPos, Vec3 rayDir, float *t) {
    Vec3 V = subtract(rayPos, sphere->pos);
    float a = dot(rayDir, rayDir);
    float b = 2.0f * dot(rayDir, V);
    float c = dot(V, V) - (sphere->r * sphere->r);
    float discriminant = b*b - 4*a*c;

    if (discriminant < 0) return 0;

    float sqrtD = sqrtf(discriminant);
    float t1 = (-b - sqrtD)/(2*a);
    float t2 = (-b + sqrtD)/(2*a);

    float t_candidate = INFINITY;
    if (t1 > 0.001f) t_candidate = t1;
    if (t2 > 0.001f && t2 < t_candidate) t_candidate = t2;

    if (t_candidate != INFINITY) {
        *t = t_candidate;
        return 1;
    }
    return 0;
}

// --- CUBE FUNCTIONS (NEW) ---

void addCube(World *world, Cube *cube) {
    if (world->cube_size >= world->cube_capacity) {
        world->cube_capacity *= 2;
        world->cubes = realloc(world->cubes, world->cube_capacity * sizeof(Cube*));
    }
    world->cubes[world->cube_size++] = cube;
}

Cube *createCube(Vec3 min, Vec3 max, Vec3 color) {
    Cube *c = malloc(sizeof(Cube));
    c->min = min;
    c->max = max;
    c->color = color;
    return c;
}

// The "Slab Method" for Ray-Box Intersection
int doesCubeIntersect(const Cube *cube, Vec3 rayPos, Vec3 rayDir, float *t) {
    float tmin = -INFINITY;
    float tmax = INFINITY;

    // Check X Slab
    if (rayDir.x != 0.0f) {
        float tx1 = (cube->min.x - rayPos.x) / rayDir.x;
        float tx2 = (cube->max.x - rayPos.x) / rayDir.x;
        tmin = fmaxf(tmin, fminf(tx1, tx2));
        tmax = fminf(tmax, fmaxf(tx1, tx2));
    } else if (rayPos.x < cube->min.x || rayPos.x > cube->max.x) {
        return 0; // Parallel and outside
    }

    // Check Y Slab
    if (rayDir.y != 0.0f) {
        float ty1 = (cube->min.y - rayPos.y) / rayDir.y;
        float ty2 = (cube->max.y - rayPos.y) / rayDir.y;
        tmin = fmaxf(tmin, fminf(ty1, ty2));
        tmax = fminf(tmax, fmaxf(ty1, ty2));
    } else if (rayPos.y < cube->min.y || rayPos.y > cube->max.y) {
        return 0;
    }

    // Check Z Slab
    if (rayDir.z != 0.0f) {
        float tz1 = (cube->min.z - rayPos.z) / rayDir.z;
        float tz2 = (cube->max.z - rayPos.z) / rayDir.z;
        tmin = fmaxf(tmin, fminf(tz1, tz2));
        tmax = fminf(tmax, fmaxf(tz1, tz2));
    } else if (rayPos.z < cube->min.z || rayPos.z > cube->max.z) {
        return 0;
    }

    // Did we miss the box?
    if (tmax < tmin || tmax < 0) {
        return 0;
    }

    // We hit it!
    if (tmin > 0.001f) *t = tmin;
    else *t = tmax;
    
    return 1;
}