#include "Buffer.h"

Buffer::Buffer()
{
}

Buffer::~Buffer()
{
}

void Buffer::append(const char *data, size_t size)
{
    buf_.append(data, size);
}

void Buffer::appendwithhead(const char *data, size_t size)
{
    buf_.append((char *)&size, 4);
    buf_.append(data, size);
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