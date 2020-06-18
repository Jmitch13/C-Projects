#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define SIZE ( sizeof(list)/sizeof(*list))
int list[] = {7, 12, 19, 3, 18, 4, 2, -5, 6, 15, 8};
int result[SIZE] = {0};
int count = 0;


typedef struct
{
int *subArray;
unsigned int size;
} SortingThreadParameters;

typedef struct
{
SortingThreadParameters left;
SortingThreadParameters right;
} MergingThreadParameters;


void *sort(void *params){
 SortingThreadParameters *data = (SortingThreadParameters *) params;
 int i, j, temp; 
    //nested for loop
    for (i = 0; i < data->size; i++) { 
        for (j = i + 1; j < data->size; j++) { 
  	    //If the element prior is larger than the given, flip it
            if (*(data->subArray + j) < *(data->subArray + i)) { 
                temp = *(data -> subArray + i); 
                *(data -> subArray + i) = *(data -> subArray + j); 
                *(data -> subArray + j) = temp; 
            } 
        } 
    } 
    // print the numbers 

    for (i = 0; i < data -> size; i++){ 
    printf("%d ", *(data -> subArray + i));}
    printf("\n");
    pthread_exit(NULL); 
} 

void *merge(void *params){
 MergingThreadParameters *data = (MergingThreadParameters *) params;
    int i = 0, j = 0, k = 0, a, b;
    //Simplicity 
    a = data -> left.size;
    b = data -> right.size;
    //Traverse through at least of the arrays fully
    while(i < a && j < b){
	//Checks to if the value from the left is greater than the right
	if (data -> left.subArray[i] <= data -> right.subArray[j]){
`		//The smaller of the comparison is pushed to the Result
		result[k] = data -> left.subArray[i];
		i++;}
	else{
		result[k] = data -> right.subArray[j];
		j++;} 
	k++;}
    //After one of the arrays is completed, pushes the rest of the values to the result
    if(i < a) {
	for(i; i < a; i++)
	result[k] = data -> left.subArray[i];
	k++;}
    else {
	for(j; j < b; j++)
	result[k] = data -> right.subArray[j];
	k++;
    }
for(int z = 0; z < SIZE; z++){
    printf("%d ", result[z]);}
    pthread_exit(NULL); 
} 



//Sort using pointers
int main(){	
//Creating the parameters for the left thread
SortingThreadParameters *paramsLeft = malloc(sizeof(SortingThreadParameters));
paramsLeft -> subArray = list;
paramsLeft -> size = SIZE/2;
pthread_t leftChild, rightChild, merging;
pthread_create(&leftChild, NULL, sort, paramsLeft);
//Creating the parameters for the right thread
SortingThreadParameters *paramsRight = malloc(sizeof(SortingThreadParameters));
paramsRight -> subArray = list + paramsLeft -> size;
paramsRight -> size = SIZE - paramsLeft -> size;
pthread_create(&rightChild, NULL, sort, paramsRight);
//Joins the threads
pthread_join(leftChild, NULL);
pthread_join(rightChild, NULL);
//Merges and sorts the new threads
MergingThreadParameters *paramsMerge = malloc(sizeof(MergingThreadParameters));
paramsMerge -> left = *paramsLeft;
paramsMerge -> right = *paramsRight;
pthread_create(&merging, NULL, merge, paramsMerge); 
pthread_join(merging, NULL);
return 0;
}
