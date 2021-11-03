#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#define MAXCMDLINE 2049 // to account for the null char
#define MAXARGS 512

void handle_SIGINT(int signo) {
	printf("\n");
}

int main() {
	// help with userInput code via Painless Programming: https://youtu.be/gnIxlT_40rU
	char userInput[MAXARGS];
	char cwd[256]; // var to hold current working directory
	int status; // var for exit status
	char* newDirectory; // var for usr chosen directory
	pid_t spawnpid = -5;

	// from signal handling API module - ignoring CTRL C
	struct sigaction SIGINT_action = { 0 };
	SIGINT_action.sa_handler = handle_SIGINT;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;
	sigaction(SIGINT, &SIGINT_action, NULL);
	//sigaction(SIGQUIT, &SIGINT_action, NULL);

	// make these variables without the struct stuff:
	struct userCommand
	{
		char* command;
		char* inputFile;
		char* outputFile;
		char* background;
	};




	while (1) {

		// getting user input and either use built in commands or pass it to variables that will determine input thingy, output thingy, or regular command: 
		printf(": ");
		char* token;
		token = strtok(userInput, " ");

		// use fgets to fill the argument array:
		fgets(userInput, MAXCMDLINE, stdin);

		// ignore comment lines
		if (userInput[0] == '#')  {
			continue;
		}

		// ignore blank lines
		if (strncmp(userInput, " ", strlen(userInput)-1) == 0) {
			continue;
		}

		// exit shell if user types "exit"
		else if (strncmp(userInput, "exit", strlen(userInput) - 1) == 0) {
			exit(0);
		}

		// print status value if user types "status"
		else if (strncmp(userInput, "status", strlen(userInput) - 1) == 0) {
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

				if (strncmp(token, "cd", strlen(token) - 1) == 0) {
					// if next token is not NULL, try to go to that directory:
					token = strtok(NULL, " ");
					while (token != NULL) {

						// remove trailing new line from before passing token to chdir:
						// code found on stack overflow:
						token[strlen(token) - 1] = 0;

						chdir(token);
						printf("this should be blablah/vsprojects: %s\n", getcwd(cwd, 256));
						if (chdir(token) == 0) {
							printf("pwd: %s\n", getcwd(cwd, 256));
							token = strtok(NULL, " ");
							main();
						}
						else {
							printf("what is the token now? %s", token);
							printf("Directory not found\n");
							main();
						}
					}
					// if cd is typed without chosen directory, go to the home directory:
					chdir(getenv("HOME"));
					printf("printing this one? pwd: %s\n", getcwd(cwd, 256));
					main();
				}

				// take each of these tokens and if no symbol then add to argument array
				// otherwise make if < then add to input file variable or with > make outputfile or & (background??) 
				// then continue???
			}
			token = strtok(NULL, " ");


		}
	}


	// PLACEHOLDER - function with execvp, child process stuff etc here:
	// take the values from the command array, input file, output file 

	/*spawnpid = fork();
	switch (spawnpid) {
	case -1:
		// perror
	}
	case 0:
		// this means the fork was ok so execute command
		// take the array with the arguments and use in execlp?
		// execlp to run chosen command
		// execlp (token, token,
		// perror
		// exit(EXIT_FAILURE)
	case 1:
		// child exits?


	// or take argument with < or > for redirect
	// or & and do that stuff*/



	
}