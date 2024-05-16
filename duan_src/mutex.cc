/*
信号量实现
2024/5/14 10:57
by 六七
*/
#include "mutex.h"

namespace sylar{

Semaphore::Semaphore(uint32_t count){
    if(sem_init(&m_semaphore, 0, count)){
        throw std::logic_error("sem_init error");
    }
}

Semaphore::~Semaphore(){
    sem_destroy(&m_semaphore);
}

void Semaphore::wait(){
    if(sem_wait(&m_semaphore)){
        throw std::logic_error("sem_wait error");
    }
}

void Semaphore::notify(){
    if(sem_post(&m_semaphore)){
        throw std::logic_error("sem_post error");
    }
}

} // namespace sylar