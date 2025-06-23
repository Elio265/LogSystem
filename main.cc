
#include "Log.hpp"

void bench(const std::string &name, size_t tcount, size_t msgCount, size_t msgLen)
{
    wzh::Logger::ptr logger = wzh::getLogger(name);
    if(logger.get() == nullptr) return;

    std::cout << "测试日志: " << msgCount << "条， 总大小: " << (msgCount * msgLen) / 1024 << "KB\n";

    std::string msg(msgCount - 1, 'a');
    std::vector<std::thread> threads;
    std::vector<double> costArray(tcount);
    size_t msg_thread_count = msgCount / tcount;
    for(int i = 0; i < tcount; i++)
    {
        threads.emplace_back([&, i](){
            auto start = std::chrono::high_resolution_clock::now();
            for(int j = 0; j < msg_thread_count; j++)
            {
                logger->fatal("%s", msg);
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> cost = end - start;
            costArray[i] = cost.count();
            std::cout << "tid = " << i << "\n\t output : " << msg_thread_count << "\n\t cost time : " << costArray[i] << "s\n";
        });
    }
    for(int i = 0; i < tcount; i++)
    {
        threads[i].join();
    }

    double maxCost = costArray[0];
    for(int i = 0; i < tcount; i++) maxCost = std::max(maxCost, costArray[i]);
    size_t msg_pre_sec = msgCount / maxCost;
    size_t size_pre_sec = (msgCount * msgLen) / (maxCost * 1024);

    std::cout << "每秒输出日志数量：" << msg_pre_sec << "条\n";
    std::cout << "每秒输出日志大小：" << size_pre_sec << "KB\n";
}

void syncBench()
{
    std::unique_ptr<wzh::LoggerBuilder> builder(new wzh::GlobalBuildLogger());
    builder->buildLoggerName("sync_logger");
    builder->buildLoggerMatter("[%d{%Y-%m-%d %H:%M:%S}][%t][%p][%c][%f:%l] %m%n");
    builder->buildLoggerType(wzh::LogType::SyncLogger);
    builder->buildSink<wzh::FileSink>("./logs/test.log");
    builder->build();
    bench("sync_logger", 1, 1000000, 100);
}

void asyncBench()
{
    std::unique_ptr<wzh::LoggerBuilder> builder(new wzh::GlobalBuildLogger());
    builder->buildLoggerName("async_logger");
    builder->buildLoggerMatter("[%d{%Y-%m-%d %H:%M:%S}][%t][%p][%c][%f:%l] %m%n");
    builder->buildEnableUnsafe();
    builder->buildLoggerType(wzh::LogType::AsyncLogger);
    builder->buildSink<wzh::FileSink>("./logs/test.log");
    builder->build();
    bench("async_logger", 3, 1000000, 100);
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

    // wzh::Logger::ptr logger = wzh::LoggerManager::getInstance().getLogger("Async_logger");
    DEBUG("%s", "测试日志...");
    INFO("%s", "测试日志...");
    WARN("%s", "测试日志...");
    ERROR("%s", "测试日志...");
    FATAL("%s", "测试日志...");

    // int count = 0;
    for(int i = 0; i < 200000; i++)
    {
        FATAL("测试日志...%d", i);
    }
}

int main()
{
    // testlogger();
    // syncBench();
    asyncBench();
    return 0;
}
