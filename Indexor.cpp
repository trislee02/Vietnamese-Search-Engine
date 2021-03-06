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

/*	Merge two tokens whose data is wchar_t* into wchar_t*
*/
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

/*	Merge two tokens whose data is TokenData into wchar_t*
*/
wchar_t* mergeWordTokenDatas(SNode* token1, SNode* token2) {
	wchar_t* s1 = ((TokenData*)token1->data)->word;
	wchar_t* s2 = ((TokenData*)token2->data)->word;
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

/*	Compare two tokens whose data is wchar_t*
*/
bool cmpTokens(SNode* token1, SNode* token2) {
	wchar_t* s1 = (wchar_t*)token1->data;
	wchar_t* s2 = (wchar_t*)token2->data;
	if (!s1 || !s2) return false;
	if (wcscmp(s1, s2) == 0) return true;
	return false;
}

/*	Compare two tokens whose data is TokenData by word
*/
bool isMatchedTokenDatasbyWord(SNode* a, SNode* b) {
	TokenData* token1 = (TokenData*)a->data;
	TokenData* token2 = (TokenData*)b->data;
	//wprintf(L"%ls %ls ==> %d\n", token1->word, token2->word, wcscmp(token1->word, token2->word));
	if (wcscmp(token1->word, token2->word) == 0) return true;
	return false;
}

/*	Compare two tokens whose data is TokenData by word
*/
int cmpTokenDatasbyWord(void* a, void* b) {
	TokenData* token1 = (TokenData*)a;
	TokenData* token2 = (TokenData*)b;
	int res = wcscmp(token2->word, token1->word);
	if (res == 0) return token1->position < token2->position ? 1 : -1;
	return res;
}

/*	Compare token 1 with token 3 and token 2 with token 4 whose data is wchar_t*
*/
bool cmpConsecTokens(SNode* token1, SNode* token2, SNode* token3, SNode* token4) {
	wchar_t* s1 = (wchar_t*)token1->data;
	wchar_t* s2 = (wchar_t*)token2->data;
	wchar_t* s3 = (wchar_t*)token3->data;
	wchar_t* s4 = (wchar_t*)token4->data;
	if (!s1 || !s2 || !s3 || !s4) return false;
	if (wcscmp(s1, s3) == 0 && wcscmp(s2, s4) == 0) return true;
	return false;
}

/*	Check a token whose data is wchar_t* is a stop word in given lexicon
*/
bool isTokenSW(lexicon lex, SNode* token) {
	wchar_t* word = (wchar_t*)token->data;
	if (findString(lex, word)) return true;
	return false;
}

/*	Check a token whose data is TokenData is a stop word in given lexicon
*/
bool isTokenDataSW(lexicon lex, SNode* token) {
	wchar_t* word = ((TokenData*)token->data)->word;
	if (findString(lex, word)) return true;
	return false;
}

/*	Return a duplicate of a wchar_t*
*/
void* copywchar(void* a) {
	wchar_t* word = (wchar_t*)a;
	wchar_t* copiedWord = new wchar_t[wcslen(word) + 1];
	wcscpy(copiedWord, word);
	return copiedWord;
}

/*	Make TokenList which is a list of TokenData from a given list whose elements are wchar_t*
*/
SList makeTokensList(SList token) {
	SList tokensList;
	initialize(tokensList);
	if (isEmpty(token)) return tokensList;
	int index = 0;
	for (SNode* cur = token.head; cur; cur = cur->next) {
		wchar_t* word = (wchar_t*)cur->data;
		TokenData* tokData = new TokenData;
		tokData->position = index;
		tokData->word = new wchar_t[wcslen(word) + 1];
		wcscpy(tokData->word, word);
		addTail(tokensList, tokData, sizeof(TokenData));
		free(tokData);
		index++;
	}
	return tokensList;
}

/*	Make word array from TokenList which is a list of TokenData
*	Return a word array (WData*) and update argument n
*/
WData* makeWordArrayFromTokensList(SList tokensList, int& n) {
	if (isEmpty(tokensList)) return NULL;
	//Sort the token list in order for similar words to be placed consecutively
	SList sortedTokensList;
	listcpy(sortedTokensList, tokensList, copyTokenData);
	mergesort(sortedTokensList, cmpTokenDatasbyWord, 1);
	//Compute the tf for each word
	float unitTf = 1.0 / sortedTokensList.size;
	WData* wordArray = (WData*)malloc(sizeof(WData) * sortedTokensList.size);
	n = 0;
	for (SNode* cur = sortedTokensList.head; cur; cur = cur->next) {
		TokenData* curTokenData = (TokenData*)cur->data;
		wchar_t* curWord = curTokenData->word;
		wchar_t* prevWord = NULL;
		//Get prevWord if word array has at least 1 element
		if (n >= 1) prevWord = wordArray[n - 1].word;
		//If there is prevWord and current word is similar to it, then increase the tf and add position to posArray
		if (prevWord && (wcscmp(prevWord, curWord) == 0)) {
			WData* wdata = &wordArray[n - 1];
			wchar_t* word = wdata->word;
			wdata->tf += unitTf;
			wdata->npos += 1;
			//Reallocate memory for posArray if it reach the limit size
			if (wdata->npos % 10 == 0)
				wdata->posarray = (short*)realloc(wdata->posarray, sizeof(short) * (wdata->npos + 11));
			wdata->posarray[wdata->npos - 1] = curTokenData->position;
		}
		else {
			WData* wdata = &wordArray[n];
			wdata->tf = unitTf;
			wdata->word = new wchar_t[wcslen(curWord) + 1];
			wcscpy(wdata->word, curWord);
			wdata->npos = 1;
			//Give the posArray a memory of ten elements
			wdata->posarray = (short*)malloc(sizeof(short) * 10);
			wdata->posarray[0] = curTokenData->position;
			n++;
		}
	}
	if (n != sortedTokensList.size) wordArray = (WData*)realloc(wordArray, sizeof(WData) * n);
	removeAll(sortedTokensList, freeTokenData);
	return wordArray;
}

/*	Remove stop words from a given TokenList in order:
*	- Remove all stop words that have two syllables
*	- Check for a repeat pair of word (including either of them is stop word or not)
*		+ occurrences variable stores the occurrence of the term "a b"
*		+ occurrencesfirst variable stores the occurrence of the term "a xxx"
*		+ occurrencessecond variable stores the occurrence of the term "xxx b"
*	- If occurrences / occurrencesfirst < threshold or occurrences / occurrencessecond < threshold then, the stop word in that term is no meaningful in that term
*	- else the stop word in that term contribute a meaning to that term
*/
SList rmvStopWords(SList& tokensList, lexicon stopwordlex, Config config) {
	int totalwords = tokensList.size;
	SList cleanTokensList;
	initialize(cleanTokensList);
	
	for (SNode* cur = tokensList.head; cur; cur = cur->next) {
		TokenData* curData = (TokenData*)cur->data;
		TokenData* curNextData = NULL;
		if (!curData->word) continue;
		//Merge two consecutive tokens & find in stop words list
		bool isSW = false;
		if (cur->next) {
			curNextData = (TokenData*)cur->next->data;
			wchar_t* strmerge = mergeWordTokenDatas(cur, cur->next);
			//wprintf(L"strmerge = %ls\n", strmerge);
			tnode* temp = findString(stopwordlex, strmerge);
			//Remove words that have two syllables are stop words
			if (temp) {
				//wprintf(L"Two-syllable stop word: %ls %ls\n", ((TokenData*)cur->data)->word, ((TokenData*)cur->next->data)->word);
				free(curData->word);
				free(curNextData->word);
				curData->word = NULL;
				curNextData->word = NULL;
				isSW = true;
			}
			free(strmerge);
		}

		int occurrences = 0;
		int occurrencesfirst = 0;
		int occurrencessecond = 0;
		//If two consecutive syllables is not a stop words then check one of them
		if (!isSW) {
			//Check for that word is stop word
			tnode* temp = findString(stopwordlex, curData->word);
			if (temp) isSW = true;
			if (cur->next && cur->next->next && cur->next->next->next) {
				//Check for	occurrence of two consecutive syllables
				bool isFstTokenSW = isTokenDataSW(stopwordlex, cur);
				bool isSecTokenSW = isTokenDataSW(stopwordlex, cur->next);
				//for (SNode* cur2 = cur->next->next; cur2 && cur2->next; cur2 = cur2->next) {
				for (SNode* cur2 = tokensList.head; cur2 && cur2->next; cur2 = cur2->next) {
					TokenData* cur2Data = (TokenData*)cur2->data;
					TokenData* cur2NextData = (TokenData*)cur2->next->data;
					if (!cur2Data->word || !cur2NextData->word) continue;
					bool isFstMatch = isMatchedTokenDatasbyWord(cur, cur2);
					bool isSecMatch = isMatchedTokenDatasbyWord(cur->next, cur2->next);
					if (isFstMatch && isSecMatch) {
						occurrences++;
						isSW = false;
					}
					if (isFstTokenSW && isSecMatch)	occurrencessecond++;
					if (isSecTokenSW && isFstMatch)	occurrencesfirst++;
				}
				occurrences--;
			}
			//If there is no occurrences of two consecutive syllables, then that stop word should be remove
			if (isSW) {
				//wprintf(L"One-syllable stop word: %ls %ls\n", ((TokenData*)cur->data)->word);
				free(curData->word);
				curData->word = NULL;
			}
			else {
				if (occurrencesfirst != 0 && occurrencessecond == 0) {
					double firstmatchTF = occurrences / ((double)occurrencesfirst);
					double tf = ((double)occurrences) / totalwords;
					//wprintf(L"%ls %ls - tf = %lf, fm = %lf\n", ((TokenData*)cur->data)->word, ((TokenData*)cur->next->data)->word, tf, firstmatchTF);
					//Check for the threshold
					if (firstmatchTF < config.THRESHOLD_STOP_WORD) {
						wchar_t* temp = curData->word;
						//wprintf(L"Mearningful --> %ls\n", temp);		
						occurrences = 0;
						isSW = false;
					}
				}
				if (occurrencesfirst == 0 && occurrencessecond != 0) {
					double secondmatchTF = occurrences / ((double)occurrencessecond);
					double tf = ((double)occurrences) / totalwords;
					//wprintf(L"%ls %ls - tf = %lf, sm = %lf\n", ((TokenData*)cur->data)->word, ((TokenData*)cur->next->data)->word, tf, secondmatchTF);
					//Check for the threshold
					if (secondmatchTF < config.THRESHOLD_STOP_WORD) {
						wchar_t* temp = curData->word;
						//wprintf(L"REMOVE --> %ls\n", temp);
						free(curData->word);
						curData->word = NULL;
						isSW = true;
					}
				}
				if (occurrencesfirst != 0 && occurrencessecond != 0) {
					double firstmatchTF = occurrences / ((double)occurrencesfirst);
					double secondmatchTF = occurrences / ((double)occurrencessecond);
					double tf = ((double)occurrences) / totalwords;
					//wprintf(L"%ls %ls - tf = %lf, fm = %lf, sm = %lf\n", ((TokenData*)cur->data)->word, ((TokenData*)cur->next->data)->word, tf, firstmatchTF, secondmatchTF);
					//Check for the threshold
					if (firstmatchTF < config.THRESHOLD_TWO_STOP_WORD && secondmatchTF < config.THRESHOLD_TWO_STOP_WORD) {
						wchar_t* temp = curData->word;
						//wprintf(L"REMOVE --> %ls\n", temp);				
						free(curData->word);
						curData->word = NULL;
						isSW = true;
					}
				}
			}
		}

		//Add to clean tokens list
		if (!isSW) {
			if (occurrences <= 0) {
				wchar_t* temp = cpywchar(curData->word);
				removeDiacritic(temp);
				TokenData tokdata1;
				tokdata1.position = curData->position;
				tokdata1.word = new wchar_t[wcslen(curData->word) + 1];
				wcscpy(tokdata1.word, temp);
				addTail(cleanTokensList, &tokdata1, sizeof(TokenData));
				free(temp);
			}
			else {
				//wchar_t* temp1 = curData->word;
				//wchar_t* temp2 = curNextData->word;
				wchar_t* temp1 = cpywchar(curData->word);
				wchar_t* temp2 = cpywchar(curNextData->word);
				removeDiacritic(temp1);
				removeDiacritic(temp2);
				TokenData tokdata1;
				tokdata1.position = curData->position;
				tokdata1.word = new wchar_t[wcslen(curData->word) + 1];
				wcscpy(tokdata1.word, temp1);
				TokenData tokdata2;
				tokdata2.position = curNextData->position;
				tokdata2.word = new wchar_t[wcslen(curNextData->word) + 1];
				wcscpy(tokdata2.word, temp2);
				addTail(cleanTokensList, &tokdata1, sizeof(TokenData));
				addTail(cleanTokensList, &tokdata2, sizeof(TokenData));
				free(temp1);
				free(temp2);
				cur = cur->next;
				if (!cur) break;
			}
		}
	}
	//printList(cleanTokensList, printTokenData);
	//wprintf(L"\n");
	wprintf(L"tokens = %d, cleanTokens = %d\n", tokensList.size, cleanTokensList.size);
	return cleanTokensList;
}

/*	Make word array from a list of wchar_t* (Method 1) - Simple
*	Simply count the occurences of every tokens and compute the term frequency
*/
WData* makeWordArray1(SList token, int& n) {
	if (isEmpty(token)) return NULL;
	SList dupTok;
	listcpy(dupTok, token, copywchar);
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

/*	Make word array from a list of wchar_t* (Method 2) - Advanced
*	Remove stop words before compute the term frequency. Last part of this function reuses makeWordArray1
*/
WData* makeWordArray2(SList token, int& n, lexicon stopwordlex, float thresholdStopWord) {
	if (isEmpty(token)) return NULL;

	SList dupTok;
	listcpy(dupTok, token, copywchar);
	int totalwords = dupTok.size;
	SList cleanTokens;
	initialize(cleanTokens);
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
				rmvwordcount += 1;
			}
			else {
				if (occurrencesfirst != 0 && occurrencessecond != 0) {
					double firstmatchTF = occurrences / ((double)occurrencesfirst);
					double tf = ((double)occurrences) / totalwords;
					//Open/close comment to print result
					//wprintf(L"%ls %ls - tf = %lf, fmtf = %lf\n", (wchar_t*)cur->data, (wchar_t*)cur->next->data, tf, firstmatchTF);
					if (firstmatchTF < thresholdStopWord) {
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
					//Open/close comment to print result
					//wprintf(L"%ls %ls - tf = %lf, smtf = %lf\n", (wchar_t*)cur->data, (wchar_t*)cur->next->data, tf, secondmatchTF);
					if (secondmatchTF < thresholdStopWord) {
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
	//Print clean tokens
	//printList(cleanTokens, printwchar);
	//printf("\n");
	wprintf(L"tokens = %d, cleanTokens = %d, remove words = %d\n", token.size, cleanTokens.size, rmvwordcount);
	removeAll(dupTok, rmvwchar);
	WData* res = makeWordArray1(cleanTokens, n);
	removeAll(cleanTokens, rmvwchar);
	return res;
}

/*	Make word array (Method 3) - Sophisticated
*	Save the position of each tokens, remove stop words and compute the term frequency.
*/
WData* makeWordArray3(SList token, int& n, lexicon stopwordlex, Config config) {
	if (isEmpty(token)) return NULL;
	//Convert raw token list to tokensList whose elements are TokenData
	SList tokensList = makeTokensList(token);
	SList cleanTokensList = rmvStopWords(tokensList, stopwordlex, config);
	removeAll(tokensList, freeTokenData);
	WData* res = makeWordArrayFromTokensList(cleanTokensList, n);
	removeAll(cleanTokensList, freeTokenData);
	return res;
}

/*	Make word array that contains info of words from a token
*	Return a word array
*/
WData* mkWordArray(SList token, int &n, lexicon stopwordlex, Config config) {
	return makeWordArray3(token, n, stopwordlex, config);
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
	fclose(fdocidxor);
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
	fwrite(&wdata.npos, sizeof(wdata.npos), 1, fbarrel);
	fwrite(wdata.posarray, sizeof(short), wdata.npos, fbarrel);
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
				//printf("wldata dlistAdd = %d\n", ((WLData*)charNode->adapter)->dlistAdd);
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
		if (docArray[doci].docId < 0) continue;
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
	if (remove(barrelfname)) {
		status = false;
		wprintf(L"Unable to remove file\n");
	}
	if (rename(cachebarrelfname, barrelfname)) {
		status = false;
		wprintf(L"Unable to rename file\n");
	}
	if (remove(docidxorfname)) {
		status = false;
		wprintf(L"Unable to remove file\n");
	}
	if (rename(cachedocixorfname, docidxorfname)) {
		status = false;
		wprintf(L"Unable to rename file\n");
	}
	free(cachebarrelfname);
	free(cachedocixorfname);
	return status;
}

/*	Preprocess a file and return a word array.
*/
WData* preprocessFile(char* filename, int& nword, lexicon stopwordlex, Config config) {
	//Read file
	clock_t begin = clock();
	wchar_t* text = readFile(filename);
	//removeDiacritic(text);
	normalize(text);
	rmvRedundant(text);
	SList wordList = splitword(text, L' ');
	free(text);
	wprintf(L"Complete read file time spent = %lf\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
	
	//Make word array
	nword = 0;
	WData* wordArray = mkWordArray(wordList, nword, stopwordlex, config);
	removeAll(wordList, rmvwchar);
	return wordArray;
}

/*	Indexing (Method 1) 
*	Lexicon will be doclist lexicon while running; after run through all files, save to barrel indexor and convert to wldata lexicon
*	Pros: Faster
*	Cons: Consume more RAM memory
*/
void indexing1(lexicon& mainlex, Config config) {
	clock_t beginTotal = clock();
	init(mainlex);

	FILE* fbarrel = fopen(config.BARRELS_FILES, "wb");
	FILE* findex = fopen(config.INDEX_FILES, "r");
	FILE* fdocidxor = fopen(config.DOC_INDEXOR, "wb");
	lexicon swlex = mkstopwordLex(config.STOPWORD_FILES);
	int nfile = 0;
	char** listfile = readCorpus(findex, nfile);
	fclose(findex);

	for (int filei = 0; filei < nfile; filei++) {
		int docId = filei;
		wchar_t* tempfname = chr2wchr(listfile[filei]);
		wprintf(L"File %d in total %d files, file name = %s\n", filei, nfile, tempfname);
		free(tempfname);

		clock_t begin = clock();
		int nword = 0;
		WData* wordArray = preprocessFile(listfile[filei], nword, swlex, config);
		
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
		adtWAdd2Lex(mainlex, filei, addressArray, nword);
		freeAddressArray(addressArray, nword);
		//printLexicon(mainlex, printDoclist);
		wprintf(L"Complete write barrel time spent = %lf\n", (double)(clock() - begin) / CLOCKS_PER_SEC);
	}

	//Write barrel indexor & lexicon
	wprintf(L"-------------------------------------------------------\n");
	clock_t begin = clock();
	FILE* fmainlex = fopen(config.LEXICON_FILES, "wb");
	FILE* fbarrelindexor = fopen(config.BARRELS_INDEXOR_FILES, "wb");
	saveBIandLex(fbarrelindexor, fmainlex, mainlex);
	//printLexicon(mainlex, printwldata);
	for (int i = 0; i < nfile; i++) free(listfile[i]);
	free(listfile);
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
void indexing2(lexicon& mainlex, Config config) {
	clock_t beginTotal = clock();
	init(mainlex);

	FILE* fbarrel = fopen(config.BARRELS_FILES, "wb");
	FILE* findex = fopen(config.INDEX_FILES, "r");
	FILE* fdocidxor = fopen(config.DOC_INDEXOR, "wb");
	FILE* fbarrelindexor = fopen(config.BARRELS_INDEXOR_FILES, "wb+");
	lexicon swlex = mkstopwordLex(config.STOPWORD_FILES);

	int nfile = 0;
	char** listfile = readCorpus(findex, nfile);
	fclose(findex);

	for (int filei = 0; filei < nfile; filei++) {
		int docId = filei;
		wchar_t* tempfname = chr2wchr(listfile[filei]);
		wprintf(L"FILE %d trong tong so %d, FILE NAME = %s\n", filei, nfile, tempfname);
		free(tempfname);

		clock_t begin = clock();
		int nword = 0;
		WData* wordArray = preprocessFile(listfile[filei], nword, swlex, config);
		
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
	FILE* fmainlex = fopen(config.LEXICON_FILES, "wb");
	saveLex(fmainlex, mainlex);
	printLexicon(mainlex, printwldata);
	for (int i = 0; i < nfile; i++) free(listfile[i]);
	free(listfile);
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
int doIndexing(lexicon& mainlex, Config config) {
	indexing1(mainlex, config);
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

/*	Index new file from a given doc directory
*	Update doc indexor and lexicon. Return 0 for successful adding; negative number for failure
*/
int indexNewFile(char* dir, DocIndexor& docidxor, lexicon& lex, Config config) {
	//Check for a valid document
	FILE* ftemp = fopen(dir, "r");
	if (!ftemp) return -1;
	fclose(ftemp);

	FILE* fbarrel = fopen(config.BARRELS_FILES, "ab");
	FILE* fbarrelindexor = fopen(config.BARRELS_INDEXOR_FILES, "ab+");
	FILE* fdocidxor = fopen(config.DOC_INDEXOR, "ab");
	FILE* flex = fopen(config.LEXICON_FILES, "wb");
	lexicon swlex = mkstopwordLex(config.STOPWORD_FILES);
	DocData* pdocData = addDoc2DocIndexor(dir, docidxor);
	int nword = 0;
	wchar_t* tempfname = chr2wchr(dir);
	wprintf(L"Indexing file %ls\n", tempfname);
	free(tempfname);
	WData* wordArray = preprocessFile(dir, nword, swlex, config);
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
	//Two following code lines should be used carefully to avoid time consuming
	wprintf(L"Please wait to rewrite metadata...\n");
	rewriteBIandLex(config.LEXICON_FILES, config.BARRELS_INDEXOR_FILES, lex);
	rewriteBarrelandDocIdxor(config.DOC_INDEXOR, config.BARRELS_FILES, docidxor);
	return 0;
}

/*	Remove a file
*	Update doc indexor and lexicon. Return 0 for successful removing; negative number for failure
*/
int removeFile(int docId, DocIndexor& docidxor, lexicon& lex, Config config) {
	FILE* fbarrel = fopen(config.BARRELS_FILES, "rb");
	FILE* fbarrelindexor = fopen(config.BARRELS_INDEXOR_FILES, "ab+");
	FILE* flex = fopen(config.LEXICON_FILES, "wb");
	DocData* docdata = rmvDocInDocIndexor(docId, docidxor);
	if (!docdata) return -1;
	WData* wordArray = getWordArray(fbarrel, docdata->nword, docdata->docAdd);
	//printWordArray(wordArray, docdata->nword);
	rmvWDDataInDlist(fbarrelindexor, lex, wordArray, docId, docdata->nword);
	//printLexicon(lex, printwldata);
	saveLex(flex, lex);
	//printDocIndexor(docidxor);
	saveDocIndexor(config.DOC_INDEXOR, docidxor);
	fclose(fbarrel);
	fclose(fbarrelindexor);
	fclose(flex);
	//Two following code lines should be used carefully to avoid time consuming
	wprintf(L"Please wait to rewrite metadata...\n");
	rewriteBIandLex(config.LEXICON_FILES, config.BARRELS_INDEXOR_FILES, lex);
	rewriteBarrelandDocIdxor(config.DOC_INDEXOR, config.BARRELS_FILES, docidxor);
	return 0;
}
