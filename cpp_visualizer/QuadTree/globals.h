#pragma once
#include <SDL.h>

// ���������� SDL-�������
extern SDL_Window* win;
extern SDL_Renderer* ren;
extern int win_width;
extern int win_height;

// ��������� �������
struct Dt {
    const double fps = 60.0;
    double newtime = 0;
    double lasttime = SDL_GetTicks() / 1000.0;
    double dt = 0;
};

// ���������� ���������� �������
extern Dt fps;