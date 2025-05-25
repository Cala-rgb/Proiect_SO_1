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
    // Create a pipe for command output
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        write(STDERR_FILENO, "pipe creation failed\n", 20);
        return;
    }

    FILE *file;
    if ((file = fopen("./given_command.txt", "r")) == NULL) {
        write(STDERR_FILENO, "failed to open command file\n", 27);
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }

    char buffer[PATH_MAX];
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        int pid = fork();
        if(pid == 0) {
            // Child process
            close(pipefd[0]);  // Child closes read end
            dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe
            close(pipefd[1]);  // Close original write end after dup2
            
            execl("/bin/sh", "sh", "-c", buffer, NULL);
            write(STDERR_FILENO, "execl failed\n", 12);
            _exit(1);
        } else if (pid > 0) {
            // Parent process
            close(pipefd[1]);  // Parent closes write end
            
            // Read from pipe and write directly to the monitor's stdout
            // (which is already redirected to the main process pipe)
            char output[4096];
            ssize_t n;
            while ((n = read(pipefd[0], output, sizeof(output)-1)) > 0) {
                write(STDOUT_FILENO, output, n);  // Write to monitor's stdout pipe
                fflush(stdout);  // Ensure it's flushed
            }
            
            // Wait for command to complete
            int status;
            waitpid(pid, &status, 0);
            
            close(pipefd[0]);  // Close read end after we're done reading
        } else {
            write(STDERR_FILENO, "fork failed\n", 11);
            close(pipefd[0]);
            close(pipefd[1]);
        }
    }
    fclose(file);
}

void handle_usr() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = usr_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        write(STDERR_FILENO, "sigaction failed\n", 15);
        exit(EXIT_FAILURE);
    }
}