#include"Linked.h"

List createList() {
	List L = (List)malloc(sizeof(struct ListRecord));
	if (L == NULL) {
		printf("\n***Couldn't Allocate memory***\n");
		exit(1);
	}
	makeEmptyList(L);
	return L;
}

void makeEmptyList(List L) {
	L->head = (patInfo)malloc(sizeof(struct Node));
	if (L ->head == NULL) {
		printf("\n***Couldn't Allocate memory***\n");
		exit(1);
	}
	L->head->next = NULL;
	L->tail = L->head;
	L->size = 0;
}

void insertOnArrival(List L, patInfo newPatient) {
	// if the list is empty insert the element
	if (IsEmptyList(L)) {
		L->head->next = newPatient;
		L->tail = newPatient;
	}
	// else check for the arrival condition
	else {
		//if the patient arrival time was bigger or equal to the last one in the list put him the last one
		if (newPatient->arrival >= L->tail->arrival) {
			L->tail->next = newPatient;
			L->tail = L->tail->next;
		}
		else {
			// check for the patient location
			patInfo Location = L->head;
			int i = 0;
			int Done = 0;
			for (i = 0; Location->next != NULL && !Done; i++) {
				// if his arrival time was bigger or equal move the pointer
				if (newPatient->arrival >= Location->next->arrival)
					Location = Location->next;
				else {
					// add the patient to the right location
					newPatient->next = Location->next;
					Location->next = newPatient;
					Done = 1;
				}
			}
		}
	}
	L->size++;
}

int IsEmptyList(List L) {
	return L->size == 0;
}
patInfo removePat(List L) {
	if (!IsEmptyList(L)) {
		// if the list is not empty return the first node
		patInfo removeNode = L->head->next;
		L->head->next = L->head->next->next;
		L->size--;
		removeNode->next = NULL;
		return removeNode;
	}
	else printf("\n**ERROR can't remove Patient from Empty List**\n");
}

