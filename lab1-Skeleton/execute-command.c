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
			{
				fprintf(stderr, "failed to create reader\n") ;
				_exit(1) ; // TODO: more error handling later
			}
			else if( p1 == 0 ) {// IN CHILD for RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
				fprintf(stderr, "beginning reader process\n");
				
				// create the pipe 
				int fd[2] ;
				if( pipe( fd ) == -1 )
				{
					fprintf(stderr, "failed to create pipe\n" ) ;
					_exit(1) ; // TODO: more error handling later
				}

				//spawn a process for W
				pid_t p2 = fork() ; 
				if( p2 == -1 )
				{
					fprintf(stderr, "failed to make writer\n" ) ;
					// if we failed to make a writer, exit the reader
					_exit(1) ; // TODO: more erroring
				}
				else if( p2 == 0 ) { // IN CHILD for WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
					fprintf(stderr, "beginning writer process\n") ;
					// Writer doesn't need the input of the pipe, so close that
					if( close(fd[0]) == -1 )
					{
						fprintf(stderr, "failed to close read end of pipe in writer\n" ) ;
						_exit(1) ; // TODO
					}
					//fprintf(stdout, "can still print to stdout?\n") ;

					// WRITER turns stdout into fd[1]
					if( dup2(fd[1], STDOUT_FILENO) == -1) 
					{
						fprintf(stderr, "failed to swap outputs in writer\n" ) ;
						_exit(1) ; // TODO: error
					}
					//fprintf(stdout, "can still print to stdout?\n") ;
					// run W
					fprintf(stderr, "running writing command\n") ;
					execute_command( c->u.command[0], profiling ) ; 
					fprintf(stderr, "done with writing command\n") ;

					// close W's output fd  // do we need to close the new stdout? prolly not?, we're exiting
					if(close(fd[1]) == -1) 
					{	
						fprintf(stderr, "failed to close write end of writer\n") ; 
						_exit(1) ; // TODO: more error
					}

					fprintf(stderr, "exiting write process\n") ;
					_exit(0) ; // TODO: change later
				}//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
				// READER does't need the output side of the pipe, so close that
				if( close(fd[1]) == -1)
				{
					fprintf(stderr, "failed to close write end of pipe in reader\n") ;
					_exit(1) ; // TODO
				}
				// READER gets stdin replaced
				if( dup2(fd[0], STDIN_FILENO) == -1) 
				{
					fprintf(stderr, "failed to swap inputs in reader\n") ;
					_exit(1) ; // TODO: error
				}

				fprintf(stderr, "running read command\n") ;
				// run R
				execute_command( c->u.command[1], profiling ) ; 
				fprintf(stderr, "done with read command\n") ;

				if( waitpid( p2, &status, 0 ) == -1) // status is p1's
				{
					fprintf(stderr, "error in terminating writer\n") ;
					_exit(1) ; // TODO
				}
				// close R's input fd
				if(close(fd[0]) == -1) 
				{
					fprintf(stderr,"failed to close read end of pipe in reader\n") ;
					_exit(1) ; // TODO: more erroring
				}

				fprintf(stderr, "exiting read process\n") ;
				_exit( c->u.command[1]->status ) ; // TODO: check this
			} // if we couldn't spawn a reader, we'll have no writer, TODO: verify this is right? RRRRRRRRRRRRRR

			if( waitpid( p1 , &status , 0 ) == -1 || !WIFEXITED(status) )
			{
				fprintf(stderr, "error in terminating reader\n") ;
				_exit(1) ;
			}

			fprintf(stderr, "finished with pipe command\n") ;
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
			char input[100];
			if( fgets(input, 100, stdin) == NULL )
				fprintf(stderr, "error taking in input\n") ;
			else
				printf("command starting with %s took in input: %s", c->u.word[0], input ) ;

			fprintf(stderr, "executed  ") ;
			int i = 0;
			while( c->u.word[i] != NULL) 
			{
				fprintf(stderr, "%s ", c->u.word[i]) ;
				i ++ ;

			}
			fprintf(stderr, "\n") ;
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
