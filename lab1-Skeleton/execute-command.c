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
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

// I think ret -1 on file fail, otherwise fd of log file
// will be called by main to open/create a file
// or should we just create the file, and have processes use open to
// write to it, with exclusive flags? I like this second one better

int
prepare_profiling (char const *name)
{
  return open( name, O_WRONLY | O_APPEND | O_CREAT,0666 ) ;
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

// things to consider: 
// 		exiting mid execution
//			exit in main/ read-command: hmm...
//			exit in execute command -> 
//				parent: 'tis fine?, why not record time values anyways
//				child: hmm, 
// returns -1 on fail, 0 on success
int tlog( int fd, struct timespec* start, command_t cmd, pid_t p)
{	
	if(fd == -1) //not most accurrate time reading if this is here 
		return -1 ;

	struct timespec abs;
	struct timespec end;
	struct timespec resa ;
	struct timespec resb ;
	struct rusage use;
	struct flock fl ;
	fl.l_type =  F_WRLCK;
	fl.l_whence = SEEK_SET ;
	fl.l_start = 0 ;
	fl.l_len = 0 ;
	
	if( clock_gettime(CLOCK_REALTIME, &abs) == -1 )
		return -1 ;
	if( clock_gettime(CLOCK_MONOTONIC, &end) == -1 )
		return -1 ;
	if( getrusage(RUSAGE_CHILDREN, &use ) == -1 ) 
		return -1 ;
	if( clock_getres(CLOCK_REALTIME, &resa) == -1 )
		return -1 ;
	if( clock_getres(CLOCK_MONOTONIC, &resb) == -1 )
		return -1 ;

	if( fcntl( fd, F_SETLKW, &fl ) == -1 )
		return -1 ;
	
	long a_nsec = abs.tv_nsec - (abs.tv_nsec % resa.tv_nsec) ;
	long e_nsec = end.tv_nsec - (end.tv_nsec % resb.tv_nsec) ;
	long s_nsec = start->tv_nsec - (start->tv_nsec % resb.tv_nsec) ;
	long cmd_sec = end.tv_sec - start->tv_sec ;
	long cmd_nsec;
	if(e_nsec > s_nsec)
		cmd_nsec = (e_nsec - s_nsec); 
	else
	{
		cmd_sec-- ;
		cmd_nsec = 1000000000 - (s_nsec - e_nsec) ;
	}

	dprintf(fd, "%li.%09li %li.%09li %li.%06li %li.%06li",
			abs.tv_sec, a_nsec,
			end.tv_sec - start->tv_sec, cmd_nsec,
			use.ru_utime.tv_sec, use.ru_utime.tv_usec,
			use.ru_stime.tv_sec, use.ru_stime.tv_usec ) ;
	
	if( cmd != NULL )
	{
		int i = 0;
		while( cmd->u.word[i] != NULL )
		{
			if( strcmp(cmd->u.word[i], "\n") )
				dprintf(fd, " %s", cmd->u.word[i] ) ;
			i++ ;
		}
	}
	else if( p > 0 )
	{
		dprintf(fd, " [%d]", p ) ;
	}

	dprintf(fd, "\n") ;

	fl.l_type = F_UNLCK ;
	if( fcntl( fd, F_SETLK, &fl ) == -1 )
		return -1 ;

	return 0 ;
}

//
//		do nothing if profiling == -1 => can probably leave this to log()
//		right before each fork, get a start time,
//		at the end of each, call log()
void
execute_command (command_t c, int* profiling)
{
	switch( c->type )
	{
		case IF_COMMAND:
		{
			int status ;
			struct timespec start ;
			if( *profiling != -1) // grab the start time
				if( clock_gettime(CLOCK_MONOTONIC, &start) == -1 )
					*profiling = -1 ;

			pid_t p = fork();
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

			if( *profiling != -1)
				if ( tlog( *profiling, &start, NULL, p ) == -1 ) 
					*profiling = -1 ; // log the process

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
			struct timespec startA ;
			if( *profiling != -1) // grab the start time
				if( clock_gettime(CLOCK_MONOTONIC, &startA ) == -1 )
					*profiling = -1 ;

			pid_t p1 = fork() ; // FORK A CHILD FOR R
			if( p1 == -1 ) 
  				error (1, 0, "failed to create reader\n");
			else if( p1 == 0 ) 
			{// IN CHILD for R
				
				// create the pipe 
				int fd[2] ;
				if( pipe( fd ) == -1 )	
  					error (1, 0, "failed to create pipe\n");

				
				struct timespec startB ;
				if( *profiling != -1) // grab the start time
					if( clock_gettime(CLOCK_MONOTONIC, &startB) == -1 )
						*profiling = -1 ;

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
			
				if( *profiling != -1)
					if ( tlog( *profiling, &startB, NULL, p2 ) == -1 ) 
						*profiling = -1 ; // log the process
				
				//fd[0] and the new STDIN_FILENO are closed at exit
				_exit( c->u.command[1]->status ) ; // TODO: check this
			} // if we couldn't spawn a reader, we'll have no writer

			if( waitpid( p1 , &status , 0 ) == -1 || !WIFEXITED(status) ) 
  				error (1, 0, "Reader terminated with error\n");
			
			if( *profiling != -1 )
				if( tlog( *profiling, &startA, NULL, p1 ) == -1 ) 
					*profiling = -1 ; // log the process

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
		  int is_exec = !strcmp(c->u.word[0], "exec") ;
		  char** word_arr = c->u.word;
		  struct timespec start ;
		  pid_t pid ;
		  if( !is_exec ) // don't fork for exec
		  {
		  	if( *profiling != -1) // grab the start time
				if( clock_gettime(CLOCK_MONOTONIC, &start) == -1 )
					*profiling = -1 ;
		  	pid = fork();
		  }
		  else
		  	word_arr ++;
		  if(pid < 0) error(3, 0, "Failure to fork process");
		  else if (pid == 0 || is_exec){ //CHILD or exec
		  	setup_io( c );
		    execvp(word_arr[0], word_arr);  //let it be known that I am an idiot. The first element of the argument array is the command itself. anything else will result in errors
		    error(5, 0, "%s: command not found\n", c->u.word[0]);
		  }
		  int status;
		  // kinda janky, but in the special case of exec, this wait isn't called
		  pid_t returned = waitpid(pid, &status, 0);
		  
		  if( *profiling != -1)// log the process
				if ( tlog( *profiling, &start, c, -1 ) == -1 ) 
					*profiling = -1 ; 
		  
		  if(returned < 0 || !WIFEXITED(status))
		    error(6, 0, "Process failed to return");
		  //some stuff to process the status
		  
		  c->status = WEXITSTATUS(status);
		  
		  break ;
		  
		}
		case SUBSHELL_COMMAND: 
		{
			int status ;
			struct timespec start ;
			if( *profiling != -1) // grab the start time
				if( clock_gettime(CLOCK_MONOTONIC, &start) == -1 )
					*profiling = -1 ;
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
			
			if( *profiling != -1)
				if ( tlog( *profiling, &start, NULL, p ) == -1 ) 
					*profiling = -1 ; // log the process
			
			c->status = WEXITSTATUS(status) ;
			
			break ;
		}
		case UNTIL_COMMAND: 
		{
			int status ;
			struct timespec start ;
			if( *profiling != -1) // grab the start time
				if( clock_gettime(CLOCK_MONOTONIC, &start) == -1 )
					*profiling = -1 ;
			
			pid_t p = fork();
			if( p == -1 )
				error( 1, 0, "failed to create process\n");
			else if( p == 0 ) // Kiddy land!
			{
				setup_io( c ) ;
				do{
					execute_command(c->u.command[1], profiling) ;
					execute_command(c->u.command[0], profiling) ;
				} while ( c->u.command[0]->status != 0 ) ;
				_exit( c->u.command[1]->status );
			}
			
			// parent land! wait for your kids, then set your status or cry
			if( waitpid( p , &status , 0 ) == -1 || !WIFEXITED(status) ) 
			{
  				error (1, 0, "Until command terminated with error\n");
			}

			if( *profiling != -1)
				if ( tlog( *profiling, &start, NULL, p ) == -1 ) 
					*profiling = -1 ; // log the process

			c->status = WEXITSTATUS(status) ;
			break ;
		}
		case WHILE_COMMAND: 
		{
			int status ;
			struct timespec start ;
			
			if( *profiling != -1) // grab the start time
				if( clock_gettime(CLOCK_MONOTONIC, &start) == -1 )
					*profiling = -1 ;
			
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

			if( *profiling != -1)
				if ( tlog( *profiling, &start, NULL, p ) == -1 ) 
					*profiling = -1 ; // log the process
			
			c->status = WEXITSTATUS(status) ;

			break ;
		}
		default: 
			printf("not a command!, shouldn't have gotten here!\n") ;
			_exit(1) ;
	}
}
