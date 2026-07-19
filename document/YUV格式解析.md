# YUV 格式解析

YUV 描述亮度分量 Y 和两个色度分量 U、V。`4:4:4`、`4:2:2` 和 `4:2:0` 表示色度采样比例，不直接规定字节在内存中的排列方式。解析原始数据时必须同时确认采样比例、平面布局、分量顺序、位深和 stride。

## 4:2:0 采样

对于宽高均为偶数的 8-bit 4:2:0 图像：

- Y 平面尺寸为 `width * height`。
- U 平面尺寸为 `(width / 2) * (height / 2)`。
- V 平面尺寸与 U 平面相同。
- 紧密排列时，一帧总大小为 `width * height * 3 / 2` 字节。

U、V 平面各包含 Y 平面四分之一的采样点，而不是二分之一。

## 常见内存布局

### I420 / YUV420P

I420 是三平面格式，排列为：

```text
[Y plane][U plane][V plane]
```

FFmpeg 的 `AV_PIX_FMT_YUV420P` 通常使用这种分量顺序，但每个平面可能带有独立的 `linesize`。

### YV12

YV12 同样是三平面 4:2:0，U、V 顺序相反：

```text
[Y plane][V plane][U plane]
```

### NV12

NV12 使用一个 Y 平面和一个交错 UV 平面：

```text
[Y plane][UVUVUV...]
```

NV21 的色度分量顺序为 `VUVUVU...`。

## 4:2:2 与 4:4:4

4:2:2 只在水平方向将色度采样减半。它既可以是平面格式，也可以是 packed 格式。例如 YUY2/YUYV 的字节顺序为：

```text
Y0 U0 Y1 V0
```

该顺序只描述 YUY2/YUYV，不能作为所有 YUV422 格式的统一布局。

4:4:4 为每个像素保留完整的 Y、U、V 采样，同样可以采用 planar 或 packed 布局。

## 4x4 I420 示例

一帧 4x4 I420 图像包含 16 个 Y 采样、4 个 U 采样和 4 个 V 采样：

```text
Y plane (4x4)
Y00 Y01 Y02 Y03
Y10 Y11 Y12 Y13
Y20 Y21 Y22 Y23
Y30 Y31 Y32 Y33

U plane (2x2)
U00 U01
U10 U11

V plane (2x2)
V00 V01
V10 V11
```

紧密排列时该帧占 `16 + 4 + 4 = 24` 字节。

## Stride 与 FFmpeg `AVFrame`

解码后的 `AVFrame` 不保证每行紧密排列。FFmpeg 可能为了 SIMD 或硬件要求在行尾增加 padding：

- `data[0]`、`data[1]`、`data[2]` 指向各平面起点。
- `linesize[0]`、`linesize[1]`、`linesize[2]` 表示对应平面每行跨越的字节数。
- 逐行复制或上传纹理时必须使用 `linesize`，不能只按图像宽度递增指针。

本项目使用 `SDL_UpdateYUVTexture` 更新 YUV420P texture，并分别传入 Y、U、V 平面指针及其 pitch。

## 多帧原始文件

只有在格式固定且没有额外容器头、元数据或 padding 时，才能按固定帧大小连续读取原始 YUV 文件。对于紧密排列的 8-bit I420：

```text
frame_size = width * height * 3 / 2
frame_offset = frame_index * frame_size
```

容器中的压缩视频不能使用该公式定位解码帧，必须先经过解封装和解码。
