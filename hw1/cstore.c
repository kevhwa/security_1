#include <string.h> 
#include <stdlib.h>
#include "aes.h"
#include "sha256.h"
#include <stdio.h>
#include <ctype.h> /* For isascii */
#include <unistd.h>
#include <sys/stat.h>
// include others

#define MAX_NAME_BUFF_BYTES 100 /* Maximum buffer for names */
#define MAX_PASS_BUFF_BYTES 32
#define MAX_DATA_BUFF_BYTES 4096 /* Maximum buffer for transfering data */
#define MAX_ENCRYPT_BLOCK_BITS 128
#define HASH_ITR 100000

static void die(const char *message)
{
	perror(message);
	exit(1);
}
	
void checkPassValid(char *password)
{
	int length;
	if((length = strlen(password)) > MAX_PASS_BUFF_BYTES)
		die("Password too long");
	
	for(int i = 0; i < length; i++) {
		if(!isascii(password[i]))
			die("ASCII characters only");
	}
}

void addFile(FILE *newFile, FILE *archiveFile, BYTE hash_pass[])
{
        size_t n;
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        WORD key_schedule[60];
        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        int count = 0;

        aes_key_setup(hash_pass, key_schedule, 256);
        
        // Read in 16 bytes
        while ((n = fread(buf, 1, (sizeof(buf) / 8), newFile)) > 0) {
                
                printf("encrypt read in %d bytes\n", n);
                // If less than 16 bytes, pad it with a value that its missing (in char)
                if (n < (MAX_ENCRYPT_BLOCK_BITS / 8)) {
                       
                        char fill_amount = (MAX_ENCRYPT_BLOCK_BITS / 8)- n;
                        printf("fill value %d\n" ,fill_amount);
                        printf("\n");

                        for (int i = n; i < (sizeof(buf) / 8); i++ ) {
                                printf("i value: %d\n", i);
                                buf[i * 8] = fill_amount;
                                printf("print buf: %s\n", buf);
                                count++;
                        }
                }

                printf("encrypt added %d bytes\n", count);
                
                char temp = buf[MAX_ENCRYPT_BLOCK_BITS - 8];
                printf("testing temp %d\n", temp);

                printf("plaintext: %s\n", buf);

                aes_encrypt(buf, enc_buf, key_schedule, 256);

                if ((fwrite(enc_buf, 1, n, archiveFile)) != n) {
                        die("write failed\n");
                }
        }

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 

        fclose(archiveFile);
        fclose(newFile);
}

void extractFile(FILE *newFile, FILE *archiveFile, BYTE hash_pass[])
{
        size_t n;
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        WORD key_schedule[60];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];

        aes_key_setup(hash_pass, key_schedule, 256);
        
        // Read in 16 bytes
        while ((n = fread(buf, 1, sizeof(buf) / 8, archiveFile)) > 0) {
                
                printf("decrypt read in %d bytes\n", n); 
                
                aes_decrypt(buf, dec_buf, key_schedule, 256);
                
                printf("%s\n", dec_buf);

                if ((fwrite(dec_buf, 1, n, newFile)) != n) {
                        die("write failed\n");
                }
        }
        /*       
        char filler = dec_buf[((sizeof(buf) -1)- 8)];
        char null_char = '\0';
        char erase_buf[filler];
        */
        char filler_buf[1];
        fseek(newFile, -1 , SEEK_END);

        if (fread(filler_buf, 1, 1, newFile) != 1) {
                die("read failed\n");
        }
        
        printf("how many fillers %d\n", filler_buf[0]);
        char null_char = '\0';
        char erase_buf[filler_buf[0]];

        for (int i = 0; i < filler_buf[0]; i++)
        {
                erase_buf[i] = null_char;
        }
        
        fseek(newFile, 0 - filler_buf[0], SEEK_END);

        if ((fwrite(erase_buf, 1, filler_buf[0], newFile)) != filler_buf[0]) {
                die("write failed\n");
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
        BYTE hash_pass[SHA256_BLOCK_SIZE];
	// If password option is given
	if(strcmp(pass_option, pass_opt) == 0) {
		password = *argv++; // argv 3 -> 4
		checkPassValid(password);
	} else {
		password = getpass("Please enter password");
		checkPassValid(password);
		argv++; //argv  3 -> 4
	}

	//BYTE text3[] = {"aaaaaaaaaa"};
	//BYTE hash3[SHA256_BLOCK_SIZE] = {0xcd,0xc7,0x6e,0x5c,0x99,0x14,0xfb,0x92,0x81,0xa1,0xc7,0xe2,0x84,0xd7,0x3e,0x67,
	                                 //0xf1,0x80,0x9a,0x48,0xa4,0x97,0x20,0x0e,0x04,0x6d,0x39,0xcc,0xc7,0x11,0x2c,0xd0};

        sha256_init(&ctx);
        for (int i = 0; i < HASH_ITR; ++i) {
                sha256_update(&ctx, password, strlen(password));
        }
        sha256_final(&ctx, hash_pass);
/*
        if (memcmp(pass_hash, hash3, SHA256_BLOCK_SIZE) == 0){
                printf("%s\n", "hash success");
        } else {
                printf("%s\n", "hash failed");
        }
*/

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

                                addFile(newFile_fp, archive_fp, hash_pass); 

	                } else {
                               
                               
                               //Add what happens if file doesn't exist



	                }
                } else if ((strcmp(func_name, "extract")) == 0) {
                        if (access(archive_name, F_OK) == 0) {
                                
                                archive_fp = fopen(archive_name, "rb");
                                if (archive_fp == NULL) {
                                        die("open failed");
                                }

                                newFile_name = "decryptFileTest1";
                                newFile_fp = fopen(newFile_name, "wb+");
                                if(newFile_fp == NULL) {
                                        die("open failed");
                                }

                                extractFile(newFile_fp, archive_fp, hash_pass); 

	                } else {
                               
                               
                               //Add what happens if archive doesn't exist



	                }
                }
        }
                

func_end: 
	return 0;
}
