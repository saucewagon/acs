/* https://stackoverflow.com/questions/4237084/reading-from-a-file-and-storing-in-array  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { MAXLINES = 30 };

int numOfCustomers;

void getInput();

struct Customer{
		
	int id;
	int serviceTime;
	int arrivalTime;
};

struct Customer customers[30];

int main(){

	getInput();
	
   return 0;
}

void getInput(){

	int i = 0;
    char lines[MAXLINES][BUFSIZ];
    FILE *fp = fopen("customers.txt", "r");

    if (fp == 0)
    {
        fprintf(stderr, "failed to open input.txt\n");
        exit(1);
    }
    while (i < MAXLINES && fgets(lines[i], sizeof(lines[0]), fp))
    {
        lines[i][strlen(lines[i])-1] = '\0';
        i = i + 1;
    }
    fclose(fp);
    printf("%d\n", i);
    srand(time(0));
    int j = 1;
	numOfCustomers = atoi(lines[0]);
	printf("num of customers = %d\n",numOfCustomers);
	while (j < i){

    		printf("%s\n", lines[j]);
		j++; 
	}   

	
	int k = 0;

	while (k < numOfCustomers){
		customers[k].id = 5;
		customers[k].arrivalTime = 6;
		customers[k].serviceTime = 7;
		k++;	
	} 

	k = 0;

	while (k < numOfCustomers){

		printf("customer %d id = %d",k,customers[k].id);
		k++;
	}


