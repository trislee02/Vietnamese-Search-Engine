#pragma once
/*Word Data*/
struct WData {
	wchar_t* word;
	float tf;
	short npos;
	short* posarray;
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

struct TokenData {
	wchar_t* word;
	short position;
};

struct EleMinInterval {
	int keywordNo;
	short position;
};

struct Config {
	char INDEX_FILES[100];
	char STOPWORD_FILES[100];
	char BARRELS_FILES[100];
	char BARRELS_INDEXOR_FILES[100];
	char LEXICON_FILES[100];
	char DOC_INDEXOR[100];
	int K_BEST_DOC_DEFAULT;
	float THRESHOLD_STOP_WORD;			//Parameter to remove stopword when indexing
	float THRESHOLD_TWO_STOP_WORD;		//Parameter to remove stopword when indexing
	int DISTANCE_MINIMAL_INTERVAL;		//Parameter to limit the distance for a valid minimal interval
	bool WRITE_SEARCHING_LOG;
	int EXPONENTIAL_MATCHED_TOKEN;		//Parameter to weight the number of matched tokens
	int DISTANCE_ORDERED_PAIR;			//Parameter to limit the distance between two ordered pair
	int EXPONENTIAL_PROXIMITY;			//Parameter to weight the proximity score
};