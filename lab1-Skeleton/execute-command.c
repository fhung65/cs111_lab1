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
		case PIPE_COMMAND:
			execute_command( c->u.command[0], profiling ) ;
			// do stuff for redirect, maybe buffer
			execute_command( c->u.command[1], profiling ) ;
			c->status = c->u.command[1]->status ;
			break ;
		case SEQUENCE_COMMAND:
			execute_command(c->u.command[0], profiling) ;
			execute_command(c->u.command[1], profiling) ;
			c->status = (c->u.command[1]->status) ;
			break ;
		case SIMPLE_COMMAND:
			printf("executed ") ;
			int i = 0;
			while( c->u.word[i] != NULL)
			{
				printf("%s ", c->u.word[i]) ;
				i ++ ;
			}
			printf("\n") ;
			break ;
		case SUBSHELL_COMMAND:
			// ?
			break ;
		case UNTIL_COMMAND:
			do{
				execute_command(c->u.command[1], profiling) ;
				execute_command(c->u.command[0], profiling) ;
			} while ( c->u.command[0]->status == 0 ) ;
			c->status = c->u.command[1]->status ;
			break ;
		case WHILE_COMMAND:
			execute_command( c->u.command[0], profiling ) ;
			while( c->u.command[0]->status == 0 )
			{
				execute_command( c->u.command[1], profiling ) ;
				execute_command( c->u.command[0], profiling ) ;
			}
			c->status = c->u.command[1]->status ;
			break ;
	}
}
