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
    return window;
}

void draw_window(SDL_Renderer* renderer, TTF_Font* font, Text_window* window){
    EditorData* data = window->data;
    SDL_Window* w = SDL_GetRenderWindow(renderer);
    const int line_num_off = 40;
    float scale = SDL_GetWindowDisplayScale(w);
    SDL_SetRenderDrawColor(renderer, window->background.r,window->background.g,window->background.b,window->background.a);
    SDL_RenderFillRect(renderer, &(SDL_FRect){window->x*scale,window->y*scale,window->w*scale,window->h*scale});
    if (data == NULL) return;
    float text_height = TTF_GetFontHeight(font);
    SDL_FRect dst = {.x=5,.y=5,.w=0,.h=0};
    SDL_Rect clip_rect = {
        .y = window->y*scale,
        .x = window->x*scale,
        .w = window->w*scale,
        .h = window->h*scale,
    };
    SDL_SetRenderClipRect(renderer, &clip_rect);
    SDL_Texture* t = NULL;
    for (int i = 0; i < data->line_count; i++){
        EditorLine* l = &(data->lines[i]);
        if (l == NULL) continue;
        if (l->dirty){
            if (l->texture != NULL) SDL_DestroyTexture(l->texture);
            SDL_Surface *line = TTF_RenderText_Shaded(font,l->text,l->length,window->foreground,window->background);
            char number[10];
            if (window->display_numbers) {
                snprintf(number,10,"%-3d",i+1);
                SDL_Surface *num = TTF_RenderText_Shaded(font, number,strlen(number),window->foreground,window->background);
                t = SDL_CreateTextureFromSurface(renderer, num);
            }
            l->texture = SDL_CreateTextureFromSurface(renderer, line);
            SDL_DestroySurface(line);
        }
        SDL_GetTextureSize(l->texture, &dst.w, &dst.h);
        dst.y = (window->y)*scale+5+text_height*i;
        dst.x = (window->x+5)*scale + ((window->display_numbers) ? line_num_off*scale : 0);
        SDL_RenderTexture(renderer,l->texture,NULL,&dst);
        if (window->display_numbers){
            dst.x -= line_num_off*scale;
            SDL_GetTextureSize(t, &dst.w, &dst.h);
            SDL_RenderTexture(renderer,t,NULL,&dst);
        }
    }
    SDL_SetRenderDrawColor(renderer, window->foreground.r,window->foreground.g,window->foreground.b,window->foreground.a);
    int width = 0;
    int h = 0;
    TTF_GetStringSize(font,get_line(data)->text,data->cursor_x,&width,&h);
    float y_off = (window->y)*scale+5+h*data->cursor_y;
    if (data->cursor_x == 0) width = 0;
    if (window->display_numbers) {
        width+=line_num_off*scale;
        SDL_RenderFillRect(renderer,&(SDL_FRect){(window->x+line_num_off)*scale, window->y*scale,1*scale,window->h*scale});
    }
    // float x_off = e_data->cursor_x*12;
    SDL_FRect cursor =  {.x=(window->x+5)*scale+width,.y=y_off,.w=2,.h=h};
    SDL_RenderFillRect(renderer, &cursor);
    SDL_SetRenderClipRect(renderer, NULL);
}