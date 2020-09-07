#ifndef _KAIYE_LOG_H_
#define _KAIYE_LOG_H_

#include <string>
#include <memory>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <stdio.h>
#include <stdarg.h>


#define KAIYE_LOG_SS(logger, level) \
    if (logger->GetLogLevel() <= level) \
        Kaiye::LogEventWrap(logger, \
            Kaiye::LogEvent::ptr(new Kaiye::LogEvent(__FILE__, __LINE__, level, ""))).GetSS()


#define KAIYE_LOG_SS_DEBUG(logger) KAIYE_LOG_SS(logger, Kaiye::LogLevel::Level::DEBUG)
#define KAIYE_LOG_SS_INFO(logger) KAIYE_LOG_SS(logger, Kaiye::LogLevel::Level::INFO)
#define KAIYE_LOG_SS_WARN(logger) KAIYE_LOG_SS(logger, Kaiye::LogLevel::Level::WARN)
#define KAIYE_LOG_SS_ERROR(logger) KAIYE_LOG_SS(logger, Kaiye::LogLevel::Level::ERROR)
#define KAIYE_LOG_SS_FATAL(logger) KAIYE_LOG_SS(logger, Kaiye::LogLevel::Level::FATAL)

#define KAIYE_LOG_FMT(logger, level, fmt, ...) \
    if (logger->GetLogLevel() <= level) \
        Kaiye::LogEventWrap(logger, \
            Kaiye::LogEvent::ptr(new Kaiye::LogEvent(__FILE__, __LINE__, level, ""))).GetLogEvent()->Format(fmt, __VA_ARGS__)

#define KAIYE_LOG_FMT_DEBUG(logger, fmt, ...) KAIYE_LOG_FMT(logger, Kaiye::LogLevel::Level::DEBUG, fmt, __VA_ARGS__)
#define KAIYE_LOG_FMT_INFO(logger, fmt, ...) KAIYE_LOG_FMT(logger, Kaiye::LogLevel::Level::INFO, fmt, __VA_ARGS__)
#define KAIYE_LOG_FMT_WARN(logger, fmt, ...) KAIYE_LOG_FMT(logger, Kaiye::LogLevel::Level::WARN, fmt, __VA_ARGS__)
#define KAIYE_LOG_FMT_ERROR(logger, fmt, ...) KAIYE_LOG_FMT(logger, Kaiye::LogLevel::Level::ERROR, fmt, __VA_ARGS__)
#define KAIYE_LOG_FMT_FATAL(logger, fmt, ...) KAIYE_LOG_FMT(logger, Kaiye::LogLevel::Level::FATAL, fmt, __VA_ARGS__)


namespace Kaiye {

/*
* 日志级别
*/
class LogLevel {
public:
    enum Level {
        UNKNOWN = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static std::string ToString(LogLevel::Level level);
};

/*
* 日志事件
*/
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(const char* file, int line, LogLevel::Level level, std::string m_ss);

    /*
    * 获取文件名
    */
    const char* GetFile() const { return m_file; }

    /*
    * 获取行号
    */
    int GetLine() const { return m_line; }

    /*
    * 获取程序启动到现在毫秒数
    */
    long GetElapse() const { return m_elapse; }

    /*
    * 获取时间戳
    */
    long GetTime() const { return m_time; }

    /*
    * 获取线程id
    */
    int GetThreadId() const { return m_threadId; }

    /*
    * 获取线程id
    */
    int GetFiberId() const { return m_fiberId; }

    /*
    * 获取字符串原始信息
    */
    std::string GetMessage() const { return m_ss.empty() ? m_sstream.str() : m_ss; }

    /*
    * 获取流式原始信息
    */
    std::stringstream& GetStreamMessage() { return m_sstream; }

    /*
    * 获取日志级别
    */
    LogLevel::Level GetLogLevel() const { return m_level; }

    /*
    * 设置日志级别
    */
    void SetLogLevel(LogLevel::Level level) { m_level = level; }

    void Format(const char* fmt, ...);
    void Format(const char* fmt, va_list arg);

private:
    const char* m_file = nullptr;           //文件命
    int m_line = 0;                         //行号
    long m_elapse = 0;                      //程序启动到现在的毫秒数
    long m_time = 0;                        //当前时间戳
    int m_threadId = 0;                     //线程id
    int m_fiberId = 0;                      //协程id
    std::string m_ss;                       //原始信息
    std::stringstream m_sstream;            //流式原始信息
    LogLevel::Level m_level;                //日志级别
};

/*
* 用于指定日志输出的格式。每个Appender需要设置一个LogFormatter。
*/
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(std::string& format);
    virtual ~LogFormatter();

    virtual std::string Format(LogEvent::ptr event) = 0;

    void SetLogLevel(LogLevel::Level level) { m_level = level; }
    LogLevel::Level GetLogLevel() const { return m_level; }

protected:
    std::string m_format;
    LogLevel::Level m_level;
};

/*
* 日子输出器 用于指定内容输出位置（如：控制台，文件、远程服务器等）
*/
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    LogAppender();
    virtual ~LogAppender();
    virtual void Log(LogLevel::Level level, LogEvent::ptr event) = 0;
    
    void SetLogLevel(LogLevel::Level level) { m_level = level; }
    LogLevel::Level GetLogLevel() const { return m_level; }

    void SetFormatter(LogFormatter::ptr format) { m_formatter = format; }
    LogFormatter::ptr GetFormatter() const { return m_formatter; }

protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};


/*
* 记录日志的句柄
*/
class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;
    Logger(std::string name = "root");
    void Log(LogLevel::Level level, LogEvent::ptr event);

    void Debug(LogEvent::ptr event);
    void Info(LogEvent::ptr event);
    void Warn(LogEvent::ptr event);
    void Error(LogEvent::ptr event);
    void Fatal(LogEvent::ptr event);

    void AddLogAppender(LogAppender::ptr appender);
    void RemoveLogAppender(LogAppender::ptr appender);

    void SetLogLevel(LogLevel::Level level) { m_level = level; }
    LogLevel::Level GetLogLevel() const { return m_level; }

private:
    std::string m_name;                         //日志名称
    LogLevel::Level m_level;                    //日志级别
    std::list<LogAppender::ptr> m_appenders;    //日志输出器列表
};


/*
* 日志事件包装器
*/
class LogEventWrap {
public:
    LogEventWrap(Logger::ptr logger, LogEvent::ptr event);
    ~LogEventWrap();
    
    std::stringstream& GetSS() { return m_event->GetStreamMessage(); }
    LogEvent::ptr GetLogEvent() { return m_event; }

private:
    Logger::ptr m_logger;
    LogEvent::ptr m_event;
};



class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void Log(LogLevel::Level level, LogEvent::ptr event) override;
};

class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void Log(LogLevel::Level level, LogEvent::ptr event) override;


    /*
    * 重新打开文件输出流 返回true表示成功
    */
    bool Reopne();

private:
    std::string m_filename;
    std::ofstream m_filestream;
};


/*
* 自定义日志输出模式，采用类C输出格式
* %% - 转义% 输出%
* %f – 输出当前记录器所在的文件名称，比如”main.cpp”
* %l – 输出当前记录器所在的文件行号，比如”51”
* %m – 输出原始信息。
* %n – 换行符。
* %p – 输出LogLevel，比如”DEBUG”
* %t – 输出记录器所在的线程ID，比如 “1075298944”
* %d - 输出当前时间
* %F - 输出协程id
* %N - 输出程序启动到现在的毫秒数
*/
class PatternConverter;
class PatternLogFormatter : public LogFormatter {
public:
    PatternLogFormatter(std::string& format);
    ~PatternLogFormatter();
    virtual std::string Format(LogEvent::ptr event) override;

    void Init();

private:
    std::vector<PatternConverter*> m_patternConverters;     //转换器列表
};

};

#endif