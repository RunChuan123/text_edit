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
#include <iostream>

termios E;

void enableRawMode(void){

    tcgetattr(STDIN_FILENO,&E);

    struct termios raw = E;
    // 禁用ctrl+x，原始模式
    raw.c_iflag &= ~(BRKINT| IXON |INPCK | ISTRIP| ICRNL);
    raw.c_cflag |= CS8;
    raw.c_oflag &= ~(OPOST);

    // ICANON 字节读入 ISIG 关闭终端信号
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    // read返回前需要的最小字节数
    raw.c_cc[VMIN] = 0;
    // read返回之前等待的最大时间
    raw.c_cc[VTIME] = 10;



    tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw);
}

void disableRawMode(){
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&E);
}

int main(){
    enableRawMode();
    char nread;
    while(read(STDIN_FILENO,&nread,1)==1){
        
        
        std::cout << nread << " " << int(nread);
        if(nread == 'c')break;
    }


    disableRawMode();
    return 0;
}