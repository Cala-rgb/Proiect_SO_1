#ifndef INTERFACE_H
#define INTERFACE_H

#include "common.h"

// Function to clear the input buffer
void clear_input_buffer(void);

// Function to clear the screen
void clear_screen(void);

// Function to display the initial menu (before monitor starts)
void display_initial_menu(void);

// Function to display the main menu (after monitor starts)
void display_main_menu(void);

// Function to display the stopped menu
void display_stopped_menu(void);

// Function to handle user input and menu navigation
int handle_menu(monitor_t *monitor);

#endif // INTERFACE_H 