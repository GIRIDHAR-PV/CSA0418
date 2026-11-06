#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
 
int main(void)
{
    int count = 3;
 
    for (int i = 0; i < count; i++) {
        pid_t pid = fork();
 
        if (pid < 0) {
            perror("fork");
            return 1;
        }
 
        if (pid == 0) {
            printf("Child %d: PID = %d, PPID = %d\n",
                   i + 1, getpid(), getppid());
            _exit(0);
        }
    }
 
    for (int i = 0; i < count; i++)
        wait(NULL);
 
    printf("Parent: all child processes completed\n");
    return 0;
}
