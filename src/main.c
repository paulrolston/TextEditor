#define SDL_MAIN_USE_CALLBACKS 1
#include <ctype.h>
#include <limits.h>
#include "editor.h"
#include "toolbar.h"
#include "text_window.h"
#include "button.h"
#include "toastmanager.h"
#include "globals.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;
static Toolbar* tool_bar = NULL;
static Text_window* text_window=NULL;
static Button* save_button=NULL;
static ToastManager* t_manager;

double deltaTime = 0;
double lastTime = 0;

int screenW = 800;
int screenH = 600;

void save_callback(Button* b, void* data) {
    EditorData * e_data = (EditorData *) data;
    if (e_data == NULL) {
        printf("Save: data passed was NULL\n");
        return;
    }
    save_file(e_data, t_manager);
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (argc < 2) {
        printf("Please enter a file to edit.\n");
        return SDL_APP_FAILURE;
    }
    text_window = create_window(0,30,screenW,screenH-30);
    //Get absolute path to the provided file.
    realpath(argv[1], text_window->data->file_path);
    if (text_window->data->file_path == NULL){
        printf("Path couldn't be resolved.");
        return SDL_APP_FAILURE;
    }
    char* t = strrchr(text_window->data->file_path, '/');
    if (t == NULL){
        strcpy(text_window->data->file_name, text_window->data->file_path);
    }else{
        strcpy(text_window->data->file_name, (t+1));
    }
    //Now load file contents into our struct
    load_file(text_window->data);
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Text editor", screenW, screenH, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE, &window, &renderer)) {
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

    tool_bar = create_toolbar();
    change_text(tool_bar,text_window->data->file_name,renderer,font);

    save_button = create_button(renderer, font, "Save",0,0,75,tool_bar->toolbar_r.h-8,
    (SDL_Color){100,100,115,SDL_ALPHA_OPAQUE},(SDL_Color){240,240,255,SDL_ALPHA_OPAQUE},(SDL_Color){240,240,255,SDL_ALPHA_OPAQUE},
    NULL, save_callback, NULL, (void *) text_window->data);
    
    // test = create_toast(renderer, font, "Saved!",810,560,690,560,100,30,(SDL_Color){40,40,50,SDL_ALPHA_OPAQUE},(SDL_Color){240,240,255,SDL_ALPHA_OPAQUE},(SDL_Color){240,240,255,SDL_ALPHA_OPAQUE});

    t_manager = create_toast_manager();

    SDL_StartTextInput(window);

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type){
        case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
        case SDL_EVENT_WINDOW_RESIZED:
            SDL_GetWindowSize(window, &screenW, &screenH);
            return SDL_APP_CONTINUE;
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
                        save_file(text_window->data, t_manager);
                        break;
                    }
                    case SDLK_L:{
                        text_window->display_numbers=!text_window->display_numbers;
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
    double currentTime = SDL_GetTicksNS()/1e9;
    deltaTime = (currentTime)-lastTime;
    float scale = SDL_GetWindowDisplayScale(SDL_GetRenderWindow(renderer));
    //Update components.
    update_button(save_button);
    update_toast_manager(t_manager);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    draw_toolbar(renderer, tool_bar);
    //update button position.
    save_button->button_r.x = tool_bar->toolbar_r.x + (tool_bar->toolbar_r.w - save_button->button_r.w) - 4;
    save_button->button_r.y = tool_bar->toolbar_r.y+4;
    //draw button
    draw_button(renderer, save_button);
    //draw the text window
    draw_window(renderer, font, text_window);
    draw_toast_manager(renderer, t_manager);
    SDL_RenderPresent(renderer);
    lastTime = currentTime;
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