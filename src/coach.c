#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../headers/record.h"

#define CEIL(a,b) (((a)+(b)-1)/(b))

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
    // Usage: ./coach <coach_id> <filename> [-h|q columnid]
    if (argc != 5) {
        fprintf(stderr,"Coach Usage:./coach <coach_id> <filename> [-h|q columnid]\n");
        exit(1);
    }
    // Get coach id
    int id = atoi(argv[1]);
    // Open file to get amount of records
    FILE *inputfile;
    if ((inputfile = fopen(argv[2],"r")) == NULL) {
        fprintf(stderr,"File %s error\n",argv[2]);
        exit(1);
    }
    // Calculate amount of records
    fseek(inputfile,0L,SEEK_END);
    long int bytes = ftell(inputfile);
    int records = bytes/Record_Size();
    // Close input file
    fclose(inputfile);
    // Calculate amount of sorters
    int sorters = power(2,id);
    // Create sorters
    int i;
    // Pipes for each sorter
    //int fd[sorters][2];
    int start = 1,div = power(2,id <= 1 ? id : id + 1),end = CEIL(records,div),curSorter = 1,fence = id <= 2 ? sorters : sorters/2;
    for (i = 0; i < sorters; i++) {
        /*// Create pipes for each sorter
        if (pipe(fd[i]) < 0) {
            perror("Pipe creation error");
            exit(1);
        }*/
        // Create sorter child process
        pid_t pid;
        // Fork error
        if ((pid = fork()) == -1) {
            perror("Fork failed");
            exit(1);
        }
        // Child
        else if (pid == 0) {
            char firstRecord[8],lastRecord[8];
            sprintf(firstRecord,"%d",start);
            sprintf(lastRecord,"%d",end);
            if (!strcmp(argv[3],"-q")) {
                printf("\tCoach [%d] with id %d sorting %s's column %s from record %s to %s using quicksort.\n",getpid(),id,argv[2],argv[4],firstRecord,lastRecord);
                execl("./sorter_quicksort","sorter_quicksort",argv[2],firstRecord,lastRecord,argv[4],NULL);
                perror("Exec failed");
                exit(1);
            }
            else if (!strcmp(argv[3],"-h")) {
                printf("\tCoach [%d] with id %d sorting %s's column %s from record %s to %s using heapsort.\n",getpid(),id,argv[2],argv[4],firstRecord,lastRecord);
                execl("./sorter_heapsort","sorter_heapsort",argv[2],firstRecord,lastRecord,argv[4],NULL);
                perror("Exec failed");
                exit(1);
            } else {
                fprintf(stderr,"Coach Usage:./coach <coach_id> <filename> [-h|q columnid]\n");
                exit(1);
            }
        }
        // Parent
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
    return EXIT_SUCCESS;
}
