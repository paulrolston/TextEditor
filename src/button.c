#include "button.h"

Button* create_button(SDL_Renderer* rend, TTF_Font* font, 
    const char* text, float x, float y, float w, float h, 
    SDL_Color background, SDL_Color border, SDL_Color foreground, 
    ButtonCallback down, ButtonCallback clicked, ButtonCallback released) {
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
    b->isPressed = false;
    b->isReleased = false;
    b->isHovered = false;
    //can pass NULL for the callbacks
    b->pressed_cb = down;
    b->released_cb = released;
    b->clicked_cb = clicked;
    return b;
}

void update_button(Button* b){
    b->isReleased = false;
    bool was_pressed = b->isPressed;
    float mouse_x = 0, mouse_y = 0;
    SDL_MouseButtonFlags state = SDL_GetMouseState(&mouse_x, &mouse_y);
    //Update the buttons boolean state variables.
    if (((mouse_x >= b->button_r.x) && (mouse_x <= b->button_r.x+b->button_r.w)) &&
        ((mouse_y >= b->button_r.y) &&(mouse_y <= b->button_r.y+b->button_r.h))){
            b->isHovered = true;
            if ((state & SDL_BUTTON_LMASK) == 1) b->isPressed = true;
            else if (was_pressed) {b->isReleased = true; b->isPressed = false;}
        }else{
            b->isHovered = false;
        }
    //After updating state, call the relevant callbacks
    if (b->isPressed){
        if (!was_pressed){
            if (b->clicked_cb) b->clicked_cb(b, NULL);
        }
        if (b->pressed_cb) b->pressed_cb(b, NULL);
    }
    if (b->isReleased){
        if (b->released_cb) b->released_cb(b, NULL);
    }
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