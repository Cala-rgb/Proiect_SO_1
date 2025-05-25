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
#include <limits.h>
#include "hub_commands.h"
#include "handle_signals.h"

#define BUFFER_SIZE 4096

// Get the working directory path
static char working_dir[PATH_MAX] = {0};

const char* get_working_dir(void) {
    if (working_dir[0] == '\0') {
        if (getcwd(working_dir, sizeof(working_dir)) == NULL) {
            perror("getcwd failed");
            return ".";
        }
    }
    return working_dir;
}

// Helper function to build file paths
static void build_path(char *dest, size_t dest_size, const char *filename) {
    snprintf(dest, dest_size, "./%s", filename);
}

FILE* openFile() {
    char path[PATH_MAX];
    build_path(path, sizeof(path), "given_command.txt");

    FILE *file;
    if ((file = fopen(path, "w")) == NULL) {
        printf("Error at opening command file: %s\n", path);
        perror("fopen failed");
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
            char command[PATH_MAX];
            build_path(command, sizeof(command), "tm");
            fprintf(file, "%s --list_hunts", command);
            fclose(file);
        }

        printf("DEBUG: Sending SIGUSR1 to monitor process %d\n", monitor->pid);
        fflush(stdout);
        printf("Listing all hunts...\n");
        if (kill(monitor->pid, SIGUSR1) == -1) {
            perror("kill failed");
        } else {
            printf("DEBUG: Signal sent successfully to PID: %d\n",monitor->pid);
            fflush(stdout);
            
            // Give the monitor process some time to execute the command
            usleep(100000);  // Sleep for 0.1 seconds
            
            // Read the output
            read_from_pipe(monitor);
        }
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
            char command[PATH_MAX];
            build_path(command, sizeof(command), "tm");
            fprintf(file, "%s --view %s %s", command, hid, tid);
            fclose(file);
        }

        printf("Viewing treasure with Hunt ID: %s and Treasure ID: %s\n", hid, tid);
        if (kill(monitor->pid, SIGUSR1) == -1) {
            perror("kill failed");
        } else {
            // Give the monitor process some time to execute the command
            usleep(100000);  // Sleep for 0.1 seconds
            
            // Read the output
            read_from_pipe(monitor);
        }
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
            char command[PATH_MAX];
            build_path(command, sizeof(command), "tm");
            fprintf(file, "%s --list %s", command, hid);
            fclose(file);
        }

        printf("Listing treasures for Hunt ID: %s\n", hid);
        if (kill(monitor->pid, SIGUSR1) == -1) {
            perror("kill failed");
        } else {
            // Give the monitor process some time to execute the command
            usleep(100000);  // Sleep for 0.1 seconds
            
            // Read the output
            read_from_pipe(monitor);
        }
    }
    else
    {
        printf("Monitor is not running!\n");
    }
}

void calculate_score(monitor *monitor) {
    if (monitor->status == 1) {
        printf("Calculating scores for all hunts...\n");
        
        // Build path for hunts directory
        char hunts_path[PATH_MAX];
        build_path(hunts_path, sizeof(hunts_path), "hunts");
        
        // Check if hunts directory exists
        struct stat st = {0};
        if (stat(hunts_path, &st) == -1) {
            printf("Hunts directory does not exist at: %s\n", hunts_path);
            return;
        }
        
        // Open hunts directory
        DIR *dir = opendir(hunts_path);
        if (!dir) {
            printf("Error: Could not open hunts directory at: %s\n", hunts_path);
            perror("opendir failed");
            return;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                // Create a process for each hunt
                pid_t pid = fork();
                
                if (pid == 0) {
                    // Child process
                    char sc_path[PATH_MAX];
                    build_path(sc_path, sizeof(sc_path), "sc");
                    char *args[] = {sc_path, entry->d_name, NULL};
                    execv(sc_path, args);
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

        // Set stdout to be unbuffered
        setvbuf(stdout, NULL, _IONBF, 0);
        
        printf("Monitor started with PID %d!\n", getpid());
        fflush(stdout);
        
        handle_sigterm();
        handle_usr();  // Set up SIGUSR1 handler

        monitor->status = 1; // Running
        
        // Keep the monitor running and wait for signals
        while(1) {
            pause();  // Wait for any signal
        }
        
    } else if (monitor->pid > 0) {
        // Parent process
        close(monitor->pipe_fd[1]); // Close write end in parent
        handle_sigchld();
        monitor->status = 1; // Running

        // Make read end non-blocking
        fcntl(monitor->pipe_fd[0], F_SETFL, O_NONBLOCK);
        
        // Give the monitor a moment to start and read initial output
        usleep(100000);  // Sleep for 0.1 seconds
        read_from_pipe(monitor);
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