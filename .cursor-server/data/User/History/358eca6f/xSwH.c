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
#include "interface.h"
#include "common.h"

monitor_t m = {-1, 0, {-1, -1}};

monitor_t *getMainMonitor() {
    return &m;
}

int monitor_is_stopping() {
    if (m.status == 2) {
        return 1; // Stopping
    }
    return 0; // Not stopping
}

// Classic command-line mode
void run_classic_mode() {
    char command[100];
    while(1) {
        command[0] = '\0';
        
        if(m.pid!=0) {
            printf("th> ");
            scanf("%99s", command);

            if(monitor_is_stopping()) {
                if(m.pid>0) {
                    printf("Monitor is stopping, can not execute commands!\n");
                }
            } else if (strcmp(command, "start_monitor") == 0) {
                start_monitor(&m);
            }
            else if (strcmp(command, "list_hunts") == 0) {
                list_hunts_cmd(&m);
            }
            else if (strcmp(command, "list_treasures") == 0) {
                list_treasures_cmd(&m);
            }
            else if (strcmp(command, "view_treasure") == 0) {
                view_treasure_cmd(&m);
            }
            else if (strcmp(command, "calculate_score") == 0) {
                calculate_score_cmd(&m);
            }
            else if (strcmp(command, "stop_monitor") == 0) {
                stop_monitor(&m);
            }
            else if (strcmp(command, "exit") == 0) {
                if(m.pid!=0) {
                    if (m.status == 1) {
                        printf("Monitor is still running!\n");
                    }
                    else {
                        printf("Exiting...\n");
                        exit(0);
                    }
                }
            } else {
                printf("%s is not an existing command!\n",command);
            }
        } 
    }
}

int main(int argc, char *argv[]) {
    // Check if interface mode is requested
    if (argc > 1 && strcmp(argv[1], "--interface") == 0) {
        return handle_menu(&m);
    }
    
    // Default to classic mode
    run_classic_mode();
    return 0;
}