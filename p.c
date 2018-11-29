#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#define BUFFER_SIZE 128
#define READ_END     0
#define WRITE_END    1

int64_t getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main(void) {
    char write_msg[BUFFER_SIZE];
    char read_msg[BUFFER_SIZE];
    char buf[BUFFER_SIZE];    // used for writing of child5
    
    int fd[2], i;     // file descriptors for the pipe
    pid_t pid;        // child process id
    
    int64_t start = getCurrentTime();
    int64_t finish;
    double sT;
    double spendTime[20];
    
    FILE *fp;
    fp = fopen("output.txt", "w");
    
    // Create the pipe.
    if (pipe(fd) == -1) {
        fprintf(stderr, "pipe() failed");
        exit(1);
    }
    
    
    // Fork five child process.
    for (i = 0; i < 5; i++) {
        int random = rand() % 3;
        sleep(rand() % 3);
        finish = getCurrentTime();
        spendTime[i] = difftime(finish, start) / 1000;
        pid = fork();
        // random 0 1 2s
        if (pid == 0) {
            
            
            printf("Time: %f , child %d, create childId=%d, parentId=%d\n", spendTime[i], i + 1, getpid(), getppid());
            if (fp != NULL)
                fprintf(fp, "Time: %f , child %d, create childId=%d, parentId=%d\n", spendTime[i], i + 1, getpid(),
                        getppid());
            break;
        } else if (pid == -1) {
            perror("pipe error");
            exit(1);
        }
    }
    
    if (i == 0) {
        // CHILD PROCESS.
        //printf("Child process\n");
        // Close the unused WRITE end of the pipe.
        close(fd[READ_END]);
        
        // Read from the READ end of the pipe.
        write(fd[WRITE_END], "child1", BUFFER_SIZE);
        // Close the READ end of the pipe.
        close(fd[WRITE_END]);
        sleep(30);
        exit(1);
    } else if (i == 1) {
        // CHILD PROCESS.
        close(fd[READ_END]);
        write(fd[WRITE_END], "child2", BUFFER_SIZE);
        close(fd[WRITE_END]);
        sleep(30);
        exit(1);
    } else if (i == 2) {
        // CHILD PROCESS.
        close(fd[READ_END]);
        write(fd[WRITE_END], "child3", BUFFER_SIZE);
        close(fd[WRITE_END]);
        sleep(30);
        exit(1);
    } else if (i == 3) {
        // CHILD PROCESS.
        close(fd[READ_END]);
        write(fd[WRITE_END], "child4", BUFFER_SIZE);
        close(fd[WRITE_END]);
        sleep(30);
        exit(1);
    } else if (i == 4) {
        // CHILD PROCESS.
        close(fd[READ_END]);
        for (int x = 0; x < 5; x++) {
            finish = getCurrentTime();
            spendTime[5+x] =difftime(finish, start) / 1000;
            memset(buf, 0, sizeof(buf));
            printf("child5, Please input: \n");
            scanf("%s", buf);
            printf("\n");
            write(fd[WRITE_END], buf, BUFFER_SIZE);
        }
        close(fd[WRITE_END]);
        sleep(30);
        exit(1);
        
    } else {
        // PARENT PROCESS.
        close(fd[WRITE_END]);
        // Close the unused READ end of the pipe.
        for (i = 0; i < 9; i++) {
            finish = getCurrentTime();
            
            memset(read_msg, 0, sizeof(read_msg));
            read(fd[READ_END], read_msg, BUFFER_SIZE);
            
            printf("ParentTime=%f, Read '%s' to the pipe, id=%d, childTime=%f\n", difftime(finish, start) / 1000,
                   read_msg, getpid(), spendTime[i]);
            fprintf(fp, "ParentTime=%f, Read '%s' to the pipe, id=%d, childTime=%f\n", difftime(finish, start) / 1000,
                    read_msg, getpid(), spendTime[i]);
            //wait(NULL);
        }
        sleep(30);
        exit(0);
    }
    
    return 0;
}

