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
#include "lexer.h"


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE

FILE* fp = NULL;




// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name)
{
  fp = fopen(file_name, 'r');
  if(fp == NULL){
    printf("File did not open correctly");
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

  // get the first character of the file
  int c = getc(fp);

  // while the current character is whitespace, continue and consume the next character
  while(isspace(c)){
    // this will remove the next char from the file
    c = getc(fp);
  }

  // if we come across what may be the start of a comment
  if(c == '/'){

    // we get the next character 
    c = getc(fp);

    // if it is a * it is the start of a multi line comment
    if(c == '*'){

      // to keep track of where the multi line comment ends, we will be looking for 
      // the '/' and '*' symbols to be next to eachother, so we keep track of two connsecutive characters, until we see these two together
      c = getc(fp);
      nc = getc(fp);

      while(c != '/' && nc !='*'){
        c = getc(fp);
        nc = getc(fp);
      }

    }

    // two // means a single line comment, so consume characters until we see the new line symbol
    if(c == '/'){
      c = getc(fp);
      while(c != '\n'){
        c = getc(fp);
      }
    }
  }

  // if the end of file marker is detected, save that as a token
  if(c == EOF){
    t.type = eof;
    strcpy(t.lexeme,"End Of File");
    return t;
  }

  // setup a variable to hold the word
  char lexeme[128];
  int i = 0;

  // if the character is alphabetic, then it is part of a keyword or string
  if(isalpha(c)){

    // keep looping until the word is found
    while(isalpha(c)){
      lexeme[i] = c;
      c = getc(fp);
      i++;
    }

    // now that we have the full word, we need to check whether it is a keyword or an identifier
    bool inKW = false;

    // here we loop through the keywords and if our word is in the keywords typdef
    // then it must be a keyword, so we set it to be true and break the search
    for(i = 0; i < sizeof(keywords); i++){
      if(strcmp(lexeme, keywords[i]) == 0){ inKW = true; break; }
    }

    // if it is in keywords, we now know everything we need to about our token, so we can create it and return it
    if(inKW){
      t.type = keyword;
      strcpy(t.lexeme,"Keyword");
      return t;
    }

  } 
  return t;
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken ()
{
  Token t;
  t.type = error;
  return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
	return 0;
}

// do not remove the next line
#ifndef TEST
int main ()
{
	// implement your main function here
  // NOTE: the autograder will not use your main function

  
	return 0;
}
// do not remove the next line
#endif
