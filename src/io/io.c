//
// Created by kps on 16/11/20.
//
// Includes
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "io.h"
#include "../init/init.h"

// Defines
#define CTRL_KEY(k) ((k)&0x1f) // Macro imitating the CTRL+key

// Functions
void move_cursor(int key)
{
    switch (key)
    {
        case ARROW_LEFT:
            if (Epex.cursor_x != 0)
                Epex.cursor_x--;
            break;
        case ARROW_RIGHT:
            if (Epex.cursor_x != Epex.cols - 1)
                Epex.cursor_x++;
            break;
        case ARROW_UP:
            if (Epex.cursor_y != 0)
                Epex.cursor_y--;
            break;
        case ARROW_DOWN:
            if (Epex.cursor_y != Epex.rows - 1)
                Epex.cursor_y++;
            break;
        default:
            break;
    }
}

int read_keypress()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }

    // Read ESC sequences
    if (c == '\x1b')
    {
        char seq[3];

        // Read 2 more chars
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[')
        {
            // Catch PAGE_UP, PAGE_DOWN
            if (seq[1] >= '0' && seq[1] <= '9')
            {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~')
                {
                    switch (seq[1])
                    {
                        case '1':
                            return HOME;
                        case 3:
                            return DELETE;
                        case '4':
                            return END;
                        case '5':
                            return PAGE_UP;
                        case '6':
                            return PAGE_DOWN;
                        case '7':
                            return HOME;
                        case '8':
                            return END;
                    }
                }
            } else
            {
                // Catch ARROWS
                switch (seq[1])
                {
                    case 'A':
                        return ARROW_UP;
                    case 'B':
                        return ARROW_DOWN;
                    case 'C':
                        return ARROW_RIGHT;
                    case 'D':
                        return ARROW_LEFT;
                    case 'H':
                        return HOME;
                    case 'F':
                        return END;
                }
            }
        } else if (seq[0] == 'O')
        {
            switch (seq[1])
            {
                case 'H':
                    return HOME;
                case 'F':
                    return END;
            }
        }
        return '\x1b';
    } else
        return c;
}

void process_keypress()
{
    int c = read_keypress();

    switch (c)
    {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(EXIT_SUCCESS);

        case HOME:
            Epex.cursor_x = 0;
            break;
        case END:
            Epex.cursor_x = Epex.cols - 1;
            break;
        case PAGE_UP:
        case PAGE_DOWN:
        {
            int times = Epex.rows;
            while (times--)
                move_cursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
        }
        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
            move_cursor(c);
            break;
        default:
            break;
    }
}

