/*
协程调度模块
2024/6/5 16:50
by 六七
*/
#include "scheduler.h"
#include "log.h"

namespace duan{

static duan::Logger::ptr g_logger = DUAN_LOG_NAME("system");

Scheduler::Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = ""){

}
Scheduler::~Scheduler(){                           

}

Scheduler* Scheduler::GetThis(){

}

Fiber* Scheduler::GetMainFiber(){

}

void Scheduler::start(){

}

void Scheduler::stop(){

}

} // end of namespace