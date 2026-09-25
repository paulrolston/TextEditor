#ifndef EDITOR_H
#define EDITOR_H

#define INITIAL_LINE_COUNT 4
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "toastmanager.h"

typedef enum EditMode {
    REPLACE, INSERT
} EditMode;

typedef struct EditorLine{
    char* text;
    int capacity;
    int length;
    bool dirty;
    SDL_Texture* texture;
} EditorLine;

typedef struct EditorData{
    char file_path[PATH_MAX];
    char file_name[50];
    XXH64_hash_t original_hash;
    EditorLine* lines;
    int line_count;
    int line_capacity;
    int cursor_y;
    int cursor_x;
    EditMode mode;
} EditorData;

EditorData* init_editor();
EditorLine* get_line(EditorData* data);
void line_backspace(EditorData* data, EditorLine* line);
void append_line(EditorData* data, EditorLine* line, const char* text);
void create_new_line(EditorData* data);
void load_file(EditorData* data);
void save_file(EditorData* data, ToastManager* t_manager);
XXH64_hash_t hash_contents(EditorData* data);

#endif
