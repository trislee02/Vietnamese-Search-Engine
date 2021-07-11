#include "Indexor.h"
#include "Searcher.h"
#include <time.h>
#include "TextProcessing.h"
#include "DocCleaner.h"
#include "Runner.h"
/*Lib cho xuat unicode ra man hinh*/
#include <fcntl.h>
#include <io.h>

void main()
{
    Run();
    //Config config = getConfigData(CONFIG_FILE);
    /************************************REAL FILES*************************************************/
    /*------------------------------- INDEXING CORPUS ----------------------------*/
    
    /*
    printf("*************************INDEXING CORPUS***********************************\n");
    char** listfiles = NULL;
    lexicon seclex;
    doIndexing(seclex, config);
    printf("Done indexing\n");
    printf("Press enter to search\n");
    getchar();

    /*--------------------------------- SEARCHING -------------------------------*/
    /*
    printf("*************************SEARCHING***********************************\n");
    FILE* fmainlex = fopen(config.LEXICON_FILES, "rb");
    lexicon mainlex = readLexicon(fmainlex);
    //printLexicon(mainlex, printwldata);
    printf("Press enter to continue\n");
    fclose(fmainlex);
    getchar();

    wchar_t query[] = L"le quang tri";

    SList tokens = splitword(query, L' ');
    clock_t begin = clock();
    SList QDoclist = doSearching(mainlex, tokens, config);

    printf("Searching spent time: %lf - %d files have found\n", (double)(clock() - begin) / CLOCKS_PER_SEC, QDoclist.size);
    printf("=======================================\n");
    printf("                RESULTS\n");
    printf("=======================================\n");
    printList(QDoclist, printSDData);

    DocIndexor docidxor = readDocIndexor(config.DOC_INDEXOR);
    //printDocIndexor(docidxor);
    int ndocs = docidxor.ndocs;
    DocData* docArray = docidxor.docarray;
    int k;
    do {
        printf("Press a number to see file name or enter -1 to exit: ");
        scanf("%d", &k);
        if (k == -1) break;
        DocData docdata;
        int res = getDocDatabyId(docArray, ndocs, docdata, k);
        if (res == -1)
            printf("Not found\n");
        else
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
    FILE* fmainlex = fopen(config.LEXICON_FILES, "rb");
    lexicon mainlex = readLexicon(fmainlex);
    fclose(fmainlex);
    DocIndexor docidxor = readDocIndexor(config.DOC_INDEXOR);
    printDocIndexor(docidxor);
    printLexicon(mainlex, printwldata);
    printf("Press enter to continue\n");
    getchar();

    char fname[] = "corpus/Tu them/doc3.txt";
    indexNewFile(fname, docidxor, mainlex, config);
    printDocIndexor(docidxor);
    printLexicon(mainlex, printwldata);
    printf("Successful\n");
    
    /*-----------------------------------REMOVE A FILE-------------------------------------*/
    /*
    FILE* fmainlex = fopen(config.LEXICON_FILES, "rb");
    lexicon mainlex = readLexicon(fmainlex);
    fclose(fmainlex);
    DocIndexor docidxor = readDocIndexor(config.DOC_INDEXOR);
    printLexicon(mainlex, printwldata);
    printf("Press enter to continue\n");
    getchar();

    DocData* doc = getDocDatabyDir(docidxor, "corpus/Tu them/doc3.txt");
    printf("docId = %d\n", doc->docId);
    removeFile(doc->docId, docidxor, mainlex, config);
    /*------------------------------------------------------------------------*/
    /*wprintf(L"config = %lf\n", config.THRESHOLD_TWO_STOP_WORD);
    lexicon swlex = mkstopwordLex(config.STOPWORD_FILES);
    _setmode(_fileno(stdout), _O_U16TEXT);
    wchar_t* text = readFile("corpus/doc7.txt");
    rmvRedundant(text);
    normalize(text);
    wprintf(L"Complete reading file\n");
    int n;
    SList tokens = splitword(text, L' ');
    WData* wordArray = makeWordArray3(tokens, n, swlex, config);
    wprintf(L"Press enter to print word array\n");
    getchar();
    printWordArray(wordArray, n);
    /*------------------------------------------------------------------------*/
/*
    short** posTable;
    posTable = new short* [4];
    posTable[0] = new short[5];
    posTable[1] = new short[4];
    posTable[2] = new short[2];
    posTable[3] = new short[4];
    posTable[0][0] = 5; posTable[0][1] = 6; posTable[0][2] = 7; posTable[0][3] = 11; posTable[0][4] = 12;
    posTable[1][0] = 1; posTable[1][1] = 8; posTable[1][2] = 13; posTable[1][3] = 14;
    posTable[2][0] = 2; posTable[2][1] = 10;
    posTable[3][0] = 3; posTable[3][1] = 4; posTable[3][2] = 9; posTable[3][3] = 15;
    short nposArr[] = { 5,4,2,4 };
    printf("Score = %f\n", computeProximityScore(posTable, nposArr, 4, false));
    /*------------------------------------------------------------------------*/


    /*------------------------------------------------------------------------*/
}