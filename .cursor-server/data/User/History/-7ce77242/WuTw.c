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
#include <fcntl.h>
#include "common.h"
#include "hub_commands.h"
#include "handle_signals.h"

// Global pointer to monitor structure
static monitor_t *current_monitor = NULL;

// Function to set the current monitor
void set_monitor(monitor_t *monitor) {
    current_monitor = monitor;
}

void sigterm_handler(int sig)
{
    usleep(3000000); // Simulate cleanup
    printf("SIGTERM received, cleaning up...\n");
    exit(0);
}

void handle_sigterm()
{
    if (current_monitor != NULL) {
        current_monitor->status = 0;
    }
    exit(0);
}

void sigchld_handler(int signo) {
    if (current_monitor != NULL && current_monitor->status == 1) {
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        
        if (pid > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                current_monitor->status = 0;
                printf("\nMonitor process terminated.\n");
            }
        }
    }
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
            dup2(pipefd[1], STDERR_FILENO);  // Also capture stderr
            close(pipefd[1]);
            
            execl("/bin/sh", "sh", "-c", buffer, NULL);
            _exit(1);
        } else if (pid > 0) {
            // Parent process (monitor)
            close(pipefd[1]);  // Close write end
            
            // Read from command pipe and write to monitor's stdout
            // (which is already redirected to main process)
            char output[4096];
            ssize_t n;
            while ((n = read(pipefd[0], output, sizeof(output)-1)) > 0) {
                write(STDOUT_FILENO, output, n);
            }
            
            // Wait for command to complete
            int status;
            waitpid(pid, &status, 0);
            
            close(pipefd[0]);
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
    sa.sa_handler = SIG_IGN;
    sigaction(SIGUSR1, &sa, NULL);
    
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "given_command.txt");
    
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Error opening command file");
        return;
    }
    
    char command[4096];
    if (fgets(command, sizeof(command), file) != NULL) {
        fclose(file);
        system(command);
    } else {
        fclose(file);
        printf("Error reading command\n");
    }
    
    sa.sa_handler = handle_usr;
    sigaction(SIGUSR1, &sa, NULL);
}