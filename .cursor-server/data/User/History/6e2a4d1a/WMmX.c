#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "interface_arrows.h"
#include "hub_commands.h"

#define ARROW_UP    65
#define ARROW_DOWN  66
#define ENTER       10
#define ESC        27
#define CLEAR_SCREEN() printf("\033[H\033[J")
#define MOVE_CURSOR(x, y) printf("\033[%d;%dH", (y), (x))
#define HIDE_CURSOR() printf("\033[?25l")
#define SHOW_CURSOR() printf("\033[?25h")

// ANSI color codes
#define COLOR_RESET   "\x1b[0m"
#define COLOR_SELECTED "\x1b[7m"  // Inverted colors for selection

static struct termios orig_termios;

// Function to disable raw mode
void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    SHOW_CURSOR();
}

// Function to enable raw mode
void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    HIDE_CURSOR();
}

// Function to temporarily restore normal terminal mode for input
void restore_normal_input() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    SHOW_CURSOR();
}

// Function to restore raw mode after input
void restore_raw_mode() {
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    HIDE_CURSOR();
}

// Function to read a single keypress
int read_key() {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == ESC) {
            char seq[3];
            if (read(STDIN_FILENO, &seq[0], 1) != 1) return ESC;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) return ESC;
            
            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                }
            }
        }
        return c;
    }
    return -1;
}

// Function to draw menu options
void draw_menu(const char *options[], int num_options, int selected, int start_y) {
    for (int i = 0; i < num_options; i++) {
        MOVE_CURSOR(5, start_y + i);
        if (i == selected) {
            printf("%s> %s%s", COLOR_SELECTED, options[i], COLOR_RESET);
        } else {
            printf("  %s", options[i]);
        }
        printf("\033[K");  // Clear to end of line
    }
    fflush(stdout);
}

// Function to wait for Enter key
void wait_for_enter() {
    printf("\n\nPress Enter to return to menu...");
    fflush(stdout);
    while (read_key() != ENTER);
}

// Function to handle the initial menu
int handle_initial_menu(monitor_t *monitor) {
    const char *options[] = {
        "Start Monitor",
        "Exit"
    };
    int num_options = sizeof(options) / sizeof(options[0]);
    int selected = 0;

    while (1) {
        CLEAR_SCREEN();
        printf("\n\n   Welcome to Treasure Hunt System\n\n");
        draw_menu(options, num_options, selected, 5);

        int c = read_key();
        switch (c) {
            case ARROW_UP:
                if (selected > 0) selected--;
                break;
            case ARROW_DOWN:
                if (selected < num_options - 1) selected++;
                break;
            case ENTER:
                if (selected == 0) {
                    start_monitor(monitor);
                    return 1;  // Go to main menu
                } else {
                    return 0;  // Exit
                }
        }
    }
}

// Function to handle the stopped menu
int handle_stopped_menu(monitor_t *monitor) {
    const char *options[] = {
        "Start Monitor",
        "Exit"
    };
    int num_options = sizeof(options) / sizeof(options[0]);
    int selected = 0;

    while (1) {
        CLEAR_SCREEN();
        printf("\n\n   Treasure Hunt System - Monitor Stopped\n\n");
        draw_menu(options, num_options, selected, 5);

        int c = read_key();
        switch (c) {
            case ARROW_UP:
                if (selected > 0) selected--;
                break;
            case ARROW_DOWN:
                if (selected < num_options - 1) selected++;
                break;
            case ENTER:
                if (selected == 0) {
                    start_monitor(monitor);
                    return 1;  // Go to main menu
                } else {
                    return 0;  // Exit
                }
        }
    }
}

// Function to handle command execution
void execute_command(monitor_t *monitor, void (*cmd_func)(monitor_t*)) {
    CLEAR_SCREEN();
    restore_normal_input();  // Restore normal terminal mode for input
    cmd_func(monitor);
    wait_for_enter();
    restore_raw_mode();  // Restore raw mode for menu navigation
}

// Function to handle the main menu
int handle_main_menu(monitor_t *monitor) {
    const char *options[] = {
        "List Hunts",
        "List Treasures",
        "View Treasure",
        "Calculate Score",
        "Stop Monitor"
    };
    int num_options = sizeof(options) / sizeof(options[0]);
    int selected = 0;

    while (1) {
        CLEAR_SCREEN();
        printf("\n\n   Treasure Hunt System - Main Menu\n\n");
        draw_menu(options, num_options, selected, 5);

        int c = read_key();
        switch (c) {
            case ARROW_UP:
                if (selected > 0) selected--;
                break;
            case ARROW_DOWN:
                if (selected < num_options - 1) selected++;
                break;
            case ENTER:
                switch (selected) {
                    case 0:  // List Hunts
                        execute_command(monitor, list_hunts_cmd);
                        break;
                    case 1:  // List Treasures
                        execute_command(monitor, list_treasures_cmd);
                        break;
                    case 2:  // View Treasure
                        execute_command(monitor, view_treasure_cmd);
                        break;
                    case 3:  // Calculate Score
                        execute_command(monitor, calculate_score_cmd);
                        break;
                    case 4:  // Stop Monitor
                        CLEAR_SCREEN();
                        stop_monitor(monitor);
                        wait_for_enter();
                        return 1;  // Go back to initial menu
                }
                break;
        }
    }
}

// Main interface handler
int handle_menu_arrows(monitor_t *monitor) {
    enable_raw_mode();
    
    while (1) {
        int result;
        if (monitor->status != 1) {
            if (monitor->pid == -1) {
                result = handle_initial_menu(monitor);
            } else {
                result = handle_stopped_menu(monitor);
            }
            if (result == 0) break;  // Exit selected
        } else {
            result = handle_main_menu(monitor);
            if (result == 0) break;  // Exit selected
        }
    }

    disable_raw_mode();
    printf("\nExiting...\n");
    return 0;
} 