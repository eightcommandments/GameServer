#include "Log.h"
#include <iostream>
#include <tuple>
#include <time.h>


namespace Kaiye {

//=========================LogLevel============================

LogEvent::LogEvent(const char* file, int line, LogLevel::Level level, std::string ss)
    :m_file(file)
    ,m_line(line)
    ,m_level(level)
    ,m_ss(ss) {
}
    

std::string LogLevel::ToString(LogLevel::Level level) {
    switch (level)
    {
#define XX(name)\
        case Level::name:\
            return #name;\
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
    return "UNKNOWN"; 
}

void LogEvent::Format(const char* fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    this->Format(fmt, arg);
    va_end(arg);

}

void LogEvent::Format(const char* fmt, va_list arg) {
    char* buffer = nullptr;
    int len = vasprintf(&buffer, fmt, arg);
    if (len != -1)
    {
        m_ss = std::string(buffer, len);
    }
}


//=========================Logger==============================

Logger::Logger(std::string name) : m_name(name), m_level(LogLevel::Level::UNKNOWN) {
}

void Logger::Log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level)
    {
        for (auto iter = m_appenders.begin(); iter != m_appenders.end(); iter++)
        {
            (*iter)->Log(level, event);
        }
    }
}

void Logger::Debug(LogEvent::ptr event) {
    Log(LogLevel::Level::DEBUG, event);
}

void Logger::Info(LogEvent::ptr event) {
    Log(LogLevel::Level::DEBUG, event);
}

void Logger::Warn(LogEvent::ptr event) {
    Log(LogLevel::Level::DEBUG, event);
}

void Logger::Error(LogEvent::ptr event) {
    Log(LogLevel::Level::DEBUG, event);
}

void Logger::Fatal(LogEvent::ptr event) {
    Log(LogLevel::Level::DEBUG, event);
}

void Logger::AddLogAppender(LogAppender::ptr appender) {
    m_appenders.push_back(appender);
}

void Logger::RemoveLogAppender(LogAppender::ptr appender) {
    for (auto iter = m_appenders.begin(); iter != m_appenders.end(); iter++)
    {
        if (*iter == appender)
        {
            m_appenders.erase(iter);
            break;
        }
    }
}

//==========================LogEventWrap=======================================
LogEventWrap::LogEventWrap(Logger::ptr logger, LogEvent::ptr event)
    : m_logger(logger),  m_event(event) {
}

LogEventWrap::~LogEventWrap() {
    m_logger->Log(m_event->GetLogLevel(), m_event);
}


//==========================LogAppender========================================
LogAppender::LogAppender() : m_level(LogLevel::Level::UNKNOWN) {
}

LogAppender::~LogAppender() {
}

void StdoutLogAppender::Log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level)
    {
        m_formatter->SetLogLevel(level);
        std::cout << m_formatter->Format(event);
    }
}

FileLogAppender::FileLogAppender(const std::string& filename)
    : m_filename(filename) {

}

bool FileLogAppender::Reopne() {
    if (m_filestream)
    {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
}

void FileLogAppender::Log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level)
    {
        m_formatter->SetLogLevel(level);
        m_filestream << m_formatter->Format(event);
    }
}

//===========================LogFormatter=======================================

LogFormatter::LogFormatter(std::string& format)
    : m_format(format) {
}

LogFormatter::~LogFormatter() {
}

/*
* 格式模式翻译器
*/
class PatternConverter {
public:
    typedef std::shared_ptr<PatternConverter> ptr;
    PatternConverter() {}
    virtual ~PatternConverter(){}
    virtual void Format(std::stringstream& ss, LogEvent::ptr event) = 0;
};

/*
* 字符串格式翻译器
*/
class StringPatternConverter : public PatternConverter {
public:
    typedef std::shared_ptr<StringPatternConverter> ptr;
    StringPatternConverter(std::string str)
        : m_string(str) {} 

    void Format(std::stringstream& ss, LogEvent::ptr event) {
        ss << m_string;
    }
private:
    std::string m_string;
};

/*
* 基础字符格式翻译器
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
class BasePatternConverter : public PatternConverter {
public:
    typedef std::shared_ptr<BasePatternConverter> ptr;
    BasePatternConverter(std::string str)
        :m_string(str) {}
    ~BasePatternConverter() {}
    void Format(std::stringstream& ss, LogEvent::ptr event) override;
private:
    std::string m_string;
};

/*
* 时间格式翻译器
*/
class DateTimePatternConverter : public PatternConverter {
public:
    typedef std::shared_ptr<DateTimePatternConverter> ptr;
    DateTimePatternConverter(std::string str)
        :m_string(str) {}
    ~DateTimePatternConverter() {}
    void Format(std::stringstream& ss, LogEvent::ptr event) override;
private:
    std::string m_string;
};

/*
* BasePatternConverter
*/
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
void BasePatternConverter::Format(std::stringstream& ss, LogEvent::ptr event) {
    switch(m_string.front())
    {
        case 'f':
            ss << event->GetFile();
            return;
        
        case 'l':
            ss << event->GetLine();
            return;
        
        case 'm':
            ss << event->GetMessage();
            return;
        
        case 'n':
            ss << "\n";
            return;

        case 'p':
            ss << LogLevel::ToString(event->GetLogLevel());
            return;

        case 't':
            ss << event->GetThreadId();
            return;   

        case 'd':
            ss << event->GetTime();
            return;

        case 'F':
            ss << event->GetFiberId();
            return;

        case 'N':
            ss << event->GetElapse();
            return;
        default:
            std::cout << "BasePatternConverter::Format ERROR!" << std::endl;
            return;          
    }
}

/*
* DateTimePatternConverter
*/
void DateTimePatternConverter::Format(std::stringstream& ss, LogEvent::ptr event) {
    char buffer[80];
    struct tm* info;
    time_t rawtime;

    time(&rawtime);
    info = localtime(&rawtime);
    // std::cout << "text time :" << rawtime << " " << m_string << "=" << m_string.size() << "====" << std::endl; 
    size_t err = strftime(buffer, sizeof(buffer), m_string.c_str(), info);
    if (err <= 0)
    {
        std::cout << "DateTimePatternConverter::Format ERROR!" << std::endl;
        return;
    }
    ss << buffer;
    return;
}


PatternLogFormatter::PatternLogFormatter(std::string& format)
    : LogFormatter(format) {
    Init();
}

PatternLogFormatter::~PatternLogFormatter() {
    for (auto iter : m_patternConverters)
    {
        delete iter;
    }
}

// %x %x{} %%
void PatternLogFormatter::Init() {
    if (m_format.empty())
    {
        std::cout << "PatternLogFormatter Init ERROR!" << std::endl;
    }
    int pos = 0;
    // 标记 内容 类别
    std::vector<std::tuple<char, std::string, int> > vec;
    std::string tempStr;
    while (pos < m_format.size())
    {
        char c = m_format[pos++];
        if (c == '%')
        {
            if (pos >= m_format.size())
            {
                std::cout << "PatternLogFormatter::Init not pos." << std::endl;
                return;
            }
            c = m_format[pos++];
            if (!tempStr.empty())
            {
                vec.push_back(std::make_tuple('T', tempStr, 3));
                tempStr.clear();
            }
            switch (c)
            {
                case '%':
                {
                    tempStr.push_back(c);
                    break;
                }
                case 'd':
                {
                    if (pos >= m_format.size() || m_format[pos] != '{')
                    {
                        vec.push_back(std::make_tuple(c, "%Y-%m-%d %H:%M:%S", 2));
                        break;
                    }
                    std::string::size_type tar = m_format.find_first_of("}", pos);
                    if (tar == std::string::npos)
                    {
                        vec.push_back(std::make_tuple(c, "%Y-%m-%d %H:%M:%S", 2));
                        std::cout << "PatternLogFormatter Init ERROR! not fount }." << std::endl;
                        break;
                    }
                    vec.push_back(std::make_tuple(c, m_format.substr(pos, (int)tar - pos), 2));
                    pos = (int)tar + 1;
                    break;
                }
                default:
                {
                    vec.push_back(std::make_tuple(c, "", 1));
                    break;
                }
            }
        }
        else
        {
            tempStr.push_back(c);
        }
    }

    //创建PatternConverter类，从为后续格式转换准备
    for (auto& item : vec)
    {
        int status = std::get<2>(item);
        // std::cout << "=== text " << std::get<0>(item) << " " << std::get<1>(item) << " " << std::get<2>(item) << "===" << std::endl;
        if (status == 1)
        {
            char c = std::get<0>(item);
            std::string str;
            str.push_back(c);
            m_patternConverters.push_back(new BasePatternConverter(str));
        }
        else if (status == 2)
        {
            std::string str = std::get<1>(item);
            m_patternConverters.push_back(new DateTimePatternConverter(str));
        }
        else if (status == 3)
        {
            std::string str = std::get<1>(item);
            m_patternConverters.push_back(new StringPatternConverter(str));
        }
    }

    std::cout << "PatternLogFormatter good!" << std::endl;

}

std::string PatternLogFormatter::Format(LogEvent::ptr event) {
    event->SetLogLevel(GetLogLevel());
    std::stringstream ss;
    for (auto& pattern : m_patternConverters)
    {
        pattern->Format(ss, event);
    }
    return ss.str();
}

}