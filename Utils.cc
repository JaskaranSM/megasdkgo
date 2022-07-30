#include <string>
#include <unistd.h>


bool IsMegaFolder(const char* link) 
{
    std::string mlink = link;
    return mlink.find("folder") != std::string::npos || mlink.find("#F!") != std::string::npos;
}

std::string GenerateRandomID(const int len) 
{
    std::string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    srand( (unsigned) time(NULL) * getpid());
    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i) 
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    return tmp_s;
}