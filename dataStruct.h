#pragma once
/*Word Data*/
struct WData {
	wchar_t* word;
	float tf;
};
/*Word Address Data*/
struct WAData {
	wchar_t* word;
	int wordAdd;
};
/*Word in Doc Data*/
struct WDData {
	int docId;
	int wordAdd;
};
/*A list of docs that con contain a specific word*/
struct Doclist {
	int ndoc;
	WDData* docArray;
};

/*Word in Lexicon Data*/
struct WLData {
	short nchar;
	wchar_t* word;
	int dlistAdd;
	int ndoc;
};

/*Searched doc data*/
struct SDData {
	int docId;
	int nword;
	WDData* wordAddArray;
	float score;
};

/*Doc data in doc indexor*/
struct DocData {
	int docId;
	int nchardir;
	char* docdir;
	int nword;
	int docAdd;
};

struct DocIndexor {
	int ndocs;
	DocData* docarray;
};