/*
协程调度器封装
2024/6/5 16:50
by 六七
*/
#include "scheduler.h"
#include "log.h"
#include "macro.h"
#include "fiber.h"

namespace duan{

static duan::Logger::ptr g_logger = DUAN_LOG_NAME("system");

static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name)
    : m_name(name){
    DUAN_ASSERT(threads > 0);

    if(use_caller){
        duan::Fiber::GetThis();                 // 没有协程的时候  会初始化一个主协程  用来调度其它协程
        --threads;                              // 此时线程作为了主协程 threads -1 故在此之前 需要判断threads > 0

        DUAN_ASSERT(GetThis() == nullptr);      // 此时这个线程调度器里面 应当是没有调度器的 一山不容二虎
        t_scheduler = this;

        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this)));             // reset的新协程才是真正执行方法的协程
        duan::Thread::SetName(m_name);          // 属于哪个线程
        t_fiber = m_rootFiber.get();
        m_rootThread = duan::GetFiberId();
        m_threadIds.push_back(m_rootThread);    // 放到线程id容器中
    }
    else{
        m_rootThread = -1;
    }
    m_threadCount = threads;
}

Scheduler::~Scheduler(){                           
    DUAN_ASSERT(m_stopping);
    if(GetThis() == this){
        t_scheduler = nullptr;
    }
}

Scheduler* Scheduler::GetThis(){
    return t_scheduler;
}

Fiber* Scheduler::GetMainFiber(){
    return t_fiber;
}

void Scheduler::start(){
    MutexType::Lock lock(m_mutex);
    if(!m_stopping){
        return;                         // 启动
    }
    m_stopping = false;                 // 已关闭
    DUAN_ASSERT(m_threads.empty());

    m_threads.resize(m_threadCount);    // 重置线程池的大小
    for(size_t i = 0; i < m_threadCount; ++i){
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i)));     // 从哪个线程池来的
        m_threadIds.push_back(m_threads[i]->getId());
    }
    lock.unlock();

    // if(m_rootFiber){
    //     // m_rootFiber->swapIn();
    //     m_rootFiber->call();
    //     DUAN_LOG_INFO(g_logger) << "call out" <<  m_rootFiber->getState();
    // }
}

/*
我现在学这里的状态就是  懵懵懵！
从测试代码找思路~
*/
void Scheduler::stop(){
    m_autoStop = true;
    if(m_rootFiber && m_threadCount == 0 && (m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::INIT)){
        DUAN_LOG_INFO(g_logger) << this << "stopped";
        m_stopping = true;
        if(stopping()){
            return;
        }
    }
    // bool exit_on_this_fiber = false;
    if(m_rootThread != -1){
        DUAN_ASSERT(GetThis() == this);
    }
    else{
        DUAN_ASSERT(GetThis() != this);
    }
    m_stopping = true;
    for(size_t i = 0; i < m_threadCount; ++i){
        tickle();                                                        // 唤醒线程
    }

    if(m_rootFiber){
        tickle();
    }

    // 避免有的任务没有做完
    if(m_rootFiber){
        // while(!stopping()){
        //     // 当主协程结束的时候
        //     if(m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::EXCEPT){
        //         m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        //         DUAN_LOG_INFO(g_logger) << "root fiber is term, reset";
        //         t_fiber = m_rootFiber.get();
        //     }
        //     m_rootFiber->call();                                        // 再次执行  回收没有执行完的任务
        // }
        if(!stopping()){
            m_rootFiber->call();
        }
    }

    std::vector<Thread::ptr> thrs;
    {
        MutexType::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }

    for(auto& i : thrs){
        i->join();        // 释放线程
    }

    // if(stopping()){
    //     return;
    // }

    // if(exit_on_this_fiber){
    // }
}

void Scheduler::setThis(){
    t_scheduler = this;
}

// 新创建的线程 在run里启动
// 线程调度主函数
void Scheduler::run(){
    DUAN_LOG_INFO(g_logger) << "run";
    // return;
    setThis();

    // 设置
    if(duan::GetThreadId() != m_rootThread){
        t_fiber = Fiber::GetThis().get();
    }
    
    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::ptr cb_fiber;

    FiberAndThread ft;
    while(true){   
        ft.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            // 去协程的消息队列 取一个协程/函数(任务)(消息)
            MutexType::Lock lock(m_mutex);
            auto it = m_fibers.begin();
            while(it != m_fibers.end()){
                // 这个任务已经分配好了  无需处理
                // 当前线程id不是我所期望的 就跳过
                if(it->thread != -1 && it->thread != duan::GetThreadId()){
                    ++it;
                    // 通知其它线程  去做
                    tickle_me = true;                  // 我这个线程不做处理
                    continue;
                }

                DUAN_ASSERT(it->fiber || it->cb);      // 至少要有一个
                if(it->fiber && it->fiber->getState() == Fiber::EXEC){
                    // 正在执行状态  所以也不需要处理
                    ++it;
                    continue;
                }
                // 然后需要处理
                ft = *it;
                // tickle_me = true;
                m_fibers.erase(it);
                ++m_activeThreadCount;
                is_active = true;
                break;
            }
        }
        if(tickle_me){
            tickle();         // 唤醒其他线程
        }

        if(ft.fiber && (ft.fiber->getState() != Fiber::TERM || ft.fiber->getState() != Fiber::EXCEPT)){
            ++m_activeThreadCount;     // 活跃
            ft.fiber->swapIn();
            --m_activeThreadCount;

            if(ft.fiber->getState() == Fiber::READY){
                scheduler(ft.fiber);      // 继续处理
            }
            else if(ft.fiber->getState() != Fiber::TERM && ft.fiber->getState() != Fiber::EXCEPT){
                ft.fiber->m_state = Fiber::HOLD;
            }
            ft.reset();      // 释放指针指向的空间
        }
        else if(ft.cb){
            if(cb_fiber){
                cb_fiber->reset(ft.cb);
            }
            else{
                cb_fiber.reset(new Fiber(ft.cb));
            }
            ft.reset();
            cb_fiber->swapIn();
            --m_activeThreadCount;
            if(cb_fiber->getState() == Fiber::READY){
                scheduler(cb_fiber);
                cb_fiber.reset();
            }
            else if(cb_fiber->getState() == Fiber::EXCEPT || cb_fiber->getState() == Fiber::TERM){
                cb_fiber->reset(nullptr);
            }
            else { // if(cb_fiber->getState() != Fiber::TERM){
                cb_fiber->m_state = Fiber::HOLD;
                cb_fiber.reset();
            }
        }
        else{
            if(is_active){
                --m_activeThreadCount;
                continue;
            }
            // 没有任务可以做了
            if(idle_fiber->getState() == Fiber::TERM){
                DUAN_LOG_INFO(g_logger) << "idle fiber term";
                break;
            }

            // 空闲线程数
            ++m_idleThreadCount;
            idle_fiber->swapIn();
            --m_idleThreadCount;
            if(idle_fiber->getState() != Fiber::TERM && idle_fiber->getState() != Fiber::EXCEPT){
                idle_fiber->m_state = Fiber::HOLD;
            }
        }
    }
}

void Scheduler::tickle(){
    DUAN_LOG_INFO(g_logger) << "tickle";
}

bool Scheduler::stopping(){
    MutexType::Lock lock(m_mutex);
    // 是否自动停止 且 正在停止 且 m_fibers为空 且 活跃线程为0  才是真正意义上的停止
    return m_autoStop && m_stopping && m_fibers.empty() && m_activeThreadCount == 0;
}

void Scheduler::idle(){
    DUAN_LOG_INFO(g_logger) << "idle";
    while(!stopping()){
        duan::Fiber::YieldToHold();             // 让出执行权
    }
}

} // end of namespace