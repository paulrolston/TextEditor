#include "text_window.h"

Text_window* create_window(int x, int y, int w, int h){
    Text_window* window = malloc(sizeof(Text_window));
    window->background = (SDL_Color){.r=25,.g=25,.b=30,.a=SDL_ALPHA_OPAQUE};
    window->data = init_editor();
    window->x = x;
    window->h = h;
    window->w = w;
    window->h = h;
    return window;
}

void draw_window(SDL_Renderer* renderer, TTF_Font* font, Text_window* window){
    EditorData* data = window->data;
    if (data == NULL) return;
    float text_height = TTF_GetFontHeight(font);
    SDL_FRect dst = {.x=5,.y=5,.w=window->w,.h=window->h};
    for (int i = 0; i < data->line_count; i++){
        EditorLine* l = &(data->lines[i]);
        if (l == NULL) continue;
        if (l->dirty){
            if (l->texture != NULL) SDL_DestroyTexture(l->texture);
            SDL_Surface *surface = TTF_RenderText_Shaded(font,l->text,l->length,window->foreground,window->background);
            l->texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
        }
        // SDL_GetTextureSize(l->texture, &dst.w, &dst.h);
        dst.y = window->y + 5 + text_height*i;
        dst.x += window->x;
        SDL_RenderTexture(renderer,l->texture,NULL,&dst);
    }
    SDL_SetRenderDrawColor(renderer, window->foreground.r,window->foreground.a,window->foreground.a,window->foreground.a);
    int width = 0;
    int h = 0;
    TTF_GetStringSize(font,get_line(data)->text,data->cursor_x,&width,&h);
    float y_off = window->y + 5+h*data->cursor_y;
    if (data->cursor_x == 0) width = 0;
    // float x_off = e_data->cursor_x*12;
    SDL_FRect cursor =  {.x=window->x+5+width,.y=y_off,.w=2,.h=h};
    SDL_RenderFillRect(renderer, &cursor);
}