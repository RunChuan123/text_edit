
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
#include "window_op.hpp"
#include "config.hpp"
#include "text_editor.hpp"
#include "file_op.hpp"
#include "status.hpp"
int editorReadKey(){
    DEBUG_LOG("in editorReadKey");
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO,&c,1)) != 1){
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    DEBUG_LOG("keycode:", c);
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
                    case '3':
                        DEBUG_LOG("delkey");
                        return DEL_KEY;
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
    static int quit_times = QUIT_TIMES;
    // DEBUG_LOG("enter editorProcessKeypress");
    int c = editorReadKey();
    // if (c == -1) return;
    // DEBUG_LOG("this");
    switch (c)
    {
    case '\r':
        break;
    case CTRL_KEY('q'): 
        if(E.dirty && quit_times > 0){
            editorSetStatusMessage("File has unsaved changes. "
            "Press Ctrl-Q %d more times to quit.", quit_times);
            quit_times--;
            return;
        }
        
        // editorRefreshScreen();
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
        break;
    case CTRL_KEY('s'):
        editorSave();
        break;
    case BACKSPACE:
    case CTRL_KEY('h'):
    case DEL_KEY:
        editorMoveCursor(ARROW_RIGHT);
        editorDelChar();
        break;
    
        // if(E.cx != 0){
        //     E.cx --;
        //     memmove(&E.row[E.cy].chars[E.cx],&E.row[E.cy].chars[E.cx+1],E.row[E.cy].size-E.cx);
        //     E.row[E.cy].size--;
        //     editorUpdateRow(&E.row[E.cy]);
        // } else {
        //     // 需要改，暂时先这样
        //     if(E.cy>0){
        //         // editorDelRow();
                
        //     }
        // }
        
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
    case CTRL_KEY('l'):
    case '\x1b':

        break;
    default:
        editorInsertChar(c);
        break;
    }
    quit_times = QUIT_TIMES;
}
