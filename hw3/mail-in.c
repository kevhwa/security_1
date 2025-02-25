
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define BUFF_SIZE 500
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

int eof = 0;

int checkEOF(void); 
int checkValidUser(char *); 
int recvrExists(struct headers *, char *);
int verifyUserFormat(void);
void skipNext(void);
void skipUser(void);
void resizeList(char **, int *);
int writeTempFile(FILE *, struct headers *);
void free_list(struct headers *);
void removeTempFile(struct headers *);
void addBlankLine(struct headers *);
int getSender(struct headers *); 
int getRecvr(struct headers *, int *);
int checkMailCount(char *);
int checkInvalidRecipient(pid_t); 
FILE *getTempFile(struct headers *);
char *getMailCountString(char *);
char *getFromString(struct headers *);
char *getToString(struct headers *);

int main(int argc, char **argv) {

	struct headers list;

	while(1) {
		//printf("\nnew mail\n");
		
		list.rec_list = malloc (5 * sizeof(list.rec_list));
		if (list.rec_list == NULL)
			die("malloc failed\n");
		list.size = 5;
		list.count = 0;
		int r_count = 0;

		//Check if EOF
		
		if (checkEOF() == 1) {
			//printf("EOF REACHED\n");
			free(list.rec_list);
			break;
		}

		if (getSender(&list) < 0) {
			free_list(&list);
			if (eof)
				break;

			continue;
		}

		int flag = 0;
		while (1) {
			int x = getRecvr(&list, &r_count);
			if (x < 0) {
				flag = 1;
				break;
			} else if (x == 0)
				break;
		}
		if (flag == 1) {
			free_list(&list);
			continue;
		}
		if (eof) {
			free_list(&list);
			break;
		}
		
		//write from and to to temp file

		FILE *temp_fp = getTempFile(&list);
		writeTempFile(temp_fp, &list);
		if (eof) {
			free_list(&list);
			fclose(temp_fp);
			break;
		}
/*		if (res < 0) {
			free_list(&list);
			fclose(temp_fp);
			break;
		}
*/	
		char requestLine[BUFF_SIZE];
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
			
				if (execlp("./bin/mail-out", "mail-out", temp, NULL) < 0)
					die("execl failed\n");

			} else{
				//sleep(1);
				close(fd[0]);

				while (1) {
	
					if (fgets(requestLine, sizeof(requestLine), temp_fp) == NULL) {    
						die("fgets failed\n");	//includes new line
					}

					if (checkInvalidRecipient(pid) < 0)
						break;

					//printf("%s", requestLine);

					if (strcmp(requestLine, ".\n" ) == 0 || strcasecmp(requestLine, "data\r\n") == 0 ) {
						//printf("reached end of data\n");
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
			//printf("\nwaiting for child\n");
			if (waitpid(pid, &status, 0) < 0) {
				die("waitpid failed\n");
			}

			if (WIFEXITED(status)) {
			        int es = WEXITSTATUS(status);

				if (es == 128) {
					fprintf(stderr, "Error from child: Invalid recipient\n");
				}

				//printf("Exit status was %d\n", es);
			}
		}
		fclose(temp_fp);
		removeTempFile(&list);
		free_list(&list);

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

	if ((dfd = opendir("mail")) == NULL) {
		die("Can't open mail dir\n");
	}

	while ((dp = readdir(dfd)) != NULL)
	{
		
		if (strcmp(user, dp->d_name) == 0) {
			//printf("user found\n");
			closedir(dfd);
			return 1;
		} else {
			continue;
		}
		
	}
	closedir(dfd);

	return 0;
}

void skipNext(void) {

	//printf("Skipping to next mail\n");
	char requestLine[BUFF_SIZE];

	while (1) {
	
		if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
			if (feof(stdin)) {
				fprintf(stderr, "Invalid message termination\n");
				eof = 1;
				break;
			}
			die("fgets failed\n");	
		}
			
		if (strcmp(requestLine, ".\n" ) == 0 ) {
			//printf("reached end of data\n");
			return;	
		}
	}
}

void skipUser(void) {

	//printf("Finishing reading long username\n");
	char requestLine[BUFF_SIZE];

	while (1) {
	
		if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
			if (feof(stdin)) {
				fprintf(stderr, "Invalid message termination\n");
				eof = 1;
				break;
			}
			die("fgets failed\n");	
		}
			
		if (requestLine[strlen(requestLine) - 1] == '\n') {
			//printf("reached end of username \n");
			return;	
		}
	}
}

int verifyUserFormat(void) {

	//printf("Finishing reading long username\n");
	char requestLine[BUFF_SIZE];

	while (1) {
	
		if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
			if (feof(stdin)) {
				fprintf(stderr, "Invalid message termination\n");
				eof = 1;
				return -1;
			}
			die("fgets failed\n");	
		}
			
		if (requestLine[strlen(requestLine) - 1] == '\n') {
			if( requestLine[strlen(requestLine) - 2] == '>')
				return 1;
			return -1;
		}
	}
	return -1;
}

void addLine(struct headers *list, char *line) {
	list->rec_list[list->count++] = line;

	if (list->count == list->size) {
		//printf("resizing list\n");
		char **temp = realloc(list->rec_list, PTR_SIZE * (list->size * 2));
		list->rec_list = temp;
		list->size *= 2;
	}
}

int recvrExists(struct headers *list, char *recvr) {

	for(int i = 1; i < list->count; i++) { //ignoring sender
		char *temp = list->rec_list[i];
		if (strcmp(temp, recvr) == 0){
			return 1;
		}
	}
	return 0;
}

int getSender(struct headers *list) {
	
	char fromLine[BUFF_SIZE];
	char *sender;
	char *separator = ":";
	char *method = "";

	//First check mail from 
	if (fgets(fromLine, sizeof(fromLine), stdin) == NULL) {
		if (feof(stdin)) {
			fprintf(stderr, "Invalid message termination\n");
			eof = 1;
			return -1;
		}
		die("fgets failed\n");	
	}

	method = strtok(fromLine, separator);
	sender = strtok(NULL, separator);
	if (sender == NULL) {
		fprintf(stderr, "Invalid blank line, skipping mail\n");
		skipNext();
		return -1;
	}

	if (strlen(sender) > 258) {
		if (sender[strlen(sender) - 1] != '\n') { //means more to read
			int res = verifyUserFormat();
			if (res == 1) {
				fprintf(stderr, "Sender name too long, skipping mail\n");
				skipNext();
				return -1;
			} else if (res < 0) {
				fprintf(stderr, "Invalid RCPT to format, skipping mail\n");
				skipNext();
				return -1;
			}
		}

		// we read the whole username
		if (sender[strlen(sender) - 2] != '>') {
			fprintf(stderr, "Invalid RCPT to format, skipping mail\n");
			skipNext();
			return -1;
		}
		
		fprintf(stderr, "Sender name too long, skipping mail\n");
		skipNext();
		return -1;
	}

	sender[strlen(sender) - 1] = '\0'; // get rid of new line

	if (strcasecmp(method, "mail from") != 0 || sender[0] != '<' || sender[strlen(sender) - 1] != '>') {
		fprintf(stderr, "Invalid sender format, skipping mail\n");
		skipNext();
		return -1;
	}

	sender[strlen(sender) - 1] = '\0'; // get rid of ending >
	sender++;

	//printf("sender: %s\n", sender);

	if (checkValidUser(sender) != 1) {
		skipNext();
		return -1;
	}

	char *sendername = malloc(strlen(sender) + 1); 
	if (sendername == NULL)
		die("malloc failed\n");
	strcpy(sendername, sender);
	sendername[strlen(sender)] = '\0';

	addLine(list, sendername);
	return 1;

}

int getRecvr(struct headers *list, int *r_count) {
	//printf("inside get recvr\n");

	char requestLine[BUFF_SIZE];
	char *separator = ":";
	char *method = "";
	char *user = "";

	if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
		if (feof(stdin)) {
			fprintf(stderr, "Invalid message termination\n");
			eof = 1;
			return -1;
		}
		
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
	if (user == NULL) {
		fprintf(stderr, "Invalid blank line, skipping mail\n");
		skipNext();
		return -1;
	}

	if (strlen(user) > 258) {
		if (user[strlen(user) - 1] != '\n') { //means more to read
			int res = verifyUserFormat();
			if (res == 1) {
				fprintf(stderr, "Recipient name too long, skipping recipient\n");
				return 1;
			} else if (res < 0) {
				fprintf(stderr, "Invalid RCPT to format, skipping mail\n");
				skipNext();
				return -1;
			}
		}

		// we read the whole username
		if (user[strlen(user) - 2] != '>') {
			fprintf(stderr, "Invalid RCPT to format, skipping mail\n");
			skipNext();
			return -1;
		}
		
		fprintf(stderr, "Recipient name too long, skipping recipient\n");
		return 1;
	}

	user[strlen(user) - 1] = '\0';


	if (strcasecmp(method, "rcpt to") != 0 || user[0] != '<' || user[strlen(user)- 1] != '>') { 
		//printf("here1\n");
		fprintf(stderr, "Invalid RCPT to format, skipping mail\n");
		skipNext();
		return -1;
	} 

	user[strlen(user) -1] = '\0';
	user++;

	//printf("recipients %s\n", user);

	//printf("passed rcpt to test, sending to : %s\n", user);
	char *recvr = malloc(strlen(user) + 1);
	if (recvr == NULL)
		die("malloc failed\n");
	strcpy(recvr, user);
	recvr[strlen(user)] = '\0';
	
	if (!recvrExists(list, recvr)){
		addLine(list, recvr);
		*r_count = *r_count + 1;
		return 1;
	} 

	fprintf(stderr, "Duplicate reciever, skipping receiver\n");
	return 1;
}

int checkMailCount(char *user) {
	
	//printf("\nInside checkMailCount function\n");

	struct dirent *dp;
	DIR *dfd;
	int count = 0;
	char path[] = "../mail/";
	char dir_name[strlen(path) + strlen(user) + 1];
	struct stat st;

	//create "test/mail/username"
	strcpy(dir_name, path);
	strcat(dir_name, user);
	//printf("path name %s\n", dir_name);

	if ((dfd = opendir(dir_name)) == NULL) {
		die("Can't open mail dir in checkMailCount\n");
	}

	while ((dp = readdir(dfd)) != NULL) {
		
		char filePath[strlen(dir_name) + 1 + strlen(dp->d_name) + 1];

		strcpy(filePath, dir_name);
		strcat(filePath, "/");
		strcat(filePath, dp->d_name);
		
		if(stat(filePath, &st) == -1 ) {
			//printf("Unable to stat file: %s\n",filePath) ;
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
	//printf("mail count: %d\n", count);i
	closedir(dfd);
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
	if (num == NULL)
		die("malloc failed\n");
	int zero = 5 - dig;
	char temp[5];
	int k = 0;
	sprintf(temp, "%d", mailCount);

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

	//printf("prefile\n");
	char path[] = "tmp/";
	char filePath[strlen(path) + strlen(rec_one) + 1];
	strcpy(filePath, path);
	strcat(filePath, rec_one);

        //printf("temp file path: %s\n", filePath);
	
	FILE *fp = fopen(filePath, "w+");

	if (fp == NULL) {
		die("fopen failed");
	}

	//free(num);
	return fp;
}

void removeTempFile (struct headers *list) {
	char *rec_one = list->rec_list[0];
	//printf("pre checkmail %s\n", rec_one);

	//printf("prefile\n");
	char path[] = "tmp/";
	char filePath[strlen(path) + strlen(rec_one) + 1];
	strcpy(filePath, path);
	strcat(filePath, rec_one);

	remove(filePath);
	//fprintf(stderr, "error: %d\n", res);
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
	if (toString == NULL) {
		die("malloc failed\n");
	}
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

int checkInvalidRecipient(pid_t pid) {

	int status;
	int finish;
	if ((finish = waitpid(pid, &status, WNOHANG)) < 0) {
		die("waitpid failed\n");
	}

	if (finish == 0)
		return 1;

	if (WIFEXITED(status)) {
	        int es = WEXITSTATUS(status);

		if (es < 0) {
			fprintf(stderr, "Invalid recipient\n");
		}

		//printf("Exit status was %d\n", es);
		return -1;
	}

	return 1;
}

int writeTempFile(FILE *temp_fp, struct headers *list) {
	char *msg_from = getFromString(list);
	char *msg_to = getToString(list);

	fwrite(msg_from, 1, strlen(msg_from), temp_fp);
	fwrite(msg_to, 1, strlen(msg_to), temp_fp);
	fputs("\n", temp_fp);

	char requestLine[BUFF_SIZE];
	//read in data and write to tmp
	while (1) {
	
		if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
			if (feof(stdin)) {
				fprintf(stderr, "Invalid message termination\n");
				eof = 1;
				break;
			}
			die("fgets failed\n");	
		}

		//printf("debug: %s", requestLine);
		if (strcmp(requestLine, ".\n" ) == 0 || strcasecmp(requestLine, "data\r\n") == 0 ) {
			//printf("parent: reached end of data\n");
			fputs(requestLine, temp_fp);
			break;	
		} else {
			fputs(requestLine, temp_fp);
		}
	}
	
	free(msg_from);
	free(msg_to);
	return 1;
}

void addBlankLine(struct headers *list) {
	char blank[] = "\n";
	addLine(list, blank);
}

void free_list(struct headers *list) {
	for (int i = 0; i < list->count; i++) {
		free(list->rec_list[i]);
	}
	free(list->rec_list);
}
