/*
实现日志模块
2024/5/16 16:47
by 六七
*/
#include "util.h"
#include "log.h"
#include "fiber.h"
#include <execinfo.h>

namespace duan{

duan::Logger::ptr g_logger = DUAN_LOG_NAME("system");

pid_t GetThreadId(){
    return syscall(SYS_gettid);   // 获取线程id
}

uint32_t GetFiberId(){
    return duan::Fiber::GetFiberId();                     // 获取协程id 后续补全
}

void Backtrace(std::vector<std::string>& bt, int size, int skip){
    void** array = (void**)malloc(sizeof(void*) * size);   // 栈指针 栈大小
    // backtrace() 返回一组地址，backtrace_symbols()象征性地翻译这些地址为一个描述地址的字符串数组
    size_t s = ::backtrace(array, size);                   // 返回大小

    char** strings = backtrace_symbols(array, s);
    if(strings == NULL){
        DUAN_LOG_ERROR(g_logger) << "backtrace_symbols error";
        return;
    }

    for(size_t i = skip; i < s; ++i){
        bt.push_back(strings[i]);
    }
    free(strings);
    free(array);
    return;
}

std::string BacktraceToString(int size, int skip, const std::string& prefix){
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for(size_t i = 0; i < bt.size(); ++i){
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

} // end of namespace duan