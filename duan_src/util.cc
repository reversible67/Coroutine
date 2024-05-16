/*
实现日志模块
2024/5/16 16:47
by 六七
*/
#include "util.h"

namespace duan{

pid_t GetThreadId(){
    return syscall(SYS_gettid);   // 获取线程id
}

uint32_t GetFiberId(){
    return 0;                     // 获取协程id 后续补全
}

} // end of namespace duan