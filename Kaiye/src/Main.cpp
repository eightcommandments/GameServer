#include "Log.h"

int main()
{
    Kaiye::Logger::ptr logger(new Kaiye::Logger());
    Kaiye::LogAppender::ptr logAppenderPtr(new Kaiye::StdoutLogAppender());
    std::string str("[%d]%f - %l : %m %n");
    Kaiye::LogFormatter::ptr logFormatterPtr(new Kaiye::PatternLogFormatter(str));
    logAppenderPtr->SetFormatter(logFormatterPtr);
    logger->AddLogAppender(logAppenderPtr);

    // str = "hello world!";
    // Kaiye::LogEvent::ptr eventPtr(new Kaiye::LogEvent("Main.cpp", 11, str));

    // logger.Debug(eventPtr);

    KAIYE_LOG_SS_DEBUG(logger) << "hello world!" << "kaiye";
    KAIYE_LOG_FMT_DEBUG(logger, "%s %d", "hello world!", 110);
}