#ifndef EPEX_INIT_H
#define EPEX_INIT_H

#include <termios.h> // used for the "epex struct"

// Defines
#define APPEND_BUFFER_INIT {NULL, 0}
#define EPEX_VERSION "1.0"

// Structs
struct epex
{
    int cursor_x, cursor_y;
    int rows;
    int cols;
    struct termios original;
} Epex;

struct append_buffer
{
    char *b;
    int len;
};


// Functions
void disable_raw_mode();

void enable_raw_mode();

int windows_size(int *rows, int *cols);

void draw_rows(struct append_buffer *ab);

void refresh_terminal();

void append_buffer_append(struct append_buffer *ab, const char *s, int len);
void append_buffer_free(struct append_buffer *ab);

void init();

#endif //EPEX_INIT_H
