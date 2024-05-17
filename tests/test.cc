/*
实现日志模块
2024/5/16 10:30
by 六七
*/
#include <iostream>
#include <thread>
#include "../duan_src/log.h"
#include "../duan_src/util.h"

// 所有duan都是命名空间 namespace
int main(int argc, char** argv){
    // log格式
    duan::Logger::ptr logger(new duan::Logger);
    logger->addAppender(duan::LogAppender::ptr(new duan::StdoutLogAppender)); 

    duan::FileLogAppender::ptr file_appender(new duan::FileLogAppender("./log.txt"));
    // format格式
    duan::LogFormatter::ptr fmt(new duan::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);

    file_appender->setLevel(duan::LogLevel::ERROR);
    logger->addAppender(file_appender);

    // duan::LogEvent::ptr event(new duan::LogEvent(__FILE__, __LINE__, 0, duan::GetThreadId(), duan::GetFiberId(), time(0)));
    // event->getSS() << "hello duan log";

    // logger->log(duan::LogLevel::DEBUG, event);   
    std::cout << "hello duan log" << std::endl;

    DUAN_LOG_INFO(logger) << "test macro";
    DUAN_LOG_DEBUG(logger) << "test macro debug";

    DUAN_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    auto l = duan::LoggerMgr::GetInstance()->getLogger("xx");
    DUAN_LOG_INFO(l) << "xxx";

    return 0;
}