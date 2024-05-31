/*
线程模块测试
2024/5/30 11:26
by 六七
*/
#include "duan_src/duan.h"

duan::Logger::ptr g_logger = DUAN_LOG_ROOT();

volatile int count = 0;                          // 加上volatile关键字  防止编译器做优化
// duan::RWMutex s_mutex;
duan::Mutex s_mutex;

void fun1(){
    DUAN_LOG_INFO(g_logger) << "name: " << duan::Thread::GetName() 
                            << " this.name: " << duan::Thread::GetThis()->getName() 
                            << " id: " << duan::GetThreadId()
                            << " this.id: " << duan::Thread::GetThis()->getId();

    for(int i = 0; i < 100000; ++i){
        // 对象是lock
        // duan::RWMutex::WriteLock lock(s_mutex);
        duan::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void fun2(){
    while(true){
        DUAN_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void fun3(){
    while(true){
        DUAN_LOG_INFO(g_logger) << "==================================";
    }
}

int main(int argc, char** argv){
    DUAN_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("../bin/conf/log2.yml");
    duan::Config::LoadFromYaml(root);

    
    std::vector<duan::Thread::ptr> thrs;
    for(int i = 0; i < 2; ++i){
        duan::Thread::ptr thr(new duan::Thread(&fun1, "name_" + std::to_string(i * 2)));
        duan::Thread::ptr thr2(new duan::Thread(&fun2, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr);
        thrs.push_back(thr2);
    }

    // 等待线程的结束
    for(size_t i = 0; i < thrs.size(); ++i){
        thrs[i]->join();
    }
    DUAN_LOG_INFO(g_logger) << "thread test begin";
    DUAN_LOG_INFO(g_logger) << "count = " << count;
    return 0;
}