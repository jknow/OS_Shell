/* 
 * File:   shellhead.h
 * Authors: 
	Drashti Patel
	Sarid Shinwari
	Bhargav Patel
	Joe Komosinski
 *
 * Created on February 13, 2014, 3:06 PM
 */

#ifndef SHELLHEAD_H
#define	SHELLHEAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* struct to hold functions */
typedef struct customfunctions
{
	char* funcname;
	int (*f)(char**);
}cfunc;

/*This method will parse the tokens and store them into the array named command*/
void parse(char * pipeline,int * ncommand);

/*Function to pipe commands to each other and create child processes*/
void loop_pipe(char ***cmd);

/*built in change directory function*/
int customcd(char **cdcmd);

/*Built in exit function*/
int customexit(char **exitcmd);

/*function to clean up any callocs done throughout the program*/
void free_data(void);

#endif	/* SHELLHEAD_H */

