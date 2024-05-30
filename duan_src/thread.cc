/*
实现日志模块
2024/5/29 15:30
by 六七
*/
#include "thread.h"
#include "log.h"
#include "util.h"

namespace duan{

// thread_local 线程存储期  线程周期有效
// 线程局部变量
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

static duan::Logger::ptr g_logger = DUAN_LOG_NAME("system");

// 获取当前线程的指针
Thread* Thread::GetThis(){
    return t_thread;
}

// 获取当前线程的名称
const std::string& Thread::GetName(){
    return t_thread_name;
}

// 设置当前线程名称
void Thread::SetName(const std::string& name){
    if(name.empty()){
        return;
    }
    if(t_thread){
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name)
    : m_cb(cb)
    , m_name(name){
    if(name.empty()){
        m_name = "UNKNOW";
    }
    // 第一个参数为指向线程标识符的指针，也就是线程对象的指针  第二个参数用来设置线程属性  第三个参数是线程运行函数的地址，通俗理解线程要执行函数（线程做的事情的）指针
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);                  // 创建成功时 返回0
    if(rt){
        DUAN_LOG_ERROR(g_logger) << "pthread_create thread fail, rt = " << rt << " name = " << name;
        throw std::logic_error("pthread_create error");
    }
}

Thread::~Thread(){
    if(m_thread){
        pthread_detach(m_thread);                                            // 线程主动与主控线程断开关系,直接自己自动释放
    }
}

void Thread::join(){
    if(m_thread){
        // pthread_join用来等待一个线程的结束 
        int rt = pthread_join(m_thread, nullptr);
        if(rt){
            DUAN_LOG_ERROR(g_logger) << "pthread_join thread fail, rt = " << rt << " name = " << m_name;
            throw std::logic_error("pthread_join error"); 
        }
        m_thread = 0;
    }
}

// 线程执行函数
void* Thread::run(void* arg){
    Thread* thread = (Thread*) arg;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = duan::GetThreadId();
    // 设置指定线程的线程名 thread
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(thread->m_cb);

    // 至此 线程就启动了
    cb();
    return 0;
}

} // end of namespace