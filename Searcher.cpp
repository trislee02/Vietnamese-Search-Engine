#include "Searcher.h"

/*	Read the lexicon from opened lexicon file
*	Return a lexicon and the adapter of tnode will be wldata
*/
lexicon readLexicon(FILE* f) {
	lexicon lex;
	init(lex);
	int nchar = 0;
	int dlistAdd = 0;
	int ndoc = 0;
	wchar_t word[100];
	int bufCount = 0;
	//Read the file in turn: nchar - word - dlistadd - ndoc
	while ((bufCount = fread(&nchar, sizeof(short), 1, f)) > 0) {
		//Read the word and including '\0' to the word
		fread(word, sizeof(wchar_t), nchar + 1, f); 
		fread(&dlistAdd, sizeof(int), 1, f);
		fread(&ndoc, sizeof(int), 1, f);
		WLData* wldata = new WLData;
		wldata->nchar = nchar;
		wldata->dlistAdd = dlistAdd;
		wldata->ndoc = ndoc;
		wldata->word = new wchar_t[nchar + 1];
		wcscpy(wldata->word, word);
		addString(lex, word, wldata);
	}
	return lex;
}

WLData* getWLData(lexicon lex, wchar_t* word) {
	tnode* charNode = NULL;
	if ((charNode = findString(lex, word))) {
		WLData* wldata = (WLData*)charNode->adapter;
		return wldata;
	}
	return NULL;
}

/*	Get the doclist in barrel indexor thanks to wldata
*	Return a doclist
*/
Doclist* getDoclist(FILE* fbarrelidxor, WLData wldata) {
	Doclist* doclist = new Doclist;
	doclist->ndoc = wldata.ndoc;
	doclist->docArray = new WDData[wldata.ndoc];
	fseek(fbarrelidxor, wldata.dlistAdd, SEEK_SET);
	fread(doclist->docArray, sizeof(WDData), wldata.ndoc, fbarrelidxor);
	fseek(fbarrelidxor, 0, SEEK_END);
	return doclist;
}

/*	Find in doclist the WDData by docId
*	Return the index of that WDData; negative number if it cannot be found
*/
int getIndexWDData(Doclist doclist, int docId) {
	for (int i = 0; i < doclist.ndoc; i++) if (doclist.docArray[i].docId == docId) return i;
	return -1;
}

/*	Get all the info of the doc base on docAdd
*	Return a word array of that doc
*/
WData* getWordArray(FILE* fbarrel, int nword, int docAdd) {
	WData* wordArray = new WData[nword];
	if (nword < 1 || docAdd < 0) return NULL;
	fseek(fbarrel, docAdd, SEEK_SET);
	for (int i = 0; i < nword; i++) {
		int nchar = 0;
		float tf = 0.0;
		fread(&nchar, sizeof(int), 1, fbarrel);
		wordArray[i].word = new wchar_t[nchar + 1];
		fread(wordArray[i].word, sizeof(wchar_t), nchar + 1, fbarrel);
		fread(&wordArray[i].tf, sizeof(float), 1, fbarrel);
	}
	return wordArray;
}

/*	This function will return the info of word.	It will get the info directly from barrel
*	wordAdd must be the real word address which equals wordAdd + docAdd
*	Return wdata
*/
WData getWordData(FILE* fbarrel, int wordAdd) {
	WData wdata;
	//Get tf info, then skip the nchar and word 
	fseek(fbarrel, wordAdd, SEEK_SET);
	int nchar = 0;
	fread(&nchar, sizeof(nchar), 1, fbarrel);
	wdata.word = new wchar_t[nchar + 1];
	fread(wdata.word, sizeof(wchar_t), nchar + 1, fbarrel);
	fread(&wdata.tf, sizeof(wdata.tf), 1, fbarrel);
	return wdata;
}

/*	Add a word to QDocList
*/
void addDoc2QDocList(SList& QDoclist, WDData wddata) {
	int docId = wddata.docId;
	SNode* node = findData(QDoclist, &docId, cmpSDDataDocId);
	if (node) {
		SDData* sddata = (SDData*)node->data;
		if (sddata->nword % 10 == 0)
			sddata->wordAddArray = (WDData*)realloc(sddata->wordAddArray, sizeof(WDData) * (sddata->nword + 10));
		sddata->wordAddArray[sddata->nword] = wddata;
		sddata->nword++;
	}
	else {
		//In default, the score of the doc is zero
		SDData* sddata = new SDData;
		sddata->docId = docId;
		sddata->nword = 1;
		sddata->score = 0;
		sddata->wordAddArray = new WDData[10];
		sddata->wordAddArray[0] = wddata;
		//Add a new doc to QDocList and keep ascendant  by docId
		addKeepOrder(QDoclist, sddata, sizeof(SDData), cmpSDDataDocId);
	}
	
}

/*	Search a query
*	Return QDocList
*/
SList searchAQuery(lexicon mainlex, FILE* fbarrelidxor, FILE* fbarrel, SList token) {
	SList QDocList; //Data is SDData
	initialize(QDocList);
	if (isEmpty(token)) return QDocList;
	SNode* cur = token.head; 
	for (; cur; cur = cur->next) {
		wchar_t* word = (wchar_t*)cur->data;
		tnode* tn = findString(mainlex, word);
		if (tn) {
			Doclist* doclist = getDoclist(fbarrelidxor, *(WLData*)tn->adapter);
			//printDoclist(doclist);
			for (int i = 0; i < doclist->ndoc; i++) {
				addDoc2QDocList(QDocList, (doclist->docArray)[i]);
			}
			freeDoclist(doclist);
		}
	}
	return QDocList;
}

/*	Get the docindexor from a file name
*	Return an array of docData called docArray and update ndocs argument
*/
DocIndexor readDocIndexor(const char* didxorfname) {
	FILE* fdocidxor = fopen(didxorfname, "rb");
	DocData* docArray = (DocData*)malloc(sizeof(DocData) * 1000);
	int ndocs = 0;
	int docId = 0, ndir = 0, nword = 0, docAdd = 0;
	char* docdir = NULL;
	//Read the doc indexor in turn: docId - ndirchar - docdir - nword - docAdd 
	while (fread(&docId, sizeof(int), 1, fdocidxor) > 0) {
		fread(&ndir, sizeof(int), 1, fdocidxor);
		docdir = new char[ndir + 1];
		fread(docdir, sizeof(char), ndir + 1, fdocidxor);
		fread(&nword, sizeof(int), 1, fdocidxor);
		fread(&docAdd, sizeof(int), 1, fdocidxor);
		docArray[ndocs].docId = docId;
		docArray[ndocs].nchardir = ndir;
		docArray[ndocs].docdir = docdir;
		docArray[ndocs].nword = nword;
		docArray[ndocs].docAdd = docAdd;
		ndocs++;
		if (ndocs % 1000 == 0) docArray = (DocData*) realloc(docArray, (ndocs + 1000) * sizeof(DocData));
	}
	docArray = (DocData*)realloc(docArray, ndocs * sizeof(DocData));
	DocIndexor docidxor = { ndocs, docArray };
	fclose(fdocidxor);
	return docidxor;
}

/*	Find DocData in docArray by docId
*	Return docId and update docdata argument if it has been found; negative number if it can't be found
*/
int getDocDatabyId(DocData* docArray, int ndocs, DocData& docdata, int docId) {
	for (int i = 0; i < ndocs; i++) {
		if (docArray[i].docId == docId) {
			docdata = docArray[i];
			return docId;
		}
	}
	return -1;
}

DocData* getDocDatabyDir(DocIndexor docidxor, const char* dir) {
	for (int i = 0; i < docidxor.ndocs; i++) if (strcmp(docidxor.docarray[i].docdir, dir) == 0) return &docidxor.docarray[i];
	return NULL;
}

/*	Find DocData in docArray by docId
*	Return DocData* of that docData
*/
DocData* getDocDatabyId(DocData* docArray, int ndocs, int docId) {
	for (int i = 0; i < ndocs; i++) {
		if (docArray[i].docId == docId) return &docArray[i];
	}
	return NULL;
}

/*	Calculate the score for QDocList.
*	Update the QDoclist.
*/
void calculateScore(FILE* fbarrel, const char* docidxorfname, SList& QDocList) {
	if (isEmpty(QDocList)) return;
	SNode* cur = QDocList.head;
	DocIndexor docidxor = readDocIndexor(docidxorfname);
	int ndocs = docidxor.ndocs;
	DocData* docArray = docidxor.docarray;
	if (!docArray) return;
	for (; cur; cur = cur->next) {
		SDData* ddata = (SDData*)cur->data;
		float score = 0.0;
		for (int i = 0; i < ddata->nword; i++) {
			int docAdd = 0, realwordAdd = 0;
			DocData docdata;
			if (getDocDatabyId(docArray, ndocs, docdata, ddata->docId) < 0) continue;
			docAdd = docdata.docAdd;
			//To get info of word from barrel, we need docAdd + wordAdd
			realwordAdd = ddata->wordAddArray[i].wordAdd + docAdd;
			WData wdata = getWordData(fbarrel, realwordAdd);
			score += wdata.tf;
			free(wdata.word);
		}
		ddata->score = score;
	}
	freeDocArray(docArray, ndocs);
}

void rankDoc(SList& QDocList) { 
	//Base on score of tf only
	mergesort(QDocList, cmpSDDataScore, -1);
}

void freeSDData(void* a) {
	SDData* ddata = (SDData*)a;
	free(ddata->wordAddArray);
	free(ddata);
}

void filterDoc(SList& QDocList, int k) {
	filterKfirst(QDocList, k, freeSDData);
}

int getDocId(void* sddata) {
	if (!sddata) return -1;
	SDData* docdata = (SDData*)sddata;
	return docdata->docId;
}

float getDocScore(void* sddata) {
	if (!sddata) return -1;
	SDData* docdata = (SDData*)sddata;
	return docdata->score;
}

SList doSearching(lexicon mainlex, SList tokens, int k) {

	FILE* fmainlex = fopen(LEXICON_FILES, "rb");
	FILE* fbarrelindexor = fopen(BARRELS_INDEXOR_FILES, "rb");
	FILE* fbarrel = fopen(BARRELS_FILES, "rb");

	SList QDoclist = searchAQuery(mainlex, fbarrelindexor, fbarrel, tokens);
	calculateScore(fbarrel, DOC_INDEXOR, QDoclist);
	rankDoc(QDoclist);
	filterDoc(QDoclist, k);

	fclose(fmainlex);
	fclose(fbarrelindexor);
	fclose(fbarrel);
	return QDoclist;
}