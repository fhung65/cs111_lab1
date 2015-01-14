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
#include <stdlib.h>
#include <string.h>
/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

/* A collection of tokens */
struct command_stream
{
	int size ;
	int div ; // the index of next command to be read
	char** tok ; // and array of tokens (c strings)
};

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
	char c = '\0';
	char prev = c ;
	int max_size = 10 ; 
	int current = 0 ;
//	char* token = ( char* ) malloc( max_size * sizeof( char* ) ) ;

	do{
		prev = c ;
		c = get_next_byte( get_next_byte_argument ) ;
		
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
			{}	// TODO: close last token with \0, put it in stream, make new token 
		}
		else if ( ( c == '\n') || ( c == ';' ) || ( c == '|' ) || ( c == '(' ) ||
				  ( c == ')' ) || ( c == '<' ) || ( c == '>' ) || ( c == '#' ) )
		{
			if( ( c == '\n' ) && ( prev == '\n' ) )
				continue;
			else{
				// TODO: close last token, put it in the stream,
				// put c in a new token and put it in the stream
				// make a new token
				}
		}
		else // nonvalid character ( double check that this only means exit now)
		{
			// TODO: exit with error
		}
		

	}   while( c != EOF ) ;
	
  //error (1, 0, "command reading not yet implemented");
  return 0;
}

void add_word( char** arr, char* word )
{// maintains the resizeable arrays in u.word of commands
	int size = 1 ; // always needs space for at least one ending nullptr
	while(arr != NULL && *arr != NULL) // false for nullptr ?
	{
		size ++ ;
		arr ++ ;
	}
	arr = (char **) realloc( arr, (size+1) * sizeof(char*) ) ;
	arr[size-1] = word ;
	arr[size] = NULL ;
}

int fits_command( char* token, command_t cmd)
{
	enum command_type t = cmd->type ;
	command_t c_0 = cmd->u.command[0] ;
	command_t c_1 = cmd->u.command[1] ;
	command_t c_2 = cmd->u.command[2] ;
	if( ( strcmp( token, "do")) 
		&& ( ( t == WHILE_COMMAND ) || ( t == UNTIL_COMMAND ) ) )
	{
		return ( c_0 == NULL ) ;
	}
	else if( ( strcmp( token, "done" ) )
		&& ( ( t == WHILE_COMMAND ) || ( t == UNTIL_COMMAND ) ) )
	{
		return ( ( c_0 != NULL ) && ( c_1 == NULL ) ) ; 
	}
	else if( ( strcmp(token,"then") ) && ( t == IF_COMMAND ) )
	{
		return ( c_0 == NULL ) ;
	}
	else if( ( strcmp( token, "else") ) && ( t == IF_COMMAND ) )
	{
		return ( ( c_0 != NULL ) && ( c_1 == NULL ) ) ;
	}
	else if( ( strcmp( token, "fi") ) && ( t == IF_COMMAND ) )
	{
		return ( ( ( c_0 != NULL ) && ( c_1 == NULL ) )
				|| ( ( c_0 != NULL ) && ( c_1 != NULL ) && ( c_2 == NULL ) ) ) ; 
	}
	else if( ( strcmp( token, ")" ) ) && ( t == SUBSHELL_COMMAND ) )
	{
		return ( c_0 != NULL ) ;
	}
	else
		return 0 ;
} 

command_t new_command( enum command_type t)
{
	command_t cmd = (command_t) calloc( 1, sizeof(struct command) ) ; // everything is null?
	cmd->type = t ;
	return cmd ;
}

void push_base( command_t cmd , command_t* base , int* scope, int* size )
{ 
	if( *scope < 0 )
	{
		printf( "error in push_base: tried to call with negative scope\n");
		exit(1) ;
	}

	*scope ++;
	if( *scope >= *size )
	{
		*size += *size ;
		base = ( command_t* ) realloc( base, *size * sizeof( command_t ) ) ;
	}
	base[*scope] = cmd ;

	fprintf( stderr, "pushed on %s\n" , (cmd == NULL)? "NULL" 
									: (cmd->type == IF_COMMAND)? "IF" 
									: (cmd->type == PIPE_COMMAND)? "PIPE"
									: (cmd->type == SEQUENCE_COMMAND)? "SEQUENCE?"
									: (cmd->type == SIMPLE_COMMAND)? "SIMPLE"
									: (cmd->type == SUBSHELL_COMMAND)? "SUBSHELL"
									: (cmd->type == UNTIL_COMMAND)? "UNTIL"
									: (cmd->type == WHILE_COMMAND)? "WHILE"
									: "FAULTY COMMAND" ) ;
}

command_t pop_base( command_t* base , int* scope ) 
{ 
	command_t temp = base[*scope] ;
	base[*scope] = NULL ;
	if( *scope > 0 )
	{
		if( base[*scope-1] == NULL)
		{
			printf( "error in pop_base: tried to pop into null\n") ;
			exit(1) ;
		}
	
		int slot = 0;
		while( base[*scope-1]->u.command[slot] != NULL )
		{	// find first free slot
			if( slot > 3 )
			{ //error 
				printf("error in pop_base: tried to insert into command with all 3 slots taken\n") ;
				exit(1) ;
			}
			slot ++ ; 
		}
		base[*scope-1]->u.command[slot] = temp ;
		*scope -- ;
	}
	else if( *scope == 0 )
		fprintf( stderr, "in pop_base: called while scope == 0\n") ;
	else if( *scope < 0 )
	{
		printf("error in pop_base: tried to call with negative scope \n") ;
		exit(1) ;
	}

	fprintf( stderr, "popped off %s\n" , (temp == NULL)? "NULL" 
									: (temp->type == IF_COMMAND)? "IF" 
									: (temp->type == PIPE_COMMAND)? "PIPE"
									: (temp->type == SEQUENCE_COMMAND)? "SEQUENCE?"
									: (temp->type == SIMPLE_COMMAND)? "SIMPLE"
									: (temp->type == SUBSHELL_COMMAND)? "SUBSHELL"
									: (temp->type == UNTIL_COMMAND)? "UNTIL"
									: (temp->type == WHILE_COMMAND)? "WHILE"
									: "FAULTY COMMAND" ) ;
	return temp ;	
}

//REDO, see bottom for commented out old version
command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  //error (1, 0, "command reading not yet implemented");

//	base[scope] is the current top of the subtree being built
//	either null ( building ) or is a completed command
	int size = 6 ;
	int scope = 0 ;
	command_t base[ size ] ; 
	base[scope] = NULL ; // when base[scope] is null, we are ready to start a command
	char* token = NULL ;

	while( s->div < s->size ) 
	{
		token = s->tok[s->div] ;
		if( strcmp( token, "#" ) ) 
		{
			while( ( s->div + 1 < s->size ) 
					&& ( strcmp(s->tok[ s->div + 1 ], "\n") ) )
			{
				s->div ++ ; // simply ignore until we need to process a newline
			}
			continue ;
		}
		else if( strcmp( token, "if" ) || strcmp( token, "while" ) || 
				strcmp( token, "until") || strcmp( token, "(" ) )
		{
			if( base[scope] == NULL )
			{ // if top of base stack is expecting a new command
				enum command_type t = ( ( strcmp(token, "if") )? IF_COMMAND : 
							( strcmp( token, "while") )? WHILE_COMMAND :
							( strcmp(token, "until") )? UNTIL_COMMAND : 
							SUBSHELL_COMMAND );
				command_t cmd = new_command( t ) ; 
				push_base( cmd, base, &scope, &size ) ;
				push_base( NULL, base, &scope, &size ) ;
				continue ;
			} 
			else if( base[scope]->type != SIMPLE_COMMAND )
			{ // syntax error also seq and pipe 
				printf("Invalid syntax! unexpected token near %s\n", token) ;
				exit(1) ;
			} // otherwise, fall through and treat as word
		}
		else if( (strcmp(token, "done") ) || (strcmp(token, "do")) 
				|| (strcmp(token, "fi")) || ( strcmp(token, "then") ) 
				|| ( strcmp(token, ")" ) ) ) 
		{
			if(	(base[scope] == NULL)
				&& (scope > 1) && (base[scope-1]->type == SEQUENCE_COMMAND) )
			{
				if( fits_command( token, base[scope-2] ) )
				{
						pop_base( base, &scope ) ;
						pop_base( base, &scope ) ;
						if( ( strcmp(token, "then")) || (strcmp(token, "do")) 
							||(strcmp(token, "else")) ) 
							push_base( NULL, base, &scope, &size ) ;
				}
			}
			else
			{
				printf("Invalid syntax! unexpected token near %s\n", token) ;
				exit(1) ;
			}
		}
		else if( strcmp(token, ";") || strcmp(token, "|") )
		{
			if( base[scope] == NULL )
			{
				printf("Invalid syntax! unexpected token near %s\n", token) ;
				exit(1) ;
			}

			if( scope > 0  
				&& ( base[scope-1]->type == SEQUENCE_COMMAND 
					|| base[scope-1]->type == PIPE_COMMAND ) )
			{
				pop_base( base, &scope ) ;// should put complete cmd in base[scope-1]
			} 							// only if there was a ; preceeding it

			enum command_type t = (strcmp(token, ";"))? SEQUENCE_COMMAND : PIPE_COMMAND ;
			command_t cmd = new_command( t ) ;
			cmd->u.command[0] = base[scope] ; //stick complete cmd under this new one
			base[scope] = cmd ; // stick in this new one as the top of the subtree
			//push_base( cmd, base, scope, size ) ; // erroneous
			push_base( NULL, base, &scope, &size ) ;
			continue ;
		}
		else if( strcmp(token, "\n") )
		{
			if( base[scope] == NULL )
			{
				// it's been \n's since the start of this subtree, or 
				// this follows 1 semicolon
				// if scope is 0, then need check for end case, cause ;

				//if( scope != 1 ) I also had an else in the below cond
				//	continue ; // but I think this might be redundant
				if( ( scope == 1 ) && ( base[scope-1]->type == SEQUENCE_COMMAND )
					&& (s->div < s->size - 1) && ( strcmp(s->tok[s->div+1], "\n") ) )
				{ // if it's a dangling ";" at scope 0
					
						// end command, check final syntax return
						break ;  // or maybe just this ?
					
					// I think that's it for this case
				}
				else
					continue ; //?
			}
			else // base[scope] != null
			{
				if( ( scope == 0 ) 
					&&( s->div < s->size - 1 ) 
					&&( strcmp( s->tok[s->div+1], "\n" ) ) )
				{ // end case
					break;
				}
				else
				{
					command_t cmd = new_command( SEQUENCE_COMMAND ) ;
					cmd->u.command[0] = base[scope] ;
					base[scope] = cmd ;
					push_base( NULL, base, &scope, &size ) ;
				}
			}
			continue ;
		}
		else if( strcmp( token, "<" ) || strcmp( token,">") )
		{
			int in = (strcmp(token, "<"))? 1 : 0 ; // 1 => In // 0 => Out
			if( ( base[scope] != NULL ) 
 				&&  ( ( in && ( base[scope]->input != NULL ) ) 
					  || ( (!in) && ( base[scope]->output != NULL ) ) ) )
			{
				if ( s->div < s->size - 1 ) 
				{
					char * next = s->tok[s->div+1] ;
					if( ( strcmp(next, ";")) && ( strcmp( next, "|")) 
					&& ( strcmp( next, "(")) && ( strcmp( next, ")")) 
					&& ( strcmp( next, "<")) && ( strcmp( next, ">") ) )
					{// next token is a valid word token
						if( in ) // input
						{
							base[scope]->input = next ;
						}
						else // output
						{
							base[scope]->output = next ;
						}
						s->div ++ ;
						continue ;
					}
					// otherwise, next is invalid token fall through
				}
			}
			//otherwise, or if fell through, ERROR			

			printf("Invalid syntax! unexpected token near %s\n", token) ;
			exit(1) ;
	
			//if( base[scope] == NULL )
			//{ // syntax error
			//	printf("Invalid syntax! unexpected token near %s\n", token) ;
			//	exit(1) ;
			//}
			//else
			//{
			//	if( ( in && ( base[scope]->input != NULL) )
			//		|| ( (!in) && ( base[scope]->output != NULL ) ) )
			//	{ // syntax error there was already an input
			//		printf("Invalid syntax! unexpected token near %s\n", token) ;
			//		exit(1) ;
			//	}
			//	else
			//	{
			//		// get next token, if it's a word, chuck it in, else error
			//		base[scope]->input 
			//	}
			//}
		}
//		otherwise, treat as word
//		{
			if( base[scope] == NULL )
			{
				command_t cmd = new_command( SIMPLE_COMMAND ) ;
				add_word( cmd->u.word, token ) ;
				push_base( cmd, base, &scope, &size ) ;
			}
			else if( base[scope]->type == SIMPLE_COMMAND )
			{
				add_word( base[scope]->u.word, token ) ;	
			}
			else
			{ //syntax error
				printf("Invalid syntax! unexpected token near %s\n", token) ;
				exit(1) ;
			}

//		}


	} // now s->divider >= s->size, or we've finished a command (\n\n)


  return 0;
}

//command_t
//read_command_stream (command_stream_t s)
//{
//  /* FIXME: Replace this with your implementation too.  */
//  //error (1, 0, "command reading not yet implemented");
//
////	base[scope] is the current top of the subtree being built
////	either null ( building ) or is a completed command
//	int size = 6 ;
//	int scope = 0 ;
//	command* base[ size ] ; 
//	base[scope] = NULL ; // when base[scope] is null, we are ready to start a command
//	char* token = NULL ;
//
//	while( s->div < s->size ) 
//	{
//		token = s->tok[s->div] ;
//		if( token == "#" ) 
//		{
//			while( ( s->div + 1 < s->size ) && ( s->tok[ s->div + 1 ] != "\n" ) )
//				s->div ++ ; // simply ignore until we need to process a newline
//			continue ;
//		}
//		else if( token == "if" || token == "while" || 
//				token == "until" || token == "(" )
//		{
//			if( base[scope] == NULL  
//				|| ( base[scope]->type == SEQUENCE_COMMAND 
//					&& 	base[scope]->u.command[1] == NULL )
//				|| ( base[scope]->type == PIPE_COMMAND 
//					&& base[scope]->u.command[1] == NULL ) )
//			{ // if top of base stack is expecting a new command
//				command_t t = ( (token == "if")? IF_COMMAND : 
//							(token == "while")? WHILE_COMMAND :
//							(token == "until")? UNTIL_COMMAND : SUBSHELL_COMMAND );
//				command* cmd = new_command( t ) ; 
//				push_base( cmd ) ;
//				push_base( NULL ) ;
//				continue ;
//			} 
//			else if( base[scope]->type == IF_COMMAND ||
//						base[scope]->type == WHILE_COMMAND || 
//						base[scope]->type == UNTIL_COMMAND ||
//						base[scope]->type == SUBSHELL_COMMAND )
//			{ // syntax error also seq and pipe 
//				printf("Invalid syntax! unexpected token near %s\n", token) ;
//				exit(1) ;
//			} // otherwise, fall through and treat as word
//		}
//		else if( token == "done" || token == "fi" )
//		{
//			
//		}
//
//
//
//	} // now s->divider >= s->size, or we've finished a command (\n\n)
//
//
//  return 0;
//}
