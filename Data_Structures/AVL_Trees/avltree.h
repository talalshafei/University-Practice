#pragma once

#include"Linkedlist.h"
typedef struct AvlTreeNode* Tree;
struct AvlTreeNode {
    // char* artist;
	char artist[30];
	List songs;
	Tree left;
	Tree right;
	int height;
};

Tree CreateAvlTree();
void PrintTreeNode(Tree);
Tree insert(Tree , char[], lNode );
int height(Tree);
int maximum(int, int);
Tree newNode();
Tree SingleRotateWithLeft(Tree);
Tree SingleRotateWithRight(Tree);
Tree DoubleRotateWithLeft(Tree);
Tree DoubleRotateWithRight(Tree);
int checkArtistName(Tree , char[], lNode);
