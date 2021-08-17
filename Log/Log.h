/**
 * @file Log.h
 * @brief 日志模块
 * @author TenSir
 * @date 2021年08月12日
 * @copyright Copyright (c) 2021年
 */
#ifndef _TENSIR_LOG_H
#define _TENSIR_LOG_H

#include <iostream>
#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>
#include <tuple>

/**
 * @brief 使用流式方式将日志级别level的日志写入到logger
 */
#define TENSIR_LOG_LEVEL(logger, level)                                          \
    if (logger->getLevel() <= level)                                             \
    tensir::LogEventWrapper(tensir::LogEvent::ptr(new tensir::LogEvent(logger,   \
                                                                       level,    \
                                                                       __FILE__, \
                                                                       __LINE__, \
                                                                       0,        \
                                                                       1,        \
                                                                       1,        \
                                                                       time(0),  \
                                                                       "main"))) \
        .getSS()
/**
 * @brief 使用流式方式将日志级别debug的日志写入到logger
 */
#define TENSIR_LOG_DEBUG(logger) TENSIR_LOG_LEVEL(logger, tensir::LogLevel::DEBUG)

/**
 * @brief 使用格式化方式将日志级别level的日志写入到logger
 */
#define TENSIR_LOG_FMT_LEVEL(logger, level, fmt, ...)                            \
    if (logger->getLevel() <= level)                                             \
    tensir::LogEventWrapper(tensir::LogEvent::ptr(new tensir::LogEvent(logger,   \
                                                                       level,    \
                                                                       __FILE__, \
                                                                       __LINE__, \
                                                                       0,        \
                                                                       1,        \
                                                                       1,        \
                                                                       time(0),  \
                                                                       "main"))) \
        .getEvent()                                                              \
        ->format(fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别debug的日志写入到logger
 */
#define TENSIR_LOG_FMT_DEBUG(logger, fmt, ...) TENSIR_LOG_FMT_LEVEL(logger, tensir::LogLevel::DEBUG, fmt, __VA__ARGS__)

// /**
//  * @brief 获取主日志器
//  */
// #define TENSIR_LOG_ROOT() tensir::LoggerMgr->getRoot()

// /**
//  * @brief 获取name的日志器
//  */
// #define TENSIR_LOG_NAME(name) tensir::LoggerMgr->getLogger(name)

namespace tensir
{
    class Logger;
    class LoggerManager;
    /**
     * @brief 日志级别
     */
    class LogLevel
    {
    public:
        enum Level
        {
            UNKNOWN,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL
        };

        /**
         * @brief 将日志级别转化为string输出
         * @param[in] level 日志级别
         * @return 日志级别串
         */
        static const char *toString(LogLevel::Level level);

        /**
         * @brief 将字符串转化为日志级别，全大写或者全小写
         * @param[in] str 日志级别串
         * @return 日志级别
         */
        static LogLevel::Level fromString(const std::string &str);
    };

    /**
     * @brief 日志事件
     */
    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent() {}

        /**
         * @brief 构造函数
         * @param[in] logger 日志器
         * @param[in] level 日志级别
         * @param[in] filename 文件名
         * @param[in] line 行号
         * @param[in] elapse 程序启动依赖的耗时(毫秒)
         * @param[in] thread_id 线程id
         * @param[in] fiber_id 协程id
         * @param[in] time 日志时间（秒）
         * @param[in] thread_name 线程名称
         */
        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char *filename,
                 uint32_t line, uint32_t elapse, uint32_t thread_id,
                 uint32_t fiber_id, uint64_t time, const std::string &thread_name);

        /**
         * @brief 返回日志器
         */
        std::shared_ptr<Logger> getLogger() const { return m_logger; }

        /**
         * @brief 返回日志级别
         */
        LogLevel::Level getLevel() const { return m_level; }
        /**
         * @brief 返回文件名
         */
        const char *getFilename() const { return m_filename; }

        /**
         * @brief 返回行号
         */
        int32_t getLine() const { return m_line; }

        /**
         * @brief 返回耗时
         */
        uint32_t getElapse() const { return m_elapse; }

        /**
         * @brief 返回线程ID
         */
        uint32_t getThreadId() const { return m_threadId; }

        /**
         * @brief 返回协程ID
         */
        uint32_t getFiberId() const { return m_fiberId; }

        /**
         * @brief 返回时间
         */
        uint64_t getTime() const { return m_time; }

        /**
         * @brief 返回线程名称
         */
        const std::string &getThreadName() const { return m_threadName; }

        /**
         * @brief 返回日志内容
         */
        std::string getContent() const { return m_ss.str(); }

        /**
         * @brief 返回日志内容字符串流
         */
        std::stringstream &getSS() { return m_ss; }

        /**
         * @brief 格式化写入日志内容
         */
        void format(const char *fmt, ...);

        /**
         * @brief 格式化写入日志内容
         */
        void format(const char *fmt, va_list al);

    private:
        /// 日志器
        std::shared_ptr<Logger> m_logger;
        /// 日志等级
        LogLevel::Level m_level;
        /// 文件名
        const char *m_filename = nullptr;
        /// 行号
        int32_t m_line = 0;
        /// 程序启动开始到现在的毫秒数
        uint32_t m_elapse = 0;
        /// 线程ID
        uint32_t m_threadId = 0;
        /// 协程ID
        uint32_t m_fiberId = 0;
        /// 时间戳
        uint64_t m_time = 0;
        /// 线程名称
        std::string m_threadName;
        /// 日志内容流
        std::stringstream m_ss;
    };

    /**
     * @brief 日志事件包装器
     */
    class LogEventWrapper
    {
    public:
        /**
         * @brief 构造函数
         * @param[in] e 日志事件
         */
        LogEventWrapper(LogEvent::ptr e);

        /**
         * @brief 析构函数
         */
        ~LogEventWrapper();

        /**
         * @brief 获取日志事件
         */
        LogEvent::ptr getEvent() const { return m_event; }

        /**
         * @brief 获取日志内容流
         */
        std::stringstream &getSS();

    private:
        /**
         * @brief 日志事件
         */
        LogEvent::ptr m_event;
    };

    /**
     * @brief 日志格式化
     */
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;

        /**
         * @brief 构造函数
         * @param[in] pattern 格式模板
         * @details 
         *  %m 消息
         *  %p 日志级别
         *  %r 累计毫秒数
         *  %c 日志名称
         *  %t 线程id
         *  %n 换行
         *  %d 时间
         *  %f 文件名
         *  %l 行号
         *  %T 制表符
         *  %F 协程id
         *  %N 线程名称
         *
         *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
         */
        LogFormatter(const std::string &pattern);

        /**
         * @brief 返回格式化日志串
         * @param[in] logger 日志器
         * @param[in] level 日志级别
         * @param[in] event 日志事件
         */
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

        std::ostream &format(std::ostream &ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

        /**
         * @brief 日志内容项格式化
         */
        class FormatItem
        {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            /**
             * @brief 析构函数
             */
            virtual ~FormatItem() {}
            /**
             * @brief 格式化日志到流
             * @param[in, out] os 日志输出流
             * @param[in] logger 日志器
             * @param[in] level 日志等级
             * @param[in] event 日志事件
             */
            virtual void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

        /**
         * @brief 初始化,解析日志模板
         */
        void init();

        /**
         * @brief 是否有错误
         */
        bool isError() const { return m_error; }

        /**
         * @brief 返回日志模板
         */
        const std::string getPattern() const { return m_pattern; }

    private:
        /// 日志格式模板
        std::string m_pattern;
        /// 日志格式解析后格式
        std::vector<FormatItem::ptr> m_items;
        /// 是否有错误
        bool m_error = false;
    };

    /**
     * @brief 日志输出目标
     */
    class LogAppender
    {
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        // typedef Spinlock MutexType;

        /**
         * @brief 析构函数
         */
        virtual ~LogAppender() {}

        /**
         * @brief 写入日志
         * @param[in] logger 日志器
         * @param[in] level 日志级别
         * @param[in] event 日志事件
         */
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

        /**
         * @brief 将日志输出目标的配置转成YAML String
         */
        virtual std::string toYamlString() = 0;

        /**
         * @brief 更改日志格式器
         */
        void setFormatter(LogFormatter::ptr val);

        /**
         * @brief 获取日志格式器
         */
        LogFormatter::ptr getFormatter();

        /**
         * @brief 获取日志级别
         */
        LogLevel::Level getLevel() const { return m_level; }

        /**
         * @brief 设置日志级别
         */
        void setLevel(LogLevel::Level val) { m_level = val; }

        /**
         * @brief 是否定义了日志格式
         */
        bool hasFormatter() { return m_hasFormatter; }

    protected:
        /// 日志级别
        LogLevel::Level m_level = LogLevel::DEBUG;
        /// 是否有自己的日志格式器
        bool m_hasFormatter = false;
        /// Mutex
        // MutexType m_mutex;
        /// 日志格式器
        LogFormatter::ptr m_formatter;
    };

    /**
     * @brief 日志器
     */
    class Logger : public std::enable_shared_from_this<Logger>
    {
    public:
        typedef std::shared_ptr<Logger> ptr;
        // typedef Spinlock MutexType;

        /**
         * @brief 构造函数
         * @param[in] name 日志器名称
        */
        Logger(const std::string &name = "root");

        /**
         * @brief 写日志
         * @param[in] level 日志级别
         * @param[in] event 日志事件
         */
        void log(LogLevel::Level level, LogEvent::ptr event);

        /**
         * @brief 写debug级别日志
         * @param[in] event 日志事件
         */
        void debug(LogEvent::ptr event);

        /**
         * @brief 写info级别日志
         * @param[in] event 日志事件
         */
        void info(LogEvent::ptr event);

        /**
         * @brief 写warn级别日志
         * @param[in] event 日志事件
         */
        void warn(LogEvent::ptr event);

        /**
         * @brief 写error级别日志
         * @param[in] event 日志事件
         */
        void error(LogEvent::ptr event);

        /**
         * @brief 写fatal级别日志
         * @param[in] event 日志事件
         */
        void fatal(LogEvent::ptr event);

        /**
         * @brief 添加日志目标
         * @param[in] appender 日志目标
         */
        void addAppender(LogAppender::ptr appender);

        /**
         * @brief 删除日志目标
         * @param[in] appender 日志目标
         */
        void delAppender(LogAppender::ptr appender);

        /**
         * @brief 清空日志目标
         */
        void clearAppenders();

        /**
         * @brief 返回日志级别
         */
        LogLevel::Level getLevel() const { return m_level; }

        /**
         * @brief 设置日志级别
         */
        void setLevel(LogLevel::Level val) { m_level = val; }

        /**
         * @brief 返回日志名称
         */
        const std::string &getName() const { return m_name; }

        /**
         * @brief 设置日志格式器
         */
        void setFormatter(LogFormatter::ptr val);

        /**
         * @brief 设置日志格式模板
         */
        void setFormatter(const std::string &val);

        /**
         * @brief 获取日志格式器
         */
        LogFormatter::ptr getFormatter();

        /**
         * @brief 将日志器的配置转成YAML String
         */
        std::string toYamlString();

        /**
         * @brief 获得日志名称
         */
        std::string &getName() { return m_name; }

        /**
         * @brief 设置主日志器
         */
        void setRoot(Logger::ptr root) { m_root = root; }
        /**
         * @brief 获得主日志器
         */
        Logger::ptr &getRoot() { return m_root; }

    private:
        /// 日志名称
        std::string m_name;
        /// 日志级别
        LogLevel::Level m_level;
        /// Mutex
        // MutexType m_mutex;
        /// 日志目标集合
        std::list<LogAppender::ptr> m_appenders;
        /// 日志格式器
        LogFormatter::ptr m_formatter;
        /// 主日志器
        Logger::ptr m_root;
    };

    /**
     * @brief 输出到控制台的Appender
     */
    class StdoutLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
        std::string toYamlString() override;
    };

    /**
     * @brief 输出到文件的Appender
     */
    class FileLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string &filename);
        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
        std::string toYamlString() override;

        /**
         * @brief 重新打开日志文件
         * @return 成功返回true
         */
        bool reopen();

    private:
        /// 文件路径
        std::string m_filename;
        /// 文件流
        std::ofstream m_filestream;
        /// 上次重新打开时间
        uint64_t m_lastTime = 0;
    };

    /**
     * @brief 日志器管理类
     */
    class LoggerManager
    {
    public:
        // typedef Spinlock MutexType;
        /**
         * @brief 构造函数
         */
        LoggerManager();

        /**
         * @brief 获取日志器
         * @param[in] name 日志器名称
         */
        Logger::ptr getLogger(const std::string &name);

        /**
         * @brief 初始化
         */
        void init();

        /**
         * @brief 返回主日志器
         */
        Logger::ptr getRoot() const { return m_root; }

        /**
         * @brief 将所有的日志器配置转成YAML String
         */
        std::string toYamlString();

    private:
        /// Mutex
        // MutexType m_mutex;
        /// 日志器容器
        std::map<std::string, Logger::ptr> m_loggers;
        /// 主日志器
        Logger::ptr m_root;
    };

    /// 日志器管理类单例模式
    // LoggerManager LoggerMgr;
    // typedef sylar::Singleton<LoggerManager> LoggerMgr;
}
#endif