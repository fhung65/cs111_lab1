// UCLA CS 111 Lab 1 command reading

// Copyright 2012-2014 Paul Eggert.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */
#include <stdio.h>

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

/* A collection of tokens */
struct command_stream
{
	int size ;
	char* tokens [ size ] ;
};


command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
	char c = '\0' ;
	char prev = c ;
	int max_size = 10 ; 
	int current = 0 ;
	char* token = ( char* ) malloc( max_size * sizeof( char* ) ) ;

	do{
		
		if(	( ( c >= 'a' ) && ( c <= 'z' ) ) ||
			( ( c >= 'A' ) && ( c <= 'Z' ) ) ||
			( ( c >= '0' ) && ( c <= '9' ) ) ||
			( c == '!' ) ||
			( c == '%' ) ||
			( c == '+' ) ||
			( c == ',' ) ||
			( c == '-' ) ||
			( c == '.' ) ||
			( c == '/' ) ||
			( c == ':' ) ||
			( c == '@' ) ||
			( c == '^' ) ||
			( c == '_' ) )
		{	
			// TODO: append to token	
		}
		else if ( ( c == ' ' ) || ( c == '\t' ) )
		{
			if( ( prev == ' ' ) || ( prev == '\t' ) ) 
				continue ;
			else
				// TODO: close last token with \0, put it in stream, make new token 
		}
		else if ( ( c == '\n') || ( c == ';' ) || ( c == '|' ) || ( c == '(' ) ||
				  ( c == ')' ) || ( c == '<' ) || ( c == '>' ) || ( c == '#' ) )
		{
			if( ( c == '\n' ) && ( prev == '\n' ) )
				continue;
			else
				// TODO: close last token, put it in the stream,
				// put c in a new token and put it in the stream
				// make a new token
		}
		else // nonvalid character ( double check that this only means exit now)
		{
			// TODO: exit with error
		}
		
		prev = c ;
		c = get_next_byte( get_next_byte_argument ) ;

	}   while( c != EOF )
	
  error (1, 0, "command reading not yet implemented");
  return 0;
}


command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
