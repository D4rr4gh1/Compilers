/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Compiler Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Darragh Connolly
Student ID: 201533682
Email: sc21dc@leeds.ac.uk
Date Work Commenced:
*************************************************************************/

#include "compiler.h"
#include "symbols.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <glob.h>
#include <sys/dir.h>

#ifndef DT_DIR 
#define DT_DIR 4
#endif


int InitCompiler ()
{
	return 1;
}


ParserInfo compile (char* dir_name)
{
	ParserInfo p;

	// write your code below

	InitSymbolTable();


	glob_t global;

	if(glob("*.jack", 0, NULL, &global) != 0)
	{
		printf("Error: Failed to read correctly");
		exit(0);
	}

	for(int i = 0; i<global.gl_pathc; i++)
	{
		char* path = global.gl_pathv[i];
		InitParser(path);

		p = Parse();

		if(p.er != none)
		{
			return p;
		}
	}

	globfree(&global);

	DIR *dir = opendir(dir_name);

	struct dirent *file;


	while((file = readdir(dir)) != NULL)
	{	
		setScope();
		// ignore current and parent directories
		if(file->d_type == DT_DIR || file->d_name[0] == '.')
		{
			continue;
		}
		else
		{

			char path[512];

			snprintf(path, sizeof(path), "%s/%s", dir_name, file->d_name);


			InitParser(path);

			p = Parse();

			if(p.er != none)
			{
				return p;
			}

			struct Scope *temp = findRoot();

			printTree(temp);

			if(p.er != none)
			{
				return p;
			}
		}
	}	

	p.er = none;
	return p;
}

int StopCompiler ()
{

	return 1;
}


// #ifndef TEST_COMPILER
// int main ()
// {
// 	InitCompiler ();
// 	ParserInfo p = compile("Pong");
// 	//PrintError (p);
// 	StopCompiler ();
// 	return 1;
// }
// #endif
