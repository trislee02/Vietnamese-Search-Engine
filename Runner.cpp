#include "Runner.h"
/*Library to output unicode text to screen*/
#include <fcntl.h>
#include <io.h>

void sayGoodbye() {
    wprintf(L"***************************************************\n");
    wprintf(L"*                                                 *\n");
    wprintf(L"*  VNSE SEARCH ENGINE - Deliver the best quality  *\n");
    wprintf(L"*            Goodbye - See you soon!              *\n");
    wprintf(L"*                                                 *\n");
    wprintf(L"***************************************************\n");
}

void saySorry() {
    wprintf(L"************************************************************\n");
    wprintf(L"*       VNSE SEARCH ENGINE - Deliver the best quality      *\n");
    wprintf(L"*              Sorry for our long time response            *\n");
    wprintf(L"*  You may get better performance for meaningful keywords  *\n");
    wprintf(L"************************************************************\n");
}

void init(lexicon& lex, DocIndexor& docidxor, Config& config) {
    config = getConfigData(CONFIG_FILE);
    if (!config.INDEX_FILES) return;
    lex = readLexicon(config.LEXICON_FILES);
    docidxor = readDocIndexor(config.DOC_INDEXOR);
}

int Menu()
{
    int choice = 0;
    wprintf(L"\n\n***************  VNSE SEARCH ENGINE ***************\n");
    wprintf(L"*                      MENU                       *\n");
    wprintf(L"***************************************************\n");
    wprintf(L"* 1    *  Index                                   *\n");
    wprintf(L"* 2    *  Search                                  *\n");
    wprintf(L"* 3    *  Modify corpus                           *\n");
    wprintf(L"* 4    *  Refresh config                          *\n");
    wprintf(L"* 5    *  Exit                                    *\n");
    wprintf(L"***************************************************\n");
    wprintf(L"Your option: ");
    wscanf(L"%d", &choice);
    return choice;
}

void Indexing(lexicon& mainlex, Config config)
{
    wchar_t c;
    do {
        wscanf(L"%c", &c);
        wprintf(L"WARNING: Indexing process will takes a long time to complete.\nIt may affect the performance\n");
        wprintf(L"Do you really want to index the corpus? (Y/N): ");
        wscanf(L"%c", &c);
    } while (c != L'Y' && c != L'y' && c != L'n' && c != L'N');
    if (c == L'N' || c == L'n') return;
    wprintf(L"Indexing...\n");
    int res = doIndexing(mainlex, config);
    if (res == 0) wprintf(L"%ls", SUCCESS_MESSAGE);
    else wprintf(L"%ls",FAIL_MESSAGE);
}

void Search(lexicon mainlex, DocIndexor docidxor, Config config)
{
    getchar();
    wprintf(L"========================= VNSE Search Engine =========================\n");
    wprintf(L"     The more meaningful keywords are, the faster the response is\n");
    wprintf(L"======================================================================\n");
    int x, k;
    SList tokens;
    initialize(tokens);
    wchar_t* strquery = new wchar_t[1000];

    wprintf(L"Your query: ");
    fgetws(strquery, 1000, stdin);
    
    rmvRedundant(strquery);
    normalize(strquery);
    
    tokens = splitword(strquery, L' ');
    clock_t begin = clock();
    wprintf(L"Searching...\n");
    SList QDocList = doSearching(mainlex, tokens, config);
    double timespent = (double)(clock() - begin) / CLOCKS_PER_SEC;
    wprintf(L"Time spent searching = %lf\n", timespent);
    if (timespent > 3) saySorry();
    

    if (isEmpty(QDocList)) {
        wprintf(L"No files was found\n");
        return;
    }

    char** resFilename = new char*[QDocList.size];
    
    int filei = 0;
    wprintf(L"Top %d files:\n", config.K_BEST_DOC_DEFAULT);
    for (SNode* docNode = QDocList.head; docNode; docNode = docNode->next) {
        SDData* sddata = (SDData*)docNode->data;
        DocData docdata = *getDocDatabyId(docidxor.docarray, docidxor.ndocs, sddata->docId);
        wchar_t* temp = chr2wchr(docdata.docdir);
        wprintf(L"%d.\t[%f] %s\n", filei + 1, sddata->score, temp);
        free(temp);
        resFilename[filei] = new char[strlen(docdata.docdir) + 1];
        strcpy(resFilename[filei], docdata.docdir);
        filei++;
    }
    
    do {
        do {
            wprintf(L"Select one of the following options:\n\t1. Read a document\n\t2. Back to menu\nYour option: ");
            wscanf(L"%d", &k);
        } while (k != 1 && k != 2);

        if (k == 1) {
            do {
                wprintf(L"=================================================================\n");
                wprintf(L"List of file:\n");
                for (int i = 0; i < filei; i++)
                {
                    wchar_t* temp = chr2wchr(resFilename[i]);
                    wprintf(L"%d.\t%s\n", i + 1, temp);
                    free(temp);
                }
                wprintf(L"=================================================================\n");
                wprintf(L"Choose a document: ");
                wscanf(L"%d", &x);
            } while (x < 1 || x > filei);

            wchar_t* wc = readFile(resFilename[x - 1]);
            if (!wc) {
                wprintf(L"Invalid file\n");
                continue;
            }
            else {
                wprintf(L"*****************************************************************\n");
                wchar_t* temp = chr2wchr(resFilename[x - 1]);
                wprintf(L"File name = %s\n", temp);
                free(temp);
                wprintf(L"%ls\n", wc);
                wprintf(L"*****************************************************************\n");
            }
        }
    } while (k == 1);        //If the user choose to continue reading
    for (int i = 0; i < filei; i++) free(resFilename[i]);
    free(resFilename);
    removeAll(QDocList, freeSDData);
}

int addFile2Corpus(lexicon& mainlex, DocIndexor& docidxor, Config config) {
    wchar_t c;
    do {
        wscanf(L"%c", &c);
        wprintf(L"WARNING: Adding new file process will takes a few seconds to complete.\nIt may affect your current existing corpus metadata\n");
        wprintf(L"Do you really want to index new file? (Y/N): ");
        wscanf(L"%c", &c);
    } while (c != L'Y' && c != L'y' && c != L'n' && c != L'N');
    wscanf(L"%c", &c);
    if (c == L'N' || c == L'n') return -1;
    wchar_t fnamewchr[1000];
    wprintf(L"Your new file: ");
    fgetws(fnamewchr, 1000, stdin);
    if (fnamewchr[wcslen(fnamewchr) - 1] == L'\n') fnamewchr[wcslen(fnamewchr) - 1] = L'\0';
    char fnamechr[1000];
    int ret = wcstombs(fnamechr, fnamewchr, sizeof(fnamechr));
    int res = indexNewFile(fnamechr, docidxor, mainlex, config);
    if (res == 0) wprintf(L"Successfully index new file\n");
    else wprintf(L"Fail to index new file\n");
}

int rmvFileCorpus(lexicon& mainlex, DocIndexor& docidxor, Config config) {
    wchar_t c;
    do {
        wscanf(L"%c", &c);
        wprintf(L"WARNING: Removing a file process will takes a few seconds to complete.\nIt may affect your current existing corpus metadata\n");
        wprintf(L"Do you really want to remove a file? (Y/N): ");
        wscanf(L"%c", &c);
    } while (c != L'Y' && c != L'y' && c != L'n' && c != L'N');
    if (c == L'N' || c == L'n') return -1;
    wscanf(L"%c", &c);
    wchar_t fnamewchr[1000];
    wprintf(L"Your file name: ");
    fgetws(fnamewchr, 1000, stdin);
    if (fnamewchr[wcslen(fnamewchr) - 1] == L'\n') fnamewchr[wcslen(fnamewchr) - 1] = L'\0';
    char fnamechr[1000];
    int ret = wcstombs(fnamechr, fnamewchr, sizeof(fnamechr));
    DocData* doc = getDocDatabyDir(docidxor, fnamechr);
    if (!doc) wprintf(L"Oops, file not found\n");
    else {
        int res = removeFile(doc->docId, docidxor, mainlex, config);
        if (res == 0) wprintf(L"Successfully index new file\n");
        else wprintf(L"Fail to index new file\n");
    }
}

void ModifyCorpus(lexicon& mainlex, DocIndexor& docidxor, Config config) {
    int choice = 0;
    do {
        do {
            wprintf(L"\n=============  Modify corpus  =============\n");
            wprintf(L"\t1. Index new file\n");
            wprintf(L"\t2. Remove a file\n");
            wprintf(L"\t3. Back to menu\n");
            wprintf(L"Your option: ");
            wscanf(L"%d", &choice);
        } while (choice < 1 || choice > 3);
        if (choice == 1) addFile2Corpus(mainlex, docidxor, config);
        else if (choice == 2) rmvFileCorpus(mainlex, docidxor, config);
        else if (choice == 3) break;
    } while (choice > 0 && choice < 4);
}

void refreshConfig(Config& config) {
    config = getConfigData(CONFIG_FILE);
    wprintf(L"Completed refresh config\n");
    wprintf(L"---------- Current configure ----------\n");
    wprintf(L"K best document             %d\n", config.K_BEST_DOC_DEFAULT);
    wprintf(L"Threshold stop word         %f\n", config.THRESHOLD_STOP_WORD);
    wprintf(L"Threshold two stop words    %f\n", config.THRESHOLD_TWO_STOP_WORD);
    wprintf(L"Distance minimal interval   %d\n", config.DISTANCE_MINIMAL_INTERVAL);
    wprintf(L"Distance ordered pair       %d\n", config.DISTANCE_ORDERED_PAIR);
    wprintf(L"Exponential matched token   %d\n", config.EXPONENTIAL_MATCHED_TOKEN);
    wprintf(L"Exponential order pair      %d\n", config.EXPONENTIAL_ORDER_PAIR);
    wprintf(L"Weight distance interval    %f\n", config.WEIGHT_DISTANCE_MINIMAL_INTERVAL);
    wprintf(L"---------------------------------------\n");
}

void Run()
{
    int choice = 0;
    lexicon mainlex;
    DocIndexor docidxor;
    Config config;
    _setmode(_fileno(stdout), _O_U16TEXT);
    
    init(mainlex, docidxor, config);

    do {
        choice = Menu();
        if (choice == 1) Indexing(mainlex, config);
        else if (choice == 2) Search(mainlex, docidxor, config);
        else if (choice == 3) ModifyCorpus(mainlex, docidxor, config);
        else if (choice == 4) refreshConfig(config);
        else if (choice == 5) {
            sayGoodbye();
            break;
        }
    } while (choice > 0 && choice < 6);
}