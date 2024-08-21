//
// Created by turing on 24-8-21.
//

#ifndef LOGGER_H
#define LOGGER_H

#include "bits/stdc++.h"
#include "lockqueue.h"

enum LogLevel {
    INFO, //普通日志信息
    ERROR, //错误信息
};

class Logger {
public:
    static Logger &GetInstance();

    //设置日志级别
    void SetLogLevel(LogLevel level);

    //写日志
    void Log(std::string msg);

private:
    Logger();

    Logger(const Logger &) = delete;

    Logger(Logger &&) = default;

    int m_loglevel; //记录日志级别
    LockQueue<std::string> m_lckQue;
};

#define LOG_INFO(logmsgformat,...)\
    do\
    {\
        Logger &logger=Logger::GetInstance();\
        logger.SetLogLevel(INFO);\
        char c[1024]={0};\
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);\
        logger.Log(c);\
    }while(0);

#define LOG_ERR(logmsgformat,...)\
    do\
    {\
        Logger &logger=Logger::GetInstance();\
        logger.SetLogLevel(ERROR);\
        char c[1024]={0};\
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);\
        logger.Log(c);\
    }while(0);

#endif //LOGGER_H
