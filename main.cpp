// #include <stdio.h>
// #include <unistd.h>
// #include <termios.h>
// #include <stdlib.h>
// #include <ctype.h>
// #include <sys/ioctl.h>
// #include <time.h>
// #include <stdarg.h>
// #include <sys/types.h>
// #include <errno.h>
// #include <string.h> 
#include "editor_entity.hpp"
#include "basic_func.hpp"
#include "debug_logger.hpp"


editorConfig E;

int main(int argc,char *argv[]){
    enableRawMode();
    initEditor();
    if (argc >= 2){
        editorOpen(argv[1]);
    }
    editorSetStatusMessage("HELP: Control-Q = quit");
    DEBUG_LOG("main");
    while (1){
        editorRefreshScreen();
        DEBUG_LOG("end editorRefreshScreen");
        editorProcessKeypress();
        DEBUG_LOG("end editorProcessKeypress");
    };
    free(E.filename);
    return 0;
}