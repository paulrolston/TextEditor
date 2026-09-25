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
            data->content_hash = hash_contents(data);
            data->unsaved = (data->content_hash != data->original_hash);
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
    data->content_hash = hash_contents(data);
    data->unsaved = (data->content_hash != data->original_hash);
}

void append_line(EditorData* data, EditorLine* line, const char* text){
    int len = strlen(text);
    int to_end = line->length-data->cursor_x;
    bool do_realloc = false;
    while (line->length+len >= line->capacity){
        line->capacity*=2.0;
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
    data->content_hash = hash_contents(data);
    data->unsaved = (data->content_hash != data->original_hash);
}

XXH64_hash_t hash_contents(EditorData* data){
    XXH3_state_t* state = XXH3_createState();
    XXH3_64bits_reset(state);
    for (ssize_t i = 0; i < data->line_count; i++){
        // add line text
        XXH3_64bits_update(state, data->lines[i].text, data->lines[i].length);
        //add new line character (if not last line)
        if (i != data->line_count-1) XXH3_64bits_update(state, &(char){'\n'}, 1);
    }
    XXH64_hash_t r = XXH3_64bits_digest(state);
    XXH3_freeState(state);
    return r;
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
    data->original_hash = hash_contents(data);
    data->content_hash = data->original_hash;
    data->unsaved = false;
}

void save_file(EditorData* data, ToastManager* t_manager){
    // if there are no unsaved changes, just return;
    if (!(data->unsaved)){
        new_toast(t_manager, "File saved!", TOAST_SUCCESS);
        return;
    }else{
        data->original_hash = data->content_hash;
        data->unsaved = false;
    }
    FILE* file = fopen(data->file_path,"w");
    if (file == NULL) {
        printf("Error opening file: [%s]\n", data->file_path);
        new_toast(t_manager, "Error saving!", TOAST_ERROR);
        return;
    }
    for (int li = 0; li < data->line_count;li++){
        EditorLine* line = &data->lines[li];
        if (line->text != NULL) {
            //omit new line on last line (stop saving from adding a line to the file.)
            (li == data->line_count-1) ? fprintf(file, "%s",line->text) : (fprintf(file, "%s\n",line->text));
        }
    }
    new_toast(t_manager, "File saved!", TOAST_SUCCESS);
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
    data->content_hash = hash_contents(data);
    data->unsaved = (data->content_hash != data->original_hash);
}
