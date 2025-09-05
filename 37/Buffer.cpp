#include "Buffer.h"

Buffer::Buffer(uint16_t sep) : sep_(sep)
{
}

Buffer::~Buffer()
{
}

void Buffer::append(const char *data, size_t size)
{
    buf_.append(data, size);
}

void Buffer::appendwithsep(const char *data, size_t size)
{
    if (sep_ == 0)
    {
        buf_.append(data, size);
    }
    else if (sep_ == 1)
    {
        buf_.append((char *)&size, 4);
        buf_.append(data, size);
    }
    // todo: more sep_ cases
}

void Buffer::erase(size_t pos, size_t len)
{
    buf_.erase(pos, len);
}

size_t Buffer::size() const
{
    return buf_.size();
}

const char *Buffer::data() const
{
    return buf_.data();
}

void Buffer::clear()
{
    buf_.clear();
}

bool Buffer::pickmessage(std::string &msg)
{
    if (buf_.size() == 0)
        return false;

    if (sep_ == 0)
    {
        msg = buf_;
        buf_.clear();
    }
    else if (sep_ == 1)
    {
        int len;
        memcpy(&len, buf_.data(), 4);
        if (buf_.size() < (size_t)(4 + len))
            return false;

        msg = buf_.substr(4, len);
        buf_.erase(0, 4 + len);
    }

    return true;
}

/*
int main()
{
    std::string msg = "9999999999999";
    Buffer buf(1);
    buf.appendwithsep(msg.data(), msg.size());
    std::string out;
    buf.pickmessage(out);
    printf("out: %s\n", out.c_str());
}
*/