#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "vector.h"
#include "spheres.h"
#include "color.h"

// --- SCENE GLOBALS ---
Vec3 cameraPos = {0,0,0}; 
int imgWidth, imgHeight;
float vportHeight, focalLength, vportWidth;
Vec3 lightpos;
float lightBrightness;
int bgcolor;
Vec3 backgroundColor; 
World world;

// --- CONFIGURATION ---
#define SAMPLES_PER_PIXEL 4 
#define REF_EPSILON 0.001f
#define MAX_RECURSION_DEPTH 4 

// --- RENDER MODE ---
bool isNight = true; 

// --- PROTOTYPES ---
static Vec3 trace_ray(Vec3 origin, Vec3 dir, int depth, World *w);
static Vec3 calculate_ray_direction(float px, float py, int w, int h);
static Vec3 get_cube_normal(Vec3 p, Cube *c);
static Vec3 get_procedural_color(Vec3 p, Cube *c, bool *is_emissive);
static bool is_in_shadow(Vec3 point, Vec3 light_dir, World *w);
static Vec3 apply_fog(Vec3 color, float dist);
static void renderPPM(World *w, char *ppmfile);

static float random_noise(int x, int y, int z) {
    int n = x + y * 57 + z * 131;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

// --- MAIN ---
int main(int argc, char *argv[]) {
    // 1. Check for Mode Argument (day/night)
    if (argc > 3) {
        if (strcmp(argv[3], "day") == 0) {
            isNight = false;
            printf("Mode: DAYTIME\n");
        } else {
            isNight = true;
            printf("Mode: NIGHT\n");
        }
    } else {
        printf("No mode specified. Defaulting to NIGHT.\n");
        isNight = true; 
    }

    if (argc < 3) return 1;
    FILE *fp = fopen(argv[1], "r");
    if (!fp) return 1;

    fscanf(fp, "%d %d", &imgWidth, &imgHeight);
    fscanf(fp, "%f", &vportHeight);
    fscanf(fp, "%f", &focalLength);
    fscanf(fp, "%f %f %f %f", &lightpos.x, &lightpos.y, &lightpos.z, &lightBrightness);

    fscanf(fp, "%d", &noColours);
    unsigned int *colorsHex = malloc(noColours * sizeof(unsigned int));
    for (int i = 0; i < noColours; i++) fscanf(fp, "%x", &colorsHex[i]);
    fscanf(fp, "%d", &bgcolor);

    qsort(colorsHex, noColours, sizeof(unsigned int), compareColor);
    colorArray = malloc(noColours * sizeof(Vec3));
    for (int i = 0; i < noColours; i++) colorArray[i] = hexToRGB(colorsHex[i]);
    backgroundColor = colorArray[bgcolor];

    worldInit(&world);
    int n, cIdx;
    float x, y, z, r, x2, y2, z2;
    
    fscanf(fp, "%d", &n);
    for (int i = 0; i < n; i++) {
        fscanf(fp, "%f %f %f %f %d", &x, &y, &z, &r, &cIdx);
        addSphere(&world, createSphere(r, (Vec3){x,y,z}, colorArray[cIdx]));
    }
    
    int nCubes = 0;
    if (fscanf(fp, "%d", &nCubes) != 1) nCubes = 0;
    for (int i = 0; i < nCubes; i++) {
        fscanf(fp, "%f %f %f %f %f %f %d", &x, &y, &z, &x2, &y2, &z2, &cIdx);
        addCube(&world, createCube((Vec3){x,y,z}, (Vec3){x2,y2,z2}, colorArray[cIdx]));
    }
    fclose(fp);

    vportWidth = vportHeight * ((float)imgWidth / (float)imgHeight);
    renderPPM(&world, argv[2]);
    free(colorsHex); free(colorArray); freeWorld(&world);
    return 0;
}

// --- CORE RAY TRACING ---

static Vec3 lerp(Vec3 a, Vec3 b, float t) {
    return add(scalarMultiply(1.0f - t, a), scalarMultiply(t, b));
}

static Vec3 get_sky_color(Vec3 dir) {
    float t = dir.y; 
    
    
    if (t < 0.0f) t = 0.0f; 
    if (t > 1.0f) t = 1.0f;
    
    if (isNight) {
        Vec3 horizon = {0.0f, 0.15f, 0.25f}; 
        Vec3 zenith  = {0.0f, 0.02f, 0.05f}; 
        float factor = powf(t, 0.5f);
        Vec3 sky = lerp(horizon, zenith, factor);
        
        if (t > 0.2f) {
            int sx = (int)(dir.x*1000); int sy = (int)(dir.y*1000); int sz = (int)(dir.z*1000);
            if (random_noise(sx, sy, sz) > 0.995f) return (Vec3){1,1,1};
        }
        return sky;
    } else {
        Vec3 horizon = {0.6f, 0.8f, 1.0f}; 
        Vec3 zenith  = {0.1f, 0.4f, 0.9f}; 
        return lerp(horizon, zenith, t);
    }
}

static Vec3 trace_ray(Vec3 origin, Vec3 dir, int depth, World *w) {
    if (depth > MAX_RECURSION_DEPTH) return get_sky_color(dir);

    float closest_t = INFINITY;
    Sphere *hit_sphere = NULL;
    Cube *hit_cube = NULL;

    float t;
    for (int i=0; i<w->sphere_size; i++) {
        if(doesIntersect(w->spheres[i], origin, dir, &t) && t < closest_t && t > REF_EPSILON) { 
            closest_t = t; 
            hit_sphere = w->spheres[i]; 
            hit_cube = NULL; 
        }
    }
    
    for (int i=0; i<w->cube_size; i++) {
        
        if(doesCubeIntersect(w->cubes[i], origin, dir, &t) && t < closest_t && t > REF_EPSILON) { 
            closest_t = t; 
            hit_cube = w->cubes[i]; 
            hit_sphere = NULL; 
        }
    }

    if (closest_t == INFINITY) return get_sky_color(dir);

    Vec3 hitPoint = add(origin, scalarMultiply(closest_t, dir));
    Vec3 normal;
    Vec3 objColor;
    bool is_water = false;
    bool is_emissive = false;

    if (hit_sphere) {
        normal = normalize(subtract(hitPoint, hit_sphere->pos));
        objColor = hit_sphere->color;
        
        if (hit_sphere->r > 500.0f) {
            is_water = true;
            float ripple = sinf(hitPoint.x * 2.0f) + sinf(hitPoint.z * 1.5f);
            float strength = isNight ? 0.05f : 0.02f;
            normal.x += ripple * strength;
            normal.z += ripple * strength;
            normal = normalize(normal); 
        }
        else if (isNight && hit_sphere->pos.y > 10.0f) is_emissive = true;
        else if (!isNight && hit_sphere->pos.y > 10.0f) is_emissive = true;
    } else {
        normal = get_cube_normal(hitPoint, hit_cube);
        objColor = get_procedural_color(hitPoint, hit_cube, &is_emissive);
    }

    Vec3 finalColor;

    if (is_emissive) {
        float boost = isNight ? 3.5f : 10.0f;
        return apply_fog(scalarMultiply(boost, objColor), closest_t);
    } 
    
    float N_dot_I = dot(dir, normal);
    Vec3 reflectDir = subtract(dir, scalarMultiply(2.0f * N_dot_I, normal));
    reflectDir = normalize(reflectDir);

    if (is_water) {
        Vec3 reflCol = trace_ray(hitPoint, reflectDir, depth + 1, w);
        if (isNight) {
            finalColor = add(scalarMultiply(0.2f, objColor), scalarMultiply(0.8f, reflCol));
        } else {
            Vec3 waterBase = {0.1f, 0.3f, 0.5f};
            finalColor = add(scalarMultiply(0.4f, waterBase), scalarMultiply(0.6f, reflCol));
        }
    } 
    else {
        Vec3 lightDir = normalize(subtract(lightpos, hitPoint));
        float dist2 = distance2(lightpos, hitPoint);
        float N_dot_L = fmaxf(0.0f, dot(normal, lightDir));
        
        float ambient = isNight ? 0.2f : 0.4f;
        float diffuse = 0.0f;

        if (isNight) {
            diffuse = N_dot_L * (lightBrightness / dist2);
        } else {
            diffuse = N_dot_L * 0.8f; 
        }

        if (is_in_shadow(hitPoint, lightDir, w)) diffuse = 0.0f;

        Vec3 litColor = scalarMultiply(ambient + diffuse, objColor);
        
        if (isNight) {
            litColor = add(litColor, (Vec3){0.02f, 0.08f, 0.12f});
        }

        if (depth < MAX_RECURSION_DEPTH) {
            Vec3 reflCol = trace_ray(hitPoint, reflectDir, depth + 1, w);
            float refStr = isNight ? 0.3f : 0.1f; 
            finalColor = add(scalarMultiply(1.0f-refStr, litColor), scalarMultiply(refStr, reflCol));
        } else {
            finalColor = litColor;
        }
    }

    return apply_fog(finalColor, closest_t);
}

// --- PROCEDURAL SHADER ---
static Vec3 get_procedural_color(Vec3 p, Cube *c, bool *is_emissive) {
    *is_emissive = false;
    
    if (!isNight) return c->color;

    Vec3 wallColor = {0.1f, 0.1f, 0.15f}; 
    if (c->color.x < 0.1 && c->color.y < 0.1 && c->color.z < 0.1) return wallColor; 

    float floorH = 1.0f; float pillarS = 1.5f;
    bool isRow = fmodf(fabsf(p.y), floorH) < 0.7f;
    bool isCol = false;
    if (fabsf(p.x - c->min.x) < 0.1 || fabsf(p.x - c->max.x) < 0.1) isCol = fmodf(fabsf(p.z), pillarS) < 1.0f;
    else if (fabsf(p.z - c->min.z) < 0.1 || fabsf(p.z - c->max.z) < 0.1) isCol = fmodf(fabsf(p.x), pillarS) < 1.0f;
    
    if (isRow && isCol) {
        float noise = random_noise((int)(p.x*2), (int)(p.y*2), (int)(p.z*2));
        if (noise > 0.3f) { *is_emissive = true; return c->color; }
    }
    return wallColor; 
}

// --- FOG ---
static Vec3 apply_fog(Vec3 color, float dist) {
    float density = isNight ? 0.005f : 0.002f;
    float fogAmt = 1.0f - expf(-dist * density);
    
    
    if (fogAmt > 1.0f) fogAmt = 1.0f; 
    if (fogAmt < 0.0f) fogAmt = 0.0f;
    
    Vec3 fogColor = isNight ? (Vec3){0.0f, 0.15f, 0.25f} : (Vec3){0.6f, 0.8f, 1.0f};
    return lerp(color, fogColor, fogAmt);
}

// --- RENDER LOOP ---
static void renderPPM(World *w, char *ppmfile) {
    FILE *ppm = fopen(ppmfile, "w");
    fprintf(ppm, "P3\n%d %d\n255\n", imgWidth, imgHeight);
    float invSamples = 1.0f / SAMPLES_PER_PIXEL;
    int samples = sqrt(SAMPLES_PER_PIXEL);

    for (int y = 0; y < imgHeight; y++) {
        for (int x = 0; x < imgWidth; x++) {
            Vec3 col = {0,0,0};
            for (int sy = 0; sy < samples; sy++) {
                for (int sx = 0; sx < samples; sx++) {
                    float u = (float)x + (sx + 0.5f)/samples;
                    float v = (float)y + (sy + 0.5f)/samples;
                    col = add(col, trace_ray(cameraPos, calculate_ray_direction(u,v,imgWidth,imgHeight), 0, w));
                }
            }
            Vec3 final = scalarMultiply(invSamples, col);
            
            
            if (final.x > 1.0f) final.x = 1.0f;
            if (final.y > 1.0f) final.y = 1.0f;
            if (final.z > 1.0f) final.z = 1.0f;
            
            writeColour(ppm, final);
        }
    }
    fclose(ppm);
}

static Vec3 calculate_ray_direction(float px, float py, int w, int h) {
    float u = px / w; float v = py / h;
    return normalize((Vec3){(u - 0.5f)*vportWidth, (0.5f - v)*vportHeight, -focalLength});
}

static bool is_in_shadow(Vec3 point, Vec3 light_dir, World *w) {
    Vec3 orig = add(point, scalarMultiply(REF_EPSILON, light_dir));
    float t;
    for (int i=0; i<w->sphere_size; i++) if(doesIntersect(w->spheres[i], orig, light_dir, &t) && t > REF_EPSILON) return true;
    for (int i=0; i<w->cube_size; i++) if(doesCubeIntersect(w->cubes[i], orig, light_dir, &t) && t > REF_EPSILON) return true;
    return false;
}

static Vec3 get_cube_normal(Vec3 p, Cube *c) {
    float eps = 0.001f;
    if (fabsf(p.x - c->min.x) < eps) return (Vec3){-1,0,0};
    if (fabsf(p.x - c->max.x) < eps) return (Vec3){1,0,0};
    if (fabsf(p.y - c->min.y) < eps) return (Vec3){0,-1,0};
    if (fabsf(p.y - c->max.y) < eps) return (Vec3){0,1,0};
    if (fabsf(p.z - c->min.z) < eps) return (Vec3){0,0,-1};
    return (Vec3){0,0,1};
}
