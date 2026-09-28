#ifndef TEXT_WINDOW_H
#define TEXT_WINDOW_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "editor.h"
#include "globals.h"


typedef struct TextWindow {
    int x, y, w, h;
    SDL_Color background, foreground;
    EditorData* data;
    bool display_numbers;
    int scrollX, scrollY;
    int maxHorizontalScroll, maxVerticalScroll;
} TextWindow;

//Return a pointer to a text_window.
TextWindow* create_window(int x, int y, int w, int h);
//Draw the contents of the text being edited on the window.
void draw_window(SDL_Renderer* renderer, TTF_Font* font, TextWindow* window);
void scroll_text(TextWindow* window, int x_amount, int y_amount);
void destroy_window(TextWindow* window);

#endif // TEXT_WINDOW.H_H