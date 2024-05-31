/*
实现线程模块
2024/5/29 15:30
by 六七
*/
#ifndef __DUAN_THREAD_H__
#define __DUAN_THREAD_H__

#include <thread>
#include <functional>
#include <memory>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>


// C++11之前都是用pthread_xx库
// C++11之后 std::thread, 底层还是调用pthread
namespace duan{

class Semaphore{
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();
private:
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

private:
    sem_t m_semaphore;
};

template<class T>
struct ScopedLockImpl{
public:
    ScopedLockImpl(T& mutex)
        : m_mutex(mutex){
        m_mutex.lock();
        m_locked = true;            
    }

    ~ScopedLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

template<class T>
struct ReadScopedLockImpl{
public:
    ReadScopedLockImpl(T& mutex)
        : m_mutex(mutex){
        m_mutex.rdlock();
        m_locked = true;            
    }

    ~ReadScopedLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.rdunlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

template<class T>
struct WriteScopedLockImpl{
public:
    WriteScopedLockImpl(T& mutex)
        : m_mutex(mutex){
        m_mutex.wrlock();
        m_locked = true;            
    }

    ~WriteScopedLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

class Mutex{
public:
    typedef ScopedLockImpl<Mutex> Lock;
    Mutex(){
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~Mutex(){
        pthread_mutex_destroy(&m_mutex);
    }

    void lock(){
        pthread_mutex_lock(&m_mutex);
    }

    void unlock(){
        pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;    
};

// 空锁  用于测试
class NullMutex{
public:
    typedef ScopedLockImpl<NullMutex> Lock;
    NullMutex() {}
    ~NullMutex() {}
    void lock() {}
    void unlock() {}
};

class RWMutex{
public:
    typedef ReadScopedLockImpl<RWMutex> ReadLock;
    typedef WriteScopedLockImpl<RWMutex> WriteLock;

    RWMutex(){
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex(){
        pthread_rwlock_destroy(&m_lock);
    }

    // 加锁操作
    void rdlock(){
        pthread_rwlock_rdlock(&m_lock);
    }

    // 加锁操作
    void wrlock(){
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock(){
        pthread_rwlock_unlock(&m_lock);
    }
private:
    pthread_rwlock_t m_lock;
};

class NullRWMutex{
public:
    typedef ReadScopedLockImpl<NullMutex> ReadLock;
    typedef WriteScopedLockImpl<NullMutex> WriteLock;

    NullRWMutex() {}
    ~NullRWMutex() {}

    void rdlock() {}
    void wrlock() {}
    void unlock() {}
};

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

    Semaphore m_semaphore;                          // 使用一下信号量 sem_init设为0时  当前进程的所有线程共享此信号量
};

} // end of namespace

#endif