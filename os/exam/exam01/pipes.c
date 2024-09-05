#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
	char buffer[32];
	int input_pipe[2];
	int output_pipe[2];
	int num_bytes;

	pipe(input_pipe);
	pipe(output_pipe);

	// handle params
	if (argc != 3) {
		fprintf(stderr, "usage: pipes.c <integer value> <integer value>\n");
		return -1;
	}

	pid_t pid;
	pid = fork();

	if (pid < 0) {
		perror("Error forking child process");
		return -1;
	} else if (pid == 0) {	
		// Receive string to invert
		close(input_pipe[1]);
		read(input_pipe[0], buffer, sizeof buffer);
		char *ptr;
		int num_1 = strtol(buffer, &ptr, 10);
		int num_2 = strtol(ptr, &ptr, 10);
		int sum = num_1 + num_2;

		sprintf(buffer, "%d", sum);
		close(output_pipe[0]);
        write(output_pipe[1], buffer, (strlen(buffer) + 1));
		return 0;
	} else {
	 	// Send numbers
        close(input_pipe[0]);
		sprintf(buffer, "%s %s", argv[1], argv[2]);
		write(input_pipe[1], buffer, strlen(buffer) + 1);
		//write(input_pipe[1], argv[1], strlen(argv[1]) + 1);
		//write(input_pipe[1], argv[2], strlen(argv[2]) + 1);
		

        // Receive modified string
		close(output_pipe[1]);
        read(output_pipe[0], buffer, sizeof buffer);
        printf("%d + %d = %s\n", atoi(argv[1]), atoi(argv[2]), buffer);
	}
	return 0;
}
