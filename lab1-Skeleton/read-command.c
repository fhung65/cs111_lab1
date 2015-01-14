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
	int size;
	char** tokens;
};

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
	char c = '\0' ;
	char prev = c;
	
	int max_size = 10 ; 
	int current = 0 ;
	char* token = ( char* ) malloc( max_size * sizeof( char* ) ) ;
	command_stream_t stream = (command_stream_t) malloc(max_size*sizeof(command_stream_t));
	int streamsize = 10;
	stream->size = 0;
	stream->tokens = (char**) malloc(streamsize*sizeof(char**));
	int stream_iter = 0;

	c = get_next_byte(get_next_byte_argument);
	do{
		printf("%c", c);
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
			token[current] = c;
			current++;
			if(current == max_size) {
				token = (char*) realloc(token, (max_size+10)*sizeof(char*));
				max_size += 10;
			}
		}
		else if ( ( c == ' ' ) || ( c == '\t' ) )
		{
			if( ( prev == ' ' ) || ( prev == '\t' ) ) ;
				//continue ;
			else{
				token[current] = '\0';
				stream->tokens[stream_iter] = token;
				stream->size++;
				stream_iter++;
				if(stream_iter >= streamsize){
					streamsize += 10;
					stream->tokens = (char**) realloc(stream->tokens, (streamsize)*sizeof(char**));
				}
				current = 0;
				max_size = 10;
				token = (char*) malloc(max_size*sizeof(char*)); //Like this?
				
			}
		}
		else if ( ( c == '\n') || ( c == ';' ) || ( c == '|' ) || ( c == '(' ) ||
				  ( c == ')' ) || ( c == '<' ) || ( c == '>' ) || ( c == '#' ))
		{
			//if( ( c == '\n' ) && ( prev == '\n' ) );
				//continue;
			//else{
				token[current] = '\0';
				stream->tokens[stream_iter] = token;
				stream->size++;
				stream_iter++;
				if(stream_iter >= streamsize){
					streamsize += 10;
					stream->tokens = (char**) realloc(stream->tokens, (streamsize)*sizeof(char**));
				}
				current = 0;
				max_size = 10;
				token = (char*) malloc(2*sizeof(char*)); //Like this?
				
				token[0] = c;
				token[1] = '\0';
				stream->tokens[stream_iter] = token;
				stream_iter++;
				stream->size++;
				if(stream_iter >= streamsize){
					streamsize += 10;
					stream->tokens = (char**) realloc(stream->tokens, (streamsize)*sizeof(char**));
				}
				
				token = (char*) malloc(max_size*sizeof(char*)); //Like this?
				
				//}
		}
		else // nonvalid character ( double check that this only means exit now)
		{
			// TODO: exit with error
			//error(2,0, "syntax error"); //or other message?
			printf("%s\n", "Syntax Error");
			exit(EXIT_FAILURE);
		}
		
		prev = c ;
		c = get_next_byte( get_next_byte_argument ) ;

	}   while( c != EOF );
	token[current] = '\0';
	stream->tokens[stream_iter] = token;
	stream->size++;
	
  return stream;
}


command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
