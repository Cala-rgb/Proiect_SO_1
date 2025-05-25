#define _POSIX_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
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
    printf("DEBUG: Entering usr_handler for signal %d\n", sig);
    fflush(stdout);

    // Re-establish signal handler
    signal(SIGUSR1, usr_handler);

    FILE *file;
    if ((file = fopen("./given_command.txt", "r")) == NULL) {
        printf("Error at opening command file: ./given_command.txt\n");
        perror("fopen failed");
        return;
    }

    char buffer[PATH_MAX];
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("DEBUG: Read command: %s\n", buffer);
        fflush(stdout);
        
        int pid = fork();
        if(pid == 0) {
            // Child process - stdout is already redirected to pipe from parent
            printf("DEBUG: Executing command in child\n");
            fflush(stdout);
            execl("/bin/sh", "sh", "-c", buffer, NULL);
            perror("execl failed");
            exit(1);
        } else if (pid > 0) {
            // Parent process - wait for command to complete
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                printf("DEBUG: Command completed with status %d\n", WEXITSTATUS(status));
                fflush(stdout);
            } else if (WIFSIGNALED(status)) {
                printf("Command terminated by signal %d\n", WTERMSIG(status));
            }
        } else {
            perror("fork failed");
        }
    } else {
        printf("Error reading from file or file is empty.\n");
    }
    fclose(file);
}

void handle_usr() {
    printf("DEBUG: Setting up SIGUSR1 handler in process %d\n", getpid());
    fflush(stdout);

    // Block SIGUSR1 during setup
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // Set up signal handler
    if (signal(SIGUSR1, usr_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    // Unblock SIGUSR1
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    printf("DEBUG: SIGUSR1 handler setup complete\n");
    fflush(stdout);
}