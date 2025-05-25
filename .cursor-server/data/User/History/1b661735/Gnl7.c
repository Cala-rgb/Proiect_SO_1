#include "common.h"

const char* error_to_string(error_code_t error) {
    switch (error) {
        case SUCCESS:
            return "Success";
        case ERR_FILE_OPEN:
            return "Failed to open file";
        case ERR_FILE_READ:
            return "Failed to read file";
        case ERR_FILE_WRITE:
            return "Failed to write file";
        case ERR_PIPE_CREATE:
            return "Failed to create pipe";
        case ERR_FORK_FAILED:
            return "Failed to fork process";
        case ERR_EXEC_FAILED:
            return "Failed to execute command";
        case ERR_INVALID_ARGS:
            return "Invalid arguments";
        case ERR_DIR_NOT_FOUND:
            return "Directory not found";
        default:
            return "Unknown error";
    }
} 