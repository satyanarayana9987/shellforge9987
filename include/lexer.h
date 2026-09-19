#ifndef LEXER_H
#define LEXER_H

#include "token.h"

/*
 * Splits `input` into tokens (WORD, PIPE, INPUT, OUTPUT, APPEND,
 * BACKGROUND) and always finishes the list with an END token.
 * Returns 1 on success, 0 on error (message already printed).
 */
int lexer(const char *input, token_list_t *list);

#endif
