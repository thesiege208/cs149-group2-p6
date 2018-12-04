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
    // open OUTPUT file
    FILE* fp;
    fp = fopen("output.txt", "w");
    
    char write_msg[BUFFER_SIZE] = "Hello child.";
    char read_msg[BUFFER_SIZE];
    char Message[BUFFER_SIZE]; // used for fifth child process
    
    pid_t pid, waitPid;  // child process id
    int fd[5][2];  // file descriptors for the pipe
    int i, status = 0, totalRandomTime = 0;
    int startTime, finishTime, startTimeMsec, finishTimeMsec;
    
    // get the time
    struct timeval tv;

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
            
            // Close the unused WRITE end of the pipe.
            close(fd[i][WRITE_END]);
            
            // Read to the READ end of the pipe.
            read(fd[i][READ_END], read_msg, strlen(read_msg)+1);
            
            gettimeofday(&tv, NULL);
            int currTime = (int) tv.tv_sec;
            int currTimeMSec = (int) ((tv.tv_usec) / 1000);
            
            printf("%d Parent: Read '%s' from the pipe. PID: %d\n", currTimeMSec, read_msg, getppid());
            // write directly to OUTPUT file
            fprintf(fp, "%d Parent: Read '%s' from the pipe. PID: %d\n", currTimeMSec, read_msg, getppid());
            
            // Close the READ end of the pipe.
            close(fd[i][READ_END]);
        }
        else if (pid == 0) {
            // CHILD PROCESS.
            
            gettimeofday(&tv, NULL);
            startTime = (int) tv.tv_sec;
            startTimeMsec = (int) ((tv.tv_usec) / 1000);
            finishTime = (int) tv.tv_sec;
            startTimeMsec = (int) ((tv.tv_usec) / 1000);
            
            if (i < 4){
                for (;;) {
                    // random 0 1 2s
                    srand(time(NULL));
                    int randomNum = rand() % 3;

                    // Close the unused READ end of the pipe.
                    close(fd[i][READ_END]);

                    // Write from the WRITE end of the pipe.
                    write(fd[i][WRITE_END], write_msg, strlen(write_msg)+1);

                    // sleep 0, 1, 2s
                    sleep(randomNum);
                    
                    gettimeofday(&tv, NULL);
                    finishTime = (int) tv.tv_sec;
                    finishTimeMsec = (int) ((tv.tv_usec) / 1000);
                    
                    if (finishTime - startTime > 30) break;
                    
                    //printf("0:%2d.%d: Child %d: message 2\n", finishTime - startTime, finishTimeMsec, i + 1);
                }   
            }
            else { // 5th child
                for (;;) {
                    // Close the unused READ end of the pipe.
                    close(fd[i][READ_END]);
                    
                    gettimeofday(&tv, NULL);
                    int startMessageTime = (int)tv.tv_sec;
                    int startMessageTimeMsec = (int)((tv.tv_usec) / 1000);

                    // INPUT for the 5th child
                    printf("Input Message: \n");
                    scanf("%s", Message);
                    
                    gettimeofday(&tv, NULL);
                    int MessageTime = (int) tv.tv_sec;
                    int MessageTimeMsec = (int) ((tv.tv_usec) / 1000);
                    
                    printf("0:%2d.%d: Child %d: %s\n", startMessageTime - startTime, startMessageTimeMsec, i + 1, Message);
                    printf("0:%2d.%d: Child %d: %s\n", MessageTime - startTime, MessageTimeMsec, i + 1, Message);
                    
                    // Write from the WRITE end of the pipe.
                    write(fd[i][WRITE_END], Message, strlen(Message)+1);
                    
                    if (MessageTime - startTime > 30) break;
                }
            }
            
            // Close the WRITE end of the pipe.
            close(fd[i][WRITE_END]);
            exit(status);
            
        }
        else {
            fprintf(stderr, "fork() failed");
            return 1;
        }
    }
    fclose(fp);
    
    return 0;
}
