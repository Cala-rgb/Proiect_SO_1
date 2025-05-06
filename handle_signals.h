#ifndef HANDLER
#define HANDLER

void handle_sigterm();
void handle_sigchld();
void handle_usr();
void sigterm_handler(int sig);
void sigchld_handler(int sig);
void usr_handler(int sig);

#endif