#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

// Simulated system clock
struct Clock {
    int seconds;
    int nanoseconds;
};

int main(int argc, char *argv[]) {
    int n = 0; // Number of processes
    int s = 0; // Simulation time
    int t = 0; // Time limit for children
    int i = 0; // Interval in ms to launch children
    char *logfile = NULL; // Log file name

    // Command-line option parsing
    int opt;
    while ((opt = getopt(argc, argv, "hn:s:t:i:f:")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: %s [-h] [-n proc] [-s simul] [-t timelimitForChildren] [-i intervalInMsToLaunchChildren] [-f logfile]\n", argv[0]);
                exit(0);
            case 'n':
                n = atoi(optarg);
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                t = atoi(optarg);
                break;
            case 'i':
                i = atoi(optarg);
                break;
            case 'f':
                logfile = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-h] [-n proc] [-s simul] [-t timelimitForChildren] [-i intervalInMsToLaunchChildren] [-f logfile]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Print the parsed option
    printf("Number of processes (n): %d\n", n);
    printf("Simulation time (s): %d\n", s);
    printf("Time limit for children (t): %d\n", t);
    printf("Interval to launch children (i): %d\n", i);
    printf("Log file: %s\n", logfile ? logfile : "None");

    // Shared memory
    int shmid;
    struct Clock *shm_clock;

    // Create shared memory
    shmid = shmget(IPC_PRIVATE, sizeof(struct Clock), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("Create shared memory error");
        exit(1);
    }

    // Attach shared memory
    shm_clock = (struct Clock *)shmat(shmid, NULL, 0);
    if (shm_clock == (struct Clock *)(-1)) {
        perror("Attach shared memory error");
        exit(1);
    }

    // Initialize the clock
    shm_clock->seconds = 0;
    shm_clock->nanoseconds = 0;

    printf("Seconds: %d\n, nanoseconds: %d\n", shm_clock->seconds, shm_clock->nanoseconds);

    return 0;
}

