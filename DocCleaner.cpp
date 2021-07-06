#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "SList.h"
#include "ListStack.h"
#include "Lexicon.h"
#include"Utility.h"


bool rmvRedundant(wchar_t*& str)
{
    int j = 0;
    wchar_t* pwc;
    wchar_t rdd[] = L".,:;-/ @#?()!*_=+\\{}[]~`^&%$\'\"\n\r";
    wchar_t* res = (wchar_t*)malloc(sizeof(wchar_t) * 1000);
    
    bool isWord = false;
    for (int i = 0; i < wcslen(str); i++) {
        pwc = wmemchr(rdd, str[i], wcslen(rdd));
        if (pwc) {
            if (isWord) {
                res[j] = L' ';
                j++;
            }
            isWord = false;
        }
        else {
            res[j] = str[i];
            isWord = true;
            j++;
        }
        if (j % 999) res = (wchar_t*)realloc(res, sizeof(wchar_t) * (j + 1001));
    }
    res[j] = L'\0';
    res = (wchar_t*)realloc(res, sizeof(wchar_t) * (j+1));
    if (res[wcslen(res) - 1] == L' ')
        res[wcslen(res) - 1] = L'\0';
    
    free(str);
    str = res;
    
    return true;
}