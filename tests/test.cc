/*
实现日志模块
2024/5/16 10:30
by 六七
*/
#include <iostream>
#include <thread>
#include "../duan_src/log.h"
#include "../duan_src/util.h"

// 所以duan都是命名空间 namespace
int main(int argc, char** argv){
    duan::Logger::ptr logger(new duan::Logger);
    logger->addAppender(duan::LogAppender::ptr(new duan::StdoutLogAppender)); 

    // duan::LogEvent::ptr event(new duan::LogEvent(__FILE__, __LINE__, 0, duan::GetThreadId(), duan::GetFiberId(), time(0)));
    // event->getSS() << "hello duan log";

    // logger->log(duan::LogLevel::DEBUG, event);   
    std::cout << "hello duan log" << std::endl;

    DUAN_LOG_INFO(logger) << "test macro";
    DUAN_LOG_DEBUG(logger) << "test macro debug";

    return 0;
}