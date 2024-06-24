/*
IO调度模块
2024/6/12 11:37
by 六七
*/
#ifndef __DUAN_IOMANAGER_H__
#define __DUAN_IOMANAGER_H__

#include "scheduler.h"
#include "timer.h"

namespace duan{

class IOManager : public Scheduler, public TimerManager{
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    // 支持的事件
    enum Event{
        NONE   = 0x0,
        READ   = 0x1,                 // 读事件 EPOLLIN
        WRITE  = 0x4,                 // 写事件 EPOLLOUT
    };
private:
    struct FdContext{
        typedef Mutex MutexType;                       // 需要加锁
        struct EventContext{
            Scheduler* scheduler = nullptr;            // 在哪一个scheduler上面执行这个事件
            Fiber::ptr fiber;                          // 事件协程  即用哪一个协程去执行这个事件
            std::function<void()> cb;                  // 事件的回调函数
        };

        EventContext& getContext(Event event);         // 返回是读事件还是写事件
        void resetContext(EventContext& ctx);
        void triggerEvent(Event event);

        EventContext read;                             // 读事件
        EventContext write;                            // 写事件
        int fd = 0;                                    // 事件关联的句柄
        Event m_events = NONE;                         // 表示是读还是写 / 已注册的事件
        MutexType mutex;                               // 锁变量
    };
public:
    IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    ~IOManager();

    // 0 success, -1 error
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, Event event);
    bool cancelEvent(int fd, Event event);

    bool cancelAll(int fd);

    static IOManager* GetThis();

protected:
    void tickle() override;
    bool stopping() override;
    bool stopping(uint64_t& timeout);
    void idle() override;
    void onTimerInsertedAtFront() override;                       // override实现时 要去掉

    void contextResize(size_t size);
private:
    int m_epfd = 0;
    int m_tickleFds[2];

    std::atomic<size_t> m_pendingEventCount = {0};                // 正在等待执行的事件数量
    RWMutexType m_mutex;
    std::vector<FdContext*> m_fdContexts;
};

} // end of namespace

#endif