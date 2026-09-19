#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/token.h"

void print_token(Token token)
{
    printf("Token Type: %d, Value: %s\n",
           token.type,
           token.value);
}

void free_token(Token *token)
{
    if (token->value != NULL)
    {
        free(token->value);
        token->value = NULL;
    }
}
