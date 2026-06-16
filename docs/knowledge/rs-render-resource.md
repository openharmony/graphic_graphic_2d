# 渲染资源与图片缓存

## 适用范围

- RSImage 和 RSImageBase 的图片数据模型
- RSImageCache 图片缓存机制（Drawing::Image、PixelMap）
- RSFilterCacheManager 滤镜缓存（另见 rs-ui-effect.md）
- 图片压缩数据和 HDR 转换
- GPU 图片资源生命周期管理
- Typeface 缓存
- 帧间内存回收

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 图片基类 | `rosen/modules/render_service_base/include/render/rs_image_base.h` | RSImageBase：Marshall/Unmarshall、CanvasDrawImage |
| 图片类 | `rosen/modules/render_service_base/include/render/rs_image.h` | RSImage：ImageFit/ImageRepeat/HDR、CanvasDrawImage |
| 图片缓存 | `rosen/modules/render_service_base/include/render/rs_image_cache.h` | RSImageCache：Drawing::Image 和 PixelMap 缓存 |
| 滤镜缓存 | `rs_filter_cache_manager.h` | RSFilterCacheManager：滤镜结果缓存（详见 rs-ui-effect.md） |
| 滤镜缓存内存控制 | `rs_filter_cache_memory_controller.h` | 缓存内存上限管理 |
| Typeface 缓存 | `rosen/modules/render_service_base/include/render/rs_typeface_cache.h` | RSTypefaceCache：字体缓存 |
| Memory Track | `rosen/modules/render_service_base/include/memory/rs_memory_track.h` | RSMemoryTrack：GPU 内存追踪 |
| Memory Snapshot | `rs_memory_snapshot.h` | RSMemorySnapshot：内存快照 |
| DfxString | `rs_dfx_string.h` | 内存 Dfx 日志 |

## 核心模型

### RSImage 数据模型

RSImage 代表一个可渲染的图片对象，支持多种数据源：

1. **PixelMap**：来自 Media::PixelMap 的位图数据。
2. **Drawing::Image**：GPU 纹理图片。
3. **压缩数据**：通过 `SetCompressData()` 设置的压缩格式数据。

关键属性：
- `imageFit_`：图片适配模式（FILL/CONTAIN/COVER/FIT_WIDTH/FIT_HEIGHT/NONE/SCALE_DOWN/MATRIX 等）。
- `imageRepeat_`：重复模式（NO_REPEAT/REPEAT_X/REPEAT_Y/REPEAT）。
- `orientationFit_`：方向适配（NONE/VERTICAL_FLIP/HORIZONTAL_FLIP）。
- `dynamicRangeMode_`：动态范围模式（SDR/HDR）。
- `fitMatrix_`：自定义适配矩阵。
- `innerRect_`：内部裁剪矩形。

### RSImageCache 缓存机制

RSImageCache 是单例，管理跨 RSImage 实例的图片资源复用：

1. **Drawing::Image 缓存**（`drawingImageCache_`）：
   - Key：uniqueId（通常为 PixelMap 的 uniqueId）。
   - Value：`pair<shared_ptr<Drawing::Image>, uint64_t>`（图片 + 引用计数）。
   - Unmarshalling 时引用计数 +1，RSImage 析构时 -1，归零时移除。

2. **PixelMap 缓存**（`pixelMapCache_`）：
   - 结构同上，缓存 PixelMap 对象避免重复解码。

3. **按 PixelMapId 关联的 Drawing::Image 缓存**（`pixelMapIdRelatedDrawingImageCache_`）：
   - 同一 PixelMap 在不同线程可能生成不同 Drawing::Image（GPU 上下文不同）。
   - 按 `(pixelMapId, tid)` 二级索引。

4. **可编辑 PixelMap 缓存**（`editablePixelMapCache_`）：
   - 独立的缓存表，用于可编辑图片。
   - `DiscardEditablePixelMapCache()` 丢弃但不释放，`ReleaseEditablePixelMapCache()` 释放。

### 图片绘制流程

```
RSImage::CanvasDrawImage(canvas, rect, samplingOptions)
  → ApplyImageFit()                  // 计算 ImageFit 矩阵
  → GetAdaptiveImageInfo()           // 获取适配信息
  → HDRConvert() (if needed)         // HDR 转换
  → canvas.DrawImage(image, rect)    // 绘制到画布
```

### HDR 转换

- `IsHDRUiCapture()` / `IsEDRSurface()`：判断是否为 HDR 内容。
- `HDRConvert()`：SDR↔HDR 色彩空间转换。
- `SetDynamicRangeMode()`：设置动态范围偏好。

### 引用计数与生命周期

- RSImage Unmarshalling 时从 RSImageCache 获取或创建 Drawing::Image/PixelMap，引用计数 +1。
- RSImage 析构时调用 `ReleaseDrawingImageCache()` / `ReleasePixelMapCache()`，引用计数 -1。
- 引用计数归零时从缓存表移除，资源可被回收。
- `CheckRefCntAndReleaseImageCache()` 检查并释放多余引用。

### GPU 资源回收

- RSImage 关联 `NodeId`，通过 `SetNodeId()` 记录。
- Vulkan 模式下 `RemoveImageMemForWindow()` 在窗口销毁时释放关联的 GPU 图片资源。
- `ReserveImageInfo()` 保留图片信息用于后续清理。

## PixelMap / Image / SurfaceBuffer 生命周期

### PixelMap 到 RSImage

PixelMap 常见进入 RS 的路径有两类：

1. 应用侧设置图片属性，随 `RSImage::Marshalling()` 进入 transaction。
2. Render Service 通过 `CreatePixelMapFromSurface()` 从 Surface 或 SurfaceBuffer 回读。

读取链路重点看：

- `RSImage::Unmarshalling()`：读取 uniqueId、尺寸、nodeId 和 PixelMap 数据。
- `RSImageCache::CachePixelMap()` / `GetPixelMapCache()`：按 uniqueId 缓存 PixelMap。
- `RSImageCache::IncreasePixelMapCacheRefCount()`：反序列化复用时增加引用。
- `RSImageCache::ReleasePixelMapCache()`：RSImage 生命周期结束后减少引用。
- `RSImageCache::CheckRefCntAndReleaseImageCache()`：处理缓存引用和输入 PixelMap 的一致性。

修改 PixelMap 传输时，必须确认 uniqueId 不是安全边界。
它只能作为缓存键，不能证明图片内容可信、尺寸合法或调用方有权限。

### Drawing::Image 缓存

`Drawing::Image` 可能来自 PixelMap，也可能来自 GPU texture。缓存有两层语义：

- `drawingImageCache_`：按 uniqueId 共享 `Drawing::Image` 和引用计数。
- `pixelMapIdRelatedDrawingImageCache_`：按 `(pixelMapId, tid)` 缓存渲染线程上的 `Drawing::Image`。

后者存在是因为不同线程或 GPU context 下的 texture 资源不一定可共享。
修改渲染线程、并行渲染、UIFirst 或 Vulkan/GLES 分支时，
要确认缓存键包含正确线程或 context 维度。

### SurfaceBuffer 绘制

SurfaceBuffer 不走普通 PixelMap 引用计数模型，重点是 buffer、fence、回调和释放顺序：

```text
ExtendRecordingCanvas::DrawSurfaceBuffer()
  -> DrawSurfaceBufferOpItem::Marshalling()
  -> CmdListHelper::AddSurfaceBufferEntryToCmdList()
  -> DrawSurfaceBufferOpItem::Playback()
  -> DrawWithVulkan() / DrawWithGles()
  -> OnAfterDraw() / ReleaseBuffer()
```

关键点：

- `Drawing::SurfaceBufferEntry` 同时携带 `SurfaceBuffer` 和 acquire fence。
- `WaitFence()` 只允许有界等待，不能因异常 fence 阻塞渲染线程。
- `DrawSurfaceBufferOpItem::OnAfterDraw()` 会创建或传递 release fence。
- `DrawSurfaceBufferOpItem::ReleaseBuffer()` 触发 finish callback，不能提前释放仍被 GPU 使用的 buffer。
- `DrawHybridPixelMapOpItem` 也会使用 fence，需要覆盖 PixelMap 和 SurfaceBuffer 混合输入。

### 从 Surface 创建 PixelMap

`CreatePixelMapFromSurface()` 跨越 client、service 和 render process：

```text
RSRenderServiceClient::CreatePixelMapFromSurfaceId()
  -> RSClientToServiceConnection::CreatePixelMapFromSurface()
  -> RSServiceToRenderConnection::CreatePixelMapFromSurface()
  -> RSRenderPipelineAgent::CreatePixelMapFromSurface()
  -> Rosen::CreatePixelMapFromSurface()
```

检查点：

- `Surface` 为空、producer 为空、srcRect 非法、transformEnabled 组合都要有明确返回。
- Proxy 写入和 Stub 读取 `srcRect` 字段顺序必须一致。
- 返回的 PixelMap 为空时不要继续访问尺寸、像素地址或 HDR 元数据。
- 真实设备上需要覆盖屏幕旋转、虚拟屏、HDR surface 和 protected layer 场景。

## 缓存修改检查清单

- 新增缓存键时说明维度：uniqueId、nodeId、pid/tid、GPU context、ColorSpace、动态范围。
- 引用计数增加和释放必须成对，失败路径不得泄漏 cache entry。
- 释放 GPU 资源时确认是否在正确线程和正确 graphics context 下执行。
- 可编辑 PixelMap 和不可编辑 PixelMap 不得共用同一缓存表。
- SurfaceBuffer 绘制路径必须覆盖 acquire fence 超时、release fence 创建失败和回调为空。
- HDR/EDR、PixelFormat、ColorSpace、row bytes、stride 不能只按宽高推断。
- 设备截图或 capture 相关改动要补真实设备验证，不能只依赖单元测试。
- 没有真实设备时，记录设备验证缺口，不默认阻塞文档、静态验证或可本地验证的小修。
  但不能把截图、HDR、SurfaceBuffer 或 GPU 资源行为描述为完整设备验证。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 引用计数缓存 | `drawingImageCache_` value 中的 uint64_t refCount | 多个 RSImage 可共享同一 Drawing::Image，避免重复上传 GPU 纹理 |
| 线程级 Drawing::Image 缓存 | `pixelMapIdRelatedDrawingImageCache_` 按 pid 索引 | 不同 GPU 上下文/线程的 Drawing::Image 不可共享，需按线程隔离 |
| 可编辑/不可编辑分离 | `editablePixelMapCache_` vs `pixelMapCache_` | 可编辑 PixelMap 可能被修改，需独立管理避免污染共享缓存 |
| ImageFit 矩阵 | `fitMatrix_` + `ApplyImageFit()` | 统一适配算法，支持 MATRIX 模式自定义变换 |
| NodeId 关联 | `SetNodeId()` | 窗口/节点销毁时批量释放关联图片资源 |
| 延迟释放 | `editablePixelMapCacheToRelease_` | 避免在渲染帧中释放 PixelMap 导致 GPU 使用中的纹理被销毁 |

## 待补充背景

- RSImage 压缩数据的具体格式和解码流程。
- HDR 转换的完整色彩空间映射策略。
- RSTypefaceCache 的缓存键和字体资源管理。
- RSFilterCacheMemoryController 的内存上限配置和回收策略。
- 跨进程图片传递时 RSImageCache 的共享和同步机制。
