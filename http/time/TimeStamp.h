#pragma once
#include <string>
#include <chrono>
#include <iostream>

class TimeStamp{
private:
    using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;
    int64_t microSecondsSinceEpoch_;
public:
    static TimeStamp now();
    TimeStamp();
    int64_t to_micro_seconds() const;
    std::string to_string() const;
    std::chrono::microseconds operator-(const TimeStamp&rhs) const;
    ~TimeStamp();
};