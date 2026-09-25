#include "toastmanager.h"

ToastManager* create_toast_manager(){
    ToastManager* tm = malloc(sizeof(ToastManager));
    tm->num_toasts = 0;
    return tm;
}

void new_toast(ToastManager* tm, const char* text, int type){
    if (tm->num_toasts == MAX_TOASTS) return;
    UIToast* toast;
    int t_x1 = screenW+TOAST_GAP;
    int t_y1 = screenH-TOAST_HEIGHT-TOAST_GAP;
    int t_x2 = screenW-TOAST_WIDTH-TOAST_GAP;
    int t_y2 = screenH-TOAST_HEIGHT-TOAST_GAP;
    switch (type){
        case TOAST_ERROR:
        toast = create_toast(renderer, font, "Save failed!",t_x1,t_y1,t_x2,t_y2,TOAST_WIDTH,TOAST_HEIGHT,(SDL_Color){40,40,50,SDL_ALPHA_OPAQUE},(SDL_Color){240,50,50,SDL_ALPHA_OPAQUE},(SDL_Color){240,240,255,SDL_ALPHA_OPAQUE});
        break;
        case TOAST_SUCCESS:
        toast = create_toast(renderer, font, "File saved!",t_x1,t_y1,t_x2,t_y2,TOAST_WIDTH,TOAST_HEIGHT,(SDL_Color){40,40,50,SDL_ALPHA_OPAQUE},(SDL_Color){240,240,255,SDL_ALPHA_OPAQUE},(SDL_Color){240,240,255,SDL_ALPHA_OPAQUE});
        break;
    } 
    ToastItem* ti = malloc(sizeof(ToastItem));
    ti->toast = toast;
    
    if (tm->num_toasts > 0) {
        ToastItem* tail = tm->start->prev;
        tm->start->prev = ti;
        ti->next = tm->start;
        ti->prev = tail;
        tail->next = ti;
    }else{
        ti->next = ti;
        ti->prev = ti;
    }
    tm->start = ti;
    tm->num_toasts++;
}

void delete_toast(ToastManager* tm, ToastItem* item){
    if (tm->num_toasts == 0) return;
    if (tm->num_toasts == 1) tm->start = NULL;
    else{
        (item->prev)->next = item->next;
        (item->next)->prev = item->prev;
        if (tm->start == item) {
            tm->start = item->next;
        }
    }
    if (item->toast){
        if (item->toast->text_t){
            SDL_DestroyTexture(item->toast->text_t);   
        }
        free(item->toast);
    }
    free(item);
    tm->num_toasts--;
}

void update_toast_manager(ToastManager* manager){
    ToastItem* current = manager->start;
    for (int i = 0; i < manager->num_toasts; i++){
        if (!current) break;
        ToastItem* next_item = current->next;
        current->toast->x1 = current->toast->toast_r.x;
        current->toast->y1 = current->toast->toast_r.y;
        current->toast->y2 = (screenH-TOAST_HEIGHT-10) - ((current->toast->toast_r.h+10))*i;
        current->toast->t = 0;
        update_toast(current->toast);
        if (current->toast->destroy){
            delete_toast(manager, current);
        }
        current = next_item;
        if (manager->num_toasts == 0) return;
    }
}
void draw_toast_manager(SDL_Renderer* renderer, ToastManager* manager){
    ToastItem* current = manager->start;
    for (int i = 0; i < manager->num_toasts; i++){
        ToastItem* next_item = current->next;
        draw_toast(renderer, current->toast);
        current = next_item;
        if (manager->num_toasts == 0) return;
    }
}