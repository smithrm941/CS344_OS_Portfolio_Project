#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
	char userInput[2048];
	char cwd[256];
	int status;
	char* newDirectory;
	pid_t spawnpid = -5;

	struct userCommand
	{
		char* command;
		char* inputFile;
		char* outputFile;
		char* background;
	};


	while (1) {
		struct userCommand* currVal = malloc(sizeof(struct userCommand));

		printf(": ");
		scanf("%[^\n]%*c", userInput);

		// user command is first argument user typed:
		char* token;
		token = strtok(userInput, " ");
		
		// exit smallsh if command is 'exit':
		if (strcmp(token, "exit") == 0) {
			exit(0);
		}
		// if the first argument is 'cd' either change directory to chosen directory
		// or home if no directory is chosen:
		else if (strcmp(token, "cd") == 0) {
			token = strtok(NULL, " ");
			// if a directory is chosen:
			while (token != NULL) {
				newDirectory = token;
				chdir(newDirectory);
				token = strtok(NULL, " ");
				main();
			} 
			// if cd is typed without chosen directory:
			    getcwd(cwd, 256);
				chdir(getenv("HOME"));
		}

		else if (strcmp(token, "status") == 0) {
			// PLACEHOLDER - signal of last terminating process here
			// otherwise if before any foreground process is run:
			status = 0;
			printf("exit value %d\n", status);
			exit(0);
		}
		else {
			spawnpid = fork();
			if (spawnpid == 0) {
				printf("place holder for executing other commands!\n");
			}
		}
	}

	
}