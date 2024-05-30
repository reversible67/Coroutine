/*
实现日志模块
2024/5/29 15:30
by 六七
*/
#ifndef __DUAN_THREAD_H__
#define __DUAN_THREAD_H__

#include <thread>
#include <functional>
#include <memory>
#include <pthread.h>

// C++11之前都是用pthread_xx库
// C++11之后 std::thread, 底层还是调用pthread
namespace duan{

class Thread{
public:
    typedef std::shared_ptr<Thread> ptr;                         // 还是尽量多使用智能指针 这边会更少出现内存泄漏的情况
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    pid_t getId() const { return m_id;}                          // 获取线程id
    const std::string& getName() const { return m_name;}         // 获取线程名称

    void join();
    static Thread* GetThis();
    static const std::string& GetName();
    static void SetName(const std::string& name);                // 给线程设置名称
private:
    // C++11新特性 delete   这样就不会做拷贝了
    Thread(const Thread&) = delete;                 // 禁止拷贝构造函数
    Thread(const Thread&&) = delete;                // 右值引用
    Thread& operator=(const Thread&) = delete;      // 禁止赋值运算符

    static void* run(void* arg);
private:
    pid_t m_id = -1;                                // 线程id
    pthread_t m_thread = 0;                         // 线程结构
    std::function<void()> m_cb;                     // 线程执行函数
    std::string m_name;                             // 线程名称
};

} // end of namespace

#endif