#ifndef COLOR_H
#define COLOR_H

#include "vector.h"
#include <stdio.h>

extern Vec3 *colorArray;
extern int noColours;

Vec3 hexToRGB(unsigned int hex);
void writeColour(FILE *ppmFile, Vec3 color);

int compareColor(const void *a, const void *b);

#endif // COLOR_H
