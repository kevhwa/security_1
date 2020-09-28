#include <string.h> 
#include <stdlib.h>
#include "aes.h"
#include "sha256.h"
#include <stdio.h>
#include <ctype.h> /* For isascii */
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
// include others

#define MAX_NAME_BUFF_BYTES 100 /* Maximum buffer for names */
#define MAX_PASS_BUFF_BYTES 32
#define MAX_DATA_BUFF_BYTES 4096 /* Maximum buffer for transfering data */
#define MAX_ENCRYPT_BLOCK_BITS 128
#define MAX_ENCRYPT_BLOCK_BYTES 16
#define HASH_ITR 100000
#define METADATA_BLOCKSIZE 208 // added 16 for plaintext IV
// ******* CHECK OFF_T SIZE WITH INT OVERFLOW INTERACTION. MAYBE PUT EVERYTHING INTO INT AND CHECK THAT FILE SIZE CAN'T BE BIGGER THAN MAX INT SIZE;

struct listEntry {
        char name[128];
        int count;
};

static void die(const char *message)
{
	perror(message);
	exit(1);
}

char passcode[16] = "Valid key TRUE!!";

// void checkFileNameValid
// void fileLengthValid

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


void checkPassReq(char *password)
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
// XORs the in and out buffers, storing the result in out. Length is in bytes.
void xor_buf_func(const BYTE in[], BYTE out[], size_t len)
{
	size_t idx;

	for (idx = 0; idx < len; idx++)
		out[idx] ^= in[idx];
}

BYTE *genIV_16(void)
{
        srand(time(NULL));
        int intIV[4];
        BYTE *byteIV = malloc(16);

        for (int i = 0; i < 4; i++) {
                intIV[i] = rand();
        }
        memcpy(byteIV, intIV, sizeof(byteIV));
        return byteIV;
}

BYTE *genIV_32(void)
{
        srand(time(NULL));
        int intIV[8];
        BYTE *byteIV = malloc(32);

        for (int i = 0; i < 8; i++) {
                intIV[i] = rand();
        }
        memcpy(byteIV, intIV, sizeof(byteIV));
        return byteIV;
}

BYTE *genHMAC()
{
        BYTE ipad[32*8];
        BYTE opad[32*8];
        BYTE keyBuf[SHA256_BLOCK_SIZE];
        memset(ipad, 0x36, 32);
        memset(opad, 0x5C, 32);
        BYTE *iv_buf = genIV_32();

        sha256_CTX ctxIV;
        sha256_init(&ctxIV);
        sha256_update(&ctx, iv_buf, strlen(iv_buf));
        sha256_final(&ctx, keyBuf);


}


int aes_encryptCBC(const BYTE in[], size_t in_len, BYTE out[], const WORD key[], int keysize, const BYTE iv[])
{
	BYTE buf_in[AES_BLOCK_SIZE], buf_out[AES_BLOCK_SIZE], iv_buf[AES_BLOCK_SIZE];
	int blocks, idx;

	//if (in_len % AES_BLOCK_SIZE != 0)
	//	return(FALSE);

	blocks = in_len / AES_BLOCK_SIZE;

	memcpy(iv_buf, iv, AES_BLOCK_SIZE);

	for (idx = 0; idx < blocks; idx++) {
		memcpy(buf_in, &in[idx * AES_BLOCK_SIZE], AES_BLOCK_SIZE);
		xor_buf_func(iv_buf, buf_in, AES_BLOCK_SIZE);
		aes_encrypt(buf_in, buf_out, key, keysize);
		memcpy(&out[idx * AES_BLOCK_SIZE], buf_out, AES_BLOCK_SIZE);
		memcpy(iv_buf, buf_out, AES_BLOCK_SIZE);
	}

	return 1;
}

void addIV(FILE *archiveFile, BYTE *iv_buf)
{
        size_t n;
        
        if ((n = fwrite(iv_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                die("write failed\n");
        }
}       


void encryptPasswordCheck(FILE *archiveFile, WORD *key_schedule, BYTE *iv_buf)
{
        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        char byte_buff[16];
        memset(byte_buff, 0, 16);
        size_t n;

        aes_encryptCBC(passcode, 16, enc_buf, key_schedule, 256, iv_buf);

        if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                die("write failed\n");
        }
}

void encryptDeleteFileMarker(FILE *archiveFile, int deleteMarker, WORD *key_schedule, BYTE *iv_buf)
{

        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        char byte_buff[16];
        int intArray[16];
        memset(intArray, 0, 16);
        size_t n;
        intArray[0] = deleteMarker;

        //encrypt length and write into archive
        memcpy(byte_buff, intArray, 16);

        aes_encryptCBC(byte_buff, 16, enc_buf, key_schedule, 256, iv_buf);

        if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                die("write failed\n");
        }
}

void encryptFileName(FILE *archiveFile, char *fileName, WORD *key_schedule, BYTE *iv_buf)
{
        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        char byte_buff[16];
        size_t n;

        //Encrypt file name and write into archive
        for(int i = 0; i < 8; i++) {

                memcpy(byte_buff, &fileName[i * 16], 16);

                aes_encryptCBC(byte_buff, 16, enc_buf, key_schedule, 256, iv_buf);

                if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                        die("write failed\n");
                }
        }
        //printf("enc: file = %s\n", fileName);
}

void encryptFileNameLength(FILE *archiveFile, int fileNameLength, WORD *key_schedule, BYTE *iv_buf)
{

        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        char byte_buff[16];
        int intArray[4];
        memset(intArray, 0, 16);
        size_t n;
        intArray[0] = fileNameLength;

        //encrypt length and write into archive
        memcpy(byte_buff, intArray, 16);

        aes_encryptCBC(byte_buff, 16, enc_buf, key_schedule, 256, iv_buf);

        if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                die("write failed\n");
        }
}

void encryptFileLength(FILE *archiveFile, long fileLength, WORD *key_schedule, BYTE *iv_buf)
{

        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        char byte_buff[16];
        long longArray[2];
        size_t n;
        longArray[0] = fileLength;
        longArray[1] = 0;

        //encrypt length and write into archive
        memcpy(byte_buff, longArray, 16);

        aes_encryptCBC(byte_buff, 16, enc_buf, key_schedule, 256, iv_buf);

        if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                die("write failed\n");
        }

        //printf("enc: file length = %ld\n", fileLength);
        //printf("\n");
}

void addList(FILE *list, char *fileName)
{
        size_t n;
        char buf[sizeof(struct listEntry)];
        int found = 0;

        while ((n = fread(buf, 1, sizeof(struct listEntry), list)) > 0) {
                struct listEntry *temp = (struct listEntry *)buf;
                int tempCount = temp->count;
                ++tempCount;
                if (strncmp(temp->name, fileName, strlen(fileName)) == 0) {
                        fseek(list, -sizeof(temp->count), SEEK_CUR);
                        if (fwrite(&tempCount, 1, sizeof(temp->count), list) != sizeof(temp->count)) {
                                die("write failed\n");
                        }
                        found = 1;
                }
        }

        if (!found) {
                struct listEntry *newEntry = malloc(sizeof(struct listEntry));
                strncpy(newEntry->name, fileName, strlen(fileName));
                newEntry->count = 1;

                if (fwrite(newEntry, 1, sizeof(struct listEntry), list) != sizeof(struct listEntry))
                        die("Write failed");
        }
}


void addFile(FILE *newFile, FILE *archiveFile, FILE *list, BYTE hash_pass[], int fileNameLength, char *fileName, long fileLength)
{

        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        //BYTE iv_buf[MAX_ENCRYPT_BLOCK_BITS];
        WORD key_schedule[60];
        size_t n;
        char byte_buff[16];

        aes_key_setup(hash_pass, key_schedule, 256);
        BYTE *iv_buf = genIV_16();

        //Store IV first in plain text
        addIV(archiveFile, iv_buf);
        encryptPasswordCheck(archiveFile, key_schedule, iv_buf);
        encryptDeleteFileMarker(archiveFile, 0, key_schedule, iv_buf); 
        encryptFileNameLength(archiveFile, fileNameLength, key_schedule, iv_buf);
        encryptFileName(archiveFile, fileName, key_schedule, iv_buf); 
        encryptFileLength(archiveFile, fileLength, key_schedule, iv_buf);

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

                aes_encryptCBC(buf, 16, enc_buf, key_schedule, 256, iv_buf);

                if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                        die("write failed\n");
                }
        }

        printf("\n*****Sucessfully encrypted and added %s*****\n",fileName );

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 
        free(iv_buf);
        fclose(newFile);

        addList(list, fileName);
}

int aes_decryptCBC(const BYTE in[], size_t in_len, BYTE out[], const WORD key[], int keysize, const BYTE iv[])
{
	BYTE buf_in[AES_BLOCK_SIZE], buf_out[AES_BLOCK_SIZE], iv_buf[AES_BLOCK_SIZE];
	int blocks, idx;

	//if (in_len % AES_BLOCK_SIZE != 0)
	//	return(FALSE);

	blocks = in_len / AES_BLOCK_SIZE;

	memcpy(iv_buf, iv, AES_BLOCK_SIZE);

	for (idx = 0; idx < blocks; idx++) {
		memcpy(buf_in, &in[idx * AES_BLOCK_SIZE], AES_BLOCK_SIZE);
		aes_decrypt(buf_in, buf_out, key, keysize);
		xor_buf_func(iv_buf, buf_out, AES_BLOCK_SIZE);
		memcpy(&out[idx * AES_BLOCK_SIZE], buf_out, AES_BLOCK_SIZE);
		memcpy(iv_buf, buf_in, AES_BLOCK_SIZE);
	}

	return 1;
}


int extractPasswordCheck(FILE *archiveFile, WORD *key_schedule, BYTE *iv_buf)
{
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        size_t n;
        memset(buf, 0, 16);
        memset(dec_buf, 0, 16);
 
        if ((n = fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) < 0) {
                die("read failed");
        }
                
        aes_decryptCBC(buf, 16, dec_buf, key_schedule, 256, iv_buf);
        
        //printf("password check: %s\n", dec_buf);
        return strncmp(passcode, dec_buf, 16);
}

int *extractDeleteFileMarker(FILE *archiveFile, WORD *key_schedule, BYTE *iv_buf)
{
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        //int fileMarkerArray[4];
        size_t n;
 
        if ((n = fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) < 0) {
                die("read failed");
        }
                
        aes_decryptCBC(buf, 16, dec_buf, key_schedule, 256, iv_buf);
        
        int *fileMarker_ptr = malloc(sizeof(int));
        *fileMarker_ptr = *(int *)dec_buf;
        return fileMarker_ptr;
}


char *extractFileName(FILE *archiveFile, WORD *key_schedule, BYTE *iv_buf)
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
                
               aes_decryptCBC(byte_buff, 16, dec_buf, key_schedule, 256, iv_buf);

               if (i == 0) {
                       strncpy(fileName, dec_buf, 16);
               } else {
                       strncat(fileName, dec_buf, 16);
               }
        }
        return fileName;
}

int *extractFileNameLength(FILE *archiveFile, WORD *key_schedule, BYTE *iv_buf)
{
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        //int fileLengthArray[4];
        size_t n;
 
        if ((n = fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) < 0) {
                die("read failed");
        }
                
        aes_decryptCBC(buf, 16, dec_buf, key_schedule, 256, iv_buf);
        
        int *fileNameLength = malloc(sizeof(int));
        *fileNameLength = *(int *)dec_buf;
        return fileNameLength;
}

long *extractFileLength(FILE *archiveFile, WORD *key_schedule, BYTE *iv_buf)
{
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        long fileLengthArray[2];
        size_t n;
 
        if ((n = fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) < 0) {
                die("read failed");
        }
                
        aes_decryptCBC(buf, 16, dec_buf, key_schedule, 256, iv_buf);
        
        long *fileLength = malloc(sizeof(long));
        *fileLength = *(long *)dec_buf;
        return fileLength;
}

BYTE *extractIV(FILE *archiveFile)
{
        size_t n;
        BYTE *buf = malloc(MAX_ENCRYPT_BLOCK_BYTES);
        if (fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile) != MAX_ENCRYPT_BLOCK_BYTES) {
                die("Read failed");
        }
        return buf;
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

        BYTE *iv_buf = extractIV(archiveFile);        
        int passIsValid = extractPasswordCheck(archiveFile, key_schedule, iv_buf);
        int *fileDeleteMarker_ptr = extractDeleteFileMarker(archiveFile, key_schedule, iv_buf);
        int *fileNameLength = extractFileNameLength(archiveFile, key_schedule, iv_buf);
        char *fileName = extractFileName(archiveFile, key_schedule, iv_buf);
        long *fileLength = extractFileLength(archiveFile, key_schedule, iv_buf);
        long lengthCounter = *fileLength;

        //printf("decrypted file name length: %d\n", *fileNameLength);
        printf("original name: %s\n", newFileName);
        printf("decrypted name: %s\n", fileName);
        //printf("file length: %ld\n", lengthCounter);

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
                
                aes_decryptCBC(buf, 16, dec_buf, key_schedule, 256, iv_buf);

                
                if (lengthCounter <= 16) {
                        if ((fwrite(dec_buf, 1, lengthCounter, newFile_fp)) != lengthCounter) {
                                //printf("decrypted: %s\n", dec_buf);
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
        free(iv_buf);

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 
        printf("*****Successfully decrypted %s*****\n", newFileName);
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

        BYTE *iv_buf = extractIV(archiveFile);        
        int passIsValid = extractPasswordCheck(archiveFile, key_schedule, iv_buf);
        int *fileDeleteMarker_ptr = extractDeleteFileMarker(archiveFile, key_schedule, iv_buf); //16
        int *fileNameLength = extractFileNameLength(archiveFile, key_schedule, iv_buf); //16
        char *fileName = extractFileName(archiveFile, key_schedule, iv_buf); //128
        long *fileLength = extractFileLength(archiveFile, key_schedule, iv_buf); //16
        long lengthCounter = *fileLength;

        long offset = 16 + 16 + 16 + 128;
        fseek(archiveFile,0 - offset, SEEK_CUR);
        //printf("fseek success\n");
        
        int deleteMark = 1;
        encryptDeleteFileMarker(archiveFile, deleteMark, key_schedule, iv_buf); 
       
        printf("*****Successfully deleted %s*****\n", fileName);
        free(fileDeleteMarker_ptr);
        free(fileNameLength);
        free(fileName);
        free(fileLength);
        free(iv_buf);

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 
        printf("exiting delete file\n");
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

                BYTE *iv_buf = extractIV(archiveFile);        
                int passIsValid = extractPasswordCheck(archiveFile, key_schedule, iv_buf);
                int *fileDeleteMarker_ptr = extractDeleteFileMarker(archiveFile, key_schedule, iv_buf);
                int *fileNameLength = extractFileNameLength(archiveFile, key_schedule, iv_buf);
                char *fileName = extractFileName(archiveFile, key_schedule, iv_buf);
                long* fileLength = extractFileLength(archiveFile, key_schedule, iv_buf);
                long lengthCounter = *fileLength;
        
                printf("original file name: %s\n", targetFileName);
                printf("dec file name: %s\n", fileName);
                //printf("dec file name length: %d\n", fileNameLength[0]);
                //printf("file length: %ld\n", lengthCounter);
                printf("deleted file: %d\n", fileDeleteMarker_ptr[0]);

                if (strncmp(fileName, targetFileName, strlen(targetFileName)) == 0 && (fileDeleteMarker_ptr[0] == 0)) {
                        printf("Successfully found one %s\n", targetFileName);
                        fseek(archiveFile, -(long)metadataSize, SEEK_CUR);
                        printf("exiting findInArchive\n");

                        free(fileDeleteMarker_ptr);
                        free(fileNameLength);
                        free(fileName);
                        free(fileLength);
                        free(iv_buf);
 
                        return offset;
                } else if (fileDeleteMarker_ptr[0] == 1) {
                        printf("%s deleted from archive, skipping\n", targetFileName);
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
                free(iv_buf);
        }

        printf("exiting findInArchive\n");
        return -1;
}

int keyIsValid(FILE *archiveFile, BYTE *hash_pass)
{
        WORD key_schedule[60];
        aes_key_setup(hash_pass, key_schedule, 256);

        BYTE *iv_buf = extractIV(archiveFile);
        int passIsValid = extractPasswordCheck(archiveFile, key_schedule, iv_buf);

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
		checkPassReq(password);
	} else {
		password = getpass("Please enter password");
		checkPassReq(password);
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
        FILE *newList_fp = NULL;
        struct stat archive_st;
        struct stat addFile_st;
        long addFileSize;
        long archiveFileSize;
        int newArchive;
        int passCount = 0;

        if (stat(archive_name, &archive_st) == 0 && S_ISDIR(archive_st.st_mode)) {
                die("Not a file");
        }

        int newListNameLength = strlen(archive_name);
        char nameExt[] = "-list";
        int newListExtLength = strlen(nameExt);

        char listName[newListNameLength + newListExtLength + 1];
        strncpy(listName, archive_name, newListNameLength + 1);
        strncat(listName, nameExt, newListExtLength);

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
                                                
                                                newList_fp = fopen(listName, "wb+");
                                                if(newList_fp == NULL) {
                                                        die("fopen failed");
                                                }


                                        } else {
                                                archive_fp = fopen(archive_name, "ab+");
                                                if (archive_fp == NULL) {
                                                        printf("open failed\n");
                                                        goto func_end;
                                                }
                                                newArchive = 0;

                                                newList_fp = fopen(listName, "rb+");
                                                if(newList_fp == NULL) {
                                                        die("fopen failed");
                                                }
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

                                char fileName[128];
                                memset(fileName, 0, 128);
                                int nameLength = strlen(newFile_name);
                                strcpy(fileName, newFile_name);
                                fileName[nameLength] = 0;

                                if (newArchive) {
                                        addFile(newFile_fp, archive_fp, newList_fp, hash_pass, nameLength, fileName, addFileSize); 
                                        fclose(archive_fp);

                                        archive_fp = fopen(archive_name, "ab+");
                                        if (archive_fp == NULL) {
                                                printf("open failed\n");
                                                goto func_end;
                                        }

                                } else {
                                        
                                        fseek(archive_fp,0,SEEK_SET);
                                        if (keyIsValid(archive_fp, hash_pass)) {
                                                fseek(archive_fp, 0, SEEK_SET);
                                                addFile(newFile_fp, archive_fp, newList_fp, hash_pass, nameLength, fileName, addFileSize); 
                                                fseek(archive_fp, 0, SEEK_SET);
                                        } else {
                                                fclose(newFile_fp);
                                                printf("Wrong password\n");
                                                goto func_end;
                                
                                        }
                                }
                                fseek(archive_fp, 0, SEEK_END);
                                long archiveSize = ftell(archive_fp) - 1;
                                fseek(archive_fp, 0, SEEK_SET);
                                
                                printf("***Updated archive size %ld***\n", archiveSize);
                                
                                passCount++;
	                } else {
                               
                               printf("%s does not exist\n", newFile_name); 
                               continue;
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
                                        fseek(archive_fp, 0, SEEK_SET);

                                } else {
                                        printf("Wrong password\n");
                                        goto func_end;                               
                                }        
                                passCount++;
	                } else {
                                printf("Specified archive file does not exist\n");
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

                                if (keyIsValid(archive_fp, hash_pass)) {
                                        fseek(archive_fp, 0, SEEK_SET);
                                        long offset = findInArchive(archive_fp, hash_pass, newFile_name, archiveFileSize);

                                        if (offset < 0) {
                                                printf("File not in archive\n");
                                                fseek(archive_fp, 0, SEEK_SET);
                                                passCount++;
                                                continue;
                                        }

                                        deleteFile(archive_fp, hash_pass); 
                                        fseek(archive_fp, 0, SEEK_SET);

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
        //fclose(newList_fp);
	return 0;
}
