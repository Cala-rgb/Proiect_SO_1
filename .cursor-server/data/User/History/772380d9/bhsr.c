#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "common.h"
#include "config.h"

typedef struct {
    char username[25];
    int total_score;
} user_score_t;

error_code_t calculate_hunt_scores(const char* hunt_id) {
    if (!hunt_id) {
        return ERR_INVALID_ARGS;
    }

    char filename[PATH_SIZE];
    snprintf(filename, sizeof(filename), TREASURE_PATH_FORMAT, hunt_id);
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return ERR_FILE_OPEN;
    }

    user_score_t* scores = NULL;
    int num_users = 0;
    treasure_t treasure;

    while (fread(&treasure, sizeof(treasure_t), 1, file) == 1) {
        // Find or add user
        int user_found = 0;
        for (int i = 0; i < num_users; i++) {
            if (strcmp(scores[i].username, treasure.user) == 0) {
                scores[i].total_score += treasure.value;
                user_found = 1;
                break;
            }
        }

        if (!user_found) {
            user_score_t* new_scores = realloc(scores, (num_users + 1) * sizeof(user_score_t));
            if (!new_scores) {
                free(scores);
                fclose(file);
                return ERR_FILE_WRITE;
            }
            scores = new_scores;
            strncpy(scores[num_users].username, treasure.user, sizeof(scores[num_users].username) - 1);
            scores[num_users].username[sizeof(scores[num_users].username) - 1] = '\0';
            scores[num_users].total_score = treasure.value;
            num_users++;
        }
    }

    // Print scores for this hunt
    printf("Scores for hunt %s:\n", hunt_id);
    for (int i = 0; i < num_users; i++) {
        printf("%s: %d points\n", scores[i].username, scores[i].total_score);
    }
    printf("\n");

    free(scores);
    fclose(file);
    return SUCCESS;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hunt_id>\n", argv[0]);
        return ERR_INVALID_ARGS;
    }

    error_code_t result = calculate_hunt_scores(argv[1]);
    if (result != SUCCESS) {
        fprintf(stderr, "Error calculating scores: %s\n", error_to_string(result));
        return result;
    }

    return SUCCESS;
} 