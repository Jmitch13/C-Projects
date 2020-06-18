/*
Programming Assignment 3
Authors: Joshua Mitchell, Joshua Nathan
Compiled with gcc 7.5.0 on ubuntu 18.04
*/

#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

mutex mtx; //initialize mutex 



// Linked list node
typedef int value_t;
typedef struct Node
{
    value_t data;
    struct Node *next;
} StackNode;



// Stack function declarations
void push (value_t v, StackNode *top);
value_t pop (StackNode *top);
int is_empty (StackNode top);


void *testStack(void *top)
{
    for(int i = 0 ; i < 500 ; i++){
        push(rand() % 10, top);
        pop(top);
        push(rand() % 10, top);
        push(rand() % 10, top);
        pop(top);
        pop(top);
    }
    cout << "---------thread complete!\n";
}


int main(void)
{
    StackNode *top = NULL;
    int numthreads = 200;
    //void *status = 0;
    int count; 
    for (count = 0 ; count < numthreads ; count++){
        thread tid(top);
        cout << "Working on thread: " << count << "\n";
        tid.join();
    }
    return 0;
}



// Stack function definitions
void push(value_t v, StackNode *top)
{
    mtx.lock();
    StackNode * new_node = sizeof(StackNode);
    new_node->data = v;
    new_node->next = top;
    top           = new_node;
    // printf("%d \n", new_node-> data); //some printf debugging here
    cout << "pushing: " << v << "\n"; //also this
    mtx.unlock();
}




value_t pop(StackNode *top)
{
    // printf("first line of pop \n");
    if (is_empty(top)){
        return (value_t)0;
    }
    // printf("after if statement \n");
    // printf("locking pop \n");
    mtx.lock();
    value_t data = (top)->data;
    StackNode *temp = top;
    top = (top)->next;
    free(temp);
    cout << "popping: " << data << "\n";
    mtx.unlock();
    return data;
}




int is_empty(StackNode *top) {
    mtx.lock();
    if (top == NULL){
        mtx.unlock();
        return 1;
    }
    else{
        mtx.unlock();
        return 0;
    }
}
