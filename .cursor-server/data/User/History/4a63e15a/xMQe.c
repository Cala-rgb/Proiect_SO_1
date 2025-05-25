#define _POSIX_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "monitor.h"
#include "handle_signals.h"
#include "common.h"
#include "config.h"

void start_monitor(monitor_t *monitor) {
    if(monitor->status == 1) {
        printf("Monitor is already running!\n");
        return;
    }

    // Create pipe for communication
    if(pipe(monitor->pipe_fd) == -1) {
        perror("pipe");
        return;
    }

    // Create hunts directory if it doesn't exist
    struct stat st = {0};
    if (stat(HUNTS_DIR, &st) == -1) {
        if (mkdir(HUNTS_DIR, 0777) == -1) {
            perror("mkdir failed");
            return;
        }
    }

    pid_t pid = fork();
    if(pid == -1) {
        perror("fork");
        return;
    }

    if(pid == 0) {  // Child process
        close(monitor->pipe_fd[0]);  // Close read end
        dup2(monitor->pipe_fd[1], STDOUT_FILENO);
        close(monitor->pipe_fd[1]);

        // Set up signal handlers for child process
        setup_child_signal_handlers();

        while(1) {
            pause();  // Wait for signals
        }
    } else {  // Parent process
        close(monitor->pipe_fd[1]);  // Close write end
        monitor->pid = pid;
        monitor->status = 1;
        printf("Monitor started with PID: %d\n", pid);
    }
}

void stop_monitor(monitor_t *monitor) {
    if(monitor->status != 1) {
        printf("Monitor is not running!\n");
        return;
    }

    monitor->status = 2;  // Set status to stopping
    printf("Stopping monitor...\n");

    if(kill(monitor->pid, SIGTERM) == -1) {
        perror("kill");
        return;
    }

    int status;
    if(waitpid(monitor->pid, &status, 0) == -1) {
        perror("waitpid");
        return;
    }

    close(monitor->pipe_fd[0]);
    monitor->status = 0;
    monitor->pid = 0;
    printf("Monitor stopped.\n");
}

void read_from_pipe(monitor_t *monitor) {
    if(monitor->status != 1) {
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Set pipe to non-blocking mode
    int flags = fcntl(monitor->pipe_fd[0], F_GETFL, 0);
    fcntl(monitor->pipe_fd[0], F_SETFL, flags | O_NONBLOCK);

    // Read from pipe with timeout
    int retries = 10;  // Number of retries
    while(retries > 0) {
        bytes_read = read(monitor->pipe_fd[0], buffer, sizeof(buffer) - 1);
        if(bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("%s", buffer);
        } else if(bytes_read == 0 || (bytes_read == -1 && errno != EAGAIN)) {
            break;
        }
        usleep(100000);  // Wait 0.1 seconds between reads
        retries--;
    }

    // Reset pipe to blocking mode
    fcntl(monitor->pipe_fd[0], F_SETFL, flags);
}

void execute_command(void) {
    FILE *file = fopen(COMMAND_FILE, "r");
    if(file == NULL) {
        perror("fopen");
        return;
    }

    char command[BUFFER_SIZE];
    if(fgets(command, sizeof(command), file) == NULL) {
        fclose(file);
        return;
    }
    fclose(file);

    // Remove newline if present
    size_t len = strlen(command);
    if(len > 0 && command[len-1] == '\n') {
        command[len-1] = '\0';
    }

    // Execute the command
    FILE *fp = popen(command, "r");
    if(fp == NULL) {
        perror("popen");
        return;
    }

    char buffer[BUFFER_SIZE];
    while(fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    pclose(fp);
} 