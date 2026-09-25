#include "text_window.h"

Text_window* create_window(int x, int y, int w, int h){
    Text_window* window = malloc(sizeof(Text_window));
    window->background = (SDL_Color){.r=25,.g=25,.b=30,.a=SDL_ALPHA_OPAQUE};
    window->foreground = (SDL_Color){.r=240,.g=240,.b=240,.a=SDL_ALPHA_OPAQUE};
    window->data = init_editor();
    window->x = x;
    window->y = y;
    window->w = w;
    window->h = h;
    window->display_numbers = false;
    window->scrollX = 0;
    window->scrollY = 0;
    window->maxHorizontalScroll = 0;
    window->maxVerticalScroll = 0;
    return window;
}

void scroll_text(Text_window* window, int x_amount, int y_amount){
    //We need to know if we need to scroll in the x or y direction
    //TODO; update this with a scroll_velocity, to make scroll feel better
    float text_height = TTF_GetFontHeight(font);
    window->scrollX-=x_amount*5;
    window->scrollY-=y_amount*5;
    
    window->maxVerticalScroll = -(window->data->line_count-1)*text_height/displayScale;
    
    if (window->scrollX > 0) window->scrollX =0;
    if (window->scrollY > 0) window->scrollY =0;
    if (window->scrollY <= window->maxVerticalScroll) window->scrollY = window->maxVerticalScroll;
    if (window->scrollX <= window->maxHorizontalScroll) window->scrollX = window->maxHorizontalScroll;

    return;
}

void draw_window(SDL_Renderer* renderer, TTF_Font* font, Text_window* window){
    EditorData* data = window->data;
    int line_num_off = (window->display_numbers) ? 40*displayScale : 0;
    window->w = screenW;
    window->h = screenH-window->y;
    SDL_SetRenderDrawColor(renderer, window->background.r,window->background.g,window->background.b,window->background.a);
    SDL_RenderFillRect(renderer, &(SDL_FRect){window->x*displayScale,window->y*displayScale,window->w*displayScale,window->h*displayScale});
    if (data == NULL) return;
    float text_height = TTF_GetFontHeight(font);
    SDL_FRect dst = {0};
    SDL_Rect clip_rect = {
        .x = window->x*displayScale,
        .y = window->y*displayScale,
        .w = window->w*displayScale-line_num_off,
        .h = window->h*displayScale,
    };
    SDL_SetRenderClipRect(renderer, &clip_rect);
    SDL_Texture* num_texture = NULL;
    char number[10];
    for (int i = 0; i < data->line_count; i++){
        dst.y = (window->y+5+window->scrollY)*displayScale+(text_height*i);
        dst.x = (window->x+5+window->scrollX)*displayScale + line_num_off;
        if (dst.y >= screenH*displayScale) break; // break since subsequent lines will also be off screen
        if (dst.y <= 0) continue;

        EditorLine* l = &(data->lines[i]);
        if (l == NULL) continue;
        if (l->dirty){
            if (l->texture != NULL) SDL_DestroyTexture(l->texture);
            SDL_Surface *line = TTF_RenderText_Shaded(font,l->text,l->length,window->foreground,window->background);
            l->texture = SDL_CreateTextureFromSurface(renderer, line);
            SDL_DestroySurface(line);
        }
        if (window->display_numbers) {
            snprintf(number,10,"%d",i);
            SDL_Surface *num = TTF_RenderText_Shaded(font, number,strlen(number),window->foreground,window->background);
            num_texture = SDL_CreateTextureFromSurface(renderer, num);
            SDL_DestroySurface(num);
        }
        SDL_GetTextureSize(l->texture, &dst.w, &dst.h);
        if (dst.w > clip_rect.w) {
            window->maxHorizontalScroll = (clip_rect.w-dst.w)/displayScale-50;
        }
        SDL_RenderTexture(renderer,l->texture,NULL,&dst);
        if (window->display_numbers){
            dst.x = window->x*displayScale;
            SDL_GetTextureSize(num_texture, &dst.w, &dst.h);
            SDL_RenderTexture(renderer,num_texture,NULL,&dst);
        }
    }
    SDL_SetRenderDrawColor(renderer, window->foreground.r,window->foreground.g,window->foreground.b,window->foreground.a);
    int width = 0;
    int h = 0;
    TTF_GetStringSize(font,get_line(data)->text,data->cursor_x,&width,&h);
    float y_off = (window->y+5+window->scrollY)*displayScale+h*data->cursor_y;
    if (data->cursor_x == 0) width = 0;
    if (window->display_numbers) {
        width+=line_num_off;
        SDL_RenderFillRect(renderer,&(SDL_FRect){(window->x)*displayScale+line_num_off, window->y*displayScale,1*displayScale,window->h*displayScale});
    }
    SDL_FRect cursor =  {.x=(window->x+5+window->scrollX)*displayScale+width,.y=y_off,.w=2,.h=h};
    SDL_RenderFillRect(renderer, &cursor);
    SDL_SetRenderClipRect(renderer, NULL);
}