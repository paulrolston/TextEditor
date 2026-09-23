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
    b->background = background;
    b->border = border;
    b->foreground = foreground;
    //transparent text background.
    SDL_Surface* s = TTF_RenderText_Shaded(font, b->text, 0, b->foreground,(SDL_Color){0,0,0,SDL_ALPHA_TRANSPARENT});
    b->text_t = SDL_CreateTextureFromSurface(rend, s);
    b->text_r = (SDL_FRect){0};
    SDL_GetTextureSize(b->text_t,&b->text_r.w,&b->text_r.h);
    SDL_DestroySurface(s);
    b->pressed = false;
    b->hovered = false;
    return b;
}

void draw_button(SDL_Renderer* renderer, Button* b){
    float scale = SDL_GetWindowDisplayScale(SDL_GetRenderWindow(renderer));
    SDL_FRect scaled = {
        .x=b->button_r.x*scale,
        .y=b->button_r.y*scale,
        .w=b->button_r.w*scale,
        .h=b->button_r.h*scale,
    };
    SDL_SetRenderDrawColor(renderer,b->background.r,b->background.g,b->background.b,b->background.a);
    SDL_RenderFillRect(renderer, &scaled);
    SDL_SetRenderDrawColor(renderer,b->border.r,b->border.g,b->border.b,b->border.a);
    SDL_RenderRect(renderer, &scaled);
    // shrink text w/h by 1/scale to account for double font size on high dpi display
    b->text_r.x = (b->button_r.x + (b->button_r.w-b->text_r.w*(1.0/scale))*0.5);
    b->text_r.y = (b->button_r.y + (b->button_r.h-b->text_r.h*(1.0/scale))*0.5)-1;
    SDL_FRect text_scaled = {
        .x=b->text_r.x*scale,
        .y=b->text_r.y*scale,
        .w=b->text_r.w,
        .h=b->text_r.h,
    };
    SDL_RenderTexture(renderer, b->text_t,NULL, &text_scaled);
}