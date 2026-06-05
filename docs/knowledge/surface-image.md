# SurfaceImage

## 适用范围

- SurfaceImage 将 SurfaceBuffer 转为 OpenGL 纹理
- EGLImage 创建与生命周期管理
- 从 ConsumerSurface 获取缓冲区并更新纹理
- 变换矩阵获取（GetTransformMatrix / GetTransformMatrixV2 / GetBufferMatrix）
- 缓冲区可用监听（OnBufferAvailableListener）
- NativeWindowBuffer 获取与释放
- 颜色空间查询
- 丢帧模式（dropFrameMode）

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| surface_image.h | `frameworks/surfaceimage/include/surface_image.h` | SurfaceImage 类定义 |
| SurfaceImageListener | `frameworks/surfaceimage/include/surface_image.h` | 缓冲区可用监听器 |
| BUILD.gn | `frameworks/surfaceimage/BUILD.gn` | 构建配置 |
| src/ | `frameworks/surfaceimage/src/` | 实现源文件 |
| test/ | `frameworks/surfaceimage/test/` | 测试 |

## 核心模型

**SurfaceImage** 继承自 `ConsumerSurface`，将 Surface 生产的图形缓冲区桥接到 OpenGL 纹理：

```
生产者 (SurfaceProducer)
  → QueueBuffer
  → ConsumerSurface::AcquireBuffer
  → SurfaceImage::UpdateSurfaceImage()
  → 创建 EGLImage → 绑定到 GL_TEXTURE_EXTERNAL_OES
  → 应用消费纹理

SurfaceImage 核心字段:
  textureId_        → OpenGL 纹理 ID
  textureTarget_    → GL_TEXTURE_EXTERNAL_OES (默认)
  eglDisplay_       → EGL 显示连接
  eglContext_       → EGL 上下文
  imageCacheSeqs_   → 按 seqNum 缓存的 EGLImage + EGLSync
  currentSurfaceImage_  → 当前图像序列号
  currentSurfaceBuffer_ → 当前 SurfaceBuffer
  currentTransformMatrix_[16]  → 变换矩阵
  colorSpace_       → OH_NativeBuffer_ColorSpace
  dropFrameMode_    → 丢帧模式开关
  isReleased_       → 释放状态
```

**BufferProperties** 结构追踪缓冲区属性变化：
- `crop`：有效区域
- `transformType`：旋转/变换类型
- `bufferWidth` / `bufferHeight`：缓冲区尺寸
- 用于检测缓冲区属性是否变化，仅在变化时重建 EGLImage

**关键流程**：
1. `InitSurfaceImage()` → 初始化 ConsumerSurface
2. `UpdateSurfaceImage()` → AcquireBuffer → 创建/更新 EGLImage → 更新纹理
3. `GetTransformMatrix(matrix)` → 获取当前变换矩阵
4. `AttachContext(textureId)` / `DetachContext()` → 绑定/解绑 GL 上下文
5. `AcquireNativeWindowBuffer` / `ReleaseNativeWindowBuffer` → 获取原生窗口缓冲区

**SurfaceImageListener** 实现 `IBufferConsumerListener`，在缓冲区可用时回调 `OnBufferAvailable`。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 继承 ConsumerSurface | `class SurfaceImage : public ConsumerSurface` | 复用 BufferQueue 的生产消费模型 |
| GL_TEXTURE_EXTERNAL_OES | `textureTarget_ = GL_TEXTURE_EXTERNAL_OES` | OES 纹理支持 YUV 直接采样，无需 CPU 转换 |
| ImageCacheSeq 缓存 | `std::map<uint32_t, ImageCacheSeq> imageCacheSeqs_` | 按 seqNum 缓存 EGLImage 避免每帧重建 |
| BufferProperties 变化检测 | `bufferProperties_ != preBufferProperties_` | 仅在缓冲区属性变化时重建 EGLImage |
| EGLSync 同步 | `ImageCacheSeq.eglSync_` | 确保 GPU 操作完成后再释放缓冲区 |
| 丢帧模式 | `dropFrameMode_` + `SetDropBufferSwitch` | 某些场景（如后台）可开启丢帧减少 GPU 负载 |
| 释放状态追踪 | `isReleased_` + `ReleaseTextImage` | 防止释放后继续使用 EGLImage |

## 待补充背景

- EGLImage 创建中 SurfaceBuffer 到 EGLImage 的格式映射
- ImageCacheSeq 的缓存淘汰策略
- GL_TEXTURE_EXTERNAL_OES 与 GL_TEXTURE_2D 的选择条件
- SurfaceImage 在 WebGL 场景下的使用方式
- dropFrameMode 的具体丢帧策略
