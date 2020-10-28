
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
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
//void skipNext(void);
//void resizeList(char **, int *);
//int checkMailCount(char *);
//void addLine(struct headers *, char *);
//FILE *getTempFile(struct headers *);
FILE *getRecvFile(char *);
//char *getMailCountString(char *);
//char *getFromString(char *);
//char *getToString(struct headers *);

int main(int argc, char **argv) {

	printf("\n*****IM STARTING**********\n");

	char *reciever = argv[1];
	if (checkValidUser(reciever) != 1) {
		printf("invalid user\n");
		exit(-1);
	}

	FILE *fp1 = getRecvFile(reciever);
	char requestLine[BUFF_SIZE];
	
	while (1) {

		if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
			die("fgets failed\n");	
		}
		printf("debug: %s", requestLine);

		
		//open file using rec_list[i < rec_count]

		if (strcmp(requestLine, ".\n" ) == 0 || strcasecmp(requestLine, "data\r\n") == 0 ) {
			printf("reached end of data\n");
			break;	
		} else {
			fputs(requestLine, fp1);	
			//the buffer was full and last char is null terminated
			//buffer is not full and there is a /n somewhere
			//if (requestLine[strlen(requestLine) - 1] == '\n') {
			//	fputc('\n', fp1);
				
			//}
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

FILE *getRecvFile(char *recver) {
	//printf("\nInside recv file function\n");
	
	//open a tmp file
	char *num = getMailCountString(recver);

	char path[] = "test/mail/";
	char filePath[strlen(path) + strlen(recver) + 1 + strlen(num) + 1];
	//sprintf(filePath, "%s%s%s", path, rec_one, num);
	strcpy(filePath, path);
	strcat(filePath, recver);
	strcat(filePath, "/");
	strcat(filePath, num);

	//printf("file path: %s\n", filePath);
	FILE *fp = fopen(filePath, "w+");
	
	if (fp == NULL) {
		die("fopen failed");
	}

	free(num);
	return fp;

}


/*
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

void resizeList(char **list, int *count) {
	char **temp = realloc(list, PTR_SIZE * *count * 2);
	*count = *count * 2;

	temp = temp;
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



char *getFromString(char *sender) {
	//printf("Inside from string\n");
	
	char from[] = "From: ";
	char *fromString = malloc(strlen(from) + strlen(sender) + 1);
	strcpy(fromString, from);
	strcat(fromString, sender);

	fromString[strlen(sender) + strlen(from)] = '\0'; //finish with null terminator
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
	size += strlen(list->rec_list[0]);
	//printf("%s\n", list->rec_list[0]);

	for (int i = 1; i < list->count; i++) {
		size += (strlen(next) + strlen(list->rec_list[i]));
	}

	char *toString = malloc(strlen(to) + size + 2); // make space for new line and null
	if (toString == NULL)
		printf("malloc failed\n");
	
	strcpy(toString, to);
	strcat(toString, list->rec_list[0]);


	for (int i = 1; i < list->count; i++) {
		strcat(toString, next);
		strcat(toString, list->rec_list[i]);
	}
	
	toString[strlen(to) + size] = '\n'; //finish with new line
	toString[strlen(to) + size + 1] = '\0';
	return toString;
}
*/
//misc code from checking directory name
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
/*
	char requestLine[BUFF_SIZE];
	char fromLine[BUFF_SIZE];
	char *sender;
	int r_count = 0;

	struct headers list;
	list.rec_list = malloc (5 * sizeof(list.rec_list));
	list.size = 5;
	list.count = 0;


	//First check mail from 
	if (fgets(fromLine, sizeof(fromLine), stdin) == NULL) {
		die("child fgets failed\n");	
	}
	//sender[strlen(sender) - 1] = '\0';

	//printf("I RECEIVED SOMETHING: \n%s\n", fromLine);
	//fflush(stdout);
	char *separator = ":";
	char *method = "";
	char *user = "";
	
	method = strtok(fromLine, separator);
	sender = strtok(NULL, separator);

	//trimming name just for mailbox comparison
	sender[strlen(sender) - 1] = '\0'; //get rid of new line
	sender[strlen(sender) - 1] = '\n'; // get rid of ending >
	sender++; //get rid of starting <


	//get receiver info
		if (strcasecmp(requestLine, "data\n") == 0 || strcasecmp(requestLine, "data\r\n") == 0)  {
			if (r_count == 0) {
				fprintf(stderr, "DATA out of place, skipping\n");
				skipNext();
				break;
			}
			break;
		}
		
		// add to list of recipients
		method = strtok(requestLine, separator);
		user = strtok(NULL, separator);
		
		user[strlen(user) - 1] = '\0'; // get rid of new line
		user[strlen(user) - 1] = '\0'; // get rid of ending >
		user++; // get rid of starting <
		method = method;

		//check if user is in mailbox. ---- moved to beginning
		
		char *recvr = malloc(strlen(user) ); 
		strcpy(recvr, user);

		addLine(&list, recvr);
		r_count++;
		
	}
	//At this point, we have all the sender and receiver data
	//we need to get file count and open the file
	//for everything else we read in, we write to the file
	
	FILE *fp = getTempFile(&list);

	//write to temp file
	printf("\nreading from stdin, writing to temp\n");
	while (1) {
	
		if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {    
			die("fgets failed\n");	//includes new line
		}

		printf("%s", requestLine);

		if (strcmp(requestLine, ".\n" ) == 0 || strcasecmp(requestLine, "data\r\n") == 0 ) {
			printf("reached end of data\n");
			fputs(requestLine, fp);	
			break;	
		} else {
			if (strncmp(requestLine, ".", 1) == 0) {
				char *ptr = requestLine;
				ptr++;
				fputs(ptr, fp);
			} else {
			
			fputs(requestLine, fp);	
			}
			//the buffer was full and last char is null terminated
			//buffer is not full and there is a /n somewhere
			
			//if (requestLine[strlen(requestLine) - 1] == '\n') {
			//	fputc('\n', fp);
				
			//}
		}
	}

	// now tmp file has all the messages
	char *msg_from = getFromString(sender);
	char *msg_to = getToString(&list);

	//printf("from: %s\n", msg_from);
	//printf("to: %s\n", msg_to);

	printf("\nReading from tmp, writing to receiver\n");
	for (int i = 0; i < list.count; i++) {

	fwrite(msg_from, 1, strlen(msg_from), fp1);
	fwrite(msg_to, 1, strlen(msg_to) , fp1);
	fseek(fp, 0, SEEK_SET);

	for (int i = 0; i < list.count; i++) {
		free(list.rec_list[i]);

	}

	printf("in reclist: \n");

	free(list.rec_list);
	free(msg_from);
	free(msg_to);
	fclose(fp);


*/
