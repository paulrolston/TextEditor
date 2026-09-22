#include "editor.h"

EditorLine create_line(){
    EditorLine l = {0};
    l.capacity = 64;
    l.length = 0;
    l.text = malloc(sizeof(char)*l.capacity);
    l.dirty = false;
    l.texture = NULL;
    return l;
}

EditorData* init_editor(){
    EditorData* d = malloc(sizeof(EditorData));
    d->lines = malloc(sizeof(EditorLine)*INITIAL_LINE_COUNT);
    d->line_capacity = INITIAL_LINE_COUNT;
    d->line_count = 1;
    d->cursor_x = 0;
    d->cursor_y = 0;
    d->lines[0] = create_line();
    d->mode = INSERT;
    return d;
}

EditorLine* get_line(EditorData* data){
    if (data->cursor_y > data->line_count) return NULL;
    return &(data->lines[data->cursor_y]);
}

void delete_line(EditorData* data, EditorLine* line){
    SDL_DestroyTexture(line->texture);
    free(line->text);
    int size = data->line_count-(data->cursor_y+1);
    memmove(line, &data->lines[data->cursor_y+1],sizeof(EditorLine)*size);
}

void line_backspace(EditorData* data, EditorLine* line){
    //line deletion
    if (line->length == 0) {
        if (data->cursor_y > 0 &&data->line_count>1){
            delete_line(data, line);
            data->cursor_y--;
            data->line_count--;
            data->cursor_x=data->lines[data->cursor_y].length;
            return;
        }
    }
    //move line into one above
    if (data->cursor_x == 0) {
        if (data->cursor_y == 0) return;
        EditorLine* above = &data->lines[data->cursor_y-1];
        int return_pos = above->length;
        data->cursor_x =above->length;
        append_line(data,above,line->text);
        delete_line(data,line);
        above->dirty = true;
        data->cursor_y--;
        data->cursor_x=return_pos;
        data->line_count--;
        return;
    }
    int len = line->length-data->cursor_x;
    // delete to the left of the cursor.
    memmove(&line->text[data->cursor_x-1], &line->text[data->cursor_x], len+1);
    line->length--;
    data->cursor_x--;
    line->text[line->length] = 0;
    line->dirty = true;
}

void append_line(EditorData* data, EditorLine* line, const char* text){
    int len = strlen(text);
    int to_end = line->length-data->cursor_x;
    bool do_realloc = false;
    while (line->length+len >= line->capacity){
        // use 1.2 since most of the time this will be a single character overflow.
        line->capacity*=1.2;
        do_realloc = true;
    }
    if (do_realloc){
        printf("Realloc for this line.\n");
        line->text = realloc(line->text, sizeof(char)*line->capacity);
    }
    switch (data->mode){
        case REPLACE:{
            memcpy(&line->text[data->cursor_x], text, len);
            break;
        }
        case INSERT:{
            if (to_end > 0) memmove(&line->text[data->cursor_x+len],&line->text[data->cursor_x],to_end);
            memcpy(&line->text[data->cursor_x], text, len);
            break;
        }
    }
    data->cursor_x+=len;
    if (data->mode == INSERT) line->length+=len;
    else if(data->mode == REPLACE && data->cursor_x>line->length) line->length=data->cursor_x;
    line->text[line->length] = 0;
    line->dirty = true;
}

void load_file(EditorData* data){
    FILE* file = fopen(data->file_path, "r");
    if (file != NULL){
        char line_buf[1024];
        while (fgets(line_buf, 1024, file)){
            line_buf[strcspn(line_buf,"\r\n")] = 0;
            append_line(data,get_line(data),line_buf);
            create_new_line(data);
        }
    }
    fclose(file);
}

void save_file(EditorData* data){
    FILE* file = fopen(data->file_path,"w");
    if (file == NULL) {
        printf("Error opening file: [%s]\n", data->file_path);
        return;
    }
    for (int li = 0; li < data->line_count;li++){
        EditorLine* line = &data->lines[li];
        if (line->text != NULL) fprintf(file, "%s\n",line->text);
    }
    fclose(file);
}

void create_new_line(EditorData* data){
    if (data->line_count+1 >= data->line_capacity) {
        data->line_capacity*=2;
        data->lines = realloc(data->lines, sizeof(EditorLine)*data->line_capacity);
    }
    data->cursor_y++;
    data->cursor_x = 0;
    *get_line(data) = create_line();
    data->line_count++;
}
