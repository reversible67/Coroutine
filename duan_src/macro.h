/*
配置模块测试
2024/6/3 10:24
by 六七
*/
#ifndef __DUAN_MACRO_H__
#define __DUAN_MACRO_H__

#include <string.h>
#include <assert.h>
#include "util.h"

// 用宏去打印报错的位置 栈信息
// 进入协程前的准备工作 工欲善其事必先利其器
#define DUAN_ASSERT(x) \
    if(!(x)){ \
        DUAN_LOG_ERROR(DUAN_LOG_ROOT()) << " ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << duan::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define DUAN_ASSERT2(x, w) \
   if(!(x)){ \
        DUAN_LOG_ERROR(DUAN_LOG_ROOT()) << " ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << duan::BacktraceToString(100, 2, "    "); \
        assert(x); \
    } 


#endif