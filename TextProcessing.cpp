#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "SList.h"
#include "ListStack.h"
#include "Lexicon.h"
#include"Utility.h"
#define MAXONELINE 100

void removeDiacritic(wchar_t*& str)
{
    wchar_t coDau[] = L"àáạảãâầấậẩẫăằắặẳẵèéẹẻẽêềếệểễìíịỉĩòóọỏõôồốộổỗơờớợởỡùúụủũưừứựửữỳýỵỷỹđÀÁẠẢÃÂẦẤẬẨẪĂẰẮẶẲẴÈÉẸẺẼÊỀẾỆỂỄÌÍỊỈĨÒÓỌỎÕÔỒỐỘỔỖƠỜỚỢỞỠÙÚỤỦŨƯỪỨỰỬỮỲÝỴỶỸĐ";
    wchar_t koDau[] = L"aaaaaaaaaaaaaaaaaeeeeeeeeeeeiiiiiooooooooooooooooouuuuuuuuuuuyyyyydAAAAAAAAAAAAAAAAAEEEEEEEEEEEIIIIIOOOOOOOOOOOOOOOOOUUUUUUUUUUUYYYYYD";
    for (int i = 0; i < wcslen(str); i++)
    {
        wchar_t* pwc;
        pwc = wmemchr(coDau, str[i], wcslen(coDau));
        if (pwc != NULL) {
            int a = pwc - coDau;
            str[i] = koDau[a];
        }
    }
}

void normalize(wchar_t*& str)
{
    wchar_t inhoa[] =  L"ÀÁẠẢÃÂẦẤẬẨẪĂẰẮẶẲẴÈÉẸẺẼÊỀẾỆỂỄÌÍỊỈĨÒÓỌỎÕÔỒỐỘỔỖƠỜỚỢỞỠÙÚỤỦŨƯỪỨỰỬỮỲÝỴỶỸĐABCDEFGHIJKLMNOPQRSTUVWXYZ";
    wchar_t thuong[] = L"àáạảãâầấậẩẫăằắặẳẵèéẹẻẽêềếệểễìíịỉĩòóọỏõôồốộổỗơờớợởỡùúụủũưừứựửữỳýỵỷỹđabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < wcslen(str); i++)
    {
        wchar_t* pwc;
        pwc = wmemchr(inhoa, str[i], wcslen(inhoa));
        if (pwc != NULL) {
            int a = pwc - inhoa;
            str[i] = thuong[a];
        }
    }
}

wchar_t* getline(FILE* f)
{
    wchar_t str[MAXONELINE];
    fgetws(str, MAXONELINE, f);
    if (!str) return NULL;
    wchar_t* newline = (wchar_t*)wmemchr(str, L'\n', wcslen(str));
    if (newline) str[newline - str] = '\0';
    wchar_t* text = new wchar_t[wcslen(str) + 1];
    wcscpy(text, str);
    return text;
}

wchar_t* readFile(const char* filename)
{
    wchar_t* fname = chr2wchr(filename);
    
    FILE* f;
    _wfopen_s(&f, fname, L"r, ccs=UTF-16LE");
    free(fname);
    if (!f) return NULL;

    int x;
    fseek(f, 0, SEEK_END);
    x = ftell(f);
    fseek(f, 2, SEEK_SET);
    wchar_t* str = new wchar_t[x + 1];
    wchar_t* s = new wchar_t[20];
    fgetws(s, 20, f);
    wcscpy(str, s);
    while (!feof(f)) {
        fgetws(s, 20, f);
        wcscat(str, s);
    }
    free(s);
    fclose(f);
    return str;
}

SList splitword(wchar_t* str, wchar_t deliminator)
{
    SList s;
    initialize(s);
    wchar_t ss[2000];
    int j = 0;
    wchar_t* word;
    for (int i = 0; i < wcslen(str) + 1; i++)
    {
        if (str[i] != deliminator) {
            ss[j] = str[i];
            j++;
        }
        else {
            ss[j] = L'\0';
            word = new wchar_t[wcslen(ss) + 1];
            wcscpy(word, ss);
            addTail(s, word, wcharsize(ss));
            free(word);
            wcscpy(ss, L"");
            j = 0;
        }
    }
    if (wcslen(ss) > 0) {
        word = new wchar_t[wcslen(ss) + 1];
        wcscpy(word, ss);
        addTail(s, ss, wcharsize(ss));
        free(word);
    }
    return s;
}

char** readCorpus(FILE* f, int& n)
{
    char** s = new char* [20000];
    n = 0;
    while (!feof(f)) {
        char str[100];
        fgets(str, 100, f);
        char* newline = (char*) memchr(str, '\n', strlen(str));
        if (newline) str[newline - str] = '\0';

        s[n] = new char[strlen(str) + 1];
        strcpy(s[n], str);
        n++;
    }
    return s;
}


bool insert2file(const char* filename, void* buffer, int nbyte, int pos) {
    FILE* f = fopen(filename, "rb+");
    if (!f) return false;
    int movebyte = 1000; //Hard code
    fseek(f, 0, SEEK_END);
    int totalByte = ftell(f);
    char buf[1000]; // = movebyte   (Hard code)
    if (pos > totalByte) return false;
    while (ftell(f) - pos >= movebyte) {
        fseek(f, -movebyte, SEEK_CUR);
        int startcpypos = ftell(f);
        fread(buf, sizeof(char), movebyte, f);
        fseek(f, -movebyte + nbyte, SEEK_CUR);
        fwrite(buf, sizeof(char), movebyte, f);
        fseek(f, startcpypos - ftell(f), SEEK_CUR);
    }
    int nbyteleft = ftell(f) - pos;
    fseek(f, -nbyteleft, SEEK_CUR);
    int p = ftell(f);
    fread(buf, sizeof(char), nbyteleft, f);
    fseek(f, -nbyteleft + nbyte, SEEK_CUR);
    fwrite(buf, sizeof(char), nbyteleft, f);
    fseek(f, pos, SEEK_SET);
    fwrite(buffer, sizeof(char), nbyte, f);
    fclose(f);
    return true;
}

bool rmvPartFile(const char* filename, int nbyte, int pos) {
    if (nbyte <= 0) return false;

    bool stt = true;
    int movebyte = 1000; //Hard code
    char buf[1000]; // = movebyte
    
    FILE* f = fopen(filename, "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    int totalbyte = ftell(f);

    if (pos > totalbyte) return false;

    fseek(f, 0, SEEK_SET);
    char* tempfilename = new char[strlen(filename) + 5];
    strcpy(tempfilename, filename);
    strcat(tempfilename, "tmp");
    FILE* ftemp = fopen(tempfilename, "wb");
    while (pos - ftell(f) >= movebyte) {
        fread(buf, sizeof(char), movebyte, f);
        fwrite(buf, sizeof(char), movebyte, ftemp);
    }
    int nbyteleft = pos - ftell(f);
    fread(buf, sizeof(char), nbyteleft, f);
    fwrite(buf, sizeof(char), nbyteleft, ftemp);
    fseek(f, nbyte, SEEK_CUR);
    while (totalbyte - ftell(f) >= movebyte) {
        fread(buf, sizeof(char), movebyte, f);
        fwrite(buf, sizeof(char), movebyte, ftemp);
    }
    nbyteleft = totalbyte - ftell(f);
    fread(buf, sizeof(char), nbyteleft, f);
    fwrite(buf, sizeof(char), nbyteleft, ftemp);
    fclose(f);
    fclose(ftemp);

    if (remove(filename)) {
        stt = false;
        wprintf(L"Unable to remove file\n");
    }
    if (rename(tempfilename, filename)) {
        stt = false;
        wprintf(L"Unable to rename file\n");
    }
    free(tempfilename);
    return stt;
}