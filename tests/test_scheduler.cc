/*
调度器测试
2024/6/11 10:12
by 六七
*/
#include "duan_src/duan.h"

duan::Logger::ptr g_logger = DUAN_LOG_ROOT();

void test_fiber(){
    DUAN_LOG_INFO(g_logger) << "test in fiber";
}

int main(int argc, char** argv){
    DUAN_LOG_INFO(g_logger) << "main";
    duan::Scheduler sc;
    sc.scheduler(&test_fiber);
    sc.start();
    DUAN_LOG_INFO(g_logger) << "scheduler";
    sc.scheduler(&test_fiber);
    sc.stop();
    DUAN_LOG_INFO(g_logger) << "over";

    return 0;
}