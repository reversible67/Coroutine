/*
实现日志模块
2024/5/16 16:47
by 六七
*/
#ifndef __DUAN_UTIL_H__
#define __DUAN_UTIL_H__

#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

namespace duan{

pid_t GetThreadId();

uint32_t GetFiberId();

} // end of namespace duan

#endif