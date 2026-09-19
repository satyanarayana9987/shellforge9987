#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "expand.h"
#include "builtin.h"
#include "executor.h"

int main(void) {
    token_list_t tokens;
    pipeline_t pipeline;
    char *line;
    int should_exit = 0;

    /* set SHELLFORGE_QUIET=1 to hide the TOKENS / PIPELINE debug output */
    int quiet = (getenv("SHELLFORGE_QUIET") != NULL);

    printf("========================================\n");
    printf("    Shellforge\n");
    printf(" A Unix Style Shell written in C\n");
    printf("========================================\n");

    while (1) {
        line = readline("shellforge$ ");

        if (line == NULL) {
            printf("\nGoodbye!\n");
            break;
        }

        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        add_history(line);

        if (lexer(line, &tokens)) {
            if (!quiet) {
                token_print(&tokens);
            }

            if (parse(&tokens, &pipeline)) {
                expand_variables(&pipeline);

                if (!quiet) {
                    pipeline_print(&pipeline);
                }

                if (execute_pipeline(&pipeline) == 1) {
                    should_exit = 1;
                }
            }
        }

        free(line);

        if (should_exit) {
            break;
        }
    }

    return 0;
}
