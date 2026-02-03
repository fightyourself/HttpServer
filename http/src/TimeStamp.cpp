#include "TimeStamp.h"

TimeStamp::TimeStamp():
microSecondsSinceEpoch_(std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
).count()){}

TimeStamp TimeStamp::now(){ return TimeStamp(); }

int64_t TimeStamp::to_micro_seconds() const{
    return microSecondsSinceEpoch_;
}

std::chrono::microseconds TimeStamp::operator-(const TimeStamp& rhs)const{
    return std::chrono::microseconds(this->microSecondsSinceEpoch_-
        rhs.microSecondsSinceEpoch_);
}

std::string TimeStamp::to_string()const{
    auto duration = std::chrono::microseconds(microSecondsSinceEpoch_);
    auto tp = TimePoint(duration);

    // 将 time_point 转换为 time_t (秒) 和 微秒部分
    auto total_duration = tp.time_since_epoch();
    auto seconds_part = std::chrono::duration_cast<std::chrono::seconds>(total_duration);
    auto subseconds_part = total_duration - seconds_part;

    time_t time_t_time = seconds_part.count();
    int microseconds_part = static_cast<int>(subseconds_part.count());

    struct tm timeinfo;
    #ifdef _WIN32
        localtime_s(&timeinfo, &time_t_time); // Windows
    #else
        localtime_r(&time_t_time, &timeinfo); // POSIX
    #endif

    // 缓冲区，长度足以容纳 "YYYY-MM-DD HH:MM:SS.mmmuuu\0" (26 + 1 chars)
    char buffer[30];
    // 先格式化日期时间部分
    int len = snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
                        timeinfo.tm_year + 1900,
                        timeinfo.tm_mon + 1,
                        timeinfo.tm_mday,
                        timeinfo.tm_hour,
                        timeinfo.tm_min,
                        timeinfo.tm_sec);

    // 再追加微秒部分
    snprintf(buffer + len, sizeof(buffer) - len, ".%06d", microseconds_part);

    return std::string(buffer);
}

TimeStamp::~TimeStamp(){}