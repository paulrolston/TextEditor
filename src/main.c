#define SDL_MAIN_USE_CALLBACKS 1
#include <ctype.h>
#include <limits.h>
#include "editor.h"
#include "toolbar.h"
#include "text_window.h"
#include "button.h"
#include "toastmanager.h"
#include "globals.h"
#include "theme.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *text_font = NULL;
TTF_Font *toolbar_font = NULL;
static Toolbar* tool_bar = NULL;
// static Button* save_button=NULL;
static ToastManager* t_manager;
UITheme* current_theme = &default_light;

double deltaTime = 0;
double lastTime = 0;
float displayScale = 1;

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
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Text editor", screenW, screenH, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    displayScale = SDL_GetWindowDisplayScale(window);
    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize SDL_ttf: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    text_font = TTF_OpenFont("/Users/paul/code/C/Text/fonts/mono-regular.ttf",24.0);
    if (!text_font) {
        SDL_Log("Couldn't open font: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    toolbar_font = TTF_OpenFont("/Users/paul/code/C/Text/fonts/mono-bold.ttf", 24.0f);
    if (!toolbar_font) {
        SDL_Log("Couldn't open font: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    t_manager = create_toast_manager();
    tool_bar = create_toolbar();
    //add a 
    char file_path[PATH_MAX];
    for (size_t i = 1; i < argc; i++){
        if (i > 8) break;
        realpath(argv[i], file_path);
        // tab creation creates the text window.
        add_tab(tool_bar, file_path, t_manager);
        load_file(tool_bar->tabs[i-1].window->data);
    }
    tool_bar->tabs[0].focused = true;
    tool_bar->current = &tool_bar->tabs[0];

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
            EditorLine* l = get_line(get_current_window(tool_bar)->data);
            if (l == NULL) break;
            append_line(get_current_window(tool_bar)->data, l, event->text.text);
            break;
        }
        case SDL_EVENT_MOUSE_WHEEL:{
            scroll_text(get_current_window(tool_bar), event->wheel.integer_x, event->wheel.integer_y);
            return SDL_APP_CONTINUE;
            break;
        }
        case SDL_EVENT_KEY_DOWN:{
            EditorLine* l = get_line(get_current_window(tool_bar)->data);
            if (l == NULL) break;
            if (event->key.key == SDLK_RETURN){
                create_new_line(get_current_window(tool_bar)->data);
            }
            if (event->key.key == SDLK_BACKSPACE) {
                line_backspace(get_current_window(tool_bar)->data, l);
            }
            if (event->key.mod & (SDL_KMOD_LCTRL|SDL_KMOD_RCTRL)){
                switch (event->key.key){
                    case SDLK_R:{
                        if (get_current_window(tool_bar)->data->mode == REPLACE) return SDL_APP_CONTINUE;
                        new_toast(t_manager, "Replace mode", TOAST_SUCCESS);
                        get_current_window(tool_bar)->data->mode = REPLACE;
                        break;
                    }
                    case SDLK_I:{
                        if (get_current_window(tool_bar)->data->mode == INSERT) return SDL_APP_CONTINUE;
                        new_toast(t_manager, "Insert mode", TOAST_SUCCESS);
                        get_current_window(tool_bar)->data->mode = INSERT;
                        break;
                    }
                    case SDLK_P:{
                        for (int li = 0; li < get_current_window(tool_bar)->data->line_count;li++){
                            printf("%s\n",get_current_window(tool_bar)->data->lines[li].text);
                        }
                        break;
                    }
                    case SDLK_S:{
                        save_file(get_current_window(tool_bar)->data, t_manager);
                        break;
                    }
                    case SDLK_L:{
                        get_current_window(tool_bar)->display_numbers=!get_current_window(tool_bar)->display_numbers;
                        break;
                    }
                    case SDLK_W:{
                        close_tab(tool_bar, t_manager);
                    }
                }
            }
            
            if (event->key.key == SDLK_UP){
                if (get_current_window(tool_bar)->data->cursor_y > 0) {
                    get_current_window(tool_bar)->data->cursor_y--;
                    int len = get_line(get_current_window(tool_bar)->data)->length;
                    if (get_current_window(tool_bar)->data->cursor_x > len) {
                        get_current_window(tool_bar)->data->cursor_x = len;
                    }
                }
                return SDL_APP_CONTINUE;
            }
            if (event->key.key == SDLK_DOWN){
                if (get_current_window(tool_bar)->data->cursor_y +1 < get_current_window(tool_bar)->data->line_count) {
                    get_current_window(tool_bar)->data->cursor_y++;
                    int len = get_line(get_current_window(tool_bar)->data)->length;
                    if (get_current_window(tool_bar)->data->cursor_x > len) {
                        get_current_window(tool_bar)->data->cursor_x = len;
                    }
                }
                return SDL_APP_CONTINUE;
            }
            if (event->key.key == SDLK_LEFT){
                if (get_current_window(tool_bar)->data->cursor_x == 0) {
                    if (get_current_window(tool_bar)->data->cursor_y > 0) {
                        get_current_window(tool_bar)->data->cursor_y--;
                        get_current_window(tool_bar)->data->cursor_x = get_current_window(tool_bar)->data->lines[get_current_window(tool_bar)->data->cursor_y].length;
                    }
                    return SDL_APP_CONTINUE;
                }
                get_current_window(tool_bar)->data->cursor_x--;
            }
            if (event->key.key == SDLK_RIGHT){
                if (get_current_window(tool_bar)->data->cursor_x==get_current_window(tool_bar)->data->lines[get_current_window(tool_bar)->data->cursor_y].length){
                    if (get_current_window(tool_bar)->data->cursor_y < get_current_window(tool_bar)->data->line_count-1){
                        get_current_window(tool_bar)->data->cursor_x=0;
                        get_current_window(tool_bar)->data->cursor_y++;
                    }
                    return SDL_APP_CONTINUE;
                }
                get_current_window(tool_bar)->data->cursor_x++;
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
    //Update components.
    update_toolbar(tool_bar);
    // update_button(save_button);
    update_toast_manager(t_manager);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    draw_toolbar(renderer, tool_bar);
    draw_window(renderer, get_current_window(tool_bar));
    draw_toast_manager(renderer, t_manager);
    SDL_RenderPresent(renderer);
    lastTime = currentTime;
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (text_font) {
        TTF_CloseFont(text_font);
    }
    if (toolbar_font){
        TTF_CloseFont(toolbar_font);
    }
    TTF_Quit();
}