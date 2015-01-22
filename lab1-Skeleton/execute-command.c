// UCLA CS 111 Lab 1 command execution

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
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
prepare_profiling (char const *name)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  error (0, 0, "warning: profiling not yet implemented");
  return -1;
}

int
command_status (command_t c)
{
  return c->status;
}

void
execute_command (command_t c, int profiling)
{
	switch( c->type )
	{
		case IF_COMMAND:
			execute_command( c->u.command[0], profiling ) ;
			if( c->u.command[0]->status == 0 ) 
			{
				execute_command( c->u.command[1], profiling ) ;
				c->status = c->u.command[1]->status ;
			}
			else if( c->u.command[2] != NULL )
			{
				execute_command( c->u.command[2], profiling ) ;
				c->status = c->u.command[2]->status ;
			}
			break ;
		case PIPE_COMMAND: { // W | R 
			int status ;

			pid_t p1 = fork() ; // FORK A CHILD FOR R
			if( p1 == -1 ) 
				_exit(1) ; // TODO: more error handling later
			else if( p1 == 0 ) {	// IN CHILD for R
				int fd[2] ;
				if( pipe( fd ) == -1 )
					_exit(1) ; // TODO: more error handling later
			
				pid_t p2 = fork() ; // SPAWN PROCESS FOR W
				if( p2 == -1 )
				{
					// if we failed to make a writer, exit the reader
					_exit(1) ; // TODO: more erroring
				}
				else if( p2 == 0 ) { // IN CHILD for W
					// Writer doesn't need the input of the pipe, so close that
					if( close(fd[0]) == -1 )
						_exit(1) ; // TODO

					// WRITER gets stdout replaced
					if( dup2(fd[1], 1) == -1) 
						_exit(1) ; // TODO: error

					// run W
					execute_command( c->u.command[1], profiling ) ; 

					// close W's output  fd
					if(close(fd[1]) == -1) 
						_exit(1) ; // TODO: more error
					
					_exit(0) ;
				}
				// READER does't need the output side of the pipe, so close that
				if( close(fd[1]) == -1)
					_exit(1) ; // TODO

				// READER gets stdin replaced
				if( dup2(fd[0], 0) == -1) 
					_exit(1) ; // TODO: error

				// run R
				execute_command( c->u.command[0], profiling ) ; 

				if( waitpid( p2, &status, 0 ) == -1) // status is p1's
				{
					_exit(1) ; // TODO
				}
				// close R's input fd
				if(close(fd[0]) == -1) 
					_exit(1) ; // TODO: more erroring

				_exit( c->u.command[1]->status ) ; // TODO: check this
			} // if we couldn't spawn a reader, we'll have no writer, TODO: verify this is right?

			if( waitpid( p1 , &status , 0 ) == -1 || !WIFEXITED(status) )
				_exit(1) ;

			c->status = WEXITSTATUS(status) ;
			break ;
		}
		case SEQUENCE_COMMAND: {
			execute_command(c->u.command[0], profiling) ;
			execute_command(c->u.command[1], profiling) ;
			c->status = (c->u.command[1]->status) ;
			break ;
		}
		case SIMPLE_COMMAND: {
			printf("executed ") ;
			int i = 0;
			while( c->u.word[i] != NULL) 
			{
				printf("%s ", c->u.word[i]) ;
				i ++ ;

			}
			printf("\n") ;
			break ;
		}
		case SUBSHELL_COMMAND: {
			int status ;
			pid_t retval ;
			pid_t p = fork() ;
			if(p == -1) {
				// handle error
			}
			else if(p == 0) {
				execute_command(c->u.command[0], profiling) ;
				_exit(c->u.command[0]->status) ;
			}
			else {
				retval = waitpid( p, &status, 0 ) ;
				if( retval == -1 || !WIFEXITED(status)) {
					//handle error
				}
				else { 
					c->status = WEXITSTATUS(status) ;
				}
			}
			break ;
		}
		case UNTIL_COMMAND: {
			do{
				execute_command(c->u.command[1], profiling) ;
				execute_command(c->u.command[0], profiling) ;
			} while ( c->u.command[0]->status == 0 ) ;
			c->status = c->u.command[1]->status ;
			break ;
		}
		case WHILE_COMMAND: {
			execute_command( c->u.command[0], profiling ) ;
			while( c->u.command[0]->status == 0 )
			{
				execute_command( c->u.command[1], profiling ) ;
				execute_command( c->u.command[0], profiling ) ;
			}
			c->status = c->u.command[1]->status ;
			break ;
		}
		default: 
			printf("not a command!, shouldn't have gotten here!\n") ;
			_exit(1) ;
	}
}
