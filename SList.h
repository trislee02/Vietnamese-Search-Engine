#pragma once
#include <stdio.h>
#include <stdlib.h>

struct SNode {
	void* data;
	SNode* next;
};

struct SList {
	SNode* head;
	int size;
};

void initialize(SList& list);

bool isEmpty(SList list);

SNode* findI(int index, SList list);

SNode* findData(SList list, void* data, int (*compare)(void*, void*));

SNode* findTail(SList list);

SNode* addHead(SList& list, void* data, int data_size);

SNode* addTail(SList& list, void* data, int data_size);

void addAfter(SList& list, void* data, int data_size, int index);

SNode* addKeepOrder(SList& list, void* data, int data_size, int (*compare)(void*, void*));

void removeHead(SList& list, void (*removeElement)(void* data));

void removeTail(SList& list, void (*removeElement)(void* data));

void removeI(SList& list, int index, void (*removeElement)(void* data));

void removeData(SList& list, void* data, int (*compare)(void*, void*), void (*removeElement)(void* data));

void removeAll(SList& list, void (*removeElement)(void* data));

void reverse(SList& list);

SNode* findMid(SList list);

void mergeSortedList(SList& la, SList lb, int (*compare)(void*, void*), int condition);

void mergesort(SList& list, int (*compare)(void*, void*), int condition);

void listcpy(SList& des, SList src, void* (*copyElement)(void*));

void printList(SList& list, void (*print)(void*));

void filterKfirst(SList& list, int k, void (*removeElement)(void* data));

SNode* findDataBinary(SList list, void* data, int (*compare)(void*, void*));