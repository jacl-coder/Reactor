#include <time.h>

#include "Timestamp.h"


/*
class Timestamp
{
private:
    time_t secsinceepoch_;
public:
    Timestamp();
    Timestamp(int64_t secsinceepoch);

    static Timestamp now();

    time_t toint() const;
    std::string toString() const;
};
*/

Timestamp::Timestamp()
{
    secsinceepoch_ = time(NULL);
}

Timestamp::Timestamp(int64_t secsinceepoch): secsinceepoch_(secsinceepoch)
{
}

Timestamp Timestamp::now()
{
    return Timestamp();
}

time_t Timestamp::toint() const
{
    return secsinceepoch_;
}

std::string Timestamp::tostring() const
{
    char buf[128]= {0};
    tm *tm = localtime(&secsinceepoch_);
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
             tm->tm_hour, tm->tm_min, tm->tm_sec);
    return buf;
}

/*
#include <unistd.h>
#include <iostream>

int main()
{
    Timestamp t1;
    std::cout << t1.tostring() << std::endl;

    Timestamp t2(1000);
    std::cout << t2.tostring() << std::endl;

    sleep(1);
    std::cout<< Timestamp::now().toint() << std::endl;
    std::cout << Timestamp::now().tostring() << std::endl;

    return 0;
}
*/
