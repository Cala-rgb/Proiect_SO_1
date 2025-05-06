#define _POSIX_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "hub_commands.h"
#include "handle_signals.h"

void sigterm_handler(int sig)
{
    usleep(3000000); // Simulate cleanup
    printf("SIGTERM received, cleaning up...\n");
    exit(0);
}

void handle_sigterm()
{
    getMainMonitor()->status=2; // Stopping
    struct sigaction sa;
    sa.sa_handler = sigterm_handler;
    //printf("PID:%d\n",getpid());
    if (sigaction(SIGTERM, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

void sigchld_handler(int sig)
{
    int saved_errno = errno;
    pid_t pid;
    int status;

    getMainMonitor()->status = 0; // Stopped

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (WIFEXITED(status))
        {
            printf("Child process %d terminated with exit status %d\n", pid, WEXITSTATUS(status));

        }
        else if (WIFSIGNALED(status))
        {
            printf("Child process %d terminated by signal %d\n", pid, WTERMSIG(status));
        }
    }

    errno = saved_errno;
}

void handle_sigchld()
{
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}


void usr_handler(int sig) {

    //printf("Received SIGUSR1 signal\n");

    static char path[128];
    snprintf(path, sizeof(path), "%s", "given_command.txt");

    FILE *file;

    if ((file = fopen(path, "r")) == NULL)
    {
        printf("Error at opening log file!\n");
        return ;
    }

    if (file != NULL) {
        char buffer[101];
        if (fgets(buffer, sizeof(buffer), file) != NULL) {
            int pid = fork();
            if(pid==0) {
                chdir("..");
                execl("/bin/sh", "sh", "-c", buffer, NULL);
            }
        } else {
            printf("Error reading from file or file is empty.\n");
        }
        fclose(file);
    }
}

void handle_usr() {
    struct sigaction sa;
    sa.sa_handler = usr_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    //printf("SIGUSR1 handler set\n");
}