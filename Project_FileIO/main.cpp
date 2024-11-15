#include "Mystdio.h"
#include <string.h>


int main()
{
    MFILE *fp = mfopen("test.txt", "w");
    if(fp == nullptr) return 1;

    const char *msg1 = "My stdio1";
    mfwrite(fp, msg1, strlen(msg1));
    sleep(5);
    const char *msg2 = "My stdio2";
    mfwrite(fp, msg2, strlen(msg2));

    sleep(5);
    const char *msg3 = "My stdio3\n";
    mfwrite(fp, msg3, strlen(msg3));
    while(1){
        sleep(1);
        mfwrite(fp, msg3, strlen(msg3));
    }

    mfclose(fp);
    return 0;

}