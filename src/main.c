#define SDL_MAIN_USE_CALLBACKS 1
#include <ctype.h>
#include <limits.h>
#include "editor.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static TTF_Font *font = NULL;

EditorData* e_data = NULL;

SDL_Color bg = { 0, 0, 0, SDL_ALPHA_TRANSPARENT };
SDL_Color fg = { 240, 240, 240, SDL_ALPHA_OPAQUE };

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (argc < 2) {
        printf("Please enter a file to edit.\n");
        return SDL_APP_FAILURE;
    }

    e_data = init_editor();
    //Get absolute path to the provided file.
    realpath(argv[1], e_data->file_path);
    if (e_data->file_path == NULL){
        printf("Path couldn't be resolved.");
        return SDL_APP_FAILURE;
    }

    //Now load file contents into our struct
    load_file(e_data);
    

    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", 800, 600, SDL_WINDOW_HIGH_PIXEL_DENSITY, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize SDL_ttf: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Open the font */
    font = TTF_OpenFont("/Users/paul/code/C/Text/fonts/default.ttf", 28.0f);
    if (!font) {
        SDL_Log("Couldn't open font: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_StartTextInput(window);

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type){
        case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
        case SDL_EVENT_TEXT_INPUT:{
            EditorLine* l = get_line(e_data);
            if (l == NULL) break;
            append_line(e_data, l, event->text.text);
            break;
        }
        case SDL_EVENT_KEY_DOWN:{
            EditorLine* l = get_line(e_data);
            if (l == NULL) break;
            if (event->key.key == SDLK_RETURN){
                create_new_line(e_data);
            }
            if (event->key.key == SDLK_BACKSPACE) {
                line_backspace(e_data, l);
            }
            if (event->key.mod & (SDL_KMOD_LCTRL|SDL_KMOD_RCTRL)){
                switch (event->key.key){
                    case SDLK_R:{
                        if (e_data->mode == REPLACE) return SDL_APP_CONTINUE;
                        printf("Now in replace mode!\n");
                        e_data->mode = REPLACE;
                        break;
                    }
                    case SDLK_I:{
                        if (e_data->mode == INSERT) return SDL_APP_CONTINUE;
                        printf("Now in insert mode!\n");
                        e_data->mode = INSERT;
                        break;
                    }
                    case SDLK_P:{
                        for (int li = 0; li < e_data->line_count;li++){
                            printf("%s\n",e_data->lines[li].text);
                        }
                        break;
                    }
                    case SDLK_S:{
                        save_file(e_data);
                        break;
                    }
                }
            }
            
            if (event->key.key == SDLK_UP){
                if (e_data->cursor_y > 0) {
                    e_data->cursor_y--;
                    int len = get_line(e_data)->length;
                    if (e_data->cursor_x > len) {
                        e_data->cursor_x = len;
                    }
                }
                return SDL_APP_CONTINUE;
            }
            if (event->key.key == SDLK_DOWN){
                if (e_data->cursor_y +1 < e_data->line_count) {
                    e_data->cursor_y++;
                    int len = get_line(e_data)->length;
                    if (e_data->cursor_x > len) {
                        e_data->cursor_x = len;
                    }
                }
                return SDL_APP_CONTINUE;
            }
            if (event->key.key == SDLK_LEFT){
                if (e_data->cursor_x == 0) {
                    if (e_data->cursor_y > 0) {
                        e_data->cursor_y--;
                        e_data->cursor_x = e_data->lines[e_data->cursor_y].length;
                    }
                    return SDL_APP_CONTINUE;
                }
                e_data->cursor_x--;
            }
            if (event->key.key == SDLK_RIGHT){
                if (e_data->cursor_x==e_data->lines[e_data->cursor_y].length){
                    if (e_data->cursor_y < e_data->line_count-1){
                        e_data->cursor_x=0;
                        e_data->cursor_y++;
                    }
                    return SDL_APP_CONTINUE;
                }
                e_data->cursor_x++;
            }
        }
        break;
    }
    return SDL_APP_CONTINUE;
}


/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderDrawColor(renderer, 25, 25, 30, 255);
    SDL_RenderClear(renderer);
    float text_height = TTF_GetFontHeight(font);
    SDL_FRect dst = {.x=5,.y=5};
    for (int i = 0; i < e_data->line_count; i++){
        EditorLine* l = &(e_data->lines[i]);
        if (l == NULL) continue;
        if (l->dirty){
            if (l->texture != NULL) SDL_DestroyTexture(l->texture);
            SDL_Surface *surface = TTF_RenderText_Shaded(font,l->text,l->length,fg,bg);
            l->texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
        }
        SDL_GetTextureSize(l->texture, &dst.w, &dst.h);
        dst.y = 5 + text_height*i;
        SDL_RenderTexture(renderer,l->texture,NULL,&dst);
    }
    SDL_RenderTexture(renderer, texture, NULL, &dst);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int width = 0;
    int h = 0;
    TTF_GetStringSize(font,get_line(e_data)->text,e_data->cursor_x,&width,&h);
    float y_off = 5+h*e_data->cursor_y;
    if (e_data->cursor_x == 0) width = 0;
    // float x_off = e_data->cursor_x*12;
    SDL_FRect cursor =  {.x=5+width,.y=y_off,.w=2,.h=h};
    SDL_RenderFillRect(renderer, &cursor);
    // SDL_RenderLine(renderer, 5+x_off,y_off,5+x_off,text_height+y_off+5);
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
}