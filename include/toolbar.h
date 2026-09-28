#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "button.h"
#include "text_window.h"

#define MAX_TABS 8

typedef struct ToolbarTab {
    char file_path[PATH_MAX];
    char file_name[50];
    SDL_Texture* text_texture;
    Button button;
    SDL_FRect rect, text_rect;
} ToolbarTab;

typedef struct Toolbar {
    SDL_FRect toolbar_r;
    SDL_FRect text_r;
    char text[50];
    ToolbarTab tabs[MAX_TABS];
    ssize_t tab_count;
    SDL_Color background, border, foreground;
    SDL_Texture* text_t;
} Toolbar;

Toolbar* create_toolbar();
void add_tab(Toolbar* tb, const char* file_path);
void change_text(Toolbar* tb, char* text, SDL_Renderer* r, TTF_Font* font);
void draw_toolbar(SDL_Renderer* renderer, Toolbar* tb, Text_window* tw);

#endif // TOOLBAR.H_H