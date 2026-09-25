#include "toastmanager.h"

ToastManager* create_toast_manager(){
    ToastManager* tm = malloc(sizeof(ToastManager));
    tm->num_toasts = 0;
    return tm;
}

//TODO: replace with taking in text, and a toast_type then call create_toast here
// this will allow the toast_manager to decide where to place toasts (always bottom right for example)
void new_toast(ToastManager* tm, UIToast* toast){
    if (tm->num_toasts == MAX_TOASTS) return;
    ToastItem* ti = malloc(sizeof(ToastItem));
    ti->toast = toast;
    
    if (tm->num_toasts > 0) {
        ToastItem* curr = tm->start;
        for (int i = 0; i < tm->num_toasts; i++){
            ToastItem* next_item = curr->next;
            curr->toast->x1 = curr->toast->toast_r.x;
            curr->toast->y1 = curr->toast->toast_r.y;
            curr->toast->y2 -= 40;
            curr->toast->t = 0;
            curr = next_item;
            if (tm->num_toasts == 0) return;
        }
        ToastItem* tail = tm->start->prev;
        //point old head's back pointer to ti
        tm->start->prev = ti;
        // point ti->next to start and ti->prev to tail
        ti->next = tm->start;
        ti->prev = tail;
        // point tail->next to ti
        tail->next = ti;
    }else{
        ti->next = ti;
        ti->prev = ti;
    }
    //reassign start to ti
    tm->start = ti;
    tm->num_toasts++;
}

//Todo: deleting from middle of list crashes program
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