下面按 **FFmpeg 7.1.x（你写的 7.7.1 可能是笔误；截至我知识库，主线是 7.1/7.0 系列）** 的 API 思路，讲一套“解码链路最佳实践”（best practices）。如果你项目是 C++（你仓库也是 C++/CMake/MSVC），这些原则直接适用。

# 1. 解码主流程（概览）
解码视频/音频的推荐流水线基本一致：

1. **打开输入**：`avformat_open_input`  
2. **读流信息**：`avformat_find_stream_info`  
3. **选流**：`av_find_best_stream`（视频/音频分别选）  
4. **创建解码器上下文**：  
   - `avcodec_find_decoder` / `avcodec_find_decoder_by_name`  
   - `avcodec_alloc_context3`  
   - `avcodec_parameters_to_context`  
   - `avcodec_open2`
5. **读包（demux）**：循环 `av_read_frame` 拿到 `AVPacket`
6. **送包进解码器**：`avcodec_send_packet`
7. **从解码器取帧**：`avcodec_receive_frame`（可能一次送包，多次出帧）
8. **处理帧**：渲染 / 重采样 / 转码 / 入队列等
9. **冲刷（flush）**：读完后对每个解码器 `avcodec_send_packet(ctx, nullptr)`，再 `receive_frame` 直到 `AVERROR_EOF`
10. **释放资源**：按对象生命周期释放 `AVPacket/AVFrame/AVCodecContext/AVFormatContext`

关键点：**“demux 与 decode 解耦”**，以及 **“send/receive 的状态机语义正确处理”**。

---

# 2. `send_packet` / `receive_frame` 状态机（最容易踩坑）
最佳实践是严格遵循以下规则：

## 2.1 正常解码循环（单解码器）
对每个 `packet`：

- 调用 `avcodec_send_packet`
  - 返回 `0`：成功接受输入
  - 返回 `AVERROR(EAGAIN)`：表示解码器内部输出队列满了，你需要先 `receive_frame` 把帧取空后再送包
  - 返回其他错误：直接处理失败（记录日志、丢弃或终止）

- 然后循环 `avcodec_receive_frame`
  - 返回 `0`：得到一帧，继续收（可能还有）
  - 返回 `AVERROR(EAGAIN)`：当前没有更多输出帧，回去继续读下一包
  - 返回 `AVERROR_EOF`：解码器已完全结束（通常在 flush 阶段出现）
  - 返回其他错误：解码失败（通常需要重置/丢帧/终止）

**推荐模板（逻辑层面）**：
- “送包成功后，尽可能把当前可输出的帧都取完”
- “遇到 EAGAIN：该换另一侧动作（send 失败→先 receive；receive 失败→先 send）”

## 2.2 flush 的正确姿势
输入结束后：

- `avcodec_send_packet(ctx, nullptr)`  
- 循环 `avcodec_receive_frame` 直到 `AVERROR_EOF`（或持续 `EAGAIN` 但通常不会长期发生）

flush 是必做的，否则 B 帧、重排序缓存会丢。

---

# 3. 时间戳（PTS/DTS）与同步：最重要的“工程最佳实践”
很多播放器“能播但不同步/跳帧/卡顿”就是这里不规范。

## 3.1 demux 层：拿到 packet 的时间基
- `packet.pts` / `packet.dts` 在 `stream->time_base` 下
- 统一转换用 `av_rescale_q`

## 3.2 decode 层：拿到 frame 的 PTS
最佳实践：
- 优先用 `frame->best_effort_timestamp`（FFmpeg 在很多情况下会补齐/推断）
- 然后转换到一个统一时间基（常用 `AV_TIME_BASE_Q` 或 `1/1000` ms）

## 3.3 音视频同步（AV sync）
常见推荐：
- **以音频为主时钟（audio master clock）**：视频根据音频时间做丢帧/延迟
- 若无音频（纯视频）：用系统时钟/单调时钟（monotonic clock）

---

# 4. 线程模型：解码线程与 IO/渲染线程解耦
工程上最稳的结构（适合你这种“解码服务 + 渲染器”分层）：

- **Demux 线程**：`av_read_frame`，按 stream id 分发到音频包队列、视频包队列
- **Audio Decode 线程**：packet queue → `send/receive` → frame queue（或直接送重采样/音频设备）
- **Video Decode 线程**：packet queue → `send/receive` → frame queue
- **Render 线程/UI 线程**：从 video frame queue 按时钟取帧渲染（SDL/Qt/OpenGL）

队列最佳实践：
- **有界队列（bounded queue）**，避免内存爆
- 有“中止标记”（abort flag）和条件变量（condition_variable）
- 入队时做背压（backpressure），例如队列满则等待或丢包（视频可丢，音频慎重）

---

# 5. 解码器参数与性能/稳定性建议
## 5.1 多线程解码
通常你会设置：
- `codec_ctx->thread_count = std::thread::hardware_concurrency()`（或限制上限）
- `codec_ctx->thread_type = FF_THREAD_FRAME | FF_THREAD_SLICE`（视编解码器而定）
但注意：
- 低延迟场景（实时）可能不希望太深的 frame threading 带来延迟。
- 某些硬解/特定 codec 对线程模型有限制，要按实际解码器行为调整。

## 5.2 低延迟（可选）
如果是“直播/实时”：
- 尽量减少缓存：控制队列大小
- 合理设置 `AVFormatContext` 的探测与缓冲（`probesize`/`analyzeduration` 等），但这属于“输入协议/封装”相关，需按场景调参

---

# 6. 像素格式与转换：不要在解码线程里做重活（除非必要）
最佳实践：
- 解码线程只负责产出原始 `AVFrame`（可能是 YUV420P / NV12 / P010 等）
- 渲染前再做 `sws_scale`（软件）或走 GPU 路径（SDL texture / OpenGL / D3D11）
- 如果你要“统一输出格式”，建议在独立的转换阶段处理，并且**缓存 `SwsContext`**，不要每帧创建/销毁。

音频同理：
- 用 `SwrContext` 重采样/重排/格式转换
- 缓存 `SwrContext`，参数变化时再重建

---

# 7. 错误处理与资源生命周期：建议统一封装
最佳实践（C++）：
- 用 RAII 封装 `AVFormatContext`、`AVCodecContext`、`AVPacket`、`AVFrame`
- 所有 FFmpeg API 的返回值都用 `av_strerror` 打日志（便于定位）
- 对 `EAGAIN/EOF` 做“分支处理”，不要当作普通错误

---

# 8. 你如果要我按你仓库落地：我需要你确认 3 件事
你现在的工程结构里有 `video_decode_service`、SDL 渲染器等。我可以按“最佳实践”把解码主循环、队列、flush、PTS 统一这些落到你现有代码里，但我需要你确认：

1. **你要支持音频吗？**（当前仓库看起来偏视频渲染；是否已有音频管线？）  
2. **目标是本地文件播放还是网络流（RTSP/HTTP）？**（影响探测/缓冲策略）  
3. **你用软件解码还是要硬解（D3D11VA/NVDEC/QSV）？**（影响 `AVHWDeviceContext` 相关流程）

你回我这 3 点，我就可以按你现有 `include/main/decode_mp4.hpp` / `source/main/decode_mp4.cpp` / `service/video_decode_service.*` 的结构，把“最佳实践版本”的解码流程整理成你项目可直接用的实现。  

# 状态
- **已完成**：给出 FFmpeg 7.x 解码流程的工程最佳实践要点（状态机、flush、PTS、线程模型、队列、转换与资源管理）。  
- **待确认**：你的目标场景（音频/网络/硬解）以便进一步针对性落地到你的代码库。