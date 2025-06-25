#ifndef __MY_LOGGER__
#define __MY_LOGGER__

#include "Formatter.hpp"
#include "Level.hpp"
#include "Sink.hpp"
#include "Looper.hpp"

#include <mutex>
#include <atomic>
#include <memory>
#include <stdarg.h>
#include <unordered_map>


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

        const std::string& name() { return _logname; }

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
            log(ss.str(), ss.str().size());
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
        bool log(const std::string &data, const size_t len) override
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

    class AsyncLogger : public Logger
    {
    public:
        AsyncLogger(const std::string &logname, Formatter::ptr &formatter, 
            Level::value level, std::vector<Sink::ptr> &sinks, AsyncType type)
            : Logger(logname, formatter, level, sinks), _looper(std::make_shared<AsyncLooper>(std::bind(&AsyncLogger::realLog, this, std::placeholders::_1), type))
        {}

        bool log(const std::string &data, const size_t len) override
        {
            _looper->push(data, len);
            return true;
        }

        void realLog(Buffer &buffer)
        {
            if(_sinks.empty()) return;
            for(auto &sink : _sinks)
            {
                sink->log(buffer.readBegin(), buffer.readAbleSize());
            }
        }
    private:
        AsyncLooper::ptr _looper;
    };

    enum class LogType
    {
        AsyncLogger,
        SyncLogger
    };

    class LoggerBuilder
    {
    public:
        LoggerBuilder() : _loggerType(LogType::SyncLogger), _level(Level::value::DEBUG), _asyncType(AsyncType::ASYNC_SAFE)
        {}

        ~LoggerBuilder()
        {}

        void buildLoggerName(const std::string &name) { _loggerName = name; }
        void buildLoggerType(const LogType type) { _loggerType = type; }
        void buildEnableUnsafe() { _asyncType = AsyncType::ASYNC_UNSAFE; }
        void buildLoggerLevel(Level::value level) { _level = level; }
        void buildLoggerMatter(const std::string &pattern) { _formatter = std::make_shared<Formatter>(pattern); }

        template<typename SinkType, typename ...Args>
        void buildSink(Args &&...args)
        {
            Sink::ptr sink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
            _sinks.push_back(sink);
        }

        virtual Logger::ptr build() = 0;

    protected:
        AsyncType _asyncType;
        LogType _loggerType;
        std::string _loggerName;
        Level::value _level;
        Formatter::ptr _formatter;
        std::vector<Sink::ptr> _sinks;
    };

    class LocalBuildLogger : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(!_loggerName.empty());
            if(_formatter.get() == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }
            if(_sinks.empty())
            {
                buildSink<StdoutSink>();
            }
            if(_loggerType == LogType::AsyncLogger)
            {
                return std::make_shared<AsyncLogger>(_loggerName, _formatter, _level, _sinks, _asyncType);
            }
            return std::make_shared<SyncLogger>(_loggerName, _formatter, _level, _sinks);
        }
    };


    class LoggerManager
    {
    public:
        static LoggerManager &getInstance()
        {
            static LoggerManager _eton;
            return _eton;
        }

        bool addLogger(Logger::ptr &logger)
        {
            if(hasLogger(logger->name())) return true;
            std::unique_lock<std::mutex> lock(_mtx);
            _loggers.insert({logger->name(), logger});
            return true;
        }

        bool hasLogger(const std::string &name)
        {
            return _loggers.find(name) != _loggers.end();
        }

        Logger::ptr getLogger(const std::string &loggerName)
        {
            if(hasLogger(loggerName)) return _loggers[loggerName];
            return nullptr;
        }

        Logger::ptr rootLogger()
        {
            return getLogger("root");
        }
    private:
        LoggerManager()
        {
            std::unique_ptr<wzh::LoggerBuilder> builder(new wzh::LocalBuildLogger());
            builder->buildLoggerName("root");
            _root_logger = builder->build();
            _loggers.insert({"root", _root_logger});
        }
    private:
        std::mutex _mtx;
        Logger::ptr _root_logger;
        std::unordered_map<std::string, Logger::ptr> _loggers;
    };

    class GlobalBuildLogger : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(!_loggerName.empty());
            if(_formatter.get() == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }
            if(_sinks.empty())
            {
                buildSink<StdoutSink>();
            }
            Logger::ptr logger;
            if(_loggerType == LogType::AsyncLogger)
            {
                logger = std::make_shared<AsyncLogger>(_loggerName, _formatter, _level, _sinks, _asyncType);
            }
            else 
            {
                logger = std::make_shared<SyncLogger>(_loggerName, _formatter, _level, _sinks);
            }
            LoggerManager::getInstance().addLogger(logger);
            return logger;
        }
    };
}

#endif