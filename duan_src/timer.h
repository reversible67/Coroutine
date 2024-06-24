/*
实现线程模块
2024/6/19 9:41
by 六七
*/
#ifndef __DUAN_TIMER_H
#define __DUAN_TIMER_H

#include <memory>
#include "thread.h"
#include <set>
#include <vector>

namespace duan{

class TimerManager;
class Timer : public std::enable_shared_from_this<Timer> {
friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;
    bool cancel();                                  // 取消
    bool refresh();                                 // 重新设置时间
    bool reset(uint64_t ms, bool from_now);         // 重新设置时间  是否从当前时间开始

private:
    // bool recurring 表示是否是循环定时器
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager);
    Timer(uint64_t next);

private:
    bool m_recurring = false;                       // 是否循环定时器
    uint64_t m_ms = 0;                              // 执行周期
    uint64_t m_next = 0;                            // 精确的执行时间
    std::function<void()> m_cb;
    TimerManager* m_manager = nullptr;
private:
    struct Comparator{
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };
};

class TimerManager{
friend class Timer;
public:
    typedef RWMutex RWMutexType;

    TimerManager();
    virtual ~TimerManager();

    // 添加定时器
    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);
    // 添加条件定时器
    // 用智能指针作为条件  因为它有引用计数  如果智能指针没有了  说明这个定时器就不用执行了
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool, bool recurring = false);

    uint64_t getNextTimer();          // 获取下一个定时器的执行时间
    void listExpiredCb(std::vector<std::function<void()> >& cbs);
protected:
    virtual void onTimerInsertedAtFront() = 0;
    void addTimer(Timer::ptr val, RWMutexType::WriteLock& lock);
    bool hasTimer();
private:
    bool detectClockRollover(uint64_t now_ms);
private:
    RWMutexType m_mutex;
    // 传递一个比较结构体
    std::set<Timer::ptr, Timer::Comparator> m_timers;
    bool m_tickled = false;
    uint64_t m_previousTime = 0;
};

} // end of namespace

#endif