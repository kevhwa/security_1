#include <string.h> 
#include <stdlib.h>
#include "aes.h"
#include "sha256.h"
#include <stdio.h>
#include <ctype.h> /* For isascii */
#include <unistd.h>
// include others

#define MAX_NAME_BUFF 100 /* Maximum buffer for names */

static void die(const char *message)
{
	perror(message);
	exit(1);
}
	
void checkPassValid(char *password)
{
	int length;
	if((length = strlen(password)) > MAX_NAME_BUFF)
		die("Password too long");
	
	for(int i = 0; i < length; i++)
	{
		if(!isascii(password[i]))
			die("ASCII characters only");
	}
}

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		die("Invalid arguments");
	}

	argv++;
	char *func_name = *argv;
	char func_list[] = "list";

	/*
	//If command is list, list and end
	if(strcmp(func_name,func_list) == 1)
	{
		if(archiveExists(argv[2]))
		{
			listArchive();
		}

		goto func_end;
	}*/

	//From here, must have a password
	//Must add capability to see if password is given. Currently assuming password is always given
	argv++;
	char *pass_option = *argv;
	char pass_opt[] = "-p\0";
	char *password;
	argv++;

	// If password option is given
	if(strcmp(pass_option, pass_opt) == 0)
	{
		password = *argv;
		checkPassValid(password);
	} else {
		password = getpass("Please enter password");
		checkPassValid(password);
		argv += 2;
	}


	// Now we have password set and argv is aligned 
	
	// Verify if archive file exists. If it does, check pass. Otherwise create
	argv++;
	char *file_name = *argv;
	if(access(file_name, F_OK) != -1)
	{
		//encrypt password
		//check if password is correct
		//
	} else {
		
		// DO THIS FIRST
		//open file name in write mode to create file
		//encrypt password
		//add to file
		//close
		
		
		//will have to add file later

	
	
	}
		


func_end: 
	return 0;
}
