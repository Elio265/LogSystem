#ifndef __MY_SINK__
#define __MY_SINK__

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include "Util.hpp"
namespace wzh
{
    class Sink
    {
    public:
        using ptr = std::shared_ptr<Sink>;
        Sink() {}
        virtual ~Sink() {}
        virtual bool log(const std::string &data, size_t len) = 0;
    };

    class StdoutSink : public Sink
    {
    public:
        StdoutSink() {}
        ~StdoutSink() {}

        bool log(const std::string &data, size_t len) override
        {
            std::cout.write(data.c_str(), len);
            return true;
        }
    };

    class FileSink : public Sink
    {
    public:
        FileSink(const std::string &filename)
            : _filename(filename)   
        {
            if(Util::File::createPath(Util::File::path(_filename)) == false)
            {
                std::cerr << "create file path faild, file sink faild, file name is : " << _filename << std::endl;
                abort();
            }
            _ofs.open(_filename, std::ios::binary | std::ios::app);
            if(_ofs.is_open() == false)
            {
                std::cerr << "open file faild, file name is : " << _filename << std::endl;
                abort();
            }
        }

        ~FileSink() 
        {
            _ofs.close();
        }

        bool log(const std::string &data, size_t len) override
        {
            _ofs.write(data.c_str(), len);
            if(_ofs.good() == false)
            {
                std::cerr << "write file is faild, file name is : " << _filename << std::endl;
                return false;
            }
            return true;
        }

    private:
        std::ofstream _ofs;
        std::string _filename;
    };

    class RollSinkBySize : public Sink
    {
    public:
        RollSinkBySize(const std::string &basename, const size_t maxsize = 1024 * 1024)
            : _basename(basename), _maxsize(maxsize), _cursize(0), _count(0)
        {
            if(Util::File::createPath(Util::File::path(_basename)) == false)
            {
                std::cerr << "create roll file path faild, roll file base name is : " << _basename << std::endl;
                abort();
            }
            // init();
        }

        ~RollSinkBySize()
        {
            _ofs.close();
        }

        bool log(const std::string &data, size_t len) override
        {
            if(init() == false)
            {
                std::cerr << "roll sink init faild\n";
                return false;
            }

            _ofs.write(data.c_str(), len);
            if(_ofs.good() == false)
            {
                std::cerr << "write roll file faild, file name is : " << _curfilename << std::endl;
                return false;
            }
            _cursize += len;
            return true;
        }

    private:
        bool init()
        {
            if(_ofs.is_open() == false || _cursize >= _maxsize)
            {
                if(_ofs.is_open()) _ofs.close();
                _curfilename = getRollfileName();
                ++_count;
                _ofs.open(_curfilename, std::ios::binary | std::ios::app);
                if(_ofs.is_open() == false)
                {
                    std::cerr << "init open file faild, file name is : " << _curfilename << std::endl;
                    return false;
                }
                _cursize = 0;
            }
            return true;
        }

        std::string getRollfileName()
        {
            std::stringstream ss;
            ss << _basename;
            ss << Util::Date::dateTime("%Y-%m-%d_%H:%M:%S");
            ss << "_" + std::to_string(_count);
            ss << ".log";
            return ss.str();
        }

    private:
        std::string _basename;
        std::string _curfilename;
        std::ofstream _ofs;
        size_t _maxsize;
        size_t _count;
        size_t _cursize;
    };

    class SinkFactory
    {
    public:
        template<typename SinkType, typename ...Args>
        static Sink::ptr create(Args &&...args)
        {
            return std::make_shared<SinkType>(std::forward<Args>(args)...);
        }
    };
}

#endif