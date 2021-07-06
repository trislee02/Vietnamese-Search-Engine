#include "Menu.h"
/*Lib cho xuat unicode ra man hinh*/
#include <fcntl.h>
#include <io.h>

void init(lexicon& mainlex, int& kBestDoc, char**& listfile, int& nfiles) {
    FILE* fmainlex = fopen("mainlex.txt", "rb");
    mainlex = readLexicon(fmainlex);
    //printLexicon(mainlex, printwldata);
    fclose(fmainlex);

    kBestDoc = K_BEST_DOC_DEFAULT;
    FILE* findex = fopen("index.txt", "r");
    listfile = readCorpus(findex, nfiles);
    fclose(findex);
}

void Menu()
{
    wprintf(L"***************************************************\n");
    wprintf(L"*                      MENU                       *\n");
    wprintf(L"***************************************************\n");
    wprintf(L"* 1    *  Indexing                                *\n");
    wprintf(L"* 2    *  Search                                  *\n");
    wprintf(L"* 3    *  Change the number of doc is displayed   *\n");
    wprintf(L"* 4    *  Add file to corpus (coming soon)        *\n");
    wprintf(L"* 5    *  Remove file from corpus (coming soon)   *\n");
    wprintf(L"* 6    *  Exit                                    *\n");
    wprintf(L"***************************************************\n");
}

/*Return a list file in corpus*/
char** Indexing(lexicon& mainlex)
{
    wchar_t c;
    do {
        wprintf(L"Ban co muon thuc hien index khong? (Y/N): ");
        wscanf(L"%c", &c);
    } while (c != L'Y' && c != L'y' && c != L'n' && c != L'N');
    if (c == L'N' || c == L'n') return NULL;
    wprintf(L"Indexing...\n");
    char** listfile = NULL;
    int res = doIndexing(mainlex, listfile);
    if (res == 0) wprintf(L"%ls", SUCCESS_MESSAGE);
    else wprintf(L"%ls",FAIL_MESSAGE);
    return listfile;
}

void Search(lexicon mainlex, int kBestDoc, char** listfile)
{
    getchar();
    wprintf(L"===================Search Engine SE===================\n");

    int x, k;
    SList tokens;
    initialize(tokens);
    wchar_t* strquery = new wchar_t[80];

    wprintf(L"Moi nhap thu ban can tim: ");
    fgetws(strquery, 80, stdin);
    
    rmvRedundant(strquery);
    normalize(strquery);
    
    tokens = splitword(strquery, L' ');
    SList QDocList = doSearching(mainlex, tokens, kBestDoc);
    
    if (isEmpty(QDocList)) {
        wprintf(L"Khong tim thay file phu hop\n");
        return;
    }
    char** resFilename = new char*[QDocList.size];
    
    int filei = 0;
    SNode* docNode;
    wprintf(L"Danh sach file tim kiem duoc:\n");
    for (docNode = QDocList.head; docNode; docNode = docNode->next) {
        SDData* ddata = (SDData*)docNode->data;
        resFilename[filei] = listfile[getDocId(ddata)];
        wchar_t* temp = chr2wchr(resFilename[filei]);
        wprintf(L"%d.\t[%f] %s\n", filei + 1, getDocScore(ddata), temp);
        free(temp);
        filei++;
    }
    
    do {
        do {
            wprintf(L"Ban muon doc hay quay lai Menu ban dau: \n1. Doc\n2. Quay lai\nMoi ban nhap lua chon: ");
            wscanf(L"%d", &k);
        } while (k != 1 && k != 2);

        if (k == 1) {
            wprintf(L"Danh sach cac file:\n");
            for (int i = 0; i < filei; i++)
            {
                wchar_t* temp = chr2wchr(resFilename[i]);
                wprintf(L"%d.\t%s\n",i + 1, temp);
                free(temp);
            }
            wprintf(L"Ban muon xem file nao: ");
            wscanf(L"%d", &x);

            wchar_t* wc = readFile(resFilename[x - 1]);
            if (!wc) {
                wprintf(L"File khong ton tai.\n");
                continue;
            }
            else {
                wprintf(L"*****************************************************************\n");
                wprintf(L"%ls\n", wc);
                wprintf(L"*****************************************************************\n");
            }
        }
    } while (k == 1);        // nếu người đọc chọn đọc sẽ hiện lại

}

bool ParaSetting()
{
    int x, k;
    wprintf(L"Parameter Setting:  \n");
    wprintf(L"Ban co muon thay doi thong so k hay khong:\n1. Co\n2.Khong\nMoi ban lua chon:  ");
    wscanf(L"%d", &x);
    if (x == 0) return false;
    else {
        wprintf(L"Moi ban nhap vao thong so k moi:  ");
        wscanf(L"%d", &k);
    }
}

void Run()
{
    int chose;
    lexicon mainlex;
    int kBestDoc, nfiles;
    char** listfile = NULL;
    _setmode(_fileno(stdout), _O_U16TEXT);

    init(mainlex, kBestDoc, listfile, nfiles);
    do {
        Menu();
        wprintf(L"Moi ban nhap lua chon: ");
        wscanf(L"%d", &chose);
        if (chose == 1) {
            char** temp = NULL;
            temp = Indexing(mainlex);
            if (temp) {
                for (int i = 0; i < nfiles; i++)
                    free(listfile[i]);
                free(listfile);
                listfile = temp;
            }
        }
        else if (chose == 2) Search(mainlex, kBestDoc, listfile);
        //else if (chose == 3) ParaSetting();
        // ...
        else if (chose == 6) break;
    } while (chose > 0 && chose < 7);
}