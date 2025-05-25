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
    printf("\nDebug: Entering usr_handler in process %d for signal %d (SIGUSR1)\n", getpid(), sig);
    
    // Print current working directory
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Debug: Current working directory: %s\n", cwd);
    }

    FILE *file;
    if ((file = fopen("./given_command.txt", "r")) == NULL) {
        printf("Error at opening command file: ./given_command.txt\n");
        perror("fopen failed");
        return;
    }

    char buffer[PATH_MAX];
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("Debug: Read command from file: %s\n", buffer);
        int pid = fork();
        if(pid == 0) {
            // Child process
            printf("Debug: Executing command in child process %d\n", getpid());
            execl("/bin/sh", "sh", "-c", buffer, NULL);
            perror("execl failed");
            exit(1);
        } else if (pid > 0) {
            // Parent process - wait for command to complete
            printf("Debug: Waiting for command process %d to complete\n", pid);
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                printf("Debug: Command process exited with status %d\n", WEXITSTATUS(status));
                if (WEXITSTATUS(status) != 0) {
                    printf("Command failed with status %d\n", WEXITSTATUS(status));
                }
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
    printf("Debug: Exiting usr_handler\n");
}

void handle_usr() {
    // Check if SIGUSR1 is blocked
    sigset_t current_mask;
    sigprocmask(SIG_BLOCK, NULL, &current_mask);
    if (sigismember(&current_mask, SIGUSR1)) {
        printf("Debug: SIGUSR1 was blocked, unblocking it\n");
        sigset_t unmask;
        sigemptyset(&unmask);
        sigaddset(&unmask, SIGUSR1);
        sigprocmask(SIG_UNBLOCK, &unmask, NULL);
    }

    struct sigaction sa, old_sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = usr_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    printf("Debug: Setting up SIGUSR1 handler in process %d\n", getpid());
    
    // Check if there was a previous handler
    if (sigaction(SIGUSR1, NULL, &old_sa) != -1) {
        if (old_sa.sa_handler != SIG_DFL && old_sa.sa_handler != SIG_IGN) {
            printf("Debug: Previous SIGUSR1 handler found\n");
        }
    }

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Verify the handler was set
    if (sigaction(SIGUSR1, NULL, &old_sa) != -1) {
        if (old_sa.sa_handler == usr_handler) {
            printf("Debug: SIGUSR1 handler verified and set correctly to usr_handler\n");
        } else {
            printf("Debug: WARNING: SIGUSR1 handler not set correctly!\n");
        }
    }

    // Test the signal handling by sending ourselves a SIGUSR1
    printf("Debug: Testing SIGUSR1 handler with self-signal\n");
    kill(getpid(), SIGUSR1);
}