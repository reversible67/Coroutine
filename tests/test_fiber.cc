/*
协程模块测试
2024/6/4 20:45
by 六七
*/
#include "duan_src/duan.h"

duan::Logger::ptr g_logger = DUAN_LOG_ROOT();

void run_in_fiber(){
    DUAN_LOG_INFO(g_logger) << "run_in_fiber begin";
    duan::Fiber::YieldToHold();
    DUAN_LOG_INFO(g_logger) << "run_in_fiber end";
    duan::Fiber::YieldToHold();
}

void test_fiber(){
    DUAN_LOG_INFO(g_logger) << "main begin -1";
    {
        duan::Fiber::GetThis();
        DUAN_LOG_INFO(g_logger) << "main begin";
        duan::Fiber::ptr fiber(new duan::Fiber(run_in_fiber));
        fiber->swapIn();
        DUAN_LOG_INFO(g_logger) << "main after swapIn";
        fiber->swapIn();
        DUAN_LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }
    DUAN_LOG_INFO(g_logger) << "main after end2";
}

int main(int argc, char** argv){
    duan::Thread::SetName("main_thread");

    std::vector<duan::Thread::ptr> thrs;
    for(int i = 0; i < 3; ++i){
        thrs.push_back(duan::Thread::ptr(
                    new duan::Thread(&test_fiber, "name_" + std::to_string(i))));
    }
    for(auto i : thrs){
        i->join();
    }
    return 0;
}