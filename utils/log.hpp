#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <cstdarg>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define InfoLength 1024

namespace fs = std::filesystem;

enum
{
    SCREEN = 1,
    DOCUMENT
};

enum
{
    Info = 0,
    Debug,
    Warning,
    Error,
    Fatal
};

class Log
{
public:
    Log(int printMethod = SCREEN, std::string path = "./log")
        : _printMethod(printMethod), _path(path)
    {
        if (_printMethod == DOCUMENT)
        {
            // 创建文件夹
            if (!fs::exists(_path))
            {
                if (fs::create_directory(_path))
                {
                    std::cout << "Log directory created successfully." << std::endl;
                }
                else
                {
                    std::cerr << "Failed to create log directory." << std::endl;
                }
            }
            else
            {
                std::cout << "Log directory already exists." << std::endl;
            }            
        }
    }

    void operator()(int type, const char *info, ...)
    {
        std::string info_type = typeToSting(type);
        std::string time_info = getTime();

        va_list args;
        va_start(args, info);
        char info_detail[InfoLength];
        int format_print_length = vsnprintf(info_detail, sizeof(info_detail), info, args);
        if (format_print_length < 0)
        {
            std::cout << "Error formatting the string." << std::endl;
        }
        else if (format_print_length >= InfoLength)
        {
            std::cout << "Warning: Output truncated." << std::endl;
        }

        std::string format_string = formatString(info_type, time_info, info_detail);
        if(_printMethod == DOCUMENT){
            printWithFile(info_type);
        }
        std::cout << format_string << std::endl;
    }

private:
    void printWithFile(std::string info_type)
    {
        std::string file_name(_path);
        file_name += "/";
        file_name += "log.txt.";
        file_name += info_type;
        int fd = open(file_name.c_str(), O_RDWR | O_APPEND | O_CREAT, 0664);
        if(fd == -1){
            perror("Open error");
            exit(-1);
        }
        int dup_ret = dup2(fd, 1);
        if(dup_ret == -1){
            perror("Redirection error");
            exit(-1);
        }
    }


    std::string formatString(std::string info_type, std::string time_info, const char *info_detail)
    {
        std::string format_string;
        format_string += "[";
        format_string += info_type;
        format_string += "]";
        format_string += "[";
        format_string += time_info;
        format_string += "] ";
        format_string += info_detail;
        return format_string;
    }

    std::string getTime()
    {
        std::time_t now = std::time(nullptr);

        std::tm *local_time = std::localtime(&now);

        // 输出格式化时间
        std::string time_info;
        time_info += std::to_string(local_time->tm_year + 1900);
        time_info += '-';
        time_info += std::to_string(local_time->tm_mon + 1);
        time_info += '-';
        time_info += std::to_string(local_time->tm_mday);
        time_info += ' ';
        time_info += std::to_string(local_time->tm_hour);
        time_info += ':';
        time_info += std::to_string(local_time->tm_min);
        time_info += ':';
        time_info += std::to_string(local_time->tm_sec);
        return time_info;
    }

    std::string typeToSting(int info_type)
    {
        std::string string_type;
        switch (info_type)
        {
        case Info:
            return std::string("Info");
        case Debug:
            return std::string("Debug");
        case Warning:
            return std::string("Warning");
        case Error:
            return std::string("Error");
        case Fatal:
            return std::string("Fatal");
        default:
            std::cout << "Without info type" << std::endl;
            exit(0);
        }
    }

private:
    int _printMethod;
    std::string _path;
};