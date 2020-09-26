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
#define MAX_ENCRYPT_BLOCK_BYTES 16
#define HASH_ITR 100000

// ******* CHECK OFF_T SIZE WITH INT OVERFLOW INTERACTION. MAYBE PUT EVERYTHING INTO INT AND CHECK THAT FILE SIZE CAN'T BE BIGGER THAN MAX INT SIZE;

static void die(const char *message)
{
	perror(message);
	exit(1);
}

char passcode[16] = "Valid key TRUE!!";

// void checkFileNameValid
// void fileLengthValid

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

void encryptPasswordCheck(FILE *archiveFile, WORD *key_schedule)
{
        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        char byte_buff[16];
        size_t n;

        aes_encrypt(passcode, enc_buf, key_schedule, 256);

        if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                die("write failed\n");
        }
        
}

void encryptFileName(FILE *archiveFile, char *fileName, WORD *key_schedule)
{
        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        char byte_buff[16];
        size_t n;

        //Encrypt file name and write into archive
        for(int i = 0; i < 8; i++) {

                memcpy(byte_buff, &fileName[i * 16], 16);

                aes_encrypt(byte_buff, enc_buf, key_schedule, 256);

                if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                        die("write failed\n");
                }
        }
}

void encryptFileLength(FILE *archiveFile, long fileLength, WORD *key_schedule)
{

        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        char byte_buff[16];
        long longArray[2];
        size_t n;
        longArray[0] = fileLength;

        //encrypt length and write into archive
        memcpy(byte_buff, longArray, 16);

        aes_encrypt(byte_buff, enc_buf, key_schedule, 256);

        if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                die("write failed\n");
        }
}

void addFile(FILE *newFile, FILE *archiveFile, BYTE hash_pass[], char *fileName, long fileLength)
{

        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        WORD key_schedule[60];
        size_t n;
        char byte_buff[16];
        // ******************** CHECK IF PASSWORD IS CORRECT

        aes_key_setup(hash_pass, key_schedule, 256);
        
        encryptPasswordCheck(archiveFile, key_schedule);
        encryptFileName(archiveFile, fileName, key_schedule); 
        encryptFileLength(archiveFile, fileLength, key_schedule);

        // Read in 16 bytes intervals
        while ((n = fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, newFile)) > 0) {
                
                // If less than 16 bytes, pad it with a value that its missing (in char)
                if (n < MAX_ENCRYPT_BLOCK_BYTES) {
                       
                        memcpy(byte_buff, buf, n);

                        char fill_amount = sizeof(byte_buff)- n;

                        for (int i = (n -1) ; i < sizeof(byte_buff) ; i++ ) {
                                byte_buff[i] = fill_amount;
                        }

                        memcpy(buf, byte_buff, MAX_ENCRYPT_BLOCK_BYTES);
                }

                aes_encrypt(buf, enc_buf, key_schedule, 256);

                if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                        die("write failed\n");
                }
        }

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 

        fclose(archiveFile);
        fclose(newFile);
}

int extractPasswordCheck(FILE *archiveFile, WORD *key_schedule)
{
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        size_t n;
 
        if ((n = fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) < 0) {
                die("read failed");
        }
                
        aes_decrypt(buf, dec_buf, key_schedule, 256);
        
        return strncmp(passcode, dec_buf, 16);
}


char *extractFileName(FILE *archiveFile, WORD *key_schedule)
{
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        size_t n;
        char byte_buff[MAX_ENCRYPT_BLOCK_BYTES];
        char *fileName = malloc(128);


        for (int i = 0; i < (128/16); i++) {
               if ((n = fread(byte_buff, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) < 0) {
               die("read failed");
               }
                
               aes_decrypt(byte_buff, dec_buf, key_schedule, 256);

               strcat(fileName, dec_buf);
        }
        return fileName;
}

long *extractFileLength(FILE *archiveFile, WORD *key_schedule)
{
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        long fileLengthArray[2];
        size_t n;
 
        if ((n = fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) < 0) {
                die("read failed");
        }
                
        aes_decrypt(buf, dec_buf, key_schedule, 256);
        
        long *fileLength = malloc(sizeof(long));
        *fileLength = *(long *)dec_buf;
        return fileLength;
}

void extractFile(FILE *newFile, FILE *archiveFile, BYTE hash_pass[])
{

        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        WORD key_schedule[60];
        size_t n;
        int fileByteCount = 0;
        
        aes_key_setup(hash_pass, key_schedule, 256);

        int passIsValid = extractPasswordCheck(archiveFile, key_schedule);
        //if (passIsValid == 0)
                //printf("pass is valid\n");

        char *fileName = extractFileName(archiveFile, key_schedule);
        long *fileLength = extractFileLength(archiveFile, key_schedule);
        long lengthCounter = *fileLength;

        // Read in 16 bytes
        while ((n = fread(buf, 1, sizeof(buf) / 8, archiveFile)) > 0) {
                
                aes_decrypt(buf, dec_buf, key_schedule, 256);
                
                if (lengthCounter <= 16) {
                        if ((fwrite(dec_buf, 1, lengthCounter, newFile)) != lengthCounter) {
                                die("write failed leftover write\n");
                        }
                } else {
                        if ((fwrite(dec_buf, 1, n, newFile)) != n) {
                                die("write failed\n");
                        }

                }
                lengthCounter -= n;
        }
        
        free(fileName);
        free(fileLength);

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 

        fclose(archiveFile);
        fclose(newFile);
}

int keyIsValid(FILE *archiveFile, BYTE *hash_pass)
{
        WORD key_schedule[60];
        aes_key_setup(hash_pass, key_schedule, 256);

        int passIsValid = extractPasswordCheck(archiveFile, key_schedule);

        return passIsValid == 0;
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

        sha256_init(&ctx);
        for (int i = 0; i < HASH_ITR; ++i) {
                sha256_update(&ctx, password, strlen(password));
        }
        sha256_final(&ctx, hash_pass);
	
	// Verify if archive file exists. If it does, check pass. Otherwise create
	char *archive_name = *argv++; // argv 4 -> 5
        char *newFile_name;
        FILE *archive_fp = NULL;
        FILE *newFile_fp = NULL;
        struct stat archive_st;
        //struct stat addFile_st;
        long addFileSize;
        int newArchive;

        if (stat(archive_name, &archive_st) == 0 && S_ISDIR(archive_st.st_mode)) {
                die("Not a file");
        }

        // ******** CHECK THAT THE ARCHIVE IS NOT EMPTY

        /*if (stat(newFile_name, &addFile_st) == 0 && S_ISDIR(addFile_st.st_mode)) {
                die("Not a file");
        }*/
        
        while (newFile_name = *argv++) {
        
                //* **** ADD CHECK TO SEE IF FILE IS TOO BIG FOR INT SIZE
                // ******** ADD CHECK TO SEE IF FILE NAME IS LONGER THAN 100 CHARS

                if ((strcmp(func_name, "add")) == 0) {
                        if (access(newFile_name, F_OK) == 0) {
 
                                if (stat(archive_name, &archive_st) != 0) {
                                        archive_fp = fopen(archive_name, "wb+");
                                        if (archive_fp == NULL) {
                                                die("open failed");
                                        }
                                        newArchive = 1;
                                } else {
                                        archive_fp = fopen(archive_name, "ab+");
                                        if (archive_fp == NULL) {
                                                die("open failed");
                                        }
                                        newArchive = 0;
                                }
                               
                                newFile_fp = fopen(newFile_name, "rb");
                                if(newFile_fp == NULL) {
                                        die("open failed");
                                }
                                
                                
                                fseek(newFile_fp, 0, SEEK_END);
                                addFileSize = ftell(newFile_fp) - 1;
                                fseek(newFile_fp, 0, SEEK_SET);
                                //printf("file size %ld\n", addFileSize);

                                char fileName[128];
                                int nameLength = strlen(newFile_name);
                                strcpy(fileName, newFile_name);
                                fileName[nameLength] = 0;
                                //printf("original file name %s\n", fileName);

                                if (newArchive) {
                                        addFile(newFile_fp, archive_fp, hash_pass, fileName, addFileSize); 
                                } else {
                                        if (keyIsValid(archive_fp, hash_pass)) {

                                                fseek(archive_fp, 0, SEEK_SET);

                                                addFile(newFile_fp, archive_fp, hash_pass, fileName, addFileSize); 
                                        } else {
                                                die("Wrong password");
                                
                                        }
                                }
	                } else {
                               
                               
                               //Add what happens if file doesn't exist
                               //nothing right?



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

                                if (keyIsValid(archive_fp, hash_pass)) {

                                        fseek(archive_fp, 0, SEEK_SET);

                                        extractFile(newFile_fp, archive_fp, hash_pass); 
                                } else {
                                        die("Wrong password");
                                
                                }



	                } else {
                               
                               
                               //Add what happens if archive doesn't exist



	                }
                }
        }
                

func_end: 
	return 0;
}
