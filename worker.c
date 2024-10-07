#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <sys/msg.h>
#include <signal.h>

// Structure for the Clock
struct Clock {
    int seconds;
    int nanoseconds;
};

// Message structure for communication
struct msg_buffer {
    long msg_type; 
    int msg_data;  
};

int main(int argc, char *argv[]) {
    // Check command line arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <seconds> <nanoseconds>\n", argv[0]);
        exit(1);
    }

    int run_seconds = atoi(argv[1]);
    int run_nanoseconds = atoi(argv[2]);
    int total_nanoseconds = run_seconds * 1000000000 + run_nanoseconds;

    // Shared memory variables
    int shmid;
    struct Clock *shm_clock;

    // Get the shared memory ID
    shmid = shmget(IPC_PRIVATE, sizeof(struct Clock), 0777);
    if (shmid < 0) {
        perror("Get shared memory ID error");
        exit(1);
    }

    // Attach shared memory
    shm_clock = (struct Clock *)shmat(shmid, NULL, 0);
    if (shm_clock == (struct Clock *)(-1)) {
        perror("Attach shared memory error");
        exit(1);
    }

    // Create a message queue
    int msgid = msgget(IPC_PRIVATE, 0777 | IPC_CREAT);
    if (msgid < 0) {
        perror("Message error");
        exit(1);
    }

    // Worker process logic
    printf("WORKER PID:%d PPID:%d SysClockS: %d SysClockNano: %d -- Just Starting\n", 
           getpid(), getppid(), shm_clock->seconds, shm_clock->nanoseconds);

    while (1) {
        // Read the current clock value
        int current_time = shm_clock->seconds * 1000000000 + shm_clock->nanoseconds;

        // Check if the current time has reached the total time to run
        if (current_time >= total_nanoseconds) {
            struct msg_buffer msg;
            msg.msg_type = 1; 
            msg.msg_data = 0;

            msgsnd(msgid, &msg, sizeof(msg.msg_data), 0);
            printf("WORKER PID:%d -- Terminating after reaching specified time.\n", getpid());
            break; // Exit the loop and terminate the worker
        }

        // Simulate doing work and send a message back to oss
        struct msg_buffer msg;
        msg.msg_type = 1; 
        msg.msg_data = 1; 
        msgsnd(msgid, &msg, sizeof(msg.msg_data), 0);

        // Sleep for delay
        usleep(100000); 
    }

    // Cleanupy
    shmdt(shm_clock);
    return 0;
}

