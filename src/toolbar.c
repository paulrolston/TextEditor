#include "toolbar.h"

Toolbar* create_toolbar(){
    Toolbar* tb = malloc(sizeof(Toolbar));
    tb->toolbar_r = (SDL_FRect) {
        .x=0,
        .y=0,
        .h=30,
        .w=0,
    };
    tb->text_r = (SDL_FRect) {
        .x=0,
        .y=0,
        .h=0,
        .w=0,
    };
    tb->background = (SDL_Color){.r=15,.g=15,.b=18,.a=255};
    tb->border = (SDL_Color){.r=200,.g=200,.b=200,.a=255};
    tb->foreground = (SDL_Color){.r=240,.g=240,.b=240,.a=255};
    tb->text_t = NULL;
}

void change_text(Toolbar* tb, char* new_text, SDL_Renderer* r, TTF_Font* font){
    if (strcmp(tb->text, new_text) == 0) return;
    float scale = SDL_GetWindowDisplayScale(SDL_GetRenderWindow(r));
    char text[100] = "Editing: \0";
    strcat(text, new_text);
    SDL_Surface* file_surface = TTF_RenderText_Shaded(font,text,0,tb->foreground,tb->background);
    tb->text_t = SDL_CreateTextureFromSurface(r, file_surface);
    SDL_DestroySurface(file_surface);
    tb->text_r = (SDL_FRect){.x=tb->toolbar_r.x*scale,.y=tb->toolbar_r.y*scale};
    SDL_GetTextureSize(tb->text_t,&tb->text_r.w,&tb->text_r.h);
    tb->text_r.y = ((tb->toolbar_r.h*scale)-tb->text_r.h)*0.5;
    tb->text_r.x = 8*scale;
}

void draw_toolbar(SDL_Renderer* renderer, Toolbar* tb){
    float scale = SDL_GetWindowDisplayScale(SDL_GetRenderWindow(renderer));
    int w, h;
    SDL_GetWindowSize(SDL_GetRenderWindow(renderer), &w, &h);
    tb->toolbar_r.w = w;
    SDL_FRect scaled = {
        .x = tb->toolbar_r.x*scale,
        .y = tb->toolbar_r.y*scale,
        .w = tb->toolbar_r.w*scale,
        .h = tb->toolbar_r.h*scale,
    };
    SDL_SetRenderDrawColor(renderer, tb->background.r,tb->background.g, tb->background.b,tb->background.a);
    SDL_RenderFillRect(renderer, &scaled);
    SDL_SetRenderDrawColor(renderer, tb->foreground.r,tb->border.g, tb->border.b,tb->border.a);
    SDL_RenderRect(renderer, &scaled);
    if (tb->text_t != NULL) SDL_RenderTexture(renderer, tb->text_t,NULL,&tb->text_r);
}