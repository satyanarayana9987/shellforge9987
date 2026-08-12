  #ifndef TOKEN_H
#define TOKEN_H

typedef enum
{
    TOKEN_WORD,
    TOKEN_END
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
} Token;

#endif
