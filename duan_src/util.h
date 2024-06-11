/*
实现工具类模块
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
#include <vector>
#include <string>

namespace duan{

pid_t GetThreadId();

uint32_t GetFiberId();

// 打印栈信息
void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

} // end of namespace duan

#endif