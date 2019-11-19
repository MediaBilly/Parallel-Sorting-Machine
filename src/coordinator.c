#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <limits.h>
#include "../headers/record.h"

char columns[8];
char coachOk[4];

int main(int argc, char const *argv[])
{
    double t1, t2;
    struct tms tb1, tb2;
    double ticspersec;
    ticspersec = (double)sysconf(_SC_CLK_TCK);
    t1 = (double)times(&tb1);
    // Read parameters
    // Check for correct amount of arguments
    if (argc < 5) {
        fprintf(stderr,"Not enough arguments\n");
        exit(1);
    }
    if (argc > 11) {
        fprintf(stderr,"Too many arguments.You must create up to 4 coaches.\n");
        exit(1);
    }
    // Input file
    if (strcmp(argv[1],"-f")) {
        fprintf(stderr,"Usage: ./mysort -f inputfile -h|q columnid [-h|q columnid]\n");
        exit(1);
    }
    int i;
    // Check if each sorting method is followed by a column number
    if ((argc - 3) % 2 != 0) {
        fprintf(stderr,"Usage: ./mysort -f inputfile -h|q columnid [-h|q columnid]\n");
        exit(1);
    }
    // Calculate coaches
    int coaches = (argc - 3)/2;
    // Open file to get amount of records
    FILE *inputfile;
    if ((inputfile = fopen(argv[2],"r")) == NULL) {
        fprintf(stderr,"File %s error\n",argv[2]);
        exit(1);
    }
    // Calculate amount of records
    fseek(inputfile,0L,SEEK_END);
    long int bytes = ftell(inputfile);
    char records[10];
    sprintf(records,"%ld",bytes/Record_Size());
    // Close input file
    fclose(inputfile);
    // Create coaches
    char fifo[coaches][8];
    int column,failedCoaches = 0;
    for(i = 0;i < coaches;i++) {
        // Read sorting method
        if (!strcmp(argv[2*i + 3],"-h") || !strcmp(argv[2*i + 3],"-q")) {
            // Check if column was already selected.If so, ignore it
            column = atoi(argv[2*i + 4]);
            if (columns[column - 1]) {
                printf("Column %d already selected.Ignoring it!\n",column);
                failedCoaches++;
                continue;
            }
            // Check if column is in range.If not, ignore it
            if (column <= 0 || column > 8) {
                printf("Column %d out of range.Ignoring it!\n",column);
                failedCoaches++;
                continue;
            }
            columns[column - 1] = coachOk[i] = 1;
            // Create named pipe for each coach
            sprintf(fifo[i],"coach%d",i);
            if (mkfifo(fifo[i],0666) < 0) {
                perror("Fifo creation error");
                exit(1);
            }
            // Create coach child process
            pid_t pid;
            // Fork error
            if ((pid = fork()) == -1) {
                perror("Fork failed");
                exit(1);
            }
            // Child
            else if (pid == 0) {
                char coachId[2];
                sprintf(coachId,"%d",i);
                execl("./coach","coach",coachId,argv[2],records,argv[2*i + 3],argv[2*i + 4],fifo[i],NULL);
                perror("Exec failed");
                exit(1);
            }
        }
        else {
            fprintf(stderr,"Usage: ./mysort -f inputfile -h|q columnid [-h|q columnid]\n");
            exit(1);
        }
    }
    int fd;
    double timing,min_time = ULLONG_MAX,max_time = 0,avg_time = 0;
    char buf[4*sizeof(double) + sizeof(int)];
    int signals[coaches];
    memset(signals,0,sizeof(signals));
    // Print timing for each coach
    for (i = 0; i < coaches; i++) {
        if (!coachOk[i])
            continue;
        printf("Coach %d sorter times:\n",i);
        fd = open(fifo[i],O_RDONLY);
        read(fd,buf,4*sizeof(double) + sizeof(int));
        memcpy(&timing,buf,sizeof(double));
        printf("\tMin Sorter Time:%.2lf\n",timing);
        memcpy(&timing,buf + sizeof(double),sizeof(double));
        printf("\tMax Sorter Time:%.2lf\n",timing);
        memcpy(&timing,buf + 2*sizeof(double),sizeof(double));
        printf("\tAverage Sorter Time:%.2lf\n",timing);
        memcpy(&timing,buf + 3*sizeof(double),sizeof(double));
        avg_time += timing;
        if (timing > max_time)
            max_time = timing;
        if (timing < min_time)
            min_time = timing;
        memcpy(signals + i,buf + 4*sizeof(double),sizeof(int));
        close(fd);
    }
    avg_time /= (coaches - failedCoaches);
    // Wait for coaches to finish execution
    for(i = 0;i < coaches;i++) {
        if (!coachOk[i])
            continue;
        int exit_status;
        pid_t exited_pid;
        if ((exited_pid = wait(&exit_status)) == -1) {
            perror("Wait failed");
            exit(1);
        }
        unlink(fifo[i]);
    }
    printf("\nCoach times:\n\tMin time:%.2lf\n\tMax time:%.2lf\n\tAverage time:%.2lf\n",min_time,max_time,avg_time);
    t2 = (double)times(&tb2);
    double turnaround_time = (t2 - t1)/ticspersec;
    printf("\nTurnaround Time:%.2lf\n\n",turnaround_time);
    for (i = 0; i < coaches; i++)
        printf("Coach %d SIGUSR2 signals:%d\n",i,signals[i]);
    return EXIT_SUCCESS;
}
