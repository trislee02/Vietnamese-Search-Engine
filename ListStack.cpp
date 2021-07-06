#include "ListStack.h"

void init(Stack& stack) {
	stack.top = nullptr;
}

bool isEmpty(Stack stack) {
	return stack.top == nullptr;
}

bool push(Stack& stack, void* data, int data_size) {
	SNode* node = (SNode*)malloc(sizeof(SNode));
	if (!node) return false;
	node->data = malloc(data_size);
	char* ptr = (char*)data;
	for (int i = 0; i < data_size; i++)
		((char*)(node->data))[i] = ptr[i];
	node->next = stack.top;
	stack.top = node;
	//printf("node: %c\n", (SNode*)(node->data)->data)
	return true;
}

bool pop(Stack& stack, void*& data) {
	if (isEmpty(stack)) return false;
	SNode* top = stack.top;
	stack.top = stack.top->next;
	data = top->data;
	free(top);
	return true;
}

bool pop(Stack& stack, void*& data, void (*removeElement)(void*)) {
	if (isEmpty(stack)) return false;
	SNode* top = stack.top;
	stack.top = stack.top->next;
	data = top->data;
	
	free(top);
	return true;
}

bool peek(Stack& stack, void*& data) {
	if (isEmpty(stack)) return false;
	data = stack.top->data;
	return true;
}