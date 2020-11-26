/**
 *
 * epex is an open-source text editor (GNU "nano" clone) made with the help of antirez's kilo text editor.
 * This tutorial was a help aswell https://viewsourcecode.org/snaptoken/kilo
 *
 * The epex name comes from the first 4 letters of the greek word "επεξ-εργαστής" which stands
 * for editor.
 *
*/

// System Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



// Global functions
void die(const char* s)
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(s);
    exit(EXIT_FAILURE);
}

// Project includes
#include "src/init/init.h"
#include "src/io/io.h"

int main()
{
    init();

    while (1)
    {
        refresh_terminal();
        process_keypress();
    };

    return(EXIT_SUCCESS);

}
