#define _POSIX_SOURCE 
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "hub_commands.h"
#include "handle_signals.h"

void start_monitor(monitor *monitor) {

    monitor->pid = fork();
    if (monitor->pid == 0) {
        printf("Monitor started!\n");
        handle_sigterm();
        handle_usr();

        monitor->status = 1; // Running
        
    } else if (monitor->pid > 0) {
        handle_sigchld();
        // Parent process
        monitor->status = 1;
         // Running
    } else {
        // Fork failed
        perror("Fork failed");
    }
}

void stop_monitor(monitor *monitor) {
    if(monitor->pid>0) {
        printf("Stopping monitor with PID %d...\n", monitor->pid);
        if (monitor->status == 1)
        {
            kill(monitor->pid, SIGTERM);
        }
        else
        {
            printf("Monitor is not running!\n");
        }
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
            fprintf(file, "./tm --list_hunts");
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
            fprintf(file, "./tm --view %s %s",hid,tid);
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
            fprintf(file, "./tm --list %s", hid);
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