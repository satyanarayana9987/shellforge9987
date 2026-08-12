
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"
#include "../include/history.h"

void print_tokens(Token *tokens, int count)
{
    printf("\n----------- TOKENS ------------\n");

    for (int i = 0; i < count; i++)
    {
        if (tokens[i].type == TOKEN_WORD)
        {
            printf("%d : WORD        %s\n", i, tokens[i].value);
        }
        else if (tokens[i].type == TOKEN_END)
        {
            printf("%d : END         %s\n", i, tokens[i].value);
        }
    }

    printf("-------------------------------\n");
}

int main()
{
    char input[1024];
    Token *tokens;
    int count;

    printf("================================\n");

    while (1)
    {
        printf("shellforge$ ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
        {
            break;
        }

        if (input[0] == '\0')
        {
            continue;
        }

        add_history(input);

        if (strcmp(input, "history") == 0)
        {
            print_history();
            continue;
        }

        tokens = tokenize(input, &count);

        print_tokens(tokens, count);

        for (int i = 0; i < count; i++)
        {
            free(tokens[i].value);
        }

        free(tokens);
    }

    clear_history();

    return 0;
}
