#ifndef __MY_LOG__
#define __MY_LOG__

#include "Logger.hpp"

namespace wzh
{
    Logger::ptr getLogger(const std::string &name)
    {
        return wzh::LoggerManager::getInstance().getLogger(name);
    }

    Logger::ptr getRootLogger()
    {
        return wzh::LoggerManager::getInstance().rootLogger();
    }

    #define debug(fmt, ...) debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define info(fmt, ...) info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define warn(fmt, ...) warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define error(fmt, ...) error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define fatal(fmt, ...) fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

    #define DEBUG(fmt, ...) wzh::getRootLogger()->debug(fmt, ##__VA_ARGS__)
    #define INFO(fmt, ...) wzh::getRootLogger()->info(fmt, ##__VA_ARGS__)
    #define WARN(fmt, ...) wzh::getRootLogger()->warn(fmt, ##__VA_ARGS__)
    #define ERROR(fmt, ...) wzh::getRootLogger()->error(fmt, ##__VA_ARGS__)
    #define FATAL(fmt, ...) wzh::getRootLogger()->fatal(fmt, ##__VA_ARGS__)
}

#endif