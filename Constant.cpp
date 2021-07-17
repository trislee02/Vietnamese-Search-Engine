#include "Constant.h"

Config getConfigData(const char* configfname) {
	Config config = { NULL, NULL, NULL, NULL, NULL, NULL, 10, 0.8, 0 };
	FILE* fconfig = fopen(CONFIG_FILE, "r");
	if (!fconfig) return config;
	while (!feof(fconfig)) {
		char str[100];
		fscanf(fconfig, "%s", &str);
		if (strcmp(str, INDEX_FILES_KEY) == 0) {
			fscanf(fconfig, "%s", &config.INDEX_FILES);
		}
		else if (strcmp(str, STOPWORD_FILES_KEY) == 0) {
			fscanf(fconfig, "%s", &config.STOPWORD_FILES);
		}
		else if (strcmp(str, BARRELS_FILES_KEY) == 0) {
			fscanf(fconfig, "%s", &config.BARRELS_FILES);
		}
		else if (strcmp(str, BARRELS_INDEXOR_FILES_KEY) == 0) {
			fscanf(fconfig, "%s", &config.BARRELS_INDEXOR_FILES);
		}
		else if (strcmp(str, LEXICON_FILES_KEY) == 0) {
			fscanf(fconfig, "%s", &config.LEXICON_FILES);
		}
		else if (strcmp(str, DOC_INDEXOR_KEY) == 0) {
			fscanf(fconfig, "%s", &config.DOC_INDEXOR);
		}
		else if (strcmp(str, K_BEST_DOC_KEY) == 0) {
			fscanf(fconfig, "%d", &config.K_BEST_DOC_DEFAULT);
		}
		else if (strcmp(str, THRESHOLD_STOP_WORD_KEY) == 0) {
			fscanf(fconfig, "%f", &config.THRESHOLD_STOP_WORD);
		}
		else if (strcmp(str, DISTANCE_MINIMAL_INTERVAL_KEY) == 0) {
			fscanf(fconfig, "%d", &config.DISTANCE_MINIMAL_INTERVAL);
		}
		else if (strcmp(str, WRITE_SEARCHING_LOG_KEY) == 0) {
			int temp = 0;
			fscanf(fconfig, "%d", &temp);
			if (temp == 0) config.WRITE_SEARCHING_LOG = false;
			else config.WRITE_SEARCHING_LOG = true;
		}
		else if (strcmp(str, EXPONENTIAL_MATCHED_TOKEN_KEY) == 0) {
			fscanf(fconfig, "%d", &config.EXPONENTIAL_MATCHED_TOKEN);
		}
		else if (strcmp(str, DISTANCE_ORDERED_PAIR_KEY) == 0) {
			fscanf(fconfig, "%d", &config.DISTANCE_ORDERED_PAIR);
		}
		else if (strcmp(str, EXPONENTIAL_ORDER_PAIR_KEY) == 0) {
			fscanf(fconfig, "%d", &config.EXPONENTIAL_ORDER_PAIR);
		}
		else if (strcmp(str, THRESHOLD_TWO_STOP_WORD_KEY) == 0) {
			fscanf(fconfig, "%f", &config.THRESHOLD_TWO_STOP_WORD);
		}
		else if (strcmp(str, WEIGHT_DISTANCE_MINIMAL_INTERVAL_KEY) == 0) {
			fscanf(fconfig, "%f", &config.WEIGHT_DISTANCE_MINIMAL_INTERVAL);
		}
	}
	fclose(fconfig);
	return config;
}

char* generateSearchingLogName(SList tokens) {
	if (isEmpty(tokens)) return NULL;
	char ftemp[1000] = "searching-log/";
	char tokenchr[100];
	int ret;
	wchar_t* tokenwchr = (wchar_t*)tokens.head->data;
	ret = wcstombs(tokenchr, tokenwchr, sizeof(tokenchr));
	if (ret) strcat(ftemp, tokenchr);
	for (SNode* cur = tokens.head->next; cur; cur = cur->next) {
		tokenwchr = (wchar_t*)cur->data;
		ret = wcstombs(tokenchr, tokenwchr, sizeof(tokenchr));
		if (ret) {
			strcat(ftemp, "-");
			strcat(ftemp, tokenchr);
		}
	}
	strcat(ftemp, ".txt");
	char* res = new char[strlen(ftemp) + 1];
	strcpy(res, ftemp);
	return res;
}