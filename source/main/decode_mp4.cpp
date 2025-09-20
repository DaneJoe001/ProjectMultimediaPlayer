#include <memory>

#include "main/decode_mp4.hpp"
#include "log/manage_logger.hpp"
#include "codec/av_common.hpp"
#include "codec/av_error.hpp"
#include "codec/av_packet_ptr.hpp"
#include "mt_queue/mt_queue.hpp"

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

int decode_mp4(const std::string& file_path, std::shared_ptr<DaneJoe::MTQueue<AVFramePtr>> frame_queue)
{
#if FFMPEG_VERSION<771
    av_register_all();
#endif
    /// @note 新版本ffmpeg采用自动注册机制，无需调用av_register_all();
    DANEJOE_LOG_TRACE("default", "decode_mp4", "version: {}", av_version_info());
    /// @brief 创建AVFormatContext
    AVFormatContext* ic = avformat_alloc_context(); // 正确初始化

    if (!ic)
    {
        DANEJOE_LOG_ERROR("default", "decode_mp4", "无法分配 AVFormatContext！");
        return -1;
    }
    /// @brief 打开输入流并读取标头。编解码器未打开。流必须使用 avformat 关闭_关闭_input()
    AVError error = avformat_open_input(&ic, file_path.c_str(), nullptr, nullptr);
    if (error == 0)
    {
        uint64_t duration = ic->duration;
#if FFMPEG_VERSION>=771
        /// @brief 新增：探测流信息,不调用不能获取duration
        error = avformat_find_stream_info(ic, nullptr);
#endif
        if (error.failed())
        {
            DANEJOE_LOG_ERROR("default", "decode_mp4", "错误信息: {}", AVError(error).message());
            /// @brief 关闭输入流
            avformat_close_input(&ic);
            return -1;
        }
        /// @brief 通过duration获取总时长
        int total_seconds = ic->duration / AV_TIME_BASE;
        DANEJOE_LOG_TRACE("default", "decode_mp4", "视频文件打开成功,视频总时长为：{}分{}秒", total_seconds / 60, total_seconds % 60);

        /// @brief 视频流下标
        int video_stream_index = 0;
        int audio_stream_index = 0;

#ifdef REFERENCE

        //==================================================

        AVCodec* acodec = avcodec_find_decoder(ic->streams[audio_stream_index]->codecpar->codec_id);

        AVCodecContext* audio_codec_context = avcodec_alloc_context3()

            /// @brief 音频解码器上下文
            SwrContext * swr_context = swr_alloc();

        // 设置输出参数
        int out_channels = 2; // 输出通道数
        int out_sample_rate = ic->sample_rate; // 输出采样率
        enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16; // 输出样本格式

        // 设置输入参数
        int in_channels = ac->channels; // 输入通道数
        int in_sample_rate = ac->sample_rate; // 输入采样率
        enum AVSampleFormat in_sample_fmt = ac->sample_fmt; // 输入样本格式

        // 配置 SwrContext
        av_opt_set_int(actx, "in_channel_count", in_channels, 0);
        av_opt_set_int(actx, "out_channel_count", out_channels, 0);
        av_opt_set_int(actx, "in_sample_rate", in_sample_rate, 0);
        av_opt_set_int(actx, "out_sample_rate", out_sample_rate, 0);
        av_opt_set_sample_fmt(actx, "in_sample_fmt", in_sample_fmt, 0);
        av_opt_set_sample_fmt(actx, "out_sample_fmt", out_sample_fmt, 0);
        av_opt_set_int(actx, "out_channel_layout", AV_CHANNEL_LAYOUT_STEREO, 0); // 输出通道布局

        // 初始化 SwrContext
        if (swr_init(actx) < 0)
        {
            // 错误处理
            swr_free(&actx);
            return -1;
        }

        //===========================================
#endif

        /// @brief 视频解码器上下文 
        AVCodecContext* video_codec_context = nullptr;
        /// @brief 音频解码器上下文
        AVCodecContext* audio_codec_context = nullptr;

        /// @brief 遍历媒体流
        for (int i = 0;i < ic->nb_streams;i++)
        {
#if FFMPEG_VERSION>=771
            /// @brief 获取当前下标的媒体流
            AVStream* stream = ic->streams[i];
            /// @brief 获取当前下标媒体流的参数
            /// @note 高版本无法直接通过nb_streams[i]获取参数
            auto codecpar = stream->codecpar;
#elif FFMPEG_VERSION<771
            AVCodecContext* codec = ic->nb_streams[i]->codec;
#endif
            /// @brief 判断媒体流类型是否为视频流
            if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                video_stream_index = i;
                /// @brief 寻找解码器
                const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
                if (!codec)
                {
                    DANEJOE_LOG_ERROR("default", "decode_mp4", "无法找到解码器！");
                    return -1;
                }
                /// @brief 创建解码器上下文
                video_codec_context = avcodec_alloc_context3(codec);
                /// @brief 将解码参数复制到解码器上下文
                error = avcodec_parameters_to_context(video_codec_context, stream->codecpar);
                if (error.failed())
                {
                    DANEJOE_LOG_ERROR("default", "decode_mp4", "错误信息: {}", AVError(error).message());
                    return -3;
                }
                error = avcodec_open2(video_codec_context, codec, nullptr);
                if (error.failed())
                {
                    DANEJOE_LOG_ERROR("default", "decode_mp4", "错误信息: {}", AVError(error).message());
                    return -2;
                }
                DANEJOE_LOG_TRACE("default", "decode_mp4", "视频解码器名称：{}", codec->name);
            }
            else if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            {
                audio_stream_index = i;
                const AVCodec* acodec = avcodec_find_decoder(codecpar->codec_id);
                if (!acodec)
                {
                    DANEJOE_LOG_ERROR("default", "decode_mp4", "无法找到音频解码器！");
                    return -1;
                }
                audio_codec_context = avcodec_alloc_context3(acodec);
            }
        }
        /// @brief 循环计算每一帧的播放位置
        while (true)
        {
            /// @brief 视频压缩数据
            // AVPacket* packet = av_packet_alloc();
            AVPacketPtr packet;
            packet.ensure_allocated();
            if (!packet)
            {
                DANEJOE_LOG_ERROR("default", "decode_mp4", "无法分配AVPacket");
            }
            /// @brief 读取视频数据
            error = av_read_frame(ic, packet.get());
            if (error != 0)
            {
                DANEJOE_LOG_ERROR("default", "decode_mp4", "错误信息: {}", AVError(error).message());
                break;
            }
            /// @brief 添加此判断避免处理非视频流数据包
            if (packet->stream_index != video_stream_index)
            {
                continue;
            }
            /// @brief 计算播放位置
            int pts = packet->pts * AVRationalInfo(ic->streams[packet->stream_index]->time_base).get_double() * 1000;
            DANEJOE_LOG_TRACE("default", "decode_mp4", "packet pts: {}", pts);
#if FFMPEG_VERSION < 771
            int got_picture = 0;
            ret = avcodec_decode_video2(video_codec_context, frame, &got_picture, packet);
#endif
            /// @brief 解码队列接收数据包待解码
            /// @note 非传统意义异步方式
            error = avcodec_send_packet(video_codec_context, packet.get());
            if (error.failed())
            {
                DANEJOE_LOG_ERROR("default", "decode_mp4", "错误信息: {}", AVError(error).message());
                // av_packet_free(&packet);
                continue;
            }
            /// @brief 循环解码数据包队列，确保队列中的数据包全部解码完毕
            while (error.ok())
            {
                AVFramePtr frame;
                frame.ensure_allocated();
                /// @brief 获取解码后的数据帧
                error = avcodec_receive_frame(video_codec_context, frame.get());
                if (error.ok())
                {
                    DANEJOE_LOG_TRACE("default", "decode_mp4", "Frame width: {}, height: {}", frame->width, frame->height);
                    if (frame_queue)
                    {
                        frame_queue->push(std::move(frame));
                    }
                }
                /// @note EAGAIN 表示需要更多数据才能继续解码
                /// @note AVERROR_EOF 表示数据包队列已空
                if (error == AVERROR(EAGAIN))
                {
                    break;
                }
                else if (error == AVERROR_EOF)
                {
                    DANEJOE_LOG_INFO("default", "decode_mp4", "AVERROR_EOF");
                    break;
                }
                else if (error.failed())
                {
                    DANEJOE_LOG_ERROR("default", "decode_mp4", "错误信息: {}", AVError(error).message());
                    // av_packet_free(&packet);
                    continue;
                }
            }
            /// @brief 释放数据包
            // av_packet_unref(packet);
        }

#ifdef TEST_AV_SEEK
        auto video_stream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        AVPacketPtr packet;
        packet.ensure_allocated();
        int times = 1000;
        while (true)
        {
            AVError error = av_read_frame(ic, packet.get());
            times--;
            if (times == 0 && error.ok())
            {
                int time = 3000;
                long long pos = (double)time / (double)1000 * AVRationalInfo(ic->streams[packet->stream_index]->time_base).get_double();
                DANEJOE_LOG_TRACE("default", "decode_mp4", "pos: {}", pos);
                av_seek_frame(ic, video_stream, pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
            }
            if (error.failed())
            {
                break;
            }
            DANEJOE_LOG_TRACE("default", "decode_mp4", "packet size:{}", packet->size);
            DANEJOE_LOG_TRACE("default", "decode_mp4", "packet pts:{}", packet->pts);
            DANEJOE_LOG_TRACE("default", "decode_mp4", "packet dts:{}", packet->dts);
            if (packet->stream_index == video_stream_index)
            {
                DANEJOE_LOG_TRACE("default", "decode_mp4", "Video packet");
            }
            else if (packet->stream_index == audio_stream_index)
            {
                DANEJOE_LOG_TRACE("default", "decode_mp4", "Audio packet");
            }
            packet.unref();
        }
#endif

        /// @brief 释放解码器上下文
        avcodec_free_context(&video_codec_context);
        /// @brief 关闭输入流
        avformat_close_input(&ic);
    }
    else
    {
        DANEJOE_LOG_ERROR("default", "decode_mp4", "错误信息: {}", AVError(error).message());
        DANEJOE_LOG_ERROR("default", "decode_mp4", "视频打开失败！");
        /// @brief 释放内存
        avformat_free_context(ic);
        return -1;
    }
    return 0;
}