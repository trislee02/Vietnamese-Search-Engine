#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <math.h>
#include "dataStruct.h"

int cmpwchar(void* a, void* b);

void printwchar(void* a);

int wcharsize(void* a);

void rmvwchar(void* a);

void printAddressArray(WAData* addressArray, int n);

void freeAddressArray(WAData* addressArray, int n);

void printDoclist(void* a);

void freeDoclist(void* a);

void printwldata(void* a);

void freewldata(void* a);

void printWord(WData wd);

void printWordArray(WData* wordArray, int n);

void freeWordArray(WData* wordArray, int n);

void printSDData(void* a);

wchar_t* chr2wchr(const char* c);

int cmpSDDataDocId(void* a, void* b);

int cmpSDDataScore(void* a, void* b);

void freeDocArray(void* a, int n);

void printDocIndexor(DocIndexor docidxor);