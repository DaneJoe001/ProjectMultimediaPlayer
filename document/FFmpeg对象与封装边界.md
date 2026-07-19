# FFmpeg 对象与封装边界

本文说明播放器当前使用的 FFmpeg 对象、所有权关系和项目内封装类型。

## 输入容器

### `AVFormatContext`

`AVFormatContext` 管理输入容器、流列表和 packet 读取状态。项目使用 `AVFormatContextPtr` 封装以下操作：

- `avformat_open_input`
- `avformat_find_stream_info`
- `av_read_frame`
- `avformat_close_input`

成功打开输入后由封装对象负责关闭。`AVStream` 和 `AVCodecParameters` 由 format context 持有，只能在输入上下文有效期间访问。

### `AVStream` 与 `AVCodecParameters`

这两个类型在当前项目中作为非持有视图使用：

- `AVStream` 提供 `index`、`time_base` 和帧率等流信息。
- `AVCodecParameters` 用于初始化 `AVCodecContext`。

调用方不单独释放它们，也不能让相关指针超过 `AVFormatContext` 的生命周期。

## 解码器

### `AVCodec`

`AVCodec` 是 FFmpeg 注册的解码器描述，由 FFmpeg 管理。项目只保存非持有指针，不负责释放。

### `AVCodecContext`

`AVCodecContextPtr` 管理解码器实例，封装上下文分配、参数复制、打开解码器以及 send/receive 状态机。上下文包含解码缓存，不能跨媒体会话直接复用。

## 数据包与帧

### `AVPacketPtr`

`AVPacketPtr` 负责 `AVPacket` 结构体生命周期，并通过 FFmpeg 引用计数支持复制：

- `ensure_allocated` 确保底层结构存在。
- `ref` 和拷贝操作共享 packet buffer 引用。
- `unref` 清空数据引用但保留结构体。
- `reset` 释放结构体并置空。
- `release` 将裸指针所有权交给调用方。

读取下一包前应解除旧引用，避免让 packet 长时间持有输入缓冲。

### `AVFramePtr`

`AVFramePtr` 采用惰性分配，并通过 `av_frame_ref` 共享底层 frame buffer：

- 默认构造不分配 `AVFrame`。
- `ensure_allocated` 只确保结构体存在。
- `init` 为指定尺寸和像素格式分配视频缓冲。
- `unref` 释放帧数据引用但保留结构体。
- `reset` 释放结构体和缓冲。
- `release` 转移裸指针所有权。

队列中的 `SessionFrame` 保存 `AVFramePtr`，复制时共享 FFmpeg buffer，而不是复制整帧像素数据。

## 状态与错误

`FFmpegStatusDetail` 把 FFmpeg 返回码转换为项目状态类型，并使用 `av_strerror` 生成可记录的错误消息。调用方仍需根据 API 语境区分：

- 成功返回值；
- `EAGAIN` 等待状态；
- `EOF` 结束状态；
- 需要终止当前操作的错误。

不能仅根据“返回值为负数”把所有状态归为同一种失败。

## 转换上下文

### `SwrContext`

音频渲染器使用 `SwrContext` 完成采样格式、采样率和声道布局转换。它由音频输出模块创建和释放，不进入帧对象的所有权范围。

### `SwsContext`

当前主要视频路径直接更新 YUV420P SDL texture，尚未引入持久化的 `SwsContext`。后续支持更多像素格式时，应由独立的视频转换组件持有并按参数复用该上下文。

## 生命周期顺序

媒体会话结束或切换文件时，资源按以下方向退出：

1. 停止产生新 packet 和 frame。
2. 停止或断开消费者。
3. 清空音视频队列中的引用。
4. 释放 codec context。
5. 关闭 format context。

该顺序避免 frame buffer、stream 视图或解码器状态引用已经失效的输入上下文。
