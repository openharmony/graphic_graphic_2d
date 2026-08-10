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

## 补充背景

### Bitmap 到 Image 的 GPU 纹理上传路径与缓存

- CPU 路径：`BuildFromBitmap(bitmap, ignoreAlpha)` 不需 GPUContext；`MakeFromRaster(Pixmap, releaseProc, ctx)` 共享外部像素。
- GPU 上传：`BuildFromBitmap(GPUContext&, bitmap, ignoreAlpha)` 把像素上传到 GPU 后端生成纹理 Image。
- 状态查询：`IsTextureBacked()` 是否 GPU 纹理；`IsLazyGenerated()` 是否惰性生成；`GetROPixels(bitmap)` 在非 lazy 时直接取回 CPU 像素。
- 缓存提示：`HintCacheGpuResource()` 指示引擎在纹理创建时尝试缓存 GPU 资源。
- 子集：`BuildSubset(image, rect, GPUContext&)` 取子图，纹理后端 Image 必须传匹配的 context；raster-backed 子集会被转为 texture-backed。

### Image 的 YUV 数据在 GPU 后端的处理

- 入口：`MakeFromYUVAPixmaps(GPUContext&, YUVInfo&, void* memory, ColorSpace)`，需 GPU context。
- `YUVInfo` 描述：`PlaneConfig`（Y_UV / Y_VU 两平面）、`SubSampling`（K420）、`YUVColorSpace`（JPEG_FULL / BT2020_10BIT_LIMITED / IDENTITY）、`YUVDataType`（UNORM_8 / UNORM_16）。
- Vulkan 路径：`VKYcbcrConversionInfo` 携带 sampler YCbcr model/range/chroma offset/filter 等，`VKTextureInfo.ycbcrConversionInfo` 在纹理侧携带 YUV 转换信息，依赖 Vulkan `VK_KHR_sampler_ycbcr_conversion`。

### GPUContext 与 Image 的关系（GPU 纹理生命周期管理）

- GPUContext 是 GPU 资源上下文，所有 GPU Image 构造路径（Bitmap/Compressed/Texture/Surface/Subset/YUVA）都需传入它。
- 回收：`PerformDeferredCleanup(ms)`、`PurgeUnlockedResources`、`PurgeUnlockedResourcesByTag/Pid`、`PurgeCacheBetweenFrames`、`FreeGpuResources`、`ReclaimResources`、`PurgeUnlockAndSafeCacheGpuResources`。
- 限额/统计：`Get/SetResourceCacheLimits`、`GetResourceCacheUsage`、`SetPurgeableResourceLimit`、`DumpGpuStats`、`DumpMemoryStatistics(ByTag)`。
- 标签回收：`GPUResourceTag`（pid/tid/wid/fid/fName/sid）+ `SetCurrentGpuResourceTag` 支持按进程/窗口/场景回收。
- 提交：`Flush` / `FlushCommands` / `Submit` / `FlushAndSubmit`；缓存由 `GPUContextOptions::PersistentCache`（实践中为 `ShaderCache`）落盘。
- 生命周期：Image 的 GPU 纹理随 GPUContext 缓存，由其 purge/free 接口统一回收；context 失效后用 `IsValid(context)` 校验纹理可用性。

### CompressedType 各格式在 GPU 后端的兼容性

- 枚举：`ETC2_RGB8_UNORM`（兼容 ETC1）、`BC1_RGB8_UNORM`、`BC1_RGBA8_UNORM`、`ASTC_RGBA8_4x4/6x6/8x8`、`ASTC_RGBA10_4x4`。
- 入口：`BuildFromCompressed(GPUContext&, data, w, h, CompressedType, ColorSpace)`，需 GPU context。
- 兼容性由 Skia `GrBackendFormat` 判定：ETC2 偏移动 GPU（Mali/Adreno），BC1 偏桌面，ASTC 兼容性较广但块大小不同；后端不支持时 `BuildFromCompressed` 返回 false，Drawing 层无显式 fallback 表。
- `TextureInfo.format` 与 `BackendTexture` 用于描述底层纹理，跨后端（GL/VK）由各自 TextureInfo 字段承载。

### Picture 录制回放与 Bitmap/Image 的交互

- `Picture` 头文件极简：`Serialize/Deserialize`、`ApproximateOpCount(nested)`、`Serialize(SerialProcs*)` 支持外部序列化回调；回放入口由 Canvas 侧 `drawPicture` 触发，Picture 本身不暴露 Playback。
- 流程：录制阶段绘制命令序列化进 Picture → 回放时 Canvas 在目标 Surface 上执行命令 → 若目标是 GPU Surface 则生成 GPU 纹理，可经 Surface/`GetBackendTexture` 转 Image。
- 与 Bitmap/Image：`Image::MakeFromPicture` 类入口由 Skia `SkPicture::makeImage` 走 raster 路径生成，Picture → Image 通常经 Canvas 回放截图。
