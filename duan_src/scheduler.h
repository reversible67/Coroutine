/*
协程调度器封装
2024/6/5 16:50
by 六七
*/
#ifndef __DUAN_SCHEDULER_H__
#define __DUAN_SCHEDULER_H__

#include <memory>
#include "fiber.h"
#include "thread.h"
#include <vector>
#include <list>
#include <string>
#include <functional>

namespace duan{

/*
协程调度器
封装的是N-M的协程调度器
内部有一个线程池，支持协程在线程池里面切换
*/
class Scheduler{
public:
    typedef std::shared_ptr<Scheduler> ptr;         // 自己的智能指针
    typedef Mutex MutexType;                        // 锁

    // 默认线程池线程数量为1  true代表纳入协程调度器
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();                           // 调度器是个虚拟的基类  后续加相应的功能 实现不同的子类

    // 返回协程调度器名称
    const std::string& getName() const { return m_name;}

    static Scheduler* GetThis();                    // 返回当前协程调度器
    static Fiber* GetMainFiber();                   // 返回当前协程调度器的调度协程

    void start();                                   // 启动协程调度器
    void stop();                                    // 停止协程调度器

    // 调度协程  fc：协程或函数  thread：协程执行的线程id
    template<class FiberOrcb>
    void scheduler(FiberOrcb fc, int thread = -1){
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }
        if(need_tickle){
            tickle();
        }
    }

    // 批量调度协程  begin：协程数组的开始  end：协程数组的结束
    template<class InputIterator>
    void scheduler(InputIterator begin, InputIterator end){
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            while(begin != end){
                need_tickle = scheduleNoLock(&*begin) || need_tickle;
            }
        }
        if(need_tickle){
            tickle();
        }
    }
protected:
    // 通知协程调度器有任务了
    virtual void tickle();
    // 协程调度函数
    void run();
    // 让子类有清理任务的机会 所以是虚函数
    virtual bool stopping();
    // 既没有任务做 又不能让线程终止
    virtual void idle();

    void setThis();
private:
    template<class FiberOrcb>
    bool scheduleNoLock(FiberOrcb fc, int thread = -1){
        bool need_tickle = m_fibers.empty();
        FiberAndThread ft(fc, thread);
        if(ft.fiber || ft.cb){
            m_fibers.push_back(ft);
        }
        return need_tickle;
    }
private:
    struct FiberAndThread{
        Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;

        FiberAndThread(Fiber::ptr f, int thr)
            : fiber(f), thread(thr) {}

        // 智能指针的智能指针版本 主要是为了swap
        FiberAndThread(Fiber::ptr* f, int thr)
            : thread(thr) {
                fiber.swap(*f);
        }

        FiberAndThread(std::function<void()> f, int thr)
            : cb(f), thread(thr){
        }

        FiberAndThread(std::function<void()>* f, int thr)
            : thread(thr){
            cb.swap(*f);
        }

        FiberAndThread()
            : thread(-1){
        }

        void reset(){
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };
private:
    MutexType m_mutex;                              // 互斥量
    std::vector<Thread::ptr> m_threads;             // 线程池
    std::list<FiberAndThread> m_fibers;             // 即将要执行的协程或计划要执行的协程
    Fiber::ptr m_rootFiber;                         // 主协程
    std::string m_name;

protected:
    std::vector<int> m_threadIds;                    // 协程下的线程id数组
    size_t m_threadCount = 0;                        // 线程数量
    std::atomic<size_t> m_activeThreadCount = {0};                  // 工作线程数量
    std::atomic<size_t> m_idleThreadCount = {0};                    // 空闲线程数量
    bool m_stopping = true;                          // 是否正在停止
    bool m_autoStop = false;                         // 是否自动停止
    int m_rootThread = 0;                            // 主线程

};

} // end of duan

#endif