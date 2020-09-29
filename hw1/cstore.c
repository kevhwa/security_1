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
#define METADATA_BLOCKSIZE 272
#define IVDATA_BLOCKSIZE 80
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

        memcpy(byteIV, intIV, 16);
        return byteIV;
}

BYTE *genIV_32(void)
{
        srand(time(NULL));
        int intIV[8];
        BYTE *byteIV = malloc(32);

        for (int i = 0; i < 8; i++) {
                intIV[i] = 4;
        }

        memcpy(byteIV, intIV, 32);
        if (memcmp(byteIV, intIV, 32) == 0)
                printf("the same in genIV");
        return byteIV;
}

BYTE *genHMAC(BYTE *key, BYTE *text, BYTE *iv_buf)
{
        BYTE ipad[32*8];
        BYTE opad[32*8];
        BYTE temp[32*8];
        BYTE temp2[32*8];
        memset(ipad, 0x36, 32);
        memset(opad, 0x5C, 32);
        
        SHA256_CTX ctxIV;
        BYTE *keyBuf;

        if(key == NULL) {

                sha256_init(&ctxIV);
                sha256_update(&ctxIV, iv_buf, 32);                
                keyBuf = malloc(32);
                memset(keyBuf, 0, 32);
                sha256_final(&ctxIV, keyBuf);
                key = keyBuf;
        }

        memcpy(temp, key, 32);
        memcpy(temp2, key, 32);
       
        xor_buf_func(ipad, temp, 32); // keyBuf now holds the XOR
        BYTE textAppend[SHA256_BLOCK_SIZE + 16*8];

        memset(textAppend, 0, 32 + 16);
        memcpy(textAppend, temp, 32);
        memcpy(textAppend + 32, text, 16);

        BYTE key1_buf[SHA256_BLOCK_SIZE];
        SHA256_CTX ctx1; 
        sha256_init(&ctx1);
        sha256_update(&ctx1, textAppend, 48);
        sha256_final(&ctx1, key1_buf);

        xor_buf_func(opad, temp2, 32); // keyBuf now holds the XOR
        BYTE textAppend2[SHA256_BLOCK_SIZE + 32];
        memset(textAppend2, 0, 32 + 16);
        memcpy(textAppend2, temp2, 32);
        memcpy(textAppend2 + 32, key1_buf, 32);

        SHA256_CTX ctx2; 
        sha256_init(&ctx2);
        sha256_update(&ctx2, textAppend2, 64);
        sha256_final(&ctx2, key);

        return key;
}


int aes_encryptCBC(const BYTE in[], size_t in_len, BYTE out[], const WORD key[], int keysize, const BYTE iv[])
{
	BYTE buf_in[AES_BLOCK_SIZE], buf_out[AES_BLOCK_SIZE], iv_buf[AES_BLOCK_SIZE];
	int blocks, idx;
        memset(buf_in, 0, MAX_ENCRYPT_BLOCK_BYTES);
        memset(buf_out, 0, MAX_ENCRYPT_BLOCK_BYTES);
        memset(iv_buf, 0, MAX_ENCRYPT_BLOCK_BYTES);

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

void addAES_IV(FILE *archiveFile, BYTE *iv_buf)
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
        memset(enc_buf, 0, 16);
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
        memset(enc_buf, 0, 16);
        memset(byte_buff, 0, 16);
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
        memset(enc_buf, 0, 16);
        memset(byte_buff, 0, 16);
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
        memset(enc_buf, 0, 16);
        memset(byte_buff, 0, 16);
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
        memset(enc_buf, 0, 16);
        memset(byte_buff, 0, 16);
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

void addHMAC_iv(FILE *archiveFile, BYTE *HMAC_iv)
{
        size_t n;
        
        if ((n = fwrite(HMAC_iv, 1, 32, archiveFile)) != 32) {
                die("write failed\n");
        }
}

void addHMAC_CodeSpace(FILE *archiveFile)
{
        size_t n;
        char emptyBuf[32];
        memset(emptyBuf, 0, 32);
        
        if ((n = fwrite(emptyBuf, 1, 32, archiveFile)) != 32) {
                die("write failed\n");
        }
}

//void addHMAC_Code(File *archiveFile, BYTE *HMAC_Code)
//{

//}

void addFile(FILE *newFile, FILE *archiveFile, FILE *list, BYTE hash_pass[], int fileNameLength, char *fileName, long fileLength)
{
        printf("\nIn addFile\n");
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        //BYTE iv_buf[MAX_ENCRYPT_BLOCK_BITS];
        WORD key_schedule[60];
        size_t n;
        char byte_buff[16];
        int round=0;
        BYTE *key = NULL;
        BYTE text[16*8];
        memset(buf, 0, 16);        
        memset(enc_buf, 0, 16);
        
        aes_key_setup(hash_pass, key_schedule, 256);
        BYTE *iv_buf = genIV_16();
        BYTE *HMAC_iv = genIV_32();
        
        //Store IV first in plain text
        addHMAC_iv(archiveFile, HMAC_iv); //32
        addHMAC_CodeSpace(archiveFile); //32
        addAES_IV(archiveFile, iv_buf); //16
        encryptPasswordCheck(archiveFile, key_schedule, iv_buf); //16
        encryptDeleteFileMarker(archiveFile, 0, key_schedule, iv_buf); //16
        encryptFileNameLength(archiveFile, fileNameLength, key_schedule, iv_buf); //16
        encryptFileName(archiveFile, fileName, key_schedule, iv_buf); //128
        encryptFileLength(archiveFile, fileLength, key_schedule, iv_buf); //16

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
                memcpy(text, enc_buf, 16);
                BYTE *temp = genHMAC(key, text, HMAC_iv);
                key = temp;

                if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                        die("write failed\n");
                }

                round++;
        }

        printf("\n*****Sucessfully encrypted and added %s*****\n",fileName );

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 
        addList(list, fileName);

        int offset = (round * 16) + (16 * 5) + 128 + 32;

        fseek(archiveFile, -offset, SEEK_CUR); //get you to HMAC code space
        if (fwrite(key, 1, 32, archiveFile) != 32)
                die("write failed");
                
        
        //printf("rounds: %d\n", round);        
        //printf("written key: %s\n", key);
        //printf("ADDED HMAC IV: %s\n", HMAC_iv);

        free(key);
        fclose(newFile);
/*
        printf("size of first hmac iv %d\n", strlen(HMAC_iv));
///////////////////////////////////
        FILE *temp = fopen("temp", "r+");
        fwrite(HMAC_iv, 1, 32, temp);
        fseek(temp, 0, SEEK_SET);
        BYTE tempbuf[32];
        if (fread(tempbuf, 1, 32, temp) != 32) {
                die("fread failed here");
        }

        BYTE tempbuf2[32];
        memcpy(tempbuf2, tempbuf, 32);

        FILE *temp20 = fopen("temp2", "r+");
        fwrite(tempbuf, 1, 32, temp20);

        //printf("size of second hmac iv %d\n", strlen(tempbuf));

        if ( memcmp(HMAC_iv, tempbuf, 32) == 0)
                printf("this is right beg\n");
        int intIV[8];

        for (int i = 0; i < 8; i++) {
                intIV[i] = 4;
        }

        if (memcmp(HMAC_iv, intIV, 32) == 0)
                printf("the same data in add\n");
 
       if (memcmp(intIV, tempbuf, 32) == 0)
                printf("something not wrong with fread\n");

       if (memcmp(tempbuf, HMAC_iv,32) == 0)
                printf("whats going on\n");


        ///////
        FILE *temp16 = fopen("16temp", "w+");
        fwrite(iv_buf, 1, 16, temp16);
        fseek(temp16, 0, SEEK_SET);
        BYTE tempbuf16[16*8];
        fread(tempbuf16, 1, 16, temp16);
        
        if (memcmp(iv_buf, tempbuf16, 16) == 0)
                printf("this is right\n");

        fclose(temp);
  */      
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

/*int checkFileHMAC(FILE *archiveFile) 
{
        BYTE *temp = genHMAC(key, text, HMAC_iv);
}*/

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

BYTE *extractASE_IV(FILE *archiveFile)
{
        size_t n;
        BYTE *buf = malloc(MAX_ENCRYPT_BLOCK_BYTES);
        if (fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile) != MAX_ENCRYPT_BLOCK_BYTES) {
                die("Read failed");
        }
        return buf;
}

BYTE *extractHMAC_Code(FILE *archiveFile)
{
        size_t n;
        BYTE *buf = malloc(32);
        if (fread(buf, 1, 32, archiveFile) != 32) {
                die("Read failed");
        }
        return buf;
}

BYTE *extractHMAC_IV(FILE *archiveFile)
{
        size_t n;
        BYTE *buf = malloc(32);
        if (fread(buf, 1, 32, archiveFile) != 32) {
                die("Read failed");
        }
        return buf;
}

int checkFileHMAC(FILE *archiveFile, BYTE hash_pass[], char *newFileName)
{
        printf("\nInside checkFileHMAC file\n");
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        //BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE text[16*8];
        BYTE *key = NULL;
        WORD key_schedule[60];
        size_t n;
        long offset = 0;

        aes_key_setup(hash_pass, key_schedule, 256);

        BYTE *HMAC_IV = extractHMAC_IV(archiveFile);
        BYTE *HMAC_Code = extractHMAC_Code(archiveFile);
        BYTE *iv_buf = extractASE_IV(archiveFile);  
        int passIsValid = extractPasswordCheck(archiveFile, key_schedule, iv_buf);
        int *fileDeleteMarker_ptr = extractDeleteFileMarker(archiveFile, key_schedule, iv_buf);
        int *fileNameLength = extractFileNameLength(archiveFile, key_schedule, iv_buf);
        char *fileName = extractFileName(archiveFile, key_schedule, iv_buf);
        long *fileLength = extractFileLength(archiveFile, key_schedule, iv_buf);
        long lengthCounter = *fileLength;

        int roundup = 0; 
        while (roundup < lengthCounter) {
                roundup += 16;
        }

        printf("original name: %s\n", newFileName);
        printf("decrypted name: %s\n", fileName);
        int round = 0; //this is for a check, can remove later
        // Read in 16 bytes
        while (roundup > 0 && (n = fread(buf, 1, sizeof(buf) / 8, archiveFile)) > 0) {
                memcpy(text, buf, 16);
                BYTE *temp = genHMAC(key, text, HMAC_IV);
                key = temp;
                offset += n;
                roundup -= n;
                round++;
        }
 
        int memcmperror = 0;
        if((memcmperror = memcmp(key, HMAC_Code, 32)) != 0) {
                printf("READ HMAC CODE VS RECALCULATED HMAC CODE: %d\n", memcmperror);
                return -1;
        }

       
        free(fileDeleteMarker_ptr);
        free(fileNameLength);
        free(fileName);
        free(fileLength);
        free(iv_buf);
        free(HMAC_IV);
        free(HMAC_Code);

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 

        //printf("round: %d\n", round);
        //printf("key: %s\n", key);

        //FILE *temp = fopen("temp", "r");
        //BYTE tempbuf[32];
        //fread(tempbuf, 1, 32, temp);

        //int error1 = memcmp(HMAC_IV, tempbuf, 32);
        //printf("read iv vs stored iv %d\n", error1);
 

        //printf("READ HMAC IV %s\n", HMAC_IV);


        fseek(archiveFile, -(offset + METADATA_BLOCKSIZE), SEEK_CUR);
        printf("*****Successfully decrypted %s HMAC *****\n", newFileName);
        printf("exiting extract file\n");

        return 1;
}

int extractFile(FILE *archiveFile, BYTE hash_pass[], char *newFileName)
{
        printf("\nInside extract file\n");
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE text[16*8];
        BYTE *key = NULL;
        WORD key_schedule[60];
        size_t n;
        int fileByteCount = 0;

        aes_key_setup(hash_pass, key_schedule, 256);

        BYTE *HMAC_IV = extractHMAC_IV(archiveFile);
        BYTE *HMAC_Code = extractHMAC_Code(archiveFile);
        BYTE *iv_buf = extractASE_IV(archiveFile);  
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
                int roundup = 0;
                while (roundup < lengthCounter) {
                        roundup += 16;
                }


        // Read in 16 bytes
        while (lengthCounter >= 0 && (n = fread(buf, 1, sizeof(buf) / 8, archiveFile)) > 0) {
 
                //memcpy(text, buf, 16);
               // BYTE *temp = genHMAC(key, text, HMAC_IV);
                //key = temp;
               
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
        free(HMAC_IV);
        free(HMAC_Code);

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 
        printf("*****Successfully decrypted %s*****\n", newFileName);
        printf("exiting extract file\n");
        fclose(newFile_fp);
        
       
        if(memcmp(key, HMAC_Code, 32) != 0) {
                free(key);

                return -1;
        }

        free(key);
        return 1;
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

        BYTE *HMAC_IV = extractHMAC_IV(archiveFile);
        BYTE *HMAC_Code = extractHMAC_Code(archiveFile);
        BYTE *iv_buf = extractASE_IV(archiveFile);        
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
        free(HMAC_IV);
        free(HMAC_Code);

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

                BYTE *HMAC_IV = extractHMAC_IV(archiveFile);    //32            
                BYTE *HMAC_Code = extractHMAC_Code(archiveFile); //32
                BYTE *iv_buf = extractASE_IV(archiveFile);      //16
                int passIsValid = extractPasswordCheck(archiveFile, key_schedule, iv_buf); //16
                int *fileDeleteMarker_ptr = extractDeleteFileMarker(archiveFile, key_schedule, iv_buf); //16
                int *fileNameLength = extractFileNameLength(archiveFile, key_schedule, iv_buf); //16
                char *fileName = extractFileName(archiveFile, key_schedule, iv_buf); //128
                long* fileLength = extractFileLength(archiveFile, key_schedule, iv_buf); //16
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
                        free(HMAC_Code);
                        free(HMAC_IV);
 
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
                free(HMAC_IV);
                free(HMAC_Code);
        }

        printf("exiting findInArchive\n");
        return -1;
}

int keyIsValid(FILE *archiveFile, BYTE *hash_pass)
{
        WORD key_schedule[60];
        aes_key_setup(hash_pass, key_schedule, 256);

        BYTE *HMAC_IV = extractHMAC_IV(archiveFile);
        BYTE *HMAC_Code = extractHMAC_Code(archiveFile);
        BYTE *iv_buf = extractASE_IV(archiveFile);
        int passIsValid = extractPasswordCheck(archiveFile, key_schedule, iv_buf);

        free(HMAC_IV);
        free(HMAC_Code);
        free(iv_buf);

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

                                                fclose(archive_fp);

                                                
                                                newList_fp = fopen(listName, "wb+");
                                                if(newList_fp == NULL) {
                                                        die("fopen failed in main add1");
                                                }
                                                fclose(newList_fp);
                                                
                                                archive_fp = fopen(archive_name, "rb+");
                                                if (archive_fp == NULL) {
                                                        printf("open failed\n");
                                                        goto func_end;
                                                }
                                                
                                                newList_fp = fopen(listName, "rb+");
                                                if(newList_fp == NULL) {
                                                        die("fopen failed in main add1");
                                                }

                                        } else {
                                                archive_fp = fopen(archive_name, "rb+");
                                                if (archive_fp == NULL) {
                                                        printf("open failed\n");
                                                        goto func_end;
                                                }
                                                newArchive = 0;

                                                newList_fp = fopen(listName, "rb+");
                                                if(newList_fp == NULL) {
                                                        printf("fopen failed. Could be because archive file exists but not list file, which are always created together.\n");
                                                        goto func_end;
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
                                        //Add space for archive IV and Code
                                        addFile(newFile_fp, archive_fp, newList_fp, hash_pass, nameLength, fileName, addFileSize); 
                                        //addHMAC_Code(archive_fp);
                                        /*archive_fp = fopen(archive_name, "ab+");
                                        if (archive_fp == NULL) {
                                                printf("open failed\n");
                                                goto func_end;
                                        }*/

                                } else {
                                        
                                        fseek(archive_fp,0,SEEK_SET);
                                        if (keyIsValid(archive_fp, hash_pass)) {
                                                fseek(archive_fp, 0, SEEK_END);
                                                addFile(newFile_fp, archive_fp, newList_fp, hash_pass, nameLength, fileName, addFileSize); 
                                                //addHMAC_Code(archive_fp);
                                                fseek(archive_fp, 0, SEEK_SET);
                                        } else {
                                                printf("Wrong password\n");
                                                goto func_end;
                                
                                        }
                                }
                                fseek(archive_fp, 0, SEEK_END);
                                long archiveSize = ftell(archive_fp) - 1;
                                fseek(archive_fp, 0, SEEK_SET);
                                
                                printf("***Updated archive size %ld***\n", archiveSize);
                                
                                passCount++;
                                newArchive = 0;
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

                                        if(checkFileHMAC(archive_fp, hash_pass, newFile_name) != 1) {
                                                printf("Archived file, %s, has been tampered with. Aborting process\n", newFile_name);   
                                                goto func_end;

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
