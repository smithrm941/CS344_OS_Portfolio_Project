#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>

// making room for null terminator in command line and list of arguments
#define MAXCMDLINE 2049
#define MAXARGS 513

// signal handler ignoring CTRL C
void handle_SIGINT(int signo) {
	printf("\n");
	fflush(stdout);
}

// signal handler ignoring CTRL Z
void handle_SIGTSTP(int signo) {
	printf("\n");
	fflush(stdout);
}

// storage for smallsh user's choices
struct userCommands {
	char inputFile[256];
	char outputFile[256];
	char* userArguments[MAXARGS];
	int argumentCount;
};


// defining functions for procesing smallsh user's input and executing commands
// help with processInput code via Painless Programming: https://youtu.be/gnIxlT_40rU
// and Dy Classroom: dyclassroom.com/c/c-function-returning-structure
struct userCommands processInput(void);
void executeCommands(struct userCommands);
int status = 0;


int main() {
	// Ignoring CTRL C in smallsh 
	// code from signal handling API class module 
	struct sigaction SIGINT_action = { 0 };
	SIGINT_action.sa_handler = handle_SIGINT;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;
	sigaction(SIGINT, &SIGINT_action, NULL);


	// Ignoring CTRL Z in smallsh 
	// code from signal handling API class module 
	struct sigaction SIGTSTP_action = { 0 };
	SIGTSTP_action.sa_handler = handle_SIGTSTP;
	sigfillset(&SIGTSTP_action.sa_mask);
	SIGTSTP_action.sa_flags = 0;
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	// process input and execute commands:
	struct userCommands theCommands = processInput();
	executeCommands(theCommands);
}


// defining function that takes input from smallsh user and processes it depeding on input values
// immediately acting on input or storing it in a struct of arguments to be executed
struct userCommands processInput(void) {
	int processingInput = 1;
	while (processingInput == 1) {
		printf(": ");
		fflush(stdout);
		char userInput[MAXARGS];

		// declaring instance of previously defined struct to hold variables to pass to command execution function:
		struct userCommands currentCommands;

		// the variables to pass to command execution function
		char inputFile[256];
		char outputFile[256];
		char userArguments[MAXARGS][256];

		char cwd[256]; // var to hold current working directory
		char* newDirectory; // var for user chosen directory
		char* token; // variable for manipulating input string

		// use fgets to create string of user inputs:
		fgets(userInput, MAXCMDLINE, stdin);

		// remove trailing new line from user input:
		// code found on stack overflow:
		userInput[strlen(userInput) - 1] = 0;

		// ignore comment lines
		if (userInput[0] == '#') {
			continue;
		}

		// ignore blank lines
		if (strcmp(userInput, "") == 0) {
			printf("");
			continue;
		}

		// exit shell if user types "exit"
		else if (strcmp(userInput, "exit") == 0) {
			exit(0);
			break;
		}

		// print status value if user types "status"
		else if (strcmp(userInput, "status") == 0) {
			status = 0;
			printf("exit value %d\n", status);
			fflush(stdout);
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
						// changing $$ in directory names to parent pid:
						int i;
						int parentPid = getpid();
						char parentPidStr[256];
						sprintf(parentPidStr, "%d", parentPid);
						int usedPidVariable = 0;
						for (i = 0; i < strlen(token); i++) {
							if (token[i] == '$' && token[i + 1] == '$') {
								token[i + 1] = '\0';
								token[i] = '\0';
								usedPidVariable = 1;
							}
						}
						if (usedPidVariable == 1) {
							strcat(token, parentPidStr);
						}

						// move to chosen directory or print error message:
						if (chdir(token) == 0) {
							token = strtok(NULL, " ");
							main();
						}
						else {

							printf("Directory not found\n");
							fflush(stdout);
							main();
						}
					}
					// if cd is typed without chosen directory, go to the home directory:
					chdir(getenv("HOME"));
					main();
				}

				// handle if something besides "cd" is typed:
				else {
					// initializing "userArguments" with NULL to ensure there is null terminator at end of arguments:
					int i;
					for (i = 0; i < MAXARGS; i++) {
						currentCommands.userArguments[i] = NULL;
					}

					while (token != NULL) {
						if (strcmp(token, "<") == 0) {
							token = strtok(NULL, " ");
							strcpy(currentCommands.inputFile, token);
						}
						else if (strcmp(token, ">")  == 0) {
							token = strtok(NULL, " ");
							strcpy(currentCommands.outputFile, token);
						}
						else {
							currentCommands.userArguments[currentCommands.argumentCount] = token;
							currentCommands.argumentCount++;
						}
						// changing $$ in arguments to parent pid:
						int i;
						int parentPid = getpid();
						char parentPidStr[256];
						sprintf(parentPidStr, "%d", parentPid);
						int usedPidVariable = 0;
						for (i = 0; i < strlen(token); i++) {
							if (token[i] == '$' && token[i + 1] == '$') {
								token[i + 1] = '\0';
								token[i] = '\0';
								usedPidVariable = 1;
							}
						}
						if (usedPidVariable == 1) {
							strcat(token, parentPidStr);
							token = strtok(NULL, " ");
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

// defining function that takes arguments from user input and executes them
void executeCommands(struct userCommands theCommands) {
	int childStatus;
	// fork new process
	pid_t spawnPid = fork();

	switch (spawnPid) {
	case -1: // error on fork
		perror("fork()\n");
		status = 1;
		printf("exit value %d\n", status);
		exit(1);
		break;
	case 0: // no error on fork

		// code to redirect stdin to source file if "<" was in user's input:
		if (theCommands.inputFile[0] != '\0') {
			// code from prcesses and I/O module
			int sourceFD = open(theCommands.inputFile, O_RDONLY);
			if (sourceFD == -1) {
				perror("source open()");
				status = 1;
				printf("exit value %d\n", status);
				fflush(stdout);
				exit(1);
			}

			int result = dup2(sourceFD, 0);
			if (result == -1) {
				perror("source dup2()");
				status = 2;
				printf("exit value %d\n", status);
				fflush(stdout);
				exit(2);
			}
		}
		
		// code to redirect stdout to source file if ">" was in user's input:
		if (theCommands.outputFile[0] != '\0') {
			// code from processes and I/O module
			int targetFD = open(theCommands.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (targetFD == -1) {
				perror("target open()");
				status = 1;
				printf("exit value %d\n", status);
				fflush(stdout);
				exit(1);
			}

			int result = dup2(targetFD, 1);
			if (result == -1) {
				perror("target dup2()");
				status = 2;
				printf("exit value %d\n", status);
				fflush(stdout);
				exit(2);
			}
			
		}

		// if not redirecting input or output, just execute commands as usual:
		execvp(theCommands.userArguments[0], theCommands.userArguments);
		perror("execvp()");
		status = 1;
		printf("exit value %d\n", status);
		fflush(stdout);
		exit(1);

	default:
		// in parent process, wait for child's termination before running again:
		spawnPid = waitpid(spawnPid, &childStatus, 0);
		fflush(stdin);
		main();
	}
}