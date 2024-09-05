#include <pthread.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

//ugly globals for now, maybe fix later
uint64_t *fibbs;

void *fibby();


int main(int argc, char *argv[]) {

	// Make the thread
	pthread_t tid;
	pthread_attr_t attr;

	// handle params
	if (argc != 2) {
		fprintf(stderr, "usage: fibby.c <integer value>\n");
		return -1;
	}
	uint64_t num_fibbs = atoi(argv[1]);
	if (num_fibbs < 1) {
		fprintf(stderr, "%" PRIu64 "must be greater than 1\n", num_fibbs);	
		return -1;		
	}
	// 94 is the highest we can go with 64 bits
	if (num_fibbs > 93) {
		fprintf(stderr, "%" PRIu64 "must be less than 94\n", num_fibbs);
		return -1;
	}

	// pthread prep and creation
	if ( !(fibbs = calloc(num_fibbs, sizeof *fibbs)) ) {
		fprintf(stderr, "could not alloc for %" PRIu64 "fibbonacci numbers!", num_fibbs);
	}
	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, fibby, argv[1]);

	// wait and print
	pthread_join(tid, NULL);
	printf("Generated %lu fibonacci numbers:\n", num_fibbs);
	for(uint64_t i = 0; i < num_fibbs; i++) {
		printf("%3" PRIu64 ":  %-10" PRIu64 "\n", i, fibbs[i]);
	}
	return 0;
}

void *fibby(void *param) {
	fibbs[0] = 0;
	fibbs[1] = 1;
	for (int i = 2; i < atoi(param); i++) {
		fibbs[i] = fibbs[i-1] + fibbs[i-2];
	}
	pthread_exit(0);
}
