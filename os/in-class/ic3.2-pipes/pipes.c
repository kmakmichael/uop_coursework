/*	Michael Kmak
 *	In-Class Assignment 3-2: Pipes
 *
 * 	Referenced:
 * 	https://tldp.org/LDP/lpg/node11.html
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char invert(char c) {
	if (c >= 'a' && c <= 'z')
		return c - 32;
	if (c >= 'A' && c <= 'Z')
		return c + 32;
	return c;
}
int main() {
	char message[] = "PiPing hot PiPes\n";
	char buffer[32];
	int input_pipe[2];
	int output_pipe[2];
	int num_bytes;

	pipe(input_pipe);
	pipe(output_pipe);

	pid_t pid;
	pid = fork();

	if (pid < 0) {
		perror("Error forking child process");
		return -1;
	} else if (pid == 0) {	
		// Receive string to invert
		close(input_pipe[1]);
		read(input_pipe[0], buffer, sizeof(buffer));

		for (int i = 0; i < sizeof(buffer); i++) {
			buffer[i] = invert(buffer[i]);
		}

		// Return inverted string
		close(output_pipe[0]);
                write(output_pipe[1], buffer, (strlen(buffer) + 1));
	} else {
		printf("Original string: %s\n", message);
	 	// Send string
                close(input_pipe[0]);
		write(input_pipe[1], message, (strlen(message) + 1));
		

                // Receive modified string 
		close(output_pipe[1]);
                read(output_pipe[0], buffer, sizeof(buffer));
                printf("Inverted string: %s\n", buffer);
	}
	return 0;
}
