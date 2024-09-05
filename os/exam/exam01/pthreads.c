#include <pthread.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

//ugly globals for now, maybe fix later
int sum;

void *p_add();


int main(int argc, char *argv[]) {

	// Make the thread
	pthread_t tid;
	pthread_attr_t attr;

	// handle params
	if (argc != 3) {
		fprintf(stderr, "usage: p_add.c <integer value> <integer value>\n");
		return -1;
	}

	// pthread prep and creation
	pthread_attr_init(&attr);

	char *padd_args[2];
	padd_args[0] = argv[1];
	padd_args[1] = argv[2];
	pthread_create(&tid, &attr, p_add, padd_args);

	// wait and print
	pthread_join(tid, NULL);
	printf("%d + %d = %d\n", atoi(argv[1]), atoi(argv[2]), sum);
	return 0;
}

void *p_add(void *params[]) {
	sum = atoi(params[0]) + atoi(params[1]);
	pthread_exit(0);
}
