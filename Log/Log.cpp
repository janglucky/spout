#include "Log.h"
#include <time.h>

namespace tensir
{
    const char *LogLevel::toString(LogLevel::Level level)
    {
        switch (level)
        {

#define XX(name)         \
    case LogLevel::name: \
        return #name;    \
        break;

            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
#undef XX
        default:
            return "UNKNOWN";
        }
    }

    LogLevel::Level LogLevel::fromString(const std::string &str)
    {
#define XX(level, v)            \
    if (str == #v)              \
    {                           \
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
#undef XX

        return LogLevel::UNKNOWN;
    }

    LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char *filename,
                       uint32_t line, uint32_t elapse, uint32_t thread_id,
                       uint32_t fiber_id, uint64_t time, const std::string &thread_name)
        : m_filename(filename),
          m_line(line),
          m_elapse(elapse),
          m_threadId(thread_id),
          m_fiberId(fiber_id),
          m_time(time),
          m_threadName(thread_name),
          m_logger(logger),
          m_level(level)
    {
    }

    void LogEvent::format(const char *fmt, ...)
    {
        va_list al;        // 定义可变参数列表指针
        va_start(al, fmt); // 初始化可变参数指针为第一个参数的地址
        format(fmt, al);
        va_end(al); // 清空可变参数指针
    }

    void LogEvent::format(const char *fmt, va_list al)
    {
        char *buf = nullptr;
        int len = vasprintf(&buf, fmt, al);

        if (len != -1)
        {
            m_ss << std::string(buf);
            free(buf);
        }
    }

    LogEventWrapper::LogEventWrapper(LogEvent::ptr e)
        : m_event(e)
    {
    }

    LogEventWrapper::~LogEventWrapper()
    {
        // m_event->getLogger()->log(m_event->getLevel(), m_event);
    }

    std::stringstream &LogEventWrapper::getSS()
    {
        return m_event->getSS();
    }

    LogFormatter::LogFormatter(const std::string &pattern)
        : m_pattern(pattern)
    {
        init();
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        std::stringstream ss;
        for (auto &i : m_items)
        {
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }

    std::ostream &LogFormatter::format(std::ostream &ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        for (auto &i : m_items)
        {
            i->format(ofs, logger, level, event);
        }
        return ofs;
    }

    class MessageFormatItem : public LogFormatter::FormatItem
    {
    public:
        MessageFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem
    {
    public:
        LevelFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << LogLevel::toString(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem
    {
    public:
        ElapseFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getElapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem
    {
    public:
        NameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getLogger()->getName();
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadIdFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getThreadId();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        FiberIdFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFiberId();
        }
    };

    class ThreadNameFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadNameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            std::cout<< "#"<<event->getThreadName()<<std::endl;
            os << event->getThreadName();
        }
    };

    class DateTimeFormatItem : public LogFormatter::FormatItem
    {
    public:
        DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S")
            : m_format(format)
        {
            if (m_format.empty())
            {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
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

    class FilenameFormatItem : public LogFormatter::FormatItem
    {
    public:
        FilenameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFilename();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem
    {
    public:
        LineFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getLine();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem
    {
    public:
        NewLineFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << std::endl;
        }
    };

    class StringFormatItem : public LogFormatter::FormatItem
    {
    public:
        StringFormatItem(const std::string &str)
            : m_string(str) {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << m_string;
        }

    private:
        std::string m_string;
    };

    class TabFormatItem : public LogFormatter::FormatItem
    {
    public:
        TabFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << "\t";
        }

    private:
        std::string m_string;
    };

    void LogFormatter::init()
    {

        /**
         * 1. 从左往右扫描，第一个不为%的，用nstr存储普通字符
         * 2. 碰到第一个%，开始解析，如果下一个为%，当前认定为普通字符，直接append到nstr，否则，进入3
         * 3. 从%下一个字符开始，直到解析出非alpha，得到当前的格式化类型标记（如d，T等）
         * 4. 如果当前格式化类型标记紧跟{则认为是格式化模式，例如日期模式{%Y-%m-%d %H:%M:%S}，开始解析日期，否则，直接加入到vec
         * 5. 对于需要解析日期模式的，需要使用fmt_status标记是否结束扫描{}，fmt_begin记录{位置，来提取出%Y-%m-%d %H:%M:%S
         * 6. 遍历vec三元组，得到最终的格式项列表，每一个项根据实际情况解析
         */

        //str, format, type
        std::vector<std::tuple<std::string, std::string, int> > vec;
        std::string nstr; // 非格式化串（普通字符串）
        for (size_t i = 0; i < m_pattern.size(); ++i)
        {
            /**
             * 开始两个if吃掉m_pattern前面不需要格式化的普通字符串（非%开始的所有字符串）,并保存在nstr中，
             */
            if (m_pattern[i] != '%') // 为经过检查的第一个字符，如果不为%直接解析为普通字符
            {
                nstr.append(1, m_pattern[i]);
                continue;
            }

            if ((i + 1) < m_pattern.size() && m_pattern[i + 1] == '%') // 两个%%相邻表示转义，解析为一个%
            {
                nstr.append(1, '%');
                continue;
            }

            /**
             * 遇到%，且%后面为可解析item（不为%），开始做解析
             */
            size_t n = i + 1;
            int fmt_status = 0;   // {}在本程序中为特殊字符，必须成对出现，该flag标记{}是否出错
            size_t fmt_begin = 0; // item格式开始位置（ "%d{}" 为 '{' 的位置）

            std::string str; // item标记（d、T等）
            std::string fmt; // item格式（仅针对日期才有，其他为空）
            while (n < m_pattern.size())
            {
                if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')) //解析除日期格式解析的其他项如%T
                {

                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }
                if (fmt_status == 0)
                {
                    if (m_pattern[n] == '{') // item格式开始
                    {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        fmt_status = 1; //解析item格式
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                }
                else if (fmt_status == 1)
                {
                    if (m_pattern[n] == '}') // item格式结束
                    {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if (n == m_pattern.size())
                {
                    if (str.empty())
                    {
                        str = m_pattern.substr(i + 1);
                    }
                }
            }

            if (fmt_status == 0)
            {
                if (!nstr.empty()) // 开始有普通串，直接加入到vec中，标记为0
                {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1)); // 需要解析的字符串，标记为1
                i = n - 1;
            }
            else if (fmt_status == 1) // {}必须成对存在，否则报错
            {
                m_error = true;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }

        if (!nstr.empty())
        {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }
        static std::map<std::string, std::function<FormatItem::ptr(const std::string &str)> > s_format_items = {
#define XX(str, C)                                                               \
    {                                                                            \
#str, [](const std::string &fmt) { return FormatItem::ptr(new C(fmt)); } \
    }

            XX(m, MessageFormatItem),    //m:消息
            XX(p, LevelFormatItem),      //p:日志级别
            XX(r, ElapseFormatItem),     //r:累计毫秒数
            XX(c, NameFormatItem),       //c:日志名称
            XX(t, ThreadIdFormatItem),   //t:线程id
            XX(n, NewLineFormatItem),    //n:换行
            XX(d, DateTimeFormatItem),   //d:时间
            XX(f, FilenameFormatItem),   //f:文件名
            XX(l, LineFormatItem),       //l:行号
            XX(T, TabFormatItem),        //T:Tab
            XX(F, FiberIdFormatItem),    //F:协程id
            XX(N, ThreadNameFormatItem), //N:线程名称
#undef XX
        };

        for (auto &i : vec)
        {
            if (std::get<2>(i) == 0)
            {
                m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            }
            else
            {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end())
                {
                    m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    m_error = true;
                }
                else
                {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }

            //std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
        }
    }

    void LogAppender::setFormatter(LogFormatter::ptr val)
    {
        // MutexType::Lock lock(m_mutex);
        m_formatter = val;
        if (m_formatter)
        {
            m_hasFormatter = true;
        }
        else
        {
            m_hasFormatter = false;
        }
    }

    LogFormatter::ptr LogAppender::getFormatter()
    {
        // MutexType::Lock lock(m_mutex);
        return m_formatter;
    }

    Logger::Logger(const std::string &name)
        : m_name(name),
          m_level(LogLevel::DEBUG) // 默认日志级别为DEBUG
    {
        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    }

    void Logger::setFormatter(LogFormatter::ptr val)
    {
        // MutexType::Lock lock(m_mutex);
        m_formatter = val;

        for (auto &i : m_appenders)
        {
            // MutexType::Lock ll(i->m_mutex);
            if (!i->hasFormatter()) // 没有设置日志格式器的添加日志格式器
            {
                i->setFormatter(m_formatter);
            }
        }
    }

    void Logger::setFormatter(const std::string &val)
    {
        std::cout << "---" << val << std::endl;
        LogFormatter::ptr new_val(new LogFormatter(val));
        if (new_val->isError())
        {
            std::cout << "Logger setFormatter name=" << m_name
                      << " value=" << val << " invalid formatter"
                      << std::endl;
            return;
        }
        //m_formatter = new_val;
        setFormatter(new_val);
    }

    std::string Logger::toYamlString()
    {
        // MutexType::Lock lock(m_mutex);
        // YAML::Node node;
        // node["name"] = m_name;
        // if (m_level != LogLevel::UNKNOW)
        // {
        //     node["level"] = LogLevel::ToString(m_level);
        // }
        // if (m_formatter)
        // {
        //     node["formatter"] = m_formatter->getPattern();
        // }

        // for (auto &i : m_appenders)
        // {
        //     node["appenders"].push_back(YAML::Load(i->toYamlString()));
        // }
        // std::stringstream ss;
        // ss << node;
        // return ss.str();
        return "";
    }

    void Logger::addAppender(LogAppender::ptr appender)
    {
        // MutexType::Lock lock(m_mutex);
        if (!appender->getFormatter())
        {
            // MutexType::Lock ll(appender->m_mutex);
            appender->setFormatter(m_formatter);
        }
        m_appenders.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender)
    {
        // MutexType::Lock lock(m_mutex);
        for (auto it = m_appenders.begin();
             it != m_appenders.end(); ++it)
        {
            if (*it == appender)
            {
                m_appenders.erase(it);
                break;
            }
        }
    }

    void Logger::clearAppenders()
    {
        // MutexType::Lock lock(m_mutex);
        m_appenders.clear();
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            auto self = shared_from_this();
            // MutexType::Lock lock(m_mutex);
            if (!m_appenders.empty())
            {
                for (auto &i : m_appenders)
                {
                    i->log(self, level, event);
                }
            }
            else if (m_root)
            {
                m_root->log(level, event);
            }
        }
    }

    void Logger::debug(LogEvent::ptr event)
    {
        log(LogLevel::DEBUG, event);
    }

    void Logger::info(LogEvent::ptr event)
    {
        log(LogLevel::INFO, event);
    }

    void Logger::warn(LogEvent::ptr event)
    {
        log(LogLevel::WARN, event);
    }

    void Logger::error(LogEvent::ptr event)
    {
        log(LogLevel::ERROR, event);
    }

    void Logger::fatal(LogEvent::ptr event)
    {
        log(LogLevel::FATAL, event);
    }

    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            // MutexType::Lock lock(m_mutex);
            m_formatter->format(std::cout, logger, level, event);
        }
    }

    std::string StdoutLogAppender::toYamlString()
    {
        // MutexType::Lock lock(m_mutex);
        // YAML::Node node;
        // node["type"] = "StdoutLogAppender";
        // if (m_level != LogLevel::UNKNOW)
        // {
        //     node["level"] = LogLevel::ToString(m_level);
        // }
        // if (m_hasFormatter && m_formatter)
        // {
        //     node["formatter"] = m_formatter->getPattern();
        // }
        // std::stringstream ss;
        // ss << node;
        // return ss.str();
        return "";
    }

    LoggerManager::LoggerManager()
    {
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

        m_loggers[m_root->getName()] = m_root;

        init();
    }

    Logger::ptr LoggerManager::getLogger(const std::string &name)
    {
        // MutexType::Lock lock(m_mutex);
        auto it = m_loggers.find(name);
        if (it != m_loggers.end())
        {
            return it->second;
        }

        Logger::ptr logger(new Logger(name));
        logger->setRoot(m_root);
        m_loggers[name] = logger;
        return logger;
    }

    std::string LoggerManager::toYamlString()
    {
        // MutexType::Lock lock(m_mutex);
        // YAML::Node node;
        // for (auto &i : m_loggers)
        // {
        //     node.push_back(YAML::Load(i.second->toYamlString()));
        // }
        // std::stringstream ss;
        // ss << node;
        // return ss.str();
        return "";
    }

    void LoggerManager::init()
    {
    }

}