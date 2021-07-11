#pragma once
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "SList.h"
#include "TextProcessing.h"
#include "Searcher.h"
#include "Indexor.h"
#include "Constant.h"

int Menu();
void Indexing(lexicon& mainlex, Config config);
void Search(lexicon mainlex, DocIndexor docidxor, Config config);
bool ParaSetting();
void Run();