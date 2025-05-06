cmake_minimum_required(VERSION 3.10)

# Project name and language
project(ProjectSO C)

# All source files
set(SOURCES
    hub_commands.c
    treasure_hub.c
    handle_signals.c
)

# Create the executable
add_executable(hub ${SOURCES})
