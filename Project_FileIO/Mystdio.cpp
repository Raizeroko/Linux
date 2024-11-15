#include "Mystdio.h"

MFILE* mfopen(const char *pathname, const char *mode)
{
    int flags = 0;
    if(strcmp(mode, "w") == 0){
        flags = (O_WRONLY|O_CREAT|O_TRUNC);
        printf("flags: %d\n", flags);
    }
    else if(strcmp(mode, "r") == 0){
        flags = O_RDONLY;
    }
    else if(strcmp(mode, "a") == 0){
        flags = (O_WRONLY|O_CREAT|O_APPEND);
    }
    else{
        perror("Error mode");
        return nullptr;
    }
    int fd = open(pathname, flags, 0666);
    if(fd < 0)
    {
        perror("open fail");
        return nullptr;
    }
    MFILE* stream = new MFILE;
    stream->fileno = fd;
    stream->buffer = new char[BUFFER_SIZE];
    stream->b_pos = 0;
    stream->flag = F_BUFFER;
    return stream;
}

int mfwrite(MFILE *fp, const char* msg, int len)
{
    int flag = 0;
    if(fp->flag == F_BUFFER){
        // 全缓冲
        if(fp->b_pos+len > BUFFER_SIZE){
            flag = 1;
        }
    }
    else if(fp->flag == L_BUFFER){
        // 行缓冲
        if(*(msg+len-1) == '\n'){
            flag = 1;
        }
    }
    else if(fp->flag == N_BUFFER){
        // 无缓冲
        flag = 1;
    }
    int ret = 0;
    if(flag){
        ret = mfflush(fp);
        if(ret<0){
            perror("write flush fail");
        }
        else{
            ret = write(fp->fileno, msg, len);
        }
    }
    else
    {
        memcpy(fp->buffer+fp->b_pos, msg, len);
        fp->b_pos += len;
    }

    if(ret < 0){
        perror("write fail");
        return -1;
    }
    return ret;
}

int mfflush(MFILE *fp){
    int ret = 0;
    if(fp->b_pos > 0){
        ret = write(fp->fileno, fp->buffer, fp->b_pos);
        printf("flush: %dret:%d\n",fp->b_pos, ret);
        fp->b_pos = 0;
    }
    return ret;
}


void mfclose(MFILE *fp)
{
    int ret = mfflush(fp);
    if(ret)
    close(fp->fileno);
    free(fp);
    fp = nullptr;
}