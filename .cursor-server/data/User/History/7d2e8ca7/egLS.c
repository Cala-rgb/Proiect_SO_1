#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "commands.h"
#include "common.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: treasure_manager --opt\n");
        return ERR_INVALID_ARGS;
    }

    error_code_t result = SUCCESS;

    if (strcmp(argv[1], "--add") == 0) {
        if (argc < 3) {
            printf("Usage: --add <hunt_id>\n");
            return ERR_INVALID_ARGS;
        }

        char path[PATH_SIZE], huntspath[PATH_SIZE];
        snprintf(path, sizeof(path), "hunts/%s", argv[2]);
        strcpy(huntspath, "hunts");

        struct stat path_stat;
        if (stat(path, &path_stat) == 0) {
            if (S_ISDIR(path_stat.st_mode)) {
                result = create_treasure(argv[2]);
            }
        } else if (stat(huntspath, &path_stat) == 0) {
            if (S_ISDIR(path_stat.st_mode)) {
                if (mkdir(path, 0777) == 0) {
                    if (create_bin_file(argv[2]) == SUCCESS && create_log_file(argv[2]) == SUCCESS) {
                        result = create_treasure(argv[2]);
                    } else {
                        update_log_file(argv[2], "--add", "", 1);
                        result = ERR_FILE_CREATE;
                    }
                } else {
                    result = ERR_DIR_CREATE;
                }
            }
        } else {
            if (mkdir("hunts", 0777) == 0 && mkdir(path, 0777) == 0) {
                if (create_bin_file(argv[2]) == SUCCESS && create_log_file(argv[2]) == SUCCESS) {
                    result = create_treasure(argv[2]);
                } else {
                    update_log_file(argv[2], "--add", "", 1);
                    result = ERR_FILE_CREATE;
                }
            } else {
                result = ERR_DIR_CREATE;
            }
        }
    } else if (strcmp(argv[1], "--list") == 0) {
        if (argc < 3) {
            printf("Usage: --list <hunt_id>\n");
            return ERR_INVALID_ARGS;
        }

        char path[PATH_SIZE];
        snprintf(path, sizeof(path), "hunts/%s", argv[2]);
        struct stat path_stat;
        if (stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
            result = list_treasures(argv[2]);
        } else {
            result = ERR_DIR_NOT_FOUND;
        }
    } else if (strcmp(argv[1], "--view") == 0) {
        if (argc < 4) {
            printf("Usage: --view <hunt_id> <treasure_id>\n");
            return ERR_INVALID_ARGS;
        }
        result = view_treasure(argv[2], argv[3]);
    } else if (strcmp(argv[1], "--remove_treasure") == 0) {
        if (argc < 4) {
            printf("Usage: --remove_treasure <hunt_id> <treasure_id>\n");
            return ERR_INVALID_ARGS;
        }

        char path[PATH_SIZE];
        snprintf(path, sizeof(path), "hunts/%s", argv[2]);
        struct stat path_stat;
        if (stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
            result = delete_treasure(argv[2], argv[3]);
        } else {
            update_log_file(argv[2], "--remove_treasure", "", 1);
            result = ERR_DIR_NOT_FOUND;
        }
    } else if (strcmp(argv[1], "--remove_hunt") == 0) {
        if (argc < 3) {
            printf("Usage: --remove_hunt <hunt_id>\n");
            return ERR_INVALID_ARGS;
        }

        char path[PATH_SIZE];
        snprintf(path, sizeof(path), "hunts/%s", argv[2]);
        struct stat path_stat;
        if (stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
            result = delete_hunt(argv[2]);
        } else {
            update_log_file(argv[2], "--remove_hunt", "", 1);
            result = ERR_DIR_NOT_FOUND;
        }
    } else if (strcmp(argv[1], "--list_hunts") == 0) {
        result = list_hunts("hunts");
    } else {
        printf("%s is not an existing command!\n", argv[1]);
        result = ERR_INVALID_ARGS;
    }

    if (result != SUCCESS) {
        printf("Error: %s\n", error_to_string(result));
    }
    return result;
}