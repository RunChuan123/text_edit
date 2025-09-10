#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h> 
#include "debug_logger.hpp"

// #include <fstream>
#define CTRL_KEY(k) ((k) & 0x1f)
#define Imagine_VERSION "0.0.1"
#define TAB_STOP 8
enum editorKey{
    ARROW_LEFT =1000,
    ARROW_DOWN ,
    ARROW_RIGHT ,
    ARROW_UP  ,
    PAGE_UP,
    PAGE_DOWN,
    HOME_KEY,
    END_KEY
};
void die(const char*);
void disableRawMode();
void enableRawMode(void);
int editorReadKey();
void editorProcessKeypress();

void editorRefreshScreen();
int getWindowSize(int*,int*);
void initEditor();
void editorDrawRows(struct abuf *);
int getCursorPosition(int *rows,int *cols);

// 存储一行文本
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
};
struct editorConfig E;





struct abuf{
    char *b;
    int len;
};
#define ABUF_INIT {NULL,0}

// 添加文本
void abAppend(struct abuf *ab,const char *s, int len){
    char *new_ = (char*)realloc(ab->b,ab->len+len);
    if (new_ == NULL) return;
    memcpy(&new_[ab->len],s,len);
    ab->b=new_;
    ab->len +=len;
}

void abFree(struct abuf *ab){
    free(ab->b);
}


/*** terminal ***/
void die(const char* s){
    editorRefreshScreen();
    perror(s);
    exit(1);
}

void disableRawMode(){
    if (tcsetattr(STDIN_FILENO,TCSAFLUSH,&E.orig_termios) == -1)die("tcsetattr");
    
}

void enableRawMode(void){
    
    if(tcgetattr(STDIN_FILENO,&E.orig_termios)==-1)die("tcgetattr");

    atexit(disableRawMode);
    struct termios raw = E.orig_termios;
    // 禁用ctrl+x，原始模式
    raw.c_iflag &= ~(BRKINT| IXON |INPCK | ISTRIP| ICRNL);
    raw.c_cflag |= CS8;
    raw.c_oflag &= ~(OPOST);

    // ICANON 字节读入 ISIG 关闭终端信号
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    // read返回前需要的最小字节数
    raw.c_cc[VMIN] = 0;
    // read返回之前等待的最大时间
    raw.c_cc[VTIME] = 1;



    if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw)==-1)die("tcsetattr");
}



// 获取窗口大小
int getWindowSize(int* rows,int* cols){
    struct winsize ws;
    // 如果不能从函数获得，还能通过光标移动的方式探测具体的窗口大小
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        DEBUG_LOG("rows:",rows," | cols: ",cols);
        return getCursorPosition(rows,cols);
        // 可以直接获得大小
    } else{ 

        *cols = ws.ws_col;
        *rows = ws.ws_row;
        DEBUG_LOG("rows:",ws.ws_row," | cols: ",ws.ws_col);
        return 0;
    }
}
// 已经移动完毕光标，获取光标位置
int getCursorPosition(int *rows,int *cols){
    char buf[32];
    unsigned int i=0;

    if (write(STDOUT_FILENO,"\x1b[6n",4) != 4) return -1;
    
    while (( i<sizeof(buf)-1)){
        if (read(STDIN_FILENO,&buf[i],1) != 1) break;
        if (buf[i] == 'R')break;
        i++;
    }
    buf[i] = '\0';
    // printf("\r\n&buf[1]: '%s'\r\n",&buf[1]);
    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    sscanf(&buf[2],"%d;%d" , rows,cols);
    return 0;
}



/*** input ***/
void editorMoveCursor(int key){
    erow *row = (E.cy >= E.numrows)? NULL : &E.row[E.cy];
    switch (key){
        case ARROW_LEFT:
            if (E.cx != 0)E.cx--;
            else if (E.cy != 0){
                E.cy--;
                E.cx = E.row[E.cy].size;
            }
            break;
        case ARROW_DOWN:
            if (E.cy < E.numrows)E.cy++;
            break;
        case ARROW_RIGHT:
            // if (E.cx != E.screencols-1)
            if (row && E.cx <row->size){
                E.cx++;
            } else if (row && E.cx == row->size){
                E.cy ++;
                E.cx =0;
            }

            break;   
        case ARROW_UP:
            if (E.cy != 0)E.cy--;
            break;
    }
    // 调整光标变化后的cx位置
    row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    int rowlen = row ? row->size : 0;
    if (E.cx > rowlen) E.cx = rowlen;
}

int editorReadKey(){
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO,&c,1)) != 1){
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    DEBUG_LOG(c);
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
    int c = editorReadKey();
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
/*** append buffer ***/



/*** output ***/

void editorScroll(){
    E.rx = E.cx;

    if (E.cy < E.rowoff){
        E.rowoff = E.cy;
    }
    if (E.cy >= E.rowoff + E.screenrows){
        E.rowoff = E.cy - E.screenrows +1;
    }
    if (E.cx < E.coloff){
        E.coloff = E.cx;
    }
    if (E.cx >= E.coloff + E.screencols){
        E.coloff = E.cx - E.screencols +1;
    } 
}


void editorDrawRows(struct abuf *ab){
    int y;
    // 显示文本
    for (y=0;y< E.screenrows;y++){
        int filerow = y + E.rowoff;
        
        // 上侧文本，下侧信息
        if (filerow >= E.numrows){
            // 相关信息，不涉及文本
            if (E.numrows==0 && y == E.screenrows /3 *2){
                char welcome[80];
                int welcomelen = snprintf(welcome,sizeof (welcome),
                "Imagine editor -- version %s",Imagine_VERSION);
                if (welcomelen > E.screencols) welcomelen = E.screencols;
                int padding = (E.screencols - welcomelen) /2;
                if (padding){
                    abAppend(ab,"~",1);
                    padding -=1;
                }
                while (padding--)
                {
                    abAppend(ab," ",1);
                }
                
                abAppend(ab,welcome ,welcomelen);
            }else {
                abAppend(ab,"~",1);
            }
    }else {
        int len = E.row[filerow].rsize - E.coloff;
        if (len < 0) len=0;
        if (len > E.screencols) len = E.screencols;
        abAppend(ab,&E.row[filerow].render[E.coloff],len);
    }
        abAppend(ab,"\x1b[K",3);
        if (y < E.screenrows -1){
            abAppend(ab,"\r\n",2);
        }
    }

}


void editorRefreshScreen(){
    editorScroll();

    struct abuf ab = ABUF_INIT;

    abAppend(&ab,"\x1b[?25l",6);
    
    // abAppend(&ab,"\x1b[2J",4);

    abAppend(&ab,"\x1b[H",3);
    editorDrawRows(&ab);
    char buf[32];
    // 光标索引是从1开始，但是不加1好像也没问题
    snprintf(buf,sizeof(buf),"\x1b[%d;%dH",(E.cy - E.rowoff)+1,
                                            (E.cx-E.coloff)+1);
    abAppend(&ab,buf,strlen(buf));

    abAppend(&ab,"\x1b[?25h",6);

    write(STDOUT_FILENO,ab.b,ab.len);
    abFree(&ab);
}


void editorUpdateRow(erow * row){
    int tabs = 0;
    int j;
    for (j=0;j<row->size;j++){
        if (row->chars[j] == '\t') tabs++;
    }
    free(row->render);
    row->render = (char *)malloc(row->size +tabs*(TAB_STOP-1) + 1);
    
    int idx=0;
    for(j=0;j<row->size;j++){
        if (row->chars[j] == '\t'){
            row->render[idx++] = ' ';
            while (idx % TAB_STOP != 0)row->render[idx++] = ' ';
        }
        row->render[idx++] = row->chars[j];
    }
    row->render[idx] = '\0';
    row->rsize = idx;
}

void editorAppendRow(char *s,size_t len){
    E.row = (erow *)realloc(E.row,sizeof(erow)*(E.numrows+1));
    int at = E.numrows;
    E.row[at].size = len;
    E.row[at].chars = (char *)malloc(len + 1);
    memcpy(E.row[at].chars,s,len);
    E.row[at].chars[len] = '\0';
    E.row[at].render = NULL;
    E.row[at].rsize = 0;
    
    editorUpdateRow(&E.row[at]);
    E.numrows++;

}


void editorOpen(char *filename){
    FILE *fp = fopen(filename,"r");
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
}







void initEditor(){
    E.cx=0;
    E.rx=0;
    E.cy=0;
    E.numrows =0; 
    E.row = NULL;
    E.rowoff=E.coloff=0;
    if (getWindowSize(&E.screenrows,&E.screencols) == -1) die("getWindowSize");
}

int main(int argc,char *argv[]){
    enableRawMode();
    initEditor();
    if (argc >= 2){
        editorOpen(argv[1]);
    }
    while (1){
        editorRefreshScreen();
        editorProcessKeypress();
    };
    return 0;
}