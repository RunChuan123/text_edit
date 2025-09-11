#include "editor_entity.hpp"

void die(const char*);
void disableRawMode();
void enableRawMode(void);
void editorAppendRow(char *s,size_t len);
void editorUpdateRow(erow * row);
int getWindowSize(int*,int*);
void initEditor();
void editorDrawRows(struct abuf *);
void editorMoveCursor(int key);
void editorOpen(char *filename);
void editorSetStatusMessage(const char* fmt,...);

void editorRefreshScreen();
int getCursorPosition(int *rows,int *cols);
int editorRowToRx(erow *row,int cx);