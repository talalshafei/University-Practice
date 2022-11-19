#pragma once

#include"patientNode.h"

// regular linked list (using dummy node)

struct ListRecord {
	struct Node* head;
	struct Node* tail;
	int size;
};

typedef struct ListRecord* List;

List createList();
void makeEmptyList(List);
void insertOnArrival(List, patInfo);// insertint with respect to arrival time
patInfo removePat(List);
int IsEmptyList(List);