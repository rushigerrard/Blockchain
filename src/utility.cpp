#include "utils.h"
#include "logger.h"
#include<openssl/sha.h>
#include<unistd.h>
#include<ctime>

//using namespace std;

Logger *log;
std::string sha256(std::string s)
{
    char outputBuffer[65];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, s.c_str(), s.length());
    SHA256_Final(hash, &sha256);
    int i = 0;
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
    std::string str(outputBuffer);
    return str;
}
template <typename T>
Logger& operator<<(Logger& h, T const& t)
{
   h.out1_ << t;
   h.out2_ << t;
   return h;
}

Logger& operator<<(Logger& h, std::ostream&(*f)(std::ostream&))
{
   h.out1_ << f;
   h.out2_ << f;
   return h;
}

unsigned long timer() {
    struct timespec currentTime;
    if (clock_gettime(CLOCK_MONOTONIC, &currentTime)) {
        return 0;
    }
    return currentTime.tv_sec*NANOSECONDS_IN_A_SECOND + currentTime.tv_nsec;
}

void create_logger(std::ostream& out1, std::ostream& out2) {
    log = new Logger(out1, out2);
}

void log_info(std::string message) {
    // std::cout << "[ INFO ] " << message << std::endl;
    *log << "[ INFO ] " << message << std::endl;
}

void log_error(std::string message) {
    *log << "[ ERROR ] " << message << std::endl;
}

void log_debug(std::string message) {
    // std::cout << "[ DEBUG ] " << message << std::endl;
    *log << "[ DEBUG ] " << message << std::endl;
}

