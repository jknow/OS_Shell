/*
Shell Prompt Program by
Drashti Patel
Sarid Shinwari
Bhargav Patel
Joe Komosinski
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shellhead.h"

cfunc cfunctions[2]; /* Static array of built-in functions */
static char ***command; /* Triple pointer to hold the commands */
int totalcommand; /* variable to keep track of the number of commands */
char ****garbage_tracker; /* Quadruple pointer to keep track of previously used commands */
int gci = 0; /* garbage collector iterator */

/*
 * This method will parse the tokens and store them into the
 * array named command
 */
void parse(char * pipeline,int * ncommand)
{
	command=((char ***)calloc(50,sizeof(char **)));
	int count=0;/*keeping track of position of string*/
	int len=strlen(pipeline);
	int startcount=0;/*used to store the token into array*/
	int endcount=0;
	int j=0;
	int com=0;
	*ncommand=0;/*total # of command*/
	int token=-1;
	
	command[com] = (char **)calloc(50,sizeof(char *));
	
	for(j=0;j<len;j++)
	{
		/*This will check for single quote ' token
		* if the token does not have ending '
		* then it will print the error mismatch and
		* exits*/
		if(pipeline[count]=='\'')
		{
			count++;
			startcount=count;
			char name=pipeline[count];
			/*check for the next coming ' to take token*/
			while(name!='\'' && count<len)
			{
				count++;
				name=pipeline[count];

			}
			if(count==len)
			{
				fprintf(stderr, "Quote Mismatch\n");
				gci++;
				garbage_tracker[gci-1] = command;
				free_data();
				exit(0);
			}
			/*store the token in array*/
			token++;
			command[com][token]=(char *)calloc(50,sizeof(char **));
			endcount=count;
			strncat(command[com][token],&pipeline[startcount],endcount-startcount);
			command[com][token][endcount-startcount]='\0';

			count++;
			j=count;
			
		}
		/*This will check for double quote " token
		* if the token does not have ending "
		* then it will print the error mismatch and
		* exits*/
		else if(pipeline[count]=='\"')
		{
			count++;
			startcount=count;
			char name=pipeline[count];
			/*check for the next coming " to take the token*/
			while(name!='\"' && count<len)
			{
				count++;
				name=pipeline[count];

			}
			if(count==len)
			{
				fprintf(stderr, "Quote Mismatch\n");
				gci++;
				garbage_tracker[gci-1] = command;
				free_data();
				exit(0);
			}
			/*store the token in array*/
			token++;
			command[com][token]=(char *)calloc(50,sizeof(char));
			endcount=count;

			strncat(command[com][token],&pipeline[startcount],endcount-startcount);
			command[com][token][endcount-startcount]='\0';

			count++;
			j=count;
		}
		/*This will check for space, new line and tab
		* and increment the counter*/
		else if(pipeline[count]==' ' || pipeline[count]=='\n' || pipeline[count]=='\t')
		{
			count++;
			j=count;
		}
		/*This will check for | and create the second command*/
		else if(pipeline[count]=='|')
		{
			startcount=count;
			count++;
			token++;
			command[com][token]=(char *)calloc(50,sizeof(char **));
			command[com][token]=0;
			j=count;
			/*create new command*/
			com++;
			(*ncommand)++;
			command[com] = (char **)calloc(50,sizeof(char *));
			token=-1;
		}
		/*This will check for regular character
		* and store the token in to the array*/
		else
		{
			startcount=count;
			count++;
			char name=pipeline[count];
			/*check for the next coming delimeter*/
			while(name!=' ' && name!='\0' && name!='\'' && name!='\"' && name!='|')
			{
				count++;
				name=pipeline[count];
				
			}
			token++;
			command[com][token]=(char *)calloc(50,sizeof(char **));
			if(pipeline[count]=='\0')
			{
				count--;
			}
			/*store the token in array*/
			endcount=count;
			strncat(command[com][token],&pipeline[startcount],endcount-startcount);
			command[com][token][endcount-startcount]='\0';
			j=count;
		}
	}
	
	
}

/*Function to pipe commands to each other and create child processes
 NOTE: This function was done with support from the following source:
 http://stackoverflow.com/questions/17630247/coding-multiple-pipe-in-c*/
void loop_pipe(char ***cmd)
{
	int   fd[2]; /*int array to hold the pipes*/
	pid_t pid; /* pid for the switch statement*/
	int   fd_in = 0; /*variable for the old input */
	int status; /*exit status of child*/
	int need2pipe = 1; /*boolean to check for built in functions. 
					 * If it is a built in function, no child process will be created */

	if ((cmd[0][50]) != NULL)/*check for a large number of arguments*/
	{
		fprintf(stderr, "Too many arguments\n");
		exit(0);
	}

	while (*cmd != NULL)/*check if there is still a valid command*/
	{
		int i;
		for(i = 0; i < (sizeof(cfunctions)/sizeof(cfunc)); i++)/*check for built in functions*/
		{
			if(strcmp(*cmd[0], cfunctions[i].funcname) == 0)
			{
				cfunctions[i].f(*cmd);
				cmd++;
				need2pipe = 0;/*There is no need for a child*/
				break;
			}
		}
		
		if(need2pipe)
		{
			pipe(fd); /*create a pipe between file descriptors*/
			if ((pid = fork()) == -1)
			{
				free_data();
				exit(EXIT_FAILURE);
			}
			else if (pid == 0) /*Child*/
			{
				dup2(fd_in, 0); /*duplicate the input according to what was being used*/
				if (*(cmd + 1) != NULL) /*check if there needs to be an output*/
				{
					dup2(fd[1], 1);
				}
				close(fd[0]); /*close the file descriptor*/
				execvp((*cmd)[0], *cmd); /*start a new process*/
				perror("execvp failure");
				free_data();
				exit(EXIT_FAILURE);
			}
			else
			{
				if ((pid=wait(&status))!=1) /*parent waits for the child*/
				{
					fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
				}
				close(fd[1]);
				fd_in = fd[0]; /*save the input for the next command*/
				cmd++;/*iterates to the next command*/
			}
		}
		else
		{
			need2pipe = 1;
		}
	}
}

/*built in change directory function*/
int customcd(char **cdcmd)
{
	char* cddestination = cdcmd[1];
	char curdir[1024];/*short for CURrent DIRectory*/
	
	if (cdcmd[2] != NULL) /*check for multiple arguments*/
	{
		fprintf(stderr, "cd: More than one argument is present\n");
		free_data();
		exit(EXIT_FAILURE);
	}
	else if (cddestination == NULL) /*check for any arguments*/
	{
		cddestination = getenv("HOME");
	}
	
	if(chdir(cddestination) != 0)/*error checking*/
	{
		perror("Changing directory failed");
		free_data();
		exit(EXIT_FAILURE);
	}
	else /*at success, prints out the current directory*/
	{
		getcwd(curdir, sizeof(curdir));
		printf("%s\n", curdir);
	}
	return 0;
}

/*Built in exit function*/
int customexit(char **exitcmd)
{
	int status = 0; /*exit status if needed*/
	
	if(exitcmd[1] != NULL) /* check and assign exit status*/
	{
		status = strtol(exitcmd[1], NULL, 0);
	}
	
	free_data();
	
	exit(status); /*exit*/
	
	return 0; /*useless but avoids complier issues*/
}

int main()
{
        /*Assigns the custom functions to the static array*/
	cfunctions[0].funcname = "cd";
	cfunctions[0].f = &customcd;
	cfunctions[1].funcname = "exit";
	cfunctions[1].f = &customexit;
	
	garbage_tracker = calloc(1000 , sizeof(char****));
	char word_buffer[1024];/*initializes the buffer to fget the input*/

	if (isatty(0)) /*check for terminal input*/
	{
		printf("<3: ");
	}
	while (fgets(word_buffer, 1024, stdin) != NULL) 
	{
		parse(word_buffer, &totalcommand);/*parser function*/

		/*Assign commands to quadruple pointer for memory clean up later*/
		gci++;
		garbage_tracker[gci-1] = command;
		
		if (command[0][0] != NULL)/*check for blank input*/
		{
			loop_pipe(command);
		}

		if (isatty(0)) /*another check for terminal input*/
		{
			printf("<3: ");
		}

	}
	
	free_data();
	exit(0);
	
}

/*function to clean up any callocs done throughout the program*/
void free_data()
{
	int freei;
	for(freei = 0; freei < gci; freei++)
	{
		int i, j;
		for(i=0; i<=totalcommand; i++)
		{
			for(j=0; j<50; j++)
			{
				free(garbage_tracker[freei][i][j]);
			}
			free(garbage_tracker[freei][i]);
		}
		free(garbage_tracker[freei]);
	}
	free(garbage_tracker);
}