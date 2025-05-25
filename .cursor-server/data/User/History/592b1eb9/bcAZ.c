#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For sleep
#include "interface.h"
#include "hub_commands.h"

// Helper function to clear input buffer
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void display_initial_menu(void) {
    clear_screen();
    printf("\n╔════════════════════════════════════╗\n");
    printf("║        Treasure Hunt System        ║\n");
    printf("╠════════════════════════════════════╣\n");
    printf("║  1. Start Monitor                  ║\n");
    printf("║  0. Exit                           ║\n");
    printf("╚════════════════════════════════════╝\n");
    printf("\nEnter your choice: ");
}

void display_main_menu(void) {
    clear_screen();
    printf("\n╔════════════════════════════════════╗\n");
    printf("║        Treasure Hunt System        ║\n");
    printf("╠════════════════════════════════════╣\n");
    printf("║  1. List All Hunts                 ║\n");
    printf("║  2. View Treasure                  ║\n");
    printf("║  3. List Treasures                 ║\n");
    printf("║  4. Calculate Score                ║\n");
    printf("║  5. Stop Monitor                   ║\n");
    printf("╚════════════════════════════════════╝\n");
    printf("\nEnter your choice: ");
}

void display_stopped_menu(void) {
    clear_screen();
    printf("\n╔════════════════════════════════════╗\n");
    printf("║        Treasure Hunt System        ║\n");
    printf("╠════════════════════════════════════╣\n");
    printf("║  1. Start Monitor                  ║\n");
    printf("║  0. Exit                           ║\n");
    printf("╚════════════════════════════════════╝\n");
    printf("\nEnter your choice: ");
}

int handle_menu(monitor_t *monitor) {
    int choice;
    char input[10];

    while (1) {
        // Display appropriate menu based on monitor status
        if (monitor->status == 0) {
            if (monitor->pid == -1) {
                display_initial_menu();
            } else {
                display_stopped_menu();
            }
        } else {
            display_main_menu();
        }

        // Get user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error reading input\n");
            continue;
        }

        // Convert input to integer
        choice = atoi(input);

        // Handle menu choices
        if (monitor->status == 0) {
            if (monitor->pid == -1) {
                // Initial menu (before monitor was ever started)
                switch (choice) {
                    case 1:
                        start_monitor(monitor);
                        break;
                    case 0:
                        return 0;  // Exit program
                    default:
                        printf("Invalid choice. Press Enter to continue...");
                        getchar();
                        break;
                }
            } else {
                // Stopped menu (after monitor was stopped)
                switch (choice) {
                    case 1:
                        start_monitor(monitor);
                        break;
                    case 0:
                        return 0;  // Exit program
                    default:
                        printf("Invalid choice. Press Enter to continue...");
                        getchar();
                        break;
                }
            }
        } else {
            // Main menu (after monitor starts)
            switch (choice) {
                case 1:
                    list_hunts_cmd(monitor);
                    printf("\nPress Enter to return to menu...");
                    getchar();
                    break;
                case 2:
                    view_treasure_cmd(monitor);
                    printf("\nPress Enter to return to menu...");
                    while (getchar() != '\n'); // Clear any extra input
                    getchar();
                    break;
                case 3:
                    list_treasures_cmd(monitor);
                    printf("\nPress Enter to return to menu...");
                    while (getchar() != '\n'); // Clear any extra input
                    getchar();
                    break;
                case 4:
                    calculate_score_cmd(monitor);
                    printf("\nPress Enter to return to menu...");
                    getchar();
                    break;
                case 5:
                    stop_monitor(monitor);
                    break;
                default:
                    printf("Invalid choice. Press Enter to continue...");
                    getchar();
                    break;
            }
        }
    }

    return 0;
} 