/*
 * Lab 04: Majority Voting
 * COMP 173: Operating Systems
 * Michael Kmak
 * Fall 2020
 *
 * Implements a random majority voting system using fork() and pipe().
 *
 * The user is asked for the number of voters (N), between 3 and 
 * 4096 inclusive). The parent process then generates N children,
 * which each generate a random 32-bit number and send it back to 
 * the parent via a pipe. This 32-bit number represents their random
 * votes for each round of voting (32 rounds total). Once this is
 * sent, the child process can exit. The parent waits for all
 * children to exit, collects the votes, processes them by round,
 * and then announces the majority decision.
 *
 * Notes:
 * 	- Any amount of voters above about 5,000
 * 	  fails to create enough forks, and 4096
 * 	  is a nice round number to cap votes at.
 *	- The random number bitstream probably
 *	  comes out slightly different than if
 *	  i were to randomly decide each vote,
 *	  but after a few tests it seems to be
 *	  close enough to 50/50.
 *	- rand() is seeded by the process' pid
 *	  * time(0). With time(0) alone, children
 *	  would often have the same seed.
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

int main() {
	uint16_t num_voters = 0;
	pid_t pid;
	uint32_t *votes; //all N vote sequences
	int **pipes; // all N pipes
	uint32_t vote; //buffer for parent and randomly generated vote for child

	// parameter input
	while(1) {
		printf("Number of voters? ");
		scanf("%5hu", &num_voters);
		if(num_voters > 2 && num_voters < 4096)
			break;
		printf("There must be between 2 and 4096 voters\n");
	}

	// pipe & child creation
	pipes = calloc(num_voters, sizeof(int *));
	votes = calloc(num_voters, sizeof(uint32_t));
	for (int i = 0; i < num_voters; i++)
	{
		if (pid > 0) {
			pipes[i] = calloc(2, sizeof(int));
			pipe(pipes[i]);
			pid = fork();
			if (pid < 0) {
				printf("fork failed!");
				return 0;

			// The children send in all of their votes in 
			// one transfer, so we can create them, send 
			// the message, and kill them all in this block.
			} else if (pid == 0) {
				close(pipes[i][0]);
				srand(getpid() * time(0));
				vote = rand();
				write(pipes[i][1], &vote, sizeof(uint32_t));
				exit(0);
			}
		}
	}

	// read and count votes
	if (pid > 0) {
		int active_children = num_voters;
		printf("Waiting for children to vote...\n");
		while(active_children > 0) {
			wait(NULL);
			--active_children;
		}
		printf("Reading votes\n");
		for(int i = 0; i < num_voters; i++) {
			close(pipes[i][1]);
			read(pipes[i][0], &vote, sizeof(uint32_t));
			votes[i] = vote;
		}
		printf("Counting votes\n");
		uint16_t num_pass = 0;

		// Each voter sends in a 32-bit number. Each bit of this number is read
		// as a vote, 1 for yes and 0 for no. Thus, there are 32 rounds of voting.
		for(int bit = 0; bit < 32; bit++) {
			uint16_t num_yes = 0;
			for(int i = 0; i < num_voters; i++) {
				if(votes[i] & (uint32_t) 1 != 0)
					num_yes++;	
				votes[i] = votes[i] >> 1;
			}
			if (num_yes >= 0.5 * (double) num_voters) {
				num_pass++;
			}
		}
		if (num_pass > 16) 
			printf("Vote passed, winning %u rounds of 32\n", num_pass);
		else
			printf("Vote failed, winning %u rounds of 32\n", num_pass);
	}

	//free memory
	for(int i = 0; i < num_voters; i++) {
		free(pipes[i]);
	}
	free(pipes);
	free(votes);

	return 0;
}
