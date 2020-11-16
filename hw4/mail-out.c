
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

	char *reciever = argv[1];
	if (checkValidUser(reciever) != 1) {
		exit(128);
	}

	FILE *fp1 = getRecvFile(reciever);
	char requestLine[BUFF_SIZE];
	char oldLine[BUFF_SIZE];

	while (1) {

		if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
			die("Fgets failed, quitting\n");	
		}
		
		if ((strcmp(requestLine, ".\n" ) == 0 || strcasecmp(requestLine, "data\r\n") == 0) && strlen(oldLine) != BUFF_SIZE - 1) {
			break;	
		} else {
			if (strncmp(requestLine, ".", 1) == 0 && strlen(oldLine) != BUFF_SIZE - 1) {
				char *ptr = requestLine;
				ptr++;
				if (fputs(ptr, fp1) < 0) {
					die("Fputs failed, quitting\n");
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
			die("Fgetc failed, quitting\n");
		}
		return 1;
	}

	return 0;
}

int checkValidUser(char *user) {

	struct dirent *dp;
	DIR *dfd;

	if ((dfd = opendir("mail")) == NULL) {
		die("Open dir failed, quitting\n");
	}

	while ((dp = readdir(dfd)) != NULL)
	{
		
		if (strcmp(user, dp->d_name) == 0) {
			return 1;
		} else {
			continue;
		}
		
	}

	return 0;
}

int checkMailCount(char *user) {

	struct dirent *dp;
	DIR *dfd;
	int count = 0;
	char path[] = "mail/";
	char dir_name[strlen(path) + strlen(user) + 1];
	struct stat st;

	strcpy(dir_name, path);
	strcat(dir_name, user);

	if ((dfd = opendir(dir_name)) == NULL) {
		die("Open dir failed, quitting\n");
	}

	while ((dp = readdir(dfd)) != NULL) {
		
		char filePath[strlen(dir_name) + 1 + strlen(dp->d_name) + 1];

		strcpy(filePath, dir_name);
		strcat(filePath, "/");
		strcat(filePath, dp->d_name);
		
		if(stat(filePath, &st) == -1 ) {
			continue ;
		}
		if (S_ISDIR(st.st_mode)) {
			continue;
		}

		count++;
	}

	if (count >= 99999) {
		die("Mail overload, quitting\n");
	}
	return count;
}

char *getMailCountString(char *user) {
	int mailCount = checkMailCount(user) + 1;

	int dig = 1;
	for (int i = 0; i < 5; i++) {
		if(mailCount/10 > 0) {
			dig++;
		}
		break;
	}

	char *num = malloc(sizeof(char) * 5 + 1);
	int zero = 5 - dig;
	char temp[5];
	int k = 0;
	sprintf(temp, "%d", mailCount);

	for (int i = 0; i < 5; i++) {
		if (zero > 0) {
			num[i] = '0';
			zero--;
		} else {
			num[i] = temp[k];
			k++;
		}
	}
	num[5] = '\0';

	return num;
}

FILE *getRecvFile(char *recver) {
	
	char *num = getMailCountString(recver);

	char path[] = "mail/";
	char filePath[strlen(path) + strlen(recver) + 1 + strlen(num) + 1];
	strcpy(filePath, path);
	strcat(filePath, recver);
	strcat(filePath, "/");
	strcat(filePath, num);

	FILE *fp = fopen(filePath, "w+");
	
	if (fp == NULL) {
		die("Fopen failed, quitting");
	}

	free(num);
	return fp;
}