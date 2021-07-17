#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SList.h"
#include "dataStruct.h"

#define SUCCESS_MESSAGE L"Success\n"
#define FAIL_MESSAGE L"Fail\n"

#define CONFIG_FILE "config/config.txt"

#define INDEX_FILES_KEY "index-file"
#define STOPWORD_FILES_KEY "stopwords-file"

#define BARRELS_FILES_KEY "barrels-filename"
#define BARRELS_INDEXOR_FILES_KEY "barrel-indexor-filename"
#define LEXICON_FILES_KEY "lexicon-filename"
#define DOC_INDEXOR_KEY "doc-indexor-filename"

#define THRESHOLD_STOP_WORD_KEY "threshold-stop-word"
#define THRESHOLD_TWO_STOP_WORD_KEY "threshold-two-stop-word"

#define K_BEST_DOC_KEY "k-best-doc"
#define DISTANCE_MINIMAL_INTERVAL_KEY "distance-minimal-interval"
#define EXPONENTIAL_MATCHED_TOKEN_KEY "exponential-matched-token"
#define DISTANCE_ORDERED_PAIR_KEY "distance-ordered-pair"
#define EXPONENTIAL_ORDER_PAIR_KEY "exponential-order-pair"
#define WEIGHT_DISTANCE_MINIMAL_INTERVAL_KEY "weight-distance-minimal-interval"

#define WRITE_SEARCHING_LOG_KEY "write-searching-log"


Config getConfigData(const char* configfname);

char* generateSearchingLogName(SList tokens);