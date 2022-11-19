#define _CRT_SECURE_NO_WARNINGS




#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Assumed Email_ID > 0, to check if a cell is empty in the table
*/
struct EmailDetails {
	int Email_ID;
	char Sender[51];
	char Recipient[51];
	int Day_of_the_month;
	int No_of_words;
};
typedef struct EmailDetails Email;


/*
no_elements: number of elements
capacity: Table size
addressing: addressing type
emails: dynamic array that will hold the emails
*/
struct HashTableRecord {
	int no_elements;
	int capacity;
	int addressing;

	Email* emails;
};

typedef struct HashTableRecord HashTable;




// required functions
HashTable* readEmails(HashTable*);
void printTable(HashTable*);
void searchEmail(HashTable*);

//helper functions

/* initialize the Table at the begining */
HashTable* createTable();

/* will be used to map the values to the table */
int calKey(int id, char sender0) { return id + (int)sender0 - 65; }
int hash(int key, int table_size) { return key % table_size; }
int hash2(int key) { return 5 - (key % 5); }

/* will be used to check if lambda > 0.5 */
float loadFactor(int elements, int table_size) { return (float)elements / table_size; }

/* to choose the addressing type of the table */
int chooseAddressing();

/* 
insertion functions 
insert will choose call the linear probing or double hashing based on the addressing type 
*/
HashTable* insert(HashTable* , Email* );
HashTable* linearProbingInsertion(HashTable* , Email* );
HashTable* doubleHashingInsertion(HashTable* , Email* );

/* rehash will create a new Table with new size to make lmabda < 0.5 */
HashTable* rehash(HashTable*);

/* 
nextPrime: finds the smaller prime number bigger than the given number
is Prime: checks if the number is prime
*/
int nextPrime(int);
int isPrime(int);


/* extra helper functions
assignEmails: like overloading the operator = it will will work like e2 = e1
memoryError: will exist if program failed to allocate memory
countWords: count words in a string
menu: print the menu
*/
void assignEmail(Email*, Email*);
void memoryError();
int countWords(char[]);
void menu();

int main() {

	// initialize size 11 hash table
	HashTable *Table = createTable();

	// choosing the open addressing
	Table-> addressing = chooseAddressing();

	
	int command = 0;
	while (command != 4) {
		// display the menu and choose the command
		printf("\n");
		menu();
		printf("\nEnter Command: ");
		scanf("%d", &command);
		printf("\n");

	
		switch (command) {
		case 1:
			// read emails from files and insert them into the table
			Table = readEmails(Table);
			break;
		case 2:
			// search for an email based on ID and sender's name
			searchEmail(Table);
			break;
		case 3:
			// printing the table
			printTable(Table);
			break;
		case 4:
			// exiting the program
			printf("Good Bye!\n");
			break;
		default:
			// wrong command error message
			printf("**Please choose a valid command**\n");
		}
	}

	// freeing the allocated memory
	free(Table->emails);
	free(Table);

	return 0;
}

void menu() {
	printf("Please choose one of the following options:\n");
	printf("	(1) Read Emails\n");
	printf("	(2) Search an Email\n");
	printf("	(3) Print Table\n");
	printf("	(4) Exit\n");
}

int chooseAddressing() {
	printf("Choose Addressing type: \n");
	printf("(1) Double Hashing\n");
	printf("(2) Linear Probing\n");

	printf("Enter: ");
	int a=0;
	scanf("%d", &a);

	// if the user chose something other than 1 or 2 let him choose again
	while (a != 1 && a != 2) {
		printf("***Please choose a valid addressing (1 or 2)***\n");
		printf("Enter: ");
		scanf("%d", &a);

	}
	
	return a;
}

HashTable* createTable() {
	// creating a table with size 11

	HashTable *Table= (HashTable*)malloc(sizeof(HashTable));
	if (Table == NULL)memoryError();

	Table->no_elements = 0;
	Table->capacity = 11;
	Table->emails = (Email*)calloc(11, sizeof(Email));
	if (Table->emails == NULL) memoryError();

	return Table;
}

HashTable* readEmails(HashTable* Table) {

	// we put error 1 just to enter the loop
	// the loop will break if there is no error in data path and files number and path
	int error = 1;
	while (error) {

		// assume no error
		error = 0;

		
		int num_files;
		// take the directory path assuming the longest path is 149 characters
		printf("Enter the path of the directory which contains data files: ");
		char path[150];
		scanf("%s", path);

		// take the number of files in dir
		printf("Enter the number of data files: ");
    	scanf("%d", &num_files);

		char operating_system = 'u';
#ifdef _WIN32
		operating_system = 'w';
#endif


		int i;

		for (i = 1; i <= num_files; i++) {

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
					printf("\nDirectory doesn't exist or it is the wrong one please choose another one \n\n");
				else
					printf("\n** File number is out of range please choose the correct number of files or check the directory  **\n\n");

				error = 1;
				// we break from the for loop
				break;
			}

			


			Email email;

			// Process the file and filling the email

			fscanf(f, "%d", &(email.Email_ID));
			char ignore[10];
			fscanf(f, "%s%s", ignore, email.Sender);
			fscanf(f, "%s%s", ignore, email.Recipient);
			fscanf(f, "%s%d\n", ignore, &(email.Day_of_the_month));

			char line[201];
			fgets(line, 200, f);

			
			email.No_of_words = countWords(line);

			// closing the file
			fclose(f);

			/* put pre_insert to number of elements before the insertion 
				if the number of elements increased after the insertion that means it was a sucessful inserion
				we compare and print the Table after a succeful inserions only!
			*/
			int pre_insert = Table->no_elements;
			// inserting email in the Table
			Table = insert(Table, &email);
			if (pre_insert < Table->no_elements) {
				printf("\n");
				printTable(Table);
				printf("\n");
			}
			
		}



	}

	/* because we might rehash that means we might destroy the old table and create a new one with bigger
		size, so we need to return the adress of the new Table at the end
		the same for the inserion functions
	*/
	return Table;
}


int countWords(char line[]) {
	// count the words in a string by counting the spaces +1
	int j, count = 1;
	for (j = 0; line[j]; j++) {
		if (line[j] == ' ')
			count++;
	}
	return count;
}


HashTable* insert(HashTable* Table, Email* email) {
	// we return adress incase we destroyed the old Table and created new one while rehashing
	if (Table->addressing == 1)
		Table = doubleHashingInsertion(Table, email);
	else
		Table = linearProbingInsertion(Table, email);

	return Table;
}

HashTable* linearProbingInsertion(HashTable* Table, Email*email) {
	// calculate the key and hash(key)
	int key = calKey(email->Email_ID, email->Sender[0]);
	int index = hash(key, Table->capacity);

	int dont_skip = 1; // flag assumed the email is not in the table

	/* while we didn't land on an empty cell 
		check if the ID already exists in the Table
		if True then set the don't_skip to 0 so we won't add it to the table
		else increase the index by one (since we are using linear probing)
	*/
	while (Table->emails[index].Email_ID != 0) {
		if (Table->emails[index].Email_ID == email->Email_ID) {
			printf("\n**ID %d already exists in the table**\n", email->Email_ID);
			dont_skip = 0;
			break;
		}
		index ++;
	}
	
	// assigning the table cell to the email details
	if (dont_skip) {
		assignEmail(&(Table->emails[index]), email);
		Table->no_elements++;
	}
	
	// if lambda > 0.5 then we rehash
	if (loadFactor(Table->no_elements, Table->capacity) > 0.5)
		Table = rehash(Table);

	return Table;
}

HashTable* doubleHashingInsertion(HashTable* Table, Email* email) {
	/* same as the Linear probing function
		the only difference is the new variable i and the calculation of the index if a collision occurs
		we use the double hashing formula index = hash( key + i*hash2(key))
		
	*/
	
	int key = calKey(email->Email_ID, email->Sender[0]);
	int index = hash(key, Table->capacity);


	int i = 1;
	int dont_skip = 1;
	while (Table->emails[index].Email_ID != 0) {
		if (Table->emails[index].Email_ID == email->Email_ID) {
			printf("\n**ID %d already exists in the table**\n", email->Email_ID);
			dont_skip = 0;
			break;
		}
		int offset = hash2(key);
		index = hash(key + i*offset, Table->capacity);
		i++;
	}

	if (dont_skip) {
		assignEmail(&(Table->emails[index]), email);
		Table->no_elements++;
	}

	if (loadFactor(Table->no_elements, Table->capacity) > 0.5)
		Table = rehash(Table);

	return Table;
}

HashTable* rehash(HashTable* old_Table) {
	//create new Table
	HashTable* new_Table = (HashTable*)malloc(sizeof(HashTable));
	if (new_Table == NULL)memoryError();

	// make it use the same addressing
	new_Table->addressing = old_Table->addressing;

	/*set the number of elements to 0, the number will update after each insertion until it
	equals the number of elements of the old table */
	new_Table->no_elements = 0;

	// set the table size to the next prime number of the double of the old Table size
	new_Table->capacity = nextPrime(2*(old_Table->capacity));

	// allocate memory for the new table emails array
	new_Table->emails = (Email*)calloc(new_Table->capacity, sizeof(Email));
	if (new_Table->emails == NULL) memoryError();


	// insert each element from the old table to the new table
	int i;
	for (i = 0; i < old_Table->capacity; i++){
		if (old_Table->emails[i].Email_ID != 0)
			insert(new_Table, &(old_Table->emails[i]));
	}

	// free the memory of the old table
	free(old_Table->emails);
	free(old_Table);

	// return the adress of the new table
	return new_Table;

}

// get the prime after n
int nextPrime(int n) {
	int next = n + 1;
	while (!isPrime(next))
		next++;

	return next;
}

// simple algorithm to check if the number is prime
int isPrime(int number) {
	int i;
	for (i = 2; i < number; i++)
		if (number % i == 0) return 0;

	return 1;
}

void assignEmail(Email* e1, Email* e2) {
	// is it like e1 = e2

	e1->Email_ID = e2->Email_ID;
	e1->Day_of_the_month = e2->Day_of_the_month;
	e1->No_of_words = e2->No_of_words;

	strcpy(e1->Sender, e2->Sender);
	strcpy(e1->Recipient, e2->Recipient);

}

void printTable(HashTable* Table) {
	// print the table in nice format
	printf("Index\tID\tSender\tRecipient\tDate\tWords\n");
	int i;
	for (i = 0; i < Table->capacity; i++) {
		if (Table->emails[i].Email_ID != 0)
			printf("%-5d\t%-2d\t%-6s\t%-9s\t%-4d\t%d\n", i, Table->emails[i].Email_ID,Table->emails[i].Sender,Table->emails[i].Recipient,Table->emails[i].Day_of_the_month, Table->emails[i].No_of_words);

		else printf("%d\n", i);
	}
}

void memoryError() {
	// display error message if the program failed to allocate memory
	printf("**\n\n Allocation Failed Not Enough Memory**\n\n");
	exit(-1);
}

void searchEmail(HashTable* Table) {

	// take the id and the sender's name from the user
	int id;
	char sender[51];
	printf("Enter unique identifier: ");
	scanf("%d", &id);
	printf("Enter sender: ");
	scanf("%s", sender);

	// calculate the index and the hashing of the input
	int key = calKey(id, sender[0]);
	int index = hash(key, Table->capacity);

	// set found to 0 (not found)
	int found = 0;


	// if the table use Linear Probing addressing search at the index and keep increasing by one
	// until it found or an empty cell (that means not found)
	if (Table->addressing == 2) {
		while (Table->emails[index].Email_ID != 0) {
			if (Table->emails[index].Email_ID == id) {
				found = index;
				break;
			}
			index++;
		}
	}

	/* else the table use double hashingand will calculate
	the next index based on the formula index = hash(key + i*offset)
	if the user reaches an empty cell that meanse not found
	*/
	else {
		int i = 1;
		while (Table->emails[index].Email_ID != 0) {
			if (Table->emails[index].Email_ID == id) {
				found = index;
				break;
			}
			int offset = hash2(key);
			index = hash(key + i * offset, Table->capacity);
			i++;
		}
	}

	// if found = id that means the email was found
	if (found) {
		printf("Recipient: %s\n", Table->emails[found].Recipient);
		printf("Date: %d\n", Table->emails[found].Day_of_the_month);
		printf("Number of words: %d\n", Table->emails[found].No_of_words);
	}
	else
		printf("Student is not found!\n");

}