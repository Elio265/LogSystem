#ifndef __MY_FORMATTER__
#define __MY_FORMATTER__

#include "Message.hpp"
#include <memory>
#include <vector>
#include <assert.h>
#include <sstream>
#include <iostream>
namespace wzh
{
    class FormatterItem
    {   
    public:
        using ptr = std::shared_ptr<FormatterItem>;
        virtual bool formatter(std::ostream &out, const Message &msg) = 0;
        virtual ~FormatterItem() {};
    };

    class msgFormatterItem : public FormatterItem
    {
    public:
        msgFormatterItem(const std::string &str = "")
        {}

        bool formatter(std::ostream &out, const Message &msg)
        {
            out << msg._payload;
            return true;
        }
    };

    class levelFormatterIter : public FormatterItem
    {
    public:
        levelFormatterIter(const std::string &str = "")
        {}

        bool formatter(std::ostream &out, const Message &msg)
        {
            out << Level::toString(msg._level);
            return true;
        }
    };

    class nameFormatterItem : public FormatterItem
    {
    public:
        nameFormatterItem(const std::string &str = "")
        {}

        bool formatter(std::ostream &out, const Message &msg)
        {
            out << msg._name;
            return true;
        }
    };

    class threadFormatterItem : public FormatterItem
    {
    public:
        threadFormatterItem(const std::string &str = "")
        {}

        bool formatter(std::ostream &out, const Message &msg)
        {
            out << msg._tid;
            return true;
        }
    };

    class timeFormatterItem : public FormatterItem
    {
    public:
        timeFormatterItem(const std::string &str = "%H:%M:%S")
        : _format(str)
        {
            if(_format.empty()) _format = "%H:%M:%S";
        }

        bool formatter(std::ostream &out, const Message &msg)
        {
            struct tm lt;
            localtime_r(&msg._time, &lt);
            char tmp[128];
            strftime(tmp, 127, _format.c_str(), &lt);
            out << tmp;
            return true;
        }
    private:
        std::string _format;
    };

    class fileFormatterItem : public FormatterItem
    {
    public:
        fileFormatterItem(const std::string &name = "")
        {}

        bool formatter(std::ostream &out, const Message &msg)
        {
            out << msg._file;
            return true;
        }
    };

    class lineFormatterItem : public FormatterItem
    {
    public:
        lineFormatterItem(const std::string &str = "")
        {}

        bool formatter(std::ostream &out, const Message &msg)
        {
            out << msg._line;
            return true;
        }
    };

    class tabFormatterItem : public FormatterItem
    {
    public:
        tabFormatterItem(const std::string &str = "")
        {}

        bool formatter(std::ostream &out, const Message &msg) override
        {
            out << "/t";
            return true;
        }
    };

    class nextFormatterItem : public FormatterItem
    {
    public:
        nextFormatterItem(const std::string &str = "")
        {}

        bool formatter(std::ostream &out, const Message &msg) override
        {
            out << "\n";
            return true;
        }
    };

    class otherFormatterItem : public FormatterItem
    {
    public:
        otherFormatterItem(const std::string &str = "")
            : _str(str)
        {}

        bool formatter(std::ostream &out, const Message &msg)
        {
            out << _str;
            return true;
        }
    private:
        std::string _str;
    };

    class Formatter
    {
    public:
        Formatter(const std::string &pattern = "[%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n")
            : _pattern(pattern)
        {
            assert(parsePattern());
        }

        void format(std::ostream &out, Message &msg)
        {
            for(auto &item : _items)
            {
                item->formatter(out, msg);
            }
        }

        std::string format(Message &msg)
        {
            std::stringstream ss;
            format(ss, msg);
            return ss.str();
        }
    private:
        FormatterItem::ptr createItem(const std::string &key, const std::string &val)
        {
            if(key == "m") return std::make_shared<msgFormatterItem>();
            if(key == "d") return std::make_shared<timeFormatterItem>(val);
            if(key == "t") return std::make_shared<threadFormatterItem>();
            if(key == "c") return std::make_shared<nameFormatterItem>();
            if(key == "f") return std::make_shared<fileFormatterItem>();
            if(key == "l") return std::make_shared<lineFormatterItem>();
            if(key == "p") return std::make_shared<levelFormatterIter>();
            if(key == "T") return std::make_shared<tabFormatterItem>();
            if(key == "n") return std::make_shared<nextFormatterItem>();
            return std::make_shared<otherFormatterItem>(val);
        }

        bool parsePattern()
        {
            std::vector<std::pair<std::string, std::string>> array;
            size_t idx = 0;
            size_t sz = _pattern.size();
            while(idx < sz)
            {
                if(_pattern[idx] == '%')
                {
                    if(++idx < sz)
                    {
                        std::cerr << "The last character of the pattern format cannot be %\n";
                        return false;
                    }
                    if(_pattern[idx] == '%')
                    {
                        array.push_back({"", "%"});
                        ++idx;
                    }
                    else
                    {
                        std::string key, val;
                        key = _pattern[idx++];
                        if(_pattern[idx] == '{')
                        {
                            int pos = _pattern.find_first_of('}', idx);
                            if(pos == std::string::npos)
                            {
                                std::cerr << "The pattern format less a }\n";
                                return false;
                            }
                            val = _pattern.substr(idx + 1, pos - idx);
                            idx = pos + 1;
                        }
                        array.push_back({key, val});
                    }
                }
                else 
                {
                    int pos = _pattern.find_first_of('%', idx);
                    std::string val;
                    if(pos == std::string::npos)
                    {
                        val = _pattern.substr(idx);
                        idx = sz;
                    }
                    else 
                    {
                        val = _pattern.substr(idx, pos - idx);
                        idx = pos + 1;
                    }
                    array.push_back({"", val});
                }
            }

            for(auto &p : array)
            {
                _items.push_back(createItem(p.first, p.second));
            }
            return true;
        }
    private:
        std::string _pattern;
        std::vector<FormatterItem::ptr> _items;
    };
} // namespace wzh



#endif