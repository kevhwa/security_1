#include <string.h> 
#include <stdlib.h>
#include "aes.h"
#include "sha256.h"
#include <stdio.h>
#include <ctype.h> /* For isascii */
#include <unistd.h>
#include <sys/stat.h>
// include others

#define MAX_NAME_BUFF 100 /* Maximum buffer for names */
#define MAX_PASS_BUFF 32
#define MAX_DATA_BUFF 4096 /* Maximum buffer for transfering data */
#define HASH_ITR 10000

static void die(const char *message)
{
	perror(message);
	exit(1);
}
	
void checkPassValid(char *password)
{
	int length;
	if((length = strlen(password)) > MAX_PASS_BUFF)
		die("Password too long");
	
	for(int i = 0; i < length; i++) {
		if(!isascii(password[i]))
			die("ASCII characters only");
	}
}

void addFile(FILE *newFile, FILE *archiveFile)
{
        size_t n;
        char buf[MAX_DATA_BUFF];
        
        while ((n = fread(buf, 1, sizeof(buf), newFile)) > 0) {
                
                if ((fwrite(buf, 1, n, archiveFile)) != n) {
                        die("write failed\n");
                }
        }

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 

        fclose(archiveFile);
        fclose(newFile);
}

int main(int argc, char *argv[])
{
	if(argc < 3) {
		die("Invalid arguments");
	}

	argv++; //argv = 1
	char *func_name = *argv++; //argv = 2
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
	char *pass_option = *argv++; //argv = 3
	char pass_opt[] = "-p\0";
	char *password;

        SHA256_CTX ctx;
        BYTE pass_hash[SHA256_BLOCK_SIZE];
	// If password option is given
	if(strcmp(pass_option, pass_opt) == 0) {
		password = *argv++; // argv 3 -> 4
		checkPassValid(password);
	} else {
		password = getpass("Please enter password");
		checkPassValid(password);
		argv++; //argv  3 -> 4
	}

        sha256_init(&ctx);
        for (int i = 0; i < HASH_ITR; ++idx) {
                sha256_update(&ctx, password, strlen(password));
        }
        sha256_final(&ctx, pass_hash);



	// Now we have password set and argv is aligned 
	
	// Verify if archive file exists. If it does, check pass. Otherwise create
	char *archive_name = *argv++; // argv 4 -> 5
        char *newFile_name;
        FILE *archive_fp = NULL;
        FILE *newFile_fp = NULL;
        struct stat st;

        if (stat(archive_name, &st) == 0 && S_ISDIR(st.st_mode)) {
                die("Not a file");
        }
        // NEED TO ADD LOOP FOR MULTIPLE FILES
        while (newFile_name = *argv++) {
                if ((strcmp(func_name, "add")) == 0) {
                        if (access(newFile_name, F_OK) == 0) {
                                
                                archive_fp = fopen(archive_name, "ab+");
                                if (archive_fp == NULL) {
                                        die("open failed");
                                }

                                newFile_fp = fopen(newFile_name, "rb");
                                if(newFile_fp == NULL) {
                                        die("open failed");
                                }

                                addFile(newFile_fp, archive_fp); 

	                } else {
		                
	                }
                }

        }
                

func_end: 
	return 0;
}
