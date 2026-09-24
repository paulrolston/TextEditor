#ifndef TOAST_H
#define TOAST_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define EASE_IN_CUBE(x) x*x*x
#define EASE_OUT_CUBE(x) 1 - ((1 - x)*(1 - x)*(1 - x))
#define LIFESPAN 5.0

typedef struct UIToast {
    // store 2 positions to interpolate between
    // so we can start off screen and slide
    float x1, x2, y1, y2, w, h;
    float t; // progress of lerp
    double age;
    char text[50]; // text should not be more than a sentance.
    SDL_Color background, border, foreground;
    SDL_FRect toast_r;
    SDL_FRect text_r;
    SDL_Texture* text_t;
    bool destroy;
} UIToast;

UIToast* create_toast(SDL_Renderer* renderer, TTF_Font* font, const char* text, float x1, float y1, float x2, float y2, float w, float h, SDL_Color background, SDL_Color border, SDL_Color foreground);
void update_toast(UIToast* toast);
void draw_toast(SDL_Renderer* renderer, UIToast* toast);

#endif // TOAST.H_H