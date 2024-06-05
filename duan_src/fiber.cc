/*
实现协程模块
2024/6/3 15:57
by 六七
*/
#include "fiber.h"
#include "config.h"
#include "macro.h"
#include <atomic>
#include "log.h"

namespace duan{

static Logger::ptr g_logger = DUAN_LOG_NAME("system");

// 原子类型
static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");

// 申请内存 释放内存
class MallocStackAllocator{
public:
    static void* Alloc(size_t size){
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size){
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

uint64_t Fiber::GetFiberId(){
    if(t_fiber){
        return t_fiber->getId();
    }
    return 0;
}

Fiber::Fiber(){
    m_state = EXEC;
    SetThis(this);

    if(getcontext(&m_ctx)){
        // 获取当前上下文
        DUAN_ASSERT2(false, "getcontext");
    }
    ++s_fiber_count;   

    DUAN_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}

// 这里是真正创建一个协程
Fiber::Fiber(std::function<void()> cb, size_t stacksize)
    : m_id(++s_fiber_id)
    , m_cb(cb){
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stacksize);
    if(getcontext(&m_ctx)){
        DUAN_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);

    DUAN_LOG_DEBUG(g_logger) << "Fiber::Fiber id = " << m_id;
}

Fiber::~Fiber(){
    --s_fiber_count;
    if(m_stack){
        DUAN_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
        StackAllocator::Dealloc(m_stack, m_stacksize);
    }
    // 主协程
    else{
        DUAN_ASSERT(!m_cb);
        DUAN_ASSERT(m_state == EXEC);
        
        Fiber* cur = t_fiber;
        if(cur == this){
            SetThis(nullptr);
        }
    }
    DUAN_LOG_DEBUG(g_logger) << "Fiber::~Fiber id = " << m_id;
}

void Fiber::reset(std::function<void()> cb){
    DUAN_ASSERT(m_stack);
    DUAN_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);

    m_cb = cb;
    if(getcontext(&m_ctx)){
        DUAN_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    // 修改上下文
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT; // 状态回到INIT
}
   
// 切换到当前协程执行
void Fiber::swapIn(){
    SetThis(this);
    DUAN_ASSERT(m_state != EXEC);
    m_state = EXEC;

    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)){
        DUAN_ASSERT2(false, "swapcontext");
    }
}

// 切换到后台执行
void Fiber::swapOut(){
    SetThis(t_threadFiber.get());

    if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)){
        DUAN_ASSERT2(false, "swapcontext");
    }
}

// 设置当前协程
void Fiber::SetThis(Fiber* f){
    t_fiber = f;
}

// 返回当前协程
Fiber::ptr Fiber::GetThis(){
    if(t_fiber){
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    DUAN_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

// 协程切换到后台，并且设置为Ready状态
void Fiber::YieldToReady(){
    Fiber::ptr cur = GetThis();
    cur->m_state = READY;
    cur->swapOut();
}

// 协程切换到后台，并且设置为Hold状态
void Fiber::YieldToHold(){
    Fiber::ptr cur  = GetThis();
    cur->m_state = HOLD;
    cur->swapOut();
}

// 总协程数
uint64_t Fiber::TotalFibers(){
    return s_fiber_count;
}

void Fiber::MainFunc(){
    Fiber::ptr cur = GetThis();
    DUAN_ASSERT(cur);
    try{
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }
    catch(std::exception& ex){
        cur->m_state = EXCEPT;
        DUAN_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what();
    }
    catch(...){
        cur->m_state = EXCEPT;
        DUAN_LOG_ERROR(g_logger) << "Fiber Except: ";
    }


    auto raw_ptr = cur.get();    // 裸指针拿出来
    // p.reset(q)会令智能指针p中存放指针q，即p指向q的空间，而且会释放原来的空间
    cur.reset();                // 指针指针数量-1
    raw_ptr->swapOut();

    // 永远无法抵达的真实  如果输出了 就有错
    DUAN_ASSERT2(false, "never reach");
}

} // end of duan