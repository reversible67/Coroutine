/*
调度器测试
2024/6/11 10:12
by 六七
*/
#include "duan_src/duan.h"

duan::Logger::ptr g_logger = DUAN_LOG_ROOT();

void test_fiber(){
    static int s_count = 5;
    DUAN_LOG_INFO(g_logger) << "test in fiber s_count = " << s_count;

    sleep(1);
    if(--s_count >= 0){
        duan::Scheduler::GetThis()->scheduler(&test_fiber, duan::GetThreadId());
    }
}

int main(int argc, char** argv){
    DUAN_LOG_INFO(g_logger) << "main";
    // duan::Scheduler sc;
    // sc.scheduler(&test_fiber);
    duan::Scheduler sc(3, false, "test");                 // 改为false 不使用当前线程
    sc.start();
    sleep(2);
    DUAN_LOG_INFO(g_logger) << "scheduler";
    sc.scheduler(&test_fiber);                            // 协程调度已经完成了 就算现在放任务进去 也没有意义
    sc.stop();
    DUAN_LOG_INFO(g_logger) << "over";

    return 0;
}