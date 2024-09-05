/*
    Lab 08: Paging Simulation
    COMP 173: Operating Systems (Fall 2020)
    Michael Kmak

    compilation: gcc paging_sim.c -o paging_sim
    run: ./paging_sim


        This program attempts to simulate the paging process
    in a 512KB segment of memory. The memory is split into
    1024 512-byte frames. The user can load memory from a
    file, load and unload "executables" to be paged or removed,
    and dump the memory contents to stdout. When closed, the
    program will write the current memory state to a the file
    "memory.txt".

    Memory files:
        The memory file begins by stating how many page tables
    it contains. The program then reads through these page
    tables and uses them to reconstruct the described memory
    state. I felt that the additional cost of recreating the
    memory rather than reading a full dump was balanced by the
    greatly reduced size of the memory file. Depending on the
    size of each process, this reduces the filesize by some
    amount greater than half.
*/
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#define PROCESSES 512 // should be frames/2
#define FRAMES 1024

typedef struct {
    size_t len;
    char ftype; // T for page table, P for a page
    size_t pid;
    uint16_t data[31];
} s_frame;

typedef struct {
    size_t frame_count; // how many free frames are left
    size_t free_frames[FRAMES + 1]; // the actual frames
    size_t process_count;
    size_t process_list[PROCESSES];
    s_frame data[FRAMES];
} s_memory;


void init_mem(s_memory *mem);

void dump(s_memory mem);
bool add_exe(s_memory *mem, size_t len);
uint16_t nextpid(s_memory *mem);
size_t place(s_memory *mem, s_frame *f);
void remove_exe(s_memory *mem, size_t pid);
void pop(s_memory *mem, size_t frame_pos);
void write_to_file(s_memory mem, const char *filename);
void write_frame(FILE *fp, s_frame frame, size_t pos);
void read_from_file(s_memory *mem, const char *filename);
size_t scan_frame(FILE *fp, s_frame *frame);


int main() {
    s_memory mem;
    init_mem(&mem);
    char inbuf[8];
    while(true) {
        // menu
        printf("\n\n===MENU===\n");
        printf("  0 : create or open memory file\n");
        printf("  1 : load an executable\n");
        printf("  2 : remove an executable\n");
        printf("  3 : view an executable's page table\n");
        printf("  4 : dump the memory to stdout\n");
        printf("  5 : quit the program\n");
        fgets(inbuf, 8, stdin);
        printf("\n");

        // process user selection
        switch (inbuf[0]) {
            // create/open memory file
            case '0':
                printf("Enter filename:\n");
                char filename[FILENAME_MAX];
                fgets(filename, FILENAME_MAX, stdin);
                sscanf(filename, "%s", filename);
                read_from_file(&mem, filename);
                break;

            // load exe
            case '1':
                printf("How many pages?\n");
                fgets(inbuf, 8, stdin);
                size_t pages = (size_t) atoi(inbuf);
                if (pages != 0 && pages < 31) {
                    if (add_exe(&mem, pages)) {
                        printf("Sucessfully loaded process with ID %zu", mem.process_count - 1);
                    } else {
                        printf("Could not load process\n");
                    }
                } else {
                    printf("Invalid input\n");
                }
                break;

            // remove exe
            case '2':
                printf("Remove which process?\n");
                fgets(inbuf, 8, stdin);
                size_t pid = (size_t) atoi(inbuf);
                if (pid < 0 || pid > mem.process_count) {
                    printf("Invalid pid\n");
                    break;
                }
                remove_exe(&mem, pid);
                printf("Removed process with ID %zu", pid);
                break;

            // view exe details
            case '3':
                if (mem.process_count == 0) {
                    printf("There are no processes to see\n");
                    break;
                }
                printf("Current Processes:\n");
                for (size_t p = 0; p < PROCESSES; p++) {
                    if (mem.process_list[p] != 0) {
                        printf("\tProcess %zu: %zu pages\n", p, mem.data[mem.process_list[p]].len);
                    }
                }
                printf("Select a process to view details\n");
                fgets(inbuf, 8, stdin);
                size_t proc = (size_t) atoi(inbuf);
                if (proc < 0 || proc > mem.process_count) {
                    printf("Invalid pid\n");
                    break;
                }
                s_frame *ptable = &(mem.data[mem.process_list[proc]]);
                if (ptable->len == 0) {
                    printf("There are no frames to see here\n");
                }
                for (size_t f = 0; f < ptable->len; f++) {
                    printf("Page %zu: Frame %"PRIu16"\n", f, ptable->data[f]);
                }
                break;

            // dump to stdout
            case '4':
                dump(mem);
                break;

            // quit program
            case '5':
                write_to_file(mem, "memory.txt");
                return 0;
            
            // bad input
            default:
                printf("Invalid input!\n");
                break;
        }
    }
    return 0;
}


/*
    Load an executable into memory
*/
bool add_exe(s_memory *mem, size_t len) {
    if (mem->frame_count < len + 1) {
        return false;
    }
    if (mem->process_count >= PROCESSES) {
        return false;
    }
    s_frame table;
    table.pid = nextpid(mem);
    table.len = len;
    table.ftype = 'T';
    size_t table_pos = place(mem, &table);
    for (uint16_t p = 0; p < table.len; p++) {
        s_frame entry;
        entry.len = 1;
        entry.ftype = 'P';
        entry.pid = table.pid;
        entry.data[0] = p;
        table.data[p] = place(mem, &entry);
    }
    mem->data[table_pos] = table;
    mem->process_list[table.pid] = table_pos;
    mem->process_count++;
}


/*
    Stores a frame in the nearest free location, returns that location
*/
size_t place(s_memory *mem, s_frame *f) {
    size_t loc = mem->free_frames[mem->frame_count];
    mem->data[loc] = *f;
    mem->frame_count--;
    return loc;
}


/*
    Returns the next available pid
*/
uint16_t nextpid(s_memory *mem) {
    for (size_t pid = 0; pid < PROCESSES; pid++) {
        if (mem->process_list[pid] == 0) {
            return pid;
        }
    }
}


/*
    Unload a process from memory
*/
void remove_exe(s_memory *mem, size_t pid) {
    s_frame *table = &(mem->data[mem->process_list[pid]]);
    for (size_t f = 0; f < table->len; f++) {
        pop(mem, table->data[table->len - f - 1]);
    }
    pop(mem, mem->process_list[pid]);
    mem->process_list[pid] = 0;
    mem->process_count--;
}


/*
    pop the frame at frame_pos from memory
*/
void pop(s_memory *mem, size_t frame_pos) {
    mem->frame_count++;
    mem->data[frame_pos].len = 0;
    mem->free_frames[mem->frame_count] = frame_pos;
}


/*
    Zeroes the important data in a memory
*/
void init_mem(s_memory *mem) {
    mem->frame_count = FRAMES;
    mem->process_count = 0;
    for (size_t f = 0; f < FRAMES; f++) {
        mem->free_frames[f + 1] = f;
        mem->data[f].len = 0;
    }
    for (size_t p = 0; p < PROCESSES; p++) {
        mem->process_list[p] = 0;
    }
}


/*
    Dump the memory info to stdout, mostly for debugging
*/
void dump(s_memory mem) {
    for(size_t f = 0; f < FRAMES; f++) {
        s_frame frame = mem.data[f];
        if (frame.len == 0) {
            continue;
        } else if (frame.ftype == 'T') {
            printf("Frame %zu: Page table for File %zu\n", f, frame.pid);
            for(size_t i = 0; i < frame.len; i++) {
                printf("\tPage: %zu\tFrame: %"PRIu16"\n", i, frame.data[i]);
            }
        } else {
            printf("Frame %zu: File %zu, Page %"PRIu16"\n", f, frame.pid, frame.data[0]);
        }
    }
}


/*
    Writes the memory to a file
*/
void write_to_file(s_memory mem, const char *filename) {
    FILE *fp;

    fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "error writing to file \"%s\"\n", filename);
        return;
    }
    fprintf(fp, "TABLES=%zu\n", mem.process_count);

    // write the tables
    size_t pos = 0;
    for(size_t p = 0; p < PROCESSES; p++) {
        pos = mem.process_list[p];
        if (pos != 0) {
            write_frame(fp, mem.data[pos], pos);
        }
    }
}


/*
    Writes a frame from the given file pointer
*/
void write_frame(FILE *fp, s_frame frame, size_t pos) {
    fprintf(fp, "%zu_%zu = %zu", pos, frame.len, frame.pid);
    for (size_t i = 0; i < frame.len; i++) {
        fprintf(fp, " %"SCNu16, frame.data[i]);
    }
    fprintf(fp, "\n");
}

/*
    Reads the memory from a file
*/
void read_from_file(s_memory *mem, const char *filename) {
    FILE *fp;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "error writing to file \"%s\"\n", filename);
        return;
    }
    size_t tables = 0;
    fscanf(fp, "TABLES=%zu", &tables);
    printf("reading %zu tables", tables);
    for(size_t t = 0; t < tables; t++) {
        s_frame table;
        table.ftype = 'T';
        size_t table_pos = scan_frame(fp, &table);
        // add it to memory
        mem->data[table_pos] = table;
        mem->frame_count--;
        // add it to the process tracker
        mem->process_list[table.pid] = table_pos;
        mem->process_count++;
        // create the pages
        for (size_t f = 0; f < table.len; f++) {
            s_frame frame;
            frame.ftype = 'P';
            frame.pid = table.pid;
            frame.len = 1;
            frame.data[0] = f;
            mem->data[table.data[f]] = frame;
        }
    }
}

/*
    Scans a frame from the given file pointer, returns its position
*/
size_t scan_frame(FILE *fp, s_frame *frame) {
    size_t pos;
    fscanf(fp, "%zu_%zu = %zu", &pos, &frame->len, &frame->pid);
    for (size_t i = 0; i < frame->len; i++) {
        fscanf(fp, " %"SCNu16, &frame->data[i]);
    }
    return pos;
}