#pragma once
#include "patientNode.h"

// regular priority queue (using dummy node)

struct QueueRecord {
	struct Node* front;
	struct Node* rear;
	int size;
};
typedef struct QueueRecord* Queue;

void priorityEnqueue(Queue, patInfo);
patInfo Dequeue(Queue);
int IsEmptyQueue(Queue);
void makeEmptyQueue(Queue);
Queue createQueue();
