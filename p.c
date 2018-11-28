#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#define BUFFER_SIZE 32
#define READ_END     0
#define WRITE_END    1

int main(void) {
    char write_msg[BUFFER_SIZE] = "Child process!";
    char read_msg[BUFFER_SIZE];
    char buffer[128];

    pid_t pid;  // child process id
    int fd[2];  // file descriptors for the pipe

    // Create the pipe.
    if (pipe(fd) == -1) {
        fprintf(stderr, "pipe() failed");
        return 1;
    }

    // Fork five child process.
    //pid = fork();
    for (int i = 0; i < 5; i++) {
        pid = fork();
        // random 0 1 2s
        sleep(rand()%3);
        if (pid == 0 || pid == -1) {
            printf("create child process id=%d, parent process id=%d\n", getpid(), getppid());

            break;

        }
    }

    if (pid > 0) {
        int result, nread;
        fd_set inputs, inputfds; // sets of file descriptors
        struct timeval timeout;

        // PARENT PROCESS.
        //printf("Parent process\n");
        // Close the unused READ end of the pipe.
        close(fd[WRITE_END]);
        read(fd[READ_END], read_msg, BUFFER_SIZE);
        printf("Parent: Read '%s' to the pipe. process id=%d\n", read_msg, getpid());

        FD_ZERO(&inputs);       // initialize inputs to the empty set
        FD_SET(0, &inputs);     // set file descriptor read_msg (stdin)
        timeout.tv_sec = 2;
        timeout.tv_usec = 500000;

        for (;;) {
            //inputfds = read_msg;
            // Write to the WRITE end of the pipe.
            inputfds=inputs;
            result = select(FD_SETSIZE, &inputfds, (fd_set *) 0, (fd_set *) 0, &timeout);


            if (result == -1) {                // timeout w/o input
                printf("@");
                fflush(stdout);
            } else if (result == 0) {// error
                printf("select");
                exit(1);
            } else { // Got input
                printf("else");
                // If, during the wait, we have some action on the file descriptor,
                // we read the input on stdin and echo it whenever an
                // <end of line> character is received, until that input is Ctrl-D.

                if (FD_ISSET(0, &inputfds)) {
                    ioctl(0, FIONREAD, &nread);
                    if (nread == 0) {
                        printf("Keyboard input done.\n");
                        exit(0);
                    }

                    nread = read(0, buffer, nread);
                    buffer[nread] = 0;
                    printf("Read %d characters from the keyboard: %s",
                           nread, buffer);
                }

            }
            // Close the WRITE end of the pipe.
            //close(fd[READ_END]);
            //exit(1);
        }
    } else if (pid == 0) {
        // CHILD PROCESS.
        //printf("Child process\n");
        // Close the unused WRITE end of the pipe.
        close(fd[READ_END]);

        // Read from the READ end of the pipe.
        write(fd[WRITE_END], write_msg, strlen(write_msg) + 1);
        printf("Child : Wrote '%s' from the pipe. process id=%d\n", write_msg, getpid());
        // Close the READ end of the pipe.
        close(fd[WRITE_END]);
        exit(1);
    } else {
        fprintf(stderr, "fork() failed");
        return 1;
    }

    return 0;
}

