#pragma once
#include "editor_entity.hpp"

// void editorRowInsertChar(erow *row,int at, int c);

void editorInsertChar(int c);
void editorRowDelChar(erow *row,int at);
void editorDelChar();
void editorDelRow(int at);
void editorRowAppendString(erow *row,char *s,size_t len);