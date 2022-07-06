#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <vector>
#include <list>
#include <sstream>
#include <fstream>
#include <stdarg.h>
#include <map>
#include "singleton.h"
#include "util.h"
#include "thread.h"



/**
* %m -- 消息体
* %p -- level
* %r -- 启动后的时间
* %c -- 日志名称
* %t -- 线程id
* %n -- 回车换行
* %d -- 时间戳
* %f -- 文件名
* %l -- 行号
*/

#define SYLAR_LOG_LEVEL(logger, level) \
        if(logger->getLevel() <= level) \
        sylar::LogEventWrap (sylar::LogEvent::ptr (new sylar::LogEvent(\
        logger, level, __FILE__, __LINE__, 0,\
        sylar::getThreadId(), sylar::getFiberId(), time(0), sylar::Thread::GetName()))).getSS()


#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
        __FILE__, __LINE__, 0, sylar::getThreadId(),\
        sylar::getFiberId(), time(0), sylar::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(\
logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(\
logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(\
logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(\
logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(\
logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)


#define SYLAR_LOG_ROOT() sylar::loggerMgr::GetInstance()->getRoot()
#define SYLAR_LOG_NAME(name) sylar::loggerMgr::GetInstance()->getLogger(name)
    

namespace sylar{

    class Logger;
    class LogManager;

/**********************************日志级别***************************/
class LogLevel {
public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
    static const char* ToString(LogLevel::Level level);
    static LogLevel::Level FromString(const std::string& str);
};
/**********************************日志级别***************************/


/**********************************日志事件***************************/
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    typedef std::shared_ptr<Logger> log_ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
            const char* file, int32_t m_line, uint32_t elapse,
            uint32_t thread_id, uint32_t fiber_id, uint64_t time,std::string thread_name);

    const char* getFile() { return m_file;}
    int32_t getElapse() { return m_elapse;}
    uint32_t getThreadId() { return m_threadId;}
    uint32_t getFiberId() { return m_fiberId;}
    uint32_t getLine() { return m_line;}
    uint64_t getTime() { return m_time;}
    std::string getContent() const { return m_ss.str();}
    std::stringstream& getSS() { return m_ss;}
    std::shared_ptr<Logger> getLogger() { return m_logger;}
    LogLevel::Level getLevel() { return m_level;}
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
    const std::string& getThreadName() const { return m_threadName; }

private:
    const char* m_file = nullptr;               //文件名
    int32_t m_line = 0;                         //行号
    uint32_t m_elapse = 0;                      //程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;                    //线程id
    uint32_t m_fiberId = 0;                     //协程id
    uint64_t m_time = 0;                        //时间戳
    std::string m_threadName;
    std::stringstream m_ss;
    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};
/**********************************日志事件***************************/


/**********************************日志事件包裹***************************/
class LogEventWrap {
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();
    LogEvent::ptr getEvent() const { return m_event;}
    std::stringstream& getSS();

private:
LogEvent::ptr m_event;
};
/**********************************日志事件***************************/


/**********************************日志格式器***************************/
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    void init();
    bool isError() const { return m_error; }
    const std::string getPattern() const { return m_pattern; }
public:
    class FormatItem {
    public:
    typedef std::shared_ptr<FormatItem> ptr;
    FormatItem(const std::string& fmt = "") {}
        virtual ~FormatItem(){}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error = false;
};
/**********************************日志格式器***************************/


/**********************************日志输出地***************************/
class LogAppender {
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef SpinLock MutexType;

    virtual ~LogAppender() {}

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    void setFormatter(LogFormatter::ptr val) { m_formatter = val;}
    LogFormatter::ptr getFormatter();
    LogLevel::Level getLevel() const { return m_level;}
    void setLevel(LogLevel::Level level) { m_level = level; }
    virtual std::string toYamlString() = 0;
protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    bool m_hasFormatter = false;
    LogFormatter::ptr m_formatter;
    MutexType  m_mutex;
};
/**********************************日志输出地***************************/


/**********************************日志器***************************/
class Logger : public std::enable_shared_from_this<Logger> {
    friend class LogManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef SpinLock MutexType;

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
    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level level) { m_level = level;}
    const std::string& getName() const { return m_name; }
    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string& val);
    LogFormatter::ptr getFormatter();
    std::string toYamlString();
private:
    std::string m_name;
    LogLevel::Level m_level= LogLevel::DEBUG;
    std::list<LogAppender::ptr> m_appenders;
    LogFormatter::ptr m_formatter;
    Logger::ptr m_root;
    MutexType m_mutex;
};
/**********************************日志器***************************/


/**********************************输出到控制台的Appender***************************/
class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;
};
/**********************************输出到控制台的Appender***************************/


/**********************************输出到文件的Appender***************************/
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

    bool reopen();
    std::string toYamlString() override;
private:
    std::string m_filename;
    std::ofstream m_filestream;
    time_t m_time;
};
/**********************************输出到文件的Appender***************************/





/**********************************日志管理***************************/
class LogManager {
public:
    typedef SpinLock MutexType;
    
    LogManager();
    Logger::ptr getLogger(std::string name);
    Logger::ptr getRoot() const { return m_root; }
    void init();
    std::string toYamlString();
private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
    MutexType m_mutex;
};
typedef Singleton<LogManager> loggerMgr;
/**********************************日志管理***************************/
}

#endif