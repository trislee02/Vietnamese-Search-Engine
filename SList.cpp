#include "SList.h"

void initialize(SList& list) {
	list.head = nullptr;
	list.size = 0;
}

bool isEmpty(SList list) {
	return list.head == nullptr;
}

SNode* findI(int index, SList list) {
	if (index < 0) return nullptr;
	for (SNode* cur = list.head; cur; cur = cur->next) {
		if (index == 0) return cur;
		index--;
	}
	return nullptr;
}

SNode* findData(SList list, void* data, int (*compare)(void*, void*)) {
	for (SNode* cur = list.head; cur; cur = cur->next) {
		if (compare(cur->data, data) == 0) return cur;
	}
	return nullptr;
}

SNode* findTail(SList list) {
	SNode* cur;
	for (cur = list.head; cur->next; cur = cur->next);
	return cur;
}

SNode* addHead(SList& list, void* data, int data_size) {
	SNode* node = (SNode*) malloc(sizeof(SNode));
	node->data = malloc(data_size);
	char* ptr = (char*)data;
	for (int i = 0; i < data_size; i++)
		((char*)(node->data))[i] = ptr[i];
	node->next = list.head;
	list.head = node;
	list.size++;
	return node;
}

SNode* addTail(SList& list, void* data, int data_size) {
	if (isEmpty(list)) 
		return addHead(list, data, data_size);
	else {
		SNode* tail = findTail(list);
		SNode* node = (SNode*)malloc(sizeof(SNode));
		node->data = malloc(data_size);
		char* ptr = (char*)data;
		for (int i = 0; i < data_size; i++)
			((char*)(node->data))[i] = ptr[i];
		node->next = nullptr;
		tail->next = node;
		list.size++;
		return node;
	}
	
}

void addAfter(SList& list, void* data, int data_size, int index) {
	SNode* nodei = findI(index, list);
	if (!nodei) addHead(list, data, data_size);
	else if (!nodei->next) addTail(list, data, data_size);
	else {
		SNode* node = (SNode*)malloc(sizeof(SNode));
		node->data = malloc(data_size);
		char* ptr = (char*)data;
		for (int i = 0; i < data_size; i++)
			((char*)(node->data))[i] = ptr[i];
		node->next = nodei->next;
		nodei->next = node;
		list.size++;
	}
}

SNode* addKeepOrder(SList& list, void* data, int data_size, int (*compare)(void*, void*)) {
	SNode* prev = nullptr;
	for (SNode* cur = list.head; cur; cur = cur->next) {
		if (compare(data, cur->data) == 1 || compare(data, cur->data) == 0) {
			SNode* node = (SNode*)malloc(sizeof(SNode));
			node->data = malloc(data_size);
			char* ptr = (char*)data;
			for (int i = 0; i < data_size; i++)
				((char*)(node->data))[i] = ptr[i];
			node->next = cur;
			if (prev) prev->next = node;
			else list.head = node;
			list.size++;
			return node;
		}
		prev = cur;
	}
	return addTail(list, data, data_size);
}

void removeHead(SList& list, void (*removeElement)(void* data)) {
	if (isEmpty(list)) return;
	SNode* head = list.head;
	list.head = list.head->next;
	removeElement(head->data);
	free(head);
	list.size--;
}

void removeTail(SList& list, void (*removeElement)(void* data)) {
	if (isEmpty(list)) return;
	SNode* cur;
	for (cur = list.head; cur->next && cur->next->next; cur = cur->next);
	if (!cur->next) 
		removeHead(list, removeElement);
	else {
		SNode* tail = cur->next;
		cur->next = nullptr;
		removeElement(tail->data);
		list.size--;
		free(tail);
	}
}

void removeI(SList& list, int index, void (*removeElement)(void* data)) {
	if (index == 0) removeHead(list, removeElement);
	else {
		SNode* prev = nullptr;
		for (SNode* cur = list.head; cur; cur = cur->next) {
			if (index == 0) {
				if (prev) prev->next = cur->next;
				else list.head = cur->next;
				removeElement(cur->data);
				free(cur);
				list.size--;
				return;
			}
			prev = cur;
			index--;
		}
	}
}

void removeData(SList& list, void* data, int (*compare)(void*, void*), void (*removeElement)(void* data)) {
	SNode* prev = nullptr;
	for (SNode* cur = list.head; cur; cur = cur->next) {
		if (compare(cur->data, data) == 0) {
			if (prev) prev->next = cur->next;
			else list.head = cur->next;
			removeElement(cur->data);
			free(cur);
			list.size--;
			return;
		}
		prev = cur;
	}
}

void removeAll(SList& list, void (*removeElement)(void* data)) {
	while (!isEmpty(list)) removeHead(list, removeElement);
}

void reverse(SList& list) {
	SNode* prev = nullptr;
	for (SNode* cur = list.head; cur;) {
		SNode* next = cur->next;
		cur->next = prev;
		prev = cur;
		list.head = cur;
		cur = next;
	}
}

SNode* findMid(SList list) {
	if (isEmpty(list)) return NULL;
	SNode* slow = list.head;
	SNode* fast = list.head->next;
	while (fast && fast->next) {
		slow = slow->next;
		fast = fast->next->next;
	}
	return slow;
}

void mergeSortedList(SList& la, SList lb, int (*compare)(void*, void*), int condition) {
	if (isEmpty(la)) la.head = lb.head;
	else if (isEmpty(lb)) return;
	SNode* cura = la.head;
	SNode* curb = lb.head;
	if (compare(cura->data, curb->data) == condition) {
		la.head = cura;
		cura = cura->next;
	}
	else {
		la.head = curb;
		curb = curb->next;
	}
	SNode* cur = la.head;
	while (cura && curb) {
		if (compare(cura->data, curb->data) == condition) {
			cur->next = cura;
			cura = cura->next;
		}
		else {
			cur->next = curb;
			curb = curb->next;
		}
		cur = cur->next;
	} 
	cur->next = cura ? cura : curb;
}

void mergesort(SList& list, int (*compare)(void*, void*), int condition) {
	if (!list.head || !list.head->next)
		return;
	SList secpart;
	initialize(secpart);
	SNode* mid = findMid(list);
	secpart.head = mid->next;
	mid->next = NULL;
	mergesort(list, compare, condition);
	mergesort(secpart, compare, condition);
	mergeSortedList(list, secpart, compare, condition);
}

void listcpy(SList& des, SList src, void* (*copyElement)(void *)) {
	initialize(des);
	if (isEmpty(src)) return;
	SNode* descur = (SNode*) malloc(sizeof(SNode));
	descur->data = copyElement(src.head->data);
	descur->next = NULL;
	des.head = descur;
	for (SNode* cur = src.head->next; cur; cur = cur->next, descur = descur->next) {
		SNode* node = (SNode*)malloc(sizeof(SNode));
		node->data = copyElement(cur->data);
		node->next = NULL;
		descur->next = node;
	}
	des.size = src.size;
}

void printList(SList& list, void (*print)(void*)) {
	for (SNode* cur = list.head; cur; cur = cur->next) {
		print(cur->data);
	}
}

void filterKfirst(SList& list, int k, void (*removeElement)(void* data)) {
	if (isEmpty(list) || k <= 0 || list.size <= k) return;
	list.size = k;
	for (SNode* cur = list.head; cur; cur = cur->next) {
		if (k == 1) {
			SList temp;
			initialize(temp);
			temp.head = cur->next;
			cur->next = NULL;
			removeAll(temp, removeElement);
			break;
		}
		k--;
	}
	
}

SNode* findMiddle(SNode* start, SNode* last) {
	if (!start) return NULL;
	SNode* slow = start;
	SNode* fast = start->next;
	while (fast != last) {
		fast = fast->next;
		if (fast != last) {
			fast = fast->next;
			slow = slow->next;
		}
	}
	return slow;
}

SNode* findDataBinary(SList list, void* data, int (*compare)(void*, void*)) {
	SNode* start = list.head;
	SNode* last = NULL;
	do {
		SNode* mid = findMiddle(start, last);
		if (mid == NULL) return NULL;
		int cmp = compare(mid->data, data);
		if (cmp == 0) return mid;
		else if (cmp < 0) last = mid;
		else start = mid->next;
	} while (last == NULL || last != start);
	return NULL;
}