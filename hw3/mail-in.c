/*
Q
- how does mail out check who the recpient is? Does it read the email?
- can we assume input files will be properly formatted? like each message will end with a period, no typos etc
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>


#define BUFF_SIZE 4096
#define PTR_SIZE 8

static void die(const char *message) {
	perror(message);
	exit(1);
}

struct headers {
	char **rec_list;
	int size;
	int count;
};

int checkEOF(void); 
int checkValidUser(char *); 
void skipNext(void);
void resizeList(char **, int *);
int getSender(struct headers *); 
int getRecvr(struct headers *, int *);


int main(int argc, char **argv) {

	struct headers list;

	while(1) {
		list.rec_list = malloc (5 * sizeof(list.rec_list));
		list.size = 5;
		list.count = 0;
		int r_count = 0;

		//Check if EOF
		
		if (checkEOF() == 1) {
			printf("EOF REACHED\n");
			break;
		}

		if (getSender(&list) < 0) 
			continue;

		while (1) {
			if (getRecvr(&list, &r_count) < 0)
				break;
		}


		int fd[2];
		if (pipe(fd) < 0)
			die("pipe error");

		pid_t pid = fork();
		//char mailout[] = "mail-out";

		if(pid < 0) {
			die("Fork failed\n");
		} else if (pid == 0) {
			if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
				die("Can't reconnect stdin\n");
			close(fd[0]);
			close(fd[1]);
			
			if (execlp("./mail-out", "mail-out", NULL) < 0)
				die("execl failed\n");

		} else{
			sleep(1);
			close(fd[0]);

/*			if (write(fd[1], afromLine, strlen(afromLine)) != strlen(afromLine)) {
				die("write error\n");
			}
*/			
			for(int i = 0; i < list.count; i++) {
				char *temp = list.rec_list[i];
				if (write(fd[1], temp, strlen(temp)) != strlen(temp)) {
					die("write error\n");
				}
		
				printf("parent: %s\n", temp);
			}

		char requestLine[1000];

		//read in data 
		while (1) {
		
			if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
				die("fgets failed\n");	
			}
			
			if (strcmp(requestLine, ".\n" ) == 0 || strcasecmp(requestLine, "data\r\n") == 0 ) {
				printf("parent: reached end of data\n");
				if (write(fd[1], requestLine, strlen(requestLine)) != strlen(requestLine)) {
					die("write error\n");
				}

				break;	
			} else {
				if (write(fd[1], requestLine, strlen(requestLine)) != strlen(requestLine)) {
					die("write error\n");
				}


			}
		}

		close(fd[1]);
		for (int i = 0; i < list.count; i++) {
			free(list.rec_list[i]);
		}
		free(list.rec_list);

		printf("\nwaiting for child\n");
		pid_t childpid = waitpid(-1, NULL, 0);
		if (childpid < -1)
			die("waitpid failed\n");
		}
	}
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
/*
void resizeList(char **list, int *count) {
	char **temp = realloc(list, PTR_SIZE * *count * 2);
	*count = *count * 2;

	temp = temp;
}
*/
void addLine(struct headers *list, char *line) {
	list->rec_list[list->count++] = line;
	//list.count++;

	if (list->count == list->size) {
		printf("resizing list\n");
		char **temp = realloc(list->rec_list, PTR_SIZE * (list->size * 2));
		list->rec_list = temp;
		list->size *= 2;
	}


}

int getSender(struct headers *list) {
	
	char fromLine[1000];
	char *sender;
	char *separator = ":";
	char *method = "";

	//First check mail from 
	if (fgets(fromLine, sizeof(fromLine), stdin) == NULL) {
		die("fgets failed\n");	
	}
	
	char *afromLine = malloc(strlen(fromLine)); 
	strcpy(afromLine, fromLine);
	
	method = strtok(fromLine, separator);
	sender = strtok(NULL, separator);
	
	sender[strlen(sender) - 1] = '\0'; // get rid of new line
	sender[strlen(sender) - 1] = '\0'; // get rid of ending >
	sender++;

	if (strcasecmp(method, "mail from") != 0) {
		fprintf(stderr, "Error in request, skipping\n");
		skipNext();
		return -1;
	} else {
		printf("Passed mail from check, mail from : %s\n", sender);
		
		if (checkValidUser(sender) != 1) {
			printf("Not valid sender\n");
			skipNext();
			return -1;;
		}
	}

	addLine(list, afromLine);

	return 1;

}

int getRecvr(struct headers *list, int *r_count) {
	
	char requestLine[1000];
	char *separator = ":";
	char *method = "";
	char *user = "";

	if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
		die("fgets failed\n");	
	}

	char *recvr = malloc(strlen(requestLine)); 
	strcpy(recvr, requestLine);
	//recvr[strlen(requestLine) - 1] = '\0'; //overwrite newline
	//printf("debug: %s\n", requestLine);
	
	//check if this is the data line.
	if (strcasecmp(requestLine, "data\n") == 0 || strcasecmp(requestLine, "data\r\n") == 0)  {
		if (*r_count == 0) {
			fprintf(stderr, "DATA out of place, skipping\n");
			skipNext();
			return -1;
		}
		addLine(list, recvr);
		//printf("Found DATA\n");
		return -1;
	}
	
	// add to list of recipients
	method = strtok(requestLine, separator);
	user = strtok(NULL, separator);
	user[strlen(user) - 1] = 0;
	user++;

	if (strcasecmp(method, "rcpt to") != 0) {
		fprintf(stderr, "Error in request, skipping\n");
		skipNext();
		return -1;
	} else {
		printf("passed rcpt to test, sending to : %s\n", user);
		
		addLine(list, recvr);
		*r_count = *r_count + 1;
	}

	return 1;
}
