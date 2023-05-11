
/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Symbol Tables Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Darragh Connolly
Student ID: 012533682
Email: sc21dc@leeds.ac.uk
Date Work Commenced:
*************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "symbols.h"


// How to - Make an array, each element will be a variable of type struct, 
// each one will have fields like string of ID, e.t.c, then a pointer to further data

// Could also do this as a linked list that can grow dynamically or as an array with x amount of memorys

struct Scope *currentScope;

// Function to initialise the Symbol table by setting the current scope to NULL
void InitSymbolTable()
{
    currentScope = malloc(sizeof(struct Scope));
    currentScope->level = 0;
    currentScope->noOfChildren = 0;
    for(int i=0; i<16; i++)
    {
        currentScope->children[i] = NULL;
    }
    currentScope->parent = NULL;
    currentScope->noOfSymbols = 0;
    
}
// Function to add a symbol to the table
void insertSymbol(char* name, DataTypes type, Kinds kind, Token token)
{

    if(currentScope->noOfSymbols >= 512)
    {
        printf("Error: Symbol Table Full\n");
        exit(0);
    }
    strcpy(currentScope->symbolTable[currentScope->noOfSymbols].name, name);
    currentScope->symbolTable[currentScope->noOfSymbols].type = type;

    currentScope->symbolTable[currentScope->noOfSymbols].kind = kind;

    if (type == FLAG)
    {
        currentScope->symbolTable[currentScope->noOfSymbols].flag = 1;
    }
    

    currentScope->symbolTable[currentScope->noOfSymbols].token = token;
    currentScope->noOfSymbols++;
}


// Function starts at the current scope and searches through
// the current and parents copes to see if the variable exists
// useful for checking if a Identifier can be used
int symbolExists(char *name)
{
    struct Scope *temp = currentScope;
    while(temp != NULL)
    {
        for(int i=0; i<temp->noOfSymbols; i++)
        {
            if(!strcmp(temp->symbolTable[i].name, name))
            {
                return i;
            }
        }
        temp = temp->parent;
    }

    return -1;
}

// Function to check specifically the current scope
// useful for adding something to the same scope
int symbolInScope(char *name)
{
    for(int i = 0; i <currentScope->noOfSymbols; i++)
    {
        if(!strcmp(currentScope->symbolTable[i].name, name))
            {
                return i;
            }
    }

    return -1;
}

// Function to create a new table for entering a new scope
void enterScope()
{
    // allocates memory for a new Scope struct
    struct Scope *newScope = malloc(sizeof(struct Scope));

    // checks if it is the first scope or not
    if(currentScope == NULL)
    {
        newScope->level = 0;
    }
    else
    {
        newScope->level = currentScope->level + 1;
    }

    // sets the members of the new scope
    newScope->parent = currentScope;
    newScope->noOfSymbols = 0;

    // lets the parent scope point to the new scope
    // and changes the current scope to this new one
    currentScope->children[currentScope->noOfSymbols] = newScope;
    currentScope->noOfChildren++;
    currentScope = newScope;
}


// Function to allow us to move out of the current scope to the one above it
void exitScope()
{
    currentScope = currentScope->parent;
}

void freeTable(struct Scope *scope)
{
    if(scope == NULL)
    {
        return;
    }
    for(int i=0; i<16; i++)
    {
        freeTable(scope->children[i]);
    }
    free(scope);
}


void setScope()
{
    if (currentScope == NULL);
    {
        return;
    }
    while(currentScope->level != 0)
    {
        currentScope = currentScope->parent;
    }
    freeTable(currentScope->children[0]);

}

int checkFlags(Token token, int level)
{
    if(level == 0)
    {
        return -1;
    }

    for(int i = level; i > 0; i--)
    {
        currentScope = currentScope->parent;
        for (int j = 0; j < currentScope->noOfSymbols; j++)
        {
            if(!strcmp(currentScope->symbolTable[j].name, token.lx) && currentScope->symbolTable[j].type == CLASS)
            {
                return 1;
            }
            if(!strcmp(currentScope->symbolTable[j].name, token.lx) && currentScope->symbolTable[j].type == SUBROUTINE)
            {
                return 0;
            }
        }
        
    }
    
    return 0;
}

ParserInfo findFlags()
{
    ParserInfo pi;
    struct Scope *temp = currentScope;
    printf("Finding Flags\n");
    for (int i = 0; i < currentScope->noOfSymbols; i++)
    {
        if(currentScope->symbolTable[i].flag == 1)
        {
            printf("Flagged: %s\n", currentScope->symbolTable[i].name);
            int issue = checkFlags(currentScope->symbolTable[i].token, currentScope->level);

            if(issue == -1)
            {
                pi.er = undecIdentifier;
                pi.tk = currentScope->symbolTable[i].token;
                return pi;
            }
            else if(issue == 1)
            {
                currentScope->symbolTable[i].flag = 0;
                currentScope->symbolTable[i].type = IDENTIFIER;
            }
            else if(issue == 0)
            {
                currentScope->symbolTable[i].flag = 0;
                currentScope->symbolTable[i].type = SUBROUTINE;
            }
        }
    }

    pi.er = none;
    return pi;
    
}

void printTreeHelper(struct Scope* node, int level) {
    if (node == NULL) {
        return;
    }

    for(int i = 0; i < node->noOfChildren; i++) {
        printTreeHelper(node->children[i], level + 1);
    }
    for (int i = 0; i < level; i++) {
        printf("  "); // print indentation based on level
    }
    for (int i = 0; i < node->noOfSymbols; i++) {
        printf("%s ", node->symbolTable[i].name); // print array elements
    }
    printf("]\n");
}

void printTree(struct Scope* root) 
{
    printTreeHelper(root, 0);
}

struct Scope* findRoot()
{
    struct Scope *temp = currentScope;
    while(temp->level != 0)
    {
        temp = temp->parent;
    }
}



