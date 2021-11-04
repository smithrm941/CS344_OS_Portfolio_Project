#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#define MAXCMDLINE 2049
#define MAXARGS 512

void handle_SIGINT(int signo) {
	printf("\n");
}


struct userCommands {
	char inputFile[256];
	char outputFile[256];
	char regularCommands[512][256];
	int commandCount;
	//int i;
/*	for (i = 0; i < userCommands.commandCount; i++) {
		userCommands.regularCommands[i] = 0;
	}*/
};




// help with processInput code via Painless Programming: https://youtu.be/gnIxlT_40rU
// and Dy Classroom: dyclassroom.com/c/c-function-returning-structure
struct userCommands processInput(void);
void executeCommand(struct userCommands);


int main() {
	// Ignoring CTRL C in smallsh 
	// code from signal handling API class module 
	struct sigaction SIGINT_action = { 0 };
	SIGINT_action.sa_handler = handle_SIGINT;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;
	sigaction(SIGINT, &SIGINT_action, NULL);


	char command[256];
	char backgroundProcess[256];


	struct userCommands theCommands = processInput();
	executeCommand(theCommands);
		
}

struct userCommands processInput(void) {
	int processingInput = 1;
	while (processingInput == 1) {
		printf(": ");

		char userInput[MAXARGS];

		// declaring a struct to hold our variables to pass to command execution function:
		struct userCommands currentCommands;
		// ******************************************************************************

		char inputFile[256];
		char outputFile[256];
		//int commandCount = 0;
		char regularCommands[512][256];

		char cwd[256]; // var to hold current working directory
		int status; // var for exit status
		char* newDirectory; // var for usr chosen directory
		pid_t spawnpid = -5;
		char* token;

		// use fgets to fill the argument variable with user inputs:
		fgets(userInput, MAXCMDLINE, stdin);

		// remove trailing new line from before passing token to chdir:
		// code found on stack overflow:
		userInput[strlen(userInput) - 1] = 0;

		// ignore comment lines
		if (userInput[0] == '#') {
			continue;
		}

		// ignore blank lines
		if (strcmp(userInput, " ") == 0) {
			continue;
		}

		// exit shell if user types "exit"
		else if (strcmp(userInput, "exit") == 0) {
			exit(0);
		}

		// print status value if user types "status"
		else if (strcmp(userInput, "status") == 0) {
			// PLACEHOLDER - signal of last terminating process here
			// otherwise if before any foreground process is run:
			status = 0;
			printf("exit value %d\n", status);
			fflush(stdin);
		}

		// handling everything else other than blank lines, comments, "exit" and "status":
		else {

			// breaking user input into tokens seperated by space
			token = strtok(userInput, " ");
			while (token != NULL) {

				// built-in function for changing directory within smallsh:
				if (strcmp(token, "cd") == 0) {
					// if user types another word beisdes cd, try to go to directory with that name:
					token = strtok(NULL, " ");
					while (token != NULL) {

						// move to chosen directory or print error message:
						if (chdir(token) == 0) {
							printf("pwd: %s\n", getcwd(cwd, 256));
							token = strtok(NULL, " ");
							main();
						}
						else {
							printf("Directory not found\n");
							main();
						}
					}
					// if cd is typed without chosen directory, go to the home directory:
					chdir(getenv("HOME"));
					printf("pwd: %s\n", getcwd(cwd, 256));
					main();
				}

				// handle if something besides cd is typed:
				else {
					//token = strtok(NULL, " ");
					while (token != NULL) {
						printf("what is the current token? %s\n", token);
						if (strcmp(token, "<") == 0) {
							printf("next one is input file!\n");
							token = strtok(NULL, " ");
							strcpy(currentCommands.inputFile, token);
							printf("the input file: %s\n", currentCommands.inputFile);
						}
						else if (strcmp(token, ">")  == 0) {
							printf("next one is output file!\n");
							token = strtok(NULL, " ");
							strcpy(currentCommands.outputFile, token);
							printf("the output file: %s\n", currentCommands.outputFile);
						}
						else {
							printf("just a command!\n");
							strcpy(currentCommands.regularCommands[currentCommands.commandCount], token);
							printf("the command: %s\n", currentCommands.regularCommands[currentCommands.commandCount]);
							currentCommands.commandCount++;
						}
						
						token = strtok(NULL, " ");
						
					}
					
				}
			}
			token = strtok(NULL, " ");
		}
		processingInput = 0;
		return currentCommands;
	}
}

void executeCommand(struct userCommands theCommands) {
	// executing a program module:
	int childStatus;
	// filler until I get my commands working:
	char* argument_list[] = { "ls", "-l", NULL };
	// fork new process
	pid_t spawnPid = fork();
	printf("is this the thing? %s", theCommands.regularCommands[0]);

	switch (spawnPid) {
	case -1:
		perror("fork()\n");
		exit(1);
		break;
	case 0:
		// replace current program with user's command
		//execvp ? ? for theCommands.regularCommands[0];
		//execl(theCommands.regularCommands[0], theCommands.regularCommands[0], NULL);
		//execvp(theCommands.regularCommands[0], theCommands.regularCommands);
		execvp(theCommands.regularCommands[0], argument_list);
	default:
		// in parent process, wait for child's termination:
		spawnPid = waitpid(spawnPid, &childStatus, 0);
		printf("PARENT(%d): child(%d) terminated. Exiting\n", getpid(), spawnPid);
		exit(0);
		break;
	}
	/*while (1) {
		printf("accessing the struct? Input: %s\n", theCommands.inputFile);
		printf("accessing the struct? Output: %s\n", theCommands.outputFile);
		int i;
		for (i = 0; i < theCommands.commandCount; i++) {
			printf("accessing the struct? Commands: %s\n", theCommands.regularCommands[i]);
		}
		break;
	}*/
}