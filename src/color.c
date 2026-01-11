#include "color.h"
#include <math.h>

Vec3 *colorArray = NULL;
int noColours = 0;

Vec3 hexToRGB(unsigned int packedRGB) {
    unsigned int r = (packedRGB >> 16) & 0xFF;
    unsigned int g = (packedRGB >> 8) & 0xFF;
    unsigned int b = (packedRGB) & 0xFF;
    Vec3 color;
    color.x = r / 255.0f;
    color.y = g / 255.0f;
    color.z = b / 255.0f;
    return color;
}

void writeColour(FILE *ppmFile, Vec3 color) {
    float r = fminf(fmaxf(color.x, 0.0f), 1.0f);
    float g = fminf(fmaxf(color.y, 0.0f), 1.0f);
    float b = fminf(fmaxf(color.z, 0.0f), 1.0f);

#ifdef MS1

    int ir = (int)(r * 255.0f + 0.5f);
    int ig = (int)(g * 255.0f + 0.5f);
    int ib = (int)(b * 255.0f + 0.5f);
    fprintf(ppmFile, "%d %d %d\n", ir, ig, ib);
#elif MS2

    float grayscale = 0.3f * r + 0.59f * g + 0.11f * b;
    int ig = (int)(grayscale * 255.0f + 0.5f);
    fprintf(ppmFile, "%d %d %d\n", ig, ig, ig);
#else

    int ir = (int)(r * 255.0f + 0.5f);
    int ig = (int)(g * 255.0f + 0.5f);
    int ib = (int)(b * 255.0f + 0.5f);
    fprintf(ppmFile, "%d %d %d\n", ir, ig, ib);
#endif
}

int compareColor(const void *a, const void *b) {
    int a1 = 0, b1 = 0;
    for (int i = 0; i < (int)sizeof(int); i++)
    {
        a1 |= ((*( (unsigned char*)a + i)) & 0x0F) << (i * 8);
        b1 |= ((*( (unsigned char*)b + i)) & 0x0F) << (i * 8);
    }
    return (a1 < b1) ? -1 : ((b1 < a1) ? 1 : ((*(int*)a < *(int*)b) ? -1 : ((*(int*)a > *(int*)b) ? 1 : 0)));
}
