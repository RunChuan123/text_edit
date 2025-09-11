
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h> 
#include "editor_entity.hpp"
#include "debug_logger.hpp"
#include "basic_func.hpp"
#include "config.hpp"


int editorReadKey(){
    DEBUG_LOG("in editorReadKey");
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO,&c,1)) != 1){
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    DEBUG_LOG("keycode:", c);
    // nread = read(STDIN_FILENO,&c,1);
    // DEBUG_LOG("editorReadKey", nread);
    // if (nread == 1) return c;
    // return -1;  // 没输入
    // DEBUG_LOG("editorReadKey return -1");
    if (c == '\x1b') {
        char seq[3];
        if (read(STDIN_FILENO,&seq[0],1) != 1)return '\x1b';
        if (read(STDIN_FILENO,&seq[1],1) != 1)return '\x1b';

        if (seq[0] == '['){
            DEBUG_LOG(seq);
            if (seq[1] >= '0' && seq[1] <= '9'){
                DEBUG_LOG("step in cntrl");
                if (read(STDIN_FILENO,&seq[2],1) != 1) return '\x1b';
                if (seq[2] == '~'){
                    
                    switch (seq[1])
                    {
                    
                    case '5':
                        DEBUG_LOG("tap page_up");
                        return PAGE_UP;
                    case '6':
                        DEBUG_LOG("tap page_down");
                        return PAGE_DOWN;
                    }
                }
            } else {
                DEBUG_LOG("step in arrow");
            switch (seq[1])
            {
            case 'A': return ARROW_UP;
            case 'B': return ARROW_DOWN;
            case 'C': return ARROW_RIGHT;
            case 'D': return ARROW_LEFT;
            default:
                break;
            }
        }
    }
        return '\x1b';
    } else {
        return c;
    }
}


void editorProcessKeypress(){
    // DEBUG_LOG("enter editorProcessKeypress");
    int c = editorReadKey();
    // if (c == -1) return;
    // DEBUG_LOG("this");
    switch (c)
    {
    case CTRL_KEY('q'): 
        editorRefreshScreen();
        exit(0);
        break;
    case PAGE_UP:
    case PAGE_DOWN:
        {
            int times = E.screenrows;
            while (times--)
                editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
        }
        break;
    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_RIGHT:
    case ARROW_LEFT:
        editorMoveCursor(c);
        break;
    
    default:
        break;
    }
}