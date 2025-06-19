#include "Util.hpp"

int main()
{
    time_t clock = wzh::Util::Date::now();
    std::cout << clock << std::endl;

    std::string pathName = "./file.txt";
    if(wzh::Util::File::exist(pathName))
        std::cout << "file exist\n";
    else 
        std::cout << "file not exist\n";

    std::string path = wzh::Util::File::path(pathName);
    std::cout << "file path : " << path << std::endl;
    
    if(wzh::Util::File::creatPath(pathName))
        std::cout << "creat path successful\n";
    else 
        std::cout << "creat path faild\n";
    return 0;
}
