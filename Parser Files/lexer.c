/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Darragh Connolly
Student ID: 201533682
Email: sc21dc@leeds.ac.uk
Date Work Commenced: 22/02/2023
*************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "lexer.h"


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE

const char* keywords[21] = {"class", "constructor", "static", "void", "method", "function", "field", "var", "int", "char",
                                "boolean", "let", "if", "while", "do", "return", "true", "false", "else", "null", "this"};

int symbols[19] = {'(', ')', '[', ']', '{', '}', ',', ';', '=', '.', '+', '-', '*', '/', '&', '|', '~', '<', '>'}; 


FILE* fp = NULL;

char fName[32];

int lineNum;

int commentRemover(){
  // we get the next character 
  int c = getc(fp);
  // if it is a * it is the start of a multi line comment
  if(c == '*'){

    // to keep track of where the multi line comment ends, we will be looking for 
    // the '/' and '*' symbols to be next to eachother, so we keep track of two connsecutive characters, until we see these two together
    c = getc(fp);
    int nc;
    while(true){
      c = getc(fp);
      nc = getc(fp);
      if(c == EOF){ return 10;}
      if((c == '*' && nc == '/') || (nc == '*' && c == '/')){
        if(nc == '/'){
          c = getc(fp);
        }
        break;
      }
      if(c == '\n') { lineNum ++; }
      ungetc(nc, fp);
    }
  }

  // two // means a single line comment, so consume characters until we see the new line symbol
  if(c == '/'){
    c = getc(fp);
    while(c != '\n'){
      c = getc(fp);
      if(c == EOF){return 10;}
    }
  }

  if(c == '\n') { lineNum++;}
  return 1;

}



// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name)
{
  lineNum = 1;
  fp = fopen(file_name, "r");
  strcpy(fName, file_name);
  if(fp == NULL){
    printf("Error: File did not open correctly\n");
    return 0;
  }
  
  return 1;
}


// Get the next token from the source file
// We want to eat the whitespace until we get to our first actual symbol
Token GetNextToken ()
{
  // initialise a Token
	Token t;
  strcpy(t.fl, fName);

  // get the first character of the file
  int c = getc(fp);


  // while the current character is whitespace, continue and consume the next character
  while(isspace(c) || c == '/' || c == '\n'){
    // this will remove the next char from the file
    if(c == '/'){
      c = getc(fp);
      if(c == '/' || c == '*'){ 
        ungetc(c, fp); 
        if(commentRemover() == 10){
          t.tp = ERR;
          t.ec = EofInCom;
          strcpy(t.lx, "Error: unexpected eof in comment");
          t.ln = lineNum; 
          return t;
        };
      }
      else{ ungetc(c, fp); c = '/'; break; }
    }

    if(c == '\n') { lineNum++; }

    c = getc(fp);
  }

  // if the end of file marker is detected, save that as a token
  if(c == EOF){
    t.tp = EOFile;
    t.ln = lineNum;
    strcpy(t.lx,"End Of File");
    return t;
  }

  // setup a variable to hold the word
  char lex[256] = "";
  int i = 0;

  // if the character is alphabetic, then it is part of a keyword or identifier

  if(c == '"') { 
    /* add to lex until next quotation found */
    c = getc(fp);
    while(c != '"'){
      lex[i] = c;
      i++;
      c = getc(fp);

      if(c == '\n'){
        t.tp = ERR;
        t.ec = NewLnInStr;
        t.ln = lineNum;
        strcpy(t.lx, "Error: new line in string constant");
        return t;
      }

      if(c == EOF){
        t.tp = ERR;
        t.ec = EofInStr;
        t.ln = lineNum;
        strcpy(t.lx, "Error: unexpected eof in string constant");
        return t;
      }
    }

    t.tp = STRING;
    t.ln = lineNum;
    strcpy(t.lx, lex);
    return t;
  }

  if(isalpha(c) || c == '_'){

    // keep looping until the word is found
    while(isalpha(c) || isdigit(c) || c == '_'){
      lex[i] = c;
      c = getc(fp);
      i++;
    }
    ungetc(c,fp);

    // now that we have the full word, we need to check whether it is a keyword or an identifier
    bool inKW = false;

    // here we loop through the keywords and if our word is in the keywords typdef
    // then it must be a keyword, so we set it to be true and break the search
    for(i = 0; i < 21; i++){
      int value = strcmp(lex, keywords[i]);
      if(value == 0){ inKW = true; break; }
    }


    // if it is in keywords, we now know everything we need to about our token, so we can create it and return it
    if(inKW){
      t.tp = RESWORD;
      t.ln = lineNum;
      strcpy(t.lx,lex);
      return t;
    }
    // if it is not in keywords, it must be an ID, which can have underscores and digits, but not spaces
    else{
      t.tp = ID;
      t.ln = lineNum;
      strcpy(t.lx, lex);
      return t;
    }

  }

  if(isdigit(c)){
    while(isdigit(c)){
      lex[i] = c;
      c = getc(fp);
      i++;
    }
    ungetc(c,fp);

    t.tp = INT;
    t.ln = lineNum;
    strcpy(t.lx, lex);
    return t;
    
  }

  bool isSym = false;

  lex[0] = c;

  for(i = 0; i < sizeof(symbols); i++){
    if(c == symbols[i]){ isSym = true; break; }
  }

  if(!isSym){
    t.tp = ERR;
    t.ec = IllSym;
    t.ln = lineNum;
    strcpy(t.lx, "Error: illegal symbol in source file");
    return t;
  }
  if(isSym){
    lex[0] = c;
    t.tp = SYMBOL;
    t.ln = lineNum;
    strcpy(t.lx, lex);
    return t;
  }

  return t;
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken ()
{ 
  fpos_t position;
  fgetpos(fp, &position);
  int currentLn = lineNum;
  Token t = GetNextToken();
  fsetpos(fp, &position);
  lineNum = currentLn;
  return t;

}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
  fclose(fp);
	return 0;
}

// do not remove the next line
#ifndef TEST
int main ()
{
	// implement your main function here
  // NOTE: the autograder will not use your main function
  InitLexer("test.txt");

	return 0;
}
// do not remove the next line
#endif
