/*
    Lab 06: Sleeping-Barber Problem
    COMP 173: Operating Systems (Fall 2020)
    Michael Kmak

    compilation: gcc barber.c -o barber -pthread
    run: ./barber.c <seats> <customers>

    Simulates the sleeping-barber problem using pthread.
    One thread represents the barber. This will sleep
    until the bell condvar is signalled. When the barber
    wakes up, it'll service the customers starting from
    index 0 of the queue. When it finishes with one
    customer, it will go to the next index of the queue
    until the queue is empty. The barber will loop back
    to the beginning of the queue. The customers are all 
    individual threads, created at random intervals by
    the parent thread. Customers will look for the first
    available seat in the queue (starting from 0) and
    leave if there are none. This, combined with the
    barber's looping pattern, creates a sort of ring
    buffer that takes customers first-come-first-served.
    Execution finishes when the specified number of
    customers has visited the shop.

    Inter-thread communication is accomplished via
    pthread's condvars and mutexes. There are two global
    condvars and three mutexes. One pair functions to
    wake the barber when a customer arrives (the bell),
    as well as wake the barber at the end of the program.
    The other serves only to let the barber know that
    all customers have been served so that its thread
    may be exited. The lone mutex protects the queue
    from concurrent access. Every customer also has a
    condvar/mutex pair. This is used first by the barber
    to pull the customer into the barber's chair (which
    doesn't really exist), and then again to let the
    customer know that the haircut is complete and it
    can exit its thread.

    To verify functionality, the code was run with small
    test batches (3 seats and 5 or 7 customers). The
    proper functionality was calculated on paper and 
    matched with the printed result.

    Unfortunately, the summary() printing method doesn't
    work quite so well. Since it needs the mutex lock to
    read the queue, it often ends up being forced to wait
    until after the data has changed. Functionality was
    verified with the text statements, but the visual is
    still nice when it works. Uncomment its calls to see.
    It should never print twice in a row. [ ] represents
    and empty seat and [*] a filled seat.
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

// how long the barber takes, in microseconds
#define HAIRCUT_TIME 10

void *barber();
void *customer();
int32_t check_waiting_room();
bool customers_waiting();
bool customer_at(size_t seat);
void rest(uint32_t us);
void summary();

size_t seats;
size_t customer_count;

pthread_mutex_t bell_lock;
pthread_cond_t bell;

pthread_mutex_t shop_open_lock;
bool shop_open;

pthread_mutex_t q_lock;

typedef struct {
    pthread_mutex_t *lock;
    pthread_cond_t *cond;
    bool occupied;
} Seat;

Seat **waiting_room;


int main(int argc, char *argv[]) {
    // handle params
	if (argc != 3) {
		fprintf(stderr, "usage: barber.c <seats> <customers>\n");
		return -1;
	}
	seats = atoi(argv[1]);
	if (seats < 1) {
		fprintf(stderr, "Number of seats (%" PRIu64 ") must be greater than 1\n", seats);	
		return -1;		
	}
	if (seats > 5) {
		fprintf(stderr, "Number of seats (%" PRIu64 ") must be less than 6\n", seats);
		return -1;
	}
    customer_count = atoi(argv[2]);
    if (customer_count < 1) {
		fprintf(stderr, "Number of customers (%" PRIu64 ") must be greater than 1\n", seats);	
		return -1;		
	}
	if (customer_count > 256) {
		fprintf(stderr, "Number of customers (%" PRIu64 ") must be less than 256\n", seats);
		return -1;
	}

    // program data
    pthread_t barber_tid;
    pthread_mutex_init(&q_lock, NULL);
    pthread_mutex_init(&shop_open_lock, NULL);
    shop_open = true;
    srand(time(0));

    printf("Creating barber thread...\n");
    pthread_mutex_init(&bell_lock, NULL);
    pthread_cond_init(&bell, NULL);
    pthread_create(&barber_tid, NULL, barber, 0);

    printf("Creating the waiting room ...\n");
    waiting_room = calloc(seats, sizeof *waiting_room);
    for (size_t i = 0; i < seats; i++) {
        waiting_room[i] = calloc(1, sizeof(Seat));
    }

    printf("\n== Opening shop ==\n\n");
    for (size_t i = 0; i < customer_count; i++) {
        pthread_t cus_tid;
        pthread_create(&cus_tid, NULL, customer, 0);
        rest(rand() % 20);
    }

    pthread_mutex_lock(&bell_lock);
    shop_open = false;
    pthread_cond_signal(&bell);
    pthread_mutex_unlock(&bell_lock);
    
    pthread_join(barber_tid, NULL);
    return 0;
}

void *barber(void *param) {
    pthread_cond_t *wc;
    pthread_mutex_t *wl;
    size_t current_seat;

    pthread_mutex_lock(&shop_open_lock);
    while (shop_open) {
        // sleep if there are no customers
        pthread_mutex_unlock(&shop_open_lock);
        pthread_mutex_lock(&bell_lock);
        pthread_cond_wait(&bell, &bell_lock);
        printf("Barber wakes\n");
        pthread_mutex_unlock(&bell_lock);

        while (customers_waiting()) {
            pthread_mutex_lock(&q_lock);
            if (waiting_room[current_seat]->occupied) {
                wc = waiting_room[current_seat]->cond;
                wl = waiting_room[current_seat]->lock;
                pthread_mutex_unlock(&q_lock);

                // signal the customer to exit a seat
                pthread_mutex_lock(wl);
                pthread_cond_signal(wc);
                pthread_mutex_unlock(wl);

                rest(HAIRCUT_TIME);

                pthread_mutex_lock(wl);
                pthread_cond_signal(wc);
                pthread_mutex_unlock(wl);
            }
            pthread_mutex_unlock(&q_lock);
            ++current_seat;
            if (current_seat >= seats) {
                current_seat = 0;
            }
        }
        printf("Barber is sleeping\n");
        // reset current_seat, the next customer will occupy seat 0
        current_seat = 0;
    }
    pthread_mutex_unlock(&shop_open_lock);
    pthread_exit(0);
}

void *customer(void *param) {
    pthread_mutex_t waiting_lock;
    pthread_cond_t waiting_cond;
    pthread_cond_init(&waiting_cond, NULL);
    pthread_mutex_init(&waiting_lock, NULL);

    // grab a seat in the waiting room
    int32_t seat = check_waiting_room();
    if (seat == -1) {
        printf("Customer left, no available seats\n");
        pthread_mutex_destroy(&waiting_lock);
        pthread_cond_destroy(&waiting_cond);
        pthread_exit(NULL);
    }

    // occupy the seat
    pthread_mutex_lock(&q_lock);
    waiting_room[seat]->cond = &waiting_cond;
    waiting_room[seat]->lock = &waiting_lock;
    waiting_room[seat]->occupied = true;
    pthread_mutex_unlock(&q_lock);

    // signal the barber to wake up & wait to be pulled
    // (the wakeup signal does nothing if the barber is not asleep)
    pthread_mutex_lock(&waiting_lock);
    pthread_mutex_lock(&bell_lock);
    printf("Customer occupied seat %" PRIi32 "\n", seat);
    //summary();
    pthread_cond_signal(&bell);
    pthread_mutex_unlock(&bell_lock);
    pthread_cond_wait(&waiting_cond, &waiting_lock);
    pthread_mutex_unlock(&waiting_lock);

    // pulled to the barber's seat, free up this one
    pthread_mutex_lock(&q_lock);
    //summary();
    printf("Customer pulled from seat %" PRIi32 "\n", seat);
    waiting_room[seat]->occupied = false;
    pthread_mutex_unlock(&q_lock);

    // wait for the haircut to finish
    pthread_mutex_lock(&waiting_lock);
    pthread_cond_wait(&waiting_cond, &waiting_lock);
    pthread_mutex_unlock(&waiting_lock);
    
    // exit the shop
    pthread_mutex_destroy(&waiting_lock);
    pthread_cond_destroy(&waiting_cond);
    pthread_exit(NULL);
}


int32_t check_waiting_room() {
    pthread_mutex_lock(&q_lock);
    for (size_t i = 0; i < seats; i++) {
        if (!waiting_room[i]->occupied) {
            pthread_mutex_unlock(&q_lock);
            return (int32_t) i;
        }
    }
    pthread_mutex_unlock(&q_lock);
    return -1;
}

bool customers_waiting() {
    bool waiting = false;
    pthread_mutex_lock(&q_lock);
    for (size_t i = 0; i < seats; i++) {
        waiting |= waiting_room[i]->occupied;
    }
    pthread_mutex_unlock(&q_lock);
    return waiting;
}

void rest(uint32_t us) {
    struct timespec req;

    req.tv_sec = 0;
    req.tv_nsec = 1000 * us;
    nanosleep(&req, NULL);
}

void summary() {
    pthread_mutex_lock(&q_lock);
    printf("\t");
    for (size_t i = 0; i < seats; i++) {
        printf("[%c]", waiting_room[i]->occupied ? '*' : ' ');
    }
    printf("\n\n");
    pthread_mutex_unlock(&q_lock);
}