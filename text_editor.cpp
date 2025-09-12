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
#include "basic_func.hpp"
#include "debug_logger.hpp"





void editorRowInsertChar(erow *row,int at, int c){
    if(at < 0 || at > row->size) at = row->size;
    row->chars = (char *)realloc(row->chars,row->size +2);
    memmove(&row->chars[at+1],&row->chars[at],row->size-at+1);
    row->size++;
    row->chars[at] = c;
    editorUpdateRow(row);
}
void editorInsertChar(int c){
    if(E.cy == E.numrows){
        editorAppendRow(strdup(""),0);
    }
    editorRowInsertChar(&E.row[E.cy],E.cx,c);
    E.cx++;
}