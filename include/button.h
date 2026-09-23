#ifndef BUTTON_H
#define BUTTON_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Button {
    SDL_FRect button_r;
    SDL_FRect text_r;
    SDL_Texture* text_t;
    char text[25];
    SDL_Color background, border, foreground;
    bool hovered;
    bool pressed;
} Button;

/*
Create a button object, pass coordinates in unscaled values, i.e raw numbers.
*/
Button* create_button(SDL_Renderer* r, TTF_Font* font, const char* text, float x, float y, float w, float h, SDL_Color background, SDL_Color border, SDL_Color foreground);

//Draws the button, automatically scales with high DPI
void draw_button(SDL_Renderer* r, Button* b);

//Update state of button by testing mouse overlap
void update_button(Button* b);

#endif // BUTTON.H_H