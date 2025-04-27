#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "commands.h"
 
typedef struct
{
    char id[25];
    char user[25];
    double lat, lon;
    char clue[25];
    int value;
} treasure;







int main(int argc, char *argv[])
{
 
    if (argc > 1)
    {
        if (strcmp(argv[1], "--add") == 0)
        {
            struct stat path_stat;
            if (stat(argv[2], &path_stat) == 0)
            {
                if (S_ISDIR(path_stat.st_mode))
                {
                    createTreasure(argv[2]);
                }
            }
            else
            {
                mkdir(argv[2], 0777);
                if(createBinFile(argv[2]) && createLogFile(argv[2]))
                    createTreasure(argv[2]);
                else
                    updateLogFile(argv[2],"--add","",1);
            }
        }
        else if (strcmp(argv[1], "--list") == 0)
        {
            if(argc>2) {
                struct stat path_stat;
                if (stat(argv[2], &path_stat) == 0)
                {
                    if (S_ISDIR(path_stat.st_mode))
                    {
                        list(argv[2]);
                    }
                }
            } else {
                printf("Usage: --list <hunt_id>\n");
            }
        }
        else if (strcmp(argv[1], "--view") == 0)
        {
            if(argc > 3) {
                view_treasure(argv[2], argv[3]);
            } else {
                printf("Usage: --view <hunt_id> <treasure_id>\n");
            }
        }
        else if (strcmp(argv[1], "--remove_treasure") == 0)
        {
            if(argc>3) {
                struct stat path_stat;
                if (stat(argv[2], &path_stat) == 0)
                {
                    if (S_ISDIR(path_stat.st_mode))
                    {
                        delete_treasure(argv[2],argv[3]);
                    } else {
                        updateLogFile(argv[2],"--remove_treasure","",1);
                        printf("Directory does not exist\n");
                    }
                }
            } else {
                printf("Usage: --remove_treasure <hunt_id> <treasure_id>\n");
            }
        }
        else if (strcmp(argv[1], "--remove_hunt") == 0)
        {
            if(argc>2) {
                struct stat path_stat;
                if (stat(argv[2], &path_stat) == 0)
                {
                    if (S_ISDIR(path_stat.st_mode))
                    {
                        delete_dir(argv[2]);
                    } else {
                        updateLogFile(argv[2],"--remove_hunt","",1);
                        printf("Directory does not exist\n");
                    }
                }
            } else {
                printf("Usage: --remove_hunt <hunt_id>\n");
            }
        } else {
            printf("%s is not an existing command!\n",argv[1]);
        }
    }
    else
    {
        printf("Usage: treasure_manager --opt\n");
    }
}