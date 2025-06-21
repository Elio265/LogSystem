/*
日志消息类用来描述一条日志消息 
*/


#ifndef __MY_MESSAGE__
#define __MY_MESSAGE__

#include "Level.hpp"
#include "Util.hpp"
#include <thread>

namespace wzh
{
    struct Message
    {
        size_t _line;
        time_t _time;
        std::thread::id _tid;
        std::string _name; //日志器名称
        std::string _file;
        std::string _payload;
        Level::value _level;
        
        Message(const size_t line, const std::string &name, const std::string &file, 
            const std::string &payload, const Level::value level)
        : _line(line), _time(Util::Date::now()), _tid(std::this_thread::get_id()), 
          _name(name), _file(file), _payload(payload), _level(level)
        {

        }
    };    
} // namespace wzh

#endif