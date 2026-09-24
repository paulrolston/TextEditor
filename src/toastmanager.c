#include "toastmanager.h"

ToastManager* create_toast_manager(){
    ToastManager* tm = malloc(sizeof(ToastManager));
    tm->num_toasts = 0;
    tm->toasts = malloc(sizeof(UIToast*)*MAX_TOASTS);
    return tm;
}

void new_toast(ToastManager* tm, UIToast* toast){
    if (tm->num_toasts == MAX_TOASTS) return;
    tm->toasts[tm->num_toasts++] = toast;
}

void update_toast_manager(ToastManager* manager){
    for (size_t i = 0; i < manager->num_toasts; i++){
        update_toast(manager->toasts[i]);
    }
}
void draw_toast_manager(SDL_Renderer* renderer, ToastManager* manager){
    for (size_t i = 0; i < manager->num_toasts; i++){
        draw_toast(renderer, manager->toasts[i]);
    }
}