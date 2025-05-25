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
#include "common.h"
#include "handle_signals.h"

#define COMMAND_SIZE 4096

static volatile sig_atomic_t should_exit = 0;
static char command_buffer[COMMAND_SIZE];

void handle_sigusr1(int signo) {
    (void)signo;  // Unused parameter
    
    // Read command from file
    FILE *cmd_file = fopen("given_command.txt", "r");
    if (!cmd_file) {
        perror("Failed to open command file");
        return;
    }

    if (fgets(command_buffer, sizeof(command_buffer), cmd_file) == NULL) {
        perror("Failed to read command");
        fclose(cmd_file);
        return;
    }
    fclose(cmd_file);

    // Remove newline if present
    size_t len = strlen(command_buffer);
    if (len > 0 && command_buffer[len-1] == '\n') {
        command_buffer[len-1] = '\0';
    }

    // Execute the command
    FILE *pipe = popen(command_buffer, "r");
    if (!pipe) {
        perror("Failed to execute command");
        return;
    }

    // Read and print output
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        printf("%s", buffer);
        fflush(stdout);
    }

    pclose(pipe);
}

void handle_sigterm(int signo) {
    (void)signo;  // Unused parameter
    should_exit = 1;
}

int main(void) {
    // Set up signal handlers
    struct sigaction sa_usr1 = {0};
    sa_usr1.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    struct sigaction sa_term = {0};
    sa_term.sa_handler = handle_sigterm;
    sigaction(SIGTERM, &sa_term, NULL);

    // Make stdout unbuffered
    setvbuf(stdout, NULL, _IONBF, 0);

    // Main loop
    while (!should_exit) {
        pause();  // Wait for signals
    }

    return 0;
} 