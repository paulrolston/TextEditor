#include "toolbar.h"

Toolbar* create_toolbar(){
    Toolbar* tb = malloc(sizeof(Toolbar));
    tb->toolbar_r = (SDL_FRect) {
        .x=0,
        .y=0,
        .h=30,
        .w=0,
    };
    tb->background = (SDL_Color){.r=20,.g=20,.b=27,.a=255};
    tb->foreground = (SDL_Color){.r=240,.g=240,.b=240,.a=255};
    tb->tab_count = 0;
    tb->current = NULL;
    return tb;
}

typedef struct TabCallbackData {
    Toolbar* tb;
    int id;
} TabCallbackData;

void tab_callback(Button* b, void* raw) {
    TabCallbackData* data = (TabCallbackData*) raw;
    if (data == NULL) {
        printf("Tabs: data passed was NULL\n");
        return;
    }
    for (size_t i = 0; i < data->tb->tab_count; i++){
        data->tb->tabs[i].focused = (i == data->id);
        if (i == data->id){
            data->tb->current = &data->tb->tabs[i];
        }
    }
}

void add_tab(Toolbar* tb, const char *file_path ,ToastManager* tm){
    if (tb->tab_count == MAX_TABS) {
        new_toast(tm, "Max files.", TOAST_ERROR);
        return;
    }
    int x = 100*tb->tab_count;
    int y = 0;
    int w = 100;
    int h = 30;
    char* file_name = strrchr(file_path, '/') + 1;
    TabCallbackData* d = malloc(sizeof(TabCallbackData));
    d->tb = tb;
    d->id = tb->tab_count;
    TextWindow* window = create_window(0,30,screenW,screenH-30);
    tb->tabs[tb->tab_count++] = (ToolbarTab){
        .rect = (SDL_FRect) {
            .x = x*displayScale, 
            .y = y*displayScale, 
            .w = w*displayScale, 
            .h = h*displayScale
        },
        .focused = false,
        .button = create_button(renderer, toolbar_font, file_name, LEFT ,x,y,w,h,
            (SDL_Color){0,0,0,SDL_ALPHA_TRANSPARENT},(SDL_Color){0,0,0,SDL_ALPHA_TRANSPARENT},(SDL_Color){240,240,240,SDL_ALPHA_OPAQUE},
            NULL, tab_callback, NULL, (void *) d),
        .window = window,
        .id = tb->tab_count-1,
        };
    strcpy(tb->tabs[tb->tab_count-1].file_path, file_path);
    strcpy(tb->tabs[tb->tab_count-1].file_name, file_name);
    strcpy(window->data->file_path, file_path);
}

void close_tab(Toolbar* tb, ToastManager* tm){
    ToolbarTab* tab = tb->current;
    if (tab->window->data->unsaved){
        new_toast(tm, "Save changes!", TOAST_ERROR);
        return;
    }
    if (tb->tab_count == 1) {
        new_toast(tm, "Last file!", TOAST_ERROR);
        return;
    }
    destroy_window(tab->window);
    destroy_button(tab->button);
    int to_end = (tb->tab_count-1)-tab->id;
    if (to_end > 0){
        for (ssize_t i = tab->id; i<tb->tab_count;i++){
            tb->tabs[i].id--;
        }
        memmove(tb->current, tb->current+1, to_end*sizeof(ToolbarTab));
    }
    int next = (tab->id == tb->tab_count-1) ? 0 : tab->id;
    tb->current = &tb->tabs[next];
    tb->current->focused = true;
    tb->tab_count--;
}

void update_toolbar(Toolbar* tb){
    for (size_t i = 0; i < tb->tab_count; i++){
        update_button(tb->tabs[i].button);
    }
}


void draw_toolbar(SDL_Renderer* renderer, Toolbar* tb){
    float scale = SDL_GetWindowDisplayScale(SDL_GetRenderWindow(renderer));
    int w, h;
    SDL_GetWindowSize(SDL_GetRenderWindow(renderer), &w, &h);
    tb->toolbar_r.w = w;
    SDL_FRect scaled = {
        .x = tb->toolbar_r.x*scale,
        .y = tb->toolbar_r.y*scale,
        .w = tb->toolbar_r.w*scale,
        .h = tb->toolbar_r.h*scale,
    };
    SDL_SetRenderDrawColor(renderer, tb->background.r,tb->background.g, tb->background.b,tb->background.a);
    SDL_RenderFillRect(renderer, &scaled);
    // SDL_RenderRect(renderer, &scaled);
    for (size_t i = 0; i < tb->tab_count; i++){
        //button just displays text
        ToolbarTab* tab = &tb->tabs[i];
        tab->rect.x = (100*i)*displayScale;
        tab->button->button_r.x = (100*i);
        SDL_FRect tab_r = tab->rect;
        // unfocussed tabs are smaller and darker
        if (tab->focused)
        {
            SDL_SetRenderDrawColor(renderer, 35,35,42, SDL_ALPHA_OPAQUE);
        }else{
            int change = tab->rect.h*0.8;
            tab_r.h = change;
            tab_r.y += tab->rect.h-change;
            //recentre the text
            tab->button->button_r.h = change/displayScale;
            tab->button->button_r.y = (tab->rect.h-change)/displayScale;
            SDL_SetRenderDrawColor(renderer, 27,27,32, SDL_ALPHA_OPAQUE);
        }
        SDL_RenderFillRect(renderer, &tab_r);
        draw_button(renderer,tab->button);
        if (tab->window->data->unsaved){
            int unsaved_w = 6*displayScale;
            int unsaved_h = 6*displayScale;
            int spacing = (tab_r.h-unsaved_w)*0.5;
            SDL_FRect unsaved_r = {
                .x = tab_r.x + tab_r.w - unsaved_w - spacing,
                .y = tab_r.y + spacing,
                .w = unsaved_w,
                .h = unsaved_h,
            };
            SDL_SetRenderDrawColor(renderer, 100,100,200, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &unsaved_r);
        }

        //seperating line from tab to text window
        SDL_SetRenderDrawColor(renderer, 60,60,68,SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer,&(SDL_FRect){scaled.x,scaled.y+scaled.h-(1*displayScale),scaled.w,1*displayScale});
    }
}

TextWindow* get_current_window(Toolbar* tb){
    return tb->current->window;
}