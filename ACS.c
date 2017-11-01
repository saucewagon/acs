/* https://stackoverflow.com/questions/4237084/reading-from-a-file-and-storing-in-array  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 20
#define NUM_CLERKS 2
#define NUM_QUEUES 2
#define TIME_MULTIPLIER 100000
#define MAX_CUSTS 20

int queueLength = 0;
enum { MAXLINES = 30 };
int numOfCustomers;
void getInput();
void initializeQueues();
int chooseMinLengthQueue();

pthread_cond_t convar;

pthread_cond_t clerkConvar;

pthread_cond_t clerk1Convar;

pthread_mutex_t lock;

pthread_mutex_t clerkLock;

int whichClerk = 0;

struct Customer{
		
	int id;
	int serviceTime;
	int arrivalTime;
};

struct Customer customers[30];

struct Clerk{

	int id;
	
};

struct Clerk clerk0;
struct Clerk clerk1;

struct Customer* queue[MAX_CUSTS];
struct Customer* queue1[MAX_CUSTS];

void insertIntoQueue(struct Customer* f) {
	queue[queueLength] = f;
	queueLength += 1;
}

void removeFromQueue() {
	int x = 0;
	while (x < queueLength-1) {
		queue[x] = queue[x+1];
		x += 1;
	}
	queueLength -= 1;
}

void *ClerkThread(void * clerkNum){
	struct Clerk *cinfo = (struct Clerk*)clerkNum;


	while(1){

		if (queueLength < 1){
			sleep(1);
			continue;
		}	
		else{
			
			struct Customer *info = (struct Customer*)queue[0];

			if (cinfo->id == 0){

				pthread_mutex_lock(&lock);
			
				whichClerk = cinfo->id;

				pthread_cond_signal(&convar);
				pthread_mutex_unlock(&lock);

				pthread_mutex_lock(&lock);
	
				if (queue[0]->id == info->id){

					pthread_cond_wait(&clerkConvar,&lock);
				}
				
				pthread_mutex_unlock(&lock);

			}

			else if (cinfo->id == 1){
				
				pthread_mutex_lock(&lock);
			
				whichClerk = cinfo->id;

				pthread_cond_signal(&convar);
				pthread_mutex_unlock(&lock);
			
				pthread_mutex_lock(&lock);

				if (queue[0]->id == info->id){

					pthread_cond_wait(&clerk1Convar,&lock);

				}
			
				pthread_mutex_unlock(&lock);

			}						
		}
	}

	pthread_exit(NULL);

	return NULL;

}
void *CustomerThread(void *currentCust){
		struct Customer *info = (struct Customer*)currentCust;


	usleep(info->arrivalTime*TIME_MULTIPLIER);

	printf("A customer arrives: customer ID: %d\n",info->id);

	pthread_mutex_lock(&lock);

	insertIntoQueue(info);

	printf("A  customer %d enters a queue: the queue ID: %d, length of queue: %d\n",info->id,whichClerk,queueLength);

	if (queue[0]->id != info->id){
		pthread_cond_wait(&convar,&lock);
	}
	
	pthread_mutex_unlock(&lock);

	int clerk = whichClerk;

	printf("A clerk starts serving a customer: customer Id %d and clerk id %d\n",info->id,clerk);

	usleep(info->serviceTime*TIME_MULTIPLIER);
	
	printf("a clerk %d  finishes serving customer %d\n",clerk,info->id);

	pthread_mutex_lock(&lock);
	
	if (clerk  == 0){

		pthread_cond_signal(&clerkConvar);

	}	

	else if (clerk == 1){
	
		pthread_cond_signal(&clerk1Convar);

	}

	removeFromQueue();
	pthread_mutex_unlock(&lock);
	
	pthread_exit(NULL);

}


int main(){

	pthread_mutex_init(&lock,NULL);

	pthread_mutex_init(&clerkLock,NULL);

	pthread_cond_init(&convar,NULL);

	pthread_cond_init(&clerkConvar,NULL);

	pthread_cond_init(&clerk1Convar,NULL);

	getInput();

	clerk0.id = 0;
	clerk1.id = 1;

	queueLength = 0;

	pthread_t clerk;
	pthread_t clerkOther;	

	pthread_t threads[numOfCustomers];

	int rc;
  	long t;

	pthread_create(&clerk,NULL,ClerkThread,(void*)&clerk0);
	pthread_create(&clerkOther,NULL,ClerkThread,(void*)&clerk1);
	
	for(t=0;t<numOfCustomers;t++){
		printf("In main: creating thread %ld\n", t);
		rc = pthread_create(&threads[t], NULL, CustomerThread, (void *)&customers[t]);
			if (rc){
				printf("ERROR; return code from pthread_create() is %d\n", rc);
				exit(-1);
				}
	}

	int k = 0;

	while (k < numOfCustomers){
		pthread_join(threads[k],NULL);
		k++;
	}

	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&convar);
	pthread_cond_destroy(&clerkConvar);

	exit(0);

   
}
/*
void initializeQueues(){

	int i = 0;
	while (i < NUM_QUEUES){
		queueLength[i] = 0;
		i++;
	}

}*/
/*
int chooseMinLengthQueue(){

	int minQueue = 0;
	int i = 0;

	while (i < NUM_QUEUES){	
		if (queueLength[i] < queueLength[minQueue]){
		minQueue = i;
		break;
		}
	i++;
	}*/
	/* TODO: Handle ties randomly*/
/*	return minQueue;}
*/
void getInput(){

	int i = 0;
	char lines[MAXLINES][BUFSIZ];
	FILE *fp = fopen("customers.txt", "r");

	if (fp == 0){
        	fprintf(stderr, "failed to open input.txt\n");
		 exit(1);
	}

	while (i < MAXLINES && fgets(lines[i], sizeof(lines[0]), fp)){
		lines[i][strlen(lines[i])-1] = '\0';
		i = i + 1;
	}

 	fclose(fp);
	srand(time(0));
	int j = 1;
	numOfCustomers = atoi(lines[0]);

	
	int k = 1;
	int custIndex = 0;

	while (k <= numOfCustomers){
	
		char* token;
		char* tmp = lines[k];
		token = strtok(tmp,":");
	
		customers[custIndex].id = atoi(token);

		char* token2;

		char* tmp2 = lines[1];

		token = strtok(NULL,",");

		customers[custIndex].arrivalTime = atoi(token);

		token = strtok(NULL,",");

		customers[custIndex].serviceTime = atoi(token);

		k++;
		custIndex++;
	}
	
}
