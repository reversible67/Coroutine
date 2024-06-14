/*
IO调度模块
2024/6/12 15:44
by 六七
*/
#include "iomanager.h"
#include "macro.h"
#include <unistd.h>
#include <sys/epoll.h>
#include "log.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>

namespace duan{

static duan::Logger::ptr g_logger = DUAN_LOG_NAME("system");

// 获取事件类型
IOManager::FdContext::EventContext& IOManager::FdContext::getContext(IOManager::Event event){
    switch(event){
        case IOManager::READ:
            return read;
        case IOManager::WRITE:
            return write;
        default:
            DUAN_ASSERT2(false, "getContext");
    }
}      

void IOManager::FdContext::resetContext(EventContext& ctx){
    ctx.scheduler = nullptr;
    ctx.fiber.reset();
    ctx.cb = nullptr;
}

void IOManager::FdContext::triggerEvent(IOManager::Event event){
    DUAN_ASSERT(m_events & event);
    m_events = (Event)(m_events & ~event);
    EventContext& ctx = getContext(event);
    if(ctx.cb){
        ctx.scheduler->scheduler(&ctx.cb);
    }
    else{
        ctx.scheduler->scheduler(&ctx.fiber);
    }
    ctx.scheduler = nullptr;
    return;
}

IOManager::IOManager(size_t threads, bool use_caller, const std::string& name)
    : Scheduler(threads, use_caller, name) {
    m_epfd = epoll_create(5000);
    DUAN_ASSERT(m_epfd > 0);

    // 成功时 返回0
    int rt = pipe(m_tickleFds);
    DUAN_ASSERT(rt == 0);

    epoll_event event;                                                  // epoll事件的结构体
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;                                   // 触发模式
    event.data.fd = m_tickleFds[0];

    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);                    // 修改句柄的一些属性
    DUAN_ASSERT(rt == 0);

    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    DUAN_ASSERT(rt == 0);

    contextResize(32);                                                  // 默认大小32
    // m_fdContexts.resize(64);                                         // 默认大小64

    start();
}

IOManager::~IOManager(){
    // 将句柄关闭掉
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for(size_t i = 0; i < m_fdContexts.size(); ++i){
        if(m_fdContexts[i]){
            delete m_fdContexts[i];
        }
    }
}

void IOManager::contextResize(size_t size){
    m_fdContexts.resize(size);

    for(size_t i = 0; i < m_fdContexts.size(); ++i){
        if(!m_fdContexts[i]){
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

int IOManager::addEvent(int fd, Event event, std::function<void()> cb){
    FdContext* fd_ctx = nullptr;
    RWMutexType::ReadLock lock(m_mutex);
    // 空间足够
    if((int)m_fdContexts.size() > fd){
        fd_ctx = m_fdContexts[fd];
        lock.unlock();
    }
    else{
        lock.unlock();
        RWMutexType::WriteLock lock2(m_mutex);
        contextResize(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    }

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    if(fd_ctx->m_events & event){
        // 该事件已经有人加过了
        DUAN_LOG_ERROR(g_logger) << "addEvent assert fd = " << fd << " event = " << event << " fd_ctx.event = " << fd_ctx->m_events;
        DUAN_ASSERT(!(fd_ctx->m_events & event));
    }

    int op = fd_ctx->m_events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | fd_ctx->m_events | event;
    epevent.data.ptr = fd_ctx;

    // 操作成功返回0 失败返回-1
    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if(rt){
        DUAN_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", " 
            << op << ", " << fd << ", " << epevent.events << "):" 
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return -1;
    }

    ++m_pendingEventCount;      // 事件数量++
    fd_ctx->m_events = (Event)(fd_ctx->m_events | event);
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);              // 读事件还是写事件  并把事件成员进行完善   这里是引用
    DUAN_ASSERT(!event_ctx.scheduler && !event_ctx.fiber && !event_ctx.cb);
    // DUAN_ASSERT(!event_ctx.scheduler);
    // DUAN_ASSERT(!event_ctx.fiber);
    // DUAN_ASSERT(!event_ctx.cb);
    event_ctx.scheduler = Scheduler::GetThis();
    if(cb){
        event_ctx.cb.swap(cb);
    }
    else{
        event_ctx.fiber = Fiber::GetThis();
        DUAN_ASSERT(event_ctx.fiber->getState() == Fiber::EXEC);
    }
    return 0;
}

bool IOManager::delEvent(int fd, Event event){
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    // 没有这个事件
    if(!(fd_ctx->m_events & event)){
        return false;
    }

    Event new_events = (Event)(fd_ctx->m_events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if(rt){
        DUAN_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", " 
            << op << ", " << fd << ", " << epevent.events << "):" 
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }
    --m_pendingEventCount;              // 删除一个事件
    fd_ctx->m_events = new_events;
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);
    return true;
}

bool IOManager::cancelEvent(int fd, Event event){
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    // 没有这个事件
    if(!(fd_ctx->m_events & event)){
        return false;
    }

    Event new_events = (Event)(fd_ctx->m_events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if(rt){
        DUAN_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", " 
            << op << ", " << fd << ", " << epevent.events << "):" 
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

    fd_ctx->triggerEvent(event);
    --m_pendingEventCount;              

    return true;
}

bool IOManager::cancelAll(int fd){
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    // 没有这个事件
    if(!fd_ctx->m_events){
        return false;
    }

    int op = EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = 0;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if(rt){
        DUAN_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", " 
            << op << ", " << fd << ", " << epevent.events << "):" 
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

    if(fd_ctx->m_events & READ){
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;             
    }
    if(fd_ctx->m_events & WRITE){
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEventCount;              
    }
    DUAN_ASSERT(fd_ctx->m_events == 0);
    return true;
}

IOManager* IOManager::GetThis(){
    return dynamic_cast<IOManager*>(Scheduler::GetThis());
}

// 唤醒线程
void IOManager::tickle(){
    if(hasIdleThreads()){
        return;
    }
    // 写一个字母 这样就会执行idle 从而唤醒
    int rt = write(m_tickleFds[1], "T", 1);
    DUAN_ASSERT(rt == 1);
    return;
}

bool IOManager::stopping(){
    return Scheduler::stopping() && m_pendingEventCount == 0;
}

// 当线程什么事都不干的时候 就会陷入idle
void IOManager::idle(){
    epoll_event* events = new epoll_event[64]();
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event* ptr){
        delete[] ptr;
    });

    int rt = 0;
    while(true){
        if(stopping()){
            DUAN_LOG_INFO(g_logger) << "name = " << getName() << " idle stopping exit";
            break;
        }

        do{
            static const int MAX_TIMEOUT = 5000;                // 5s
            /*
            epoll_wait（）系统调用等待文件描述符epfd引用的epoll实例上的事件
            epoll_wait（）最多返回最大事件。 maxevents参数必须大于零。 timeout参数指定epoll_wait（）将阻止的最小毫秒数
            返回值：成功时，epoll_wait（）返回为请求的I / O准备就绪的文件描述符的数目；如果在请求的超时毫秒内没有文件描述符准备就绪，则返回零。
            发生错误时，epoll_wait（）返回-1并正确设置errno。
            */
            rt = epoll_wait(m_epfd, events, 64, MAX_TIMEOUT);

            // 中断了
            if(rt < 0 && errno == EINTR){

            }
            else{
                break;
            }
        }
        while(true);

        for(int i = 0; i < rt; ++i){
            epoll_event& event = events[i];
            if(event.data.fd == m_tickleFds[0]){
                uint8_t dummy;
                // 一直读 读到没有数据
                while(read(m_tickleFds[0], &dummy, 1) == 1);
                continue;
            }

            FdContext* fd_ctx = (FdContext*)event.data.ptr;
            FdContext::MutexType::Lock lock(fd_ctx->mutex);
            // 错误或是中断
            if(event.events & (EPOLLERR | EPOLLHUP)){
                event.events |= EPOLLIN | EPOLLOUT;     // 将读和写加入
            }
            int real_events = NONE;
            if(event.events & EPOLLIN){
                real_events |= READ;
            }
            if(event.events & EPOLLOUT){
                real_events |= WRITE;
            }

            if((fd_ctx->m_events & real_events) == NONE){
                continue;
            }

            // 当前事件 - 触发事件 = 遗留事件
            int left_events = (fd_ctx->m_events & ~real_events);
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;

            // 成功时返回0 错误时返回-1
            int rt2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
            if(rt2){
                DUAN_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", " 
                    << op << ", " << fd_ctx->fd << ", " << event.events << "):" 
                    << rt2 << " (" << errno << ") (" << strerror(errno) << ")";
                continue;
            }

            // 处理事件
            if(real_events & READ){
                fd_ctx->triggerEvent(READ);
                --m_pendingEventCount;
            }
            if(real_events & WRITE){
                fd_ctx->triggerEvent(WRITE);
                --m_pendingEventCount;
            }
        }

        // 让出执行权
        Fiber::ptr cur = Fiber::GetThis();
        auto raw_ptr = cur.get();
        cur.reset();

        raw_ptr->swapOut();    
    }
}

} // end of namespace