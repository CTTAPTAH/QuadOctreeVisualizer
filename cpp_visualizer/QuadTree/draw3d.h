#pragma once
#include "globals.h"
#include "geometry3d.h"

int projectX(double x, double z);
int projectY(double y, double z);
void draw3DBox(const Box3& box, SDL_Color color);
void drawFilledFace(SDL_Renderer* ren, SDL_Point p0, SDL_Point p1, SDL_Point p2, SDL_Point p3, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void drawFilledBox(const Box3& box, SDL_Renderer* ren, Uint8 r, Uint8 g, Uint8 b, Uint8 a);