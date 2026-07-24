# PixelMapFromSurface

## 适用范围

- 从 Surface 创建 PixelMap（截图/录屏场景）
- 从 SurfaceBuffer 直接创建 PixelMap
- 指定区域截取（srcRect）
- 是否启用 Transform 变换（**仅 Vulkan 路径生效**，详见下文）

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| pixel_map_from_surface.h | `rosen/modules/create_pixelmap_surface/include/pixel_map_from_surface.h` | 对外自由函数声明 |
| pixel_map_from_surface.cpp | `rosen/modules/create_pixelmap_surface/src/pixel_map_from_surface.cpp` | 实现 + 内部类 `PixelMapFromSurface` |
| pixelmapfromsurface_fuzzer | `rosen/modules/create_pixelmap_surface/test/fuzztest/pixelmapfromsurface_fuzzer/` | Vulkan 路径 fuzz（`RS_ENABLE_VK`） |
| pixelmapfromsurfacegl_fuzzer | `rosen/modules/create_pixelmap_surface/test/fuzztest/pixelmapfromsurfacegl_fuzzer/` | OpenGL 路径 fuzz（`RS_ENABLE_GL`，含 stub `RSSystemProperties` 把 `systemGpuApiType_` 硬编码为 `OPENGL`） |
| BUILD.gn | `rosen/modules/create_pixelmap_surface/` | 构建配置 |

## 核心模型

模块对外暴露两个自由函数，内部委托给 `PixelMapFromSurface` 类。

**`CreatePixelMapFromSurface`**：
```cpp
std::shared_ptr<Media::PixelMap> CreatePixelMapFromSurface(
    sptr<Surface> surface,
    const Media::Rect& srcRect,
    bool transformEnabled = false);
```
- 输入：Surface 指针 + 源矩形区域 + 是否启用变换
- 输出：`Media::PixelMap` 共享指针（内部用 `unique_ptr`，外层隐式转换为 `shared_ptr`）
- 流程（`pixel_map_from_surface.cpp:1057-1075`）：
  1. `surface->GetTransform()` 读 Surface 的 transform 类型（注意：来自 Surface 而非 SurfaceBuffer）
  2. 进入 `PixelMapFromSurface::Create`，按 GPU 后端分派到 `CreateForGL` / `CreateForVK`
  3. `CreateForGL/VK` 内部调用 `GetNativeWindowBufferFromSurface`：`AcquireLastFlushedBuffer`（拿最近一帧已 flush 的 buffer，不是 `AcquireBuffer`），并 `fence->Wait(3000)` 最多等 3 秒
  4. GPU 把源纹理画进 PixelMap 的 DMA 内存（详见下节）
  5. 析构时 `Clear()` 调用 `surface_->ReleaseLastFlushedBuffer`

**`CreatePixelMapFromSurfaceBuffer`**：
```cpp
std::shared_ptr<Media::PixelMap> CreatePixelMapFromSurfaceBuffer(
    sptr<SurfaceBuffer> surfaceBuffer,
    const Media::Rect& srcRect);
```
- 输入：SurfaceBuffer 指针 + 源矩形区域
- 输出：`Media::PixelMap` 共享指针
- 流程（`pixel_map_from_surface.cpp:1077-1087`）：跳过 Acquire/Release，直接用调用方持有的 SurfaceBuffer 进入同样的 GPU 拷贝路径

### GPU 后端分发

`PixelMapFromSurface::Create`（`pixel_map_from_surface.cpp:856-867`）按 `RSSystemProperties::GetGpuApiType()` 分派：

| GpuApiType | 路径 | 编译开关 |
| --- | --- | --- |
| `OPENGL` | `CreateForGL` → `GetPixelMapForGL` → `CreatePixelMapForGL` | `RS_ENABLE_GL` + `RS_ENABLE_UNI_RENDER` |
| `VULKAN` / `DDGR` | `CreateForVK` → `GetPixelMapForVK` | `RS_ENABLE_VK` |

未启用 GL/VK 时直接返回 nullptr。

### 像素拷贝是 GPU 绘制，不是 CPU memcpy

两条路径都用 GPU 把源 SurfaceBuffer 画进 PixelMap 的 DMA 内存，源码里**没有任何 CPU 读像素代码**：

- **GL 路径**（`CreatePixelMapForGL`, line 346-377；`DrawImage`, line 759-830）：
  1. `PixelMap::Create` 分配空 PixelMap
  2. `LocalDmaMemAlloc` 分配 DMA SurfaceBuffer，通过 `pixelmap->SetPixelsAddr(..., DMA_ALLOC, ...)` 注入 PixelMap
  3. 用同一块 DMA SurfaceBuffer 创建 EGLImageKHR + `GL_TEXTURE_2D` → 包成 `SkSurface`
  4. 把源 SurfaceBuffer 包成 EGLImage + `GL_TEXTURE_EXTERNAL_OES` → `SkImage`
  5. `canvas->drawImageRect(srcRect → dstRect)` GPU 绘制
  6. `flushAndSubmit(GrSyncCpu::kYes)` 同步落盘

- **VK 路径**（`GetPixelMapForVK`, line 636-684；`DrawImageRectVK`, line 514-560）：
  1. `LocalDmaMemAlloc` 给 PixelMap 注入 DMA 内存（若 `ShouldSwapDimensions()` 且 `transformEnabled_`，宽高对调）
  2. `Drawing::Surface::MakeFromBackendTexture` 创建目标 Surface
  3. `CreateDrawingImage` 把源 SurfaceBuffer 包成 `Drawing::Image`（`BuildFromTexture`）
  4. `CanvasDrawImage` 在画布上 `DrawImageRect`

### 像素格式

`DrawImage`（GL）/ `CanvasDrawImage`（VK）支持：

- `GRAPHIC_PIXEL_FMT_RGBA_8888`（默认）
- `GRAPHIC_PIXEL_FMT_BGRA_8888`
- `GRAPHIC_PIXEL_FMT_YCBCR_P010` / `YCRCB_P010` / `RGBA_1010102`：**仅 VK 路径**在 `CanvasDrawImage` 中走 `RSColorSpaceConvert`（VPE）做 HDR→SDR 色调映射（依赖 `USE_VIDEO_PROCESSING_ENGINE`）；GL 路径仅切 `colorType`/`glType`，不做色域转换

### Transform 处理（仅 VK 路径）

- `transformEnabled` 默认 false，**完全不应用变换**
- 来自 `surface->GetTransform()`（`pixel_map_from_surface.cpp:1064-1065`），不是 SurfaceBuffer 字段
- VK 路径在 `CanvasDrawImage → CreateMatrixBySurface` 中按 `transform_` 类型应用矩阵，覆盖 10 种：`ROTATE_90/180/270`、`FLIP_H/V`、`FLIP_H_ROT90/180/270`、`FLIP_V_ROT90/180/270`
- 维度交换（90/270 系列）由 `ShouldSwapDimensions()` 决定，会同时影响 PixelMap 的分配尺寸
- **GL 路径目前完全忽略 `transformEnabled_` 与 `transform_`**，`DrawImage` 直接用原始 srcRect 绘制——是遗漏还是有意为之需要确认

### 边界与异常

- `srcRect.left/top < 0` 或 `width/height <= 0`：`Create` 直接返回 nullptr（line 840、884）
- srcRect 越界 bufferWidth/bufferHeight：`GetNativeWindowBufferFromSurface` 完整校验（含 int64 防溢出），越界返回 nullptr（line 708-716）
- `surface == nullptr`：transform 强制为 `GRAPHIC_ROTATE_NONE`，函数继续；但 `AcquireLastFlushedBuffer` 会失败，最终返回 nullptr
- GPU context 不可用（`RSBackgroundThread::Instance().GetShareGPUContext() == nullptr`）：返回 nullptr

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 两个独立函数 | `CreatePixelMapFromSurface` vs `CreatePixelMapFromSurfaceBuffer` | 区分是否需要自行管理 Buffer 生命周期 |
| transformEnabled 默认 false | `bool transformEnabled = false` | 默认不应用变换，截图通常需要屏幕原始方向 |
| 返回 shared_ptr | `std::shared_ptr<Media::PixelMap>` | 自动管理 PixelMap 生命周期；内部 `unique_ptr` 在自由函数返回时隐式转换 |
| 两套 fuzz 测试 | surface（VK）/ surfacegl（GL） | 覆盖两种 GPU 后端路径，**不是** "CPU buffer vs GPU buffer" |
| PixelMap 用 DMA 内存 | `LocalDmaMemAlloc` + `SetPixelsAddr(DMA_ALLOC)` | 与 GPU 绘制路径零拷贝对接 |
| 取 `AcquireLastFlushedBuffer` 而非 `AcquireBuffer` | `pixel_map_from_surface.cpp:700` | 拿最近一帧已完成渲染的 buffer，避免取到正在绘制的帧 |
| fence 等待 3000ms | `fence->Wait(3000)` | 等 GPU 渲染完成，否则读到未完成帧 |
| GL 路径 `GrSyncCpu::kYes` | `directContext->flushAndSubmit(GrSyncCpu::kYes)`（line 821） | 强制 CPU 同步，确保返回 PixelMap 时像素已落盘 |
