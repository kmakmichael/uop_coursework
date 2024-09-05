/*
    Lab 07: Deadlock Detection
    COMP 173: Operating Systems (Fall 2020)
    Michael Kmak

    compilation: gcc main.c -o main
    run: ./main

        Simulates the Banker's Algorithm with
    text file input. When a deadlock is
    detected, it will print the processes
    involved and ask the user for another
    text file. When the state is safe, it
    will print a sequence proving such. 
*/
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    size_t process_count;
    size_t resource_count;
    uint32_t *available;
    uint32_t **max;
    uint32_t **have;
    uint32_t **need;
} system_state;

typedef struct {
    size_t count;
    size_t *processes;
} sequence;

bool read_file(const char *path, system_state *state);

bool bankers(system_state *s, sequence *solution);
bool affordable(const system_state *s, size_t process);
void diagnose_deadlock(sequence *solution, sequence *tracker);

void alloc_sys_state(system_state *s);
void free_sys_state(system_state *s);

void print_sys_state(const system_state *s);
void print_sequence(const sequence *solution);
void print_deadlock(const system_state *state, const sequence *solution);


int main() {
    system_state s;
    sequence solution;
    char filename[FILENAME_MAX];
    while(true) {
        printf("\nWhat file would you like to check?\n");
        fgets(filename, FILENAME_MAX, stdin);
        sscanf(filename, "%s", filename); // remove \n
        if (!read_file(filename, &s)) {
            continue;
        }
        //print_sys_state(&s);
        solution.processes = calloc(s.process_count, sizeof(size_t));
        solution.count = 0;
        if (bankers(&s, &solution)) {
            printf("Safe! ");
            print_sequence(&solution);
            free_sys_state(&s);
            free(solution.processes);
            break;
        }
        printf("Deadlock!\n");
        print_deadlock(&s, &solution);
        //print_sys_state(&s);
    }
    return 0;
}

/*
    note: allocates the state
*/
bool read_file(const char *path, system_state *s) {
    FILE *fp;

    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "error reading file \"%s\"\n", path);
        return false;
    }
    fscanf(fp, "%zu", &s->process_count);
    fscanf(fp, "%zu", &s->resource_count);

    alloc_sys_state(s);

    // read s.available
    for(size_t i = 0; i < s->resource_count; i++) {
        fscanf(fp, "%"PRIu32, &s->available[i]);
    }

    // read s.have
    for(size_t p = 0; p < s->process_count; p++) {
        for(size_t i = 0; i < s->resource_count; i++) {
            fscanf(fp, "%"PRIu32, &s->have[p][i]);
        }
    }

    // read s.need
    for(size_t p = 0; p < s->process_count; p++) {
        for(size_t i = 0; i < s->resource_count; i++) {
            fscanf(fp, "%"PRIu32, &s->need[p][i]);
            // also fill s.max
            s->max[p][i] = s->have[p][i] + s->need[p][i];
        }
    }
    return true;
}

bool bankers(system_state *state, sequence *solution) {
    /*
        the tracker is the sequence struct used to keep track of
        what processes have run. the .count represents how many
        processes have been terminated. each index in .processes
        represents a process from the state, where 0 means it is
        running and 1 (or anything else) means it has terminated.
    */
    sequence tracker;
    tracker.processes = calloc(state->process_count, sizeof(size_t));
    tracker.count = state->process_count;

    bool empty_cycle = false;
    while (!empty_cycle) {
        //printf("\n============\nCYCLE BEGIN\n============\n");
        empty_cycle = true;
        for (size_t p = 0; p < state->process_count; p++) {
            //printf("\n== PROCESS %zu (%zu) ==\n", p, tracker.processes[p]);
            //skip if process is terminated
            if (tracker.processes[p] != 0) {
                //printf("\tProcess %zu is complete\n", p);
                continue;
            }
            // if the request is doable
            if (affordable(state, p)) {
                //printf("\tgranting request of process %zu\n", p);
                //grant the request
                for(size_t r = 0; r < state->resource_count; r++) {
                    state->available[r] += state->have[p][r];
                    state->need[p][r] = 0;
                    state->have[p][r] = 0;
                }
                // "terminate" the process
                tracker.processes[p] = 1;

                // add it to the solution sequence
                //printf("adding %zu at solution[%zu]\n", p, solution->count);
                solution->processes[solution->count] = p;
                solution->count += 1;
            
                // raise the flag
                empty_cycle = false;
            }
        }
    }

    // if we didn't terminate every process, then we're in deadlock
    //printf("Completed. Terminated %zu/%zu processes\n", solution->count, state->process_count);
    if (solution->count < state->process_count) {
        diagnose_deadlock(solution, &tracker);
        //free(tracker.processes);
        return false;
    }
    //free(tracker.processes);
    return true;
}

bool affordable(const system_state *s, size_t process) {
    /*printf("--> Affordability check <--\n");
    printf("\tAvailable:\t{");
        for(size_t i = 0; i < s->resource_count; i++) {
            printf(" %" PRIu32 " ", s->available[i]);
        }
    printf("}\n");
    printf("\tProcess:\t{");
        for(size_t i = 0; i < s->resource_count; i++) {
            printf(" %" PRIu32 " ", s->need[process][i]);
        }
    printf("}\n");
    */

    bool ret = true;
    for (size_t r = 0; r < s->resource_count; r++) {
        ret &= s->need[process][r] <= s->available[r];
    }
    return ret;
}

void diagnose_deadlock(sequence *solution, sequence *tracker) {
    //printf("Diagnosing deadlock\n");
    solution->count = 0;
    for(size_t p = 0; p < tracker->count; p++) {
        if (tracker->processes[p] == 0) {
            //printf("\tprocess %zu not satisfied\n", p);
            solution->processes[solution->count] = p;
            ++solution->count;
        }
    }
}



void alloc_sys_state(system_state *s) {
    s->available = calloc(s->resource_count, sizeof(uint32_t));

    s->have = calloc(s->process_count, sizeof(uint32_t *));
    s->max = calloc(s->process_count, sizeof(uint32_t *));
    s->need = calloc(s->process_count, sizeof(uint32_t *));

    for(size_t p = 0; p < s->process_count; p++) {
        s->have[p] = calloc(s->resource_count, sizeof(uint32_t));
        s->max[p] = calloc(s->resource_count, sizeof(uint32_t));
        s->need[p] = calloc(s->resource_count, sizeof(uint32_t));
    }
}

void free_sys_state(system_state *s) {
    for(size_t p = 0; p < s->process_count; p++) {
        free(s->have[p]);
        free(s->max[p]);
        free(s->need[p]);
    }
    free(s->have);
    free(s->max);
    free(s->need);
    free(s->available);
}



void print_sys_state(const system_state *s) {
    printf("=== System State ===\n");

    printf("Available:\n");
    printf("\t{");
    for(size_t i = 0; i < s->resource_count; i++) {
        printf(" %" PRIu32 " ", s->available[i]);
    }
    printf("}\n");

    printf("Have:\n");
    for(size_t p = 0; p < s->process_count; p++) {
        printf("\t{");
        for(size_t i = 0; i < s->resource_count; i++) {
            printf(" %" PRIu32 " ", s->have[p][i]);
        }
        printf("}\n");
    }

    printf("Max:\n");
    for(size_t p = 0; p < s->process_count; p++) {
        printf("\t{");
        for(size_t i = 0; i < s->resource_count; i++) {
            printf(" %" PRIu32 " ", s->max[p][i]);
        }
        printf("}\n");
    }

    printf("Need:\n");
    for(size_t p = 0; p < s->process_count; p++) {
        printf("\t{");
        for(size_t i = 0; i < s->resource_count; i++) {
            printf(" %" PRIu32 " ", s->need[p][i]);
        }
        printf("}\n");
    }

    printf("====================\n");
}

void print_sequence(const sequence *solution) {
    printf("{");
    for(size_t p = 0; p < solution->count; p++) {
        printf(" p%zu", solution->processes[p]);
    }
    printf("}\n");
}

void print_deadlock(const system_state *state, const sequence *solution) {
    printf("Processes involved in deadlock:\n");
    printf(" Available:\t{");
    for(size_t r = 0; r < state->resource_count; r++) {
        printf(" %"PRIu32" ", state->available[r]);
    }
    printf("}\n");
    for(size_t p = 0; p < solution->count; p++) {
        printf("\tP%zu:\t{", p);
        for(size_t r = 0; r < state->resource_count; r++) {
            printf(" %"PRIu32" ", state->need[p][r]);
        }
        printf("}\n");
    }
}