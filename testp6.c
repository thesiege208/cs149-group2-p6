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

void childProc (void){
	// Close the unused READ end of the pipe.
            close(fd[i][READ_END]);
                
            // Write from the WRITE end of the pipe.
            write(fd[i][WRITE_END], write_msg, BUFFER_SIZE);

            // Close the WRITE end of the pipe.
            close(fd[i][WRITE_END]);
}

void parentProc (void){
	// Close the unused WRITE end of the pipe.
            close(fd[i][WRITE_END]);

            // Read to the READ end of the pipe.
            read(fd[i][READ_END], read_msg, strlen(read_msg)+1);
            printf("Parent: Read '%s' from the pipe. PID: %d\n", read_msg, getppid());
            
            // Close the READ end of the pipe.
            close(fd[i][READ_END]);
}

int main() {char write_msg[BUFFER_SIZE];
    char read_msg[BUFFER_SIZE];
    char Message[BUFFER_SIZE]; // used for fifth child process
    
    pid_t pid, waitPid;  // child process id
    int fd[6];  // file descriptors for the pipe; 0 1 2 3 4 5
    int i;
    
    	// Create the pipe.
        if (pipe(fd[i]) == -1) {
            fprintf(stderr,"pipe() failed");
            return 1;
        }
        
        // Fork a child process.
        pid = fork();
        
        if (pid > 0) {
            // PARENT PROCESS.
            parentProc();
        }
        else if (pid == 0) {
            // CHILD PROCESS.
            childProc();            
        }
        else {
            fprintf(stderr, "fork() failed");
            return 1;
        }
    
    return 0;
}
