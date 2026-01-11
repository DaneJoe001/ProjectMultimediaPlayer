#pragma once

#include <cstdint>
#include <atomic>

class TimeService
{
public:
    TimeService();
    void add_audio_time_us(int64_t us);
    int64_t get_audio_time_us() const;
    void reset_audio_time();
private:
    std::atomic<int64_t> m_audio_time_us = 0;
};