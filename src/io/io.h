//
// Created by kps on 16/11/20.
//

#ifndef EPEX_IO_H
#define EPEX_IO_H

// Keys
enum epex_keys
{
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DELETE,
    HOME,
    END,
    PAGE_UP,
    PAGE_DOWN
};

void die(const char* s);

void move_cursor(int key);
int read_keypress();
void process_keypress();


#endif //EPEX_IO_H
