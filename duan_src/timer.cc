/*
实现线程模块
2024/6/19 9:41
by 六七
*/
#include "timer.h"
#include "util.h"

namespace duan{

bool Timer::Comparator::operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const{
    if(!lhs && !rhs){
        return false;
    }
    if(!lhs){
        return true;
    }
    if(!rhs){
        return false;
    }
    if(lhs->m_next < rhs->m_next){
        return true;
    }
    if(rhs->m_next < lhs->m_next){
        return false;
    }
    return lhs.get() < rhs.get();
}

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager)
    : m_recurring(recurring)
    , m_ms(ms)
    , m_cb(cb)
    , m_manager(manager){
        m_next = duan::GetCurrentMS() + m_ms;
}

Timer::Timer(uint64_t next)
    : m_next(next){

}

bool Timer::cancel(){
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if(m_cb){
        m_cb = nullptr;
        // 把它拿出来
        auto it = m_manager->m_timers.find(shared_from_this());
        m_manager->m_timers.erase(it);
        return true;
    }
    return false;
}   

bool Timer::refresh(){
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if(!m_cb){
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if(it == m_manager->m_timers.end()){
        return false;
    }
    m_manager->m_timers.erase(it);
    m_next = duan::GetCurrentMS() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true; 
}   

bool Timer::reset(uint64_t ms, bool from_now){
    if(ms == m_ms && !from_now){
        return true;
    }
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if(!m_cb){
        return false;
    }

    auto it = m_manager->m_timers.find(shared_from_this());
    if(it == m_manager->m_timers.end()){
        return false;
    }
    m_manager->m_timers.erase(it);
    uint64_t start = 0;
    if(from_now){
        start = duan::GetCurrentMS();
    }
    else{
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = start + m_ms;
    m_manager->addTimer(shared_from_this(), lock);
    return true; 
}       

TimerManager::TimerManager(){
    m_previousTime = duan::GetCurrentMS();
}

TimerManager::~TimerManager(){

}

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring){
    Timer::ptr timer(new Timer(ms, cb, recurring, this));
    RWMutexType::WriteLock lock(m_mutex);
    addTimer(timer, lock);
    return timer;
}

// weak_ptr的好处就是 不会使引用计数+1 但是又可以知道所指向的指针是否被释放了
static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb){
    std::shared_ptr<void> tmp = weak_cond.lock();     // 返回一下它的智能指针
    // 如果没释放的话会拿到它的智能指针 如果释放了就是空的
    if(tmp){
        cb();    // 如果没释放  就执行这个方法
    }
}

Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool, bool recurring){
    // bind返回一个函数对象
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTimer(){
    RWMutexType::ReadLock lock(m_mutex);
    m_tickled = false;
    if(m_timers.empty()){
        return ~0ull;
    }

    const Timer::ptr& next = *m_timers.begin();
    uint64_t now_ms = duan::GetCurrentMS();
    // 这个定时器 应该执行了  但是不知道什么原因 晚了
    if(now_ms >= next->m_next){
        return 0;
    }
    else{
        // 还需要等待的时间
        return next->m_next - now_ms;
    }
}

void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs){
    uint64_t now_ms = duan::GetCurrentMS();
    std::vector<Timer::ptr> expired;           // 存放的已经超时的数组

    {
        RWMutexType::ReadLock lock(m_mutex);
        if(m_timers.empty()){
            return;
        }
    }
    RWMutexType::WriteLock lock2(m_mutex);

    bool rollover = detectClockRollover(now_ms);
    if(!rollover && ((*m_timers.begin())->m_next > now_ms)){
        return;
    }

    Timer::ptr now_timer(new Timer(now_ms));
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    while(it != m_timers.end() && (*it)->m_next == now_ms){
        ++it;
    }
    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expired.size());

    for(auto& timer : expired){
        cbs.push_back(timer->m_cb);
        if(timer->m_recurring){
            timer->m_next = now_ms + timer->m_ms;
            m_timers.insert(timer);
        }
        else{
            timer->m_cb = nullptr;
        }
    }
    // lock2.unlock();
}

void TimerManager::addTimer(Timer::ptr val, RWMutexType::WriteLock& lock){
    auto it = m_timers.insert(val).first;
    bool at_front = (it == m_timers.begin() && !m_tickled);
    // 避免频繁修改时 来回执行
    if(at_front){
        m_tickled = true;
    }
    lock.unlock();

    if(at_front){
        onTimerInsertedAtFront();
    }
}

bool TimerManager::detectClockRollover(uint64_t now_ms){
    bool rollover = false;
    if(now_ms < m_previousTime && now_ms < (m_previousTime - 60 * 60 * 1000)){
        rollover = true;
    }
    m_previousTime = now_ms;
    return rollover;
}

bool TimerManager::hasTimer(){
    RWMutexType::ReadLock lock(m_mutex);
    return !m_timers.empty();
}

}