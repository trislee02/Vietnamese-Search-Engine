#include "Lexicon.h"

void init(lexicon& lex) {
    initialize(lex.main);
}

int cmptNode(void* a, void* b) {
    tnode* nodea = (tnode*)a;
    tnode* nodeb = (tnode*)b;
    if (nodea->chr == nodeb->chr) return 0;
    if (nodea->chr > nodeb->chr) return -1;
    return 1;
}

/*  Add a string of wchar_t to an existing lexicon 
 */
void addString(lexicon& lex, wchar_t* str, void* adapter) {
    SList* curL = &lex.main;    //current List dung de chi den cac list cua tnode, tnode dau tien xuat phat tu lexicon.main
    for (int i = 0; i < wcslen(str); i++) { //Di qua tung ky tu mot
        SList branch;
        initialize(branch);
        bool iw = false;
        tnode c = { str[i], iw, branch, adapter }; //Tao mot tnode tam de kiem tra
        SNode* t = findData(*curL, &c, cmptNode);   //Kiem tra ky tu co xuat hien tai list hien tai khong
        if (!t) { 
            //Neu khong tim thay ky tu -> phai them vao list hien tai
            if (i == wcslen(str) - 1) {   //Danh dau ket thuc mot tu
                iw = true;
                c = { str[i], iw, branch, adapter };
            }
            SNode* temp = addKeepOrder(*curL, &c, sizeof(tnode), cmptNode); //Them vao va giu nguyen thu tu tu dien
            if (temp) {
                tnode* b = (tnode*)temp->data;
                curL = &(b->branch); //Thay doi current List den list phia duoi
            }
        }
        else {
            //Neu tim thay ky tu -> di xuong list phia duoi
            tnode* b = (tnode*)t->data;
            if (i == wcslen(str) - 1) {  //Danh dau ket thuc mot tu
                b->isWord = true;
                b->adapter = adapter;
            }
            curL = &(b->branch);  //Thay doi current List den list phia duoi
        }
    }
}

/*Mo ta: Tim mot chuoi trong lexicon
  Tra ve: tnode cuoi cung*/
tnode* findString(lexicon lex, wchar_t* str) {
    SList* curL = &lex.main;    //current List dung de chi den cac list cua tnode, tnode dau tien xuat phat tu lexicon.main
    tnode* b = NULL;
    for (int i = 0; i < wcslen(str); i++) { //Di qua tung ky tu mot
        SList branch;
        initialize(branch);
        bool iw = false;
        tnode c = { str[i],iw, branch }; //Tao mot tnode tam de kiem tra
        SNode* t = findData(*curL, &c, cmptNode);   //Kiem tra ky tu co xuat hien tai list hien tai khong
        if (!t) {
            //Neu khong tim thay ky tu -> tra ve NULL vi khong tim thay
            return NULL;
        }
        else {
            //Neu tim thay ky tu -> di xuong list phia duoi
            b = (tnode*)(t->data);
            curL = &(b->branch);  //Thay doi current List den list phia duoi
        }
    }
    if (b->isWord)
        return b;
    return NULL;
}

void printWord(tnode* node, wchar_t* word, int nchar) {
    if (!node->isWord) return;
    word[nchar] = L'\0';
    wprintf(L"%ls\n", word);
}

void freeLexicon(lexicon& lex, void (*freeTnode)(void *)) {
    
}

void traverseLex(lexicon lex, void (*doTaskforWord)(wchar_t*, void*), void (*doTaskforAdapter)(void*)) {
    SNode* root = lex.main.head;
    tnode* charNode = NULL;
    //Temporary word that contains letter of each level.
    wchar_t word[100];
    for (; root; root = root->next) {
        //Initialize first char, increase nchar for deeper level (latter character)
        int nchar = 1;
        charNode = (tnode*)(root->data);
        word[nchar - 1] = charNode->chr;
        word[nchar] = L'\0';
        //If it is a word, then do task for that word
        if (charNode->isWord) {
            doTaskforWord(word, charNode);
            //If there is adapter, do task for adapter
            if (doTaskforAdapter && charNode->adapter) doTaskforAdapter(charNode->adapter);
        }

        //Go deep into each root if it has branch
        if (!isEmpty(((tnode*)(root->data))->branch)) {
            //Increase the number of character of word
            nchar++;
            //Use stack to store the parent node
            Stack stackn;
            init(stackn);
            SNode* cur = ((tnode*)(root->data))->branch.head;
            do {
                for (; cur; cur = cur->next) {
                    charNode = (tnode*)(cur->data);
                    word[nchar - 1] = charNode->chr;
                    word[nchar] = L'\0';
                    //If it is a word, then do task for that word
                    if (charNode->isWord) {
                        doTaskforWord(word, charNode);
                        //If there is adapter, do task for adapter
                        if (doTaskforAdapter && charNode->adapter) doTaskforAdapter(charNode->adapter);
                    }
                    //Check if there exist branch of the current node
                    tnode* temp = (tnode*)(cur->data);
                    if (!isEmpty(temp->branch)) {
                        //Increase the number of character of word
                        nchar++;
                        push(stackn, cur, sizeof(SNode));
                        cur = temp->branch.head;
                        break;
                    }
                }
                void* t;
                while (!cur && !isEmpty(stackn)) {
                    if (pop(stackn, t)) {
                        //Decrease the number of character of word
                        nchar--;
                        cur = (SNode*)t;
                        cur = cur->next;
                        free(t);
                    }
                }
            } while (cur);
        }
    }
}

void printLex1(lexicon lex, void (*printAdapter)(void*)) {
    SNode* root = lex.main.head;
    tnode* charNode = NULL;
    wchar_t word[100];
    for (; root; root = root->next) {
        int nchar = 1;
        charNode = (tnode*)(root->data);
        word[nchar - 1] = charNode->chr;
        printWord(charNode, word, nchar);
        if (charNode->isWord && printAdapter) printAdapter(charNode->adapter);
        //printf("%c: ", ((tnode*)(root->data))->chr);

        /*Go deep into each root*/
        if (!isEmpty(((tnode*)(root->data))->branch)) {
            nchar++; //Tang vi tri ky tu
            Stack stackn;
            init(stackn);
            SNode* cur = ((tnode*)(root->data))->branch.head;
            do {
                for (; cur; cur = cur->next) {
                    charNode = (tnode*)(cur->data);
                    word[nchar - 1] = charNode->chr;
                    printWord(charNode, word, nchar);
                    if (charNode->isWord && printAdapter) printAdapter(charNode->adapter);
                    //printf("%c ", ((tnode*)(cur->data))->chr);
                    tnode* temp = (tnode*)(cur->data);
                    if (!isEmpty(temp->branch)) {
                        nchar++; //Tang vi tri ky tu
                        //printf("- ");
                        push(stackn, cur, sizeof(SNode));
                        cur = temp->branch.head;
                        break;
                    }
                }
                void* t;
                while (!cur && !isEmpty(stackn)) {
                    if (pop(stackn, t)) {
                        //printf("| ");
                        nchar--; //Giam vi tri ky tu
                        cur = (SNode*)t;
                        cur = cur->next;
                    }
                }
            } while (cur);
        }
        //printf("\n");
    }
}

void printLex2(lexicon lex, void (*printAdapter)(void*)) {
    SNode* root = lex.main.head;
    tnode* charNode = NULL;
    wchar_t word[100];
    for (; root; root = root->next) {
        int nchar = 1;
        charNode = (tnode*)(root->data);
        word[nchar - 1] = charNode->chr;
        printWord(charNode, word, nchar);
        if (charNode->isWord && printAdapter) printAdapter(charNode->adapter);
        //printf("%c: ", ((tnode*)(root->data))->chr);

        /*Go deep into each root*/
        if (!isEmpty(((tnode*)(root->data))->branch)) {
            nchar++; //Tang vi tri ky tu
            Stack stackn;
            init(stackn);
            SNode* cur = ((tnode*)(root->data))->branch.head;
            do {
                for (; cur; cur = cur->next) {
                    charNode = (tnode*)(cur->data);
                    word[nchar - 1] = charNode->chr;
                    printWord(charNode, word, nchar);
                    if (charNode->isWord && printAdapter) printAdapter(charNode->adapter);
                    //printf("%c ", ((tnode*)(cur->data))->chr);
                    tnode* temp = (tnode*)(cur->data);
                    if (!isEmpty(temp->branch)) {
                        nchar++; //Tang vi tri ky tu
                        //printf("- ");
                        push(stackn, cur, sizeof(SNode));
                        cur = temp->branch.head;
                        break;
                    }
                }
                void* t;
                while (!cur && !isEmpty(stackn)) {
                    if (pop(stackn, t)) {
                        //printf("| ");
                        nchar--; //Giam vi tri ky tu
                        cur = (SNode*)t;
                        cur = cur->next;
                        free(t);
                    }
                }
            } while (cur);
        }
        //printf("\n");
    }
}

void printLexWord(wchar_t* word, void* node) {
    tnode* tn = (tnode*)node;
    wprintf(L"%ls\n", word);
}

void printLex3(lexicon lex, void (*printAdapter)(void*)) {
    traverseLex(lex, printLexWord, printAdapter);
}

void printLexicon(lexicon lex, void (*printAdapter)(void*)) {
    printLex3(lex, printAdapter);
}