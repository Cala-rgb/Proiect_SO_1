#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

typedef struct {
    char id[25];
    char username[50];
    double latitude;
    double longitude;
    char clue[256];
    int value;
} Treasure;

void calculate_hunt_scores(const char* hunt_id) {
    char filename[256];
    snprintf(filename, sizeof(filename), "hunts/%s/treasure.bin", hunt_id);
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Could not open hunt file: %s\n", filename);
        return;
    }

    // Store user scores in a dynamic array
    struct UserScore {
        char username[50];
        int total_score;
    } *scores = NULL;
    int num_users = 0;

    Treasure treasure;
    while (fread(&treasure, sizeof(Treasure), 1, file) == 1) {
        // Find or add user
        int user_found = 0;
        for (int i = 0; i < num_users; i++) {
            if (strcmp(scores[i].username, treasure.username) == 0) {
                scores[i].total_score += treasure.value;
                user_found = 1;
                break;
            }
        }

        if (!user_found) {
            scores = realloc(scores, (num_users + 1) * sizeof(struct UserScore));
            strcpy(scores[num_users].username, treasure.username);
            scores[num_users].total_score = treasure.value;
            num_users++;
        }
    }

    // Print scores for this hunt
    printf("%d",num_users);
    printf("Scores for hunt %s:\n", hunt_id);
    for (int i = 0; i < num_users; i++) {
        printf("%s: %d points\n", scores[i].username, scores[i].total_score);
    }
    printf("\n");

    free(scores);
    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hunt_id>\n", argv[0]);
        return 1;
    }

    calculate_hunt_scores(argv[1]);
    return 0;
} 