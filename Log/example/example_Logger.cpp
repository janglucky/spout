#include "../Log.h"
#include <iostream>

using namespace tensir;

int main()
{
    tensir::Logger::ptr logger(new tensir::Logger);
    logger->addAppender(tensir::LogAppender::ptr(new tensir::StdoutLogAppender));

    tensir::LogEvent::ptr event(new tensir::LogEvent(logger,
                                                     tensir::LogLevel::DEBUG,
                                                     __FILE__,
                                                     __LINE__,
                                                     123,
                                                     0,
                                                     1,
                                                     time(0),
                                                     "main"));
    event->getSS() << "hello tensir log";
    logger->log(tensir::LogLevel::DEBUG, event);
    // std::cout << "hello tensir log" << std::endl;

    TENSIR_LOG_DEBUG(logger) << "test macro";

    LoggerManager LoggerMgr;
    TENSIR_LOG_DEBUG(LoggerMgr.getRoot()) << "hello";
}