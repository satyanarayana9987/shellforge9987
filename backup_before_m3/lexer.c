#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"

Token *tokenize(const char *input, int *count)
{
    Token *tokens;
    char *copy;
    char *word;
    int capacity = 10;

    *count = 0;

    copy = strdup(input);

    tokens = malloc(capacity * sizeof(Token));

    word = strtok(copy, " \t\n");

    while (word != NULL)
    {
        if (*count >= capacity - 1)
        {
            capacity *= 2;
            tokens = realloc(tokens, capacity * sizeof(Token));
        }

        tokens[*count].type = TOKEN_WORD;
        tokens[*count].value = strdup(word);
        (*count)++;

        word = strtok(NULL, " \t\n");
    }

    /* Add END token */
    tokens[*count].type = TOKEN_END;
    tokens[*count].value = strdup("END");
    (*count)++;

    free(copy);

    return tokens;
}
