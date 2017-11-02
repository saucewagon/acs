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

int queueLength[NUM_QUEUES];
enum { MAXLINES = 30 };
int numOfCustomers;
void getInput();
void initializeQueues();
int chooseMinLengthQueue();
int queuesAllEmpty();

/* Condition variables for each clerk */
pthread_cond_t clerkConvar;
pthread_cond_t clerk1Convar;

pthread_mutex_t clerkLock;
pthread_mutex_t clerk1Lock;

/* Locks and condition variables for each queue */
pthread_mutex_t lock; /* First queue */
pthread_cond_t convar;
pthread_mutex_t lock1; /* Second queue */
pthread_cond_t convar1;

pthread_mutex_t queueAccessLock;

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

void insertIntoQueue(struct Customer* f,int whichQueue) {

	if (whichQueue == 0){
		queue[queueLength[0]] = f;
		queueLength[0] += 1;
	}
	else if (whichQueue == 1){
		queue1[queueLength[1]] = f;
		queueLength[1] += 1;
	}
}

void removeFromQueue(int whichQueue) {
	
	if (whichQueue == 0){
		int x = 0;
		while (x < queueLength[0]-1) {
			queue[x] = queue[x+1];
			x += 1;
		}
		queueLength[0] -= 1;
	}
	else if (whichQueue == 1){
		int x = 0;
		while (x < queueLength[1]-1){
			queue1[x] = queue1[x+1];
			x+=1;
		}
		queueLength[1] -= 1;
	}
}

void *ClerkThread(void * clerkNum){
	struct Clerk *cinfo = (struct Clerk*)clerkNum;

	while(1){

		if (queueLength[0] > 0 || queueLength[1] > 0){
			int maxQueue;
			pthread_mutex_lock(&queueAccessLock);
			maxQueue = chooseMaxLengthQueue();
			pthread_mutex_unlock(&queueAccessLock);
			if (maxQueue == 0){
				pthread_mutex_lock(&lock);
				struct Customer *info = (struct Customer*)queue[0];
				pthread_mutex_unlock(&lock);
				if (cinfo->id == 0){
		
					pthread_mutex_lock(&lock);
			
					whichClerk = cinfo->id;

					pthread_cond_broadcast(&convar);
					pthread_mutex_unlock(&lock);

					pthread_mutex_lock(&clerkLock);
					pthread_cond_wait(&clerkConvar,&clerkLock);
					pthread_mutex_unlock(&clerkLock);
				}

				else if (cinfo->id == 1){
				
					pthread_mutex_lock(&lock);
			
					whichClerk = cinfo->id;

					pthread_cond_broadcast(&convar);
					pthread_mutex_unlock(&lock);
			
					pthread_mutex_lock(&clerk1Lock);
					pthread_cond_wait(&clerk1Convar,&clerk1Lock);
					pthread_mutex_unlock(&clerk1Lock);
				}
			}

			else if (maxQueue == 1){
				pthread_mutex_lock(&lock1);
				struct Customer *info = (struct Customer*)queue1[0];
				pthread_mutex_unlock(&lock1);
				if (cinfo->id == 0){
				
					pthread_mutex_lock(&lock1);
			
					whichClerk = cinfo->id;

					pthread_cond_broadcast(&convar1);
					pthread_mutex_unlock(&lock1);

					pthread_mutex_lock(&clerkLock);
					pthread_cond_wait(&clerkConvar,&clerkLock);
					pthread_mutex_unlock(&clerkLock);
				}

				else if (cinfo->id == 1){
				
					pthread_mutex_lock(&lock1);
			
					whichClerk = cinfo->id;

					pthread_cond_broadcast(&convar1);
					pthread_mutex_unlock(&lock1);
			
					pthread_mutex_lock(&clerk1Lock);
					pthread_cond_wait(&clerk1Convar,&clerk1Lock);
					pthread_mutex_unlock(&clerk1Lock);
				}
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

	int whichQueue = 0;
	pthread_mutex_lock(&queueAccessLock);
	whichQueue = chooseMinLengthQueue();
	pthread_mutex_unlock(&queueAccessLock);

	if (whichQueue == 0){
		
		pthread_mutex_lock(&lock);

		insertIntoQueue(info,whichQueue);

		printf("A  customer %d enters a queue: the queue ID: %d, length of queue: %d\n",info->id,whichQueue,queueLength[0]);

		if (queue[0]->id != info->id){
			printf("cond wait signalled for queue 0 and customer %d\n",info->id);
			pthread_cond_wait(&convar,&lock);
		}
	
		pthread_mutex_unlock(&lock);

		int clerk = whichClerk;

		printf("A clerk starts serving a customer: customer Id %d and clerk id %d\n",info->id,clerk);

		usleep(info->serviceTime*TIME_MULTIPLIER);
	
		printf("a clerk %d  finishes serving customer %d\n",clerk,info->id);
	
		if (clerk  == 0){
			pthread_mutex_lock(&clerkLock);
			pthread_cond_broadcast(&clerkConvar);
			pthread_mutex_unlock(&clerkLock);
		}	
		
		else if (clerk == 1){
			pthread_mutex_lock(&clerk1Lock);
			pthread_cond_broadcast(&clerk1Convar);
			pthread_mutex_unlock(&clerk1Lock);
		}

		pthread_mutex_lock(&lock);
		removeFromQueue(whichQueue);
		pthread_mutex_unlock(&lock);
	}

	else if (whichQueue == 1){

		pthread_mutex_lock(&lock1);
		insertIntoQueue(info,whichQueue);

		printf("A  customer %d enters a queue: the queue ID: %d, length of queue: %d\n",info->id,whichQueue,queueLength[1]);

		if (queue1[0]->id != info->id){
			pthread_cond_wait(&convar1,&lock1);
		}
	
		pthread_mutex_unlock(&lock1);

		int clerk = whichClerk;

		printf("A clerk starts serving a customer: customer Id %d and clerk id %d\n",info->id,clerk);

		usleep(info->serviceTime*TIME_MULTIPLIER);
	
		printf("a clerk %d  finishes serving customer %d\n",clerk,info->id);

		if (clerk  == 0){
			pthread_mutex_lock(&clerkLock);
			pthread_cond_broadcast(&clerkConvar);
			pthread_mutex_unlock(&clerkLock);
		}	
		else if (clerk == 1){
			pthread_mutex_lock(&clerk1Lock);
			pthread_cond_broadcast(&clerk1Convar);
			pthread_mutex_unlock(&clerk1Lock);
		}
		pthread_mutex_lock(&lock1);
		removeFromQueue(whichQueue);
		pthread_mutex_unlock(&lock1);

	}

		pthread_exit(NULL);

}

int main(){

	pthread_mutex_init(&lock,NULL);
	pthread_mutex_init(&clerkLock,NULL);
	pthread_mutex_init(&clerk1Lock,NULL);
	pthread_mutex_init(&lock1,NULL);
	pthread_mutex_init(&queueAccessLock,NULL);
	pthread_cond_init(&convar,NULL);
	pthread_cond_init(&convar1,NULL);
	pthread_cond_init(&clerkConvar,NULL);
	pthread_cond_init(&clerk1Convar,NULL);

	getInput();
	initializeQueues();	

	clerk0.id = 0;
	clerk1.id = 1;

	pthread_t clerk;
	pthread_t clerkOther;	

	pthread_t threads[numOfCustomers];

	int rc;
  	long t;

	pthread_create(&clerk,NULL,ClerkThread,(void*)&clerk0);
	pthread_create(&clerkOther,NULL,ClerkThread,(void*)&clerk1);
	
	for(t=0; t < numOfCustomers; t++){
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

int queuesAllEmpty(){
	int notempty = 0;

	int i;

	while (i < NUM_QUEUES){
		if (queueLength[i] > 0){
			return 0;
		}

	}
	return 1;

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
		}
	i++;
	}
	/* TODO: Handle ties randomly*/
	return minQueue;}

int chooseMaxLengthQueue(){

	int maxQueue = 0;
	int i = 0;

	while (i < NUM_QUEUES){

		if (queueLength[i] > queueLength[maxQueue]){
			maxQueue = i;
		}
		i++;
	}
	return maxQueue;
	

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
