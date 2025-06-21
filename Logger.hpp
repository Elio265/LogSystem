#ifndef __MY_LOGGER__
#define __MY_LOGGER__

#include "formatter.hpp"
#include "Level.hpp"
#include "Sink.hpp"
#include <mutex>
#include <atomic>
#include <memory>
#include <stdarg.h>

namespace wzh
{
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;
        Logger(const std::string &logname, Formatter::ptr &formatter, Level::value level, std::vector<Sink::ptr> &sinks)
            : _logname(logname), _formatter(formatter), _level(level), _sinks(sinks.begin(), sinks.end())
        {}

        ~Logger()
        {}

        void debug(const std::string &file, const size_t line, const std::string &fmt, ...)
        {
            if(_level > Level::value::DEBUG) return;
            va_list ap;
            va_start(ap, fmt);
            char *str;
            if(vasprintf(&str, fmt.c_str(), ap) == -1)
            {
                std::cerr << "vasprintf faild\n";
                return;
            }
            va_end(ap);

            Message msg(line, _logname, file, str, Level::value::DEBUG);
            std::stringstream ss;
            _formatter->format(ss, msg);
            log(ss.str().c_str(), ss.str().size());
            free(str);
        }

        void info(const std::string &file, const size_t line, const std::string &fmt, ...)
        {
            if(_level > Level::value::INFO) return;
            va_list ap;
            va_start(ap, fmt);
            char *str;
            if(vasprintf(&str, fmt.c_str(), ap) == -1)
            {
                std::cerr << "vasprintf faild\n";
                return;
            }
            va_end(ap);

            Message msg(line, _logname, file, str, Level::value::INFO);
            std::stringstream ss;
            _formatter->format(ss, msg);
            log(ss.str().c_str(), ss.str().size());
            free(str);
        }

        void warn(const std::string &file, const size_t line, const std::string &fmt, ...)
        {
            if(_level > Level::value::WARN) return;
            va_list ap;
            va_start(ap, fmt);
            char *str;
            if(vasprintf(&str, fmt.c_str(), ap) == -1)
            {
                std::cerr << "vasprintf faild\n";
                return;
            }
            va_end(ap);

            Message msg(line, _logname, file, str, Level::value::WARN);
            std::stringstream ss;
            _formatter->format(ss, msg);
            log(ss.str().c_str(), ss.str().size());
            free(str);
        }

        void error(const std::string &file, const size_t line, const std::string &fmt, ...)
        {
            if(_level > Level::value::ERROR) return;
            va_list ap;
            va_start(ap, fmt);
            char *str;
            if(vasprintf(&str, fmt.c_str(), ap) == -1)
            {
                std::cerr << "vasprintf faild\n";
                return;
            }
            va_end(ap);

            Message msg(line, _logname, file, str, Level::value::ERROR);
            std::stringstream ss;
            _formatter->format(ss, msg);
            log(ss.str().c_str(), ss.str().size());
            free(str);
        }

        void fatal(const std::string &file, const size_t line, const std::string &fmt, ...)
        {
            if(_level > Level::value::FATAL) return;
            va_list ap;
            va_start(ap, fmt);
            char *str;
            if(vasprintf(&str, fmt.c_str(), ap) == -1)
            {
                std::cerr << "vasprintf faild\n";
                return;
            }
            va_end(ap);

            Message msg(line, _logname, file, str, Level::value::FATAL);
            std::stringstream ss;
            _formatter->format(ss, msg);
            log(ss.str().c_str(), ss.str().size());
            free(str);
        }

    protected:
        virtual bool log(const std::string &data, const size_t len) = 0;

    protected:
        std::mutex _mtx;
        std::string _logname;
        Formatter::ptr _formatter;
        std::atomic<Level::value> _level;
        std::vector<Sink::ptr> _sinks;
    };

    class SyncLogger : public Logger
    {
    public:
        SyncLogger(const std::string &logname, Formatter::ptr &formatter, Level::value level, std::vector<Sink::ptr> &sinks)
            : Logger(logname, formatter, level, sinks)
        {}

        ~SyncLogger()
        {}

    protected:
        bool log(const std::string &data, const size_t len)
        {
            std::unique_lock<std::mutex> lock(_mtx);
            if(_sinks.empty()) return true;
            for(auto &sink : _sinks)
            {
                sink->log(data, len);
            }
            return true;
        }
    };
}

#endif