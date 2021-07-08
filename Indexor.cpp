#include "Indexor.h"

/*	Make stopword lexicon
*/
lexicon mkstopwordLex(const char* stopwordfname) {
	lexicon swlex; 
	init(swlex);
	wchar_t* fname = chr2wchr(stopwordfname);
	FILE* fstopword;
	_wfopen_s(&fstopword, fname, L"r, ccs=UTF-16LE");
	free(fname);
	wchar_t* line;
	while (!feof(fstopword)) {
		line = getline(fstopword);
		addString(swlex, line, NULL);
		free(line);
	}
	return swlex;
}

/*	Make word array (Method 1) - Simple
*/
WData* makeWordArray1(SList token, int& n) {
	if (isEmpty(token)) return NULL;
	SList dupTok;
	listcpy(dupTok, token, wcharsize);
	mergesort(dupTok, cmpwchar, 1);
	float unit = 1.0 / dupTok.size;
	WData* wordArray = (WData*)malloc(sizeof(WData) * dupTok.size);
	wordArray[0].tf = unit;
	wordArray[0].word = new wchar_t[wcslen((wchar_t*)(dupTok.head->data)) + 1];
	wcscpy(wordArray[0].word, (wchar_t*)(dupTok.head->data));
	n = 1;
	SNode* cur = dupTok.head->next;
	for (; cur; cur = cur->next) {
		wchar_t* word = (wchar_t*)cur->data;
		if (wcscmp(wordArray[n - 1].word, word) == 0)
			wordArray[n - 1].tf += unit;
		else {
			wordArray[n].word = new wchar_t[wcslen(word) + 1];
			wcscpy(wordArray[n].word, word);
			wordArray[n].tf = unit;
			n++;
		}
	}
	if (n != dupTok.size)
		wordArray = (WData*)realloc(wordArray, n * sizeof(WData));
	removeAll(dupTok, rmvwchar);
	return wordArray;
}

wchar_t* mergeTokens(SNode* token1, SNode* token2) {
	wchar_t* s1 = (wchar_t*)token1->data;
	wchar_t* s2 = (wchar_t*)token2->data;
	wchar_t* strmerge;
	if (s1 && s2) {
		strmerge = new wchar_t[wcslen(s1) + wcslen(s2) + 2];
		wcscpy(strmerge, s1);
		wcscat(strmerge, L" ");
		wcscat(strmerge, s2);
		return strmerge;
	}
	if (s1) {
		strmerge = new wchar_t[wcslen(s1) + 1];
		wcscpy(strmerge, s1);
		return strmerge;
	}
	if (s2) {
		strmerge = new wchar_t[wcslen(s2) + 1];
		wcscpy(strmerge, s2);
		return strmerge;
	}
	strmerge = new wchar_t;
	strmerge[0] = L'\0';
	return strmerge;
}

bool cmpTokens(SNode* token1, SNode* token2) {
	wchar_t* s1 = (wchar_t*)token1->data;
	wchar_t* s2 = (wchar_t*)token2->data;
	if (!s1 || !s2) return false;
	if (wcscmp(s1, s2) == 0) return true;
	return false;
}

bool cmpConsecTokens(SNode* token1, SNode* token2, SNode* token3, SNode* token4) {
	wchar_t* s1 = (wchar_t*)token1->data;
	wchar_t* s2 = (wchar_t*)token2->data;
	wchar_t* s3 = (wchar_t*)token3->data;
	wchar_t* s4 = (wchar_t*)token4->data;
	if (!s1 || !s2 || !s3 || !s4) return false;
	if (wcscmp(s1, s3) == 0 && wcscmp(s2, s4) == 0) return true;
	return false;
}

bool isTokenSW(lexicon lex, SNode* token) {
	wchar_t* word = (wchar_t*)token->data;
	if (findString(lex, word)) return true;
	return false;
}

WData* makeWordArray21(SList token, int& n, lexicon stopwordlex) {
	if (isEmpty(token)) return NULL;

	SList dupTok;
	listcpy(dupTok, token, wcharsize);
	SList cleanTokens;
	initialize(cleanTokens);

	for (SNode* cur = dupTok.head; cur; cur = cur->next) {
		if (!cur->data) continue;
		//Merge two consecutive tokens & find in stop words list
		bool isSW = false;
		if (cur->next) {
			wchar_t* strmerge = mergeTokens(cur, cur->next);
			tnode* temp = findString(stopwordlex, strmerge);
			if (temp) {
				free(cur->data);
				free(cur->next->data);
				cur->data = NULL;
				cur->next->data = NULL;
				isSW = true;
			}
			free(strmerge);
		}

		int occurrences = 0;
		if (!isSW) {
			if (cur->next && cur->next->next && cur->next->next->next) {
				//Check for	occurrence of two consecutive syllables
				for (SNode* cur2 = cur->next->next; cur2 && cur2->next; cur2 = cur2->next) {
					if (cmpConsecTokens(cur, cur->next, cur2, cur2->next)) {
						occurrences++;
						free(cur2->data);
						free(cur2->next->data);
						cur2->data = NULL;
						cur2->next->data = NULL;
						cur2 = cur2->next;
						isSW = false;
					}
				}
			}
		}
		//Check current word is a stop word
		if (!isSW && occurrences == 0) {
			//Check for that word is stop word
			tnode* temp = findString(stopwordlex, (wchar_t*)cur->data);
			if (temp) {
				isSW = true;
				free(cur->data);
				cur->data = NULL;
			}
		}

		//Add to clean tokens list
		if (!isSW) {
			if (occurrences == 0) {
				wchar_t* temp = (wchar_t*)cur->data;
				removeDiacritic(temp);
				addHead(cleanTokens, temp, wcharsize(temp));
			}
			else {
				wchar_t* temp1 = (wchar_t*)cur->data;
				wchar_t* temp2 = (wchar_t*)cur->next->data;
				removeDiacritic(temp1);
				removeDiacritic(temp2);
				for (int i = 0; i <= occurrences; i++) {
					addHead(cleanTokens, temp1, wcharsize(temp1));
					addHead(cleanTokens, temp2, wcharsize(temp2));
				}
				cur = cur->next;
				if (!cur) break;
			}
		}
	}
	printf("-------------------------------------\n");
	printList(cleanTokens, printwchar);
	printf("tokens = %d\ncleanTokens = %d", token.size, cleanTokens.size);
	removeAll(dupTok, rmvwchar);
	removeAll(cleanTokens, rmvwchar);
	return NULL;
}

WData* makeWordArray22(SList token, int& n, lexicon stopwordlex) {
	if (isEmpty(token)) return NULL;

	SList dupTok;
	listcpy(dupTok, token, wcharsize);
	int totalwords = dupTok.size;
	SList cleanTokens;
	initialize(cleanTokens);
	wchar_t rmvword[] = L"--";
	int rmvwordcount = 0;
	//printList(dupTok, printwchar);
	//printf("\n");
	for (SNode* cur = dupTok.head; cur; cur = cur->next) {
		if (!cur->data) continue;
		//Merge two consecutive tokens & find in stop words list
		bool isSW = false;
		if (cur->next) {
			wchar_t* strmerge = mergeTokens(cur, cur->next);
			tnode* temp = findString(stopwordlex, strmerge);
			if (temp) {
				free(cur->data);
				free(cur->next->data);
				cur->data = NULL;
				cur->next->data = NULL;
				//addHead(cleanTokens, rmvword, 6);
				//addHead(cleanTokens, rmvword, 6);
				//addTail(cleanTokens, rmvword, 6);
				//addTail(cleanTokens, rmvword, 6);
				rmvwordcount += 2;
				isSW = true;
			}
			free(strmerge);
		}

		int occurrences = 0;
		int occurrencesfirst = 0;
		int occurrencessecond = 0;
		//Check current word is a stop word
		if (!isSW) {
			//Check for that word is stop word
			tnode* temp = findString(stopwordlex, (wchar_t*)cur->data);
			if (temp) isSW = true;
			if (isSW) {
				if (cur->next && cur->next->next && cur->next->next->next) {
					//Check for	occurrence of two consecutive syllables
					bool isFstTokenSW = isTokenSW(stopwordlex, cur);
					bool isSecTokenSW = isTokenSW(stopwordlex, cur->next);
					for (SNode* cur2 = cur->next->next; cur2 && cur2->next; cur2 = cur2->next) {
						if (cmpConsecTokens(cur, cur->next, cur2, cur2->next)) {
							occurrences++;
							isSW = false;
							//break;
						}
						if (isFstTokenSW && cmpTokens(cur->next, cur2->next)) 
							occurrencessecond++;
						if (isSecTokenSW && cmpTokens(cur, cur2)) 
							occurrencesfirst++;
					}
				}
			}
			if (isSW) {
				free(cur->data);
				cur->data = NULL;
				//addHead(cleanTokens, rmvword, 6);
				//addTail(cleanTokens, rmvword, 6);
				rmvwordcount += 1;
			}
			else {
				if (occurrencesfirst != 0 && occurrencessecond != 0) {
					double firstmatchTF = occurrences / ((double)occurrencesfirst);
					double tf = ((double)occurrences) / totalwords;
					//wprintf(L"%ls %ls - tf = %lf, fm = %lf\n", (wchar_t*)cur->data, (wchar_t*)cur->next->data, tf, firstmatchTF);
					if (firstmatchTF < THRESHOLD) {
						wchar_t* temp = (wchar_t*)cur->next->data;
						removeDiacritic(temp);
						//wprintf(L"%ls [Removed]\n", temp);
						free(cur->next->data);
						cur->next->data = NULL;
						//addHead(cleanTokens, rmvword, 6);
						//addTail(cleanTokens, rmvword, 6);
						isSW = true;
						rmvwordcount += 1;
					}
				}
				if (!isSW && occurrencessecond != 0) {
					double secondmatchTF = occurrences / ((double)occurrencessecond);
					double tf = ((double)occurrences) / totalwords;
					//wprintf(L"%ls %ls - tf = %lf, sm = %lf\n", (wchar_t*)cur->data, (wchar_t*)cur->next->data, tf, secondmatchTF);
					if (secondmatchTF < THRESHOLD) {
						wchar_t* temp = (wchar_t*)cur->data;
						removeDiacritic(temp);
						//wprintf(L"%ls [Removed]\n", temp);
						free(cur->data);
						cur->data = NULL;
						//addHead(cleanTokens, rmvword, 6);
						//addTail(cleanTokens, rmvword, 6);
						isSW = true;
						rmvwordcount += 1;
					}
				}
			}
		}
		
		//Add to clean tokens list
		if (!isSW) {
			if (occurrences == 0) {
				wchar_t* temp = (wchar_t*)cur->data;
				removeDiacritic(temp);
				addHead(cleanTokens, temp, wcharsize(temp));
				//addTail(cleanTokens, temp, wcharsize(temp));
			}
			else {
				wchar_t* temp1 = (wchar_t*)cur->data;
				wchar_t* temp2 = (wchar_t*)cur->next->data;
				removeDiacritic(temp1);
				removeDiacritic(temp2);
				for (int i = 0; i < 1; i++) {
					addHead(cleanTokens, temp1, wcharsize(temp1));
					addHead(cleanTokens, temp2, wcharsize(temp2));
					//addTail(cleanTokens, temp1, wcharsize(temp1));
					//addTail(cleanTokens, temp2, wcharsize(temp2));
				}
				cur = cur->next;
				if (!cur) break;
			}
		}
	}
	//printf("-------------------------------------\n");
	printList(cleanTokens, printwchar);
	printf("\n");
	printf("tokens = %d, cleanTokens = %d, remove words = %d\n", token.size, cleanTokens.size, rmvwordcount);
	removeAll(dupTok, rmvwchar);
	WData* res = makeWordArray1(cleanTokens, n);
	removeAll(cleanTokens, rmvwchar);
	return res;
}

WData* makeWordArray23(SList token, int& n, lexicon stopwordlex) {
	if (isEmpty(token)) return NULL;

	SList dupTok;
	listcpy(dupTok, token, wcharsize);
	SList cleanTokens;
	initialize(cleanTokens);
	wchar_t rmvword[] = L"--";

	int* markArray = new int[dupTok.size];
	int rmvwordcount = 0;

	for (SNode* cur = dupTok.head; cur; cur = cur->next) {
		if (!cur->data) continue;
		//Merge two consecutive tokens & find in stop words list
		bool isSW = false;
		if (cur->next) {
			wchar_t* strmerge = mergeTokens(cur, cur->next);
			tnode* temp = findString(stopwordlex, strmerge);
			if (temp) {
				free(cur->data);
				free(cur->next->data);
				cur->data = NULL;
				cur->next->data = NULL;
				//addHead(cleanTokens, rmvword, 6);
				//addHead(cleanTokens, rmvword, 6);
				addTail(cleanTokens, rmvword, 6);
				addTail(cleanTokens, rmvword, 6);
				rmvwordcount += 2;
				isSW = true;
			}
			free(strmerge);
		}

		int occurrences = 0;

		//Check current word is a stop word
		if (!isSW) {
			//Check for that word is stop word
			tnode* temp = findString(stopwordlex, (wchar_t*)cur->data);
			if (temp) isSW = true;
			if (isSW) {
				if (cur->next && cur->next->next && cur->next->next->next) {
					//Check for	occurrence of two consecutive syllables
					for (SNode* cur2 = cur->next->next; cur2 && cur2->next; cur2 = cur2->next) {
						if (cmpConsecTokens(cur, cur->next, cur2, cur2->next)) {
							occurrences++;
							isSW = false;
							break;
						}
					}
				}
			}
			if (isSW) {
				free(cur->data);
				cur->data = NULL;
				//addHead(cleanTokens, rmvword, 6);
				addTail(cleanTokens, rmvword, 6);
				rmvwordcount += 1;
			}
		}

		//Add to clean tokens list
		if (!isSW) {
			if (occurrences == 0) {
				wchar_t* temp = (wchar_t*)cur->data;
				removeDiacritic(temp);
				//addHead(cleanTokens, temp, wcharsize(temp));
				addTail(cleanTokens, temp, wcharsize(temp));
			}
			else {
				wchar_t* temp1 = (wchar_t*)cur->data;
				wchar_t* temp2 = (wchar_t*)cur->next->data;
				removeDiacritic(temp1);
				removeDiacritic(temp2);
				for (int i = 0; i < occurrences; i++) {
					//addHead(cleanTokens, temp1, wcharsize(temp1));
					//addHead(cleanTokens, temp2, wcharsize(temp2));
					addTail(cleanTokens, temp1, wcharsize(temp1));
					addTail(cleanTokens, temp2, wcharsize(temp2));
				}
				cur = cur->next;
				if (!cur) break;
			}
		}
	}
	printf("-------------------------------------\n");
	printList(cleanTokens, printwchar);
	printf("\n");
	printf("tokens = %d\ncleanTokens = %d\nremove sw = %d\n", token.size, cleanTokens.size, rmvwordcount);
	removeAll(dupTok, rmvwchar);
	removeAll(cleanTokens, rmvwchar);
	return NULL;
}

/*	Make word array (Method 2) - Advanced
*/
WData* makeWordArray2(SList token, int& n, lexicon stopwordlex) {
	//makeWordArray22(token, n, stopwordlex);
	return makeWordArray22(token, n, stopwordlex);
}

/*	Make word array that contains info of words from a token
*	Return a word array
*/
WData* mkWordArray(SList token, int &n, lexicon stopwordlex) {
	return makeWordArray2(token, n, stopwordlex);
}

/*	Append new doc info at the end of doc indexor with an opened file
*	Return true if it succeed
*/
bool saveDoc2DocIndexor(FILE* fdocidxor, DocData docdata) {
	//Write to the doc indexor in turn: docId - nchardir - docdir - nword - docAdd
	fseek(fdocidxor, 0, SEEK_END);
	fwrite(&docdata.docId, sizeof(docdata.docId), 1, fdocidxor);
	fwrite(&docdata.nchardir, sizeof(docdata.nchardir), 1, fdocidxor);
	fwrite(docdata.docdir, sizeof(char), docdata.nchardir + 1, fdocidxor);
	fwrite(&docdata.nword, sizeof(int), 1, fdocidxor);
	fwrite(&docdata.docAdd, sizeof(int), 1, fdocidxor);
	return true;
}

/*	Save a full doc array to doc indexor
*	Return true if it succeed
*/
bool saveDocIndexor(const char* docidxorfname, DocIndexor docidxor) {
	FILE* fdocidxor = fopen(docidxorfname, "wb");
	if (!fdocidxor) return false;
	int ndocs = docidxor.ndocs;
	DocData* docArray = docidxor.docarray;
	for (int i = 0; i < ndocs; i++) {
		if (docArray[i].docId >= 0)	
			saveDoc2DocIndexor(fdocidxor, docArray[i]);
	}
	return true;
}

/*	Append new word info at the end of barrel with an opened file
*	Return true if it succeed
*/
bool saveWordData(FILE* fbarrel, WData wdata) {
	int nchar = wcslen(wdata.word);
	fseek(fbarrel, 0, SEEK_END);
	fwrite(&nchar, sizeof(nchar), 1, fbarrel);
	//Including '\0'
	fwrite(wdata.word, wcharsize(wdata.word), 1, fbarrel);
	fwrite(&wdata.tf, sizeof(wdata.tf), 1, fbarrel);
	return true;
}

/*	Save a doc to the end of the barrel
*	Return address array and update docAdd argument
*	Returned word addresses in address array are addresses that exclusive the docAdd
*/
WAData* saveBarrel(FILE* fbarrel, WData* wordarray, int nword, int& docAdd) {
	WAData* addressArray = new WAData[nword];
	//Update the docAdd by the start position
	fseek(fbarrel, 0, SEEK_END);
	docAdd = ftell(fbarrel);
	//Write to barrel each word in wordArray and update addressArray
	for (int i = 0; i < nword; i++) {
		addressArray[i].word = new wchar_t[wcslen(wordarray[i].word) + 1];
		wcscpy(addressArray[i].word, wordarray[i].word);
		//Substract docAdd to get the address of that word in the doc
		addressArray[i].wordAdd = (int)ftell(fbarrel) - docAdd; 
		saveWordData(fbarrel, wordarray[i]);
	}
	return addressArray;
}

/*	Adapt word address to lexicon
*	Lexicon will be updated in which the adapter will be doclist
*/
void adtWAdd2Lex(lexicon& lex, int docId, WAData* addressArray, int nword) {
	//Adapter of tnode is Doclist in this function
	for (int i = 0; i < nword; i++) {
		//Find word in the lexicon. If it is found, update that word's doclist, else, add that word to lexicon
		tnode* word = findString(lex, addressArray[i].word);
		if (word) {
			//Check for the adapter is not NULL
			if (word->adapter) { 
				Doclist* doclist = (Doclist*)word->adapter;
				//If docArray need more space to store data, reallocate it
				if (doclist->ndoc % 10 == 0) doclist->docArray = (WDData*)realloc(doclist->docArray, sizeof(WDData) * (doclist->ndoc + 10));
				//Append new doc info to doclist
				doclist->docArray[doclist->ndoc] = { docId, addressArray[i].wordAdd };
				doclist->ndoc++;
			}
			else {
				wprintf(L"Error in indexor.cpp void adtWAdd2Lex(...) function\n");
				return;
			}
		}
		else {
			Doclist* doclist = new Doclist;
			doclist->ndoc = 1;
			doclist->docArray = new WDData[10];
			doclist->docArray[0] = { docId, addressArray[i].wordAdd };
			addString(lex, addressArray[i].word, doclist);
		}
	}
}

/*	Adapt word address to wldata lexicon
*	The adapter of lexicon must be wldata before calling this function
*	While running this function, it will write updated doclist to the end of the barrel indexor
*	Note: fbarrelidxor should be in append mode to write at the end
*/
void adtWAdd2Lex(lexicon& lex, int docId, WAData* addressArray, int nword, FILE* fbarrelidxor) {
	for (int i = 0; i < nword; i++) {
		wchar_t* word = addressArray[i].word;
		int wordAdd = addressArray[i].wordAdd;
		//Find word in the lexicon. 
		//If it is found, get the word's doclist from barrel indexor and update it, then append new doclist to the end of barrel indexor 
		//and update the dlistAdd of current tnode;
		//else, add that word to lexicon
		tnode* charNode = findString(lex, word);
		if (charNode) {
			//Check for the adapter is not NULL
			if (charNode->adapter) {
				WLData* wldata = (WLData*)charNode->adapter;
				Doclist* doclist = getDoclist(fbarrelidxor, *wldata);
				//Append new doc info to doclist
				doclist->docArray = (WDData*)realloc(doclist->docArray, sizeof(WDData) * (doclist->ndoc + 1));
				doclist->docArray[doclist->ndoc] = { docId, wordAdd };
				doclist->ndoc++;
				//Write new doclist to barrerl indexor & update current wldata
				saveDoclist2BIndexor(fbarrelidxor, doclist, wldata->dlistAdd);
				wldata->ndoc++;
				printf("wldata dlistAdd = %d\n", ((WLData*)charNode->adapter)->dlistAdd);
				freeDoclist(doclist);
			}
			else {
				wprintf(L"Error in indexor.cpp void adtWAdd2Lex(...) function\n");
				return;
			}
		}
		else {
			Doclist* doclist = new Doclist;
			doclist->ndoc = 1;
			doclist->docArray = new WDData[10];
			doclist->docArray[0] = { docId, wordAdd };
			
			int nchar = wcslen(word);
			WLData* wldata = new WLData;
			wldata->word = new wchar_t[nchar + 1];
			wcscpy(wldata->word, word);
			wldata->nchar = nchar;
			wldata->ndoc = 1;
			saveDoclist2BIndexor(fbarrelidxor, doclist, wldata->dlistAdd);
			freeDoclist(doclist);

			addString(lex, word, wldata);
		}
	}
}

/*	Save a doclist to barrel indexor
*	Update the dlistAdd argument
*/
void saveDoclist2BIndexor(FILE* fbarrelidxor, Doclist* doclist, int& dlistAdd) {
	WDData* docArray = doclist->docArray;
	fseek(fbarrelidxor, 0, SEEK_END);
	dlistAdd = ftell(fbarrelidxor);
	fwrite(docArray, sizeof(WDData), doclist->ndoc, fbarrelidxor);
}

/*	Save WDDataArray to Barrel indexor
*	Update the dlistAdd argument
*/
void saveWDDataArray2BIndexor(FILE* fbarrelidxor, WDData* wddataArray, int nElements, int& dlistAdd) {
	fseek(fbarrelidxor, 0, SEEK_END);
	dlistAdd = ftell(fbarrelidxor);
	fwrite(wddataArray, sizeof(WDData), nElements, fbarrelidxor);
}

/*	Remove words info in doclists and update dlistAdd of lexicon's tnode
*	Note: fbarrelidxor should be in append mode to write at the end
*/
void rmvWDDataInDlist(FILE* fbarrelidxor, lexicon& lex, WData* wordArray, int docId, int nword) {
	for (int i = 0; i < nword; i++) {
		wchar_t* word = wordArray[i].word;
		tnode* charNode;
		if ((charNode = findString(lex, word))) {
			WLData* wldata = (WLData*)charNode->adapter;
			Doclist* doclist = getDoclist(fbarrelidxor, *wldata);
			int originNdoc = doclist->ndoc;
			int rmvWDDataIndex = getIndexWDData(*doclist, docId);
			if (rmvWDDataIndex < 0) continue;
			int dlistAdd = 0, temp = 0;
			saveWDDataArray2BIndexor(fbarrelidxor, doclist->docArray, rmvWDDataIndex, dlistAdd);
			saveWDDataArray2BIndexor(fbarrelidxor, &doclist->docArray[rmvWDDataIndex + 1], originNdoc - rmvWDDataIndex - 1, temp);
			wldata->ndoc--;
			if (wldata->ndoc == 0) {
				charNode->isWord = false;
				freewldata(wldata);
			}
			else wldata->dlistAdd = dlistAdd;
		}
	}
}

/*	Write all lexicon to file. Adapter of tnode in the lexicon must be wldata
*/
void saveWLData(FILE* flexicon, void* a) {
	WLData* wldata = (WLData*)a;
	fwrite(&wldata->nchar, sizeof(wldata->nchar), 1, flexicon);
	fwrite(wldata->word, wcharsize(wldata->word), 1, flexicon);
	fwrite(&wldata->dlistAdd, sizeof(wldata->dlistAdd), 1, flexicon);
	fwrite(&wldata->ndoc, sizeof(wldata->ndoc), 1, flexicon);
}

/*	Save barrel indexor and lexicon file
*	Update the lexicon. The adapter of lexicon must be doclist before calling the function
*/
void saveBIandLex(FILE* fbi, FILE* flex, lexicon& lex) {
	SNode* root = lex.main.head;
	WLData* wldata = NULL;
	tnode* charNode = NULL;
	//Temporary word that contains letter of each level.
	wchar_t word[100]; 
	for (; root; root = root->next) {
		//Initialize first char, increase nchar for deeper level (latter character)
		int nchar = 1; 
		charNode = (tnode*)(root->data);
		word[nchar - 1] = charNode->chr;
		word[nchar] = L'\0';
		//If it is a word, then save wldata to lexicon
		if (charNode->isWord) {
			wldata = new WLData;
			wldata->word = new wchar_t[nchar + 1];
			wcscpy(wldata->word, word);
			wldata->nchar = nchar;
			wldata->ndoc = ((Doclist*)charNode->adapter)->ndoc;
			saveDoclist2BIndexor(fbi, (Doclist*)charNode->adapter, wldata->dlistAdd);
			//Replace adapter of tnode from doclist to wldata
			freeDoclist(charNode->adapter);
			charNode->adapter = wldata;
			saveWLData(flex, wldata);
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
					//If it is a word, then save wldata to lexicon
					if (charNode->isWord) {
						wldata = new WLData;
						wldata->word = new wchar_t[nchar + 1];
						wcscpy(wldata->word, word);
						wldata->nchar = nchar;
						wldata->ndoc = ((Doclist*)charNode->adapter)->ndoc;
						saveDoclist2BIndexor(fbi, (Doclist*)charNode->adapter, wldata->dlistAdd);
						//Replace adapter of tnode from doclist to wldata
						freeDoclist(charNode->adapter);
						charNode->adapter = wldata;
						saveWLData(flex, wldata);
					}
					/*if (wldata = saveWord2BIndexor(fbi, charNode, word, nchar)) {
						saveWLData(flex, wldata);
					}*/

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

/*	Save lexicon file from wldata lexicon
*	The adapter of lexicon must be wldata before calling this function
*/
void saveLex(FILE* flex, lexicon lex) {
	SNode* root = lex.main.head;
	WLData* wldata = NULL;
	tnode* charNode = NULL;
	//Temporary word that contains letter of each level.
	wchar_t word[100];
	for (; root; root = root->next) {
		//Initialize first char, increase nchar for deeper level (latter character)
		int nchar = 1;
		charNode = (tnode*)(root->data);
		word[nchar - 1] = charNode->chr;
		word[nchar] = L'\0';
		//If it is a word, then save wldata to lexicon
		if (charNode->isWord) {
			wldata = (WLData*)charNode->adapter;
			saveWLData(flex, wldata);
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
					//If it is a word, then save wldata to lexicon
					if (charNode->isWord) {
						wldata = (WLData*)charNode->adapter;
						saveWLData(flex, wldata);
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

/*	Rewrite the barrel indexor and lexicon files, respectively, from current lexicon
*	Lexicon is still point to the old barrel indexor before running this function. The adapter of tnode of this lexicon must be wldata
*	Lexicon will be updated to point to the new barrel indexor after that.
*	This process will generate cache files while running
*/
bool rewriteBIandLex(const char* lexfname, const char* barrelidxorfname, lexicon& lex) {
	bool status = true;
	char* cachelexfname = new char[strlen(lexfname) + 4];
	char* cachebifname = new char[strlen(barrelidxorfname) + 4];
	strcpy(cachelexfname, lexfname);
	strcpy(cachebifname, barrelidxorfname);
	strcat(cachelexfname, "tmp");
	strcat(cachebifname, "tmp");

	FILE* fcachelex = fopen(cachelexfname, "wb");
	FILE* fcachebarrelidxor = fopen(cachebifname, "wb");
	FILE* fbarrelidxor = fopen(barrelidxorfname, "rb");

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
			//Get doclist info from old barrel indexor base on current wldata
			WLData* wldata = (WLData*)charNode->adapter;
			Doclist* doclist = getDoclist(fbarrelidxor, *wldata);
			//Save to cache barrel indexor & get the new dlistAdd
			int newdlistAdd = 0;
			saveDoclist2BIndexor(fcachebarrelidxor, doclist, newdlistAdd);
			//Change dlistAdd of current wldata
			wldata->dlistAdd = newdlistAdd;
			saveWLData(fcachelex, wldata);
			freeDoclist(doclist);
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
						//Get doclist info from old barrel indexor base on current wldata
						WLData* wldata = (WLData*)charNode->adapter;
						Doclist* doclist = getDoclist(fbarrelidxor, *wldata);
						//Save to cache barrel indexor & get the new dlistAdd
						int newdlistAdd = 0;
						saveDoclist2BIndexor(fcachebarrelidxor, doclist, newdlistAdd);
						//Change dlistAdd of current wldata
						wldata->dlistAdd = newdlistAdd;
						saveWLData(fcachelex, wldata);
						freeDoclist(doclist);
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
	fclose(fcachelex);
	fclose(fcachebarrelidxor);
	fclose(fbarrelidxor);
	/*
	if (remove(lexfname)) {
		status = false;
		wprintf(L"Unable to remove file\n");
	}
	if (rename(cachelexfname, lexfname)) {
		status = false;
		wprintf(L"Unable to rename file\n");
	}
	if (remove(barrelidxorfname)) {
		status = false;
		wprintf(L"Unable to remove file\n");
	}
	if (rename(cachebifname, barrelidxorfname)) {
		status = false;
		wprintf(L"Unable to rename file\n");
	}
	*/
	return true;
}

/*	Rewrite the barrel and doc indexor files, respectively, from current doc array
*	DocArray is still point to old barrel before calling this function
*	DocArray will point to new barrel after calling this function
*	This process will generate cache files while running
*/
bool rewriteBarrelandDocIdxor(const char* docidxorfname, const char* barrelfname, DocIndexor docidxor) {
	DocData* docArray = docidxor.docarray;
	int ndocs = docidxor.ndocs;
	bool status = true;
	char* cachedocixorfname = new char[strlen(docidxorfname) + 4];
	char* cachebarrelfname = new char[strlen(barrelfname) + 4];
	strcpy(cachedocixorfname, docidxorfname);
	strcpy(cachebarrelfname, barrelfname);
	strcat(cachedocixorfname, "tmp");
	strcat(cachebarrelfname, "tmp");

	FILE* fcachedocidxor = fopen(cachedocixorfname, "wb");
	FILE* fcachebarrel = fopen(cachebarrelfname, "wb");
	FILE* fbarrel = fopen(barrelfname, "rb");

	for (int doci = 0; doci < ndocs; doci++) {
		int nword = docArray[doci].nword;
		int docAdd = docArray[doci].docAdd;
		WData* wordArray = getWordArray(fbarrel, nword, docAdd);
		//Save word array to cache barrel and update new docAdd
		WAData* addressArray = saveBarrel(fcachebarrel, wordArray, nword, docAdd);
		freeWordArray(wordArray, nword);
		freeAddressArray(addressArray, nword);
		//Update docAdd of current doc in docArray
		docArray[doci].docAdd = docAdd;
		saveDoc2DocIndexor(fcachedocidxor, docArray[doci]);
	}

	fclose(fcachedocidxor);
	fclose(fcachebarrel);
	fclose(fbarrel);
	return status;
}

WData* preprocessFile(char* filename, int& nword, lexicon stopwordlex) {
	//Read file
	clock_t begin = clock();
	wchar_t* text = readFile(filename);
	removeDiacritic(text);
	normalize(text);
	rmvRedundant(text);
	SList wordList = splitword(text, L' ');
	free(text);
	wprintf(L"Complete read file time spent = %lf\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
	
	//Make word array
	nword = 0;
	WData* wordArray = mkWordArray(wordList, nword, stopwordlex);
	removeAll(wordList, rmvwchar);
	return wordArray;
}

/*	Indexing (Method 1) 
*	Lexicon will be doclist lexicon while running; after run through all files, save to barrel indexor and convert to wldata lexicon
*	Pros: Faster
*	Cons: Consume more RAM memory
*/
void indexing1(lexicon& mainlex, char**& listfile) {
	clock_t beginTotal = clock();
	init(mainlex);

	FILE* fbarrel = fopen(BARRELS_FILES, "wb");
	FILE* findex = fopen(INDEX_FILES, "r");
	FILE* fdocidxor = fopen(DOC_INDEXOR, "wb");
	lexicon swlex = mkstopwordLex(STOPWORD_FILES);
	int nfile = 0;
	listfile = readCorpus(findex, nfile);
	fclose(findex);

	for (int filei = 0; filei < nfile; filei++) {
		int docId = filei;
		wchar_t* tempfname = chr2wchr(listfile[filei]);
		wprintf(L"FILE %d trong tong so %d, FILE NAME = %s\n", filei, nfile, tempfname);
		free(tempfname);

		clock_t begin = clock();
		int nword = 0;
		WData* wordArray = preprocessFile(listfile[filei], nword, swlex);
		/*printf("n = %d\n", n);
		printf("wordArray:\n");
		printWordArray(wordArray, n);
		printf("---------------------\n");*/

		//Write to barrel
		int docAdd = 0;
		WAData* addressArray = saveBarrel(fbarrel, wordArray, nword, docAdd);
		freeWordArray(wordArray, nword);
		//Write to doc indexor
		DocData docdata;
		docdata.docId = docId;
		docdata.nchardir = strlen(listfile[filei]);
		docdata.docdir = listfile[filei];
		docdata.nword = nword;
		docdata.docAdd = docAdd;
		saveDoc2DocIndexor(fdocidxor, docdata);
		/*printf("addressArray:\n");
		printAddressArray(addressArray, n);
		printf("---------------------\n");*/

		//Adapt word address to lexicon
		adtWAdd2Lex(mainlex, filei, addressArray, nword);
		freeAddressArray(addressArray, nword);
		//printLexicon(mainlex, printDoclist);
		wprintf(L"Complete write barrel time spent = %lf\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
	}

	//Write barrel indexor & lexicon
	wprintf(L"*-------------------------------------------------------*\n");
	clock_t begin = clock();
	FILE* fmainlex = fopen(LEXICON_FILES, "wb");
	FILE* fbarrelindexor = fopen(BARRELS_INDEXOR_FILES, "wb");
	saveBIandLex(fbarrelindexor, fmainlex, mainlex);
	printLexicon(mainlex, printwldata);
	fclose(fmainlex);
	fclose(fbarrel);
	fclose(fdocidxor);
	fclose(fbarrelindexor);
	wprintf(L"Write lexicon time spent = %lf\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
	wprintf(L"Total time spent = %lf\n", (double)(clock() - beginTotal) / CLOCKS_PER_SEC);
}

/*	Indexing (Method 2)
*	Lexicon will be wldata lexicon while running; save to barrel indexor and get doclist from cache barrel indexor for every files
*	Pros: Consume a little RAM memory than method 1
*	Cons: Slower and consume a memory to store cache
*/
void indexing2(lexicon& mainlex, char**& listfile) {
	clock_t beginTotal = clock();
	init(mainlex);

	FILE* fbarrel = fopen(BARRELS_FILES, "wb");
	FILE* findex = fopen(INDEX_FILES, "r");
	FILE* fdocidxor = fopen(DOC_INDEXOR, "wb");
	FILE* fbarrelindexor = fopen(BARRELS_INDEXOR_FILES, "wb+");
	lexicon swlex = mkstopwordLex(STOPWORD_FILES);

	int nfile = 0;
	listfile = readCorpus(findex, nfile);
	fclose(findex);

	for (int filei = 0; filei < nfile; filei++) {
		int docId = filei;
		wchar_t* tempfname = chr2wchr(listfile[filei]);
		wprintf(L"FILE %d trong tong so %d, FILE NAME = %s\n", filei, nfile, tempfname);
		free(tempfname);

		clock_t begin = clock();
		int nword = 0;
		WData* wordArray = preprocessFile(listfile[filei], nword, swlex);
		
		//Write to barrel
		int docAdd = 0;
		WAData* addressArray = saveBarrel(fbarrel, wordArray, nword, docAdd);
		freeWordArray(wordArray, nword);
		//Write to doc indexor
		DocData docdata;
		docdata.docId = docId;
		docdata.nchardir = strlen(listfile[filei]);
		docdata.docdir = listfile[filei];
		docdata.nword = nword;
		docdata.docAdd = docAdd;
		saveDoc2DocIndexor(fdocidxor, docdata);
		
		//Adapt word address to lexicon
		adtWAdd2Lex(mainlex, filei, addressArray, nword, fbarrelindexor);
		freeAddressArray(addressArray, nword);
		//printLexicon(mainlex, printDoclist);
		wprintf(L"Complete write barrel time spent = %lf\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
	}

	//Rewrite barrel indexor & lexicon
	wprintf(L"*-------------------------------------------------------*\n");
	clock_t begin = clock();
	FILE* fmainlex = fopen(LEXICON_FILES, "wb");
	saveLex(fmainlex, mainlex);
	printLexicon(mainlex, printwldata);
	fclose(fmainlex);
	fclose(fbarrel);
	fclose(fdocidxor);
	fclose(fbarrelindexor);
	wprintf(L"Write lexicon time spent = %lf\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
	wprintf(L"Total time spent = %lf\n", (double)(clock() - beginTotal) / CLOCKS_PER_SEC);
}

/*	This function will update listfile. Do not need to have a list file before.
*	This function will index a list of file 
*/
int doIndexing(lexicon& mainlex, char**& listfile) {
	indexing1(mainlex, listfile);
	return 0;
}

/*	Add new doc to docArray keep order ascendant by docId 
*	Return that doc data and update the docArray
*/
DocData* addDoc2DocIndexor(char* dir, DocIndexor& docidxor) {
	int ndocs = docidxor.ndocs;
	DocData* docArray = docidxor.docarray;
	//Unique docId is get by add one to the last doc's docId
	int docId = docArray[ndocs - 1].docId + 1;
	docArray = (DocData*)realloc(docArray, sizeof(DocData) * (ndocs + 1));
	docidxor.ndocs++;
	docidxor.docarray = docArray;
	DocData* newdoc = &docArray[ndocs];
	newdoc->nchardir = strlen(dir);
	newdoc->docdir = new char[newdoc->nchardir + 1];
	strcpy(newdoc->docdir, dir);
	newdoc->nword = 0;
	newdoc->docAdd = 0;
	newdoc->docId = docId;
	return newdoc;
}

/*	Remove a doc from docArray and keep order ascendant by label the Id of that doc to a negative number
*	Return docData* if successful; NULL if fail
*/
DocData* rmvDocInDocIndexor(int docId, DocIndexor& docidxor) {
	DocData* docdata = NULL;
	if (docdata = getDocDatabyId(docidxor.docarray, docidxor.ndocs, docId)) 
		docdata->docId = -1;
	return docdata;
}

int indexNewFile(char* dir, DocIndexor& docidxor, lexicon& lex) {
	FILE* fbarrel = fopen(BARRELS_FILES, "ab");
	FILE* fbarrelindexor = fopen(BARRELS_INDEXOR_FILES, "ab+");
	FILE* fdocidxor = fopen(DOC_INDEXOR, "ab");
	FILE* flex = fopen(LEXICON_FILES, "wb");
	lexicon swlex = mkstopwordLex(STOPWORD_FILES);
	DocData* pdocData = addDoc2DocIndexor(dir, docidxor);
	int nword = 0;
	wchar_t* tempfname = chr2wchr(dir);
	wprintf(L"Indexing... FILE = %ls\n", tempfname);
	free(tempfname);
	WData* wordArray = preprocessFile(dir, nword, swlex);
	int docAdd = 0;
	WAData* addressArray = saveBarrel(fbarrel, wordArray, nword, docAdd);
	freeWordArray(wordArray, nword);
	pdocData->docAdd = docAdd;
	pdocData->nword = nword;
	//Adapt word to wldata lexicon
	adtWAdd2Lex(lex, pdocData->docId, addressArray, nword, fbarrelindexor);
	saveDoc2DocIndexor(fdocidxor, *pdocData);
	saveLex(flex, lex);
	fclose(fbarrel);
	fclose(fbarrelindexor);
	fclose(fdocidxor);
	fclose(flex);
	wprintf(L"Successful add new file to corpus\n");
	return 0;
}

int removeFile(int docId, DocIndexor& docidxor, lexicon& lex) {
	FILE* fbarrel = fopen(BARRELS_FILES, "rb");
	FILE* fbarrelindexor = fopen(BARRELS_INDEXOR_FILES, "ab+");
	FILE* flex = fopen(LEXICON_FILES, "wb");

	DocData* docdata = rmvDocInDocIndexor(docId, docidxor);
	if (!docdata) return -1;
	WData* wordArray = getWordArray(fbarrel, docdata->nword, docdata->docAdd);
	printWordArray(wordArray, docdata->nword);
	rmvWDDataInDlist(fbarrelindexor, lex, wordArray, docId, docdata->nword);
	printLexicon(lex, printwldata);
	saveLex(flex, lex);
	printDocIndexor(docidxor);
	saveDocIndexor(DOC_INDEXOR, docidxor);
	
	fclose(fbarrel);
	fclose(fbarrelindexor);
	fclose(flex);
	wprintf(L"Successful remove file from corpus\n");
	return 0;
}
