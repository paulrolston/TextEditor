#ifndef TEXT_WINDOW_H
#define TEXT_WINDOW_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "editor.h"


typedef struct Text_window {
    int x, y, w, h;
    SDL_Color background, foreground;
    EditorData* data;
} Text_window;

//Return a pointer to a text_window.
Text_window* create_window(int x, int y, int w, int h);
//Draw the contents of the text being edited on the window.
void draw_window(SDL_Renderer* renderer, TTF_Font* font, Text_window* window);

#endif // TEXT_WINDOW.H_H