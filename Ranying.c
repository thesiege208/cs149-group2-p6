#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 32
#define READ_END     0
#define WRITE_END    1

int main() {
    
    char write_msg[BUFFER_SIZE] = "Hello";
    char read_msg[BUFFER_SIZE];
    char Message[BUFFER_SIZE]; // used for fifth child process
    
    pid_t pid, waitPid;  // child process id
    int fd[5][2];  // file descriptors for the pipe
    int i, status = 0, totalRandomTime = 0;
    int startTime, finishTime, startTimeMsec, finishTimeMsec;
    
    // get the time
    struct timeval tv;
    gettimeofday(&tv, NULL);
    startTime = (int)tv.tv_sec;
    startTimeMsec = (int)((tv.tv_usec) / 1000);
    finishTime = (int)tv.tv_sec;
    startTimeMsec = (int)((tv.tv_usec) / 1000);
    
    // Fork five children processes.
    for (i = 0; i < 5; i++) {
        
        // Create the pipe.
        if (pipe(fd[i]) == -1) {
            fprintf(stderr,"pipe() failed");
            return 1;
        }
        
        // Fork a child process.
        pid = fork();
        
        if (pid > 0) {
            // PARENT PROCESS.
            
            // the parent need to wait for each child process to finish
            while ((waitPid = wait(&status)) > 0);
            
            // Close the unused WRITE end of the pipe.
            close(fd[i][WRITE_END]);
            
            // Read to the READ end of the pipe.
            read(fd[i][READ_END], read_msg, strlen(read_msg)+1);
            printf("Parent: Read '%s' from the pipe. PID: %d\n", read_msg, getppid());
            
            // Close the READ end of the pipe.
            close(fd[i][READ_END]);
        }
        else if (pid == 0) {
            // CHILD PROCESS.
            
            if ( i < 4 ){
                // random 0 1 2s
                srand(time(NULL));
                int randomNum = rand() % 3;
                
                // TODO: for test, need to remove
                printf("ramdom = %d\n", randomNum);

                // Close the unused READ end of the pipe.
                close(fd[i][READ_END]);
                
                // Write from the WRITE end of the pipe.
                write(fd[i][WRITE_END], write_msg, BUFFER_SIZE);
                
                // sleep 0, 1, 2s
                sleep(randomNum);
                
                gettimeofday(&tv, NULL);
                finishTime = (int)tv.tv_sec;
                finishTimeMsec = (int)((tv.tv_usec) / 1000);
                
                // TODO: need to remove this output
                printf("Time: %ld - Child %d: Wrote '%s' to the pipe. PID: %d\n", tv.tv_sec, i + 1, write_msg, getpid());
                printf("0:%2d.%d: Child %d: message 1\n", finishTime - startTime - randomNum, startTimeMsec, i + 1);

                
                printf("0:%2d.%d: Child %d: message 2\n", finishTime - startTime, finishTimeMsec, i + 1);

            }
            else {
                // Close the unused READ end of the pipe.
                close(fd[i][READ_END]);
                
                gettimeofday(&tv, NULL);
                int startMessageTime = (int)tv.tv_sec;
                int startMessageTimeMsec = (int)((tv.tv_usec) / 1000);
                
                printf("For the 5th Child, Please input Message 1: \n");
                scanf("%s", Message);
                
                gettimeofday(&tv, NULL);
                int MessageTime = (int)tv.tv_sec;
                int MessageTimeMsec = (int)((tv.tv_usec) / 1000);
                
                printf("0:%2d.%d: Child %d: %s\n", startMessageTime - startTime, startMessageTimeMsec, i + 1, Message);
                
                printf("0:%2d.%d: Child %d: %s\n", MessageTime - startTime, MessageTimeMsec, i + 1, Message);
                
                // Write from the WRITE end of the pipe.
                write(fd[i][WRITE_END], Message, BUFFER_SIZE);
            }
    
            // Close the WRITE end of the pipe.
            close(fd[i][WRITE_END]);
            sleep(30);
            exit(status);
            
        }
        else {
            fprintf(stderr, "fork() failed");
            return 1;
        }
    }
    
    return 0;
}
