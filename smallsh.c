#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>

#define MAXCMDLINE 2049
#define MAXARGS 513

void handle_SIGINT(int signo) {
	printf("\n");
}

void handle_SIGTSTP(int signo) {
	printf("do something!");
}


struct userCommands {
	char inputFile[256];
	char outputFile[256];
	char* regularCommands[MAXARGS];
	int commandCount;
//	int backgroundProcess;

};




// help with processInput code via Painless Programming: https://youtu.be/gnIxlT_40rU
// and Dy Classroom: dyclassroom.com/c/c-function-returning-structure
struct userCommands processInput(void);
void executeCommand(struct userCommands);
int status = 0; // var for exit status

//void handle_SIG


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
		char regularCommands[MAXARGS][256];

		char cwd[256]; // var to hold current working directory
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
			//token[strlen(token) - 1] = 0;
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
					// initializing "regularCommands" with NULL:
					int i;
					for (i = 0; i < MAXARGS; i++) {
						currentCommands.regularCommands[i] = NULL;
					}

					//token = strtok(NULL, " ");
					while (token != NULL) {
						//printf("what is the current token? %s\n", token);
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
						/*else if (strcmp(token, "&") == 0) {
							printf("do the background thing!");
							currentCommands.backgroundProcess = 1;
							token = strtok(NULL, " ");
						}*/
						else {
						//	printf("just a command!\n");
							currentCommands.regularCommands[currentCommands.commandCount] = token;
							currentCommands.commandCount++;
						}
						int i;
						int parentPid = getpid();
						char parentPidStr[256];
						sprintf(parentPidStr, "%d", parentPid);
						int usedPidVariable = 0;
						//char* parentPidStr = sprintf("%s", parentPid);
						for (i = 0; i < strlen(token); i++) {
							//printf("letter from token: %c\n", token[i]);
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

void executeCommand(struct userCommands theCommands) {
	// executing a program module:
	int childStatus;

	// fork new process
	pid_t spawnPid = fork();

	switch (spawnPid) {
	case -1:
		perror("fork()\n");
		exit(1);
		break;
	case 0:
		if (theCommands.inputFile[0] != '\0') {
			printf("we have an input file!\n");
			// code from prcesses and I/O module
			int sourceFD = open(theCommands.inputFile, O_RDONLY);
			if (sourceFD == -1) {
				perror("source open()");
				status = 1;
				printf("exit value %d\n", status);
				exit(1);
			}
			// Written to terminal
			printf("sourceFD == %d\n", sourceFD);

			// Redirect stdin to source file
			int result = dup2(sourceFD, 0);
			if (result == -1) {
				perror("source dup2()");
				status = 2;
				printf("exit value %d\n", status);
				exit(2);
			}
		}
		
		if (theCommands.outputFile[0] != '\0') {
			printf("we have an output file!\n");
			// code from processes and I/O module
			int targetFD = open(theCommands.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (targetFD == -1) {
				perror("target open()");
				status = 1;
				printf("exit value %d\n", status);
				exit(1);
			}
			printf("targetFD == %d\n", targetFD); // written to terminal

			// Redirect stdout to target file
			int result = dup2(targetFD, 1);
			if (result == -1) {
				perror("target dup2()");
				status = 2;
				printf("exit value %d\n", status);
				exit(2);
			}
			
		}
		execvp(theCommands.regularCommands[0], theCommands.regularCommands);
		perror("execvp()");
		exit(1);
		//fflush(stdout);
	default:
		// in parent process, wait for child's termination:
		spawnPid = waitpid(spawnPid, &childStatus, 0);
		// something about signals here?
		//printf("PARENT(%d): child(%d) terminated. Exiting\n", getpid(), spawnPid);
		main();
	}
}