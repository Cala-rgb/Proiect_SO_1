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
#include "common.h"
#include "config.h"

#define BUFFER_SIZE 4096

// Get the working directory path
static char working_dir[PATH_MAX] = {0};

const char* get_working_dir(void) {
    if (working_dir[0] == '\0') {
        if (getcwd(working_dir, sizeof(working_dir)) == NULL) {
            perror("getcwd failed");
            return BASE_DIR;
        }
    }
    return working_dir;
}

// Helper function to build file paths
static void build_path(char *dest, size_t dest_size, const char *filename) {
    snprintf(dest, dest_size, "%s/%s", BASE_DIR, filename);
}

FILE* openFile() {
    FILE *file;
    if ((file = fopen(COMMAND_FILE, "w")) == NULL) {
        printf("Error at opening command file: %s\n", COMMAND_FILE);
        perror("fopen failed");
        return NULL;
    }
    return file;
}

// Helper function to count treasures in a hunt
static int count_treasures(const char *hunt_path) {
    char bin_path[PATH_MAX];
    snprintf(bin_path, sizeof(bin_path), "%s/%s", hunt_path, TREASURE_FILE);

    struct stat st;
    if (stat(bin_path, &st) == -1) {
        return 0;
    }

    return st.st_size / sizeof(treasure_t);
}

void list_hunts_cmd(monitor_t *monitor) {
    if (monitor->status == 1) {
        // Check if hunts directory exists
        struct stat st = {0};
        if (stat(HUNTS_DIR, &st) == -1) {
            printf("Hunts directory does not exist at: %s\n", HUNTS_DIR);
            return;
        }
        
        // Open hunts directory
        DIR *dir = opendir(HUNTS_DIR);
        if (!dir) {
            printf("Error: Could not open hunts directory at: %s\n", HUNTS_DIR);
            perror("opendir failed");
            return;
        }

        printf("Available hunts:\n");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char full_path[PATH_MAX];
                snprintf(full_path, sizeof(full_path), HUNT_PATH_FORMAT, entry->d_name);
                int num_treasures = count_treasures(full_path);
                printf("Hunt ID: %s, Number of Treasures: %d\n", entry->d_name, num_treasures);
            }
        }
        closedir(dir);

        usleep(100000);  // Sleep for 0.1 seconds
        read_from_pipe(monitor);
    } else {
        printf("Monitor is not running!\n");
    }
}

void view_treasure_cmd(monitor_t *monitor) {
    if (monitor->status == 1) {
        printf("Hunt ID:\n");
        char hid[25];
        scanf("%25s", hid);
        printf("Treasure ID:\n");
        char tid[25];
        scanf("%25s", tid);

        FILE *file = openFile();
        if(file != NULL) {
            fprintf(file, "%s --view %s %s", TREASURE_MANAGER, hid, tid);
            fclose(file);
        }

        printf("Viewing treasure with Hunt ID: %s and Treasure ID: %s\n", hid, tid);
        if (kill(monitor->pid, SIGUSR1) == -1) {
            perror("kill failed");
        } else {
            usleep(100000);  // Sleep for 0.1 seconds
            read_from_pipe(monitor);
        }
    } else {
        printf("Monitor is not running!\n");
    }
}

void list_treasures_cmd(monitor_t *monitor) {
    if (monitor->status == 1) {
        printf("Hunt ID:\n");
        char hid[25];
        scanf("%25s", hid);

        FILE *file = openFile();
        if (file != NULL) {
            fprintf(file, "%s --list %s", TREASURE_MANAGER, hid);
            fclose(file);
        }

        printf("Listing treasures for Hunt ID: %s\n", hid);
        if (kill(monitor->pid, SIGUSR1) == -1) {
            perror("kill failed");
        } else {
            usleep(100000);  // Sleep for 0.1 seconds
            read_from_pipe(monitor);
        }
    } else {
        printf("Monitor is not running!\n");
    }
}

void calculate_score_cmd(monitor_t *monitor) {
    if (monitor->status == 1) {
        printf("Calculating scores for all hunts...\n");
        
        // Check if hunts directory exists
        struct stat st = {0};
        if (stat(HUNTS_DIR, &st) == -1) {
            printf("Hunts directory does not exist at: %s\n", HUNTS_DIR);
            return;
        }
        
        // Open hunts directory
        DIR *dir = opendir(HUNTS_DIR);
        if (!dir) {
            printf("Error: Could not open hunts directory at: %s\n", HUNTS_DIR);
            perror("opendir failed");
            return;
        }

        // Write command to calculate scores for all hunts
        FILE *file = openFile();
        if (file != NULL) {
            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    fprintf(file, "%s %s && ", SCORE_CALCULATOR, entry->d_name);
                }
            }
            fprintf(file, "true");  // Add true at the end to make the command valid
            fclose(file);
        }
        closedir(dir);

        if (kill(monitor->pid, SIGUSR1) == -1) {
            perror("kill failed");
        } else {
            usleep(100000);  // Sleep for 0.1 seconds
            read_from_pipe(monitor);
        }
    } else {
        printf("Monitor is not running!\n");
    }
}

void start_monitor(monitor_t *monitor) {
    if (monitor->status == 0) {
        // Create pipe for communication
        if (pipe(monitor->pipe_fd) == -1) {
            perror("pipe failed");
            return;
        }

        // Fork a new process
        monitor->pid = fork();
        if (monitor->pid == -1) {
            perror("fork failed");
            return;
        }

        if (monitor->pid == 0) {  // Child process (monitor)
            // Close read end of pipe
            close(monitor->pipe_fd[0]);

            // Redirect stdout to pipe
            dup2(monitor->pipe_fd[1], STDOUT_FILENO);
            close(monitor->pipe_fd[1]);

            // Set up signal handlers
            handle_sigterm();  // Set up SIGTERM handler
            handle_usr();      // Set up SIGUSR1 handler
            
            // Make stdout unbuffered
            setvbuf(stdout, NULL, _IONBF, 0);
            
            printf("Monitor started with PID: %d\n", getpid());
            fflush(stdout);

            // Keep the monitor running and wait for signals
            while(1) {
                pause();  // Wait for signals
            }
            
            exit(1);  // Should never reach here
        } else {  // Parent process
            // Close write end of pipe
            close(monitor->pipe_fd[1]);
            monitor->status = 1;
            
            // Make read end non-blocking
            int flags = fcntl(monitor->pipe_fd[0], F_GETFL, 0);
            fcntl(monitor->pipe_fd[0], F_SETFL, flags | O_NONBLOCK);
            
            // Read initial output
            read_from_pipe(monitor);
        }
    } else {
        printf("Monitor is already running!\n");
    }
}

void stop_monitor(monitor_t *monitor) {
    if (monitor->status == 1) {
        monitor->status = 2;  // Set status to stopping
        if (kill(monitor->pid, SIGTERM) == -1) {
            perror("kill failed");
        }
        int status;
        waitpid(monitor->pid, &status, 0);
        close(monitor->pipe_fd[0]);
        monitor->status = 0;
        printf("Monitor stopped\n");
    } else {
        printf("Monitor is not running!\n");
    }
}

void read_from_pipe(monitor_t *monitor) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int empty_reads = 0;
    
    // Set pipe to non-blocking mode
    int flags = fcntl(monitor->pipe_fd[0], F_GETFL, 0);
    fcntl(monitor->pipe_fd[0], F_SETFL, flags | O_NONBLOCK);
    
    // Try to read from pipe, but stop if we get no data twice in a row
    while (empty_reads < 2) {
        bytes_read = read(monitor->pipe_fd[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("%s", buffer);
            empty_reads = 0;  // Reset counter when we get data
            usleep(10000);    // Small 10ms delay between reads
        } else if (bytes_read == 0 || (bytes_read == -1 && errno != EAGAIN)) {
            break;  // End of pipe or error
        } else {
            empty_reads++;    // Count unsuccessful reads
            usleep(50000);    // 50ms delay when no data
        }
    }
}