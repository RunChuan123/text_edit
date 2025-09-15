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
#include "window_op.hpp"
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
    E.dirty ++;
}



void editorRowDelChar(erow *row,int at){
    if (at <0|| at >= row->size) return;
    memmove(&row->chars[at],&row->chars[at+1],row->size - at);
    row->size --;
    editorUpdateRow(row);
    E.dirty++;
}


void editorFreeRow(erow *row){
    free(row->render);
    free(row->chars);
}

void editorDelRow(int at){
    if(at<0 || at >=E.numrows){
        editorFreeRow(&E.row[at]);
        memmove(&E.row[at],&E.row[at+1],sizeof(erow)*E.numrows-at-1);
        E.numrows--;
        E.dirty++;
    }
}

void editorRowAppendString(erow *row,char *s,size_t len){
    row->chars = (char*)realloc(row->chars,row->size+len+1);
    memcpy(&row->chars[row->size],s,len);
    row->size+=len;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
    E.dirty++;
}



void editorDelChar(){
    if(E.cy == E.numrows){
        return;
    }
    if (E.cx == 0&& E.cy ==0){
        return;
    }
    erow *row = &E.row[E.cy];
    if(E.cx>0){
        editorRowDelChar(row,E.cx-1);
        E.cx--;
    }else{
        E.cx = E.row[E.cy -1].size;
        editorRowAppendString(&E.row[E.cy-1],row->chars,row->size);
        editorDelRow(E.cy);
        E.cy--;
    }
}
