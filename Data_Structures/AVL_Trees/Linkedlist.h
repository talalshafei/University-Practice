#pragma once


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct ListNode* lNode;

struct ListNode {
	char name[31];
	char genre[15];
	char album[31];
	int year;
	char date[31];
	lNode next;
};

/*struct ListNode {
	char* name;
	char* genre;
	char* album;
	int year;
	char* date;
	lNode next;
};*/

struct ListRecord {
	lNode head;
	lNode tail;
	int size;

};
typedef struct ListRecord* List;

lNode createListNode();
List createList();
void insertInList(List,lNode);


