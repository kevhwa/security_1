
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
FILE *getRecvFile(char *);

int main(int argc, char **argv) {

	//printf("\n*****IM STARTING**********\n");

	char *reciever = argv[1];
	if (checkValidUser(reciever) != 1) {
		//printf("invalid user\n");
		exit(128);
	}

	FILE *fp1 = getRecvFile(reciever);
	char requestLine[BUFF_SIZE];
	char oldLine[BUFF_SIZE];
	
	while (1) {

		if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
			die("fgets failed\n");	
		}
		//printf("debug: %s", requestLine);
		
		if ((strcmp(requestLine, ".\n" ) == 0 || strcasecmp(requestLine, "data\r\n")) == 0 && strlen(oldLine) != BUFF_SIZE - 1) {
			//printf("reached end of data\n");
			break;	
		} else {
			if (strncmp(requestLine, ".", 1 && strlen(oldLine) != BUFF_SIZE - 1) == 0) {
				char *ptr = requestLine;
				ptr++;
				if (fputs(ptr, fp1) < 0) {
					die("fputs failed\n");
				}
			} else {
				fputs(requestLine, fp1);	
			}
		}

		strcpy(oldLine, requestLine);
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
	char path[] = "mail/";
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

	char path[] = "mail/";
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


