# 2D 图片 / 位图 / Pixmap

## 适用范围

- Bitmap 像素缓冲区创建与读写
- Pixmap 轻量像素视图（不持有内存）
- Image GPU/CPU 图像对象及编码
- ImageInfo 描述像素格式、颜色类型、Alpha 类型
- YUV 数据表示与转换
- GPUContext 图形上下文管理

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| bitmap.h | `rosen/modules/2d_graphics/include/image/bitmap.h` | Bitmap 像素缓冲区：Build/ReadPixels/PeekPixels/MakeImage |
| pixmap.h | `rosen/modules/2d_graphics/include/image/pixmap.h` | Pixmap 轻量像素视图：GetColor/ScalePixels |
| image.h | `rosen/modules/2d_graphics/include/image/image.h` | Image 对象：GPU/CPU 图像、压缩纹理、YUV |
| image_info.h | `rosen/modules/2d_graphics/include/image/image_info.h` | ImageInfo：宽高/ColorType/AlphaType/ColorSpace |
| gpu_context.h | `rosen/modules/2d_graphics/include/image/gpu_context.h` | GPUContext：GPU 渲染上下文 |
| yuv_info.h | `rosen/modules/2d_graphics/include/image/yuv_info.h` | YUVInfo：YUV 格式描述 |
| picture.h | `rosen/modules/2d_graphics/include/image/picture.h` | Picture 录制回放对象 |
| trace_memory_dump.h | `rosen/modules/2d_graphics/include/image/trace_memory_dump.h` | GPU 内存追踪 |

## 核心模型

**Bitmap** 持有实际像素内存，通过 `BitmapImpl` 适配 Skia：
- `Build(width, height, BitmapFormat)` 或 `Build(ImageInfo)` 分配像素
- `BitmapFormat` 由 `ColorType` + `AlphaType` 组成
- `ReadPixels` 从 Bitmap 拷贝指定区域像素到外部缓冲
- `PeekPixels` 获取 Pixmap 视图（不拷贝）
- `InstallPixels` 将外部内存挂载到 Bitmap
- `MakeImage` 从 Bitmap 创建 Image（零拷贝，不复制像素）
- `TryAllocPixels` 尝试分配像素内存
- `SetImmutable` / `IsImmutable` 不可变标记
- 序列化：`Serialize` / `Deserialize`

**Pixmap** 是轻量像素视图，不持有内存，仅引用外部数据：
- 构造：`Pixmap(ImageInfo, void* addr, rowBytes)`
- `GetColor(x, y)` 获取像素颜色
- `ScalePixels` 缩放像素到目标 Pixmap
- `GetAddr` / `GetRowBytes` / `GetWidth` / `GetHeight` 访问基础信息

**Image** 表示可在 GPU 或 CPU 上使用的图像对象：
- 支持压缩类型：ETC2、BC1、ASTC 等（`CompressedType` 枚举）
- 支持位深：`BitDepth::KU8` / `BitDepth::KF16`
- 支持 YUV 数据（`YUVInfo`）
- 支持纹理原点：`TextureOrigin::TOP_LEFT` / `BOTTOM_LEFT`

数据流：外部像素数据 → Bitmap（持有内存）→ Pixmap（只读视图）→ Image（绘制用对象）→ Canvas 绘制。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| Bitmap 与 Pixmap 分离 | Bitmap 持有内存，Pixmap 仅引用 | Pixmap 可零拷贝传递像素信息，减少内存分配 |
| BitmapImpl 适配层 | `std::shared_ptr<BitmapImpl> bmpImplPtr` | 隔离 Skia 实现细节，支持后端替换 |
| MakeImage 零拷贝 | `Bitmap::MakeImage()` 文档说明"never copy Pixels" | 大图场景避免不必要的内存拷贝 |
| InstallPixels 外部内存 | `InstallPixels(info, pixels, rowBytes, releaseProc)` | 允许调用方管理像素内存生命周期 |
| Image 压缩纹理支持 | `CompressedType` 枚举 + `BitDepth` | 支持 GPU 压缩纹理格式，减少显存占用 |
| Pixmap::ScalePixels | `ScalePixels(dst, SamplingOptions)` | 直接在 Pixmap 层面进行缩放，简化图像处理流程 |

## 待补充背景

- Bitmap 到 Image 的 GPU 纹理上传路径与缓存策略
- Image 的 YUV 数据在 GPU 后端的处理方式
- GPUContext 与 Image 的关系（GPU 纹理生命周期管理）
- CompressedType 各格式在不同 GPU 后端的兼容性
- Picture 录制回放与 Bitmap/Image 的交互流程
