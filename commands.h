#ifndef COMMANDS
#define COMMANDS

// Function prototype

void updateLogFile(char hunt[], char op[], char tid[], int failed) ;

void createTreasure(char *hunt);

void view_treasure(char hunt_id[], char treasure_id[]);

void list(char hunt_id[]);

void delete_dir(char hunt_id[]);

void delete_treasure(char hunt_id[], char treasure_id[]);

int createBinFile(char dir[]);

int createLogFile(char dir[]);

#endif