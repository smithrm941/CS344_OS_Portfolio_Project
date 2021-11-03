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

// help with gettingInput code via Painless Programming: https://youtu.be/gnIxlT_40rU
void gettingInput();


int main() {
	// Ignoring CTRL C in smallsh 
	// code from signal handling API class module 
	struct sigaction SIGINT_action = { 0 };
	SIGINT_action.sa_handler = handle_SIGINT;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;
	sigaction(SIGINT, &SIGINT_action, NULL);


	char command[256];
	char inputFile[256];
	char outputFile[256];
	char backgroundProcess[256];

	gettingInput();
		
}

void gettingInput() {
	while (1) {
		printf(": ");

		char userInput[MAXARGS];
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
							printf("the input file: %s\n", token);
						}
						else if (strcmp(token, ">")  == 0) {
							printf("next one is output file!\n");
							token = strtok(NULL, " ");
							printf("the output file: %s\n", token);
						}
						else {
							printf("just a command!\n");
							printf("the command: %s\n", token);
						}
						
						token = strtok(NULL, " ");
						
					}

					// check for & becomes background process
					// check for < becomes input_file variable   
					// check for > becomes output_file variable
					// check for $$ becomes process id of smallsh
					// others: become part of command array
					
				}
			}
			token = strtok(NULL, " ");
		}
	}
}