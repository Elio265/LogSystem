

// #include "Util.hpp"
// #include "Level.hpp"
// #include "Message.hpp"
#include "formatter.hpp"

// void testUtil()
// {
//     time_t clock = wzh::Util::Date::now();
//     std::cout << clock << std::endl;

//     std::string pathName = "/home/elio/LogSystem/a/b/c/file.txt";
//     if(wzh::Util::File::exist(pathName))
//         std::cout << "file exist\n";
//     else 
//         std::cout << "file not exist\n";

//     std::string path = wzh::Util::File::path(pathName);
//     std::cout << "file path : " << path << std::endl;
    
//     if(wzh::Util::File::createPath(path))
//         std::cout << "creat path successful\n";
//     else 
//         std::cout << "creat path faild\n";
// }

// void testLevel()
// {
//     std::cout << wzh::Level::toString(wzh::Level::value::DEBUG) << std::endl;
//     std::cout << wzh::Level::toString(wzh::Level::value::WARN) << std::endl;
//     std::cout << wzh::Level::toString(wzh::Level::value::ERROR) << std::endl;
//     std::cout << wzh::Level::toString(wzh::Level::value::FATAL) << std::endl;
//     std::cout << wzh::Level::toString(wzh::Level::value::INFO) << std::endl;
//     std::cout << wzh::Level::toString(wzh::Level::value::OFF) << std::endl;
//     std::cout << wzh::Level::toString(wzh::Level::value::UNKNOW) << std::endl;
// }

void testformat()
{
    wzh::Message msg(15, "root", "main.cc", "testing...", wzh::Level::value::INFO);
    wzh::Formatter fmt;
    std::string str = fmt.format(msg);
    std::cout << str << std::endl;


}

int main()
{
    // testLevel();
    // testUtil();
    testformat();
    return 0;
}
