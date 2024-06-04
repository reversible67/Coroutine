/*
实现协程模块
2024/6/3 15:57
by 六七
*/
#ifndef __DUAN_FIBER_H__
#define __DUAN_FIBER_H__

#include <memory>
#include <functional>
#include <ucontext.h>
#include "thread.h"

namespace duan{

class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State{
        // 初始化状态
        INIT,
        // 暂停状态
        HOLD,
        // 执行中状态
        EXEC,
        // 结束状态
        TERM,
        // 可执行状态
        READY,
        // 异常状态
        EXCEPT
    };
private:
    // 无参构造函数  每个线程第一个协程的构造
    Fiber();
public:
    /*
    构造函数
    cb 协程执行的函数
    stacksize 协程栈大小
    */
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();

    // 重置协程函数，并重置状态
    // INIT TERM
    void reset(std::function<void()> cb);
    // 切换到当前协程执行
    void swapIn();
    // 切换到后台执行
    void swapOut();

    uint64_t getId() const { return m_id;}
public:
    // 设置当前协程
    static void SetThis(Fiber* f);
    // 返回当前协程
    static Fiber::ptr GetThis();
    // 协程切换到后台，并且设置为Ready状态
    static void YieldToReady();
    // 协程切换到后台，并且设置为Hold状态
    static void YieldToHold();

    // 总协程数
    static uint64_t TotalFibers();

    // 协程执行函数 执行完返回到线程主协程
    static void MainFunc();
    static uint64_t GetFiberId();
private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;

    ucontext_t m_ctx;
    void* m_stack = nullptr;

    std::function<void()> m_cb;
};

} // end of namespace

#endif