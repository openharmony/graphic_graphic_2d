# PixelMapFromSurface

## 适用范围

- 从 Surface 创建 PixelMap（截图/录屏场景）
- 从 SurfaceBuffer 直接创建 PixelMap
- 指定区域截取（srcRect）
- 是否启用 Transform 变换

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| pixel_map_from_surface.h | `rosen/modules/create_pixelmap_surface/include/pixel_map_from_surface.h` | 核心接口声明 |
| pixelmapfromsurface_fuzzer.h | `rosen/modules/create_pixelmap_surface/test/fuzztest/pixelmapfromsurface_fuzzer/` | Surface 截图 fuzz 测试 |
| pixelmapfromsurfacegl_fuzzer.h | `rosen/modules/create_pixelmap_surface/test/fuzztest/pixelmapfromsurfacegl_fuzzer/` | GL 截图 fuzz 测试 |
| BUILD.gn | `rosen/modules/create_pixelmap_surface/` | 构建配置 |

## 核心模型

模块提供两个核心函数：

**`CreatePixelMapFromSurface`**：
```cpp
std::shared_ptr<Media::PixelMap> CreatePixelMapFromSurface(
    sptr<Surface> surface,
    const Media::Rect& srcRect,
    bool transformEnabled = false);
```
- 输入：Surface 指针 + 源矩形区域 + 是否启用变换
- 输出：`Media::PixelMap` 共享指针
- 流程：从 Surface AcquireBuffer → 读取指定区域像素 → 创建 PixelMap → ReleaseBuffer

**`CreatePixelMapFromSurfaceBuffer`**：
```cpp
std::shared_ptr<Media::PixelMap> CreatePixelMapFromSurfaceBuffer(
    sptr<SurfaceBuffer> surfaceBuffer,
    const Media::Rect& srcRect);
```
- 输入：SurfaceBuffer 指针 + 源矩形区域
- 输出：`Media::PixelMap` 共享指针
- 流程：直接从 SurfaceBuffer 读取像素 → 创建 PixelMap
- 无需 AcquireBuffer/ReleaseBuffer，调用方已持有缓冲区

**transformEnabled** 参数控制是否应用 SurfaceBuffer 的 Transform 变换（如旋转），在截图场景下可能需要保持原始方向。

数据流：Surface → AcquireBuffer → SurfaceBuffer 像素读取 → 指定区域裁剪 → PixelMap 创建。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 两个独立函数 | `CreatePixelMapFromSurface` vs `CreatePixelMapFromSurfaceBuffer` | 区分是否需要自行管理 Buffer 生命周期 |
| transformEnabled 参数 | `bool transformEnabled = false` | 默认不应用变换，截图通常需要屏幕原始方向 |
| 返回 shared_ptr | `std::shared_ptr<Media::PixelMap>` | 自动管理 PixelMap 生命周期 |
| 独立 fuzz 测试 | surface / surfacegl 两个 fuzzer | 覆盖 CPU 和 GPU 两种缓冲区来源 |

## 待补充背景

- GPU SurfaceBuffer 的像素读取路径（是否需要 GPU → CPU 回读）
- srcRect 越界时的处理行为
- CreatePixelMapFromSurface 中 AcquireBuffer 和 ReleaseBuffer 的时序
- transformEnabled 启用时的具体变换逻辑
- 与 Render Service 截图（capture）模块的关系
