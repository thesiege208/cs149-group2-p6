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

#define BUFFER_SIZE 128
#define READ_END     0
#define WRITE_END    1

int main() {
    // open OUTPUT file
    FILE* fp;
    fp = fopen("output.txt", "w");
    
    char write_msg[BUFFER_SIZE] = "I'm a chlid.";
    //char write_msg[BUFFER_SIZE];
    char read_msg[BUFFER_SIZE];
    
    pid_t pid;  // child process id
    int fd[5][2];  // file descriptors for the pipe
    int i, totalRandomTime = 0;
    int startTime, finishTime, finishTimeMsec;
    int result, nread; // used for 5th child
    
    fd_set inputs, inputfds; // sets of fildes??
    
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

                // using SELECT
                result = select(5, &inputfds, (fd_set*) 0, NULL, NULL);

                if (FD_ISSET(fd[j][READ_END], &inputfds)) {
                    ioctl(fd[j][READ_END], FIONREAD, &nread);
                }
                nread = read(fd[j][READ_END], read_msg, strlen(write_msg)+1);
                read_msg[nread] = 0;
                
                gettimeofday(&tv, NULL);
                int currTimeSec = (int) tv.tv_sec;
                int currTimeMsec = (int) ((tv.tv_usec) / 1000);

                printf("0:%2d.%d: Parent: Read '%s' from the pipe. PID: %d\n", currTimeSec - timeSec, currTimeMsec, read_msg, getppid());
                // write directly to OUTPUT file
                fprintf(fp, "0:%2d.%d: Parent: Read '%s' from the pipe. PID: %d\n", currTimeSec - timeSec, currTimeMsec, read_msg, getppid());
                
                // if (currTimeSec - timeSec > 30) break;
            }
            // Close the READ end of the pipe.
            close(fd[i][READ_END]);
            break;
        }
        else if (pid == 0) {
            // CHILD PROCESS.
            
            gettimeofday(&tv, NULL);
            startTime = (int) tv.tv_sec;
            
            if (i < 4) {
                // Close the unused READ end of the pipe.
                close(fd[i][READ_END]);

                    for (;;) {                    
                    sprintf(write_msg, "0:%2d.%d: Child %d (PID=%d): I'm a child.", finishTime - startTime, finishTimeMsec, i + 1, getpid());
                    
                    // Write from the WRITE end of the pipe.
                    write(fd[i][WRITE_END], write_msg, strlen(write_msg)+1);
                    
                    gettimeofday(&tv, NULL);
                    finishTime = (int) tv.tv_sec;
                    finishTimeMsec = (int) ((tv.tv_usec) / 1000);

                    printf("0:%2d.%d: Child %d (PID=%d): I'm a child.\n", finishTime - startTime, finishTimeMsec, i + 1, getpid());
                    
                    // random 0 1 2s
                    srand(time(NULL));
                    int randomNum = rand() % 3;

                    // sleep 0, 1, 2s
                    sleep(randomNum);

                    if (finishTime - startTime + randomNum + 1 > 30) break;
                }
                // Close the WRITE end of the pipe.
                close(fd[i][WRITE_END]);
            } else { // 5th child
                // Close the unused READ end of the pipe.
                close(fd[i][READ_END]);
                for (;;) {
                    char buffer[BUFFER_SIZE] = "";
                    gettimeofday(&tv, NULL);
                    int startMessageTime = (int) tv.tv_sec;

                    // prompt INPUT for the 5th child
                    printf("Input Message: ");
                    scanf("%s", buffer);
                    
                    gettimeofday(&tv, NULL);
                    int MessageTime = (int) tv.tv_sec;
                    int MessageTimeMsec = (int) ((tv.tv_usec) / 1000);

                    sprintf(write_msg, "0:%2d.%d: Child %d (PID=%d): %s", MessageTime - startMessageTime, MessageTimeMsec, 5, getpid(), buffer);

                    printf("0:%2d.%d: Child %d (PID=%d): %s\n", MessageTime - startMessageTime, MessageTimeMsec, 5, getpid(), buffer);

                    // Write from the WRITE end of the pipe.
                    write(fd[i][WRITE_END], write_msg, strlen(write_msg)+1);
                    
                    // sleep(1); // to prevent spamming
                    
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
