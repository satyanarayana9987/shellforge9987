#include <stdio.h>
#include <string.h>
#include "../include/history.h"

static char history[MAX_HISTORY][1024];
static int history_count = 0;

void add_history(const char *command)
{
    if (history_count < MAX_HISTORY)
    {
        strncpy(history[history_count], command, 1023);
        history[history_count][1023] = '\0';
        history_count++;
    }
}

void print_history(void)
{
    for (int i = 0; i < history_count; i++)
    {
        printf("%d  %s\n", i + 1, history[i]);
    }
}

void clear_history(void)
{
    history_count = 0;
}
