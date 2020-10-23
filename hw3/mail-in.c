/*
1. get email
- read in with fgets. This will get us sender and receiver until you hit DATA
- Verify that sender is in the mailbox
       - if not send error message but continue

2. fork, create a pipe from OS and call mail-out and pipe in the corresponding data

3. printf sender message, then printf all the receiver messages
4. fgets and check until you see just a .
5. kill child process somehow
6. loop back to 3


Q
- how does mail out check who the recpient is? Does it read the email?
- can we assume input files will be properly formatted? like each message will end with a period, no typos etc
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

#define BUFF_SIZE 4096

static void die(const char *message) {
	perror(message);
	exit(1);
}

int checkEOF(void); 
int checkValidUser(char *); 
void skipNext(void);

int main(int argc, char **argv) {

	char requestLine[1000];       
	
	while(1) {
		
		//Check if EOF
		int check;
		if ((check = checkEOF()) == 1) {
			printf("EOF REACHED\n");
			break;
		}
		
		//First check mail from 
		if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
			die("fgets failed\n");	
		}

		char *separator = ":";
		char *method = "";
		char *user = "";

		method = strtok(requestLine, separator);
		user = strtok(NULL, separator);

		if (strcasecmp(method, "mail from") != 0) {
			fprintf(stderr, "Error in request, skipping\n");
			skipNext();
			continue;
		} else {
			printf("Passed mail from check, mail from : %s\n", user);
			//trimming name

			printf("length of name: %ld\n", strlen(user));
			user[strlen(user) - 1] = 0;
			user[strlen(user) - 1] = 0;
			user++;
			printf("trimmed user: %s\n", user);

			if (checkValidUser(user) != 1) {
				printf("Not valid sender\n");
				skipNext();
				continue;
			}
		}

		int r_count = 0;
		while (1) {
			// Then check for rcpt. This part should be in a loop
			if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
				die("fgets failed\n");	
			}

			printf("debug: %s\n", requestLine);
	
			//check if this is the data line.
			if (strcasecmp(requestLine, "data\n") == 0 || strcasecmp(requestLine, "data\r\n") == 0)  {
				if (r_count == 0) {
					fprintf(stderr, "DATA out of place, skipping\n");
					skipNext();
					break;
				}
				printf("Found DATA\n");
				break;
			}


			method = strtok(requestLine, separator);
			user = strtok(NULL, separator);

			if (strcasecmp(method, "rcpt to") != 0) {
				fprintf(stderr, "Error in request, skipping\n");
				skipNext();
				break;
			} else {
				printf("passed rcpt to test, sending to : %s\n", user); 
				r_count++;
				//trim user name;
			}

		}

		//read in data 
		while (1) {
		
			if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
				die("fgets failed\n");	
			}
			
			if (strcmp(requestLine, ".\n" ) == 0 || strcasecmp(requestLine, "data\r\n") == 0 ) {
				printf("reached end of data\n");
				break;	
			} else {
				
				printf("read data: \n");
				printf("%s\n", requestLine);
			}
		}
	}
        // read in from standard in, this will give us the first line of MAIL FROM
        // verify that this user exists. If not, print to std err and loop until you hit the "."

        //fork or read in entire file, create a file then just read the whole thing to child in one batch

        //kill child process
        //loop back

}

int checkEOF(void) {

	int temp;
	if ((temp = fgetc(stdin)) != EOF) {
		ungetc(temp, stdin);
	} else {
		if(ferror(stdin) != 0) {
			die("error in fgetc\n");
		}
		return 1;
	}

	return 0;
}

int checkValidUser(char *user) {

	struct dirent *dp;
	DIR *dfd;

	//*********** need to change when you move exec to bin******
	if ((dfd = opendir("test/mail")) == NULL) {
		die("Can't open mail dir\n");
	}

	//char dir_name[100];
	//char new_name_qfd[100];

	while ((dp = readdir(dfd)) != NULL)
	{
		
		if (strcmp(user, dp->d_name) == 0) {
			printf("user found\n");
			return 1;
		} else {
			continue;
		}
		
	}

	return 0;
}

void skipNext(void) {

	printf("Skipping to next mail\n");
	char requestLine[1000];

	while (1) {
	
		if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
			die("fgets failed\n");	
		}
			
		if (strcmp(requestLine, ".\n" ) == 0 || strcasecmp(requestLine, "data\r\n") == 0 ) {
			printf("reached end of data\n");
			return;	
		}
	}
}

// misc code from checking directory name
//
//struct stat stbuf;
//		sprintf(dir_name, "%s"dp->d_name);
//		if(stat(dir_name, &stbuf ) == -1 ) {
//			printf("Unable to stat file: %s\n",dir_name) ;
//			continue ;
//		}

		//if ((stbuf.st_mode & S_IFMT) == S_IFDIR ) {
		//	continue;
			// Skip directories
		//}
//		else
//		{
//			char* new_name = get_new_name( dp->d_name ) ;// returns the new string
//			// after removing reqd part
//			sprintf(new_name_qfd,"%s/%s",dir,new_name) ;
//			rename( filename_qfd , new_name_qfd ) ;

//		}

