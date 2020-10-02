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
#define METADATA_BLOCKSIZE 192 //272
#define IVDATA_BLOCKSIZE 48//80
#define ARCHIVE_HMAC_BLOCKSIZE 32 
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

BYTE *genHMAC(BYTE *key, BYTE *text, long size)
{
        BYTE ipad[32*8];
        BYTE opad[32*8];
        BYTE temp[32*8];
        BYTE temp2[32*8];
        memset(ipad, 0x36, 32);
        memset(opad, 0x5C, 32);
        
        SHA256_CTX ctx1;
        BYTE keyBuf[32*8];
        BYTE *keyBuf2 = malloc(32);
        SHA256_CTX ctx2;

        memcpy(temp, key, 32);
        memcpy(temp2, key, 32);
        
        xor_buf_func(ipad, temp, 32); // temp now holds the XOR
        BYTE textAppend[SHA256_BLOCK_SIZE + (size*8)];

        memset(textAppend, 0, 32 + size);
        memcpy(textAppend, temp, 32);
        memcpy(textAppend + 32, text, size);

        sha256_init(&ctx1);
        sha256_update(&ctx1, textAppend, size + 32);
        sha256_final(&ctx1, keyBuf); //holds the interim hash value
        
        xor_buf_func(opad, temp2, 32); // temp now holds the XOR

        BYTE textAppend2[SHA256_BLOCK_SIZE + 32];
        memset(textAppend2, 0, 32 + 32);
        memcpy(textAppend2, temp2, 32);
        memcpy(textAppend2 + 32, keyBuf, 32);
        
        sha256_init(&ctx2);
        sha256_update(&ctx2, textAppend2, 64);
        sha256_final(&ctx2, keyBuf2);
        
        return keyBuf2;

}

int aes_encryptCBC(const BYTE in[], size_t in_len, BYTE out[], const WORD key[], int keysize, const BYTE iv[])
{
	BYTE buf_in[AES_BLOCK_SIZE], buf_out[AES_BLOCK_SIZE], iv_buf[AES_BLOCK_SIZE];
	int blocks, idx;
        memset(buf_in, 0, MAX_ENCRYPT_BLOCK_BYTES);
        memset(buf_out, 0, MAX_ENCRYPT_BLOCK_BYTES);
        memset(iv_buf, 0, MAX_ENCRYPT_BLOCK_BYTES);

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

int aes_decryptCBC(const BYTE in[], size_t in_len, BYTE out[], const WORD key[], int keysize, const BYTE iv[])
{
	BYTE buf_in[AES_BLOCK_SIZE], buf_out[AES_BLOCK_SIZE], iv_buf[AES_BLOCK_SIZE];
	int blocks, idx;

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
}

void addList(FILE *list, char *fileName) //, int fileNameLength)
{
        size_t n;
        char buf[sizeof(struct listEntry)];
        int found = 0;
        //printf("\nInside addlist\n");

        while ((n = fread(buf, 1, sizeof(struct listEntry), list)) > 0) {
                struct listEntry *temp = (struct listEntry *)buf;
                //printf("name of entry: %s\n", temp->name);
                //printf("count of entry: %d\n", temp->count);
                if (strncmp(temp->name, fileName, strlen(temp->name)) == 0) {
                        fseek(list, -sizeof(temp->count), SEEK_CUR);
                        int tempCount = temp->count;
                        tempCount++;
                        //char strCount[4];
                        //snprintf(strCount, 4, "%d", tempCount);
                        if (fwrite(&tempCount, 1, sizeof(temp->count), list) != sizeof(temp->count)) {
                                die("write failed\n");
                        }
                        found = 1;
                }
        }

        if (!found) {
                //printf("Writing for the first time\n");
                struct listEntry *newEntry = malloc(sizeof(struct listEntry));
                memset(newEntry, 0, sizeof(struct listEntry));
                strncpy(newEntry->name, fileName, strlen(fileName));
                newEntry->count = 1;

                //char strCount[4];
                //snprintf(strCount, 4, "%d", tempCount);

                if (fwrite(newEntry, 1, sizeof(struct listEntry), list) != sizeof(struct listEntry))
                        die("Write failed");

                free(newEntry);
        }
}

void listList(FILE *list) //, int fileNameLength)
{
        //printf("\nInside listList\n");
        
        size_t n;
        char buf[sizeof(struct listEntry)];

        while ((n = fread(buf, 1, sizeof(struct listEntry), list)) > 0) {
                struct listEntry *temp = (struct listEntry *)buf;
                if (temp->count > 0) {
                        printf("%s\n", temp->name);
                }
        } 

        //printf("Exiting listList\n");
                
}


int deleteList(FILE *list, char *fileName) //, int fileNameLength)
{
        size_t n;
        char buf[sizeof(struct listEntry)];
        int found = 0;

        //printf("\nInside delete list\n");
        while ((n = fread(buf, 1, sizeof(struct listEntry), list)) > 0) {
                struct listEntry *temp = (struct listEntry *)buf;
                //printf("file name: %s\n", temp->name);
                //printf("file count: %d\n", temp->count);
                if (strncmp(temp->name, fileName, strlen(temp->name)) == 0 && temp->count > 0) {
                        fseek(list, -sizeof(temp->count), SEEK_CUR);
                        int tempCount = temp->count;
                        tempCount--;
                        if (fwrite(&tempCount, 1, sizeof(temp->count), list) != sizeof(temp->count)) {
                                die("write failed\n");
                        }
                        found = 1;
                }
        }

        if (!found) {
                return -1;
        }
}
/*
void addHMAC_iv(FILE *archiveFile, BYTE *HMAC_iv)
{
        size_t n;
        
        if ((n = fwrite(HMAC_iv, 1, 32, archiveFile)) != 32) {
                die("write failed\n");
        }
}*/

void addHMAC_CodeSpace(FILE *archiveFile)
{
        size_t n;
        char emptyBuf[32];
        memset(emptyBuf, 0, 32);
        
        if ((n = fwrite(emptyBuf, 1, 32, archiveFile)) != 32) {
                die("write failed\n");
        }
}

void addArchiveHMAC_Space(FILE *archiveFile)
{
        //BYTE *HMAC_iv = genIV_32();
        //addHMAC_iv(archiveFile, HMAC_iv);

        size_t n;
        char emptyBuf[32];
        memset(emptyBuf, 0, 32);
        
        if ((n = fwrite(emptyBuf, 1, 32, archiveFile)) != 32) {
                die("write failed\n");
        }

        //free(HMAC_iv);
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
        BYTE *buf = malloc(16); //MAX_ENCRYPT_BLOCK_BYTES);
        if (fread(buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile) != MAX_ENCRYPT_BLOCK_BYTES) {
                printf("in extract: %s\n", buf);
                die("Read failed in extractASE");
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

void updateFileHMAC(FILE *archiveFile, BYTE hash_pass[], long size)
{
        size_t n;
        BYTE *buf = malloc(size);

        long index = ftell(archiveFile);
        //printf("archive position should be 32: %ld\n",index); 

        if ((n = fread(buf, 1, size, archiveFile)) != size) {
                die("read failed, updatefilehmac");
        }
 
        index = ftell(archiveFile);
        //printf("archive position should be near end: %ld\n",index);        

        BYTE *HMAC_code = genHMAC(hash_pass, buf, size);

        fseek(archiveFile, -(size + 32), SEEK_CUR);
        if ((n = fwrite(HMAC_code, 1, 32, archiveFile)) != 32) {
                die("write failed");
        } 
        index = ftell(archiveFile);
        //printf("archive position should be 64: %ld\n",index); 

        free(buf);
        free(HMAC_code);

        printf("*****Successfully updated all file HMAC *****\n");
        //printf("exiting update file HMAC \n");

}

void addFile(FILE *newFile, FILE *archiveFile, FILE *list, BYTE hash_pass[], int fileNameLength, char *fileName, long fileLength)
{
        printf("\nIn addFile\n");
        long index = ftell(archiveFile);
        //printf("index should be 32: %ld\n", index);


        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE enc_buf[MAX_ENCRYPT_BLOCK_BITS];
        //BYTE iv_buf[MAX_ENCRYPT_BLOCK_BITS];
        WORD key_schedule[60];
        size_t n;
        char byte_buff[16];
        int round=0;
        //BYTE *key = NULL;
        BYTE text[16*8];
        memset(buf, 0, 16);        
        memset(enc_buf, 0, 16);
        
        aes_key_setup(hash_pass, key_schedule, 256);
        BYTE *iv_buf = genIV_16();
        //BYTE *HMAC_iv = genIV_32();
      
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

                        char fill_amount = 'a';

                        for (int i = n; i < sizeof(byte_buff) ; i++ ) {
                                byte_buff[i] = fill_amount;
                        }

                        memcpy(buf, byte_buff, MAX_ENCRYPT_BLOCK_BYTES);
                }

                aes_encryptCBC(buf, 16, enc_buf, key_schedule, 256, iv_buf);

                if ((n = fwrite(enc_buf, 1, MAX_ENCRYPT_BLOCK_BYTES, archiveFile)) != MAX_ENCRYPT_BLOCK_BYTES) {
                        die("write failed\n");
                }

                round++;
        }

        printf("*****Successfully encrypted and added %s*****\n",fileName );

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 
  
        long roundup = 0;
        while (roundup < fileLength) {
                roundup += 16;
        }
/*
        int offset = (round * 16) + (16 * 5) + 128 + 32;

        fseek(archiveFile, -offset, SEEK_CUR); //get you to HMAC code space
        if (fwrite(key, 1, 32, archiveFile) != 32)
                die("write failed");
*/
        fseek(list, 0, SEEK_SET);
        addList(list, fileName);

        fseek(archiveFile, -(roundup + METADATA_BLOCKSIZE + 16), SEEK_CUR);
        updateFileHMAC(archiveFile, hash_pass, (roundup + METADATA_BLOCKSIZE + 16));
        //free(key);
        free(iv_buf);
        //free(HMAC_iv);
        fclose(newFile);
}





int checkFileHMAC(FILE *archiveFile, BYTE hash_pass[])
{
        //printf("\nInside checkFileHMAC file\n");
        WORD key_schedule[60];
        aes_key_setup(hash_pass, key_schedule, 256);

        while(1) {
        
                size_t n;

                //BYTE *HMAC_IV = extractHMAC_IV(archiveFile);
                BYTE *HMAC_code = extractHMAC_Code(archiveFile);
                BYTE *iv_buf = extractASE_IV(archiveFile);  
                int passIsValid = extractPasswordCheck(archiveFile, key_schedule, iv_buf);
                int *fileDeleteMarker_ptr = extractDeleteFileMarker(archiveFile, key_schedule, iv_buf);
                int *fileNameLength = extractFileNameLength(archiveFile, key_schedule, iv_buf);
                char *fileName = extractFileName(archiveFile, key_schedule, iv_buf);
                long *fileLength = extractFileLength(archiveFile, key_schedule, iv_buf);
                long lengthCounter = *fileLength;

                long roundup = 0;
                while (roundup < lengthCounter) {
                       roundup += 16;
                }
               
                printf("file name: %s\n", fileName);

                long size = roundup + METADATA_BLOCKSIZE + 16;
                BYTE *textBuf = malloc(size);
                

                fseek(archiveFile, -(METADATA_BLOCKSIZE + 16), SEEK_CUR);
                if ((n = fread(textBuf, 1, size, archiveFile)) != size) {
                        die("read failed");
                }
                
                BYTE *HMAC_code_calc = genHMAC(hash_pass, textBuf, size);
                //printf("read file hmac %s\n", HMAC_code);
                //printf("recalc file hmac %s\n", HMAC_code_calc);
        
                if(memcmp(HMAC_code_calc, HMAC_code, 32) != 0) {
                        free(fileDeleteMarker_ptr);
                        free(fileNameLength);
                        free(fileName);
                        free(fileLength);
                        free(iv_buf);
                        free(HMAC_code_calc);
                        free(HMAC_code);
                        free(textBuf);

                       return -1;
                }
       
                free(fileDeleteMarker_ptr);
                free(fileNameLength);
                free(fileName);
                free(fileLength);
                free(iv_buf);
                free(HMAC_code);
                free(HMAC_code_calc);
                free(textBuf);

                if (ferror(archiveFile)) {
                        die("fread failed\n");
                }

                char buf[4];
                if ((n = fread(buf, 1, 4, archiveFile)) == 0) {
                        break;
                }
                fseek(archiveFile, -n, SEEK_CUR);
        }

        //fseek(archiveFile, -(offset + METADATA_BLOCKSIZE), SEEK_CUR);
        printf("\n*****Successfully validated all file HMACs *****\n");
        //printf("exiting HMAC check file\n");

        return 1;
}

int checkArchiveHMAC(FILE *archiveFile, BYTE *hash_pass, long size)
{
        //printf("\nInside check archive hmac file\n");
        size_t n;
        BYTE textBuf[size*8];

        BYTE *HMAC_code = extractHMAC_Code(archiveFile);
       
        // Read in 16 bytes
        if ((n = fread(textBuf, 1, sizeof(textBuf) / 8, archiveFile)) != sizeof(textBuf) / 8) {
                die("read failed");
        }
        
        BYTE *HMAC_code_calc = genHMAC(hash_pass, textBuf, size);
       

        if (memcmp(HMAC_code, HMAC_code_calc, 32) != 0) {     
                free(HMAC_code_calc);
                free(HMAC_code);
                return -1;
        }


        if (ferror(archiveFile)) {
                die("fread failed\n");
        }

        free(HMAC_code_calc);
        free(HMAC_code);

        //fseek(archiveFile, -(offset + METADATA_BLOCKSIZE), SEEK_CUR);
        printf("\n*****Successfully validated archive HMAC *****\n");
        //printf("exiting update HMAC \n");

        return 1;

}

void updateArchiveHMAC(FILE *archiveFile, BYTE *hash_pass, long size)
{
        size_t n;
        BYTE textBuf[size*8];

        BYTE *HMAC_code = extractHMAC_Code(archiveFile);
       
        if ((n = fread(textBuf, 1, sizeof(textBuf) / 8, archiveFile)) != sizeof(textBuf) / 8) {
                die("read failed");
        }
        
        BYTE *HMAC_code_calc = genHMAC(hash_pass, textBuf, size);
 
        fseek(archiveFile, 0, SEEK_SET);
        

        if (ferror(archiveFile)) {
                die("fread failed\n");
        }

        if (fwrite(HMAC_code_calc, 1, 32, archiveFile) != 32)
                die("write failed");

        free(HMAC_code);
        free(HMAC_code_calc);
        //free(HMAC_IV);

        printf("*****Successfully updated archive HMAC *****\n");
        //printf("exiting update HMAC \n");

        //return 1;

}

void extractFile(FILE *archiveFile, BYTE hash_pass[], char *newFileName)
{
        printf("\nInside extract file\n");
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE text[16*8];
        //BYTE *key = NULL;
        WORD key_schedule[60];
        size_t n;
        int fileByteCount = 0;

        aes_key_setup(hash_pass, key_schedule, 256);

        //BYTE *HMAC_IV = extractHMAC_IV(archiveFile);
        //BYTE *HMAC_Code = extractHMAC_Code(archiveFile);
        BYTE *iv_buf = extractASE_IV(archiveFile);  
        int passIsValid = extractPasswordCheck(archiveFile, key_schedule, iv_buf);
        int *fileDeleteMarker_ptr = extractDeleteFileMarker(archiveFile, key_schedule, iv_buf);
        int *fileNameLength = extractFileNameLength(archiveFile, key_schedule, iv_buf);
        char *fileName = extractFileName(archiveFile, key_schedule, iv_buf);
        long *fileLength = extractFileLength(archiveFile, key_schedule, iv_buf);
        long lengthCounter = *fileLength;

        printf("original name: %s\n", newFileName);
        printf("decrypted name: %s\n", fileName);

        int newFileNameLength = strlen(newFileName);
        char nameExt[] = "Decrypted-";
        int newFileExtLength = strlen(nameExt);

        char decFileName[newFileNameLength + newFileExtLength + 1];
        strncpy(decFileName, nameExt, newFileExtLength + 1);
        strncat(decFileName, newFileName, newFileNameLength);

        FILE *newFile_fp = fopen(decFileName, "wb+");
        if(newFile_fp == NULL) {
                die("fopen failed");
        }
                
        long roundup = 0;
        while (roundup < lengthCounter) {
                roundup += 16;
        }

        // Read in 16 bytes
        while (lengthCounter >= 0 && (n = fread(buf, 1, sizeof(buf) / 8, archiveFile)) > 0) {
 
                aes_decryptCBC(buf, 16, dec_buf, key_schedule, 256, iv_buf);
              
                if (lengthCounter <= 16) {
                        if ((fwrite(dec_buf, 1, lengthCounter, newFile_fp)) != lengthCounter) {
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
        //free(HMAC_IV);
        //free(HMAC_Code);
        fclose(newFile_fp);

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 
        printf("*****Successfully extracted %s*****\n", newFileName);
        //printf("exiting extract file\n");
        
}

int deleteFile(FILE *archiveFile, BYTE hash_pass[], FILE *listFile, char *newFileName)
{
        printf("\nInside delete file\n");
        BYTE buf[MAX_ENCRYPT_BLOCK_BITS];
        BYTE dec_buf[MAX_ENCRYPT_BLOCK_BITS];
        WORD key_schedule[60];
        size_t n;
        int fileByteCount = 0;
        
        aes_key_setup(hash_pass, key_schedule, 256);

        //BYTE *HMAC_Code = extractHMAC_Code(archiveFile);
        BYTE *iv_buf = extractASE_IV(archiveFile);  //16       
        int passIsValid = extractPasswordCheck(archiveFile, key_schedule, iv_buf); //16
        int *fileDeleteMarker_ptr = extractDeleteFileMarker(archiveFile, key_schedule, iv_buf); //16
        int *fileNameLength = extractFileNameLength(archiveFile, key_schedule, iv_buf); //16
        char *fileName = extractFileName(archiveFile, key_schedule, iv_buf); //128
        long *fileLength = extractFileLength(archiveFile, key_schedule, iv_buf); //16
        long lengthCounter = *fileLength;

        long offset = 16 + 16 + 16 + 128;
        fseek(archiveFile, -offset, SEEK_CUR);
        
        int deleteMark = 1;
        encryptDeleteFileMarker(archiveFile, deleteMark, key_schedule, iv_buf); 
       
        printf("*****Successfully deleted %s*****\n", fileName);
        free(fileDeleteMarker_ptr);
        free(fileNameLength);
        free(fileName);
        free(fileLength);
        free(iv_buf);
        //free(HMAC_IV);
        //free(HMAC_Code);

        long roundup = 0;
        while (roundup < lengthCounter) {
                roundup += 16;
        }

        fseek(archiveFile, -(16*3), SEEK_CUR);
        updateFileHMAC(archiveFile, hash_pass, roundup + METADATA_BLOCKSIZE + 16); 

        fseek(listFile, 0, SEEK_SET);
        int error = deleteList(listFile, newFileName);
        if (error < 0)
                return -1;

        //return 1;

        if (ferror(archiveFile)) {
                die("fread failed\n");
        } 
        //printf("exiting delete file\n");
        return 1;
}

long findInArchive(FILE *archiveFile, BYTE *hash_pass, char *targetFileName, long archiveSize)
{
        WORD key_schedule[60];
        aes_key_setup(hash_pass, key_schedule, 256);

        int metadataSize = METADATA_BLOCKSIZE;
        long offset = 0;
        //int found = 0;

        printf("\ninside findinarchive\n");

        while(offset < archiveSize) {                

                //BYTE *HMAC_IV = extractHMAC_IV(archiveFile);    //32            
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
                printf("deleted file: %d\n", fileDeleteMarker_ptr[0]);

                if (strncmp(fileName, targetFileName, strlen(targetFileName)) == 0 && (fileDeleteMarker_ptr[0] == 0)) {
                        printf("Successfully found one %s\n", targetFileName);
                        fseek(archiveFile, -((long)metadataSize + 16), SEEK_CUR);
                        printf("exiting findInArchive\n");

                        free(fileDeleteMarker_ptr);
                        free(fileNameLength);
                        free(fileName);
                        free(fileLength);
                        free(iv_buf);
                        free(HMAC_Code);
                        //free(HMAC_IV);
 
                        return offset;
                } else if (fileDeleteMarker_ptr[0] == 1) {
                        //printf("%s deleted from archive, skipping\n", targetFileName);
                        //printf("\n");
                }
           
                long roundup = 0;
                while (roundup < lengthCounter) {
                        roundup += 16;
                }

                fseek(archiveFile, roundup, SEEK_CUR);
                offset += metadataSize;
                offset += roundup;
                offset += (16+32);

                free(fileDeleteMarker_ptr);
                free(fileNameLength);
                free(fileName);
                free(fileLength);
                free(iv_buf);
                //free(HMAC_IV);
                free(HMAC_Code);
        }

        printf("exiting findInArchive\n");
        return -1;
}

int keyIsValid(FILE *archiveFile, BYTE *hash_pass)
{
        WORD key_schedule[60];
        aes_key_setup(hash_pass, key_schedule, 256);

        //BYTE *HMAC_IV = extractHMAC_IV(archiveFile);
        BYTE *HMAC_Code = extractHMAC_Code(archiveFile);
        BYTE *iv_buf = extractASE_IV(archiveFile);
        int passIsValid = extractPasswordCheck(archiveFile, key_schedule, iv_buf);

        //free(HMAC_IV);
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


	//If command is list, list and end
	if(strcmp(func_name,func_list) == 0) {
                //ONLY USED FOR LIST
                //printf("\nInside main list\n");
        	
                char *theArchiveName;
                while (theArchiveName = *argv++) {
                
                        int theArchiveNameLength = strlen(theArchiveName);
                        char nameExtTEMP[] = "-list.txt";
                        int newListExtLengthTEMP = strlen(nameExtTEMP);

                        char listNameTEMP[theArchiveNameLength + newListExtLengthTEMP + 1];
                        strncpy(listNameTEMP, theArchiveName, theArchiveNameLength + 1);
                        strncat(listNameTEMP, nameExtTEMP, newListExtLengthTEMP);
                        //printf("archive name %s\n", *argv);
                        //printf("list name %s\n", listNameTEMP);

                        if((access(theArchiveName, F_OK) == 0) && access(listNameTEMP, F_OK) == 0) {
                                FILE *listFile_fp = fopen(listNameTEMP, "rb");
                                if(listFile_fp == NULL) {
                                        fclose(listFile_fp);
                                        die("open failed\n");
                                }
                                printf("*****Current files in %s*****\n", theArchiveName); 
        		        listList(listFile_fp);
                                fclose(listFile_fp);
		        } else {
                                printf("Cannot access either archive file or list file\n");
                                return 0;
                        }
                }
		return 0;
	}

	//From here, must have a password
	//Must add capability to see if password is given. Currently assuming password is always given
	char *pass_option = *argv++; //argv = 3
	char pass_opt[] = "-p\0";
	char *password;
        char *archive_name; // argv 4 -> 5
        
        SHA256_CTX ctx;
        BYTE hash_pass[SHA256_BLOCK_SIZE];
	// If password option is given
	if(strcmp(pass_option, pass_opt) == 0) {
		password = *argv++; // argv 3 -> 4
		checkPassReq(password);
                archive_name = *argv++;

                sha256_init(&ctx);
                for (int i = 0; i < HASH_ITR; ++i) {
                        sha256_update(&ctx, password, strlen(password));
                }
                sha256_final(&ctx, hash_pass);

	} else {
		//password = malloc(32);
                //memcpy(password, getpass("Please enter password"),32);
                password = getpass("Please enter password"),
                //printf("entered pass: %s\n", password);
		checkPassReq(password);
                //printf("passed checkpassReq\n");
                archive_name = pass_option;
                //password[31] = 0;
                sha256_init(&ctx);
                for (int i = 0; i < HASH_ITR; ++i) {
                        sha256_update(&ctx, password, strlen(password));
                }
                sha256_final(&ctx, hash_pass);
                free(password);
 
	}
	// Verify if archive file exists. If it does, check pass. Otherwise create
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
        char nameExt[] = "-list.txt";
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

                                                addArchiveHMAC_Space(archive_fp);

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
                                addFileSize = ftell(newFile_fp);
                                fseek(newFile_fp, 0, SEEK_SET);
        
                                struct stat tempstat;
                                printf("ftell size: %ld\n", addFileSize);
                                stat(newFile_name, &tempstat);
                                printf("stat size: %ld\n", tempstat.st_size);

                                long roundup = 0;
                                while (roundup < addFileSize) {
                                        roundup += 16;
                                }
 
                                char fileName[128];
                                memset(fileName, 0, 128);
                                int nameLength = strlen(newFile_name);
                                strcpy(fileName, newFile_name);
                                fileName[nameLength] = 0;

        
                                if (newArchive) {
                                        fseek(archive_fp, 0, SEEK_END);
                                        addFile(newFile_fp, archive_fp, newList_fp, hash_pass, nameLength, fileName, addFileSize);                                       
                                } else {
                                        fseek(archive_fp, 0, SEEK_END);
                                        long archiveSize = ftell(archive_fp) - ARCHIVE_HMAC_BLOCKSIZE; 
                                        fseek(archive_fp, 0, SEEK_SET);
                         
                                        if (checkArchiveHMAC(archive_fp, hash_pass, archiveSize) != 1) {
                                                printf("Archive has been tampered with. Aborting process\n");
                                                goto func_end;
                                        }
                                       
                                        fseek(archive_fp, ARCHIVE_HMAC_BLOCKSIZE, SEEK_SET);
                                        if (checkFileHMAC(archive_fp, hash_pass) != 1) {
                                                printf("A file has been tampered with. Aborting process\n");
                                                goto func_end;
                                        }

                                        fseek(archive_fp, ARCHIVE_HMAC_BLOCKSIZE, SEEK_SET);
                                        if (keyIsValid(archive_fp, hash_pass)) {
                                                fseek(archive_fp, 0, SEEK_END);
                                                addFile(newFile_fp, archive_fp, newList_fp, hash_pass, nameLength, fileName, addFileSize); 
                                        } else {
                                                printf("Wrong password\n");
                                                fclose(newFile_fp);
                                                goto func_end;
                                
                                        }
                                }
                                fseek(archive_fp, 0, SEEK_END);
                                long archiveSize = ftell(archive_fp) - ARCHIVE_HMAC_BLOCKSIZE; 
                                fseek(archive_fp, 0, SEEK_SET);
                                
                                //printf("***Updated archive size %ld***\n", archiveSize);
                                
                                passCount++;
                                newArchive = 0;
                                fseek(archive_fp, 0, SEEK_SET);
                                updateArchiveHMAC(archive_fp, hash_pass, archiveSize);
                                
	                } else {
                               
                               printf("%s does not exist\n", newFile_name); 
                               continue;
	                }
                } else if ((strcmp(func_name, "extract")) == 0) {
                        if (access(archive_name, F_OK) == 0) {
                                
                                if (passCount == 0) {
                                        archive_fp = fopen(archive_name, "rb+");
                                        if (archive_fp == NULL) {
                                                printf("open failed\n");
                                                goto func_extract_end;
                                        }

                                        fseek(archive_fp, 0, SEEK_END);
                                        archiveFileSize = ftell(archive_fp) - ARCHIVE_HMAC_BLOCKSIZE;
                                        fseek(archive_fp, 0, SEEK_SET);
                                }

                                fseek(archive_fp, 0, SEEK_SET);
                                if (checkArchiveHMAC(archive_fp, hash_pass, archiveFileSize) != 1) {
                                        printf("Archive has been tampered with. Aborting process\n");
                                        goto func_extract_end;
                                }

                                fseek(archive_fp, ARCHIVE_HMAC_BLOCKSIZE, SEEK_SET);
                                if(checkFileHMAC(archive_fp, hash_pass) != 1) {
                                       printf("An archived file has been tampered with. Aborting process\n");   
                                       goto func_extract_end;
                                }

                                fseek(archive_fp, ARCHIVE_HMAC_BLOCKSIZE, SEEK_SET);

                                if (keyIsValid(archive_fp, hash_pass)) {
                                        fseek(archive_fp, ARCHIVE_HMAC_BLOCKSIZE, SEEK_SET);
                                        long offset = findInArchive(archive_fp, hash_pass, newFile_name, archiveFileSize);

                                        if (offset < 0) {
                                                printf("%s not in archive\n", newFile_name);
                                                passCount++;
                                                continue;
                                        }
                                        extractFile(archive_fp, hash_pass, newFile_name); 
 
                                } else {
                                        printf("Wrong password\n");
                                        goto func_extract_end;                               
                                }        
                                passCount++;

                                fseek(archive_fp, 0, SEEK_SET);
                                //updateArchiveHMAC(archive_fp);
	                } else {
                                die("Specified archive file does not exist");
                                
	                }

                } else if ((strcmp(func_name, "delete")) == 0) {
                        if (access(archive_name, F_OK) == 0) {
                                
                                if (passCount == 0) {
                                        archive_fp = fopen(archive_name, "r+b");
                                        if (archive_fp == NULL) {
                                                printf("open failed\n");
                                                goto func_end;
                                        }
                                        
                                        newList_fp = fopen(listName, "rb+");
                                        if(newList_fp == NULL) {
                                                die("fopen failed in main add1");
                                        }

                                        fseek(archive_fp, 0, SEEK_END);
                                        archiveFileSize = ftell(archive_fp) - ARCHIVE_HMAC_BLOCKSIZE;
                                        fseek(archive_fp, 0, SEEK_SET);
                                }
                                // can cancel once you implment hmac for archive
                                fseek(archive_fp, 0, SEEK_SET);
                                if (checkArchiveHMAC(archive_fp, hash_pass, archiveFileSize) != 1) {
                                        printf("Archive has been tampered with. Aborting process\n");
                                        goto func_end;
                                }                               
 
                                fseek(archive_fp, ARCHIVE_HMAC_BLOCKSIZE, SEEK_SET);
                                if(checkFileHMAC(archive_fp, hash_pass) != 1) {
                                       printf("A file has been tampered with. Aborting process\n");   
                                       goto func_end;
                                }

                                fseek(archive_fp, ARCHIVE_HMAC_BLOCKSIZE, SEEK_SET);

                                if (keyIsValid(archive_fp, hash_pass)) {
                                        fseek(archive_fp, ARCHIVE_HMAC_BLOCKSIZE, SEEK_SET);
                                        long offset = findInArchive(archive_fp, hash_pass, newFile_name, archiveFileSize);

                                        if (offset < 0) {
                                                printf("File not in archive\n");
                                                passCount++;
                                                continue;
                                        }

                                        int error = deleteFile(archive_fp, hash_pass, newList_fp, newFile_name); 

                                        if (error < 0) {
                                                printf("Error mismatch. Found in archive but not in List. List file may have been tampered with\n");
                                                goto func_end;
                                        }

                                        passCount++;

                                        fseek(archive_fp, 0, SEEK_SET);
                                        updateArchiveHMAC(archive_fp, hash_pass, archiveFileSize);
                                       
                                } else {
                                        printf("Wrong password\n");
                                        goto func_end;                               
                                }        

                       }
                } else {
                        
                        die("Please choose an appropriate function: list, add, extract, delete");
                        //goto func_end;
                }
        }
                

func_end: 
        if ((strcmp(func_name, "add")) == 0 || (strcmp(func_name, "delete")) == 0) 
                fclose(newList_fp);
func_extract_end:        
        fclose(archive_fp);
	return 0;
}
