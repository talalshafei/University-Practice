#include"Linkedlist.h"

List createList() {


	List L = (List)malloc(sizeof(struct ListRecord));
	if (L == NULL) {
		printf("Out of space!!\n");
		exit(-1);
	}

	//using dummy node
	L->head = (lNode)malloc(sizeof(struct ListNode));

	if (L->head == NULL) {
		printf("Out of space!!\n");
		exit(-1);
	}

	L->head->next = NULL;
	L->tail = L->head;
	L->size = 0;

	return L;
}

void insertInList(List L,lNode node) {


	L->tail->next = node;
	L->tail = L->tail->next;

	L->size++;
}

lNode createListNode() {

	lNode node = (lNode)malloc(sizeof(struct ListNode));

	if (node == NULL) {
		printf("Out of space!!\n");
		exit(-1);
	}
	node->next = NULL;

	return node;

}
