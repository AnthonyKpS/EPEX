//
// Created by kps on 16/11/20.
//
// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "init.h"

// Functions
void die(const char* s);
void disable_raw_mode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &Epex.original) == -1)
        die("tcsetattr");
}

void enable_raw_mode()
{
    // Getting terminal's attributes struct
    if (tcgetattr(STDIN_FILENO, &Epex.original) == -1)
        die("tcgetattr");
    atexit(disable_raw_mode);

    // Making a copy and modifying it to stop terminal ECHOing
    struct termios raw_mode = Epex.original;
    raw_mode.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); // Ctrl-M fix (CarrReturn should not return to newline) and misc flag
    raw_mode.c_oflag &= ~(OPOST);								   // Output Post Process fix
    raw_mode.c_cflag |= (CS8);
    raw_mode.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // ECHOing, Canonical mode, Ctrl-V fix, Ctrl-C/Z
    raw_mode.c_cc[VMIN] = 0;
    raw_mode.c_cc[VTIME] = 1; // Timeout for read

    // Updating terminal's attributes
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_mode) == -1)
        die("tcsetattr");
}

int windows_size(int *rows, int *cols)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        return -1;
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

// Output manipulation

void draw_rows(struct append_buffer *ab)
{
    // draw numbered rows
    char num[3];
    for (int i = 1; i < Epex.rows; i++)
    {
        snprintf(num, 3, "%d", i);
        if (i == Epex.rows / 3)
        {
            // Draw a welcome message
            char welcome[80];
            int welcome_len = snprintf(welcome, sizeof(welcome),"EPEX text editor -- version %s", EPEX_VERSION);

            // Truncate message in case the terminal width is smaller than the message length
            if (welcome_len > Epex.cols)
                welcome_len = Epex.cols;

            // Center the message
            int padding = (Epex.cols - welcome_len) / 2;
            if (padding)
            {
                append_buffer_append(ab, num,  3);
            }
            while (padding--)
                append_buffer_append(ab, " ", 1);
            append_buffer_append(ab, welcome, welcome_len);
        } else {
            append_buffer_append(ab, num,  3);
        }
        append_buffer_append(ab, "\x1b[K", 3);

        // For all the lines except the last one, print the \r\n as well
        if (i < Epex.rows)
            append_buffer_append(ab, "\r\n", 2);
    }
}

void refresh_terminal()
{
    struct append_buffer ab = APPEND_BUFFER_INIT;

    append_buffer_append(&ab, "\x1b[?25l", 6);
    append_buffer_append(&ab, "\x1b[H", 3);

    draw_rows(&ab);

    // Update cursor position
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", Epex.cursor_y + 1, Epex.cursor_x + 1);
    append_buffer_append(&ab, buf, strlen(buf));

    append_buffer_append(&ab, "\x1b[?25h", 6);

    write(STDOUT_FILENO, ab.b, ab.len);
    append_buffer_free(&ab);
}

void append_buffer_append(struct append_buffer *ab, const char *s, int len)
{
    char *new = realloc(ab->b, ab->len+len);

    if(new == NULL) return;
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void append_buffer_free(struct append_buffer *ab)
{
    free(ab->b);
}

void init()
{
    // Initialize cursor position
    Epex.cursor_x = Epex.cursor_y = 0;

    enable_raw_mode();
    if (windows_size(&Epex.rows, &Epex.cols) == -1)
        die("windows_size");
}

