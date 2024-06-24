/*
IO调度模块测试
2024/5/18 11:48
by 六七
*/
#include "duan_src/duan.h"
#include "duan_src/iomanager.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

duan::Logger::ptr g_logger = DUAN_LOG_ROOT();

int sock = 0;

void test_fiber(){
    DUAN_LOG_INFO(g_logger) << "test_fiber sock = " << sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    // 对一个打开的文件描述符执行一系列控制操作
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family= AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "155.239.210.27", &addr.sin_addr.s_addr);

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))){

    }
    else if(errno == EINPROGRESS){
        DUAN_LOG_INFO(g_logger) << "add event error = " << errno << " " << strerror(errno);
        // 检查是否会回调
        duan::IOManager::GetThis()->addEvent(sock, duan::IOManager::READ, [](){
            DUAN_LOG_INFO(g_logger) << "read callback";
        });
        duan::IOManager::GetThis()->addEvent(sock, duan::IOManager::WRITE, [](){
            DUAN_LOG_INFO(g_logger) << "write callback";
            // close(sock);
            duan::IOManager::GetThis()->cancelEvent(sock, duan::IOManager::READ);
            close(sock);
        });
    }
    else{
        DUAN_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }
    connect(sock, (const sockaddr*)&addr, sizeof(addr));
}

void test1(){
    std::cout << "EPOLLIN = "  << EPOLLIN
              << "EPOLLOUT = " << EPOLLOUT << std::endl;
    duan::IOManager iom(2, false);
    iom.scheduler(&test_fiber);
}

// 测试timer
duan::Timer::ptr s_timer;
void test_timer(){
    duan::IOManager iom(2);
    s_timer = iom.addTimer(1000, [](){
        static int i = 0;
        DUAN_LOG_INFO(g_logger) << "hello timer i = " << i;
        if(++i == 3){
            s_timer->reset(2000, true);
            // s_timer->cancel();
        }
    }, true);
}

int main(int argc, char** argv){
    // test1();
    test_timer();
    return 0;
}