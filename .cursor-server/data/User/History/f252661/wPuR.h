#ifndef COMMANDS
#define COMMANDS

#include "handle_signals.h"

typedef struct monitor{
    int pid;
    int status;
    //0 => stopped
    //1 => running
    //2 => stopping
    int pipe_fd[2];  // Pipe for communication between monitor and main process
}monitor;

void stop_monitor(monitor *monitor);
void start_monitor(monitor *monitor);
void list_hunts(monitor *monitor);
void view_treasure(monitor *monitor);
void list_treasures(monitor *monitor);
void calculate_score(monitor *monitor);
monitor *getMainMonitor();

#endif