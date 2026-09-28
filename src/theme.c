#include "theme.h"

const UITheme default_dark = (UITheme){
    .text_window_background = (SDL_Color){35,35,42,SDL_ALPHA_OPAQUE},
    .text_window_foreground = (SDL_Color){240,240,240,SDL_ALPHA_OPAQUE},
    .text_cursor            = (SDL_Color){240,240,240,SDL_ALPHA_OPAQUE},
    .line_number_foreground = (SDL_Color){240,240,240,SDL_ALPHA_OPAQUE},
    .line_number_separator  = (SDL_Color){240,240,240,SDL_ALPHA_OPAQUE},
    .toolbar_background     = (SDL_Color){20,20,27,SDL_ALPHA_OPAQUE},
    .toolbar_tab_focus      = (SDL_Color){35,35,42,SDL_ALPHA_OPAQUE},
    .toolbar_tab_unfocus    = (SDL_Color){27,27,32,SDL_ALPHA_OPAQUE},
    .toolbar_tab_foreground = (SDL_Color){240,240,240,SDL_ALPHA_OPAQUE},
    .toolbar_separator      = (SDL_Color){60,60,68,SDL_ALPHA_OPAQUE},
    .toast_background       = (SDL_Color){35,35,42,SDL_ALPHA_OPAQUE},
    .toast_foreground       = (SDL_Color){240,240,240,SDL_ALPHA_OPAQUE},
    .toast_success_border   = (SDL_Color){50,240,50,SDL_ALPHA_OPAQUE},
    .toast_error_border     = (SDL_Color){240,50,50,SDL_ALPHA_OPAQUE},
    .unsaved_indicator      = (SDL_Color){100,100,200,SDL_ALPHA_OPAQUE},
};

const UITheme default_light = (UITheme){
    .text_window_background = (SDL_Color){225,225,225,SDL_ALPHA_OPAQUE},//
    .text_window_foreground = (SDL_Color){10,10,10,SDL_ALPHA_OPAQUE},//
    .text_cursor            = (SDL_Color){10,10,10,SDL_ALPHA_OPAQUE},//
    .line_number_foreground = (SDL_Color){10,10,10,SDL_ALPHA_OPAQUE},//
    .line_number_separator  = (SDL_Color){10,10,10,SDL_ALPHA_OPAQUE},//
    .toolbar_background     = (SDL_Color){170,170,170,SDL_ALPHA_OPAQUE},//
    .toolbar_tab_focus      = (SDL_Color){225,225,225,SDL_ALPHA_OPAQUE},//
    .toolbar_tab_unfocus    = (SDL_Color){190,190,190,SDL_ALPHA_OPAQUE},//
    .toolbar_tab_foreground = (SDL_Color){10,10,10,SDL_ALPHA_OPAQUE},//
    .toolbar_separator      = (SDL_Color){240,240,240,SDL_ALPHA_OPAQUE},//
    .toast_background       = (SDL_Color){225,225,225,SDL_ALPHA_OPAQUE},//
    .toast_foreground       = (SDL_Color){10,10,10,SDL_ALPHA_OPAQUE},//
    .toast_success_border   = (SDL_Color){50,240,50,SDL_ALPHA_OPAQUE},//
    .toast_error_border     = (SDL_Color){240,50,50,SDL_ALPHA_OPAQUE},//
    .unsaved_indicator      = (SDL_Color){100,100,200,SDL_ALPHA_OPAQUE},//
};