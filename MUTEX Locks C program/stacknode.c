/*
Programming Assignment 3
Authors: Joshua Mitchell, Joshua Nathan
Compiled with gcc 7.5.0 on ubuntu 18.04
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock; //initialize mutex 



// Linked list node
typedef int value_t;
typedef struct Node
{
    value_t data;
    struct Node *next;
} StackNode;





// Stack function declarations
void push (value_t v, StackNode **top);
value_t pop (StackNode **top);
int is_empty (StackNode *top);


void *testStack(void *top)
{
    for(int i = 0 ; i < 500 ; i++){
        push(rand(), top);
        pop(top);
        push(rand(), top);
        push(rand(), top);
        pop(top);
        pop(top);
    }
    printf("---------thread complete!\n");
}


int main(void)
{
    StackNode *top = NULL;
    int numthreads = 200;
    pthread_t tid;
    //void *status = 0;
    int count; 
    for (count = 0 ; count < numthreads ; count++){
        pthread_create( &tid, NULL, testStack, &top);
        printf("Working on thread: %d \n", count+1);
        pthread_join(tid, NULL);
    }
    return 0;
}








// Stack function definitions
void push(value_t v, StackNode **top)
{
    pthread_mutex_lock(&lock);
    StackNode * new_node = malloc(sizeof(StackNode));
    new_node->data = v;
    new_node->next = *top;
    *top           = new_node;
    // printf("%d \n", new_node-> data); //some printf debugging here
    printf("pushing: %d \n", v); //also this
    pthread_mutex_unlock(&lock);
}




value_t pop(StackNode **top)
{
    // printf("first line of pop \n");
    if (is_empty(*top)){
        return (value_t)0;
    }
    // printf("after if statement \n");
    // printf("locking pop \n");
    pthread_mutex_lock(&lock);
    value_t data = (*top)->data;
    StackNode * temp = *top;
    *top = (*top)->next;
    free(temp);
    printf("popping: %d \n", data);
    pthread_mutex_unlock(&lock);
    return data;
}




int is_empty(StackNode *top) {
    pthread_mutex_lock(&lock);
    if (top == NULL){
        pthread_mutex_unlock(&lock);
        return 1;
    }
    else{
        pthread_mutex_unlock(&lock);
        return 0;
    }
}