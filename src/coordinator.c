#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    //Read parameters
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
    // Create coaches
    //int fd[coaches][2];
    for(i = 0;i < coaches;i++) {
        // Read sorting method
        if (!strcmp(argv[2*i + 3],"-h") || !strcmp(argv[2*i + 3],"-q")) {
            /*// Create pipe for each coach
            if (pipe(fd[i]) < 0) {
                perror("Pipe creation error");
                exit(1);
            }*/
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
                execl("./coach","coach",coachId,argv[2],argv[2*i + 3],argv[2*i + 4],NULL);
                perror("Exec failed");
                exit(1);
            }
            // Parent
            else {
                printf("Created coach %d with pid %d.\n",i,pid);
            }
        }
        else {
            fprintf(stderr,"Usage: ./mysort -f inputfile -h|q columnid [-h|q columnid]\n");
            exit(1);
        }
    }
    // Wait for coaches to finish execution
    for(i = 0;i < coaches;i++) {
        int exit_status;
        pid_t exited_pid;
        if ((exited_pid = wait(&exit_status)) == -1) {
            perror("Wait failed");
            exit(1);
        }
        printf("Child with pid %d just finished exexution with status %d\n",exited_pid,exit_status);
    }
    return EXIT_SUCCESS;
}
