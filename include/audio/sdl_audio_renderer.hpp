#pragma once

#include <string>
#include <atomic>
#include <memory>

#include <SDL2/SDL.h>

#include <danejoe/concurrent/container/mpmc_bounded_queue.hpp>

#include "codec/av_frame_ptr.hpp"
#include "service/time_service.hpp"

class SDLAudioSystem
{
public:
    SDLAudioSystem();
    ~SDLAudioSystem();
private:
    static std::atomic<int> m_init_times;
};

class SDLAudioRenderer
{
public:
    SDLAudioRenderer();
    ~SDLAudioRenderer();
    void play(const std::string& file_path);
    void start();
    void stop();
    void append_pcm(AVFramePtr frame);
    void append_pcm(uint8_t* pcm, int size);
    void set_time_service(std::shared_ptr<TimeService> time_service);
    int64_t get_bytes_per_second()const;
private:
    std::shared_ptr<TimeService> m_time_service = nullptr;
    SDLAudioSystem m_audio_system;
    DaneJoe::MpmcBoundedQueue<uint8_t> m_pcm_buffer = DaneJoe::MpmcBoundedQueue<uint8_t>(8600);;
    SDL_AudioDeviceID m_device_id = 0;

    int m_audio_frequency = 48000;
    int m_audio_format = AUDIO_S16SYS;
    int m_audio_channels = 2;

    std::atomic<bool> m_is_playing = false;
};