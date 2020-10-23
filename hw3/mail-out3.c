
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

#define BUFF_SIZE 4096
#define PTR_SIZE 8

static void die(const char *message) {
	perror(message);
	exit(1);
}

int checkEOF(void); 
int checkValidUser(char *); 
void skipNext(void);
void resizeList(char **, int *);

int main(int argc, char **argv) {

	char requestLine[1000];
	char **rec_list = malloc (5 * sizeof(*rec_list));
	int list_count = 0;
	char sender[1000];
	int r_count = 0;


	//First check mail from 
	if (fgets(sender, sizeof(sender), stdin) == NULL) {
		die("fgets failed\n");	
	}
	sender[strlen(sender) - 1] = '\0';

	char *separator = ":";
	char *method = "";
	char *user = "";
	
	method = strtok(sender, separator);
	user = strtok(NULL, separator);
		
	//trimming name
	//printf("length of name: %ld\n", strlen(user));
	user[strlen(user) - 1] = '\0';
	user[strlen(user) - 1] = '\0';
	user++;
	printf("trimmed user: %s\n", user);

	while (1) {
		
		if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
			die("fgets failed\n");	
		}

		//printf("debug: %s\n", requestLine);

		//check if this is the data line.
		if (strcasecmp(requestLine, "data\n") == 0 || strcasecmp(requestLine, "data\r\n") == 0)  {
			if (r_count == 0) {
				fprintf(stderr, "DATA out of place, skipping\n");
				skipNext();
				break;
			}
			//printf("Found DATA\n");
			break;
		}
		
		if (r_count == list_count) {
			resizeList(rec_list, &list_count);
		}
		
		// add to list of recipients
		method = strtok(requestLine, separator);
		user = strtok(NULL, separator);
		//printf("user: %s\n", user);
		user[strlen(user) - 1] = '\0';
		user[strlen(user) - 1] = '\0';
		user++;
		method = method;

		char *recvr = malloc(strlen(user)); 
		memcpy(recvr, user, strlen(user));

		rec_list[r_count] = recvr;
		r_count++;
		
		}
		//Have to fork here since now we know there's nothing wrong with the order
	//pipe
	//fork
	//send sender
	//send reclist
	//send the data
	
	//read in data 
	while (1) {
	
		if (fgets(requestLine, sizeof(requestLine), stdin) == NULL) {
			die("fgets failed\n");	
	}
		
		if (strcmp(requestLine, ".\n" ) == 0 || strcasecmp(requestLine, "data\r\n") == 0 ) {
			printf("reached end of data\n");
		break;	
		} else {
			
		printf("reading data: \n");
			printf("%s\n", requestLine);
		}
	}

	printf("in reclist: \n");
	for (int i = 0; i < r_count; i++) {
		printf("%s\n", rec_list[i]);
		free(rec_list[i]);
	}
	free(rec_list);

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

void resizeList(char **list, int *count) {
	char **temp = realloc(list, PTR_SIZE * *count * 2);
	*count = *count * 2;

	temp = temp;
}


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

