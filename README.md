# DaneJoe MultiMedia Player (Qt + FFmpeg + SDL)

一个基于 **Qt6 + FFmpeg + SDL2** 的本地多媒体播放器练手项目，当前支持通过 GUI 打开本地媒体文件并播放（视频渲染 + 音频播放）。项目以 **CMake Presets** 管理跨平台构建，代码以 C++20 编写。

## 功能概览

- **本地文件播放**
  - 通过菜单 `File -> Open` 打开本地媒体文件
  - 支持常见容器：`*.mp4` / `*.avi` / `*.mkv` / `*.mp3` / `*.ts`（以文件对话框过滤为准）
- **视频渲染**
  - 使用 SDL2 渲染视频帧（当前主要覆盖 `AV_PIX_FMT_YUV420P`）
  - SDL 渲染窗口嵌入 Qt `QLabel`（`SDL_CreateWindowFrom(winId)`）
  - 处理了 Qt/SDL 在高 DPI（DPI scaling）下的像素尺寸差异：以物理像素（physical pixels）更新 SDL viewport
- **音频播放**
  - SDL2 audio callback 输出 PCM
  - FFmpeg `swresample` 转换到统一输出格式（当前代码固定输出：48kHz / S16 / Stereo）
  - 维护 **audio clock（音频时钟）**（微秒）用于 A/V 同步调度
- **退出路径稳定性**
  - 关闭窗口时同步 stop 音频（跨线程 `BlockingQueuedConnection`），避免退出后仍继续播放

## 工程化亮点

- **可观测性（Observability）**
  - 通过周期性输出 `METRIC` 日志（fps / drop / A/V diff / queue_len）辅助定位性能与同步问题

```text
[2026-01-12 13:09:39] [INFO] [METRIC] [...] [fps=30 drop=0 vq=21 aq=0 av_diff_avg=10.17ms av_diff_max=14.78ms]
[2026-01-12 13:09:42] [INFO] [METRIC] [...] [fps=30 drop=0 vq=49 aq=0 av_diff_avg=9.26ms av_diff_max=14.95ms]
[2026-01-12 13:09:48] [INFO] [METRIC] [...] [fps=30 drop=0 vq=101 aq=1 av_diff_avg=10.20ms av_diff_max=14.00ms]
```
- **Backpressure（背压）**
  - 通过队列高/低水位控制解码暂停/恢复，避免长时间播放队列无限增长导致的内存占用与延迟上升

## 项目结构（按职责）

- `source/main/widget_main.cpp`
  - Qt GUI 入口
- `source/main/player_app.cpp`
  - 应用装配（wiring）：创建窗口、启动解码服务/控制器、启动音频线程等
- `source/service/media_decode_service.cpp`
  - 解码线程与 worker 管理，切换文件时递增 `session_id` 并清空 controller 缓存
- `source/worker/media_decode_worker.cpp`
  - 基于 FFmpeg 的解封装/解码，产出 `SessionFrame`（含 `session_id/frame_id/pts_us`）
- `source/controller/media_controller.cpp`
  - 帧队列缓存 + 定时器调度
  - 视频侧以音频时钟为主进行简单同步/丢帧（阈值策略）
- `source/audio/sdl_audio_renderer.cpp`
  - SDL audio callback、PCM 缓冲队列、swresample
- `source/worker/sdl_audio_worker.cpp`
  - 音频渲染 worker（运行在独立 QThread），支持 stop 门禁以避免退出/stop 后继续入队
- `source/renderer/sdl_frame_renderer.cpp`
  - SDL texture/renderer/window 管理与绘制
- `source/view/main_window.cpp`、`source/view/sdl_frame_widget.cpp`
  - Qt 主窗口与 SDL 渲染承载控件

## 依赖

- **CMake**: >= 3.20
- **C++**: C++20
- **Qt6**: `Core` / `Gui` / `Widgets` 等（见 `cmake/dependencies.cmake`）
- **SDL2**: `find_package(SDL2 CONFIG REQUIRED)`
- **FFmpeg**: `find_package(FFMPEG REQUIRED)`
- **DaneJoe**（自定义/第三方库）: `find_package(DaneJoe 0.2.0 CONFIG REQUIRED COMPONENTS ... )`

> 注：依赖的安装方式随你的本机环境而定（vcpkg/系统安装/自建包均可）。本仓库的 `CMakeLists.txt` 与 `cmake/dependencies.cmake` 以 `find_package` 为准。

## 构建与运行（Windows / MSVC）

项目默认启用 GUI 入口（`BUILD_GUI_APP=ON`，见 `cmake/options.cmake`）。

- **配置**

```powershell
cmake --preset win-msvc-debug
```

- **编译**

```powershell
cmake --build --preset win-msvc-debug
```

- **运行**（Debug 可执行文件路径示例）

```powershell
./build/win-msvc-debug/Debug/executable.exe
```

> 目标名与输出路径以 `CMakeLists.txt` 与 preset 为准：
> - 核心库：`TemplateCore`
> - 可执行文件：`executable`

## 使用方式

1. 启动程序
2. 菜单 `File -> Open` 选择媒体文件
3. 播放后可直接关闭窗口退出

## 已知限制 / 现状

- **切换文件播放**
  - 当前已定位问题主要发生在 SDL 材质/渲染阶段（texture lifecycle / `SDL_UpdateYUVTexture` 等），尚待完善。
- **A/V 同步**
  - 已修复：部分媒体存在音视频首帧 PTS 基准不一致导致的固定大偏移（200ms+）。当前通过以首个音频帧 PTS 作为基准对齐视频时间轴解决。
  - 当前为基于音频时钟的简化阈值调度策略，仍可能存在轻微音画不同步（可接受但可继续优化）。
- **视频像素格式覆盖**
  - `SDLFrameRenderer` 当前主要支持 `AV_PIX_FMT_YUV420P`，其他格式会提示不支持。
- **音频输出格式固定**
  - 当前 `swresample` 输出固定为 48kHz/S16/Stereo，后续可根据输入流动态协商输出格式。

## 规划（Roadmap）

- **切换文件稳定性**：session 隔离 + renderer/texture 重建时序梳理
- **完善 A/V 同步**：以音频为 master clock 的调度控制器（延迟/追帧/丢帧策略），并考虑对齐音视频 PTS 基准与设备输出延迟补偿
- **Backpressure**：根据缓存水位暂停/恢复解码，控制内存占用
- **解码/渲染生命周期**：更严格的 stop/flush/drain，避免串帧与资源竞争

## License

本项目使用 **Apache-2.0**，详见 `LICENSE`。
