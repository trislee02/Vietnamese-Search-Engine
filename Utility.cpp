#include "Utility.h"

int cmpwchar(void* a, void* b) {
    wchar_t* stra = (wchar_t*)a;
    wchar_t* strb = (wchar_t*)b;
    return wcscmp(strb, stra);
}

void printwchar(void* a) {
    wchar_t* str = (wchar_t*)a;
    wprintf(L"1-|%ls|\n", str);
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
    printf("Doclist: ");
    for (int i = 0; i < doclist->ndoc; i++)
        printf("%d: %d, ", (doclist->docArray)[i].docId, (doclist->docArray)[i].wordAdd);
    printf("\n");
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
}

void printWordArray(WData* wordArray, int n) {
    for (int i = 0; i < n; i++) {
        printWord(wordArray[i]);
    }
}

void freeWordArray(WData* wordArray, int n) {
    for (int i = 0; i < n; i++)
        free(wordArray[i].word);
    free(wordArray);
}

void printSDData(void* a) {
    SDData* ddata = (SDData*)a;
    printf("SDData:\nDocId = %d Score = %f nword = %d\n", ddata->docId, ddata->score, ddata->nword);

    for (int i = 0; i < ddata->nword; i++) {
        printf("wordAdd = %d\n", ddata->wordAddArray[i].wordAdd);
    }
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