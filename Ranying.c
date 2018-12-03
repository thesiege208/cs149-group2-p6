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
    char buffer[BUFFER_SIZE]; // used for fifth child process
    
    pid_t pid, waitPid;  // child process id
    int fd[5][2];  // file descriptors for the pipe
    int i, status = 0;
    int startTime = 0, finishTime = 0, startTimeMsec = 0, finishTimeMsec = 0;
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    startTime = (int)tv.tv_sec;
    startTimeMsec = (int)((tv.tv_usec) / 1000);
    finishTime = startTime;
    
    
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
                
                randomNum += randomNum;
                printf("Time: %ld - Child %d: Wrote '%s' to the pipe. PID: %d\n", tv.tv_sec, i + 1, write_msg, getpid());
                printf("0:%2d.%d: Child %d: message 1\n", finishTime - startTime, startTimeMsec, i + 1);
                
                sleep(randomNum);
                
                gettimeofday(&tv, NULL);
                finishTime = (int)tv.tv_sec;
                finishTimeMsec = (int)((tv.tv_usec) / 1000);
                
                //printf("  finish time: %f. start time: %f.\n", finishTime, startTime);
                printf("0:%2d.%d: Child %d: message 2\n", finishTime - startTime, finishTimeMsec, i + 1);

            }
            else {
                // Close the unused READ end of the pipe.
                close(fd[i][READ_END]);
                
                printf("child5, Please input: \n");
                scanf("%s", buffer);
                //printf("\n");
                
                printf("0:%2d.%d: Child %d: message 1\n", finishTime, startTimeMsec, i + 1);
                gettimeofday(&tv, NULL);
                finishTime = (int)tv.tv_sec;
                
                printf("0:%2d.%d: Child %d: message 2\n", finishTime - startTime, finishTimeMsec, i + 1);
                
                // Write from the WRITE end of the pipe.
                write(fd[i][WRITE_END], buffer, BUFFER_SIZE);
            }
    
            // Close the WRITE end of the pipe.
            close(fd[i][WRITE_END]);
            sleep(3);
            exit(status);
            
        }
        else {
            fprintf(stderr, "fork() failed");
            return 1;
        }
    }
    
    return 0;
}