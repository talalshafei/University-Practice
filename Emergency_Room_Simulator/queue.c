#include <stdio.h>
#include"queue.h"


int IsEmptyQueue(Queue q) {
	return q->size == 0;
}

Queue createQueue() {
	Queue q = (Queue)malloc(sizeof(struct QueueRecord));
	if (q == NULL) {
		printf("\n***Couldn't Allocate memory***\n");
		exit(1);
	}
	makeEmptyQueue(q);
	return q;
}

void makeEmptyQueue(Queue q) {
	q->front = (struct Node*)malloc(sizeof(struct Node));
	if (q->front == NULL) {
		printf("\n***Couldn't Allocate memory***\n");
		exit(1);
	}
	q->front->next = NULL;
	q->rear = q->front->next;
	q->size = 0;

}

patInfo Dequeue(Queue q) {
	/// ***///
	//thats how I saw Hocam doing it
	// I think we can do this with easier set a tmp to the front the moving the front and set tmp next to null and return tmp
	// I used the second method with the linked list remove function

	if (!IsEmptyQueue(q)) {
		patInfo tmp = (patInfo)malloc(sizeof(struct Node));
		if (tmp == NULL) {
			printf("\n***Couldn't Allocate memory***\n");
			exit(1);
		}
		tmp->type = q->front->next->type;
		tmp->arrival = q->front->next->arrival;
		tmp->serv = q->front->next->serv;
		tmp->doctor = q->front->next->doctor;
		tmp->insuarance = q->front->next->insuarance;
		tmp->start = q->front->next->start;
		tmp->next = NULL;
		
		struct Node* removeNode = q->front->next;
		q->front->next=q->front->next->next;
		q->size--;
		free(removeNode);
		return tmp;
	}
	else {
		printf("\n** ERROR QUEUE IS EMPTY -can't Dequeue- **\n");
	}
}

void priorityEnqueue(Queue q, patInfo newPatient) {

	// if the queue is empty add the element
	if (IsEmptyQueue(q)) {
		q->front->next = newPatient;
		q->rear = newPatient;
	}
	// else check for the priority
	else {
		//if it was larger put him the first in the queue
		if (newPatient->type > q->front->next->type) {
			newPatient->next = q->front->next;
			q->front->next = newPatient;
		}
		// if it was less or equal the last one put him the last one
		else if (newPatient->type <= q->rear->type) {
			q->rear->next = newPatient;
			q->rear = q->rear->next;
		}
		// here we find the right position to insert him
		else {
			struct Node* location = q->front->next;
			int i = 0;
			int Done = 0;//flag to terminate the loop whe finished adding
			//location->next != NULL because I already checked for the last element
			for (i = 0; location->next != NULL && !Done; i++) {
				// if the patient type is less than the location->next then move the location until patient type is bigger
				if (newPatient->type <= location->next->type)
					location = location->next;
				else {
					// add the patient
					newPatient->next = location->next;
					location->next = newPatient;
					Done = 1;
				}
			}
		}
	}
	q->size++;
}
