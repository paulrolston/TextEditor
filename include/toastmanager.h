#ifndef TOASTMANAGER_H
#define TOASTMANAGER_H

#define MAX_TOASTS 8

#include "globals.h"
#include "toast.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct ToastItem {
    UIToast* toast;
    struct ToastItem* next;
    struct ToastItem* prev;
} ToastItem;

typedef struct ToastManager {
    ToastItem* start;
    size_t num_toasts;
} ToastManager;

ToastManager* create_toast_manager();
//update all of the toasts
void update_toast_manager(ToastManager* manager);
//draw the toasts
void draw_toast_manager(SDL_Renderer* renderer, ToastManager* manager);
void new_toast(ToastManager* tm, const char* text, int type);

#endif // TOASTMANAGER.H_H