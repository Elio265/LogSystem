#ifndef __MY_LEVEL__
#define __MY_LEVEL__

namespace wzh
{
    class Level
    {
    public:
        enum class value
        {
            UNKNOW = 0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            OFF
        };

        // 将错误级别转换为字符串
        static const char* toString(const Level::value val)
        {
            switch (val)
            {
            case Level::value::DEBUG:
                return "DEBUG";
            case Level::value::ERROR:
                return "ERROR";
            case Level::value::FATAL:
                return "FATAL";
            case Level::value::INFO:
                return "INFO";
            case Level::value::OFF:
                return "OFF";     
            case Level::value::WARN:
                return "WARN";
            default:
            return "UNKNOW";
            }
        }
    };
}


#endif