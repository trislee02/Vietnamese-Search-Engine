#pragma once
#include <math.h>
#include "Lexicon.h"
#include "Utility.h"
#include "dataStruct.h"
#include "Constant.h"

lexicon readLexicon(FILE* f);

WLData* getWLData(lexicon lex, wchar_t* word);

Doclist* getDoclist(FILE* fbarrelidxor, WLData wldata);

int getIndexWDData(Doclist doclist, int docId);

WData* getWordArray(FILE* fbarrel, int nword, int docAdd);

WData getWordData(FILE* fbarrel, int wordAdd);

SList searchAQuery(lexicon mainlex, FILE* fbarrelidxor, FILE* fbarrel, SList token);

DocIndexor readDocIndexor(const char* didxorfname);

int getDocDatabyId(DocData* docArray, int ndocs, DocData& docdata, int docId);

DocData* getDocDatabyId(DocData* docArray, int ndocs, int docId);

DocData* getDocDatabyDir(DocIndexor docidxor, const char* dir);

void calculateScore(FILE* fbarrel, const char* docidxorfname, SList& QDocList);

void rankDoc(SList& QDocList);

int getDocId(void* ddata);

float getDocScore(void* ddata);

SList doSearching(lexicon mainlex, SList tokens, int k);