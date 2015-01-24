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
#include <sys/stat.h>
#include <fcntl.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
prepare_profiling (char const *name)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  error (0, 0, "warning: profiling not yet implemented\n");
  return -1;
}

int
command_status (command_t c)
{
  return c->status;
}


// called by conditionals
// sets io of subcommands of c to c's io
// for subcommands with no io already specified
// so priority is given to closest io
// doesn't mess with file descriptors
void
setup_io( command_t c ) // called by conditionals and compounds and subshells
{
	int in, out;
	if( c->input != NULL )
	{
		in = open(c->input, O_RDONLY);
		if(in == -1) error(12, 0, "Couldn't open input");
		if(dup2(in,0) < 0) error(13, 0, "Couldn't set STDIN");
	}
	
	if( c->output != NULL )
	{
		out = open(c->output, O_WRONLY | O_CREAT | O_TRUNC,
				 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
				 S_IROTH | S_IWOTH);
		if(out == -1) error(14, 0, "Couldn't open output");
		if(dup2(out,1) < 0) error(15, 0, "Couldn't set STDOUT");
	}
}

void
execute_command (command_t c, int profiling)
{
	switch( c->type )
	{
		case IF_COMMAND:
		{
			int status ;
			pid_t p = fork();
			setup_io(c);
			if( p == -1 )
				error(1 , 0 , "failed to create new process\n");
			else if( p == 0 ) // Kiddy land!
			{
				setup_io( c ) ;
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
				_exit( c->status ) ; // TODO: reduce memory access later
			}
			// parent's land!
			if( waitpid( p, &status, 0 ) == -1 || !WIFEXITED(status)) 
  				error (1, 0, "Subshell command terminated with error\n");
			
			c->status = WEXITSTATUS(status) ;
			break ;
		}
		case PIPE_COMMAND: { // W | R 
			if(c->input != NULL)
				if( c->u.command[0]->input == NULL)
					c->u.command[0]->input = c->input ;
			if(c->output != NULL)
				if( c->u.command[1]->output == NULL)
					c->u.command[1]->output = c->output ;
			
			int status ;

			pid_t p1 = fork() ; // FORK A CHILD FOR R
			if( p1 == -1 ) 
  				error (1, 0, "failed to create reader\n");
			else if( p1 == 0 ) 
			{// IN CHILD for R
				
				// create the pipe 
				int fd[2] ;
				if( pipe( fd ) == -1 )	
  					error (1, 0, "failed to create pipe\n");

				//spawn a process for W
				pid_t p2 = fork() ; 
				if( p2 == -1 ) 	
  					error (1, 0, "failed to create writer\n");
				else if( p2 == 0 ) 
				{ // IN CHILD for W
					
					// Writer doesn't need the input of the pipe, so close that
					if( close(fd[0]) == -1 ) 
  						error (1, 0, "failed to close input of pipe in writer\n");

					// WRITER turns stdout into fd[1]
					if( dup2(fd[1], STDOUT_FILENO) == -1) 
  						error (1, 0, "failed to reassign stdout in writer\n");
					
					// run W
					execute_command( c->u.command[0], profiling ) ; 

					_exit(0) ; // TODO: change exit status later?
				}

				// READER does't need the output side of the pipe, so close that
				if( close(fd[1]) == -1)	
  					error (1, 0, "failed to close write end of pipe in reader\n");

				// READER gets stdin replaced
				if( dup2(fd[0], STDIN_FILENO) == -1) 
  					error (1, 0, "failed to swap inputs in reader\n");

				// run R
				execute_command( c->u.command[1], profiling ) ; 

				if( waitpid( p2, &status, 0 ) == -1) 
  					error (1, 0, "error in terminating writer\n");
				
				//fd[0] and the new STDIN_FILENO are closed at exit
				_exit( c->u.command[1]->status ) ; // TODO: check this
			} // if we couldn't spawn a reader, we'll have no writer

			if( waitpid( p1 , &status , 0 ) == -1 || !WIFEXITED(status) ) 
  				error (1, 0, "Reader terminated with error\n");

			c->status = WEXITSTATUS(status) ;
			break ;
		}
		case SEQUENCE_COMMAND: 
		{
			execute_command(c->u.command[0], profiling) ;
			execute_command(c->u.command[1], profiling) ;
			c->status = (c->u.command[1]->status) ;
			break ;
		}
		case SIMPLE_COMMAND: {
		 // int in,out;
		  pid_t pid = fork();
		  if(pid < 0) error(3, 0, "Failure to fork process");
		  else if (pid == 0){ //CHILD
		  	setup_io( c );
		    execvp(c->u.word[0], c->u.word);  //let it be known that I am an idiot. The first element of the argument array is the command itself. anything else will result in errors
		    error(5, 0, "%s: command not found\n", c->u.word[0]);
		  }
		  int status;
		  pid_t returned = waitpid(pid, &status, 0);
		  if(returned < 0 || !WIFEXITED(status))
		    error(6, 0, "Process failed to return");
		  //some stuff to process the status
		  
		  c->status = WEXITSTATUS(status);
		  
		  break ;
		  
		}
		case SUBSHELL_COMMAND: 
		{
			int status ;
			pid_t p = fork() ;
			if(p == -1) 
			{
  				error (1, 0, "failed to spawn subshell\n");
			}
			else if(p == 0) // kiddy land!
			{
				setup_io( c ) ;
				execute_command(c->u.command[0], profiling) ;
				_exit(c->u.command[0]->status) ;
			}
			// parent's land!
			if( waitpid( p, &status, 0 ) == -1 || !WIFEXITED(status)) 
			{
  				error (1, 0, "Subshell command terminated with error\n");
			}
			c->status = WEXITSTATUS(status) ;
			
			break ;
		}
		case UNTIL_COMMAND: 
		{
			int status;
			pid_t p = fork();
			if( p == -1 )
				error( 1, 0, "failed to create process\n");
			else if( p == 0 ) // Kiddy land!
			{
				setup_io( c ) ;
				do{
					execute_command(c->u.command[1], profiling) ;
					execute_command(c->u.command[0], profiling) ;
				} while ( c->u.command[0]->status == 0 ) ;
				_exit( c->u.command[1]->status );
			}
			
			// parent land! wait for your kids, then set your status or cry
			if( waitpid( p , &status , 0 ) == -1 || !WIFEXITED(status) ) 
			{
  				error (1, 0, "Until command terminated with error\n");
			}

			c->status = WEXITSTATUS(status) ;
			break ;
		}
		case WHILE_COMMAND: 
		{
			int status ;
			pid_t p = fork() ;
			
			if( p == -1)
				error( 1, 0, "falied to create process\n") ;
			else if( p == 0 ) // Kiddy land!
			{
				setup_io( c ) ;
				execute_command( c->u.command[0], profiling ) ;
				while( c->u.command[0]->status == 0 )
				{
					execute_command( c->u.command[1], profiling ) ;
					execute_command( c->u.command[0], profiling ) ;
				}
				_exit( c->u.command[1]->status );
			}
			// parent land! wait for your kids, then set your status or cry
			if( waitpid( p , &status , 0 ) == -1 || !WIFEXITED(status) ) 
			{
  				error (1, 0, "While command terminated with error\n");
			}
			c->status = WEXITSTATUS(status) ;

			break ;
		}
		default: 
			printf("not a command!, shouldn't have gotten here!\n") ;
			_exit(1) ;
	}
}
