#ifndef __MY_LOOPER__
#define __MY_LOOPER__

#include "Buffer.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace wzh
{
    using Function = std::function<void(Buffer&)>;
    enum class AsyncType
    {
        ASYNC_SAFE,
        ASYNC_UNSAFE
    };

    class AsyncLooper
    {
    public:
        using ptr = std::shared_ptr<AsyncLooper>;
        AsyncLooper(const Function &callback, AsyncType type = AsyncType::ASYNC_SAFE) 
            : _type(type), _callbackFunc(callback), _stop(false), _thread(&AsyncLooper::threadEntry, this)
        {}

        ~AsyncLooper() { stop(); }

        void stop()
        {
            _stop = true;
            _consumer_cond.notify_all();
            _thread.join();
        }

        void push(const std::string &data, const size_t len)
        {
            std::unique_lock<std::mutex> lock(_mtx);
            if(_type == AsyncType::ASYNC_SAFE)
                _product_cond.wait(lock, [&](){ return _product_buf.writeAbleSize() >= len; });
            _product_buf.push(data.c_str(), len);
            _consumer_cond.notify_all();
        }

    private:
        void threadEntry()
        {
            while (true)
            {
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    if(_stop && _product_buf.empty()) break;

                    _consumer_cond.wait(lock, [&](){ return _stop || !_product_buf.empty(); });
                    _consumer_buf.swap(_product_buf);
                    _product_cond.notify_all();
                }
                _callbackFunc(_consumer_buf);
                _consumer_buf.reset();
            }
        }
    private:
        Function _callbackFunc;

    private:
        AsyncType _type;
        bool _stop;
        Buffer _product_buf;
        Buffer _consumer_buf;
        std::mutex _mtx;
        std::condition_variable _product_cond;
        std::condition_variable _consumer_cond;
        std::thread _thread;
    };
} // namespace wzh


#endif