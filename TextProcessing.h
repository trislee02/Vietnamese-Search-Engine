#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "SList.h"
#include "ListStack.h"
#include "Lexicon.h"
#include"Utility.h"
#define MAXONELINE 100

void removeDiacritic(wchar_t*& str);

void normalize(wchar_t*& str);

wchar_t* getline(FILE* f);

wchar_t* readFile(const char* filename);

SList splitword(wchar_t* str, wchar_t deliminator);

char** readCorpus(FILE* f, int& n);

bool insert2file(const char* filename, void* buffer, int nbyte, int pos);

bool rmvPartFile(const char* filename, int nbyte, int pos);