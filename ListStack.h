#pragma once
#include "SList.h"

struct Stack {
	SNode* top;
};

void init(Stack& stack);

bool isEmpty(Stack stack);

bool push(Stack& stack, void* data, int data_size);

bool pop(Stack& stack, void*& data);

bool pop(Stack& stack, void*& data, void (*removeElement)(void*));

bool peek(Stack& stack, void*& data);