#include "toast.h"
#include "globals.h"

UIToast* create_toast(SDL_Renderer* renderer, TTF_Font* font, 
    const char* text, float x1, float y1, float x2, float y2, float w, float h, int type){

        UIToast* t = malloc(sizeof(UIToast));
        t->x1 = x1;
        t->x2 = x2;
        t->y1 = y1;
        t->y2 = y2;
        t->w = w;
        t->h = h;
        t->type = type;
        t->toast_r = (SDL_FRect){
            .x=x1,.y=y1,.w=w,.h=h
        };
        t->t = 0;
        strcpy(t->text, text);
        float scale = SDL_GetWindowDisplayScale(SDL_GetRenderWindow(renderer));
        //Render text to a texture;
        SDL_Surface* s = TTF_RenderText_Shaded(font, t->text, 0, current_theme->toast_foreground,(SDL_Color){0,0,0,SDL_ALPHA_TRANSPARENT});
        t->text_t = SDL_CreateTextureFromSurface(renderer, s);
        t->text_r = (SDL_FRect){0};
        t->age = 0;
        t->destroy = false;
        SDL_GetTextureSize(t->text_t,&t->text_r.w,&t->text_r.h);
        SDL_DestroySurface(s);
        return t;
}
void update_toast(UIToast* toast){
    if (toast->destroy) return;
    toast->age+=deltaTime;
    if (toast->age >= LIFESPAN) {
        toast->destroy = true;
    }
    float mouse_x = 0, mouse_y = 0;
    SDL_MouseButtonFlags state = SDL_GetMouseState(&mouse_x, &mouse_y);
    //Update the buttons boolean state variables.
    if (((mouse_x >= toast->toast_r.x) && (mouse_x <= toast->toast_r.x+toast->toast_r.w)) &&
        ((mouse_y >= toast->toast_r.y) &&(mouse_y <= toast->toast_r.y+toast->toast_r.h))){
            if ((state & SDL_BUTTON_LMASK) == 1) {toast->destroy = true; return;}
        }
    float x_diff = toast->x1 - toast->x2;
    float y_diff = toast->y1 - toast->y2;
    toast->t+=(2)*deltaTime;
    if (toast->t > 1) {
        toast->t = 1;
        toast->x1 = toast->x2;
        toast->y1 = toast->y2;
    }
    double u = EASE_OUT_CUBE(toast->t);
    toast->toast_r.x = toast->x1 - x_diff*u;
    toast->toast_r.y = toast->y1 - y_diff*u;
}

void draw_toast(SDL_Renderer* renderer, UIToast* toast){
    if (toast->destroy) return;
    float scale = SDL_GetWindowDisplayScale(SDL_GetRenderWindow(renderer));
    SDL_FRect scaled = {
        .x = toast->toast_r.x*scale,
        .y = toast->toast_r.y*scale,
        .w = toast->toast_r.w*scale,
        .h = toast->toast_r.h*scale,
    };
    SDL_SetRenderDrawColor(renderer, current_theme->toast_background.r,current_theme->toast_background.g,current_theme->toast_background.b,current_theme->toast_background.a);
    SDL_RenderFillRect(renderer, &scaled);
    switch (toast->type){
        case TOAST_ERROR:{
            SDL_SetRenderDrawColor(renderer, current_theme->toast_error_border.r,current_theme->toast_error_border.g,current_theme->toast_error_border.b,current_theme->toast_error_border.a);
            break;
        }
        case TOAST_SUCCESS:{
            SDL_SetRenderDrawColor(renderer, current_theme->toast_success_border.r,current_theme->toast_success_border.g,current_theme->toast_success_border.b,current_theme->toast_success_border.a);
            break;
        }
    }
    SDL_RenderRect(renderer, &scaled);
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