/*
 * Lab 05: CPU Scheduling Sim
 * COMP 173: Operating Systems (Fall 2020)
 * Michael Kmak
 *
 * usage: main input_file [FCFS|RR|SJF] [time quantum]
 * 
 * run and compile as you would any C program. simulates three
 * different CPU scheduling algorithms: FCFS, RR, and SJF.
 * 
 */

#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

uint64_t check_method();
uint64_t read_file();
uint64_t check_tq();
void print_processes();
void print_schedule();
void init_schedule();
void sim();
void sim_rr();
int comp_arrival_time(const void *p1, const void *p2);
int comp_burst_time(const void *p1, const void *p2);

typedef struct __process_data {
	pid_t pid;
	uint64_t arrival_time;
	uint64_t burst_time;
} process_data;

typedef struct __scheduling_data {
	process_data process;
	int64_t start_time;
	int64_t end_time;
} scheduling_data;

process_data processes [20];
scheduling_data output [20];
size_t num_processes;
uint64_t method; //FCFS: 1, RR: 2, SJF: 3
uint64_t time_quantum;

int main(int argc, char *argv[]) {
	// handle params
	if (argc < 3 || argc > 4) {
			fprintf(stderr, "usage: main.c input_file [FCFS|RR|SJF] [time quantum]\n");
			return -1;
	}
	// Fill out default arguments
	switch (argc) {
		case 2:
			method = 0;
			break;
		case 3:
			if (!check_method(argv[2]))
				return -1;
			time_quantum = 1;
			break;
		case 4:
			if (!check_method(argv[2]))
				return -1;
			if (!check_tq(argv[3]))
				return -1;
			break;
		default:
			fprintf(stderr, "failed to fill default arguments\n");
			return -1;
	}
	// read file
	if (!read_file(argv[1])) {
		return -1;
	}

	// do the scheduling
	qsort(processes, num_processes, sizeof(process_data), comp_arrival_time);
	switch(method){
		case 1:
			init_schedule();
			sim();
			break;
		case 2:
			init_schedule();
			sim_rr();
			break;
		case 3:
			//we have to start from the process that arrives at time 0, so sort everything but the first process to arrive
			qsort(&processes[1], num_processes - 1, sizeof(process_data), comp_burst_time);
			init_schedule();
			sim();
			break;
		default:
			fprintf(stderr, "failed to schedule: bad method type %" PRIu64 "\n", method);
			return -1;
	}
	printf("\n\t%s:\n", argv[2]);
	print_schedule();
	return 0;
}

uint64_t check_method(const char *meth) {
	if (strcmp("FCFS", meth) == 0) {
		method = 1;
	} else if (strcmp("RR", meth) == 0) {
		method = 2;
	} else if (strcmp("SJF", meth) == 0) {
		method = 3;
	} else {
		fprintf(stderr, "method must be FCFS, RR, or SJF\n");
		return 0;
	}
	return 1;
}

// should do a check to make sure its actually integer
uint64_t check_tq(const char *tq) {
	time_quantum = atoi(tq);
	if (!time_quantum) {
		fprintf(stderr, "time quantum must be a number\n");
	}
	return time_quantum;
}
uint64_t read_file(const char* filename) {
	FILE * read;

	read = fopen(filename, "r");
	if (read == NULL) {
		fprintf(stderr, "error reading file %s\n", filename);
		return 0;
	}
	fscanf(read, "%zu", &num_processes);

	for (size_t i = 0; i < num_processes; i++) {
		fscanf(read, "%"SCNu32, &processes[i].pid);
		fscanf(read, "%"SCNu64, &processes[i].arrival_time);
		fscanf(read, "%"SCNu64, &processes[i].burst_time);
	}
	fclose(read);
	return 1;
}

int comp_arrival_time(const void *entry_1, const void *entry_2) {
	process_data *p1 = (process_data *) entry_1;
	process_data *p2 = (process_data *) entry_2;
	if (p1->arrival_time > p2->arrival_time)
		return 1;
	if (p1->arrival_time < p2->arrival_time)
		return -1;
	return 0;
}

int comp_burst_time(const void *entry_1, const void *entry_2) {
	process_data *p1 = (process_data *) entry_1;
	process_data *p2 = (process_data *) entry_2;
	if (p1->burst_time > p2->burst_time) {
		return 1;
	}
	if (p1->burst_time < p2->burst_time) {
		return -1;
	}
	return 0;
}

void init_schedule() {
	for (size_t i = 0; i < num_processes; i++) {
		output[i].process = processes[i];
		output[i].start_time = -1;
		output[i].end_time = -1;
	}
}

void sim_rr() {
	uint64_t checksum = 1;
	uint64_t current_time = 0;
	uint64_t runtime = time_quantum;

	printf("\t\t%-8s%-16s%-16s%-16s\n", "PID", "Start Time", "End Time", "Running Time");
	while (checksum) {
		checksum = 0;
		for(size_t i = 0; i < num_processes; i++) {
			// cont if its already done
			if(output[i].process.burst_time == 0) {
				continue;
			}
			// set start time
			if(output[i].start_time == -1) {
				output[i].start_time = current_time;
			}
			if (output[i].process.burst_time < time_quantum) {
				runtime = output[i].process.burst_time;
			} else {
				runtime = time_quantum;
			}
			printf("\t\t%-8" PRIu32, output[i].process.pid); //pid
			printf("%-16" PRIu64, current_time); //start time
			current_time += runtime;
			output[i].process.burst_time -= runtime;
			checksum += output[i].process.burst_time;
			output[i].end_time = current_time;
			printf("%-16" PRIu64, current_time); //end time
			printf("%-16" PRIu64 "\n", runtime); //running time
		}
	}
}

void sim() {
	uint64_t current_time = 0;

	for(size_t i = 0; i < num_processes; i++) {
		output[i].start_time = current_time;
		current_time += output[i].process.burst_time;
		output[i].end_time = current_time;
	}
}

void print_schedule() {
	printf("\t\t%-8s%-16s%-16s%-16s%-16s%-16s\n", "PID", "Arrival Time", "Start Time", "End Time", "Running Time", "Waiting Time");
	for(size_t i = 0; i < num_processes; i++) {
		printf("\t\t%-8" PRIu32 "%-16" PRIu64 "%-16" PRIu64 "%-16" PRIu64 "%-16" PRIu64 "%-16" PRIu64 "\n",
			processes[i].pid,
			processes[i].arrival_time,
			output[i].start_time,
			output[i].end_time,
			processes[i].burst_time,
			(output[i].end_time - processes[i].arrival_time) - processes[i].burst_time
		);
	}
}

// just change this to use the global
void print_processes() {
	printf("\t\t%-8s%-16s%-16s\n", "pid", "arrival_time", "burst_time");
	for(size_t i = 0; i < num_processes; i++) {
		printf("\t\t%-8" PRIu32 "%-16" PRIu64 "%-16" PRIu64 "\n", 
			processes[i].pid, 
			processes[i].arrival_time,
			processes[i].burst_time
		);
	}
}

void print_scheduling() {
	printf("\t\t%-8s%-16s%-16s\n", "pid", "arrival_time", "burst_time");
	for(size_t i = 0; i < num_processes; i++) {
		printf("\t\t%-8" PRIu32 "%-16" PRIu64 "%-16" PRIu64 "\n", 
			output[i].process.pid, 
			processes[i].arrival_time,
			processes[i].burst_time
		);
	}
}


