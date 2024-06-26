/*
实现日志模块
2024/5/14 14:30
by 六七
*/
#ifndef __DUAN_LOG_H__
#define __DUAN_LOG_H__

#include <iostream>
#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

#include "util.h"
#include "singleton.h"
#include "thread.h"
/*
日志操作 一般使用宏来定义 会非常方便
*/
// 以析构的方式进行输出
#define DUAN_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        duan::LogEventWrap(duan::LogEvent::ptr(new duan::LogEvent(logger, level, \
                __FILE__, __LINE__, 0, duan::GetThreadId(),\
                 duan::GetFiberId(), time(0), duan::Thread::GetName()))).getSS()
    
#define DUAN_LOG_DEBUG(logger) DUAN_LOG_LEVEL(logger, duan::LogLevel::DEBUG)
#define DUAN_LOG_INFO(logger) DUAN_LOG_LEVEL(logger, duan::LogLevel::INFO)
#define DUAN_LOG_WARN(logger) DUAN_LOG_LEVEL(logger, duan::LogLevel::WARN)
#define DUAN_LOG_ERROR(logger) DUAN_LOG_LEVEL(logger, duan::LogLevel::ERROR)
#define DUAN_LOG_FATAL(logger) DUAN_LOG_LEVEL(logger, duan::LogLevel::FATAL)

#define DUAN_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        duan::LogEventWrap(duan::LogEvent::ptr(new duan::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, duan::GetThreadId(), \
                duan::GetFiberId(), time(0), duan::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

#define DUAN_LOG_FMT_DEBUG(logger, fmt, ...) DUAN_LOG_FMT_LEVEL(logger, duan::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define DUAN_LOG_FMT_INFO(logger, fmt, ...) DUAN_LOG_FMT_LEVEL(logger, duan::LogLevel::INFO, fmt, __VA_ARGS__)
#define DUAN_LOG_FMT_WARN(logger, fmt, ...) DUAN_LOG_FMT_LEVEL(logger, duan::LogLevel::WARN, fmt, __VA_ARGS__)
#define DUAN_LOG_FMT_ERROR(logger, fmt, ...) DUAN_LOG_FMT_LEVEL(logger, duan::LogLevel::ERROR, fmt, __VA_ARGS__)
#define DUAN_LOG_FMT_FATAL(logger, fmt, ...) DUAN_LOG_FMT_LEVEL(logger, duan::LogLevel::FATAL, fmt, __VA_ARGS__)

#define DUAN_LOG_ROOT() duan::LoggerMgr::GetInstance()->getRoot()
#define DUAN_LOG_NAME(name) duan::LoggerMgr::GetInstance()->getLogger(name)


namespace duan{

class Logger;
class LoggerManager;

// 日志级别
class LogLevel{
public:
    enum Level{
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5,
    };

    static const char* toString(LogLevel::Level level);        // 将level转为对应的级别
    static LogLevel::Level FromString(const std::string& str);
};

// 日志事件
class LogEvent{
public:
    // C++11智能指针 相当于有一个LogEvent类型的指针 并给它取别名为ptr
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            , const char* file, int32_t line, uint32_t elapse
            , uint32_t thread_id, uint32_t fiber_id, uint64_t time
            , const std::string& thread_name);

    const char* getFile() const { return m_file;}               // 获取文件名
    int32_t getLine() const { return m_line;}                   // 获取行号
    uint32_t getElapse() const { return m_elapse;}              // 获取启动时间
    uint32_t getThreadId() const { return m_threadId;}          // 获取线程id
    uint32_t getFiberId() const { return m_fiberId;}            // 获取协程id
    uint64_t getTime() const {return m_time;}                   // 获取时间
    std::string getContent() const { return m_ss.str();}        // 获取消息内容
    const std::string& getThreadName() const { return m_threadName;}   // 获取线程名称
    std::shared_ptr<Logger> getLogger() const { return m_logger;}
    LogLevel::Level getLevel() const { return m_level;}
    std::stringstream& getSS() { return m_ss;}
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
private:
    // C++11开始支持 可以直接对成员变量初始化
    const char* m_file = nullptr;   // 文件名
    int32_t m_line = 0;             // 行号
    uint32_t m_elapse = 0;          // 程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;        // 线程ID
    uint32_t m_fiberId = 0;         // 协程ID
    uint64_t m_time;                // 时间戳
    std::string m_threadName;       // 用来表示线程名称
    std::stringstream m_ss;         // 用来表示消息内容
    
    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};

class LogEventWrap{
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();

    std::stringstream& getSS();
    LogEvent::ptr getEvent() const { return m_event;}
private:
    // 日志事件
    LogEvent::ptr m_event;
};

// 日志格式器
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);

    // t%   %threadid   %m  %n
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        FormatItem(const std::string& fmt = "") {}
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    void init();

    bool isError() const { return m_error;}
    const std::string getPattern() const { return m_pattern;}
private:
    std::string m_pattern;                                   // 表示日志格式
    std::vector<FormatItem::ptr> m_items;                    // 表示输出多少个项
    bool m_error = false;
};

// 日志输出地
class LogAppender{
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef Mutex MutexType;
    virtual ~LogAppender() {};

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;   // 子类必须实现这个方法
    virtual std::string toYamlString() = 0;

    void setFormatter(LogFormatter::ptr val);
    LogFormatter::ptr getFormatter();

    LogLevel::Level getLevel() const { return m_level;}
    void setLevel(LogLevel::Level val) { m_level = val;} 
protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    bool m_hasFormatter = false;
    MutexType m_mutex;
    LogFormatter::ptr m_formatter;                                            // 基类有可能会用到这个变量
};

// 日志器
class Logger : public std::enable_shared_from_this<Logger>{
friend class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef Mutex MutexType;

    Logger(const std::string& name = "root");

    void log(LogLevel::Level level, LogEvent::ptr event);
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    void clearAppenders();
    LogLevel::Level getLevel() const { return m_level;}
    void SetLevel(LogLevel::Level val) { m_level = val;}

    const std::string& getName() const { return m_name;}

    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string& val);

    LogFormatter::ptr getFormatter();

    std::string toYamlString();
private:
    std::string m_name;                                  // 日志名称
    LogLevel::Level m_level;                             // 日志级别
    MutexType m_mutex;
    std::list<LogAppender::ptr> m_appenders;             // Appender集合
    LogFormatter::ptr m_formatter;
    Logger::ptr m_root;
};

// 定义输出到控制台Appender
class StdoutLogAppender : public LogAppender{
friend class Logger;
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;      // override描述这个方法确实从父类继承过来，而且重载实现
    std::string toYamlString() override;
};

// 定义输出到文件的Appender
class FileLogAppender : public LogAppender{
friend class Logger;
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

    // 重新打开文件, 文件打开成功返回true
    bool reopen();
    std::string toYamlString() override;
private:
    std::string m_filename;
    std::ofstream m_filestream;
    uint64_t m_lastTime = 0;
};

// 负责管理所有的logger  默认是m_root
class LoggerManager{
public:
    typedef Mutex MutexType;
    LoggerManager();
    Logger::ptr getLogger(const std::string& name);

    void init();
    Logger::ptr getRoot() const { return m_root;}

    std::string toYamlString();
private:
    MutexType m_mutex;
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
};

typedef duan::Singleton<LoggerManager> LoggerMgr;

}  // end of namespace duan

#endif