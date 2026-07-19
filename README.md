# DaneJoe MultiMedia Player

一个使用 C++20、Qt 6、FFmpeg 和 SDL2 实现的本地桌面播放器。项目覆盖媒体解封装与解码、音视频帧调度、SDL 视频渲染、SDL 音频输出，以及基于音频时钟的基础 A/V 同步。

## 当前功能

- 通过 Qt 文件对话框打开 `mp4`、`avi`、`mkv`、`mp3` 和 `ts` 文件。
- 使用 FFmpeg 完成音视频流发现、解码和时间戳换算。
- 使用 SDL2 texture 将视频帧渲染到嵌入 Qt 窗口的原生窗口句柄。
- 使用 `libswresample` 将音频转换为 48kHz、S16、双声道 PCM，并通过 SDL audio callback 输出。
- 以音频播放进度作为主时钟，根据视频帧时间差执行等待、渲染或丢帧。
- 使用会话 ID 隔离切换文件后迟到的旧帧。
- 根据视频队列的 32/64 低高水位恢复或暂停解码，限制缓存增长。
- 周期性记录 FPS、丢帧数、队列长度和 A/V 时间差指标。

## 主要模块

| 模块 | 职责 |
| --- | --- |
| `source/main/player_app.cpp` | 组装窗口、解码服务、媒体控制器和音频线程 |
| `source/worker/media_decode_worker.cpp` | 读取媒体包、驱动 FFmpeg 解码并生成带时间戳的会话帧 |
| `source/controller/media_controller.cpp` | 缓存音视频帧，执行背压和基于音频时钟的视频调度 |
| `source/audio/sdl_audio_renderer.cpp` | 重采样音频并维护 SDL PCM 输出队列 |
| `source/renderer/sdl_frame_renderer.cpp` | 管理 SDL window、renderer 和 texture |
| `source/view/main_window.cpp` | 提供文件选择和播放器主窗口 |

## 构建依赖

- CMake 3.20 或更高版本
- 支持 C++20 的编译器；当前验证环境为 Windows、MSVC 和 Ninja
- Qt 6：`Core`、`Gui`、`Widgets`、`Sql`、`Network`、`OpenGL`、`OpenGLWidgets`
- SDL2
- FFmpeg：`avcodec`、`avformat`、`avutil`，音频链路还使用 `swresample`
- DaneJoe 0.2.0：日志、状态、并发队列和定时器支持

依赖通过 `find_package` 解析。本机工具链、Qt、vcpkg 和 DaneJoe 路径保存在不纳入版本控制的 `CMakeUserPresets.json` 中。

## Windows 构建

配置并构建 Debug GUI：

```powershell
cmake --preset win-ninja-msvc-debug-local
cmake --build --preset win-ninja-msvc-debug-local
```

运行：

```powershell
.\build\win-ninja-msvc-debug-local\executable_gui.exe
```

测试 Preset 可验证测试构建入口：

```powershell
cmake --preset win-ninja-msvc-debug-test-local
cmake --build --preset win-ninja-msvc-debug-test-local
ctest --preset win-ninja-msvc-debug-test-local
```

仓库当前尚未提供自动化测试用例，`ctest` 会报告未发现测试。

## 实现边界

- 视频渲染路径主要处理 `AV_PIX_FMT_YUV420P`；其他像素格式尚未接入转换或对应的 SDL texture 更新路径。
- 音频输出固定为 48kHz、S16、双声道，暂未根据输出设备动态协商格式。
- A/V 同步采用固定阈值和延迟补偿，不包含播放速率微调、设备延迟探测或外部时钟同步。
- 当前只提供 Qt GUI 入口，没有独立的 console 可执行程序。

## 技术笔记

- [FFmpeg 解码流程](document/ffmpeg解码流程.md)
- [FFmpeg 对象与封装边界](document/FFmpeg对象与封装边界.md)
- [YUV 格式解析](document/YUV格式解析.md)

## License

本项目采用 Apache License 2.0，详见 [LICENSE](LICENSE)。
