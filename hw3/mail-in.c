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

#define BUFF_SIZE 4096

static void die(const char *message) {
	perror(message);
	exit(1);
}

int main(int argc, char **argv) {

	char requestLine[1000];       
	
	while(1) {
		
		//Check if EOF
		int check;
		if ((check = checkEOF) == 1) {
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
			//skip to end of email
		} else {
			printf("Passed mail from check, mail from : %s\n", user);
			//trim user name;
			//validate username is in mail system 
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
					//skip
				}
				printf("Found DATA\n");
				break;
			}


			method = strtok(requestLine, separator);
			user = strtok(NULL, separator);

			if (strcasecmp(method, "rcpt to") != 0) {
				fprintf(stderr, "Error in request, skipping\n");
				//skip to end of email
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
