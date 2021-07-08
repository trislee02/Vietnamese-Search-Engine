#include "Indexor.h"
#include "Searcher.h"
#include <time.h>
#include "TextProcessing.h"
#include "DocCleaner.h"
#include "Menu.h"
/*Lib cho xuat unicode ra man hinh*/
#include <fcntl.h>
#include <io.h>

void main()
{
    /***********************************Doc va xu ly file don gian******************************
    FILE* findex = fopen("index.txt", "r");
    int nfile = 0;
    char **listfile = readCorpus(findex, nfile);
    wchar_t* textfile;
    //_setmode(_fileno(stdout), _O_U16TEXT);
    wprintf(L"=================File: %ls=============\n", chr2wchr(listfile[1]));
    textfile = readFile(listfile[1]);
    rmvRedundant(textfile);
    wprintf(L"---->%ls<----\n", text);
    wprintf(L"*******************************\n");

    normalize(text);
    wprintf(L"---->%ls<----\n", text);
    wprintf(L"*******************************\n");

    removeDiacritic(text);
    wprintf(L"---->%ls<----\n", text);
    wprintf(L"*******************************\n");

    SList token = splitword(text, L' ');
    printList(token, printwchar);*/
    /**********************************************************************************************/

    /*********************************Indexing**************************************************/
    /*lexicon mainlex;
    SList wordList;
    SList wordList2;
    initialize(wordList);
    initialize(wordList2);
    init(mainlex);
    wchar_t text2[] = L"dai";
    wchar_t text3[] = L"nam";
    wchar_t text4[] = L"viet";
    wchar_t text5[] = L"nam";
    wchar_t text6[] = L"khong";
    wchar_t text7[] = L"o";
    wchar_t text8[] = L"dai";
    wchar_t text9[] = L"duong";
    addHead(wordList, text2, wcharsize(text2));
    addHead(wordList, text3, wcharsize(text3));
    addHead(wordList, text4, wcharsize(text4));
    addHead(wordList, text5, wcharsize(text5));
    addHead(wordList, text6, wcharsize(text6));
    addHead(wordList, text7, wcharsize(text7));
    addHead(wordList, text8, wcharsize(text8));
    addHead(wordList, text9, wcharsize(text9));

    wchar_t text2b[] = L"duong";
    wchar_t text3b[] = L"hoa";
    wchar_t text4b[] = L"phat";
    wchar_t text5b[] = L"o";
    wchar_t text6b[] = L"viet";
    wchar_t text7b[] = L"nam";
    wchar_t text8b[] = L"ne";
    addHead(wordList2, text2b, wcharsize(text2b));
    addHead(wordList2, text3b, wcharsize(text3b));
    addHead(wordList2, text4b, wcharsize(text4b));
    addHead(wordList2, text5b, wcharsize(text5b));
    addHead(wordList2, text6b, wcharsize(text6b));
    addHead(wordList2, text7b, wcharsize(text7b));
    addHead(wordList2, text8b, wcharsize(text8b));

    FILE* barrel = fopen("barrel.txt", "wb");

    int n = 0;
    WData* wordArray = mkWordArray(wordList, n);
    printf("n = %d\n", n);
    printf("wordArray:\n");
    printWordArray(wordArray, n);

    WAData* addressArray = saveBarrel(barrel, wordArray, n);
    printf("addressArray:\n");
    printAddressArray(addressArray, n);

    freeWordArray(wordArray, n);
    adtWAdd2Lex(mainlex, 3, addressArray, n);
    freeAddressArray(addressArray, n);
    //****************************************
    n = 0;
    wordArray = mkWordArray(wordList2, n);
    printf("n = %d\n", n);
    printf("wordArray:\n");
    printWordArray(wordArray, n);

    addressArray = saveBarrel(barrel, wordArray, n);
    printf("addressArray:\n");
    printAddressArray(addressArray, n);
    fclose(barrel);
    freeWordArray(wordArray, n);
    adtWAdd2Lex(mainlex, 99, addressArray, n);
    freeAddressArray(addressArray, n);

    //printLexicon(mainlex, printDoclist);
    fclose(barrel);
    printf("*-------------------------------------------------------*\n");
    FILE* fmainlex = fopen("mainlex.txt", "wb");
    FILE* fbarrelindexor = fopen("barrelindexor.txt", "wb");
    saveBIandLex(fbarrelindexor, fmainlex, mainlex);
    printLexicon(mainlex, printwldata);
    fclose(fmainlex);
    fclose(fbarrelindexor);
    */
    /**********************************************************************************************/

    /************************************Searching*************************************************/
    /*
    printf("\n===================================\n");
    FILE* fmainlex = fopen("mainlex.txt", "rb");
    FILE* fbarrelindexor = fopen("barrelindexor.txt", "rb");
    //fmainlex = fopen("mainlex.txt", "rb");
    //fbarrelindexor = fopen("barrelindexor.txt", "rb");
    FILE* fbarrel = fopen("barrel.txt", "rb");


    lexicon seclex = readLexicon(fmainlex);
    printLexicon(seclex, printwldata);
    fclose(fmainlex);

    wchar_t token1[] = L"viet";
    wchar_t token2[] = L"nam";
    SList token;
    initialize(token);
    addHead(token, token2, wcharsize(token2));
    addHead(token, token1, wcharsize(token1));

    SList QDoclist = searchAQuery(seclex, fbarrelindexor, fbarrel, token);
    printList(QDoclist, printSDData);

    printf("***************************\n");

    calculateScore(fbarrel, QDoclist);
    printList(QDoclist, printSDData);

    printf("***************************\n");

    rankDoc(QDoclist);
    printList(QDoclist, printSDData);

    tnode* sno = findString(seclex, token1);
    if (sno) printf("YES\n");
    else printf("NO\n");
    */

    /************************************REAL FILES*************************************************/
    /*------------------------------- INDEXING CORPUS ----------------------------*/
    
    /*
    printf("*************************INDEXING CORPUS***********************************\n");
    char** listfiles = NULL;
    lexicon seclex;
    doIndexing(seclex, listfiles);
    printf("Done indexing\n");
    printf("Press enter to search\n");
    getchar();
    
    /*--------------------------------- SEARCHING -------------------------------*/
    
    /*
    printf("*************************SEARCHING***********************************\n");
    FILE* fmainlex = fopen(LEXICON_FILES, "rb");
    lexicon mainlex = readLexicon(fmainlex);
    //printLexicon(mainlex, printwldata);
    printf("Press enter to continue\n");
    fclose(fmainlex);
    //getchar();
    
    wchar_t query[] = L"trinh cong son";
    SList tokens = splitword(query, L' ');
    clock_t begin = clock();
    SList QDoclist = doSearching(mainlex, tokens, K_BEST_DOC_DEFAULT);
    
    printf("Searching spent time: %lf - %d files have found\n", (double)(clock() - begin) / CLOCKS_PER_SEC, QDoclist.size);
    printf("=======================================\n");
    printf("                RESULTS\n");
    printf("=======================================\n");
    printList(QDoclist, printSDData);
    
    DocIndexor docidxor = readDocIndexor(DOC_INDEXOR);
    //printDocIndexor(docidxor);
    int ndocs = docidxor.ndocs;
    DocData* docArray = docidxor.docarray;
    int k;
    do {
        printf("Press a number to see file name or enter -1 to exit: ");
        scanf("%d", &k);
        if (k == -1) break;
        DocData docdata;
        getDocDatabyId(docArray, ndocs, docdata, k);
        printf("File name = %s\n", docdata.docdir);
    } while (k != -1);
    freeDocArray(docArray, ndocs);
    getchar();
    getchar();
    printf("Success");
    
    /*------------------------------------ENDING------------------------------------*/
    /*
    printf("*************************INDEXING CORPUS***********************************\n");
    char** listfiles = NULL;
    lexicon seclex;
    doIndexing(seclex, listfiles);
    printLexicon(seclex, printwldata);
    printf("Done indexing\n");
    printf("Press enter to continue\n");
    getchar();
    /*-----------------------------------ADD A FILE-------------------------------------*/
    /*
    FILE* fmainlex = fopen(LEXICON_FILES, "rb");
    lexicon mainlex = readLexicon(fmainlex);
    fclose(fmainlex);    
    DocIndexor docidxor = readDocIndexor(DOC_INDEXOR);
    printDocIndexor(docidxor);
    printLexicon(mainlex, printwldata);

    getchar();

    char fname[] = "corpus/doc4.txt";
    indexNewFile(fname, docidxor, mainlex);
    printDocIndexor(docidxor);
    printLexicon(mainlex, printwldata);
    printf("Successful\n");
    */
    /*-----------------------------------REMOVE A FILE-------------------------------------*/
    /*
    FILE* fmainlex = fopen(LEXICON_FILES, "rb");
    lexicon mainlex = readLexicon(fmainlex);
    fclose(fmainlex);
    DocIndexor docidxor = readDocIndexor(DOC_INDEXOR);

    DocData* doc = getDocDatabyDir(docidxor, "corpus\\doc3.txt");
    printf("docId = %d\n", doc->docId);
    removeFile(doc->docId, docidxor, mainlex);
    printf("Successful\n");
    /*------------------------------------------------------------------------*/
    
    lexicon swlex = mkstopwordLex(STOPWORD_FILES);
    
    wchar_t* text = readFile("corpus/LS_TT_ (106).txt");
    rmvRedundant(text);
    normalize(text);
    SList tokens = splitword(text, L' ');
    int n;
    WData* warray = makeWordArray2(tokens, n, swlex);
    printWordArray(warray, n);
    freeWordArray(warray, n);
    
    /*------------------------------------------------------------------------*/
}