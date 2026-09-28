#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "button.h"
#include "toastmanager.h"
#include "text_window.h"

#define MAX_TABS 8


//Tab holds a reference to the window of text
typedef struct ToolbarTab {
    char file_path[PATH_MAX];
    char file_name[50];
    Button* button;
    SDL_FRect rect;
    bool focused;
    TextWindow* window;
    int id;
} ToolbarTab;

typedef struct Toolbar {
    SDL_FRect toolbar_r;
    ToolbarTab tabs[MAX_TABS];
    size_t tab_count;
    ToolbarTab* current;
    SDL_Color background, foreground;
} Toolbar;

Toolbar* create_toolbar();
void add_tab(Toolbar* tb, const char* file_path, ToastManager* tm);
// void change_text(Toolbar* tb, char* text, SDL_Renderer* r, TTF_Font* font);
void update_toolbar(Toolbar* tb);
void draw_toolbar(SDL_Renderer* renderer, Toolbar* tb);
void close_tab(Toolbar* tb, ToastManager* tm);
TextWindow* get_current_window(Toolbar* tb);

#endif // TOOLBAR.H_H