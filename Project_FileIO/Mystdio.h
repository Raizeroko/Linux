#ifndef __MYSTDIO_H__
#define __MYSTDI0_H__

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SIZE 32
#define F_BUFFER 0  //È«»º³å
#define L_BUFFER 1  //ÐÐ»º³å
#define N_BUFFER 2  //ÎÞ»º³å


typedef struct IO_FILE {
    int fileno;
    char* buffer;
    int b_pos;
    int flag;
}MFILE;


MFILE* mfopen(const char* dir, const char* flags);
int mfwrite(MFILE* fp, const char* msg, int len);
void mfclose(MFILE* fp);
int mfflush(MFILE* fp);

#endif