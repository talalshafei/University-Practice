//Name: Talal Shafei
//ID:2542371

#include"avltree.h"
#include"avltree.c"



//Required functions
Tree readData(char*);
Tree insertSong(Tree, char[],lNode);
void displaySongs(Tree);
void popularArtist(Tree);
void oldestSong(Tree);

//Helper functions
void findPopluarArtist(Tree, Tree*, int*);
void findOldestsong(Tree, Tree*, int*, int*);

int main(int argc,char**argv) {

	// reading data from the file
	Tree playlist = readData(argv[1]);

	printf("Welcome to Song Indexing\n");
	int option;//control variable
	do {
		printf("-----------------------------------------------------------------------------\n");
		printf("1. Display the full index of songs\n2. Display the songs of popular artist\n3. Display the oldest song\n4. Exit\n");
		printf("\nOption: ");
		scanf("%d", &option);
		printf("\n");
		switch (option) {
		case 1:
			// displaying the songs in alphapatical order
			displaySongs(playlist);
			break;
		case 2:
			// printing the songs of the artist with the highest total of songs
			popularArtist(playlist);
			break;
		case 3:
			// printing the song with the oldest year
			oldestSong(playlist);
			break;
		case 4:
			// terminating the loop
			printf("\nGood Bye!\n");
			break;
		default :
			// printing error message if the user chooses invalid option
			printf("Invalid Option !!\n");

		}
	} while (option != 4);





	return 0;
}

Tree readData(char* command) {
	char input[30];
	//char input*;
	// copying the file name to input so if the file name is wrong we let the user enter it again
	strcpy(input, command);

	// opening the file
	FILE* fp = fopen(input, "r");
	while (fp == NULL) {
		printf("%s not found please enter the file name again: ",input);
		scanf("%s", input);
		fp = fopen(input, "r");

	}

	// check if the file is empty
	char c = '\0';
	//to detect if the file is empty
	if ((c = fgetc(fp)) == EOF) {
		printf("%s is empty!!\n", input);
		exit(1);
	}

	// count the lines
	int lines = 1;// shows the number of songs
	while ((c = fgetc(fp)) != EOF) {
		if (c == '\n')lines++;
	}

	// reset the file pointer to the begining of the file
	rewind(fp);

	int i = 0;
	Tree t = CreateAvlTree();


	for (i = 0; i < lines; i++) {

		char artistName[31];
		//char* artistName;
		lNode song = createListNode();


		/* copying the file data into the node and the aritist name
		into the string artistName to use them later
		to insert in th AVL Tree*/
		char L[101];
		fgets(L, 100, fp);
		char* token = strtok(L, ";");
		int order = 1;
		while (token != NULL) {
			switch (order) {
			case 1:
				strcpy(song->name, token);
				break;
			case 2:
				strcpy(song->genre, token);
				break;
			case 3:
				strcpy(song->album, token);
				break;
			case 4:
				strcpy(artistName, token);
				break;
			case 5:
				song->year = atoi(token);
				break;
			case 6:
				//to avoid copying the '\n' in the end off the date
				strcpy(song->date, token);
				if (i < lines - 1)
					song->date[strlen(song->date) - 1] = '\0';

				break;

			}
			order++;
			token = strtok(NULL, ";");
		}

		t = insertSong(t, artistName,song);

	}

	fclose(fp);

	return t;
}

Tree insertSong(Tree t, char artistName[], lNode song) {

	// flag to check if the artist already in the list
	int exist = 0;

	/* checkArtistName function will check if the
	artist in the AVL Tree if he already
	there the function will insert the
	song to th linked list in that
	tree node so returns 0
	if the artist is not in the tree
	or 1 if the artist is in the tree to the flag */

	exist = checkArtistName(t, artistName,song);

	// if the flag is 0 then artist is not in the AVL Tree
	if(exist==0)
		t = insert(t, artistName, song);

	return t;
}


void displaySongs(Tree t) {
	// in-order traversal - alphabetically -
	if (t == NULL)return;

	displaySongs(t->left);
	PrintTreeNode(t);
	displaySongs(t->right);
}

void popularArtist(Tree t) {
	/*
	* Complexity:
	 1) Time complexity:
		findPopluarArtist will take the tree and check every
		node to find the popular so we will have n comparisons therefore the algorithm is O(n)
	2) way to improve it is creating a TreeRecord structure to keep track of the node with the maximum largest linked list
		and that can be done using two members one to hold the value of the size of the largest list (Max)
		and a member to point at the node (popular) and we keep checking them every time we insert a node
		so when we call the function it will print only the node which popular point at and this will take O(1)
	*/



	// double pointer variable to point at the popular node
	Tree *popular=(Tree*)malloc(sizeof(Tree));
	if (popular == NULL) {
		printf("Out of space!!\n\n");
		exit(-1);
	}
	*popular = NULL;

	// pointer to int to point at the integer value of the largest size of a lined list inside a Tree Node
	int* Max=(int*)malloc(sizeof(int));
	if (Max == NULL) {
		printf("Out of space!!\n\n");
		exit(-1);
	}
	*Max = 0;

	// will make popular point at the tree node with the largest size linked list
	findPopluarArtist(t,popular,Max);

	// printing the songs of the popular artist
	PrintTreeNode(*popular);

	// free the allocating memory
	free(Max);
	free (popular);


}


void findPopluarArtist(Tree t,Tree* popular,int*Max ) {

	// if t point to null stop
	if (t == NULL)return;

	// if the list size bigger than Max
	// let Max equal the size of the list and popular poin to the node
	if (t->songs->size > *Max) {
		*Max = t->songs->size;
		*popular= t;

	}

	// check left and right
	findPopluarArtist(t->left, popular, Max);
	findPopluarArtist(t->right, popular, Max);

}

void oldestSong(Tree t) {
/*
	* Complexity:
	 1) Time complexity:
		findOldestsong will take the tree and check every
		node to find the popular so we will have n comparisons and for every
		node it will check its linked list so if the largest size
		of a linked list is m we will have m comparison for every node
		so the total of comparisons will be n multiplied by m
		then the algorithm time complexity is O(n.m)

	2) way to improve it is creating a TreeRecord structure to keep track
	    of the node with the minimum year in one of its linked list
		and that can be done using three members one to hold the
		value of the minimum year in the linked list (Min)
		and a member to point at the node (oldest) and a member
		to hold the index (minIndex) andwe keep checking
		them every time we insert a nodeso when we call the
		function it will print only the node which oldest
		point at and this will take O(m) where m the number
		of nodes in the linked list because in the worst case
		minIndex will be equal to the size of the linked list minus 1
	*/

	// creating double pointer to point at the node with the oldest song (minimum year)
	Tree* oldest = (Tree*)malloc(sizeof(Tree));
	if (oldest == NULL) {
		printf("Out of space!!\n\n");
		exit(-1);
	}
	// to hold the minimum year
	int* Min = (int*)malloc(sizeof(int));
	if (Min == NULL) {
		printf("Out of space!!\n\n");
		exit(-1);
	}

	// to hold the index of the song with the minimum year in the list
	int* minIndex=(int*)malloc(sizeof(int));
	if (minIndex == NULL) {
		printf("Out of space!!\n\n");
		exit(-1);
	}

	*oldest = NULL;
	*Min = t->songs->head->next->year;
	*minIndex = 0;

	// will make oldest point to the node with the minimum song and minIndex will hold the index of the oldest song in the list
	findOldestsong(t, oldest, Min, minIndex);

	// will point at the node with the oldest song after the loop
	lNode oldSong =(*oldest)->songs->head;

	int i = 0;

	for (i = 0; i <= *minIndex; i++)
		oldSong = oldSong->next;

	// print the information of the oldest song
	printf("%s;%s;%s;%s;%d;%s\n\n", oldSong->name, oldSong->genre, oldSong->album, (*oldest)->artist, oldSong->year, oldSong->date);

	// free the allocated memory
	free(oldest);
	free(Min);
	free(minIndex);


}

void findOldestsong(Tree t, Tree* oldest, int* Min, int* minIndex) {

	// if t point to null stop
	if (t == NULL)return;

	//tmp to poin at the first element in the list
	lNode tmp = t->songs->head->next;
	int i = 0;
	for (i=0;i<t->songs->size;i++) {
		// if tmp year less than Min
		// make Min tmp year
		// minIndex i
		//oldest point to t
		if (tmp->year < *Min) {
			*Min = tmp->year;
			*minIndex = i;
			*oldest = t;
		}
		// move the tmp forward
		tmp = tmp->next;
	}

	// check left and right
	findOldestsong(t->left, oldest, Min, minIndex);
	findOldestsong(t->right, oldest, Min, minIndex);

}
