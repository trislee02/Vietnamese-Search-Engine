#pragma once
#include "dataStruct.h"
#include "Utility.h"
#include "Lexicon.h"
#include "DocCleaner.h"
#include "TextProcessing.h"
#include "Searcher.h"
#include <time.h>
#include "Constant.h"

lexicon mkstopwordLex(const char* stopwordfname);

SList makeTokensList(SList token);

WData* makeWordArray2(SList token, int& n, lexicon stopwordlex, float thresholdStopWord);

WData* makeWordArray3(SList token, int& n, lexicon stopwordlex, Config config);

WData* mkWordArray(SList token, int& n, lexicon stopwordlex, Config config);

bool saveDoc2DocIndexor(FILE* fdocidxor, DocData docdata);

bool saveDocIndexor(const char* docidxorfname, DocIndexor docidxor);

WAData* saveBarrel(FILE* fbarrel, WData* wordarray, int n, int& docAdd);

void saveLex(FILE* flex, lexicon lex);

void adtWAdd2Lex(lexicon& lex, int docId, WAData* addressArray, int n);

void saveDoclist2BIndexor(FILE* fbarrelidxor, Doclist* doclist, int& dlistAdd);

void saveBIandLex(FILE* f, FILE* flex, lexicon& lex);

bool rewriteBIandLex(const char* lexfname, const char* barrelidxorfname, lexicon& lex);

bool rewriteBarrelandDocIdxor(const char* docidxorfname, const char* barrelfname, DocIndexor docidxor);

int doIndexing(lexicon& mainlex, Config config);

DocData* addDoc2DocIndexor(char* dir, DocIndexor& docidxor);

DocData* rmvDocInDocIndexor(int docId, DocIndexor& docidxor);

int indexNewFile(char* dir, DocIndexor& docidxor, lexicon& lex, Config config);

int removeFile(int docId, DocIndexor& docidxor, lexicon& lex, Config config);