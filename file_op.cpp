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
#include "window_op.hpp"
#include "status.hpp"

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
    if(fd != -1){
        if(ftruncate(fd,len) != -1){
            if(write(fd,buf,len)==len){
                close(fd);
                free(buf);
                E.dirty = 0;
                editorSetStatusMessage("%d bytes written to disk",len);
                return;
            }
        }
        close(fd);
    }
    free(buf);
    editorSetStatusMessage("Can`t save file! I/O error: %s",strerror(errno));
}

void editorOpen(char *filename){
    free(E.filename);
    E.filename = strdup(filename);
    FILE *fp = fopen(E.filename,"r");
    if (!fp) die("fopen");

    char * line = NULL;
    size_t linecap =0 ;
    ssize_t linelen;
    
    while ((linelen =getline(&line,&linecap,fp)) != -1){
        while(linelen > 0 && (line[linelen-1] == '\n' || line[linelen-1] == '\r')) linelen--;
        editorAppendRow(line,linelen);
    }
    free(line);
    fclose(fp);
    E.dirty = 0;
}
