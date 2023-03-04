typedef enum {keyword, identifier, symbols, eof, number, error} Tokentypes;

const char* keywords[17] = {"class", "constructor", "static", "void", "method", "function", "field", "var", "int", "String", 
                                "char", "boolean", "let", "if", "while", "do", "return"};


typedef struct {
    Tokentypes type;
    char lexeme[128];
} Token;