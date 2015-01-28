void
setup_io( command_t c ) // called by conditionals and compounds and subshells
{
	int in, out;
	if( c->input != NULL )
	{
		in = open(c->input, O_RDONLY);
		if(in == -1) error(12, 0, "Couldn't open input");
		if(dup2(in,0) < 0) error(13, 0, "Couldn't set STDIN");
		//for(i = 0 ; i < 3 ; i++)
		//	if(c->u.command[i] != NULL && c->u.command[i]->input == NULL)
		//		c->u.command[i]->input = c->input ;
	}
	
	if( c->output != NULL )
	{
		out = open(c->output, O_WRONLY | O_CREAT | O_TRUNC,
				 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
				 S_IROTH | S_IWOTH);
		if(out == -1) error(14, 0, "Couldn't open output");
		if(dup2(out,1) < 0) error(15, 0, "Couldn't set STDOUT");
		//for(i = 0 ; i < 3 ; i++)
			//if(c->u.command[i] != NULL && c->u.command[i]->output == NULL)
				//c->u.command[i]->output = c->output ;
	}
}
// NEW IMPLEMENTATION ^^^^^^^^^^^^^^^^^^^^^^^

// old implementation VVVVVVVVVVVv
void
setup_io( command_t c ) // called by conditionals and compounds and subshells
{
	// we don't wanna waste time if this setup will have no effect
	// ie, not one of the below commands
	//if(  c->type != IF_COMMAND &&
	//	 c->type != WHILE_COMMAND && 
	//	 c->type != UNTIL_COMMAND && 
	//	 c->type != SUBSHELL_COMMAND )
	//	return ;

	int i;
	if( c->input != NULL )
	{
		for(i = 0 ; i < 3 ; i++)
			if(c->u.command[i] != NULL && c->u.command[i]->input == NULL)
				c->u.command[i]->input = c->input ;
	}
	
	if( c->output != NULL )
	{
		for(i = 0 ; i < 3 ; i++)
			if(c->u.command[i] != NULL && c->u.command[i]->output == NULL)
				c->u.command[i]->output = c->output ;
	}
}
