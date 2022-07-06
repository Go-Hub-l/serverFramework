#include "log.h"
#include <iostream>
#include <map>
#include <functional>
#include <string.h>
#include <memory>
#include "config.h"

namespace sylar {


/*****************************各种实际输出的格式类********************************/
/**********************************消息格式化***************************/
    class MessageFormatItem : public LogFormatter::FormatItem {
    public:
        MessageFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getContent();
        }
    };
    /**********************************消息格式化***************************/


    /**********************************日志级别格式化***************************/
    class LevelFormatItem : public LogFormatter::FormatItem {
    public:
        LevelFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << LogLevel::ToString(level);
        }
    };
    /**********************************日志级别格式化***************************/


    /**********************************日志启动的毫秒数***************************/
    class ElapseFormatItem : public LogFormatter::FormatItem {
    public:
        ElapseFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getElapse();
        }
    };
    /**********************************日志启动的毫秒数***************************/


    /**********************************日志名***************************/
    class NameFormatItem : public LogFormatter::FormatItem {
    public:
        NameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << logger->getName();
        }
    };
    /**********************************日志名***************************/


    /**********************************线程id***************************/
    class ThreadIdFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadIdFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getThreadId();
        }
    };
    /**********************************线程id***************************/

    /**********************************线程名***************************/
    class ThreadNameFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadNameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getThreadName();
        }
    };
    /**********************************线程名***************************/


    /**********************************协程id***************************/
    // class FiberIdFormatItem : public LogFormatter::FormatItem {
    // public:
    //     FiberIdFormatItem(const std::string& str = "") {}
    //     void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
    //         os << event->getFiberId();
    //     }
    // };
    /**********************************协程id***************************/


    /**********************************时间戳***************************/
    class DateTimeFormatItem : public LogFormatter::FormatItem {
    public:
        DateTimeFormatItem(const std::string& format = "%Y-%m-%d  %H:%M:%S")
            :m_format(format) {
            if (m_format.empty()) {
                m_format = "%Y-%m-%d  %H:%M:%S";
            }
        }
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            struct tm tm;
            time_t time = event->getTime();
            localtime_r(&time, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), m_format.c_str(), &tm);
            os << buf;
        }
    private:
        std::string m_format;
    };
    /**********************************时间戳***************************/


    /**********************************文件名***************************/
    class FilenameFormatItem : public LogFormatter::FormatItem {
    public:
        FilenameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFile();
        }
    };
    /**********************************文件名***************************/


    /**********************************行号***************************/
    class LineFormatItem : public LogFormatter::FormatItem {
    public:
        LineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getLine();
        }
    };
    /**********************************行号***************************/


    /**********************************换行***************************/
    class NewLineFormatItem : public LogFormatter::FormatItem {
    public:
        NewLineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << std::endl;
        }
    };
    /**********************************换行***************************/


    /**********************************字符串***************************/
    class StringFormatItem : public LogFormatter::FormatItem {
    public:
        StringFormatItem(const std::string& str)
            : FormatItem(str), m_string(str) {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << m_string;
        }
    private:
        std::string m_string;
    };
    /**********************************字符串***************************/


    /**********************************协程***************************/
    class FiberFormatItem : public LogFormatter::FormatItem {
    public:
        FiberFormatItem(const std::string& str) {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFiberId();
        }
    };
    /**********************************协程***************************/


    /**********************************制表符***************************/
    class TabFormatItem : public LogFormatter::FormatItem {
    public:
        TabFormatItem(const std::string& str) {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << "\t";
        }
    };
    /**********************************制表符***************************/
    /*****************************各种实际输出的格式类********************************/


    /**********************************日志事件***************************/
    LogEvent::LogEvent(std::shared_ptr<Logger> logger,
        LogLevel::Level level, const char* file,
        int32_t line, uint32_t elapse,
        uint32_t thread_id, uint32_t fiber_id,
        uint64_t time, std::string thread_name)
        :m_file(file)
        ,m_line(line)
        ,m_elapse(elapse)
        ,m_threadId(thread_id)
        ,m_fiberId(fiber_id)
        ,m_time(time)
        ,m_threadName(thread_name)
        ,m_logger(logger)
        ,m_level(level)
        {}

    void LogEvent::format(const char* fmt, ...) {
        va_list al;
        va_start(al, fmt);
        format(fmt, al);
        va_end(al);
    }

    void LogEvent::format(const char* fmt, va_list al) {
        char* buf = nullptr;
        int len = vasprintf(&buf, fmt, al);
        if (len != -1) {
            m_ss << std::string(buf, len);
            free(buf);
        }
    }

    /**********************************日志事件***************************/


    /**********************************日志事件包裹***************************/
    LogEventWrap::LogEventWrap(LogEvent::ptr e)
        : m_event(e) {}

    LogEventWrap::~LogEventWrap() {
        m_event->getLogger()->log(m_event->getLevel(), m_event);
    }

    std::stringstream& LogEventWrap::getSS() {
        return m_event->getSS();
    }
    /**********************************日志事件包裹***************************/


    /*****************************日志器*********************************************/
    Logger::Logger(const std::string& name)
        :m_name(name), m_level(LogLevel::DEBUG) {
        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    }

    void Logger::clearAppenders() {
        MutexType::Lock lock(m_mutex);
        m_appenders.clear();
    }

    void Logger::addAppender(LogAppender::ptr appender) {
        MutexType::Lock lock(m_mutex);
        m_appenders.push_back(appender);
        if (!appender->m_formatter.get()) {
            MutexType::Lock ll(appender->m_mutex);
            appender->m_formatter = m_formatter;//设置输出器的格式化类
        }
    }

    void Logger::delAppender(LogAppender::ptr appender) {
        MutexType::Lock lock(m_mutex);
        for (auto it = m_appenders.begin();
            it != m_appenders.end(); ++it) {
            if (*it == appender) {
                m_appenders.erase(it);
                break;
            }
        }
    }

    void Logger::setFormatter(LogFormatter::ptr val) {
        MutexType::Lock lock(m_mutex);
        m_formatter = val;

        for (auto& i : m_appenders) {
            MutexType::Lock ll(i->m_mutex);
            if (!i->m_hasFormatter) {
                i->m_formatter = m_formatter;
            }
        }
    }

    void Logger::setFormatter(const std::string& val) {
        std::cout << "---" << val << std::endl;
        sylar::LogFormatter::ptr new_val(new sylar::LogFormatter(val));
        if (new_val->isError()) {
            std::cout << "Logger setFormatter name=" << m_name
                << " value=" << val << " invalid formatter"
                << std::endl;
            return;
        }
        //m_formatter = new_val;
        setFormatter(new_val);
    }

    LogFormatter::ptr Logger::getFormatter() {
        MutexType::Lock lock(m_mutex);
        return m_formatter;
    }

    std::string Logger::toYamlString() {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        node["name"] = m_name;
        if (m_level != LogLevel::UNKNOW) {
            node["level"] = LogLevel::ToString(m_level);
        }
        if (m_formatter) {
            node["formatter"] = m_formatter->getPattern();
        }

        for (auto& i : m_appenders) {
            node["appenders"].push_back(YAML::Load(i->toYamlString()));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            MutexType::Lock lock(m_mutex);
            auto self = shared_from_this();
            if (!m_appenders.empty()) {
                for (auto& i : m_appenders) {

                    i->log(self, level, event);
                }
            } else {
                for (auto& i : m_root->m_appenders) {

                    i->log(self, level, event);
                }
            }
        }
    }

    void Logger::debug(LogEvent::ptr event) {
        m_level = LogLevel::DEBUG;
        log(m_level, event);
    }

    void Logger::info(LogEvent::ptr event) {
        m_level = LogLevel::INFO;
        log(m_level, event);
    }

    void Logger::warn(LogEvent::ptr event) {
        m_level = LogLevel::WARN;
        log(m_level, event);
    }

    void Logger::error(LogEvent::ptr event) {
        m_level = LogLevel::ERROR;
        log(m_level, event);
    }

    void Logger::fatal(LogEvent::ptr event) {
        m_level = LogLevel::FATAL;
        log(m_level, event);
    }
    /*****************************日志器*********************************************/


    /*****************************日志格式器*********************************************/
    LogFormatter::LogFormatter(const std::string& pattern)
        : m_pattern(pattern) {
            //初始化所有的格式器项目（如日期项，线程项，文件名项等...）
        init();
    }

    std::string LogFormatter::format(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
        std::stringstream ss;
        for (auto& i : m_items) {
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }

    void LogFormatter::init() {
        // 模式字符串的格式定义"[%x] %x %xxx{xxx} %xx %x"
        //str, format, type

        //一个元组，第一个表示key,第二个表示value（这里是对应的函数类）,
        //第三个表示区分这个格式化的字符是普通字符还是key,0表示普通字符，1表示key
        std::vector<std::tuple<std::string, std::string, int>> vec;
        std::string nstr;
        for (size_t i = 0; i < m_pattern.size(); ++i) {
            // if(m_pattern[i] == ' ') {
            //     continue;
            // }

            if (m_pattern[i] != '%') { //如果不是%说明是字符，则直接加入nstr  Question? 比如[xxx]在]后面可能将空格也添加，会不会出问题？
                nstr.append(1, m_pattern[i]);
                continue;
            }

            if ((i + 1) < m_pattern.size()) {
                if (m_pattern[i + 1] == '%') { //判断 %%这种情况
                    nstr.append(1, '%');
                    continue;
                }
            }
            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;

            std::string str;
            std::string fmt;
            //此时说明m_pattern[i]为 '%',因此接着判断%后面的字符
            while (n < m_pattern.size()) {
                //判断当前格式化的格式是否判断完了，如果判断完则退出本次循环判断下一个格式
                if (!isalpha(m_pattern[n])
                    && (m_pattern[n] != '{'
                        && m_pattern[n] != '}')
                    && (fmt_status == 0
                        || fmt_status == 2)) {
                    break;
                }

                if (fmt_status == 0) {
                    if (m_pattern[n] == '{') {
                        str = m_pattern.substr(i + 1, n - i - 1);//取出'%' 和 '{' 之间的字符
                        fmt_status = 1;//解析格式
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                }
                if (fmt_status == 1) {
                    if (m_pattern[n] == '}') {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);//取出'{'和'}'之间的字符
                        fmt_status = 2;
                        //++n;
                        break;
                    }
                }
                ++n;
            }

            if (fmt_status == 0) {
                if (!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr, "", 0));
                    nstr.clear();
                }
                str = m_pattern.substr(i + 1, n - i - 1);//将'%'后面的格式化字符截取出来
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            } else if (fmt_status == 1) {//说明只有'{'而无'}',故模式字符串不合法
                std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            } else if (fmt_status == 2) {
                if (!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr, "", 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
        }

        if (!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, "", 0));
            //nstr.clear();
        }
        //格式字符串映射一个格式类
        static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
    #define XX(str, C) \
            {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

            XX(m, MessageFormatItem),
            XX(p, LevelFormatItem),
            XX(r, ElapseFormatItem),
            XX(c, NameFormatItem),
            XX(t, ThreadIdFormatItem),
            XX(n, NewLineFormatItem),
            XX(d, DateTimeFormatItem),
            XX(f, FilenameFormatItem),
            XX(l, LineFormatItem),
            XX(T, TabFormatItem),
            XX(F, FiberFormatItem),
            XX(N, ThreadNameFormatItem),
#undef XX
        };

        for (auto& i : vec) {
            if (std::get<2>(i) == 0) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            } else {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end()) {
                    m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                } else {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }
            //std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
        }
    }
    /*****************************日志格式器*********************************************/


    /*****************************输出到文件的Appender*********************************************/
    LogFormatter::ptr LogAppender::getFormatter() {
        MutexType::Lock lock(m_mutex);
        return m_formatter;
    }
    
    FileLogAppender::FileLogAppender(const std::string& name)
        : m_filename(name) {
        reopen();
        m_time = time(0);
    }
    void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
        //1S重新打开一次文件
        if (time(0) - m_time >= 1) {
            reopen();
            m_time = time(0);
        }
        if (level >= m_level) {//todo
            MutexType::Lock lock(m_mutex);
            m_filestream << m_formatter->format(logger, level, event);
        }
    }

    std::string FileLogAppender::toYamlString() {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        node["type"] = "FileLogAppender";
        node["file"] = m_filename;
        if (m_level != LogLevel::UNKNOW) {
            node["level"] = LogLevel::ToString(m_level);
        }
        if (m_hasFormatter && m_formatter) {
            node["formatter"] = m_formatter->getPattern();
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    bool FileLogAppender::reopen() {
        MutexType::Lock lock(m_mutex);
        if (m_filestream) {
            m_filestream.close();
        }
        //app表示以追加的方式打开
        m_filestream.open(m_filename, std::ofstream::out | std::ofstream::app);
        return !!m_filestream;
    }
    /*****************************输出到文件的Appender*********************************************/


    /*****************************输出到控制台的Appender*********************************************/
    void StdoutLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            MutexType::Lock lock(m_mutex);
            std::cout << m_formatter->format(logger, level, event);
        }
    }

    std::string StdoutLogAppender::toYamlString() {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        node["type"] = "StdoutLogAppender";
        if (m_level != LogLevel::UNKNOW) {
            node["level"] = LogLevel::ToString(m_level);
        }
        if (m_hasFormatter && m_formatter) {
            node["formatter"] = m_formatter->getPattern();
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
    /*****************************输出到控制台的Appender*********************************************/





    /**********************************日志级别***************************/
    const char* LogLevel::ToString(LogLevel::Level level) {
        switch (level) {
#define XX(name) \
    case LogLevel::name: \
        return #name;        //这里的意思就是将name转换为字符串name

            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
#undef XX
        default:
            return "UNKNOW";
        }
        return "UNKNOW";
    }

    LogLevel::Level LogLevel::FromString(const std::string& str) {
#define XX(level, v) \
    if(str == #v) { \
        return LogLevel::level; \
    }
        XX(DEBUG, debug);
        XX(INFO, info);
        XX(WARN, warn);
        XX(ERROR, error);
        XX(FATAL, fatal);

        XX(DEBUG, DEBUG);
        XX(INFO, INFO);
        XX(WARN, WARN);
        XX(ERROR, ERROR);
        XX(FATAL, FATAL);
        return LogLevel::UNKNOW;
#undef XX
    }
    /**********************************日志级别***************************/


    /**********************************日志管理***************************/
    LogManager::LogManager() {
        MutexType::Lock lock(m_mutex);
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

        m_loggers[m_root->m_name] = m_root;

        init();
    }

    Logger::ptr LogManager::getLogger(std::string name) {
        MutexType::Lock lock(m_mutex);
        auto it = m_loggers.find(name);
        if (it != m_loggers.end())
            return it->second;

        Logger::ptr logger(new Logger(name));
        logger->m_root = m_root;
        m_loggers[name] = logger;
        return logger;
    }
    /**********************************日志管理***************************/

    struct LogAppenderDefine {
        int type = 0; //1 File, 2 Stdout
        LogLevel::Level level = LogLevel::UNKNOW;
        std::string formatter;
        std::string file;

        bool operator==(const LogAppenderDefine& oth) const {
            return type == oth.type
                && level == oth.level
                && formatter == oth.formatter
                && file == oth.file;
        }
    };

    struct LogDefine {
        std::string name;
        LogLevel::Level level = LogLevel::UNKNOW;
        std::string formatter;
        std::vector<LogAppenderDefine> appenders;

        bool operator==(const LogDefine& oth) const {
            return name == oth.name
                && level == oth.level
                && formatter == oth.formatter
                && appenders == appenders;
        }

        bool operator<(const LogDefine& oth) const {
            return name < oth.name;
        }

        bool isValid() const {
            return !name.empty();
        }
    };

    template<>
    class LexicalCast<std::string, LogDefine> {
    public:
        LogDefine operator()(const std::string& v) {
            YAML::Node n = YAML::Load(v);
            LogDefine ld;
            if (!n["name"].IsDefined()) {
                std::cout << "log config error: name is null, " << n
                    << std::endl;
                throw std::logic_error("log config name is null");
            }
            ld.name = n["name"].as<std::string>();
            ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
            if (n["formatter"].IsDefined()) {
                ld.formatter = n["formatter"].as<std::string>();
            }

            if (n["appenders"].IsDefined()) {
                //std::cout << "==" << ld.name << " = " << n["appenders"].size() << std::endl;
                for (size_t x = 0; x < n["appenders"].size(); ++x) {
                    auto a = n["appenders"][x];
                    if (!a["type"].IsDefined()) {
                        std::cout << "log config error: appender type is null, " << a
                            << std::endl;
                        continue;
                    }
                    std::string type = a["type"].as<std::string>();
                    LogAppenderDefine lad;
                    if (type == "FileLogAppender") {
                        lad.type = 1;
                        if (!a["file"].IsDefined()) {
                            std::cout << "log config error: fileappender file is null, " << a
                                << std::endl;
                            continue;
                        }
                        lad.file = a["file"].as<std::string>();
                        if (a["formatter"].IsDefined()) {
                            lad.formatter = a["formatter"].as<std::string>();
                        }
                    } else if (type == "StdoutLogAppender") {
                        lad.type = 2;
                        if (a["formatter"].IsDefined()) {
                            lad.formatter = a["formatter"].as<std::string>();
                        }
                    } else {
                        std::cout << "log config error: appender type is invalid, " << a
                            << std::endl;
                        continue;
                    }

                    ld.appenders.push_back(lad);
                }
            }
            return ld;
        }
    };

    template<>
    class LexicalCast<LogDefine, std::string> {
    public:
        std::string operator()(const LogDefine& i) {
            YAML::Node n;
            n["name"] = i.name;
            if (i.level != LogLevel::UNKNOW) {
                n["level"] = LogLevel::ToString(i.level);
            }
            if (!i.formatter.empty()) {
                n["formatter"] = i.formatter;
            }

            for (auto& a : i.appenders) {
                YAML::Node na;
                if (a.type == 1) {
                    na["type"] = "FileLogAppender";
                    na["file"] = a.file;
                } else if (a.type == 2) {
                    na["type"] = "StdoutLogAppender";
                }
                if (a.level != LogLevel::UNKNOW) {
                    na["level"] = LogLevel::ToString(a.level);
                }

                if (!a.formatter.empty()) {
                    na["formatter"] = a.formatter;
                }

                n["appenders"].push_back(na);
            }
            std::stringstream ss;
            ss << n;
            return ss.str();
        }
    };

    sylar::ConfigVar<std::set<LogDefine> >::ptr g_log_defines =
        sylar::Config::Lookup("logs", std::set<LogDefine>(), "logs config");

    struct LogIniter {
        LogIniter() {//注：这里的10只是临时加的
            g_log_defines->addListener([](const std::set<LogDefine>& old_value,
                const std::set<LogDefine>& new_value) {
                    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "on_logger_conf_changed";
                    for (auto& i : new_value) {
                        auto it = old_value.find(i);
                        sylar::Logger::ptr logger;
                        if (it == old_value.end()) {
                            //新增logger
                            logger = SYLAR_LOG_NAME(i.name);
                        } else {
                            if (!(i == *it)) {
                                //修改的logger
                                logger = SYLAR_LOG_NAME(i.name);
                            } else {
                                continue;
                            }
                        }
                        logger->setLevel(i.level);
                        //std::cout << "** " << i.name << " level=" << i.level
                        //<< "  " << logger << std::endl;
                        if (!i.formatter.empty()) {
                            logger->setFormatter(i.formatter);
                        }

                        logger->clearAppenders();
                        for (auto& a : i.appenders) {
                            sylar::LogAppender::ptr ap;
                            if (a.type == 1) {
                                ap.reset(new FileLogAppender(a.file));
                            } else if (a.type == 2) {
                                // if (!sylar::loggerMgr::GetInstance()) {
                                //     ap.reset(new StdoutLogAppender);
                                // } else {
                                //     continue;
                                // }
                                ap.reset(new StdoutLogAppender);
                            }
                            ap->setLevel(a.level);
                            if (!a.formatter.empty()) {
                                LogFormatter::ptr fmt(new LogFormatter(a.formatter));
                                if (!fmt->isError()) {
                                    ap->setFormatter(fmt);
                                } else {
                                    std::cout << "log.name=" << i.name << " appender type=" << a.type
                                        << " formatter=" << a.formatter << " is invalid" << std::endl;
                                }
                            }
                            logger->addAppender(ap);
                        }
                    }

                    for (auto& i : old_value) {
                        auto it = new_value.find(i);
                        if (it == new_value.end()) {
                            //删除logger
                            auto logger = SYLAR_LOG_NAME(i.name);
                            logger->setLevel((LogLevel::Level) 0);
                            logger->clearAppenders();
                        }
                    }
                });
        }
    };

    static LogIniter __log_init;

    std::string LogManager::toYamlString() {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        for (auto& i : m_loggers) {
            node.push_back(YAML::Load(i.second->toYamlString()));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    void LogManager::init() {}
}