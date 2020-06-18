#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>


//Executes the function in a seperate thread
void *createThread(void *command) {
//executes the command
    system(command);
    printf("\n");
//Exits the child thread
    pthread_exit(NULL);
}

//Infinite loop until the user exits
void inputLoop(){
 char string[20];
//Creates userinput because string gets parsed
 char userInput[20];
    while(1){
 	//Reads the user input
        printf("==> "); 
 	fgets(string,20,stdin);
	strcpy(userInput, string);
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
	   		pthread_create(&child, NULL, createThread, userInput);
			//Ends the child thread
    	   		pthread_join(child, NULL);}
		else if(strcmp(token, "exit\n")==0 || strcmp(token, "quit\n")==0){
			printf("Thank you for using myShell!\n");
			exit(0);}		
    }
}
