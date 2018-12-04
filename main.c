#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define BUFFER_SIZE 32
#define READ_END     0
#define WRITE_END    1

int main() {
    // open OUTPUT file
    FILE* fp;
    fp = fopen("output.txt", "w");
    
    char write_msg[BUFFER_SIZE] = "Hello child.";
    char read_msg[BUFFER_SIZE];
    
    pid_t pid;  // child process id
    int fd[5][2];  // file descriptors for the pipe
    int i, totalRandomTime = 0;
    int startTime, finishTime, finishTimeMsec;
    int result, nread; // used for 5th child
    
    fd_set inputs, inputfds; // sets of fildes??
    struct timeval timeout;
    
    FD_ZERO(&inputs); // init inputs to empty set
    FD_SET(0, &inputs); // sets fildes 0 (stdin)
    
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

            gettimeofday(&tv, NULL);
            int timeSec = (int) tv.tv_sec;

            // Close the unused WRITE end of the pipe.
            close(fd[i][WRITE_END]);
            
            for (int j = 0; j < 5; j++) {
                inputfds = inputs;
                timeout.tv_sec = 2;
                timeout.tv_usec = 500000;

                // get select() results
                result = select(FD_SETSIZE, &inputfds, (fd_set*) 0, (fd_set*) 0, &timeout);

                if (FD_ISSET(0, &inputfds)) {
                    ioctl(0, FIONREAD, &nread); //????
                }
                nread = read(fd[j][READ_END], read_msg, strlen(read_msg)+1);
                read_msg[nread] = 0;
                
                gettimeofday(&tv, NULL);
                int currTimeSec = (int) tv.tv_sec;
                int currTimeMsec = (int) ((tv.tv_usec) / 1000);

                printf("0:%2d.%d: Parent: Read '%s' from the pipe. PID: %d\n", currTimeSec - timeSec, currTimeMsec, read_msg, getppid());
                // write directly to OUTPUT file
                fprintf(fp, "0:%2d.%d: Parent: Read '%s' from the pipe. PID: %d\n", currTimeSec - timeSec, currTimeMsec, read_msg, getppid());
                
                if (currTimeSec - timeSec > 30) break;
            }
            // Close the READ end of the pipe.
            close(fd[i][READ_END]);
            exit(0);
        }
        else if (pid == 0) {
            // CHILD PROCESS.
            
            gettimeofday(&tv, NULL);
            startTime = (int) tv.tv_sec;
            
            int count = 1;
            
            if (i < 4) {
                for (;;) {
                    // random 0 1 2s
                    srand(time(NULL));
                    int randomNum = rand() % 3;

                    // Close the unused READ end of the pipe.
                    close(fd[i][READ_END]);

                    // sleep 0, 1, 2s
                    sleep(randomNum+1);
                    
                    // Write from the WRITE end of the pipe.
                    write(fd[i][WRITE_END], write_msg, strlen(write_msg)+1);
                    
                    gettimeofday(&tv, NULL);
                    finishTime = (int) tv.tv_sec;
                    finishTimeMsec = (int) ((tv.tv_usec) / 1000);

                    printf("0:%2d.%d: Child %d: message %d\n", finishTime - startTime, finishTimeMsec, i + 1, count);
                    count++;

                    if (finishTime - startTime > 30) break;
                }
                // Close the WRITE end of the pipe.
                close(fd[i][WRITE_END]);
            } else { // 5th child
                for (;;) {
                    // Close the unused READ end of the pipe.
                    close(fd[i][READ_END]);

                    gettimeofday(&tv, NULL);
                    int startMessageTime = (int) tv.tv_sec;

                    // prompt INPUT for the 5th child
                    printf("Input Message: ");
                    scanf("%s", write_msg);
                    
                    gettimeofday(&tv, NULL);
                    int MessageTime = (int) tv.tv_sec;
                    int MessageTimeMsec = (int) ((tv.tv_usec) / 1000);
                    printf("0:%2d.%d: Child %d: %s\n", MessageTime - startMessageTime, MessageTimeMsec, i + 1, write_msg);
                    // Write from the WRITE end of the pipe.
                    write(fd[i][WRITE_END], write_msg, strlen(write_msg)+1);
                    
                    if (MessageTime - startMessageTime > 30) break;
                }
                // Close the WRITE end of the pipe.
                close(fd[i][WRITE_END]);
                exit(0);
            }
        } else {
            fprintf(stderr, "fork() failed");
            return 1;
        }
    }

    fclose(fp);
    return 0;
}
