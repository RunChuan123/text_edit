#include "editor_entity.hpp"
#include "basic_func.hpp"
#include "debug_logger.hpp"
#include "process_key.hpp"

editorConfig E;

int main(int argc,char *argv[]){
    enableRawMode();
    initEditor();
    if (argc >= 2){
        editorOpen(argv[1]);
    }
    editorSetStatusMessage("HELP: Control-Q = quit");

    while (1){
        editorRefreshScreen();
        editorProcessKeypress();
    };
    free(E.filename);
    return 0;
}