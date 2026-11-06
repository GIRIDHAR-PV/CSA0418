#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    printf("shell: running job 1\n");
    fflush(stdout);

    int status;
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        execlp("expr", "expr", "6", "*", "7",
               (char *)NULL);
        _exit(127);
    }

    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return 1;
    }

    if (WIFEXITED(status))
        printf("job 1 exit status %d\n",
               WEXITSTATUS(status));

    printf("shell: running job 2\n");
    fflush(stdout);

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0)
        _exit(3);

    waitpid(pid, &status, 0);

    if (WIFEXITED(status))
        printf("job 2 exit status %d\n",
               WEXITSTATUS(status));

    return 0;
}

