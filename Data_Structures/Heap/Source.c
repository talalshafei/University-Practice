#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct EmailDetails {
	int id;
	char sender[51];
	char recipient[51];
	int date;
	int words_num;
};

typedef struct EmailDetails Email;

// I am using a Heap that neglect the 0 index

//required

/*	readEmails:
	this will be O(n) because we reading file by file sequentially assuming number of files is n
	in other words we have only one loop in the function that iterate n times and with neglecting the constants
	the result will be O(n)
*/
Email* readEmails(int*);

/*	menu:
	will be O(1) since it is just 5 print statements then it will be summation of five constants
	neglecting the constants to write it in big-O we will have O(1)

*/
void menu();

/*	heapSort:
	will be O(nlog(n)) since we start by building the heap which is O(n) then we have a
	for loop that will run the size of the array/2 -> n/2 -> O(n) and we know that the
	hepify function is O(logn) from master theorem
	thus we have O(n) + O(n)*O(log(n)) and that will equal to O(nlog(n)) for the heapsort
*/
void heapSort(Email*, int, int);


/*	binSearch:
	since we are dividing the size of the array by two each time ( when we neglect half of the array
	by setting small = mid +1 or large = mid -1) then we can say the the worst case scenario is to keep
	dividing until there is 1 element left (small == large) which will be dividing log(n) times
	thus we have O(log(n)) or we can write an equation and solve it
	T(n) = T(n/2) + c that will result in O(log(n)) by master theorem

*/
Email* binSearch(Email*, int, int, int);

/*	printEmails:
	will be O(n) because we printing sequentially assuming number of emails are n we will have O(n)
	in other words one for loop that will iterate n times
*/
void printEmails(Email*, int);

// helper
void buildHeap(Email*, int, int);
int parent(int);
int left(int);
int right(int);
void swap(Email*, int, int);
void assign_email(Email*, Email*);


int main() {

	// reading the text files
	int length = 0;
	Email* emails = readEmails(&length);
	if (emails != NULL)
		printf("\n%d emails have been read successfully! \n\n", length);


	// menu loop
	int command = 0;
	while (command != 5) {

		//display the menu and taking the command
		menu();
		printf("\nCommand: ");
		scanf("%d", &command);

		// displaying commands
		if (command == 1 || command == 2 || command == 3) {
			heapSort(emails, length, command);
			printEmails(emails, length);
		}

		//searching command
		else if (command == 4) {
			//taking the input from the user
			printf("Enter the search key: ");
			int key;
			scanf("%d", &key);

			//sort based on Id before searching
			heapSort(emails, length, 1);

			//applying the binary search algorithm
			Email* e = binSearch(emails, key, 1, length);

			// the key doesn't exist in the Ids
			if (e == NULL)
				printf("\nThere is no email with the follwing Id: %d\n", key);
			// the key exist
			else {
				// I made e to be an array with one element ( actually two but the element with index 0 is ignored)
				printEmails(e, 1);
				// free the memmory
				free(e);
			}
		}

		// if not one of the above and not 5 that means the command is not valid
		else if (command != 5)
			printf("\n** Please Enter a Valid Command ** \n");


	}


	// we exited the loop so we free the memory and finish
	printf("Goodbye !\n");
	free(emails);


	return 0;
}

void menu() {
	// printing with the same format as in the pdf
	printf("Please choose one of the following options:\n");
	printf("	(1) Display emails sorted by id\n");
	printf("	(2) Display emails sorted by number of words\n");
	printf("	(3) Display emails sorted by date\n");
	printf("	(4) Search email by ID\n");
	printf("	(5) Exit\n");
}

Email* readEmails(int* num_files) {

	// enter the loop no matter what "while(1)"
	// to use it later to take input again if the directory or txt file doesn't exist
	while (1) {

		fflush(stdin);

		//set flag to error to 0
		int error = 0;

		// take the directory path assuming the longest path is 149 characters
		printf("Enter the path of the directory which contains data files: ");
		char path[150];
		scanf("%s", path);

		// take the number of files in dir
		printf("Enter the number of data files: ");
		scanf("%d", num_files);

		// allocating an array for the emails with size equal to number of files +1 ( because we ignore 0)
		Email* emails = (Email*)malloc(sizeof(Email) * (*num_files + 1));
		// there is no memory available
		if (emails == NULL) {
			printf("\n** Not Enough Memory Available **\n");
			exit(-1);
		}


		// if the file opened by unix or mac keeps the flag to u but if windows set it to w
		// this will help when we append '\' for the path if it was on windows or '/' if it was on unix or mac
		char operating_system = 'u';
#ifdef _WIN32
		operating_system = 'w';
#endif


		int i;
		for (i = 1; i <= *num_files; i++) {

			// opening the file

			//first we form the file path
			char file_path[170];
			char num[15];
			strcpy(file_path, path);

			if (operating_system == 'w') {
				strcat(file_path, "\\");
			}
			else {
				strcat(file_path, "/");
			}
			sprintf(num, "%d", i);
			strcat(file_path, num);
			strcat(file_path, ".txt");

			// then we open the file
			FILE* f;
			f = fopen(file_path, "r");
			// if there is a problem we set the error to one
			if (f == NULL) {
				if (i == 1)
					printf("\nDirectory doesn't exist please choose another one or it is the wrong one\n\n");
				else
					printf("\n** File number is out of range please choose the correct number of files or check the directory  **\n\n");
				error = 1;
				break;

			}


			// Process the file and filling the emails array

			fscanf(f, "%d", &(emails[i].id));
			char ignore[10];
			fscanf(f, "%s%s", ignore, emails[i].sender);
			fscanf(f, "%s%s", ignore, emails[i].recipient);
			fscanf(f, "%s%d\n", ignore, &(emails[i].date));

			char line[201];
			fgets(line, 200, f);

			int j, count = 1;

			for (j = 0; line[j]; j++) {
				if (line[j] == ' ')
					count++;
			}
			emails[i].words_num = count;


			//closing the file
			fclose(f);
		}

		// if there is no error stop and return the emails
		if (error == 0)
			return emails;
		// if error is not 0 then we have an error and since our loop is while(1) it will run again
	}

}

void printEmails(Email* emails, int length) {

	//printing the emails from 1 index to length
	int i;
	for (i = 1; i <= length; i++) {
		printf("Id: %d\n", emails[i].id);
		printf("Sender: %s\n", emails[i].sender);
		printf("Recipient: %s\n", emails[i].recipient);
		printf("Date: %d\n", emails[i].date);
		printf("Words: %d\n\n", emails[i].words_num);

	}
}

int parent(int i) {
	return i / 2;
}

int left(int i) {
	return 2 * i;
}

int right(int i) {
	return 2 * i + 1;
}

void assign_email(Email* e1, Email* e2) {
	// is it like e1 = e2

	e1->id = e2->id;
	e1->date = e2->date;
	e1->words_num = e2->words_num;

	strcpy(e1->sender, e2->sender);
	strcpy(e1->recipient, e2->recipient);

}

void swap(Email* emails, int a, int b) {
	// swaping between two emails positions
	Email temp;
	assign_email(&temp, &emails[a]);
	assign_email(&emails[a], &emails[b]);
	assign_email(&emails[b], &temp);


}

void heapify_id(Email* emails, int i, int length) {

	// this will heapify the subtree by id

	int l = left(i);
	int r = right(i);
	int largest = 0;

	if (l <= length && (emails[l].id > emails[i].id))
		largest = l;
	else
		largest = i;

	if (r <= length && (emails[r].id > emails[largest].id))
		largest = r;

	if (largest != i) {
		swap(emails, i, largest);
		heapify_id(emails, largest, length);
	}
}

void heapify_words(Email* emails, int i, int length) {

	// this will heapify the subtree by number of words

	int l = left(i);
	int r = right(i);
	int largest = 0;

	if (l <= length && (emails[l].words_num > emails[i].words_num))
		largest = l;
	else
		largest = i;

	if (r <= length && (emails[r].words_num > emails[largest].words_num))
		largest = r;

	if (largest != i) {
		swap(emails, i, largest);
		heapify_words(emails, largest, length);
	}
}

void heapify_date(Email* emails, int i, int length) {

	// this will heapify the subtree by date

	int l = left(i);
	int r = right(i);
	int largest = 0;

	if (l <= length && (emails[l].date > emails[i].date))
		largest = l;
	else
		largest = i;

	if (r <= length && (emails[r].date > emails[largest].date))
		largest = r;

	if (largest != i) {
		swap(emails, i, largest);
		heapify_date(emails, largest, length);
	}
}

void buildHeap(Email* emails, int length, int criteria) {

	// building a Max heap

	int i;

	// Max heap on Id
	if (criteria == 1)
		for (i = length / 2; i >= 1; i--)
			heapify_id(emails, i, length);



	// on number of words
	else if (criteria == 2)
		for (i = length / 2; i >= 1; i--)
			heapify_words(emails, i, length);

	// on date
	else
		for (i = length / 2; i >= 1; i--)
			heapify_date(emails, i, length);



}

void heapSort(Email* emails, int length, int criteria) {

	// sorting the array using heap sort algorithm

	// first we build the max heap
	buildHeap(emails, length, criteria);
	int i;

	// sort based on Id
	if (criteria == 1)
		for (i = length; i >= 2; i--) {
			swap(emails, 1, i);
			heapify_id(emails, 1, i - 1);
		}

	// sort based on words number
	else if (criteria == 2)
		for (i = length; i >= 2; i--) {
			swap(emails, 1, i);
			heapify_words(emails, 1, i - 1);
		}

	// sort based on date
	else
		for (i = length; i >= 2; i--) {
			swap(emails, 1, i);
			heapify_date(emails, 1, i - 1);
		}


}

Email* binSearch(Email* emails, int key, int small, int large) {

	// first we create an array with size 1 (0 will be ignored)
	// I used it like this just to use the printEmails on 1 element since it's for loop starts from 1
	// it will print e[1]

	Email* e = (Email*)malloc(sizeof(Email) * 2);

	//that means our array still contain elements
	while (small <= large) {

		// set the mid to the half of the array
		int mid = (small + large) / 2;

		// divide the array by 2
		if (emails[mid].id < key)
			small = mid + 1;
		else if (emails[mid].id > key)
			large = mid - 1;

		// the mid element id equals to the key then we set e[1] = emails[mid] and return e to print it
		else {
			assign_email(&e[1], &emails[mid]);
			return e;
		}
	}

	// if key doesn't exist in emails Id
	free(e);
	return NULL;
}
