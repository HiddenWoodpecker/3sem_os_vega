#define _POSIX_C_SOURCE 200809L  


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

void custom_exit(void) {
    printf("process PID=%d ends\n", (int)getpid());
}

void custom_sigint() {
    printf("process PID=%d got a signal SIGINT\n", (int)getpid());
}

void custom_sigterm() {
    printf("process PID=%d got a signal SIGTERM\n", (int)getpid());
}

int main() {
    pid_t pid;
    int status;

    atexit(custom_exit);

    signal(SIGINT, custom_sigint);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = custom_sigterm;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTERM, &sa, NULL);

    printf("start PID=%d\n", (int)getpid());

    pid = fork();

    if (pid == -1) {
        perror("fork error");
        exit(1);
    }
    else if (pid == 0) {
        printf("child PID=%d, PPID=%d\n", (int)getpid(), (int)getppid());
        exit(52);
    }
    else {
        printf("parent's PID=%d, PPID=%d\n", (int)getpid(), (int)getppid());
        printf("child's PID=%d\n", (int)pid);
        printf("waiting...\n");
        wait(&status);

        if (WIFEXITED(status)) {
            printf("child exited with %d\n", WEXITSTATUS(status));
        }
        printf("parent finished\n");
    }

    return 0;
}
