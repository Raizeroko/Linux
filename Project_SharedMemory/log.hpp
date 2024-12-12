#pragma once

#include <iostream>
#include <time.h>
#include <string>
#include <cstdarg>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define SIZE 1024

#define Info 0
#define Debug 1
#define Warning 2
#define Error 3
#define Fatal 4

#define Screen 1
#define Onefile 2
#define Class 3

#define Filename "log.txt"
using namespace std;

class Log{
public:

    Log()
    {
        printMethod = Screen;
        path = "./log/";
    }
    void Enable(int method)
    {
        printMethod = method;
    }


    string levelToString(int level)
    {
        switch(level)
        {
            case Info: return "Info";
            case Debug: return "Debug";
            case Warning: return "Warning";
            case Error: return "Error";
            case Fatal: return "Fatal";
            default : return "None";

        }
    }

    // void logmessage(int level, const char *format, ...)
    // {
    //     char leftBuffer[SIZE];
    //     time_t t = time(nullptr);
    //     struct tm *ctime = localtime(&t);
    //     snprintf(leftBuffer, sizeof(leftBuffer), "[%s][%d-%d-%d %d:%d:%d]", levelToString(level).c_str(), \
    //             1900+ctime->tm_year, 1+ctime->tm_mon, ctime->tm_mday, ctime->tm_hour, ctime->tm_min, ctime->tm_sec);
        
    //     va_list s;
    //     va_start(s, format);
    //     char rightBuffer[SIZE];

    //     vsnprintf(rightBuffer, sizeof(rightBuffer), format, s);
    //     va_end(s);

    //     char logtxt[SIZE*2];
    //     int written = snprintf(logtxt, sizeof(logtxt), "%s %s\n", leftBuffer, rightBuffer);
    //     if (written >= sizeof(logtxt)) {
    //         fprintf(stderr, "Log message truncated: %d characters written, buffer size is %ld\n", written, sizeof(logtxt));
    //     }

    //     printLog(level, logtxt);


    // }

    void printOneFile(const string& filename, string logtxt){
        string filepath = path + filename;
        int fd = open(filepath.c_str(), O_RDWR|O_CREAT|O_APPEND, 0664);
        if(fd < 0) return ;
        write(fd, logtxt.c_str(), logtxt.size());
        close(fd);
    }

    void printClassFile(int level, string logtxt){
        string filename = Filename;
        filename += ".";
        filename += levelToString(level);
        printOneFile(filename, logtxt);
    }

    void printLog(int level, string logtxt){
        switch(printMethod)
        {
            case Screen:   
                cout << logtxt << endl; break;
            case Onefile:
                printOneFile(Filename, logtxt);   break;
            case Class:
                printClassFile(level, logtxt); break;
            default:    break;
        }
    }

    void operator()(int level, const char *format, ...)
    {
        char leftBuffer[SIZE];
        time_t t = time(nullptr);
        struct tm *ctime = localtime(&t);
        snprintf(leftBuffer, sizeof(leftBuffer), "[%s][%d-%d-%d %d:%d:%d]", levelToString(level).c_str(), \
                1900+ctime->tm_year, 1+ctime->tm_mon, ctime->tm_mday, ctime->tm_hour, ctime->tm_min, ctime->tm_sec);
        
        va_list s;
        va_start(s, format);
        char rightBuffer[SIZE];

        vsnprintf(rightBuffer, sizeof(rightBuffer), format, s);
        va_end(s);

        char logtxt[SIZE*2];
        int written = snprintf(logtxt, sizeof(logtxt), "%s %s\n", leftBuffer, rightBuffer);
        if (written >= sizeof(logtxt)) {
            fprintf(stderr, "Log message truncated: %d characters written, buffer size is %ld\n", written, sizeof(logtxt));
        }

        printLog(level, logtxt);


    }
private:
    int printMethod;
    string path;
};

