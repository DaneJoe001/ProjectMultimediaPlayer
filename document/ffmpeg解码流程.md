# FFmpeg 解码流程

本文记录播放器使用 FFmpeg 解封装和解码时涉及的核心状态机、时间戳与生命周期约束。

## 解码链路

1. 使用 `avformat_open_input` 打开输入。
2. 使用 `avformat_find_stream_info`读取流信息。
3. 使用 `av_find_best_stream` 选择音频流和视频流。
4. 查找解码器，分配 `AVCodecContext`，复制 `codecpar` 并调用 `avcodec_open2`。
5. 循环调用 `av_read_frame` 获取压缩数据包，并按 `stream_index` 分发。
6. 使用 `avcodec_send_packet` 送入数据包。
7. 循环调用 `avcodec_receive_frame`，直到当前输入不再产生输出帧。
8. 将帧时间戳换算到统一时间基后交给调度、渲染或音频输出模块。
9. 输入结束后向解码器发送空包，取完内部延迟帧。

解封装负责从容器读取 packet，解码器负责把 packet 转换为 frame。两者具有独立的缓存和生命周期。

## Send/Receive 状态机

### 发送数据包

`avcodec_send_packet` 的主要返回状态：

- `0`：解码器接受了输入。
- `AVERROR(EAGAIN)`：必须先调用 `avcodec_receive_frame` 消费待输出帧。
- `AVERROR_EOF`：解码器已经进入结束状态。
- 其他负值：输入或解码器状态异常。

### 接收帧

`avcodec_receive_frame` 的主要返回状态：

- `0`：得到一个完整输出帧，应继续尝试接收。
- `AVERROR(EAGAIN)`：当前没有更多输出，需要继续发送输入。
- `AVERROR_EOF`：延迟帧已经全部取完。
- 其他负值：解码失败。

一个 packet 可能产生零个、一个或多个 frame。不能假定发送和接收是一一对应关系，也不能把 `EAGAIN` 当作普通错误。

## Flush

输入结束后，对每个已打开的解码器调用：

```cpp
avcodec_send_packet(codec_context, nullptr);
```

随后继续调用 `avcodec_receive_frame`，直到返回 `AVERROR_EOF`。该步骤用于输出 B 帧和其他仍保存在解码器内部的延迟帧。

切换媒体文件时不能复用已经进入 flush 状态的旧解码器。旧会话应停止生产数据，清空队列并重新创建输入和解码上下文。

## 时间戳

- packet 的 `pts` 和 `dts` 使用所属 `AVStream::time_base`。
- 解码后的显示时间优先使用 `AVFrame::best_effort_timestamp`。
- 使用 `av_rescale_q` 转换时间基，避免用浮点数累计时间误差。
- 音视频帧进入调度层前应转换到同一单位。本项目使用微秒。

本项目以音频播放进度作为主时钟。视频帧早于播放时刻时等待，明显落后时丢弃，其余帧进入 SDL 渲染路径。

## 队列与线程

- 解码任务运行在 worker 线程，GUI 操作保留在 Qt 主线程。
- 音频输出运行在独立 `QThread`，关闭或切换文件时同步停止 SDL 音频设备。
- 视频队列使用高低水位向解码线程施加背压。
- 每个 frame 携带会话 ID，控制器丢弃不属于当前会话的迟到帧。

队列关闭、worker 停止和对象析构需要保持固定顺序，避免生产者继续向已销毁的消费者发送数据。

## 格式转换

- 视频解码帧可能是 YUV420P、NV12、P010 或其他像素格式。SDL texture 不支持目标格式时，需要通过 `libswscale` 转换，或使用匹配的硬件渲染路径。
- 音频帧的采样格式、采样率和声道布局由输入决定。输出前通过 `SwrContext` 转换为音频设备接受的格式。
- `SwsContext` 和 `SwrContext` 应按输入输出参数复用，仅在参数发生变化时重建。

## 资源管理

`AVFormatContext`、`AVCodecContext`、`AVPacket` 和 `AVFrame` 的释放函数不同，所有权边界需要由封装类型明确表达。项目中的具体封装见 [FFmpeg 对象与封装边界](FFmpeg对象与封装边界.md)。
