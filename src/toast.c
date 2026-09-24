#include "toast.h"

UIToast* create_toast(SDL_Renderer* renderer, TTF_Font* font, 
    const char text, float x1, float y1, float x2, float y2, float w, float h, 
    SDL_Color background, SDL_Color border, SDL_Color foreground){

        UIToast* t = malloc(sizeof(UIToast));
        t->x1 = x1;
        t->x2 = x2;
        t->y1 = y1;
        t->y2 = y2;
        t->w = w;
        t->h = h;
        t->background = background;
        t->foreground = foreground;
        t->border = border;
        t->toast_r = (SDL_FRect){0};
        t->t = 0;
        strcpy(t->text, text);
        float scale = SDL_GetWindowDisplayScale(SDL_GetRenderWindow(renderer));
        //Render text to a texture;
        SDL_Surface* s = TTF_RenderText_Shaded(font, t->text, 0, t->foreground,(SDL_Color){0,0,0,SDL_ALPHA_TRANSPARENT});
        t->text_t = SDL_CreateTextureFromSurface(renderer, s);
        t->text_r = (SDL_FRect){0};
        SDL_GetTextureSize(t->text_t,&t->text_r.w,&t->text_r.h);
        SDL_DestroySurface(s);

}
void update_toast(UIToast* toast);

void draw_toast(SDL_Renderer* renderer, UIToast* toast){
    float scale = SDL_GetWindowDisplayScale(SDL_GetRenderWindow(renderer));
    SDL_FRect scaled = {
        .x = toast->toast_r.x*scale,
        .y = toast->toast_r.y*scale,
        .w = toast->toast_r.w*scale,
        .h = toast->toast_r.h*scale,
    };
    SDL_SetRenderDrawColor(renderer, toast->background.r,toast->background.g,toast->background.b,toast->background.a);
    SDL_RenderFillRect(renderer, &scaled);
    SDL_SetRenderDrawColor(renderer, toast->border.r,toast->border.g,toast->border.b,toast->border.a);
    SDL_RenderFillRect(renderer, &scaled);
    toast->text_r.x = (toast->toast_r.x + (toast->toast_r.w-toast->text_r.w*(1.0/scale))*0.5);
    toast->text_r.y = (toast->toast_r.y + (toast->toast_r.h-toast->text_r.h*(1.0/scale))*0.5)-1;
    SDL_FRect text_scaled = {
        .x=toast->text_r.x*scale,
        .y=toast->text_r.y*scale,
        .w=toast->text_r.w,
        .h=toast->text_r.h,
    };
    SDL_RenderTexture(renderer, toast->text_t, NULL, &text_scaled);
}