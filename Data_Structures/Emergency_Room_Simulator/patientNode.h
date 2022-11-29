#pragma once

// node declaration

#include<stdio.h>
#include<stdlib.h>

struct Node {
	int type;
	int arrival;
	int serv;
	int start;
	int doctor;
	int insuarance;
	struct Node* next;
};
typedef struct Node* patInfo;