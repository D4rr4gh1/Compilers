#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "symbols.h"


// you can declare prototypes of parser functions below

ParserInfo classDeclar();
ParserInfo memberDeclar();
ParserInfo classVarDeclar();
ParserInfo type(DataTypes *type);
ParserInfo subroutineDeclar();
ParserInfo paramList();
ParserInfo subroutineBody();
ParserInfo statement();
ParserInfo varDeclarStatement();
ParserInfo letStatement();
ParserInfo ifStatement();
ParserInfo whileStatement();
ParserInfo doStatement();
ParserInfo subroutineCall();
ParserInfo expressionList();
ParserInfo returnStatement();
ParserInfo expression();
ParserInfo relationalExpression();
ParserInfo arithmeticExpression();
ParserInfo term();
ParserInfo factor();
ParserInfo operand();

ParserInfo pi;

int InitParser (char* file_name)
{   
	if(InitLexer(file_name) == 0)
	{
		printf("Error: File did not open correctly\n");
		return 0;
	}

	return 1;
}

ParserInfo classDeclar()
{   
    Token t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == RESWORD && !strcmp(t.lx, "class"))
    {
        ;
    }
    else
    {
        printf("class expected error\n");
        pi.er = classExpected;
        pi.tk = t;
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == ID)
    {
        int found = symbolInScope(t.lx);
        if(found != -1)
        {
            pi.er = redecIdentifier;
            pi.tk = t;
            return pi;
        }
        else
        {
            insertSymbol(t.lx, CLASS, CLASS, t);
        }

    }
    else
    {
        pi.er = idExpected;
        pi.tk = t;
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    if(t.tp == SYMBOL && !strcmp(t.lx,"{"))
    {
        ;
    }
    else
    {
        pi.er = openBraceExpected;
        pi.tk = t;
        return pi;
    }

    t = PeekNextToken();

    enterScope();
    
    while(t.tp != EOFile && strcmp(t.lx,"}"))
    {
        pi = memberDeclar();
        if(pi.er != none)
        {
            return pi;
        }
        t = PeekNextToken();
        
    }

    exitScope();

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == SYMBOL && !strcmp(t.lx,"}"))
    {
        ;
    }
    else
    {
        pi.er = closeBraceExpected;
        pi.tk = t;
        return pi;
    }

    pi.er = none;
    return pi;

}

ParserInfo memberDeclar()
{
    Token t = PeekNextToken();
    
    if(t.tp == RESWORD && (!strcmp(t.lx, "static") || !strcmp(t.lx, "field")))
    {
        pi = classVarDeclar();
        if(pi.er != none)
        {
            return pi;
        }
    }
    else if(t.tp == RESWORD && (!strcmp(t.lx, "constructor") || !strcmp(t.lx, "function") || !strcmp(t.lx, "method")))
    {
        pi = subroutineDeclar();
        if(pi.er != none)
        {
            return pi;
        }
    }
    else{
        pi.er = memberDeclarErr;
        pi.tk = t;
        return pi;
    }

    pi.er = none;
    return pi;
}

ParserInfo classVarDeclar()
{
    Token t = GetNextToken();
    Kinds tempKind;
    DataTypes symbolType;
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    if(!strcmp(t.lx, "static"))
    {
        tempKind = STATIC;
    }
    else if(!strcmp(t.lx, "field"))
    {
        tempKind = FIELD;
    }
    else
    {
        pi.er = classVarErr;
        pi.tk = t;
        return pi;
    }

    pi = type(&symbolType);
    if(pi.er != none)
    {
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == ID)
    {
        int found = symbolInScope(t.lx);
        if(found != -1)
        {
            pi.er = redecIdentifier;
            pi.tk = t;
            return pi;
        }
        else
        {   
            insertSymbol(t.lx, symbolType, tempKind, t);
        }
    }
    else
    {
        pi.er = idExpected;
        pi.tk = t;
        return pi;
    }

    t = PeekNextToken();
    
    while(!strcmp(t.lx, ",")){
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

        
        if(t.tp != ID)
        {
            pi.er = idExpected;
            pi.tk = t;
            return pi;
        }

        int found = symbolInScope(t.lx);
        if(found != -1)
        {
            pi.er = redecIdentifier;
            pi.tk = t;
            return pi;
        }
        else
        {
            insertSymbol(t.lx, symbolType, tempKind, t);
        }

        t = PeekNextToken();
        
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    
    if(t.tp == SYMBOL && !strcmp(t.lx, ";"))
    {
        ;
    }
    else
    {
        pi.er = semicolonExpected;
        pi.tk = t;
        return pi;
    }

    pi.er = none;
    return pi;
}

ParserInfo type(DataTypes *type)
{
    Token t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    
    if(!strcmp(t.lx,"int"))
    {
        *type = INT;
    } 
    else if(!strcmp(t.lx,"char"))
    {
        *type = CHAR; 
    }
    else if(!strcmp(t.lx,"boolean"))
    {
        *type = BOOL;
    } 
    else if (t.tp == ID)
    {
        if(symbolExists(t.lx) == -1)
        {
            *type = FLAG;
        }
        *type = IDENTIFIER;
    }
    else
    {
        pi.er = illegalType;
        pi.tk = t;
        return pi;
    }

    pi.er = none;
    return pi;
}

ParserInfo subroutineDeclar()
{
    Token t = GetNextToken();
    Kinds tempKind;
    DataTypes symbolType;
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if (!strcmp(t.lx, "constructor") || !strcmp(t.lx, "function") || !strcmp(t.lx, "method"))
    {
        tempKind = SUBROUTINE;
    }
    else
    {
        pi.er = subroutineDeclarErr;
        pi.tk = t;
        return pi;
    }


    t = PeekNextToken();
    
    if(strcmp(t.lx, "void"))
    {
        pi = type(&symbolType);
        if(pi.er != none)
        {
            return pi;
        }
        
        
    }
    else
    {
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

        symbolType = VOID;
        
    }


    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp != ID)
    {
        pi.er = idExpected;
        pi.tk = t;
        return pi;
    }

    int found = symbolExists(t.lx);
    if(found != -1)
    {        
        pi.er = redecIdentifier;
        pi.tk = t;
        return pi;
    }
    else
    {
        
        insertSymbol(t.lx, symbolType, tempKind, t);

    }


    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    enterScope();
    
    if(t.tp == SYMBOL && !strcmp(t.lx, "("))
    {
        ;
    }
    else
    {
        pi.er = openParenExpected;
        pi.tk = t;
        return pi;
    }


    pi = paramList();
    if(pi.er != none)
    {
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == SYMBOL && !strcmp(t.lx, ")"))
    {
        ;
    }
    else
    {
        pi.er = closeParenExpected;
        pi.tk = t;
        return pi;
    }

    pi = subroutineBody();
    if(pi.er != none)
    {
        return pi;
    }

    exitScope();

    pi.er = none;
    return pi;

}

ParserInfo paramList()
{	
    Token t = PeekNextToken();
    DataTypes symbolType;

    insertSymbol("this", IDENTIFIER, ARGUMENT, t);
    
    if(t.tp == SYMBOL && !strcmp(t.lx, ")"))
    {
        pi.er = none;
        return pi;
    }

    if(!(!strcmp(t.lx, "int") || !strcmp(t.lx, "char") || !strcmp(t.lx, "boolean") || t.tp == ID))
    {
        pi.er = closeParenExpected;
        pi.tk = t;
        return pi;
    }


    pi = type(&symbolType);
    if(pi.er != none)
    {
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp != ID)
    {
        pi.er = idExpected;
        pi.tk = t;
        return pi;
    }

    int found = symbolInScope(t.lx);
    if(found != -1)
    {
        pi.er = redecIdentifier;
        pi.tk = t;
        return pi;
    }
    else
    {    
        insertSymbol(t.lx, symbolType, ARGUMENT, t);
    }

    t = PeekNextToken();
    
    while(!strcmp(t.lx, ","))
    {
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        pi = type(&symbolType);
        if(pi.er != none)
        {
            return pi;
        }

        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        if(t.tp != ID)
        {
            pi.er = idExpected;
            pi.tk = t;
            return pi;
        }

        found = symbolInScope(t.lx);
        if(found != -1)
        {
            pi.er = redecIdentifier;
            pi.tk = t;
            return pi;
        }
        else
        {
            insertSymbol(t.lx, symbolType, ARGUMENT, t);
        }

        t = PeekNextToken();
        

    }


    pi.er = none;
    return pi;
}

ParserInfo subroutineBody()
{
    Token t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    if(t.tp == SYMBOL && !strcmp(t.lx, "{"))
    {
        ;
    }
    else
    {
        
    }

    t = PeekNextToken();
    
    while(t.tp != EOFile && strcmp(t.lx, "}"))
    {
        pi = statement();
        if(pi.er != none)
        {
            return pi;
        }
        t = PeekNextToken();
        
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == SYMBOL && !strcmp(t.lx, "}"))
    {
        ;
    }
    else
    {
        pi.er = closeBraceExpected;
        pi.tk = t;
        return pi;
    }

    pi.er = none;
    return pi;
}

ParserInfo statement()
{
    Token t = PeekNextToken();
    
    if(t.tp == RESWORD && !strcmp(t.lx, "var"))
    {
        pi = varDeclarStatement();
        if(pi.er != none)
        {
            return pi;
        }
    }
    else if(t.tp == RESWORD && !strcmp(t.lx, "let"))
    {
        pi = letStatement();
        if(pi.er != none)
        {
            return pi;
        }
    }
    else if(t.tp == RESWORD && !strcmp(t.lx, "if"))
    {
        pi = ifStatement();
        if(pi.er != none)
        {
            return pi;
        }
    }
    else if(t.tp == RESWORD && !strcmp(t.lx, "while"))
    {
        pi = whileStatement();
        if(pi.er != none)
        {
            return pi;
        }
    }
    else if(t.tp == RESWORD && !strcmp(t.lx, "do"))
    {
        pi = doStatement();
        if(pi.er != none)
        {
            return pi;
        }
    }
    else if(t.tp == RESWORD && !strcmp(t.lx, "return"))
    {
        pi = returnStatement();
        if(pi.er != none)
        {
            return pi;
        }
    }
    else
    {
        pi.er = syntaxError;
        pi.tk = t;
        return pi;
    }

    pi.er = none;
    return pi;
}

ParserInfo varDeclarStatement()
{
    Token t = GetNextToken();
    DataTypes symbolType;

    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    

    if(t.tp == RESWORD && !strcmp(t.lx, "var"))
    {
        ;
    }
    else
    {
        pi.er = syntaxError;
        pi.tk = t;
        return pi;
    }

    pi = type(&symbolType);
    if(pi.er != none)
    {
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    if(t.tp != ID)
    {
        pi.er = idExpected;
        pi.tk = t;
        return pi;
    }

    int found = symbolInScope(t.lx);
    if (found != -1)
    {
        pi.er = redecIdentifier;
        pi.tk = t;
        return pi;
    }

    insertSymbol(t.lx, symbolType, VAR, t);

    t = PeekNextToken();
    
    while(!strcmp(t.lx, ",")){
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

        if(t.tp != ID)
        {
            pi.er = idExpected;
            pi.tk = t;
            return pi;
        }

        found = symbolInScope(t.lx);
        if (found != -1)
        {
            pi.er = redecIdentifier;
            pi.tk = t;
            return pi;
        }
        insertSymbol(t.lx, symbolType, VAR, t);
        t = PeekNextToken();
        
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    

    if(t.tp == SYMBOL && !strcmp(t.lx, ";"))
    {
        ;
    }
    else
    {
        pi.er = semicolonExpected;
        pi.tk = t;
        return pi;
    }

    pi.er = none;
    return pi;

}

ParserInfo letStatement()
{
    Token t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    if(t.tp == RESWORD && !strcmp(t.lx, "let"))
    {
        ;
    }
    else
    {
        pi.er = syntaxError;
        pi.tk = t;
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp != ID)
    {
        pi.er = idExpected;
        pi.tk = t;
        return pi;
    }

    int found = symbolExists(t.lx);
    if (found == -1)
    {
        pi.er = undecIdentifier;
        pi.tk = t;
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    if(t.tp == SYMBOL && !strcmp(t.lx, "[")){
        pi = expression();
        if(pi.er != none)
        {
            return pi;
        }
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

        
        if(t.tp == SYMBOL && !strcmp(t.lx, "]"))
        {
            ;
        }
        else
        {
            pi.er = closeBracketExpected;
            pi.tk = t;
            return pi;
        }
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
    }

    if(t.tp == SYMBOL && !strcmp(t.lx, "="))
    {
        ;
    }
    else
    {
        pi.er = equalExpected;
        pi.tk = t;
        return pi;
    }

    pi = expression();
    if(pi.er != none)
    {
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    

    if(t.tp == SYMBOL && !strcmp(t.lx, ";"))
    {
        ;
    }
    else
    {
        pi.er = semicolonExpected;
        pi.tk = t;
        return pi;
    }

    pi.er = none;
    return pi;
}

ParserInfo ifStatement()
{
    Token t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    

    if(t.tp == RESWORD && !strcmp(t.lx, "if"))
    {
        ;
    }
    else
    {
        pi.er = syntaxError;
        pi.tk = t;
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == SYMBOL && !strcmp(t.lx, "("))
    {
        ;
    }
    else
    {
        pi.er = openParenExpected;
        pi.tk = t;
        return pi;
    }

    pi = expression();
    if(pi.er != none)
    {
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    
    if(t.tp == SYMBOL && !strcmp(t.lx, ")"))
    {
        ;
    }
    else
    {
        pi.er = closeParenExpected;
        pi.tk = t;
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    

    if(t.tp == SYMBOL && !strcmp(t.lx, "{"))
    {
        ;
    }
    else
    {
        pi.er = openBraceExpected;
        pi.tk = t;
        return pi;
    }

    enterScope();

    t = PeekNextToken();
    
    while(t.tp != EOFile && strcmp(t.lx, "}"))
    {
        pi = statement();
        if(pi.er != none)
        {
            return pi;
        }
        t = PeekNextToken();
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == SYMBOL && !strcmp(t.lx, "}"))
    {
        ;
    }
    else
    {
        pi.er = closeBraceExpected;
        pi.tk = t;
        return pi;
    }

    exitScope();

    t = PeekNextToken();
    
    if(t.tp == RESWORD && !strcmp(t.lx, "else"))
    {
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        if(t.tp == SYMBOL && !strcmp(t.lx, "{"))
        {
            ;
        }
        else
        {
            pi.er = openBraceExpected;
            pi.tk = t;
            return pi;
        }

        enterScope();

        t = PeekNextToken();
        
        while(t.tp != EOFile && strcmp(t.lx, "}"))
        {
            pi = statement();
            if(pi.er != none)
            {
                return pi;
            }
            t = PeekNextToken();
            
        }

        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        if(t.tp == SYMBOL && !strcmp(t.lx, "}"))
        {
            ;
        }
        else
        {
            pi.er = closeBraceExpected;
            pi.tk = t;
            return pi;
        }

        exitScope();
    }

    pi.er = none;
    return pi;
}

ParserInfo whileStatement()
{
    Token t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == RESWORD && !strcmp(t.lx, "while"))
    {
        ;
    }
    else
    {
        pi.er = syntaxError;
        pi.tk = t;
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == SYMBOL && !strcmp(t.lx, "("))
    {
        ;
    }
    else
    {
        pi.er = openParenExpected;
        pi.tk = t;
        return pi;
    }

    pi = expression();
    if(pi.er != none)
    {
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    
    if(t.tp == SYMBOL && !strcmp(t.lx, ")"))
    {
        ;
    }
    else
    {
        pi.er = closeParenExpected;
        pi.tk = t;
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == SYMBOL && !strcmp(t.lx, "{"))
    {
        ;
    }
    else
    {
        pi.er = openBraceExpected;
        pi.tk = t;
        return pi;
    }

    enterScope();
    
    t = PeekNextToken();
    
    while(t.tp != EOFile && strcmp(t.lx, "}"))
    {
        pi = statement();
        if(pi.er != none)
        {
            return pi;
        }
        t = PeekNextToken();
        
    }


    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == SYMBOL && !strcmp(t.lx, "}"))
    {
        ;
    }
    else
    {
        pi.er = closeBraceExpected;
        pi.tk = t;
        return pi;
    }

    exitScope();

    pi.er = none;
    return pi;
}

ParserInfo doStatement()
{
    Token t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == RESWORD && !strcmp(t.lx, "do"))
    {
        ;
    }
    else
    {
        pi.er = syntaxError;
        pi.tk = t;
        return pi;
    }

    pi = subroutineCall();
    if(pi.er != none)
    {
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == SYMBOL && !strcmp(t.lx, ";"))
    {
        ;
    }
    else
    {
        pi.er = semicolonExpected;
        pi.tk = t;
        return pi;
    }

    pi.er = none;
    return pi;
}

ParserInfo subroutineCall()
{
    Token t = GetNextToken();    
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    if(t.tp != ID)
    {
        pi.er = idExpected;
        pi.tk = t;
        return pi;
    }

    int found = symbolExists(t.lx);
    Token possibleID;
    possibleID = t;

    t = PeekNextToken();
    
    if(t.tp == SYMBOL && !strcmp(t.lx, "."))
    {
        if (found == -1)
        {
            insertSymbol(possibleID.lx, FLAG, UNKNOWN, possibleID);
        }
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        if(t.tp != ID)
        {
            pi.er = idExpected;
            pi.tk = t;
            return pi;
        }

        found = symbolExists(t.lx);
        if (found == -1)
        {
            insertSymbol(t.lx, FLAG, SUBROUTINE, t);
        }
    }

    if(found == -1)
    {
        insertSymbol(possibleID.lx, FLAG, SUBROUTINE, possibleID);
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == SYMBOL && !strcmp(t.lx, "("))
    {
        ;
    }
    else
    {
        pi.er = openParenExpected;
        pi.tk = t;
        return pi;
    }

    pi = expressionList();
    if(pi.er != none)
    {
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    
    if(t.tp == SYMBOL && !strcmp(t.lx, ")"))
    {
        ;
    }
    else
    {
        pi.er = closeParenExpected;
        pi.tk = t;
        return pi;
    }

    pi.er = none;
    return pi;

}

ParserInfo expressionList()
{	
    Token t = PeekNextToken();
    

    if(t.tp == SYMBOL && !strcmp(t.lx, ")"))
    {
        pi.er = none;
        return pi;
    }

    pi = expression();
    if(pi.er != none)
    {
        return pi;
    }

    t = PeekNextToken();
    

    while(!strcmp(t.lx, ","))
    {
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        pi = expression();
        if(pi.er != none)
        {
            return pi;
        }
        t = PeekNextToken();
        
    }

    pi.er = none;
    return pi;

}

ParserInfo returnStatement()
{
    Token t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
    

    if(t.tp == RESWORD && !strcmp(t.lx, "return"))
    {
        ;
    }
    else
    {
        pi.er = syntaxError;
        pi.tk = t;
        return pi;
    }

    t = PeekNextToken();
    

    if(t.tp == SYMBOL && !strcmp(t.lx, ";"))
    {
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        pi.er = none;
        return pi;
    }

    pi = expression();
    if(pi.er != none)
    {
        if(pi.er == syntaxError)
        {
            pi.er = semicolonExpected;
            return pi;
        }
        return pi;
    }

    t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    if(t.tp == SYMBOL && !strcmp(t.lx, ";"))
    {
        ;
    }
    else
    {
        pi.er = semicolonExpected;
        pi.tk = t;
        return pi;
    }

    pi.er = none;
    return pi;
}

ParserInfo expression()
{
    pi = relationalExpression();
    if(pi.er != none)
    {
        return pi;
    }

    Token t = PeekNextToken();
    

    while(t.tp == SYMBOL && (!strcmp(t.lx, "&") || !strcmp(t.lx, "|")))
    {
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        pi = relationalExpression();
        if(pi.er != none)
        {
            return pi;
        }
        t = PeekNextToken();
        
    }

    pi.er = none;
    return pi;
}

ParserInfo relationalExpression()
{
    pi = arithmeticExpression();
    if(pi.er != none)
    {
        return pi;
    }

    Token t = PeekNextToken();
    

    while(t.tp == SYMBOL && (!strcmp(t.lx, "=") || !strcmp(t.lx, "<") || !strcmp(t.lx, ">")))
    {
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        pi = arithmeticExpression();
        if(pi.er != none)
        {
            return pi;
        }
        t = PeekNextToken();
        
    }

    pi.er = none;
    return pi;
}

ParserInfo arithmeticExpression()
{
    pi = term();
    if(pi.er != none)
    {
        return pi;
    }

    Token t = PeekNextToken();
    

    while(t.tp == SYMBOL && (!strcmp(t.lx, "+") || !strcmp(t.lx, "-")))
    {
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        pi = term();
        if(pi.er != none)
        {
            return pi;
        }
        t = PeekNextToken();
        
    }

    pi.er = none;
    return pi;
}

ParserInfo term()
{
    pi = factor();
    if(pi.er != none)
    {
        return pi;
    }

    Token t = PeekNextToken();
    

    while(t.tp == SYMBOL && (!strcmp(t.lx, "*") || !strcmp(t.lx, "/")))
    {
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        pi = factor();
        if(pi.er != none)
        {
            return pi;
        }
        t = PeekNextToken();
        
    }

    pi.er = none;
    return pi; 
}

ParserInfo factor()
{
    Token t = PeekNextToken();
    

    if(t.tp == SYMBOL && (!strcmp(t.lx, "-") || !strcmp(t.lx, "~")))
    {
        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
    }

    pi = operand();
    if(pi.er != none)
    {
        return pi;
    }

    pi.er = none;
    return pi;
}

ParserInfo operand()
{
    Token t = GetNextToken();
    if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }

    if(t.tp == INT)
    {
        ;
    }
    else if(t.tp == ID)
    {
        int found = symbolExists(t.lx);
        if(found == -1)
        {
            insertSymbol(t.lx, FLAG, UNKNOWN, t);
        }
        t = PeekNextToken();
        
        if(t.tp == SYMBOL && !strcmp(t.lx, "."))
        {
            t = GetNextToken();
            if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
            
            t = GetNextToken();
            if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
            
            if(t.tp != ID)
            {
                pi.er = idExpected;
                pi.tk = t;
                return pi;
            }
            found = symbolExists(t.lx);
            if(found == -1)
            {
                insertSymbol(t.lx, FLAG, UNKNOWN, t);
            }
            t = PeekNextToken();
            
        }

        if(t.tp == SYMBOL && !strcmp(t.lx, "["))
        {	
            t = GetNextToken();
            if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
            
            pi = expression();
            if(pi.er != none)
            {
                return pi;
            }
            t = GetNextToken();
            if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
            
            if(t.tp == SYMBOL && !strcmp(t.lx, "]"))
            {
                ;
            }
            else
            {
                pi.er = closeBracketExpected;
                pi.tk = t;
                return pi;
            }

        }

        if(t.tp == SYMBOL && !strcmp(t.lx, "("))
        {
            t = GetNextToken();
            if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
            
            pi = expressionList();
            if(pi.er != none)
            {
                return pi;
            }
            t = GetNextToken();
            if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
            
            if(t.tp == SYMBOL && !strcmp(t.lx, ")"))
            {
                ;
            }
            else
            {
                pi.er = closeParenExpected;
                pi.tk = t;
                return pi;
            }

        }

    }

    else if(t.tp == SYMBOL && !strcmp(t.lx, "("))
    {
        pi = expression();
        if(pi.er != none)
        {
            return pi;
        }

        t = GetNextToken();
        if(t.tp == ERR) { pi.er = lexerErr; pi.tk = t; return pi; }
        
        if(t.tp == SYMBOL && !strcmp(t.lx, ")"))
        {
            ;
        }
        else
        {
            pi.er = closeParenExpected;
            pi.tk = t;
            return pi;
        }
    }

    else if(t.tp == STRING)
    {
        ;
    }

    else if(t.tp == RESWORD && !strcmp(t.lx, "true"))
    {
        ;
    }

    else if(t.tp == RESWORD && !strcmp(t.lx, "false"))
    {
        ;
    }

    else if(t.tp == RESWORD && !strcmp(t.lx, "null"))
    {
        ;
    }

    else if(t.tp == RESWORD && !strcmp(t.lx, "this"))
    {
        ;
    }

    else
    {
        pi.er = syntaxError;
        pi.tk = t;
        return pi;
    }
    
    pi.er = none;
    return pi;
}

ParserInfo Parse ()
{
    pi.er = none;
    int i;

    pi = classDeclar();

	return pi;
}


int StopParser ()
{
	StopLexer();
	return 1;
}

// #ifndef TEST_PARSER
// int main ()
// {
//     InitParser("closeParenExpected1.jack");
//     pi = Parse();
// 	return 1;
// }
// #endif