#pragma once
#include <string>
#include <iostream>

class Buffer
{
private:
    std::string buf_;

public:
    Buffer();
    ~Buffer();

    void append(const char *data, size_t size);
    void appendwithhead(const char *data, size_t size);
    void erase(size_t pos, size_t len);
    size_t size() const;
    const char *data() const;
    void clear();
};
