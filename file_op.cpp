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
#include <fcntl.h>
#include <string.h> 
#include "editor_entity.hpp"
#include "debug_logger.hpp"
#include "process_key.hpp"
#include "config.hpp"

char *editorRowsToString(int *buflen){
    int totlen=0;
    int j;
    for(j=0;j<E.numrows;j++)totlen += E.row[j].size +1;
    *buflen = totlen;
    char *buf =(char*) malloc(totlen);
    char *p = buf;
    for(j=0;j<E.numrows;j++){
        memcpy(p,E.row[j].chars,E.row[j].size);
        p+=E.row[j].size;
        *p = '\n';
        p++;
    }
    return buf;    
}


void editorSave(){
    if(E.filename == NULL )return;
    int len;
    char *buf = editorRowsToString(&len);
    int fd = open(E.filename,O_RDWR | O_CREAT,0644);
    ftruncate(fd,len);
    write(fd,buf,len);
    close(fd);
    free(buf);
}