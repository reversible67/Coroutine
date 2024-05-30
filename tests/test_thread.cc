/*
实现日志模块
2024/5/30 11:26
by 六七
*/
#include "duan_src/duan.h"

duan::Logger::ptr g_logger = DUAN_LOG_ROOT();

volatile int count = 0;                          // 加上volatile关键字  防止编译器做优化
duan::RWMutex s_mutex;

void fun1(){
    DUAN_LOG_INFO(g_logger) << "name: " << duan::Thread::GetName() 
                            << " this.name: " << duan::Thread::GetThis()->getName() 
                            << " id: " << duan::GetThreadId()
                            << " this.id: " << duan::Thread::GetThis()->getId();

    for(int i = 0; i < 100000; ++i){
        // 对象是lock
        duan::RWMutex::WriteLock lock(s_mutex);
        ++count;
    }
}

void func2(){

}

int main(int argc, char** argv){
    DUAN_LOG_INFO(g_logger) << "thread test begin";
    std::vector<duan::Thread::ptr> thrs;
    for(int i = 0; i < 5; ++i){
        duan::Thread::ptr thr(new duan::Thread(fun1, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }

    // 等待线程的结束
    for(int i = 0; i < 5; ++i){
        thrs[i]->join();
    }
    DUAN_LOG_INFO(g_logger) << "thread test begin";
    DUAN_LOG_INFO(g_logger) << "count = " << count;
    return 0;
}