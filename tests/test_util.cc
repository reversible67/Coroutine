/*
栈信息返回测试
2024/6/3 10:37
by 六七
*/
#include "duan_src/duan.h"
#include <assert.h>

duan::Logger::ptr g_logger = DUAN_LOG_ROOT();

void test_assert(){
    DUAN_LOG_INFO(g_logger) << duan::BacktraceToString(10, 2, "  ");
    // DUAN_ASSERT(false);
    DUAN_ASSERT2(0 == 1, "abcdef xx");
}

int main(int agrc, char** argv){
    test_assert();
    return 0;
}