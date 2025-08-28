#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>


struct termios orig_termios;

void die(const char* s){
    perror(s);
    exit(1);
}

void disableRawMode(){
    if (tcsetattr(STDERR_FILENO,TCSAFLUSH,&orig_termios) == -1)die("tcsetattr");
    
}

void enableRawMode(void){
    
    if(tcgetattr(STDIN_FILENO,&orig_termios)==-1)die("tcgetattr");

    atexit(disableRawMode);
    struct termios raw = orig_termios;

    raw.c_iflag &= ~(BRKINT| IXON |INPCK | ISTRIP| ICRNL);
    raw.c_cflag &= ~(CS8);
    raw.c_oflag &= ~(OPOST);

    // ICANON 字节读入 ISIG 关闭终端信号
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    // read返回前需要的最小字节数
    raw.c_cc[VMIN] = 0;
    // read返回之前等待的最大时间
    raw.c_cc[VTIME] = 10;
    if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw)==-1)die("tcsetattr");
}


int main(void){
    enableRawMode();
    
    while (1){
        char c = '\0';
        if (read(STDIN_FILENO,&c,1)== -1 && errno != EAGAIN) die("read");
        if (iscntrl(c)){
            printf("%d\r\n",c);
        } else {
            printf("%d ('%c')\r\n",c,c);
        }
        if (c == 'q') break;
    };
    return 0;
}