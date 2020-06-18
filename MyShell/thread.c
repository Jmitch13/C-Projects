#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

//Executes the command in a child thread
void *CreateThread(void *command); 
void inputLoop();

int main(void) {
    //Creates the thread
    printf("Welcome to myShell\n");
    inputLoop();
    //Infinite loop until "exit" command is entered
   
    return 0;
}

void *CreateThread(void *command) {
    system(command);
    printf("\n");
    pthread_exit(NULL);
}

void inputLoop(){
 char string[20];
    while(1){
 	//Reads the user input
        printf("==> "); 
 	fgets(string,20,stdin);
	printf("\n");
	//Creates a token to see the first argument of the command in special cases
 	char *token;                                                       
 	token = strtok(string," ");
		//Checks against the list of commands
        	if(strcmp(token,"dir\n")==0 || strcmp(token,"help\n")==0 || strcmp(token,"vol\n")==0 || 		strcmp(token,"path\n")== 0 || strcmp(token,"tasklist\n")==0 || strcmp(token,"notepad\n")==0 ||
        	strcmp(token,"echo")==0 || strcmp(token,"color") == 0 || strcmp(token,"ping") == 0)
		{
			//Creates the thread
			pthread_t child;
	   		pthread_create(&child, NULL, CreateThread, string);
			//Ends the child thread
    	   		pthread_join(child, NULL);}
		else if(strcmp(token, "exit\n")==0){
			printf("Thank you for using myShell!\n");
			exit(0);}		
    }
}
