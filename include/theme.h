#ifndef THEME_H
#define THEME_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

typedef struct UITheme {
    SDL_Color text_window_background;
    SDL_Color text_window_foreground;
    SDL_Color line_number_foreground;
    SDL_Color line_number_separator;
    SDL_Color toolbar_background;
    SDL_Color toolbar_tab_focus;
    SDL_Color toolbar_tab_unfocus;
    SDL_Color toolbar_tab_foreground;
    SDL_Color toolbar_separator;
    SDL_Color toast_background;
    SDL_Color toast_foreground;
    SDL_Color toast_success_border;
    SDL_Color toast_error_border;
    SDL_Color unsaved_indicator;
} UITheme;

extern const UITheme default_dark;
extern const UITheme default_light;

extern UITheme* current_theme;

#endif // THEME.H_H