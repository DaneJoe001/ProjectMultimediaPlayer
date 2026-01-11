#include "service/time_service.hpp"

TimeService::TimeService()
{

}
void TimeService::add_audio_time_us(int64_t us)
{
    m_audio_time_us.fetch_add(us);
}
int64_t TimeService::get_audio_time_us() const
{
    return m_audio_time_us.load();
}

void TimeService::reset_audio_time()
{
    m_audio_time_us = 0;
}