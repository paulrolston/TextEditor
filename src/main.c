#define SDL_MAIN_USE_CALLBACKS 1
#include <ctype.h>
#include <limits.h>
#include "editor.h"
#include "text_window.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static TTF_Font *font = NULL;

Text_window* text_window=NULL;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (argc < 2) {
        printf("Please enter a file to edit.\n");
        return SDL_APP_FAILURE;
    }
    text_window = create_window(0,30,800,580);
    //Get absolute path to the provided file.
    realpath(argv[1], text_window->data->file_path);
    if (text_window->data->file_path == NULL){
        printf("Path couldn't be resolved.");
        return SDL_APP_FAILURE;
    }
    //Now load file contents into our struct
    load_file(text_window->data);
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", 800, 600, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE, &window, &renderer)) {
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
            EditorLine* l = get_line(text_window->data);
            if (l == NULL) break;
            append_line(text_window->data, l, event->text.text);
            break;
        }
        case SDL_EVENT_KEY_DOWN:{
            EditorLine* l = get_line(text_window->data);
            if (l == NULL) break;
            if (event->key.key == SDLK_RETURN){
                create_new_line(text_window->data);
            }
            if (event->key.key == SDLK_BACKSPACE) {
                line_backspace(text_window->data, l);
            }
            if (event->key.mod & (SDL_KMOD_LCTRL|SDL_KMOD_RCTRL)){
                switch (event->key.key){
                    case SDLK_R:{
                        if (text_window->data->mode == REPLACE) return SDL_APP_CONTINUE;
                        printf("Now in replace mode!\n");
                        text_window->data->mode = REPLACE;
                        break;
                    }
                    case SDLK_I:{
                        if (text_window->data->mode == INSERT) return SDL_APP_CONTINUE;
                        printf("Now in insert mode!\n");
                        text_window->data->mode = INSERT;
                        break;
                    }
                    case SDLK_P:{
                        for (int li = 0; li < text_window->data->line_count;li++){
                            printf("%s\n",text_window->data->lines[li].text);
                        }
                        break;
                    }
                    case SDLK_S:{
                        save_file(text_window->data);
                        break;
                    }
                }
            }
            
            if (event->key.key == SDLK_UP){
                if (text_window->data->cursor_y > 0) {
                    text_window->data->cursor_y--;
                    int len = get_line(text_window->data)->length;
                    if (text_window->data->cursor_x > len) {
                        text_window->data->cursor_x = len;
                    }
                }
                return SDL_APP_CONTINUE;
            }
            if (event->key.key == SDLK_DOWN){
                if (text_window->data->cursor_y +1 < text_window->data->line_count) {
                    text_window->data->cursor_y++;
                    int len = get_line(text_window->data)->length;
                    if (text_window->data->cursor_x > len) {
                        text_window->data->cursor_x = len;
                    }
                }
                return SDL_APP_CONTINUE;
            }
            if (event->key.key == SDLK_LEFT){
                if (text_window->data->cursor_x == 0) {
                    if (text_window->data->cursor_y > 0) {
                        text_window->data->cursor_y--;
                        text_window->data->cursor_x = text_window->data->lines[text_window->data->cursor_y].length;
                    }
                    return SDL_APP_CONTINUE;
                }
                text_window->data->cursor_x--;
            }
            if (event->key.key == SDLK_RIGHT){
                if (text_window->data->cursor_x==text_window->data->lines[text_window->data->cursor_y].length){
                    if (text_window->data->cursor_y < text_window->data->line_count-1){
                        text_window->data->cursor_x=0;
                        text_window->data->cursor_y++;
                    }
                    return SDL_APP_CONTINUE;
                }
                text_window->data->cursor_x++;
            }
        }
        break;
    }
    return SDL_APP_CONTINUE;
}


/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    float scale = SDL_GetWindowDisplayScale(SDL_GetRenderWindow(renderer));
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    int w, h;
    SDL_FRect tool_bar = {0};
    SDL_GetWindowSize(SDL_GetRenderWindow(renderer), &w, &h);
    tool_bar.w = (w)*scale;
    tool_bar.h = 30*scale;
    //draw the file name in a toolbar.
    SDL_SetRenderDrawColor(renderer, 15,15, 18,255);
    SDL_RenderFillRect(renderer, &tool_bar);
    SDL_SetRenderDrawColor(renderer, 200,200,220,255);
    SDL_RenderRect(renderer, &tool_bar);
    // SDL_SetRenderDrawColor(renderer, 240,240,240,255);
    char text[100] = "File: \0";
    strcat(text, text_window->data->file_path);
    SDL_Surface* file_surface = TTF_RenderText_Shaded(font,text,0,text_window->foreground,(SDL_Color){0,0,0,SDL_ALPHA_TRANSPARENT});
    SDL_Texture* file_tex = SDL_CreateTextureFromSurface(renderer, file_surface);
    SDL_DestroySurface(file_surface);
    SDL_FRect file_rect = {tool_bar.x,tool_bar.y};
    SDL_GetTextureSize(file_tex,&file_rect.w,&file_rect.h);
    SDL_RenderTexture(renderer, file_tex,NULL,&file_rect);
    //draw the text window
    draw_window(renderer, font, text_window);
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