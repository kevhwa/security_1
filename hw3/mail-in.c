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
#include <sys/stat.h>

#define BUFF_SIZE 50
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
int checkMailCount(char *);
FILE *getTempFile(struct headers *);
char *getMailCountString(char *);
char *getFromString(struct headers *);
char *getToString(struct headers *);

int main(int argc, char **argv) {

	struct headers list;

	while(1) {
		list.rec_list = malloc (5 * sizeof(list.rec_list));
		list.size = 5;
		list.count = 0;
		int r_count = 0;

		//Check if EOF
		
		if (checkEOF() == 1) {
			//printf("EOF REACHED\n");
			break;
		}

		if (getSender(&list) < 0) 
			continue;

		int flag = 0;
		while (1) {
			int x = getRecvr(&list, &r_count);
			if (x < 0) {
				flag = 1;
				break;
			} else if (x == 0)
				break;
		}
		
		//print something to err?
		if (flag == 1) 
			continue;

		//write from and to to temp file

		FILE *temp_fp = getTempFile(&list);

		char *msg_from = getFromString(&list);
		char *msg_to = getToString(&list);


		fwrite(msg_from, 1, strlen(msg_from), temp_fp);
		fwrite(msg_to, 1, strlen(msg_to), temp_fp);

		char requestLine[BUFF_SIZE];
		//read in data and write to tmp
		while (1) {
		
			if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
				die("fgets failed\n");	
			}
			
			if (strcmp(requestLine, ".\n" ) == 0 || strcasecmp(requestLine, "data\r\n") == 0 ) {
				printf("parent: reached end of data\n");
				fputs(requestLine, temp_fp);
				break;	
			} else {
				fputs(requestLine, temp_fp);
			}
		}

		for(int i = 1; i < list.count; i++) { //ignoring sender
			fseek(temp_fp, 0, SEEK_SET);
			char *temp = list.rec_list[i];
		
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
			
				if (execlp("./mail-out", "mail-out", temp, NULL) < 0)
					die("execl failed\n");

			} else{
				sleep(1);
				close(fd[0]);

				while (1) {
	
					if (fgets(requestLine, sizeof(requestLine), temp_fp) == NULL) {    
						die("fgets failed\n");	//includes new line
					}

					printf("%s", requestLine);

					if (strcmp(requestLine, ".\n" ) == 0 || strcasecmp(requestLine, "data\r\n") == 0 ) {
						printf("reached end of data\n");
						write(fd[1], requestLine, strlen(requestLine));
						break;	
					} else {
						write(fd[1], requestLine, strlen(requestLine));
						//the buffer was full and last char is null terminated
						//buffer is not full and there is a /n somewhere
						//if (requestLine[strlen(requestLine) - 1] == '\n') {
							//write(fd[1], "\n", 1);
				
						//}
					}

				}

			
			}


			close(fd[1]);
			int status;
			printf("\nwaiting for child\n");

			if (waitpid(pid, &status, 0) < 0) {
				die("waitpid failed\n");
			}

			if (WIFEXITED(status)) {
			        int es = WEXITSTATUS(status);

				if (es < 0) {
					fprintf(stderr, "Invalid recipient\n");
				}

				printf("Exit status was %d\n", es);
			}
	
		}


		for (int i = 0; i < list.count; i++) {
			free(list.rec_list[i]);
		}
		free(list.rec_list);
		
		
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
			//printf("user found\n");
			return 1;
		} else {
			continue;
		}
		
	}

	return 0;
}

void skipNext(void) {

	printf("Skipping to next mail\n");
	char requestLine[BUFF_SIZE];

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
	
	char fromLine[BUFF_SIZE];
	char *sender;
	char *separator = ":";
	char *method = "";

	//First check mail from 
	if (fgets(fromLine, sizeof(fromLine), stdin) == NULL) {
		die("fgets failed\n");	
	}
	
	
	method = strtok(fromLine, separator);
	sender = strtok(NULL, separator);
	
	sender[strlen(sender) - 1] = '\0'; // get rid of new line
	sender[strlen(sender) - 1] = '\0'; // get rid of ending >
	sender++;

	printf("sender: %s\n", sender);

	if (strcasecmp(method, "mail from") != 0) {
		fprintf(stderr, "Error in request, skipping\n");
		skipNext();
		return -1;
	} else {
		//printf("Passed mail from check, mail from : %s\n", sender);
		
		if (checkValidUser(sender) != 1) {
			printf("Not valid sender\n");
			skipNext();
			return -1;;
		}
	}

	char *sendername = malloc(strlen(sender) + 1); 
	strcpy(sendername, sender);
	sendername[strlen(sender)] = '\0';

	addLine(list, sendername);
	return 1;

}

int recvrExists(struct headers *list, char *recvr) {

	for(int i = 1; i < list->count; i++) { //ignoring sender
		char *temp = list->rec_list[i];
		printf("trying to match: %s\n", recvr);
		if (strcmp(temp, recvr) == 0){
			return 1;
		}
	}
	return 0;
}

int getRecvr(struct headers *list, int *r_count) {
	//printf("inside get recvr\n");

	char requestLine[BUFF_SIZE];
	char *separator = ":";
	char *method = "";
	char *user = "";

	if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
		die("fgets failed\n");	
	}

	
	//check if this is the data line.
	if (strcasecmp(requestLine, "data\n") == 0 || strcasecmp(requestLine, "data\r\n") == 0)  {
		if (*r_count == 0) {
			fprintf(stderr, "DATA out of place, skipping\n");
			skipNext();
			return -1;
		}
		//addLine(list, recvr);
		//printf("Found DATA\n");
		return 0;
	}
	
	// add to list of recipients
	method = strtok(requestLine, separator);
	user = strtok(NULL, separator);
	user[strlen(user) - 1] = '\0';
	user[strlen(user) -1] = '\0';
	user++;

	printf("recipients %s\n", user);

	if (strcasecmp(method, "rcpt to") != 0) {
		fprintf(stderr, "Error in request, skipping\n");
		skipNext();
		return -1;
	} else {
		//printf("passed rcpt to test, sending to : %s\n", user);
		if (!recvrExists(list, user)) {
			char *recvr = malloc(strlen(user) + 1);
			strcpy(recvr, user);
			recvr[strlen(user)] = '\0';
			addLine(list, recvr);
			*r_count = *r_count + 1;
		}
	}

	return 1;
}

int checkMailCount(char *user) {
	
	//printf("\nInside checkMailCount function\n");

	struct dirent *dp;
	DIR *dfd;
	int count = 0;
	char path[] = "test/mail/";
	char dir_name[strlen(path) + strlen(user) + 1];
	struct stat st;

	//create "test/mail/username"
	strcpy(dir_name, path);
	strcat(dir_name, user);
	//printf("path name %s\n", dir_name);

	//*********** need to change when you move exec to bin******
	if ((dfd = opendir(dir_name)) == NULL) {
		die("Can't open mail dir in checkMailCount\n");
	}

	while ((dp = readdir(dfd)) != NULL) {
		
		char filePath[strlen(dir_name) + 1 + strlen(dp->d_name) + 1];

		strcpy(filePath, dir_name);
		strcat(filePath, "/");
		strcat(filePath, dp->d_name);
		
		if(stat(filePath, &st) == -1 ) {
			printf("Unable to stat file: %s\n",filePath) ;
			continue ;
		}
		if (S_ISDIR(st.st_mode)) {
			continue;
		}

		count++;
	}

	if (count >= 99999) {
		die("too much mail\n");
	}
	//printf("mail count: %d\n", count);
	return count;
}



char *getMailCountString(char *user) {
	int mailCount = checkMailCount(user) + 1;
	//printf("written mail count %d\n", mailCount);

	int dig = 1;
	for (int i = 0; i < 5; i++) {
		if(mailCount/10 > 0) {
			dig++;
		}
		break;
	}
	//printf("digits: %d\n", dig);

	char *num = malloc(sizeof(char) * 5 + 1);
	int zero = 5 - dig;
	char temp[5];
	int k = 0;
	sprintf(temp, "%d", mailCount);
	//printf("curr num: %s\n", temp);

	for (int i = 0; i < 5; i++) {
		if (zero > 0) {
			num[i] = '0';
			zero--;
		} else {
			//printf("here\n");
			num[i] = temp[k];
			k++;
		}
	}
	num[5] = '\0';

	//printf("written mail count string %s\n", num);
	return num;
}




FILE *getTempFile(struct headers *list) {
	char *rec_one = list->rec_list[0];
	//printf("pre checkmail %s\n", rec_one);

	//open a tmp file
	char *num = getMailCountString(rec_one);

	//printf("prefile\n");
	char path[] = "test/tmp/";
	char filePath[strlen(path) + strlen(rec_one) + strlen(num) + 1];
	//sprintf(filePath, "%s%s%s", path, rec_one, num);
	strcpy(filePath, path);
	strcat(filePath, rec_one);
	strcat(filePath, num);

        //printf("temp file path: %s\n", filePath);
         //printf("%s\n", path);
         //printf("%s\n", rec_one);
         //printf("%s\n", num);
         //printf("seg fault?\n");
	FILE *fp = fopen(filePath, "w+");

	if (fp == NULL) {
		die("fopen failed");
	}

	free(num);
	return fp;

}

char *getFromString(struct headers *list) {
	//printf("Inside from string\n");

	char from[] = "From: ";
	char *fromString = malloc(strlen(from) + strlen(list->rec_list[0]) + 2);
	strcpy(fromString, from);
	strcat(fromString, list->rec_list[0]);

	fromString[strlen(list->rec_list[0]) + strlen(from) + 1] = '\0'; //finish with null terminator
	fromString[strlen(list->rec_list[0]) + strlen(from)] = '\n'; 	
	return fromString;
}


char *getToString(struct headers *list) {
        //printf("inside to string\n");

	char to[] = "To: ";
	char next[] = ", ";
	int size = 0;

	//char **index = list->rec_list;
	//char *temp;

	//size += strlen(to);
	size += strlen(list->rec_list[1]);
	//printf("%s\n", list->rec_list[0]);

	for (int i = 2; i < list->count; i++) {
		size += (strlen(next) + strlen(list->rec_list[i]));
	}

	char *toString = malloc(strlen(to) + size + 2); // make space for new line and null
	if (toString == NULL)
		printf("malloc failed\n");

	strcpy(toString, to);
	strcat(toString, list->rec_list[1]);


	for (int i = 2; i < list->count; i++) {
		strcat(toString, next);
		strcat(toString, list->rec_list[i]);
	}

	toString[strlen(to) + size] = '\n'; //finish with new line
	toString[strlen(to) + size + 1] = '\0';
	return toString;
}
