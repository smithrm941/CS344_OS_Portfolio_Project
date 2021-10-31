#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>


int main() {
	char userInput[2048];
	char cwd[256];
	int status;

	while (1) {
		printf(": ");
		scanf("%s", userInput);
		if (strcmp(userInput, "exit") == 0) {
			exit(0);
		}
		else if (strcmp(userInput, "cd") == 0) {
			/* getcwd(cwd, 256);
			printf("current directory: %s\n", getcwd(cwd, 256));
			printf("home directory is: %s\n", getenv("HOME")); */
			chdir(getenv("HOME"));
			// printf("current directory: %s\n", getcwd(cwd, 256));

		}
		else if (strcmp(userInput, "status") == 0) {
			status = 0;
			printf("exit value %d\n", status);
			exit(0);
		}
	}

	
}