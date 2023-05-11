#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

// define your own types and function prototypes for the symbol table(s) module below

typedef enum {CHAR, BOOL, VOID, IDENTIFIER, FLAG} DataTypes;
typedef enum {STATIC, CLASS, FIELD, VAR, ARGUMENT, SUBROUTINE, THIS, UNKNOWN} Kinds;
typedef struct 
{
    char name[128];
    DataTypes type;
    Kinds kind;
    int flag;
    Token token;

} Symbol;

struct Scope
{
    int level;
    struct Scope *parent;
    int noOfSymbols;
    Symbol symbolTable[512];
    int noOfChildren;
    struct Scope *children[16];

};

void InitSymbolTable();

int symbolExists(char *name);

int symbolInScope(char *name);

void enterScope();

void exitScope();

void insertSymbol(char* name, DataTypes type, Kinds kind, Token token);

void setScope();

ParserInfo findFlags();

void printTree(struct Scope* root)

struct Scope* findRoot();




#endif
