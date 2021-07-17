#include "Utility.h"

int cmpEleMinInterval(void* a, void* b) {
    EleMinInterval* wordA = (EleMinInterval*)a;
    EleMinInterval* wordB = (EleMinInterval*)b;
    if (wordA->position < wordB->position) return 1;
    if (wordA->position > wordB->position) return -1;
    return 0;
}

void printEleMinInterval(void* a) {
    EleMinInterval* word = (EleMinInterval*)a;
    wprintf(L"%d (%d) ", word->keywordNo, word->position);
}

int cmpwchar(void* a, void* b) {
    wchar_t* stra = (wchar_t*)a;
    wchar_t* strb = (wchar_t*)b;
    return wcscmp(strb, stra);
}

void printwchar(void* a) {
    wchar_t* str = (wchar_t*)a;
    wprintf(L"%ls ", str);
}

int wcharsize(void* a) {
    wchar_t* str = (wchar_t*)a;
    return (wcslen(str) + 1) * sizeof(wchar_t);
}

void rmvwchar(void* a) {
    wchar_t* str = (wchar_t*)a;
    free(str);
}

void printAddressArray(WAData* addressArray, int n) {
    wprintf(L"Address array: \n");
    for (int i = 0; i < n; i++) wprintf(L"%ls - %ld\n", addressArray[i].word, addressArray[i].wordAdd);
}

void freeAddressArray(WAData* addressArray, int n) {
    for (int i = 0; i < n; i++)	free(addressArray[i].word);
    free(addressArray);
}

void printDoclist(void* a) {
    Doclist* doclist = (Doclist*)a;
    wprintf(L"Doclist: ");
    for (int i = 0; i < doclist->ndoc; i++)
        wprintf(L"%d: %d, ", (doclist->docArray)[i].docId, (doclist->docArray)[i].wordAdd);
    wprintf(L"\n");
}

void freeDoclist(void* a) {
    Doclist* doclist = (Doclist*)a;
    free(doclist->docArray);
    free(doclist);
}

void printwldata(void* a) {
    WLData* wldata = (WLData*)a;
    wchar_t* w = wldata->word;
    wprintf(L"wldata: nchar = %d, word = %ls, dlistadd = %d, ndoc = %d\n", wldata->nchar, wldata->word, wldata->dlistAdd, wldata->ndoc);
}

void freewldata(void* a) {
    WLData* wldata = (WLData*)a;
    free(wldata->word);
    free(wldata);
}

void printWord(WData wd) {
    wprintf(L"%ls - tf: %lf\n", wd.word, wd.tf);
    for (int i = 0; i < wd.npos; i++) wprintf(L"%d ", wd.posarray[i]);
    wprintf(L"\n");
}

void printWordArray(WData* wordArray, int n) {
    for (int i = 0; i < n; i++) {
        printWord(wordArray[i]);
    }
}

void freeWordArray(WData* wordArray, int n) {
    for (int i = 0; i < n; i++) {
        free(wordArray[i].word);
        free(wordArray[i].posarray);
    }
    free(wordArray);
}

void printSDData(void* a) {
    SDData* ddata = (SDData*)a;
    wprintf(L"SDData:\nDocId = %d Score = %f nword = %d\n", ddata->docId, ddata->score, ddata->nword);

    for (int i = 0; i < ddata->nword; i++) {
        wprintf(L"wordAdd = %d\n", ddata->wordAddArray[i].wordAdd);
    }
}

void freeSDData(void* a) {
    SDData* ddata = (SDData*)a;
    free(ddata->wordAddArray);
    free(ddata);
}

wchar_t* chr2wchr(const char* c)
{
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);
    return wc;
}

int cmpSDDataDocId(void* a, void* b) {
    SDData* wa = (SDData*)a;
    SDData* wb = (SDData*)b;
    if (wa->docId == wb->docId) return 0;
    if (wa->docId > wb->docId) return -1;
    return 1;
}

int cmpSDDataScore(void* a, void* b) {
    SDData* wa = (SDData*)a;
    SDData* wb = (SDData*)b;
    if (fabs(wa->score - wb->score) < 1e-6) return 0;
    if (wa->score > wb->score) return -1;
    return 1;
}

void freeDocArray(void* a, int n) {
    DocData* docArray = (DocData*)a;
    for (int i = 0; i < n; i++)
        free(docArray[i].docdir);
    free(docArray);
}

void printDocIndexor(DocIndexor docidxor) {
    wprintf(L"Doc indexor:\n");
    DocData* docArray = docidxor.docarray;
    for (int i = 0; i < docidxor.ndocs; i++) {
        wchar_t* tempfname = chr2wchr(docArray[i].docdir);
        wprintf(L"docId = %d, dir = %s, nword = %d\n", docArray[i].docId, tempfname, docArray[i].nword);
        free(tempfname);
    }
}

void freeDocIndexor(void* a) {
    DocIndexor* docidxor = (DocIndexor*)a;
    freeDocArray(docidxor->docarray, docidxor->ndocs);
}

void printTokenData(void* a) {
    TokenData* tokData = (TokenData*)a;
    //wprintf(L"%ls (%d)\n", tokData->word, tokData->position);
    wprintf(L"%ls ", tokData->word, tokData->position);
}

void freeTokenData(void* a) {
    TokenData* tokData = (TokenData*)a;
    free(tokData->word);
    free(tokData);
}

void* copyTokenData(void* a) {
    TokenData* tokenData = (TokenData*)a;
    TokenData* copiedtokenData = new TokenData;
    copiedtokenData->position = tokenData->position;
    copiedtokenData->word = new wchar_t[wcslen(tokenData->word) + 1];
    wcscpy(copiedtokenData->word, tokenData->word);
    return copiedtokenData;
}

wchar_t* cpywchar(wchar_t* a) {
    wchar_t* res = new wchar_t[wcslen(a) + 1];
    wcscpy(res, a);
    return res;
}