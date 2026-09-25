#ifndef GLOBALS_H
#define GLOBALS_H

#include <stddef.h>
#include <stdint.h>
#define XXH_STATIC_LINKING_ONLY
#include "xxhash.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#define EASE_IN_CUBE(x) x*x*x
#define EASE_OUT_CUBE(x) 1 - ((1 - x)*(1 - x)*(1 - x))
#define TOAST_HEIGHT 30
#define TOAST_WIDTH 100
#define TOAST_GAP 10

extern double deltaTime;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_Font *font;

extern int screenW, screenH;

#endif // GLOBALS.H_H
