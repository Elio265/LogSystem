#ifndef __MY__BUFFER__
#define __MY__BUFFER__

#include <vector>
#include <cstddef>
#include <assert.h>
#include <iostream>
namespace wzh
{
    #define MAX_SIZE (1024 * 1024 * 10)
    #define DOUBLE_GROW_SIZE (1024 * 1024 * 80)
    #define INCREMENT_GROW_SIZE (1024 * 1024 * 2)
    class Buffer
    {
    public:
        Buffer() : _buffer(MAX_SIZE), _readerIdx(0), _writeIdx(0)
        {}
        ~Buffer()
        {}

        bool push(const char *data, const size_t len)
        {
            makeEnoughSize(len);
            std::copy(data, data + len, &_buffer[_writeIdx]);
            moveWriter(len);
            return true;
        }

        const char *readBegin() { return &_buffer[_readerIdx]; }
        size_t readAbleSize() { return _writeIdx - _readerIdx; }
        size_t writeAbleSize() { return _buffer.size() - _writeIdx; }

        void moveWriter(const size_t len)
        {
            assert((_writeIdx + len) < _buffer.size());
            _writeIdx += len;
        }

        void moveReader(const size_t len)
        {
            assert((_readerIdx + len) <= _writeIdx);
            _readerIdx += len;
        }

        void reset()
        {
            _readerIdx = 0;
            _writeIdx = 0;
        }

        void swap(Buffer &buffer)
        {
            _buffer.swap(buffer._buffer);
            std::swap(_readerIdx, buffer._readerIdx);
            std::swap(_writeIdx, buffer._writeIdx);
        }

        bool empty()
        {
            return _readerIdx == _writeIdx;
        }
    
    private:
        void makeEnoughSize(const size_t len)
        {
            if(len <= writeAbleSize()) return;
            int new_size = 0;
            if(_buffer.size() < DOUBLE_GROW_SIZE)
            {
                new_size = _buffer.size() * 2;
            }
            else 
            {
                new_size = _buffer.size() + INCREMENT_GROW_SIZE;
            }
            _buffer.resize(new_size);
        }
    private:
        std::vector<char> _buffer;
        size_t _readerIdx;
        size_t _writeIdx;
    };
} // namespace wzh


#endif