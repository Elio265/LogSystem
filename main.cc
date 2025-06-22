

#include "Util.hpp"
#include "Level.hpp"
#include "Message.hpp"
#include "formatter.hpp"
#include "Sink.hpp"
#include "Logger.hpp"
#include "Buffer.hpp"

void testUtil()
{
    time_t clock = wzh::Util::Date::now();
    std::cout << clock << std::endl;

    std::string pathName = "/home/elio/LogSystem/a/b/c/file.txt";
    if(wzh::Util::File::exist(pathName))
        std::cout << "file exist\n";
    else 
        std::cout << "file not exist\n";

    std::string path = wzh::Util::File::path(pathName);
    std::cout << "file path : " << path << std::endl;
    
    if(wzh::Util::File::createPath(path))
        std::cout << "creat path successful\n";
    else 
        std::cout << "creat path faild\n";
}

void testLevel()
{
    std::cout << wzh::Level::toString(wzh::Level::value::DEBUG) << std::endl;
    std::cout << wzh::Level::toString(wzh::Level::value::WARN) << std::endl;
    std::cout << wzh::Level::toString(wzh::Level::value::ERROR) << std::endl;
    std::cout << wzh::Level::toString(wzh::Level::value::FATAL) << std::endl;
    std::cout << wzh::Level::toString(wzh::Level::value::INFO) << std::endl;
    std::cout << wzh::Level::toString(wzh::Level::value::OFF) << std::endl;
    std::cout << wzh::Level::toString(wzh::Level::value::UNKNOW) << std::endl;
}

void testformat()
{
    wzh::Message msg(15, "root", "main.cc", "testing...", wzh::Level::value::INFO);
    wzh::Formatter fmt("[%d{%Y-%m-%d %H:%M:%S}][%t][%p][%c][%f:%l] %m%n");
    // fmt.format(std::cout, msg);
    //wzh::Formatter fmt;
    std::string str = fmt.format(msg);
    wzh::Sink::ptr stdoutsink = wzh::SinkFactory::create<wzh::StdoutSink>();
    wzh::Sink::ptr filesink = wzh::SinkFactory::create<wzh::FileSink>("filesink.log");
    wzh::Sink::ptr rollsink = wzh::SinkFactory::create<wzh::RollSinkBySize>("./logs/base-", 1024 * 1024);

    stdoutsink->log(str, str.size());
    filesink->log(str, str.size());

    for(int i = 0; i < 1024 * 1024 * 10; i += str.size())
    {
        rollsink->log(str, str.size());
    }
    //std::cout << str << std::endl;
}

void testSink()
{
    std::string str = "hello world\n";

    wzh::StdoutSink outsink;
    outsink.log(str, str.size());

    wzh::FileSink filesink("filelog.log");
    filesink.log(str, str.size());

    wzh::RollSinkBySize rollfile("./base", 1024 * 1024);
    rollfile.log(str, str.size());
}

void testlogger()
{
    // std::string loggerName = "sync_logger";
    // wzh::Level::value limit = wzh::Level::value::WARN;
    // wzh::Formatter::ptr fmt(new wzh::Formatter("[%d{%Y-%m-%d %H:%M:%S}][%t][%p][%c][%f:%l] %m%n"));
    // wzh::Sink::ptr stdoutsink = wzh::SinkFactory::create<wzh::StdoutSink>();
    // wzh::Sink::ptr filesink = wzh::SinkFactory::create<wzh::FileSink>("filesink.log");
    // wzh::Sink::ptr rollsink = wzh::SinkFactory::create<wzh::RollSinkBySize>("./logs/base-", 1024 * 1024);
    // std::vector<wzh::Sink::ptr> sinks = {stdoutsink, filesink, rollsink};
    // wzh::Logger::ptr logger(new wzh::SyncLogger(loggerName, fmt, limit, sinks));

    // std::unique_ptr<wzh::LoggerBuilder> builder(new wzh::LocalBuildLogger());
    // builder->buildLoggerLevel(wzh::Level::value::WARN);
    // builder->buildLoggerName("Async_logger");
    // builder->buildLoggerMatter("[%d{%Y-%m-%d %H:%M:%S}][%t][%p][%c][%f:%l] %m%n");
    // builder->buildLoggerType(wzh::LogType::AsyncLogger);
    // // builder->buildEnableUnsafe();
    // builder->buildSink<wzh::StdoutSink>();
    // builder->buildSink<wzh::FileSink>("./logs/test.log");
    // wzh::Logger::ptr logger = builder->build();

    wzh::Logger::ptr logger = wzh::LoggerManager::getInstance().getLogger("Async_logger");
    logger->debug(__FILE__, __LINE__, "%s", "测试日志...");
    logger->info(__FILE__, __LINE__, "%s", "测试日志...");
    logger->warn(__FILE__, __LINE__, "%s", "测试日志...");
    logger->error(__FILE__, __LINE__, "%s", "测试日志...");
    logger->fatal(__FILE__, __LINE__, "%s", "测试日志...");

    // int count = 0;
    for(int i = 0; i < 200000; i++)
    {
        logger->fatal(__FILE__, __LINE__, "测试日志...%d", i);
    }
}

void testBuffer()
{
    std::ifstream ifs;
    ifs.open("./logs/test.log", std::ios::binary);
    if(ifs.is_open() == false) 
    {
        std::cout << "open faild\n";
        return ;
    }

    ifs.seekg(0, std::ios::end);
    size_t fsize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::cout << fsize << std::endl;

    std::string str;
    str.resize(fsize);
    ifs.read(&str[0], fsize);
    if(ifs.good() == false) 
    {
        std::cout << "read faild\n";
        return ;
    }
    ifs.close();

    wzh::Buffer buffer;
    for(int i = 0; i < fsize; i++)
    {
        buffer.push(&str[i], 1);
    }
    std::cout << buffer.readAbleSize() << std::endl;

    std::ofstream ofs;
    ofs.open("./logs/tmp.log", std::ios::binary);
    size_t sz = buffer.readAbleSize();
    for(int i = 0; i < sz; i++)
    {
        ofs.write(buffer.readBegin(), 1);
        buffer.moveReader(1);
    }
    ofs.close();
}

int main()
{

    std::unique_ptr<wzh::LoggerBuilder> builder(new wzh::GlobalBuildLogger());
    builder->buildLoggerLevel(wzh::Level::value::WARN);
    builder->buildLoggerName("Async_logger");
    builder->buildLoggerMatter("[%d{%Y-%m-%d %H:%M:%S}][%t][%p][%c][%f:%l] %m%n");
    builder->buildLoggerType(wzh::LogType::AsyncLogger);
    // builder->buildEnableUnsafe();
    builder->buildSink<wzh::StdoutSink>();
    builder->buildSink<wzh::FileSink>("./logs/test.log");
    builder->build();

    // testBuffer();
    testlogger();
    // testLevel();
    // testUtil();
    // testformat();
    // testSink();
    return 0;
}
