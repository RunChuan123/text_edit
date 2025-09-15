#pragma once


// 存储一行文本
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


enum editorKey{
    BACKSPACE = 127,
    ARROW_LEFT =1000,
    ARROW_DOWN ,
    ARROW_RIGHT ,
    ARROW_UP  ,
    PAGE_UP,
    PAGE_DOWN,
    HOME_KEY,
    END_KEY,
    DEL_KEY
};

struct erow {
    int size;
    int rsize;
    char *chars;
    // 包含实际在屏幕上绘制的字符
    char *render;
};

struct editorConfig{
    int rx;
    int cx,cy;
    int rowoff,coloff;
    int screenrows;
    int screencols;
    struct termios orig_termios;
    int numrows;
    erow *row;
    char* filename;
    char statusmsg[80];
    time_t statusmsg_time;
    int dirty;
};
extern editorConfig E;

struct abuf{
    char *b;
    int len;
};
