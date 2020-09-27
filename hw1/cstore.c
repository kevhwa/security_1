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
#define METADATA_BLOCKSIZE 192
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
	if((length = strlen(password)) > MAX_PASS_BUFF_BYTES) {
		die("Password too long. Max size is 32 bytes");
        }
	
	for(int i = 0; i < length; i++) {
		if(!isascii(password[i])) {
		        die("ASCII characters only for password\n");
                }        
	}
}

void encryptPasswordCheck(FILE *archiveFile, WORD *key_schedule)
{
        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        char byte_buff[16];
        memset(byte_buff, 0, 16);
        size_t n;

        aes_encrypt(passcode, enc_buf, key_schedule, 256);

        if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                die("write failed\n");
        }
}

void encryptDeleteFileMarker(FILE *archiveFile, int deleteMarker, WORD *key_schedule)
{

        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        char byte_buff[16];
        int intArray[16];
        memset(intArray, 0, 16);
        size_t n;
        intArray[0] = deleteMarker;

        //encrypt length and write into archive
        memcpy(byte_buff, intArray, 16);

        aes_encrypt(byte_buff, enc_buf, key_schedule, 256);

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
        printf("enc: file = %s\n", fileName);
}

void encryptFileNameLength(FILE *archiveFile, int fileNameLength, WORD *key_schedule)
{

        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        char byte_buff[16];
        int intArray[4];
        memset(intArray, 0, 16);
        size_t n;
        intArray[0] = fileNameLength;

        //encrypt length and write into archive
        memcpy(byte_buff, intArray, 16);

        aes_encrypt(byte_buff, enc_buf, key_schedule, 256);

        if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                die("write failed\n");
        }
}

void encryptFileLength(FILE *archiveFile, long fileLength, WORD *key_schedule)
{

        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        char byte_buff[16];
        long longArray[2];
        size_t n;
        longArray[0] = fileLength;
        longArray[1] = 0;

        //encrypt length and write into archive
        memcpy(byte_buff, longArray, 16);

        aes_encrypt(byte_buff, enc_buf, key_schedule, 256);

        if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                die("write failed\n");
        }

        printf("enc: file length = %ld\n", fileLength);
        printf("\n");
}

void addFile(FILE *newFile, FILE *archiveFile, BYTE hash_pass[], int fileNameLength, char *fileName, long fileLength)
{

        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        WORD key_schedule[60];
        size_t n;
        char byte_buff[16];

        aes_key_setup(hash_pass, key_schedule, 256);
        
        encryptPasswordCheck(archiveFile, key_schedule);
        encryptDeleteFileMarker(archiveFile, 0, key_schedule); 
        encryptFileNameLength(archiveFile, fileNameLength, key_schedule);
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

        fclose(newFile);
}

int extractPasswordCheck(FILE *archiveFile, WORD *key_schedule)
{
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        size_t n;
        memset(buf, 0, 16);
        memset(dec_buf, 0, 16);
 
        if ((n = fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) < 0) {
                die("read failed");
        }
                
        aes_decrypt(buf, dec_buf, key_schedule, 256);
        
        //printf("password check: %s\n", dec_buf);
        return strncmp(passcode, dec_buf, 16);
}

int *extractDeleteFileMarker(FILE *archiveFile, WORD *key_schedule)
{
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        //int fileMarkerArray[4];
        size_t n;
 
        if ((n = fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) < 0) {
                die("read failed");
        }
                
        aes_decrypt(buf, dec_buf, key_schedule, 256);
        
        int *fileMarker_ptr = malloc(sizeof(int));
        *fileMarker_ptr = *(int *)dec_buf;
        return fileMarker_ptr;
}


char *extractFileName(FILE *archiveFile, WORD *key_schedule)
{
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        size_t n;
        char byte_buff[MAX_ENCRYPT_BLOCK_BYTES];
        char *fileName = malloc(128);
        memset(fileName, 0, 128);

        for (int i = 0; i < (128/16); i++) {
               if ((n = fread(byte_buff, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) < 0) {
               die("read failed");
               }
                
               aes_decrypt(byte_buff, dec_buf, key_schedule, 256);

               if (i == 0) {
                       strncpy(fileName, dec_buf, 16);
               } else {
                       strncat(fileName, dec_buf, 16);
               }
        }
        return fileName;
}

int *extractFileNameLength(FILE *archiveFile, WORD *key_schedule)
{
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        //int fileLengthArray[4];
        size_t n;
 
        if ((n = fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) < 0) {
                die("read failed");
        }
                
        aes_decrypt(buf, dec_buf, key_schedule, 256);
        
        int *fileNameLength = malloc(sizeof(int));
        *fileNameLength = *(int *)dec_buf;
        return fileNameLength;
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

void extractFile(FILE *archiveFile, BYTE hash_pass[], char *newFileName)
{
        printf("\nInside extract file\n");
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        WORD key_schedule[60];
        size_t n;
        int fileByteCount = 0;
        
        aes_key_setup(hash_pass, key_schedule, 256);

        int passIsValid = extractPasswordCheck(archiveFile, key_schedule);
        int *fileDeleteMarker_ptr = extractDeleteFileMarker(archiveFile, key_schedule);
        int *fileNameLength = extractFileNameLength(archiveFile, key_schedule);
        char *fileName = extractFileName(archiveFile, key_schedule);
        long *fileLength = extractFileLength(archiveFile, key_schedule);
        long lengthCounter = *fileLength;

        printf("decrypted file name length: %d\n", *fileNameLength);
        printf("original name: %s\n", newFileName);
        printf("decrypted name: %s\n", fileName);
        printf("file length: %ld\n", lengthCounter);

        int newFileNameLength = strlen(newFileName);
        char nameExt[] = "-decrypted";
        int newFileExtLength = strlen(nameExt);

        char decFileName[newFileNameLength + newFileExtLength + 1];
        strncpy(decFileName, newFileName, newFileNameLength + 1);
        strncat(decFileName, nameExt, newFileExtLength);

        FILE *newFile_fp = fopen(decFileName, "wb+");
        if(newFile_fp == NULL) {
                die("fopen failed");
        }

        // Read in 16 bytes
        while (lengthCounter >= 0 && (n = fread(buf, 1, sizeof(buf) / 8, archiveFile)) > 0) {
                
                aes_decrypt(buf, dec_buf, key_schedule, 256);

                
                if (lengthCounter <= 16) {
                        if ((fwrite(dec_buf, 1, lengthCounter, newFile_fp)) != lengthCounter) {
                                printf("decrypted: %s\n", dec_buf);
                                die("write failed in leftover write\n");
                        }
                } else {
                        if ((fwrite(dec_buf, 1, n, newFile_fp)) != n) {
                                die("write failed\n");
                        }

                }
                lengthCounter -= n;
        }
        
        free(fileDeleteMarker_ptr);
        free(fileNameLength);
        free(fileName);
        free(fileLength);

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 
        printf("exiting extract file\n");
        fclose(newFile_fp);
}

void deleteFile(FILE *archiveFile, BYTE hash_pass[])
{
        printf("\nInside delete file\n");
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        WORD key_schedule[60];
        size_t n;
        int fileByteCount = 0;
        
        aes_key_setup(hash_pass, key_schedule, 256);

        int passIsValid = extractPasswordCheck(archiveFile, key_schedule);
        int *fileDeleteMarker_ptr = extractDeleteFileMarker(archiveFile, key_schedule); //16
        int *fileNameLength = extractFileNameLength(archiveFile, key_schedule); //16
        char *fileName = extractFileName(archiveFile, key_schedule); //128
        long *fileLength = extractFileLength(archiveFile, key_schedule); //16
        long lengthCounter = *fileLength;

        long offset = 16 + 16 + 16 + 128;
        fseek(archiveFile,0 - offset, SEEK_CUR);
        //printf("fseek success\n");
        
        int deleteMark = 1;
        encryptDeleteFileMarker(archiveFile, deleteMark, key_schedule); 
       
        free(fileNameLength);
        free(fileName);
        free(fileLength);

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 
        printf("exiting delete file\n");
}

int keyIsValid(FILE *archiveFile, BYTE *hash_pass)
{
        WORD key_schedule[60];
        aes_key_setup(hash_pass, key_schedule, 256);

        int passIsValid = extractPasswordCheck(archiveFile, key_schedule);

        return passIsValid == 0;
}

long findInArchive(FILE *archiveFile, BYTE *hash_pass, char *targetFileName, long archiveSize)
{
        WORD key_schedule[60];
        aes_key_setup(hash_pass, key_schedule, 256);

        int metadataSize = METADATA_BLOCKSIZE;
        long offset = 0;
        //int found = 0;

        printf("\ninside findinarchive\n");

        while(offset <= archiveSize) {                

                int passIsValid = extractPasswordCheck(archiveFile, key_schedule);
                int *fileDeleteMarker_ptr = extractDeleteFileMarker(archiveFile, key_schedule);
                int *fileNameLength = extractFileNameLength(archiveFile, key_schedule);
                char *fileName = extractFileName(archiveFile, key_schedule);
                long* fileLength = extractFileLength(archiveFile, key_schedule);
                long lengthCounter = *fileLength;
        
                printf("original file name: %s\n", targetFileName);
                printf("dec file name: %s\n", fileName);
                printf("dec file name length: %d\n", fileNameLength[0]);
                printf("file length: %ld\n", lengthCounter);
                printf("deleted file: %d\n", fileDeleteMarker_ptr[0]);

                if (strncmp(fileName, targetFileName, strlen(targetFileName)) == 0 && (fileDeleteMarker_ptr[0] == 0)) {
                        printf("found one\n");
                        printf("\n");
                        fseek(archiveFile, -(long)metadataSize, SEEK_CUR);
                        printf("exiting findInArchive\n");

                        free(fileDeleteMarker_ptr);
                        free(fileNameLength);
                        free(fileName);
                        free(fileLength);
 
                        return offset;
                } else if (fileDeleteMarker_ptr[0] == 1) {
                        printf("file deleted from archive, skipping\n");
                        printf("\n");
                }
           
                int roundup = 0;
                while (roundup < lengthCounter) {
                        roundup += 16;
                }

                fseek(archiveFile, roundup, SEEK_CUR);
                offset += metadataSize;
                offset += roundup;

                free(fileDeleteMarker_ptr);
                free(fileNameLength);
                free(fileName);
                free(fileLength);
        }

        printf("exiting findInArchive\n");
        return -1;
}


void checkFileNameReq(char *newFileName)
{
        if (strlen(newFileName) > 100) {
                die("File name must be under 100 characters");
        }

	for(int i = 0; i < strlen(newFileName); i++) {
		if(!isascii(newFileName[i])) {
			die("ASCII characters only for file names");
                }
	}

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
        struct stat addFile_st;
        long addFileSize;
        long archiveFileSize;
        int newArchive;
        int passCount = 0;

        if (stat(archive_name, &archive_st) == 0 && S_ISDIR(archive_st.st_mode)) {
                die("Not a file");
        }

        // ******** CHECK THAT THE ARCHIVE IS NOT EMPTY

       
        while (newFile_name = *argv++) {
                
                //you dont need to check for extract, only for add but does access cover this?
                /*if (stat(newFile_name, &addFile_st) == 0 && S_ISDIR(addFile_st.st_mode)) {
                        die("Not a file");
                }*/ 
        
                checkFileNameReq(newFile_name);
        
                //* **** ADD CHECK TO SEE IF FILE IS TOO BIG FOR INT SIZE
                // ******** ADD CHECK TO SEE IF FILE NAME IS LONGER THAN 100 CHARS

                if ((strcmp(func_name, "add")) == 0) {
                        if (access(newFile_name, F_OK) == 0) {
 
                                if (passCount == 0) {
                                        if (stat(archive_name, &archive_st) != 0) {
                                                archive_fp = fopen(archive_name, "wb+");
                                                if (archive_fp == NULL) {
                                                        printf("open failed\n");
                                                        goto func_end;
                                                }
                                                newArchive = 1;
                                        } else {
                                                archive_fp = fopen(archive_name, "ab+");
                                                if (archive_fp == NULL) {
                                                        printf("open failed\n");
                                                        goto func_end;
                                                }
                                                newArchive = 0;
                                        }
                                }
                               
                                newFile_fp = fopen(newFile_name, "rb");
                                if(newFile_fp == NULL) {
                                        printf("open failed\n");
                                        goto func_end;
                                }
                                
                                fseek(newFile_fp, 0, SEEK_END);
                                addFileSize = ftell(newFile_fp) - 1;
                                fseek(newFile_fp, 0, SEEK_SET);
                                //printf("file size %ld\n", addFileSize);

                                char fileName[128];
                                memset(fileName, 0, 128);
                                int nameLength = strlen(newFile_name);
                                strcpy(fileName, newFile_name);
                                fileName[nameLength] = 0;

                                if (newArchive) {
                                        addFile(newFile_fp, archive_fp, hash_pass, nameLength, fileName, addFileSize); 
                                } else {
                                        
                                        fseek(archive_fp,0,SEEK_SET);
                                        if (keyIsValid(archive_fp, hash_pass)) {

                                                fseek(archive_fp, 0, SEEK_SET);

                                                addFile(newFile_fp, archive_fp, hash_pass, nameLength, fileName, addFileSize); 
                                        } else {
                                                fclose(newFile_fp);
                                                printf("Wrong password");
                                                goto func_end;
                                
                                        }
                                }
                                passCount++;
	                } else {
                               
                               printf("Specified file does not exist"); 
                               goto func_end;
	                }
                } else if ((strcmp(func_name, "extract")) == 0) {
                        if (access(archive_name, F_OK) == 0) {
                                
                                if (passCount == 0) {
                                        archive_fp = fopen(archive_name, "rb");
                                        if (archive_fp == NULL) {
                                                printf("open failed\n");
                                                goto func_end;
                                        }

                                        fseek(archive_fp, 0, SEEK_END);
                                        archiveFileSize = ftell(archive_fp) - 1;
                                        fseek(archive_fp, 0, SEEK_SET);
                                }

                                if (keyIsValid(archive_fp, hash_pass)) {
                                        fseek(archive_fp, 0, SEEK_SET);
                                        long offset = findInArchive(archive_fp, hash_pass, newFile_name, archiveFileSize);

                                        if (offset < 0) {
                                                printf("%s not in archive\n", newFile_name);
                                                fseek(archive_fp, 0, SEEK_SET);
                                                passCount++;
                                                continue;
                                        }

                                        extractFile(archive_fp, hash_pass, newFile_name); 

                                } else {
                                        printf("Wrong password\n");
                                        fclose(newFile_fp);
                                        goto func_end;                               
                                }        
                                passCount++;
	                } else {
                               
                                printf("Specified archive file does not exist\n");
                                fclose(newFile_fp);
                                goto func_end;
	                }

                } else if ((strcmp(func_name, "delete")) == 0) {
                        if (access(archive_name, F_OK) == 0) {
                                
                                if (passCount == 0) {
                                        archive_fp = fopen(archive_name, "r+b");
                                        if (archive_fp == NULL) {
                                                printf("open failed\n");
                                                goto func_end;
                                        }

                                        fseek(archive_fp, 0, SEEK_END);
                                        archiveFileSize = ftell(archive_fp) - 1;
                                        fseek(archive_fp, 0, SEEK_SET);
                                }
                                printf("\n");
                                printf("original file name: %s\n", newFile_name);

                                if (keyIsValid(archive_fp, hash_pass)) {
                                        fseek(archive_fp, 0, SEEK_SET);
                                        long offset = findInArchive(archive_fp, hash_pass, newFile_name, archiveFileSize);

                                        if (offset < 0) {
                                                printf("File not in archive\n");
                                                //fclose(newFile_fp);
                                                goto func_end;
                                        }

                                        deleteFile(archive_fp, hash_pass); 

                                } else {
                                        printf("Wrong password\n");
                                        goto func_end;                               
                                }        
                                passCount++;
                        }
                } else {
                        
                        die("Please choose an appropriate function: list, add, extract, delete");
                        //goto func_end;
                }
        }
                

func_end: 
        fclose(archive_fp);
	return 0;
}
