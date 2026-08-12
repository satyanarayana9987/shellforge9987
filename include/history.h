#ifndef HISTORY_H
#define HISTORY_H

#define MAX_HISTORY 100

void add_history(const char *command);
void print_history(void);
void clear_history(void);

#endif
