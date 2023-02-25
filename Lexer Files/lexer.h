typedef enum {keyword, identifier, symbols, eof, number, error} Tokentypes;


typedef struct {
    Tokentypes type;
    char lexeme[128];
} Token;