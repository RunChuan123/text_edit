#include "editor_entity.hpp"
#include "window_op.hpp"
#include "debug_logger.hpp"
#include "process_key.hpp"
#include "status.hpp"
#include "file_op.hpp"
#include <err.h>
editorConfig E;
 
int main(int argc,char *argv[]){
    
    enableRawMode();
    try{
        initEditor();
        if (argc >= 2){
            editorOpen(argv[1]);
        }
        editorSetStatusMessage("HELP:Ctrl+S = Save | Ctrl-Q = quit");

        while (1){
            editorRefreshScreen();
            editorProcessKeypress();
        };
        free(E.filename);
    }
    catch(std::exception e){
        disableRawMode();
    }
    return 0;
}