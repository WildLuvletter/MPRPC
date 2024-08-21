//
// Created by turing on 24-8-21.
//

#include "logger.h"

Logger &Logger::GetInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    std::thread writeLogTask([&]() {
        for (;;) {
            //获取当天日期,取日志信息,写入相应日志文件中
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);
            char file_name[128] = {0};
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);
            FILE *fp = fopen(file_name, "a+");
            if (fp == nullptr) {
                std::cout << file_name << "File open failed!" << std::endl;
                exit(EXIT_FAILURE);
            }
            std::string msg = m_lckQue.Pop();

            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d=>[%s] ", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec,
                    m_loglevel == INFO ? "info" : "error");
            msg.insert(0, time_buf);
            msg.append("\n");
            fputs(msg.c_str(), fp);
            fclose(fp);
        }
    });
    writeLogTask.detach();
}

void Logger::SetLogLevel(LogLevel level) {
    m_loglevel = level;
}

void Logger::Log(std::string msg) {
    m_lckQue.Push(msg);
}
