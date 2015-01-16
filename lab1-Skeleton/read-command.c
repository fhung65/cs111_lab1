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
// read_command_stream's command tree's simple commands have pointers to each token,
// be wary of the aliases
struct command_stream
{
	int capacity ;
	int size ;
	int div ; // the index of next command to be read
	int line_num ;
	char** tok ; // and array of tokens (c strings)
};

typedef struct command_stream *command_stream_t;

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
	stream->div = 0 ;
	stream->tok = (char**) malloc(streamsize*sizeof(char**));
	int stream_iter = 0;
	int tstart = 1;
	c = get_next_byte(get_next_byte_argument);
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
			token[current] = c;
			current++;
			tstart = 0;
			if(current == max_size) {
				token = (char*) realloc(token, (max_size+10)*sizeof(char*));
				max_size += 10;
			}
		}
		else if (c == '#'){
		  if(prev == ' ' || prev == '\t' || prev == '\n' || prev == '\0'){
			if(tstart == 0){
				token[current] = '\0';
				stream->tok[stream_iter] = token;
				stream->size++;
				stream_iter++;
				if(stream_iter >= streamsize){
					streamsize += 10;
					stream->tok = (char**) realloc(stream->tok, (streamsize)*sizeof(char**));
				}
				current = 0;
				max_size = 10;
				token = (char*) malloc(max_size*sizeof(char*));
				tstart = 1;
			}
			while(c != '\n' || c == EOF ){
				c = get_next_byte(get_next_byte_argument);
			} 
			continue; // c is replaced at the end, so continue skips
		  } else {
		    error(2,0,"Invalid character");
		  }
		}
		else if ( ( c == ' ' ) || ( c == '\t' ) )
		{
			if( ( prev == ' ' ) || ( prev == '\t' ) ) ;
				//continue ;
			else{
				if(tstart == 0){
					token[current] = '\0';
					stream->tok[stream_iter] = token;
					stream->size++;
					stream_iter++;
					if(stream_iter >= streamsize){
						streamsize += 10;
						stream->tok = (char**) realloc(stream->tok, (streamsize)*sizeof(char**));
					}
					current = 0;
					max_size = 10;
					token = (char*) malloc(max_size*sizeof(char*)); //Like this?
					tstart =1;
				}
			}
		}
		else if ( ( c == '\n') || ( c == ';' ) || ( c == '|' ) || ( c == '(' ) ||
				  ( c == ')' ) || ( c == '<' ) || ( c == '>' ))
		{
				if(tstart == 0){
					token[current] = '\0';
					stream->tok[stream_iter] = token;
					stream->size++;
					stream_iter++;
					if(stream_iter >= streamsize){
						streamsize += 10;
						stream->tok = (char**) realloc(stream->tok, (streamsize)*sizeof(char**));
					}
					current = 0;
					max_size = 10;
					token = (char*) malloc(2*sizeof(char*)); //Like this?
					tstart =1;
				}
				
				token[0] = c;
				token[1] = '\0';
				stream->tok[stream_iter] = token;
				stream_iter++;
				stream->size++;
				if(stream_iter >= streamsize){
					streamsize += 10;
					stream->tok = (char**) realloc(stream->tok, (streamsize)*sizeof(char**));
				}
				
				token = (char*) malloc(max_size*sizeof(char*)); //Like this?
				
		}
		else // nonvalid character ( double check that this only means exit now)
		{
			// TODO: exit with error
		  error(2, 0 , "syntax error"); //or other message?
			//exit(EXIT_FAILURE);
		}
		
		prev = c ;
		c = get_next_byte( get_next_byte_argument ) ;
	}   while( c != EOF );
	token[current] = '\0';
	stream->tok[stream_iter] = token;
	stream->size++;
	
  return stream;
}

void add_word( char*** arr, char* word )
{// maintains the null terminated resizeable arrays in u.word of commands
//	resizes array to be one bigger and appends the new word at end ( before null )
	int size = 1 ; // always needs space for at least one ending nullptr
	if( *arr == NULL )
	{
		*arr = (char**) malloc( (size+1) * sizeof(char*) ) ;
	}
	else
	{
		while( *(*arr + size - 1) != NULL) // false for nullptr ?
		{
			size ++ ;
		}
		*arr = (char **) realloc( *arr, (size+1) * sizeof(char*) ) ;
	}
	(*arr)[size-1] = word ;
	(*arr)[size] = NULL ;
}

int fits_command( char* token, command_t* base, int scope )
{//	checks if the string token matches the given command
//	he we assume, base[scope] is non null, 
//		and base[scope-1] is the target
	command_t cmd = base[scope-1] ;
	enum command_type t = cmd->type ; // type of target
	enum command_type in = base[scope]->type ; // type of input
	command_t c_0 = cmd->u.command[0] ;
	command_t c_1 = cmd->u.command[1] ;
	command_t c_2 = cmd->u.command[2] ;

	if( ( !strcmp( token, "do")) 
		&& ( ( t == WHILE_COMMAND ) || ( t == UNTIL_COMMAND ) ) )
	{
		return ( c_0 == NULL ) ;
	}
	else if( ( !strcmp( token, "done" ) )
		&& ( ( t == WHILE_COMMAND ) || ( t == UNTIL_COMMAND ) ) )
	{
		return ( ( c_0 != NULL ) && ( c_1 == NULL ) ) ; 
	}
	else if( ( !strcmp(token,"then") ) && ( t == IF_COMMAND ) )
	{
		return ( c_0 == NULL ) ;
	}
	else if( ( !strcmp( token, "else") ) && ( t == IF_COMMAND ) )
	{
		return ( ( c_0 != NULL ) && ( c_1 == NULL ) ) ;
	}
	else if( ( !strcmp( token, "fi") ) && ( t == IF_COMMAND ) )
	{
		return ( ( ( c_0 != NULL ) && ( c_1 == NULL ) && ( in == SEQUENCE_COMMAND ) )
				|| ( ( c_0 != NULL ) && ( c_1 != NULL ) && ( c_2 == NULL ) 
						&&	(in == SEQUENCE_COMMAND) ) ) ; 
	}
	else if( ( !strcmp( token, ")" ) ) && ( t == SUBSHELL_COMMAND ) )
	{
		return ( c_0 == NULL ) ;
	}
	//else
	
	return 0 ;
} 

command_t new_command( enum command_type t)
{
	command_t cmd = (command_t) calloc( 1, sizeof(struct command) ) ; 
	cmd->type = t ;
	return cmd ;
}

void push_base( command_t cmd , command_t** base , int* scope, int* size )
{ 
	if( *scope < 0 )
	{
		fprintf(stderr, "error in push_base: tried to call with negative scope\n");
		exit(1) ;
	}

	(*scope)++ ;
	if( *scope >= *size )
	{
		*size += *size ;
		*base = ( command_t* ) realloc( *base, *size * sizeof( command_t ) ) ;
	}
	(*base)[*scope] = cmd ;

//	fprintf( stderr, "pushed on %s\n" , (cmd == NULL)? "NULL" 
//									: (cmd->type == IF_COMMAND)? "IF" 
//									: (cmd->type == PIPE_COMMAND)? "PIPE"
//									: (cmd->type == SEQUENCE_COMMAND)? "SEQUENCE?"
//									: (cmd->type == SIMPLE_COMMAND)? "SIMPLE"
//									: (cmd->type == SUBSHELL_COMMAND)? "SUBSHELL"
//									: (cmd->type == UNTIL_COMMAND)? "UNTIL"
//									: (cmd->type == WHILE_COMMAND)? "WHILE"
//									: "FAULTY COMMAND" ) ;
}

command_t pop_base( command_t* base , int* scope ) 
{ 
	command_t temp = base[*scope] ;
	base[*scope] = NULL ;
	if( *scope > 0 )
	{
		if( base[*scope-1] == NULL)
		{
			fprintf(stderr, "error in pop_base: tried to pop into null\n") ;
			exit(1) ;
		}
	
		int slot = 0;
		while( base[*scope-1]->u.command[slot] != NULL )
		{	// find first free slot
			if( slot > 3 )
			{ //error 
				fprintf(stderr, "error in pop_base: tried to insert into command with all 3 slots taken\n") ;
				exit(1) ;
			}
			slot ++ ; 
		}
		if( ( temp != NULL ) && ( temp->type == SEQUENCE_COMMAND ) 
			&& ( temp->u.command[1] == NULL ) )
		{
			base[*scope-1]->u.command[slot] = temp->u.command[0] ;
			free( temp ) ;
		}
		else
		{	
			base[*scope-1]->u.command[slot] = temp ;
		}
		(*scope)-- ;
	}
	else if( *scope < 0 )
	{
		fprintf(stderr, "error in pop_base: tried to call with negative scope \n") ;
		exit(1) ;
	}
	// if *scope == 0, fall through, last command
//	if( *scope == 0 )
//		fprintf( stderr, "popped off the last command\n") ;
	return temp ;	
}

//	commands:
//	if A then B else C
//	while A do B done
//	untile A do B done
//	( A )
//	A ; B
//	A | B
//	A \n B 
//	base[scope] is the current top of the subtree being built
//	either null ( building ) or is a completed command
//	null when expecting an A, B, or C 
//	(see above, and yes, null if there's an incomplete ";" on top)
command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  //error (1, 0, "command reading not yet implemented");
	s->line_num = 0 ;
	int size = 6 ;
	int scope = 0 ;
	command_t* base ;
	base = ( command_t*) malloc( size* sizeof(command_t) );
	base[scope] = NULL ; // when base[scope] is null, we are ready to start a command
	char* token = NULL ;

	for( ; s->div < s->size ; s->div ++  ) 
	{
		token = s->tok[s->div] ;
		if( !strcmp( token, "if" ) || !strcmp( token, "while" ) || 
				!strcmp( token, "until") || !strcmp( token, "(" ) )
		{
			if( base[scope] == NULL )
			{ // if top of base stack is expecting a new command
				enum command_type t = ( ( !strcmp(token, "if") )? IF_COMMAND : 
							( !strcmp( token, "while") )? WHILE_COMMAND :
							( !strcmp(token, "until") )? UNTIL_COMMAND : 
							SUBSHELL_COMMAND );
				command_t cmd = new_command( t ) ; 
				base[scope] = cmd ;
				push_base( NULL, &base, &scope, &size ) ;
				continue ;
			} 
			else if( base[scope]->type != SIMPLE_COMMAND )
			{ // syntax error also seq and pipe 
				fprintf( stderr, "Invalid syntax: unexpected token near %s\n", token) ;
				exit(1) ;
			} // otherwise, fall through and treat as word
		}
		else if( (!strcmp(token, "done") ) || (!strcmp(token, "do")) 
				|| (!strcmp(token, "fi")) || ( !strcmp(token, "then") ) 
				|| (!strcmp(token, "else")) || ( !strcmp(token, ")" ) ) ) 
		{
			if(base[scope] == NULL || !strcmp( token, ")"))
			{
				if( scope >= 1 ) 
				{
					if(	( base[scope-1]->type == SEQUENCE_COMMAND 
							&& base[scope] == NULL  ) 
					 ||	( !strcmp(token, ")") 
					 		&& (base[scope-1]->type == SEQUENCE_COMMAND 
								||( base[scope-1]->type == PIPE_COMMAND 
									&& base[scope] != NULL ) ) ) )
					{
						pop_base( base, &scope ) ;
					} 
					// fall through, after this, the thing to be popped in isn't null
					
					if( fits_command( token, base, scope ) )
					{
						pop_base( base, &scope ) ;

						if( ( !strcmp(token, "then")) || (!strcmp(token, "do")) 
							|| (!strcmp(token, "else")) ) 
						{
							push_base( NULL, &base, &scope, &size ) ;
						}
						continue ;
					} // else fall through
				} // fall through

		fprintf(stderr, "Invalid syntax! unexpected token near %s\n", token) ;
				exit(1) ;
			}
		}
		else if( !strcmp(token, ";") || !strcmp(token, "|") )
		{
			if( base[scope] == NULL )
			{
				fprintf(stderr, "Invalid syntax: unexpected token near %s\n", token) ;
				exit(1) ;
			}

			if( scope > 0  
				&& ( base[scope-1]->type == SEQUENCE_COMMAND 
					|| base[scope-1]->type == PIPE_COMMAND ) )
			{	
				pop_base( base, &scope ) ;// should put complete cmd in base[scope-1]
			} 							

			enum command_type t = (!strcmp(token, ";"))? 
				SEQUENCE_COMMAND : PIPE_COMMAND ;
			command_t cmd = new_command( t ) ;
			cmd->u.command[0] = base[scope] ; //stick complete cmd under this new one
			base[scope] = cmd ; // stick in this new one as the top of the subtree
			//push_base( cmd, &base, scope, size ) ; // erroneous
			push_base( NULL, &base, &scope, &size ) ;
			continue ;
		}
		else if( !strcmp(token, "\n") )
		{
			s->line_num ++ ;
			if(	( ( scope == 0 && base[scope] != NULL) 
					||  (scope == 1 && base[0]->type == SEQUENCE_COMMAND) 
					||  (	(scope == 1 && base[0]->type == PIPE_COMMAND)
							&&(base[0]->u.command[0] != NULL) ) )
				&&( s->div < s->size - 1 ) 
				&&( !strcmp( s->tok[s->div+1], "\n" ) ) ) 
			{ // end case
				//printf("reached here! at scope: %i\n", scope) ;
				s->div += 2;
				break;
			}

			if( base[scope] == NULL )
			{
				// it's been \n's since the start of this subtree, or 
				// this follows 1 semicolon
				// if scope is 0, then need check for end case, cause ;

				//if( scope != 1 ) I also had an else in the below cond
				//	continue ; // but I think this might be redundant

			//	if( ( scope == 1 ) && ( base[scope-1]->type == SEQUENCE_COMMAND )
			//		&& (s->div < s->size - 1) && (!strcmp(s->tok[s->div+1], "\n") ) )
			//	{ // if it's a dangling ";" at scope 0
			//		
			//			// end command, check final syntax return
			//			break ;  // or maybe just this ?
			//		
			//		// I think that's it for this case
			//	}
			//	else 
					continue ; //?
			}
			else // base[scope] != null
			{
				if( scope > 0 && (base[scope-1]->type == SEQUENCE_COMMAND 
									|| base[scope-1]->type == PIPE_COMMAND ) )
				{
					pop_base( base, &scope ) ;
					// should put complete cmd in base[scope-1]
				}
				command_t cmd = new_command( SEQUENCE_COMMAND ) ;
				cmd->u.command[0] = base[scope] ;
				base[scope] = cmd ; 
				push_base( NULL, &base, &scope, &size ) ;
			}
			continue ;
		}
		else if( !strcmp( token, "<" ) || !strcmp( token,">") ) 
		{
			int in = (!strcmp(token, "<"))? 1 : 0 ; // 1 => In // 0 => Out
			if( ( base[scope] != NULL ) 
 				&&  ( ( in && ( base[scope]->input == NULL ) ) 
					  || ( (!in) && ( base[scope]->output == NULL ) ) ) )
			{
				if ( s->div < ( s->size - 1 ) ) 
				{
					char * next = s->tok[s->div+1] ;
					if( ( strcmp(next, "\n") ) && ( strcmp(next, ";")) 
					&& ( strcmp( next, "|")) && ( strcmp( next, "(")) 
					&& ( strcmp( next, ")")) && ( strcmp( next, "<")) 
					&& ( strcmp( next, ">") ) )
					{// next token is a valid word token
						if( in ) // input
						{
							if( base[scope]->output != NULL  )
							{
								fprintf(stderr, "Invalid Syntax: unexpected token near: %s\n", token) ;
								exit(1) ;
							}
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

			fprintf(stderr, "Invalid syntax! unexpected token near %s\n", token) ;
			exit(1) ;
		}
		else if( !strcmp(token, "") )
		{
			break ;
		}
	//	otherwise, treat as word
	//	{
			if( base[scope] == NULL )
			{
				//printf("token = %s\n" , token ) ;
				command_t cmd = new_command( SIMPLE_COMMAND ) ;
				add_word( &cmd->u.word, token ) ;
				base[scope] = cmd ;
			}
			else if( base[scope]->type == SIMPLE_COMMAND )
			{
				add_word( &base[scope]->u.word, token ) ;	
			}
			else
			{ //syntax error
				fprintf(stderr, 
					"Invalid syntax! unexpected token near %s\n", token) ;
				exit(1) ;
			}

	//	}


	} // now s->divider >= s->size, or we've finished a command (\n\n)
	
	// done making command now, or reached end of stream
	if( scope >= 1 )
	{
		if( scope == 1 && ( ( base[0]->type == SEQUENCE_COMMAND ) 
					|| ( base[0]->type == PIPE_COMMAND && base[1] != NULL) ) ) 
		{
			pop_base( base, &scope ) ; // it's ok to pop a null
		//	printf( "returned base[scope] = %d\n" , base[scope]->type ) ;
		
			if( base[0]->u.command[1] == NULL ) 
			{
				command_t retval = base[0]->u.command[0] ;
				free( base[0] ) ;
				free( base ) ;
				return retval ;
			}
			else
			{
				command_t retval = base[0] ;
				free( base );
				return retval  ;
			}
			//good to go
		}
		else
		{ // syntax error
			fprintf(stderr, "Invalid syntax: terminated on incomplete command\n") ;
			exit(1) ;
		}
	}
	else if( scope == 0 )
	{
		if( base[scope] != NULL )
		{ // good to go
			//printf( "returned base[scope] = %d\n" , base[scope]->type ) ;
			command_t retval = pop_base( base, &scope) ;
			free( base );
			return retval;
		}
		else
		{ 
			free( base );
			return 0 ; // return nothing command ( or false ) 
			//should only really happen for file with only \n's
		}
	}
	else if( scope < 0 )
	{
		fprintf(stderr, "error in read_command_stream: terminated with negative scope\n") ;
		exit(1) ;
	}

	fprintf(stderr, "should never have gotten here, but compiler's being a pain\n") ;
	return 0 ;
}

void free_command_stream( command_stream_t stream )
{
	for( int i = 0 ; i < stream->size ; i++ )
	{
		printf("freeing %s\n", stream->tok[i] ) ;
		free( stream->tok[i] ) ;
	}
	printf("freed stream\n") ;
	free( stream->tok ) ;
	free( stream ) ;
}

void free_command_tree( command_t tree )
{
	if( tree == NULL)
	{
		//printf("\nreached null\n\n") ;
		return ;
	}
	else if( tree->type == SIMPLE_COMMAND )
	{
		printf("\nfreed simple, word starting with %s\n\n", tree->u.word[0]);
		free( tree->u.word ) ;
		free( tree ) ;
		return ;
	} // exit cases

	for( int i = 0 ; i < 3 ; i++ )
	{
		free_command_tree( tree->u.command[i] ) ;
	}
	printf("freed: %s\n" , (tree == NULL)? "NULL" 
				: (tree->type == IF_COMMAND)? "IF" 
				: (tree->type == PIPE_COMMAND)? "PIPE"
				: (tree->type == SEQUENCE_COMMAND)? "SEQUENCE?"
				: (tree->type == SIMPLE_COMMAND)? "SIMPLE"
				: (tree->type == SUBSHELL_COMMAND)? "SUBSHELL"
				: (tree->type == UNTIL_COMMAND)? "UNTIL"
				: (tree->type == WHILE_COMMAND)? "WHILE"
				: "FAULTY COMMAND" ) ;
	free( tree ) ;

}
