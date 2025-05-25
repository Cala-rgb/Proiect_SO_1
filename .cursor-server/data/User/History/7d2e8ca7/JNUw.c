#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "commands.h"
#include "common.h"
#include "config.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: treasure_manager --opt\n");
        return 1;
    }

    int result = 0;

    if (strcmp(argv[1], "--add") == 0) {
        if (argc < 3) {
            printf("Usage: --add <hunt_id>\n");
            return 1;
        }

        char path[PATH_SIZE], huntspath[PATH_SIZE];
        snprintf(path, sizeof(path), HUNT_PATH_FORMAT, argv[2]);
        strcpy(huntspath, HUNTS_DIR);

        struct stat path_stat;
        if (stat(path, &path_stat) == 0) {
            if (S_ISDIR(path_stat.st_mode)) {
                create_treasure(argv[2]);
            }
        } else if (stat(huntspath, &path_stat) == 0) {
            if (S_ISDIR(path_stat.st_mode)) {
                if (mkdir(path, 0777) == 0) {
                    if (create_bin_file(argv[2]) && create_log_file(argv[2])) {
                        create_treasure(argv[2]);
                    } else {
                        update_log_file(argv[2], "--add", "", 1);
                        printf("Error: Failed to create files\n");
                        result = 1;
                    }
                } else {
                    printf("Error: Failed to create directory\n");
                    result = 1;
                }
            }
        } else {
            if (mkdir(HUNTS_DIR, 0777) == 0 && mkdir(path, 0777) == 0) {
                if (create_bin_file(argv[2]) && create_log_file(argv[2])) {
                    create_treasure(argv[2]);
                } else {
                    update_log_file(argv[2], "--add", "", 1);
                    printf("Error: Failed to create files\n");
                    result = 1;
                }
            } else {
                printf("Error: Failed to create directories\n");
                result = 1;
            }
        }
    } else if (strcmp(argv[1], "--list") == 0) {
        if (argc < 3) {
            printf("Usage: --list <hunt_id>\n");
            return 1;
        }

        char path[PATH_SIZE];
        snprintf(path, sizeof(path), HUNT_PATH_FORMAT, argv[2]);
        struct stat path_stat;
        if (stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
            list_treasures(argv[2]);
        } else {
            printf("Error: Directory not found\n");
            result = 1;
        }
    } else if (strcmp(argv[1], "--view") == 0) {
        if (argc < 4) {
            printf("Usage: --view <hunt_id> <treasure_id>\n");
            return 1;
        }
        view_treasure(argv[2], argv[3]);
    } else if (strcmp(argv[1], "--remove_treasure") == 0) {
        if (argc < 4) {
            printf("Usage: --remove_treasure <hunt_id> <treasure_id>\n");
            return 1;
        }

        char path[PATH_SIZE];
        snprintf(path, sizeof(path), HUNT_PATH_FORMAT, argv[2]);
        struct stat path_stat;
        if (stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
            delete_treasure(argv[2], argv[3]);
        } else {
            update_log_file(argv[2], "--remove_treasure", "", 1);
            printf("Error: Directory not found\n");
            result = 1;
        }
    } else if (strcmp(argv[1], "--remove_hunt") == 0) {
        if (argc < 3) {
            printf("Usage: --remove_hunt <hunt_id>\n");
            return 1;
        }

        char path[PATH_SIZE];
        snprintf(path, sizeof(path), HUNT_PATH_FORMAT, argv[2]);
        struct stat path_stat;
        if (stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
            delete_hunt(argv[2]);
        } else {
            update_log_file(argv[2], "--remove_hunt", "", 1);
            printf("Error: Directory not found\n");
            result = 1;
        }
    } else if (strcmp(argv[1], "--list_hunts") == 0) {
        list_hunts(HUNTS_DIR);
    } else {
        printf("%s is not an existing command!\n", argv[1]);
        result = 1;
    }

    return result;
}