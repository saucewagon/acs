/* https://stackoverflow.com/questions/4237084/reading-from-a-file-and-storing-in-array  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 20

#define NUM_CLERKS 2
#define NUM_QUEUES 4
#define TIME_MULTIPLIER 100000

int queueLength[NUM_QUEUES];

enum { MAXLINES = 30 };

int numOfCustomers;

void getInput();
void initializeQueues();
int chooseMinLengthQueue();

pthread_cond_t convar;

struct Customer{
		
	int id;
	int serviceTime;
	int arrivalTime;
};

struct Customer customers[30];

pthread_mutex_t lock;

pthread_mutex_t queue0lock;
pthread_mutex_t queue1lock;
pthread_mutex_t queue2lock;
pthread_mutex_t queue3lock;

int custWaiting = 0;

void *ClerkThread(void * clerkNum){

	while(1){

		if (custWaiting){
		
			pthread_mutex_lock(&lock);
			pthread_cond_signal(&convar);
			pthread_mutex_unlock(&lock);
				
		}


	}

	pthread_exit(NULL);

	return NULL;

}

void *CustomerThread(void *currentCust){

	

	struct Customer *info = (struct Customer*)currentCust;

	usleep(info->arrivalTime*TIME_MULTIPLIER);

	printf("A customer arrives: customer ID: %d\n",info->id);
	custWaiting = 1;
	int minQueue = chooseMinLengthQueue();
	
	pthread_mutex_lock(&lock);

	queueLength[minQueue]++;
	printf("A  customer enters a queue: the queue ID: %d, length of queue: %d\n",minQueue,queueLength[minQueue]);
	
	while (!convar){
		pthread_cond_wait(&convar,&lock);		
	
	}
	
pthread_mutex_unlock(&lock);

	printf("A clerk starts serving a customer: customer Id and clerk id\n");

	usleep(info->serviceTime*TIME_MULTIPLIER);

	printf("A clerk finishes serving  customer %d\n",info->id);

	queueLength[minQueue]--;
	custWaiting = 0;
	
	
	pthread_exit(NULL);

}


int main(){


	pthread_mutex_init(&lock,NULL);

	pthread_mutex_init(&queue0lock,NULL);
	pthread_mutex_init(&queue1lock,NULL);
	pthread_mutex_init(&queue2lock,NULL);
	pthread_mutex_init(&queue3lock,NULL);
	pthread_cond_init(&convar,NULL);
	initializeQueues();
	printf("%d\n",numOfCustomers);
	getInput();

	pthread_t clerks[NUM_CLERKS];

	int i;

	while (i < NUM_CLERKS){
		pthread_create(&clerks[i],NULL,ClerkThread,&i);
	}


	printf("%d\n",numOfCustomers);
	pthread_t threads[numOfCustomers];


	int rc;
  	long t;

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

	pthread_mutex_destroy(&queue0lock);
	pthread_mutex_destroy(&queue1lock);
	pthread_mutex_destroy(&queue2lock);
	pthread_mutex_destroy(&queue3lock);

	exit(0);

   
}

void initializeQueues(){

	int i = 0;
	while (i < NUM_QUEUES){
		queueLength[i] = 0;
		i++;
	}

}

int chooseMinLengthQueue(){

	int minQueue = 0;
	int i = 0;

	while (i < NUM_QUEUES){	
		if (queueLength[i] < queueLength[minQueue]){
		minQueue = i;
		break;
		}
	i++;
	}
	/* TODO: Handle ties randomly*/
	return minQueue;
}

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
