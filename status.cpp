
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


void editorDrawStatusBar(abuf *ab){
    abAppend(ab,"\x1b[7m",4);
    char status[80],rstatus[80];



    int len=snprintf(status,sizeof(status),"%.20s - %d lines %s",
    E.filename?E.filename:"[Not open any file]",
    E.numrows,E.dirty?"(Modified)":"");
    int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d",E.cy + 1, E.numrows);

    if(len > E.screencols) len = E.screencols;
    abAppend(ab,status,len);
    while(len<E.screencols){
        if (E.screencols -len == rlen){
            abAppend(ab,rstatus,rlen);
            break;
        }else{
            abAppend(ab, " ",1);
            len++;
        }
    }
    abAppend(ab,"\x1b[m",3);
    abAppend(ab,"\r\n",2);
}

// 第二行状态信息
void editorSetStatusMessage(const char* fmt,...){
    va_list ap;
    va_start(ap,fmt);
    vsnprintf(E.statusmsg,sizeof(E.statusmsg),fmt,ap);
    va_end(ap);
    E.statusmsg_time = time(NULL);
}

void editorDrawMessagBar(abuf *ab){
    abAppend(ab,"\x1b[K",3);
    int msglen = strlen(E.statusmsg);
    if (msglen > E.screencols) msglen=E.screencols;
    if(msglen && time(NULL) - E.statusmsg_time < 5)
        abAppend(ab,E.statusmsg,msglen);
}