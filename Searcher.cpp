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

/*	Read the lexicon from file name
*	Return a lexicon and the adapter of tnode will be wldata
*/
lexicon readLexicon(const char* lexfname) {
	lexicon lex;
	init(lex);
	FILE* flex = fopen(lexfname, "rb");
	if (!flex) return lex;
	lex = readLexicon(flex);
	fclose(flex);
	return lex;
}

/*	Get WLData of a word in a lexicon
*	Return a pointer to that wldata
*/
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
		fread(&wordArray[i].npos, sizeof(short), 1, fbarrel);
		wordArray[i].posarray = new short[wordArray[i].npos];
		fread(wordArray[i].posarray, sizeof(short), wordArray[i].npos, fbarrel);
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
	fread(&wdata.npos, sizeof(short), 1, fbarrel);
	wdata.posarray = new short[wdata.npos];
	fread(wdata.posarray, sizeof(short), wdata.npos, fbarrel);
	return wdata;
}

/*	Add a word in a doc to QDocList
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
		SDData sddata;
		sddata.docId = docId;
		sddata.nword = 1;
		sddata.score = 0;
		sddata.wordAddArray = new WDData[10];
		sddata.wordAddArray[0] = wddata;
		//Add a new doc to QDocList and keep ascendant by docId
		addHead(QDoclist, &sddata, sizeof(SDData));
	}
}

/*	Write searching log to file
*/
void writeSearchingLog(const char* logfname, const char* docidxorfname, FILE* fbarrel, SList QDocList) {
	if (isEmpty(QDocList)) return;
	FILE* flog = fopen(logfname, "w");
	DocIndexor docidxor = readDocIndexor(docidxorfname);
	int ndocs = docidxor.ndocs;
	DocData* docArray = docidxor.docarray;
	if (!docArray) return;
	fprintf(flog, "Found %d files\n", QDocList.size);
	for (SNode* cur = QDocList.head; cur; cur = cur->next) {
		SDData* sddata = (SDData*)cur->data;
		DocData docdata = *getDocDatabyId(docidxor.docarray, docidxor.ndocs, sddata->docId);
		fprintf(flog, "\n", sddata->docId);
		fprintf(flog, "%s, %f\n", docdata.docdir, sddata->score);
		for (int i = 0; i < sddata->nword; i++) {
			int docAdd = 0, realwordAdd = 0;
			DocData docdata;
			if (getDocDatabyId(docArray, ndocs, docdata, sddata->docId) < 0) continue;
			docAdd = docdata.docAdd;
			//To get info of word from barrel, we need docAdd + wordAdd
			realwordAdd = sddata->wordAddArray[i].wordAdd + docAdd;
			WData wdata = getWordData(fbarrel, realwordAdd);
			fprintf(flog, "%ls-tf:%lf\n", wdata.word, wdata.tf);
			for (int i = 0; i < wdata.npos; i++) fprintf(flog, "%d ", wdata.posarray[i]);
			fprintf(flog, "\n");
			free(wdata.word);
			free(wdata.posarray);
		}
	}

	freeDocArray(docArray, ndocs);
	fclose(flog);
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

/*	Find and return a pointer to docData of a docArray in a DocIndexor
*/
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

/*	Copy posArray of WData
*/
void* copyPosArray(WData wdata, int &npos) {
	npos = wdata.npos;
	short* posarray = new short[npos];
	memcpy(posarray, wdata.posarray, sizeof(short) * npos);
	return posarray;
}

/*	Get the keywordNo of a Element Minimal Interval
*/
int getkeywordNo(SNode* eleMinInterval) {
	return ((EleMinInterval*)eleMinInterval->data)->keywordNo;
}

/*	Get the keywordPos of a Element Minimal Interval
*/
short getKeyWordPos(SNode* eleMinInterval) {
	return ((EleMinInterval*)eleMinInterval->data)->position;
}

/*	Evaluate the minimal interval by the formula
*	- Let matchedTokenScore is ([#matched keywords] / [#query keywords])^[Exponential for matched token]
*	- Let orderPairsCount is [#ordered pair] in which the subtraction of two consecutive word in the interval must be larger the zero and smaller than [Distance for ordered pair]
*	- Let orderPairScore is [orderPairsCount]^[Exponential order pair]
*	- Let distanceScore equals 0 if leftmost tokens is so far from rightmost tokens; or equals ([#matched tokens] - 1) / (rightmost position - leftmost position)
*	The proximity score for a minimal interval is [matchedTokenScore] + [orderPairScore] + [weight]*[distanceScore]
*/
float evalMinInterval(SList interval, int kQuerywords, Config config) {
	float proximityScore = 0;
	EleMinInterval left = *(EleMinInterval*)interval.head->data;
	EleMinInterval right = *(EleMinInterval*)findTail(interval)->data;
	int orderedPairCount = 0;
	for (SNode* cur = interval.head; cur->next; cur = cur->next) {
		EleMinInterval word1 = *(EleMinInterval*)cur->data;
		EleMinInterval word2 = *(EleMinInterval*)cur->next->data;
		if (word2.keywordNo - word1.keywordNo == 1) {
			int distance = word2.position - word1.position - 1;
			if (distance >= 0 && distance <= config.DISTANCE_ORDERED_PAIR)
				orderedPairCount++;
		}
	}
	float matchedTokenScore = pow(interval.size * 1.0 / kQuerywords, config.EXPONENTIAL_MATCHED_TOKEN);
	float orderPairScore = pow(orderedPairCount, config.EXPONENTIAL_ORDER_PAIR);
	float distanceScore = 0;
	if (right.position - left.position <= interval.size - 1 + config.DISTANCE_MINIMAL_INTERVAL) {
		distanceScore = (interval.size - 1) * 1.0 / (right.position - left.position);
	}
	proximityScore = matchedTokenScore + orderPairScore + config.WEIGHT_DISTANCE_MINIMAL_INTERVAL * distanceScore;
	return proximityScore;
}

/*	Get the position of top element of a list of a token
*/
short getTopElement(short** posTable, short* nposArr, short* indexposArr, int keywordNo) {
	short* keywordPosList = posTable[keywordNo];
	short* topEle = &indexposArr[keywordNo];
	short pos = keywordPosList[*topEle];
	(*topEle)++;
	return pos;
}

/*  Implement algorithm for k-Word proximity search
*   Positions list of every keywords should be sorted before calling this function
*	[Proximity score for a doc] = Average of total [Proximity score for a interval]
*/
float computeProximityScore(short** posTable, short* nposArr, int kKeywords, int kQueryWords, Config config) {
	if (kKeywords < 2) return 0;
	float proximityScore = 0.0;
	EleMinInterval left, right;
	SList interval;
	initialize(interval);
	short* indexposArr = new short[kKeywords];
	for (int i = 0; i < kKeywords; i++) indexposArr[i] = 0;
	//Pop top elements of each list
	for (int i = 0; i < kKeywords; i++) {
		if (indexposArr[i] >= nposArr[i]) return -1;
		short pos = getTopElement(posTable, nposArr, indexposArr, i);
		EleMinInterval keyword = { i, pos };
		addKeepOrder(interval, &keyword, sizeof(EleMinInterval), cmpEleMinInterval);
	}
	int minIntervalCount = 1;
	left = *(EleMinInterval*)interval.head->data;
	right = *(EleMinInterval*)findTail(interval)->data;
	//Check for current list of the leftmost keyword in P, where P is the invertal list, is empty
	while (indexposArr[left.keywordNo] < nposArr[left.keywordNo]) {
		short pos = getTopElement(posTable, nposArr, indexposArr, left.keywordNo);
		EleMinInterval p = { left.keywordNo, pos };
		EleMinInterval q = { getkeywordNo(interval.head->next), getKeyWordPos(interval.head->next) };
		//If true, it is minimal interval, else it is not minimal interval
		if (p.position > right.position) {
			proximityScore += evalMinInterval(interval, kQueryWords, config);
			removeHead(interval, free);
			addTail(interval, &p, sizeof(EleMinInterval));
			minIntervalCount++;
		}
		else {
			removeHead(interval, free);
			addKeepOrder(interval, &p, sizeof(EleMinInterval), cmpEleMinInterval);
		}
		left = *(EleMinInterval*)interval.head->data;
		right = *(EleMinInterval*)findTail(interval)->data;
	}
	proximityScore += evalMinInterval(interval, kQueryWords, config);
	proximityScore /= minIntervalCount;
	removeAll(interval, free);
	free(indexposArr);
	return proximityScore;
}

/*	Compute the score for each doc in QDoclist - Method 1
*	Base on tf only
*/
void computeScore1(FILE* fbarrel, const char* docidxorfname, SList& QDocList) {
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

/*	Compute the score for each doc in QDoclist - Method 2
*	Base on tf and proximity
*	[Final score of a doc] = (Total of [TF]) * [Proximity for a doc] 
*/
void computeScore2(FILE* fbarrel, const char* docidxorfname, SList& QDocList, int nQueryKeyword, Config config) {
	if (isEmpty(QDocList)) return;
	DocIndexor docidxor = readDocIndexor(docidxorfname);
	int ndocs = docidxor.ndocs;
	DocData* docArray = docidxor.docarray;
	if (!docArray) return;
	for (SNode* cur = QDocList.head; cur; cur = cur->next) {
		SDData* sddata = (SDData*)cur->data;
		float score = 0.0;
		float tfscore = 0.0;
		short** posTable = new short* [sddata->nword];
		short* nposArr = new short[sddata->nword];
		for (int i = 0; i < sddata->nword; i++) {
			int docAdd = 0, realwordAdd = 0;
			DocData docdata;
			if (getDocDatabyId(docArray, ndocs, docdata, sddata->docId) < 0) continue;
			docAdd = docdata.docAdd;
			//To get info of word from barrel, we need docAdd + wordAdd
			realwordAdd = sddata->wordAddArray[i].wordAdd + docAdd;
			WData wdata = getWordData(fbarrel, realwordAdd);
			short* curTokenPosArray = wdata.posarray;
			posTable[i] = new short[wdata.npos];
			nposArr[i] = wdata.npos;
			memcpy(posTable[i], curTokenPosArray, sizeof(short) * wdata.npos);
			tfscore += wdata.tf;
			free(wdata.word);
			free(wdata.posarray);
		}
		if (nQueryKeyword > 1) {
			float proximityScore = computeProximityScore(posTable, nposArr, sddata->nword, nQueryKeyword, config);
			score = tfscore * proximityScore;
		}
		else score = tfscore;
		for (int i = 0; i < sddata->nword; i++) free(posTable[i]);
		free(posTable);
		free(nposArr);
		sddata->score = score;
	}
	freeDocIndexor(&docidxor);
}

/*	Calculate the score for QDocList.
*	Update the QDoclist.
*/
void calculateScore(FILE* fbarrel, const char* docidxorfname, SList& QDocList, int nKeywords, Config config) {
	computeScore2(fbarrel, docidxorfname, QDocList, nKeywords, config);
}

/*	Rank the doc in QDoclist	
*	Base on score of score
*/
void rankDoc(SList& QDocList) { 
	mergesort(QDocList, cmpSDDataScore, -1);
}

/*	Filter the doc in QDoclist
*	Base on score of score
*/
void filterDoc(SList& QDocList, int k) {
	filterKfirst(QDocList, k, freeSDData);
}

/*	Get the docId of a SDData
*/
int getDocId(void* sddata) {
	if (!sddata) return -1;
	SDData* docdata = (SDData*)sddata;
	return docdata->docId;
}

/*	Get the doc score of a SDData
*/
float getDocScore(void* sddata) {
	if (!sddata) return -1;
	SDData* docdata = (SDData*)sddata;
	return docdata->score;
}

/*	Do searching from a given query tokens
*/
SList doSearching(lexicon mainlex, SList tokens, Config config) {
	char* logfname = NULL;
	if (config.WRITE_SEARCHING_LOG) 
		logfname = generateSearchingLogName(tokens);

	FILE* fmainlex = fopen(config.LEXICON_FILES, "rb");
	FILE* fbarrelindexor = fopen(config.BARRELS_INDEXOR_FILES, "rb");
	FILE* fbarrel = fopen(config.BARRELS_FILES, "rb");
	SList QDoclist = searchAQuery(mainlex, fbarrelindexor, fbarrel, tokens);
	
	calculateScore(fbarrel, config.DOC_INDEXOR, QDoclist, tokens.size, config);
	rankDoc(QDoclist);
	if (config.WRITE_SEARCHING_LOG) {
		wprintf(L"Writing searching log...\n");
		writeSearchingLog(logfname, config.DOC_INDEXOR, fbarrel, QDoclist);
		wprintf(L"Complete writing log file\n");
	}
	filterDoc(QDoclist, config.K_BEST_DOC_DEFAULT);

	fclose(fmainlex);
	fclose(fbarrelindexor);
	fclose(fbarrel);
	free(logfname);
	return QDoclist;
}