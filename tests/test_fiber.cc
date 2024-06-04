#include "duan_src/duan.h"

duan::Logger::ptr g_logger = DUAN_LOG_ROOT();

void run_in_fiber(){
    DUAN_LOG_INFO(g_logger) << "run_in_fiber begin";
    duan::Fiber::YieldToHold();
    DUAN_LOG_INFO(g_logger) << "run_in_fiber end";
}

int main(int argc, char** argv){
    duan::Fiber::GetThis();
    DUAN_LOG_INFO(g_logger) << "main begin";
    duan::Fiber::ptr fiber(new duan::Fiber(run_in_fiber));
    fiber->swapIn();
    DUAN_LOG_INFO(g_logger) << "main after swapIn";
    fiber->swapIn();
    DUAN_LOG_INFO(g_logger) << "main after end";

    return 0;
}