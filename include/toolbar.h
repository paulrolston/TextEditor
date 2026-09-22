#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>

typedef struct Toolbar {
    SDL_FRect toolbar_r;
    SDL_FRect text_r;
    char text[50];
    SDL_Color background, border, foreground;
    SDL_Texture* text_t;
} Toolbar;

Toolbar* create_toolbar();
void change_text(Toolbar* tb, char* text, SDL_Renderer* r, TTF_Font* font);
void draw_toolbar(SDL_Renderer* renderer, Toolbar* tb);

#endif // TOOLBAR.H_H