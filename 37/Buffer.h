#pragma once
#include <string>
#include <iostream>
#include <cstring>
#include <cstdint>

class Buffer
{
private:
    std::string buf_;
    const uint16_t sep_;

public:
    Buffer(uint16_t sep = 1);
    ~Buffer();

    void append(const char *data, size_t size);
    void appendwithsep(const char *data, size_t size);
    void erase(size_t pos, size_t len);
    size_t size() const;
    const char *data() const;
    void clear();
    bool pickmessage(std::string &msg);
};
