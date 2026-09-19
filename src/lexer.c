#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

/* Room is always kept for the final END token. */
static int add_token(token_list_t *list, token_type_t type, const char *text)
{
    if (list->count >= MAX_TOKENS - 1) {
        printf("Error: too many tokens (maximum is %d)\n", MAX_TOKENS - 1);
        return 0;
    }
    token_add(list, type, text);
    return 1;
}

int lexer(const char *input, token_list_t *list)
{
    const char *p = input;

    token_list_init(list);

    while (*p != '\0') {

        if (isspace((unsigned char)*p)) {
            p++;
            continue;
        }

        if (*p == '|') {
            if (!add_token(list, TOKEN_PIPE, "|")) return 0;
            p++;
        } else if (*p == '<') {
            if (!add_token(list, TOKEN_INPUT, "<")) return 0;
            p++;
        } else if (*p == '>') {
            if (p[1] == '>') {
                if (!add_token(list, TOKEN_APPEND, ">>")) return 0;
                p += 2;
            } else {
                if (!add_token(list, TOKEN_OUTPUT, ">")) return 0;
                p++;
            }
        } else if (*p == '&') {
            if (!add_token(list, TOKEN_BACKGROUND, "&")) return 0;
            p++;
        } else {
            /* A word: runs until whitespace or an operator character.
               Text inside 'single' or "double" quotes is kept together. */
            char buffer[MAX_TOKEN_LEN];
            int len = 0;
            char quote = 0;

            while (*p != '\0') {
                if (quote) {
                    if (*p == quote) {
                        quote = 0;
                        p++;
                        continue;
                    }
                } else {
                    if (isspace((unsigned char)*p) || strchr("|<>&", *p) != NULL) {
                        break;
                    }
                    if (*p == '"' || *p == '\'') {
                        quote = *p;
                        p++;
                        continue;
                    }
                }

                if (len >= MAX_TOKEN_LEN - 1) {
                    printf("Error: word too long (maximum is %d characters)\n", MAX_TOKEN_LEN - 1);
                    return 0;
                }
                buffer[len++] = *p++;
            }

            if (quote) {
                printf("Error: unterminated quote\n");
                return 0;
            }

            buffer[len] = '\0';
            if (!add_token(list, TOKEN_WORD, buffer)) return 0;
        }
    }

    token_add(list, TOKEN_END, "END");
    return 1;
}
