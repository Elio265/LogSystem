/*
    通用功能类，与业务无关的功能实现
        1. 获取系统时间
        2. 获取文件大小
        3. 创建目录
        4. 获取文件所在目录
*/

#ifndef __MY_UTIL__
#define __MY_UTIL__
#include <iostream>
#include <time.h>
#include <string>
#include <sys/stat.h>

namespace wzh
{
    namespace Util
    {
        class Date
        {
        public:
            static time_t now() 
            {
                return (time_t)time(nullptr);
            }
        };        
        
        class File
        {
        public:
            // 检查文件是否存在
            static bool exist(const std::string &path_and_name)
            {
                struct stat st;
                return stat(path_and_name.c_str(), &st) == 0;
            }

            // 返回文件路径
            static std::string path(const std::string &path_and_name)
            {
                if(path_and_name.empty()) return ".";
                int pos = path_and_name.find_last_of("/\\");
                if(pos == std::string::npos) return ".";
                return path_and_name.substr(0, pos + 1);
            }

            // 创建目录
            static bool creatPath(const std::string &path_and_name)
            {
                if(path_and_name.empty() || exist(path_and_name)) return true;
                size_t idx = 0;
                while(idx < path_and_name.size())
                {
                    size_t pos = path_and_name.find_first_of("/\\", idx);
                    if(pos == std::string::npos) return true;
                    std::string substr = path_and_name.substr(0, pos);
                    if(idx == pos || substr == "." || substr == ".." || exist(substr)) 
                    {
                        idx = pos + 1;
                        continue;
                    }
                    if(mkdir(substr.c_str(), 0755))
                    {
                        std::cout << "mkdir faild\n"; // TODO
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