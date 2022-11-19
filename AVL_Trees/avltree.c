
#include"avltree.h"
#include"Linkedlist.c"


Tree CreateAvlTree() {

	//declare root
	Tree root = NULL;

	return root;
}





Tree insert(Tree t, char artistName[],lNode song) {

	if (t == NULL) {
		// if t point to null create newNode
		t = newNode();
		strcpy(t->artist, artistName);
		insertInList(t->songs, song);
	}

	// if x < t->artist insert song and the artist  in the left subtree
	else if (strcmp(artistName,t->artist)<0) {
		t->left = insert(t->left, artistName,song);
		// check the balance
		if (height(t->left) - height(t->right) == 2)
			//LL case
			if (strcmp(artistName, t->artist) < 0)
				t = SingleRotateWithLeft(t);
		//LR case
			else
				t = DoubleRotateWithLeft(t);
	}
	// if x > t->element->artist insert x in the right subtree
	else if (strcmp(artistName, t->artist) > 0) {
		t->right = insert(t->right, artistName, song);
		if (height(t->left) - height(t->right) == -2)
			//RR case
			if (strcmp(artistName, t->artist) > 0)
				t = SingleRotateWithRight(t);
		//RL case
			else
				t = DoubleRotateWithRight(t);
	}

	// increase height
	t->height = maximum(height(t->left), height(t->right)) + 1;

	return t;
}



int height(Tree t) {
	// return height
	if (t == NULL)
		return -1;
	return t->height;
}

int maximum(int a, int b) {
	return (a > b) ? a : b;
}

Tree newNode() {
	// creating a node
	Tree node = (Tree)malloc(sizeof(struct AvlTreeNode));
	if (node == NULL) {
		printf("Out of space!!\n");
		exit(1);
	}

	node->left = NULL;
	node->right = NULL;
	node->height = 0;
	node->songs = createList();

	return node;
}

Tree SingleRotateWithLeft(Tree k2) {
	//left rotate to make k1 the parent

	//from the ppt slides
	Tree k1 = k2->left;
	Tree B = k1->right;

	// perform rotation
	k2->left = B;
	k1->right = k2;

	// update the height
	k2->height = maximum(height(k2->left), height(k2->right)) + 1;
	k1->height = maximum(height(k1->left), k2->height) + 1;

	//return the new root
	return k1;

}

Tree SingleRotateWithRight(Tree k1) {
	//right rotate to make k2 the parent

	Tree k2 = k1->right;
	Tree B = k2->left;

	//perform rotation
	k1->right = B;
	k2->left = k1;

	// update the heights
	k1->height = maximum(height(k1->left), height(k1->right)) + 1;
	k2->height = maximum(height(k2->right), k1->height) + 1;

	//return the new root
	return k2;
}

Tree DoubleRotateWithLeft(Tree k3) {
	//LR
	// first left heavily rotation
	// second right heavily rotation
	k3->left = SingleRotateWithRight(k3->left);
	return SingleRotateWithLeft(k3);
}

Tree DoubleRotateWithRight(Tree k3) {
	// RL
	// first right heavily rotation
	// second left heavily rotation
	k3->right = SingleRotateWithLeft(k3->right);
	return SingleRotateWithRight(k3);
}


int checkArtistName(Tree t, char artistName[],lNode song) {

	// if tree is empty return 0
	if (t == NULL)
		return 0;

	// if the artist already in the tree insert the song to his list
	else if (strcmp(artistName, t->artist) == 0) {
		insertInList(t->songs, song);
		return 1;
	}

	// check left and right
	else if (strcmp(artistName, t->artist) < 0)
		return checkArtistName(t->left, artistName, song);

	else if (strcmp(artistName, t->artist) > 0)
		return checkArtistName(t->right, artistName, song);
}

void PrintTreeNode(Tree t) {
	if (t == NULL) {
		printf("No node to print");
		return;
	}
	int loops = t->songs->size;
	int i = 0;
	lNode current = t->songs->head->next;
	for (i = 0; i < loops; i++) {
		printf("%s;%s;%s;%s;%d;%s\n\n", current->name, current->genre, current->album, t->artist, current->year, current->date);
		current=current->next;
	}



}
