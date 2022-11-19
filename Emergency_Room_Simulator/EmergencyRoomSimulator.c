#define _CRT_SECURE_NO_WARNINGS

//Name: Talal Shafei
//Student ID: 2542371

#include "queue.h"
#include"Linked.h"
#include<math.h>
#include<string.h>
#include<time.h>


//required Functions
void parseInput(char**, int*, int*, int*, int*);
List createPatientList(int, int, int);
int* initialiseSimulator(int,Queue*);
void newPatient(Queue, List,int);
void servePatient(int,Queue,int*,int*,int,List);
void reportStatistics(List, int, int,int);

//Not required Functions
int checkDoctors(int*,int);
int checkFinishServing(int*, int*, int,int);
int* createServingTimeArr(int);

int main(int argc, char* argv[]) {

	// to get random numbers each time
	srand(time(NULL));

	// variables to hold the numbers passed with the command line
	int noPatients = 0, noDoctors = 0, maxArr = 0, maxServ = 0;

	//put the numbers in the variables
	parseInput(argv, &noPatients, &noDoctors, &maxArr, &maxServ);

	// create patient list for all the patients
	List patientList = createPatientList(noPatients, maxArr, maxServ);


	// insitialising the emergency room simulator by creatin patient queue and an array of integers for the doctors
	int* doctorArr;
	Queue patientQueue;

	doctorArr=initialiseSimulator(noDoctors,&patientQueue);

	// avalDoc - 1 will tell us the index of the the available doctor (I made it bigger by 1 than the index so I use it in conditions so if it was 0 the condition is false)
	// check doctors will traverse the array to check if there is available doctor and will return it's index+1
	int avalDoc = checkDoctors(doctorArr,noDoctors);
	int time = 0; // time counter

	// create an array of the time the doctor with the same index will finish
	int* servingTime = createServingTimeArr(noDoctors);
	int stillServing = 0;// flag to not terminate the loop when there are patients are still being served
	
	// node to hold the first patient arrive
	patInfo patient;


	//Temporary List to hold the patients after serving
	List tmpList = createList();

	// if the list is not empty or there is patients in the queue or there is patients stll serving enter the loop
	while (!IsEmptyList(patientList)||!IsEmptyQueue(patientQueue) || stillServing) {

		// make patient point to the first patien arrive
		if (!IsEmptyList(patientList)) {
			patient = patientList->head->next;

			if(patient->arrival<=time)

			// when patient arrive let him enter the queue
			if (time >= patient->arrival) {
				newPatient(patientQueue, patientList, time);
			}
		}

		// if there were available doctors treat the first one in the priorty queue
		while(avalDoc && !IsEmptyQueue(patientQueue)) {
			servePatient(avalDoc, patientQueue,doctorArr,servingTime,time,tmpList);
			// now check if there is other available doctors to traet patients in the queue
			avalDoc = checkDoctors(doctorArr, noDoctors);
		}

		// check if the doctor has finished
		stillServing = checkFinishServing(doctorArr, servingTime, time,noDoctors);

		// if there doctors who finished update the available doctor flag 
		avalDoc = checkDoctors(doctorArr, noDoctors);

		//increase time
		time++;


	}
	// copy the tmp list to the oroginal patient list
	int i = 0;
	for (i = 0; i < noPatients; i++) {
		insertOnArrival(patientList, removePat(tmpList));
	}

	// use the oroginal patient list for the report
	reportStatistics(patientList, noPatients, noDoctors,time);

	free(doctorArr);

	return 0;
}

void parseInput(char** Command, int* np, int* nd, int* ma, int* ms) {
	// take the number in the command and put it in the variables and check if it is vallid
	*np = atoi(Command[1]);
	if (*np <= 0) {
		printf("\n**Not a valid number for Patients**\n");
		exit(1);
	}
	*nd = atoi(Command[2]);
	if (*nd <= 0) {
		printf("\n**Not a valid number for Doctors**\n");
		exit(1);
	}
	*ma = atoi(Command[3]);
	if (*ma <= 0) {
		printf("\n**Not a valid time for Max Arrival time**\n");
		exit(1);
	}
	*ms = atoi(Command[4]);
	if (*ms <= 0) {
		printf("\n**Not a valid time for Max Service time**\n");
		exit(1);
	}
}

List createPatientList(int noPatients, int maxArr, int maxServ) {
	// create patient linked list
	//maxArr refers to maximum arrival time, maxServ refers to maximum serving time

	List L = createList();

	int i = 0;
	for (i = 0; i < noPatients; i++) {
		patInfo newPat = (patInfo)malloc(sizeof(struct Node));
		if (newPat == NULL) {
			printf("\n***Couldn't Allocate memory***\n");
			exit(1);
		}

		newPat->type = (int)((rand() % 4) + 1);
		newPat->arrival = (int)((rand() % maxArr) + 1);
		newPat->serv = (int)((rand() % maxServ) + 1);// in the pdf there is a patient in the list who has 0 service time but that doesn't make sence
		newPat->start = 0;
		newPat->doctor = 0;
		newPat->insuarance = (int)(rand() % 2);
		newPat->next = NULL;

		// insert the random patient to the list
		insertOnArrival(L, newPat);
	}
	return L;
}

int* initialiseSimulator(int noDoctors,Queue* Q) {
	// pass the address of a queue to create the queue inside the function
	// return the address of the doctor array after initializing every element to 1 (available)
	*Q = createQueue();

	int* doctorAval = (int*)malloc(noDoctors * sizeof(int));
	if (doctorAval == NULL) {
		printf("\n***Couldn't Allocate memory***\n");
		exit(1);
	}
	int i = 0;
	for (i = 0; i < noDoctors; i++)
		doctorAval[i] = 1;
	

	return doctorAval;



}

void newPatient(Queue q, List L,int time) {
	// when adding a new patient to the queue make sure to add the other patients after him if they ahve the same arrival time
	// here when we remove from L the head will change that's why it's a finite loop
	while (!IsEmptyList(L) && time >= L->head->next->arrival)
		priorityEnqueue(q, removePat(L));
}

void servePatient(int doc,Queue q,int* docArr,int* servingTime,int time,List L) {
	
	// remove the patient from the queue
	patInfo patient= Dequeue(q);
	//set the starting time to time
	patient->start = time;
	// make the doctor busy
	docArr[doc - 1] = 0;
	// set the patient doctor as the doctor ID
	patient->doctor = doc;

	// set the serving time fo that doctor
	servingTime[doc - 1] = patient->arrival + patient->serv;

	
	//insert the patient back to the list to use it later for the report
	insertOnArrival(L, patient);

}

int checkDoctors(int* docArr,int nd) {
	// check if there is an available doctor in the array and return it's index+1
	int i = 0;
	for (i = 0; i < nd; i++)
		if (docArr[i] == 1)return i + 1;
	return 0;
}

int checkFinishServing(int* docArr, int* servingTime, int time,int noDoc) {
	// check if the doctor with the same index as in the serving array has finished
	// if so make him available again and set his serving time to -1 so we know he is not serving anyone
	int i = 0;
	for (i = 0; i < noDoc; i++) {
		if (servingTime[i] <= time) {
			docArr[i] = 1;
			servingTime[i] = -1;
		}
	}

	// if there is an element in the serving time array that it is not -1 return 1 to the flag 
	for (i = 0; i < noDoc; i++) {
		if (servingTime[i] != -1)return 1;
	}
	return 0;
}

int* createServingTimeArr(int noDoc) {
	// create an integer array and initialise it's elements to -1
	int* servingTime = (int*)malloc(noDoc * sizeof(int));
	if (servingTime == NULL) {
		printf("\n***Couldn't Allocate memory***\n");
		exit(1);
	}
	int i = 0;
	for (i = 0; i < noDoc; i++) {
		servingTime[i] = -1;
	}
	return servingTime;
}


void reportStatistics(List patientList, int noPat, int noDoc,int time) {
	// counting the numbers of the type and insurance to use in the report
	int red = 0, orange = 0, yellow = 0, green = 0, insuranceCounter = 0;
	patInfo tmp = patientList->head->next;
	while (tmp != NULL) {
		if (tmp->type == 4)red++;
		else if (tmp->type == 3)orange++;
		else if (tmp->type == 2)yellow++;
		else if (tmp->type == 1)green++;
		if (tmp->insuarance == 1)insuranceCounter++;
		tmp = tmp->next;
	}
	printf("****************Report*****************\n");
	printf("The number of doctors: %d\nThe number of patients : %d\n", noDoc, noPat);
	printf("Number of patients for each patient type:\nred %d\norange: %d\nyellow: %d\ngreen: %d\n", red, orange, yellow, green);

	// array to hold the number of patients served by each doctor
	int* doctorCounter = (int*)calloc(noDoc, sizeof(int));
	int i = 0;
	for (i = 0; i < noDoc; i++) {
		tmp = patientList->head->next;
		while (tmp != NULL) {
			if (tmp->doctor == i + 1)doctorCounter[i]++;
			tmp = tmp->next;
		}
	}
	printf("Number of patients for each Doctor:\n");
	for (i = 0; i < noDoc; i++) {
		printf("Doctor %d:%d\n", i + 1, doctorCounter[i]);
	}

	printf("Completion time: %d\n", time);

	// variable to hold the total waiting time and the max waiting time in the queue
	int totalWaitingTime = 0,maxWaitingTime=0;
	tmp = patientList->head->next;
	while (tmp != NULL) {
		int waiting = tmp->start - tmp->arrival;
		totalWaitingTime += waiting;
		if (waiting > maxWaitingTime)maxWaitingTime = waiting;

		tmp = tmp->next;
	}
	printf("Average time spent in the queue: %.2f\n", ((float)totalWaitingTime) / noPat);
	printf("Maximum waiting time: %d\n", maxWaitingTime);

	printf("Number of patients with insurance: %d\n", insuranceCounter);
	printf("Number of patients without insurance: %d\n", noPat - insuranceCounter);
}
