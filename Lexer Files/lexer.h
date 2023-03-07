typedef enum {keyword, identifier, symbol, eof, number, error} Tokentypes;

const char* keywords[21] = {"class", "constructor", "static", "void", "method", "function", "field", "var", "int", "char",
                                "boolean", "let", "if", "while", "do", "return", "true", "false", "else", "null", "this"};

int symbols[19] = {'(', ')', '[', ']', '{', '}', ',', ';', '=', '.', '+', '-', '*', '/', '&', '|', '~', '<', '>'}; 


typedef struct {
    Tokentypes type;
    char lexeme[128];
} Token;