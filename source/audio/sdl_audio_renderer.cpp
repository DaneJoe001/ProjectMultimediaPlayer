#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cstring>

#include <SDL_audio.h>
#include <danejoe/logger/logger_manager.hpp>

extern "C"
{
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libavutil/mem.h>
#include <libavutil/common.h>
#include <libswresample/swresample.h>
}

#include "audio/sdl_audio_renderer.hpp"

std::atomic<int> SDLAudioSystem::m_init_times = 0;
SDLAudioSystem::SDLAudioSystem()
{
    // 获取操作之前的原子计数
    int prev = m_init_times.fetch_add(1, std::memory_order_acq_rel);
    if (prev == 0)
    {
        int check_video_init = SDL_InitSubSystem(SDL_INIT_AUDIO);
        if (check_video_init < 0)
        {
            DANEJOE_LOG_ERROR("default", "SDLAudioSystem", "SDL_Init failed:{}", SDL_GetError());
            // 操作失败时减少原子计数
            m_init_times.fetch_sub(1, std::memory_order_acq_rel);
            throw std::runtime_error("SDL_Init failed!");
        }
    }
}

SDLAudioSystem::~SDLAudioSystem()
{
    int prev = m_init_times.fetch_sub(1, std::memory_order_acq_rel);
    if (prev == 1)
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
}

SDLAudioRenderer::SDLAudioRenderer()
{}
SDLAudioRenderer::~SDLAudioRenderer()
{
    stop();
    m_pcm_buffer.close();
}
void SDLAudioRenderer::play(const std::string& file_path)
{
    SDL_AudioSpec wav_spec;
    SDL_zero(wav_spec);
    Uint8* wav_buffer = nullptr;
    Uint32 wav_length = 0;

    if (!SDL_LoadWAV(file_path.c_str(), &wav_spec, &wav_buffer, &wav_length))
    {
        DANEJOE_LOG_ERROR("default", "SDLAudioRenderer", "SDL_LoadWAV failed:{}", SDL_GetError());
        throw std::runtime_error("SDL_LoadWAV failed!");
    }

    SDL_AudioSpec obtained_spec;
    SDL_zero(obtained_spec);
    auto device_id = SDL_OpenAudioDevice(nullptr, 0, &wav_spec, &obtained_spec, 0);
    if (device_id == 0)
    {
        DANEJOE_LOG_ERROR("default", "SDLAudioRenderer", "SDL_OpenAudioDevice failed:{}", SDL_GetError());
        SDL_FreeWAV(wav_buffer);
        throw std::runtime_error("SDL_OpenAudioDevice failed!");
    }

    if (SDL_QueueAudio(device_id, wav_buffer, wav_length) != 0)
    {
        DANEJOE_LOG_ERROR("default", "SDLAudioRenderer", "SDL_QueueAudio failed:{}", SDL_GetError());
        SDL_CloseAudioDevice(device_id);
        SDL_FreeWAV(wav_buffer);
        throw std::runtime_error("SDL_QueueAudio failed!");
    }

    SDL_PauseAudioDevice(device_id, 0);
    while (SDL_GetQueuedAudioSize(device_id) > 0)
    {
        SDL_Delay(10);
    }

    SDL_CloseAudioDevice(device_id);
    SDL_FreeWAV(wav_buffer);

}

void SDLAudioRenderer::start()
{
    stop();
    auto audio_callback = [](void* userdata, Uint8* stream, int len)
        {
            SDL_memset(stream, 0, len);
            auto* self = static_cast<SDLAudioRenderer*>(userdata);
            if (!self->m_is_playing.load())
            {
                return;
            }
            auto data = self->m_pcm_buffer.try_pop(len);
            int copy_size = static_cast<int>(data.size());

            if (copy_size > len)
            {
                copy_size = len;
            }
            if (self->m_is_playing.load() && copy_size > 0)
            {
                int64_t us = static_cast<int64_t>(copy_size) * 1000 * 1000 / self->get_bytes_per_second();
                self->m_time_service->add_audio_time_us(us);
                std::memcpy(stream, data.data(), copy_size);
            }
        };

    SDL_AudioSpec desired_spec;
    SDL_zero(desired_spec);
    desired_spec.freq = m_audio_frequency;
    desired_spec.format = m_audio_format;
    desired_spec.channels = m_audio_channels;
    desired_spec.samples = 256;
    desired_spec.callback = audio_callback;
    desired_spec.userdata = this;


    m_device_id = SDL_OpenAudioDevice(nullptr, 0, &desired_spec, nullptr, 0);
    if (m_device_id == 0)
    {
        DANEJOE_LOG_ERROR("default", "SDLAudioRenderer", "SDL_OpenAudioDevice failed:{}", SDL_GetError());
        throw std::runtime_error("SDL_OpenAudioDevice failed!");
    }

    SDL_PauseAudioDevice(m_device_id, 0);
    m_is_playing.store(true);
}
void SDLAudioRenderer::stop()
{
    if (m_device_id > 0)
    {
        m_is_playing.store(false);
        while (true)
        {
            auto data = m_pcm_buffer.try_pop(192000);
            if (data.empty())
            {
                break;
            }
        }
        SDL_PauseAudioDevice(m_device_id, 1);

        SDL_CloseAudioDevice(m_device_id);
        m_device_id = 0;
    }
}

void SDLAudioRenderer::append_pcm(AVFramePtr frame)
{
    if (!frame)
    {
        return;
    }

    const int out_sample_rate = 48000;
    const AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    const AVChannelLayout out_ch_layout = AV_CHANNEL_LAYOUT_STEREO;

    const int in_sample_rate = frame->sample_rate;
    const AVSampleFormat in_sample_fmt = static_cast<AVSampleFormat>(frame->format);
    const AVChannelLayout in_ch_layout = frame->ch_layout;

    SwrContext* swr_context = nullptr;
    auto alloc_status = swr_alloc_set_opts2(
        &swr_context,
        &out_ch_layout,
        out_sample_fmt,
        out_sample_rate,
        &in_ch_layout,
        in_sample_fmt,
        in_sample_rate,
        0,
        nullptr);
    if (alloc_status < 0 || !swr_context)
    {
        DANEJOE_LOG_ERROR("default", "SDLAudioRenderer", "swr_alloc_set_opts2 failed");
        if (swr_context)
        {
            swr_free(&swr_context);
        }
        return;
    }
    if (swr_init(swr_context) < 0)
    {
        DANEJOE_LOG_ERROR("default", "SDLAudioRenderer", "swr_init failed");
        swr_free(&swr_context);
        return;
    }

    const int64_t delay = swr_get_delay(swr_context, in_sample_rate);
    const int out_nb_samples = static_cast<int>(
        av_rescale_rnd(delay + frame->nb_samples, out_sample_rate, in_sample_rate, AV_ROUND_UP));

    uint8_t* out_data = nullptr;
    int out_linesize = 0;
    const int out_channels = out_ch_layout.nb_channels;
    if (av_samples_alloc(
        &out_data,
        &out_linesize,
        out_channels,
        out_nb_samples,
        out_sample_fmt,
        0) < 0)
    {
        DANEJOE_LOG_ERROR("default", "SDLAudioRenderer", "av_samples_alloc failed");
        swr_free(&swr_context);
        return;
    }

    const uint8_t* const* in_data = const_cast<const uint8_t* const*>(frame->extended_data);
    const int converted_samples = swr_convert(
        swr_context,
        &out_data,
        out_nb_samples,
        in_data,
        frame->nb_samples);
    if (converted_samples < 0)
    {
        DANEJOE_LOG_ERROR("default", "SDLAudioRenderer", "swr_convert failed");
        av_freep(&out_data);
        swr_free(&swr_context);
        return;
    }

    const int out_buffer_size = av_samples_get_buffer_size(
        &out_linesize,
        out_channels,
        converted_samples,
        out_sample_fmt,
        1);
    if (out_buffer_size > 0)
    {
        std::vector<uint8_t> data(out_data, out_data + out_buffer_size);
        data.resize(out_buffer_size);
        m_pcm_buffer.push(data.begin(), data.end());
    }

    av_freep(&out_data);
    swr_free(&swr_context);
}
void SDLAudioRenderer::append_pcm(uint8_t* pcm, int size)
{
    std::vector<uint8_t> data(pcm, pcm + size);
    m_pcm_buffer.push(data.begin(), data.end());
}

void SDLAudioRenderer::set_time_service(std::shared_ptr<TimeService> time_service)
{
    m_time_service = time_service;
}

int64_t SDLAudioRenderer::get_bytes_per_second()const
{
    int bytes_per_sample = 1;
    switch (m_audio_format)
    {
        case AUDIO_S16SYS:
            bytes_per_sample = 2;
            break;
        default:
            break;
    }
    return m_audio_channels * bytes_per_sample * m_audio_frequency;
}