#pragma once
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "SList.h"
#include "TextProcessing.h"
#include "Searcher.h"
#include "Indexor.h"
#include "Constant.h"

void Menu();
char** Indexing(lexicon& mainlex);
void Search(lexicon mainlex, int kBestDoc, char** listfile);
bool ParaSetting();
void Run();