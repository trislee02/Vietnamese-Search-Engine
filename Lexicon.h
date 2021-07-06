#pragma once
#include <string.h>
#include "ListStack.h"

struct lexicon {
    SList main;
};

struct tnode {
    wchar_t chr;
    bool isWord;
    SList branch;
    void* adapter;
};

void init(lexicon& lex);

int cmptNode(void* a, void* b);

void addString(lexicon& lex, wchar_t* str, void* adapter);

tnode* findString(lexicon lex, wchar_t* str);

void traverseLex(lexicon lex, void (*doTaskforWord)(wchar_t*, void*), void (*doTaskforAdapter)(void*));

void printLexicon(lexicon lex, void (*printAdapter)(void*));