#include "button.h"

Button* create_button(SDL_Renderer* rend, TTF_Font* font, 
    const char* text, float x, float y, float w, float h,
    SDL_Color background, SDL_Color border, SDL_Color foreground){
    float scale = SDL_GetWindowDisplayScale(SDL_GetRenderWindow(rend));
    Button* b = malloc(sizeof(Button));
    strcpy(b->text, text);
    b->button_r = (SDL_FRect) {
        .x = x, .y = y, .w = w, .h = h, 
    };
    b->button_r = (SDL_FRect) {0};
    //transparent text background.
    SDL_Surface* s = TTF_RenderText_Shaded(font, b->text, 0, b->foreground,(SDL_Color){240,240,255,SDL_ALPHA_TRANSPARENT});
    b->text_t = SDL_CreateTextureFromSurface(rend, s);
    b->text_r = (SDL_FRect){0};
    SDL_GetTextureSize(b->text_t, &b->text_r.w,&b->text_r.h);
    //shrink text size by 1/scale to account for scaling in font loading.
    b->text_r.x = (b->button_r.x + (b->button_r.w-b->text_r.w*(1/scale))*0.5);
    b->text_r.y = (b->button_r.y + (b->button_r.h-b->text_r.h*(1/scale))*0.5)-2;
    SDL_DestroySurface(s);
}