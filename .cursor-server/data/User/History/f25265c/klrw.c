#define _POSIX_SOURCE 
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include "hub_commands.h"
#include "handle_signals.h"

#define BUFFER_SIZE 4096

void start_monitor(monitor *monitor) {
    // Create pipe before forking
    if (pipe(monitor->pipe_fd) == -1) {
        perror("Pipe creation failed");
        return;
    }

    monitor->pid = fork();
    if (monitor->pid == 0) {
        // Child process (monitor)
        close(monitor->pipe_fd[0]); // Close read end in child
        // Redirect stdout to pipe
        dup2(monitor->pipe_fd[1], STDOUT_FILENO);
        close(monitor->pipe_fd[1]);

        printf("Monitor started!\n");
        handle_sigterm();
        handle_usr();

        monitor->status = 1; // Running
        
    } else if (monitor->pid > 0) {
        // Parent process
        close(monitor->pipe_fd[1]); // Close write end in parent
        handle_sigchld();
        monitor->status = 1; // Running

        // Make read end non-blocking
        fcntl(monitor->pipe_fd[0], F_SETFL, O_NONBLOCK);
    } else {
        // Fork failed
        perror("Fork failed");
        close(monitor->pipe_fd[0]);
        close(monitor->pipe_fd[1]);
    }
}

void stop_monitor(monitor *monitor) {
    if(monitor->pid>0) {
        printf("Stopping monitor with PID %d...\n", monitor->pid);
        if (monitor->status == 1)
        {
            kill(monitor->pid, SIGTERM);
            close(monitor->pipe_fd[0]); // Close read end
        }
        else
        {
            printf("Monitor is not running!\n");
        }
    }
}

// Helper function to read from pipe
void read_from_pipe(monitor *monitor) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    // Read and print any available data from pipe
    while ((bytes_read = read(monitor->pipe_fd[0], buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }
}

FILE* openFile() {
    static char path[128];
    snprintf(path, sizeof(path), "%s", "given_command.txt");

    FILE *file;

    if ((file = fopen(path, "w")) == NULL)
    {
        printf("Error at opening log file!\n");
        return NULL;
    }

    return file;
}

void list_hunts(monitor *monitor) {
    if (monitor->status == 1)
    {
        FILE *file = openFile();

        if (file != NULL)
        {
            fprintf(file, "/home/cala/tm --list_hunts");
            fclose(file);
        }

        printf("Listing all hunts...\n");
        kill(monitor->pid, SIGUSR1);
    }
    else
    {
        printf("Monitor is not running!\n");
    }
}

void view_treasure(monitor *monitor) {
    if (monitor->status == 1)
    {
        // View treasure
        printf("Hunt ID:\n");
        char hid[25];
        scanf("%25s", hid);
        printf("Treasure ID:\n");
        char tid[25];
        scanf("%25s", tid);

        FILE *file = openFile();

        if(file != NULL) {
            fprintf(file, "/home/cala/tm --view %s %s",hid,tid);
            fclose(file);
        }

        printf("Viewing treasure with Hunt ID: %s and Treasure ID: %s\n", hid, tid);
        kill(monitor->pid, SIGUSR1);

    }
    else
    {
        printf("Monitor is not running!\n");
    }
}

void list_treasures(monitor *monitor) {
    if (monitor->status == 1)
    {
        // List treasures
        printf("Hunt ID:\n");
        char hid[25];
        scanf("%25s", hid);

        FILE *file = openFile();

        if (file != NULL)
        {
            fprintf(file, "/home/cala/tm --list %s", hid);
            fclose(file);
        }

        printf("Listing treasures for Hunt ID: %s\n", hid);
        kill(monitor->pid, SIGUSR1);
    }
    else
    {
        printf("Monitor is not running!\n");
    }
}

void calculate_score(monitor *monitor) {
    if (monitor->status == 1) {
        printf("Calculating scores for all hunts...\n");
        
        // Create hunts directory if it doesn't exist
        struct stat st = {0};
        if (stat("hunts", &st) == -1) {
            printf("Hunts directory does not exist.\n");
            return;
        }
        
        // Open hunts directory
        DIR *dir = opendir("hunts");
        if (!dir) {
            printf("Error: Could not open hunts directory\n");
            return;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                // Create a process for each hunt
                pid_t pid = fork();
                
                if (pid == 0) {
                    // Child process
                    char *args[] = {"/home/cala/sc", entry->d_name, NULL};
                    execv("/home/cala/sc", args);
                    perror("execv failed");
                    exit(1);
                } else if (pid > 0) {
                    // Parent process
                    int status;
                    waitpid(pid, &status, 0);
                } else {
                    perror("Fork failed");
                }
            }
        }
        closedir(dir);
    } else {
        printf("Monitor is not running!\n");
    }
}