#ifndef CONFIG_H
#define CONFIG_H

// Base directories
#define BASE_DIR "."  // Change this to your base directory path
#define HUNTS_DIR BASE_DIR "/hunts"

// File paths
#define COMMAND_FILE BASE_DIR "/given_command.txt"
#define TREASURE_FILE "treasure.bin"
#define LOG_FILE "logged_hunt.txt"

// Executables
#define TREASURE_MANAGER BASE_DIR "/tm"
#define SCORE_CALCULATOR BASE_DIR "/sc"

// Log file prefix
#define LOG_FILE_PREFIX "logged_hunt_"

// Path format strings
#define HUNT_PATH_FORMAT HUNTS_DIR "/%s"
#define TREASURE_PATH_FORMAT HUNTS_DIR "/%s/" TREASURE_FILE
#define LOG_PATH_FORMAT HUNTS_DIR "/%s/" LOG_FILE
#define ROOT_LOG_PATH_FORMAT HUNTS_DIR "/" LOG_FILE_PREFIX "%s"

#endif // CONFIG_H 