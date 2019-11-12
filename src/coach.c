#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "../headers/record.h"

#define CEIL(a,b) (((a)+(b)-1)/(b))

#define READ 0
#define WRITE 1

int sorterSignals = 0;

void signal_handler(int signum) {
    if (signum == SIGUSR2)
        sorterSignals++;
}

// Calculates num ^ exp
int power(int num,int exp) {
    // Trivial cases
    if (exp == 0)
        return 1;
    if (exp == 1)
        return num;
    int res = power(num,exp/2);
    return exp % 2 == 0 ? res * res : num * res * res;
}

int main(int argc, char const *argv[])
{
    // Usage: ./coach <coach_id> <filename> <num_records> [-h|q columnid]
    if (argc != 6) {
        fprintf(stderr,"Coach Usage:./coach <coach_id> <filename> <num_records> -h|q columnid\n");
        exit(1);
    }
    // Register SIGUSR2 handler
    signal(SIGUSR2,signal_handler);
    // Get coach id
    int id = atoi(argv[1]);
    // Get amount of records
    int records = atoi(argv[3]);
    // Calculate amount of sorters
    int sorters = power(2,id);
    // Create sorters
    int i;
    // Pipes for each sorter
    int start = 1,div = power(2,id <= 1 ? id : id + 1),end = CEIL(records,div),curSorter = 1,fence = id <= 2 ? sorters : sorters/2;
    char fifo[sorters][20];
    int numRecords[sorters];
    for (i = 0; i < sorters; i++) {
        // Save records of each sorter
        numRecords[i] = end - start + 1;
        // Create named pipe for each sorter
        sprintf(fifo[i],"sorter_%d_%d",id,i);
        if (mkfifo(fifo[i],0666) < 0) {
            perror("Fifo creation error");
            exit(1);
        }
        // Create sorter child process
        pid_t pid;
        // Fork error
        if ((pid = fork()) == -1) {
            perror("Fork failed");
            exit(1);
        }
        // Child and writer
        else if (pid == 0) {
            char firstRecord[8],lastRecord[8];
            sprintf(firstRecord,"%d",start);
            sprintf(lastRecord,"%d",end);
            if (!strcmp(argv[4],"-q")) {
                printf("\tCoach [%d] with id %d sorting %s's column %s from record %s to %s using quicksort.\n",getpid(),id,argv[2],argv[5],firstRecord,lastRecord);
                execl("./sorter_quicksort","sorter_quicksort",argv[2],firstRecord,lastRecord,argv[4],fifo[i],NULL);
                perror("Exec failed");
                exit(1);
            }
            else if (!strcmp(argv[4],"-h")) {
                printf("\tCoach [%d] with id %d sorting %s's column %s from record %s to %s using heapsort.\n",getpid(),id,argv[2],argv[5],firstRecord,lastRecord);
                execl("./sorter_heapsort","sorter_heapsort",argv[2],firstRecord,lastRecord,argv[4],fifo[i],NULL);
                perror("Exec failed");
                exit(1);
            } else {
                fprintf(stderr,"Coach Usage:./coach <coach_id> <filename> <num_records> -h|q columnid\n");
                exit(1);
            }
        }
        // Parent and reaader
        else {
            if (i < sorters - 1) {
                // Calculate new record dividers for next sorter
                if (curSorter == fence) {
                    div /= 2;
                    if (id <= 2)
                        fence /= 2;
                    curSorter = 1;
                }
                else
                    curSorter++;
                start = end + 1;
                end = start + records/div - 1;
            }
        }
    }
    // Print records sorted from all sorters
    char buf[Record_Size()];
    Record rec;
    int j,fd;
    for(i = 0;i < sorters;i++) {
        fd = open(fifo[i],O_RDONLY);
        for(j = 0;j < numRecords[i];j++) {
            read(fd,buf,Record_Size());
            Record_Init(&rec,buf);
            //Record_Print(rec);
            Record_Destroy(&rec);
        }
        close(fd);
        unlink(fifo[i]);
    }
    // Wait for sorters to finish their jobs
    for(i = 0;i < sorters;i++) {
        int exit_status;
        pid_t exited_pid;
        if ((exited_pid = wait(&exit_status)) == -1) {
            perror("Wait failed");
            exit(1);
        }
        printf("\tSorter child with pid %d just finished execution with status %d\n",exited_pid,exit_status);
    }
    printf("\t#Total SIGUSR2 signals at coach with pid [%d]:%d\n",getpid(),sorterSignals);
    return EXIT_SUCCESS;
}
