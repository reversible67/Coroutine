/*
协程调度模块
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

class Scheduler{
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    // 默认线程池线程数量为1  true代表纳入协程调度器
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();                           // 调度器是个虚拟的基类  后续加相应的功能 实现不同的子类

    const std::string& getName() const { return m_name;}

    static Scheduler* GetThis();                    // 主线程
    static Fiber* GetMainFiber();                   // 主协程

    void start();
    void stop();

    template<class FiberOrcb>
    void schedule(FiberOrcb fc, int thread = -1){
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }
        if(need_tickle){
            tickle();
        }
    }

    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end){
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
    virtual void tickle();
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
    std::string m_name;

};

} // end of duan

#endif