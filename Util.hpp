/*
    通用功能类，与业务无关的功能实现
        1. 获取系统时间
        2. 获取文件大小
        3. 创建目录
        4. 获取文件所在目录
*/
#ifndef __MY_UTIL__
#define __MY_UTIL__

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <iostream>
#include <time.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <chrono>
#include <sstream>

namespace wzh
{
    namespace Util
    {
        class Date
        {
        public:
            static time_t now() 
            {
                return time(nullptr);
            }

            static std::string dateTime(const std::string &format = "%Y-%m-%d %H:%M:%S", time_t timestamp = now())
            {
                struct tm lt;
                localtime_r(&timestamp, &lt);
                char tmp[128];
                strftime(tmp, 127, format.c_str(), &lt);
                std::stringstream ss;
                ss << tmp;
                return ss.str();
            }
        };        
        
        class File
        {
        public:
            // 检查文件是否存在
            static bool exist(const std::string &pathname)
            {
                struct stat st;
                return stat(pathname.c_str(), &st) == 0;
            }

            // 返回文件路径
            static std::string path(const std::string &pathname)
            {
                if(pathname.empty()) return ".";
                int pos = pathname.find_last_of("/\\");
                if(pos == std::string::npos) return ".";
                return pathname.substr(0, pos + 1);
            }

            // 创建目录 
            static bool createPath(const std::string &pathname)
            {
                if(pathname.empty() || exist(pathname)) return true;
                size_t idx = 0;
                while(idx < pathname.size())
                {
                    size_t pos = pathname.find_first_of("/\\", idx);
                    if(pos == std::string::npos) 
                    {
                        if(mkdir(pathname.c_str(), 0755))
                        {
                            std::cerr << "mkdir failed for path: " << pathname << " errno: " << strerror(errno) << "\n";
                            return false;
                        }
                        return true;
                    }
                    std::string substr = pathname.substr(0, pos);
                    if(idx == pos || substr == "." || substr == ".." || exist(substr)) 
                    {
                        idx = pos + 1;
                        continue;
                    }
                    if(mkdir(substr.c_str(), 0755))
                    {
                        std::cerr << "mkdir failed for path: " << substr << " errno: " << strerror(errno) << "\n";
                        return false;
                    }
                    idx = pos + 1;
                }
                return true;
            }
        };
    } // namespace Util
    
} // namespace wzh




#endif