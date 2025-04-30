#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void start_monitor() {



}

int main(int argc, char *argv[])
{
 
    if (argc > 1)
    {
        if (strcmp(argv[1], "--start_monitor") == 0)
        {
            int pid = fork();
            if(pid==0) {

            } else if(pid>0) {

            }
            printf("Started monitor!\n");
        }
        else if (strcmp(argv[1], "--list_hunts") == 0)
        {
            printf("Listed hunts!");
        }
        else if (strcmp(argv[1], "--list_treasures") == 0)
        {
            if(argc > 2) {
                printf("Listed treasures of hunt %s\n",argv[2]);
            } else {
                printf("Usage: --list_treasures <hunt_id>\n");
            }
        }
        else if (strcmp(argv[1], "--view_treasure") == 0)
        {
            if(argc>3) {
                printf("Viewed treasure %s of hunt %s\n",argv[3],argv[2]);
            } else {
                printf("Usage: --view_treasure <hunt_id> <treasure_id>\n");
            }
        }
        else if (strcmp(argv[1], "--stop_monitor") == 0)
        {
            printf("Stopped monitor!\n");
        }else if (strcmp(argv[1], "--exit") == 0)
        {
            printf("Exited!");
        } else {
            printf("%s is not an existing command!\n",argv[1]);
        }
    }
    else
    {
        printf("Usage: treasure_manager --opt\n");
    }
}