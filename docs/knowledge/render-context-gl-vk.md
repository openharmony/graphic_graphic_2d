# Render Context GL / VK

## 适用范围

- RenderContext 渲染上下文抽象基类与工厂方法
- RenderContextGL 基于 EGL/OpenGL ES 的渲染上下文
- RenderContextVK 基于 Vulkan 的渲染上下文
- ShaderCache 着色器磁盘缓存与版本/失效策略
- MemoryHandler GPU 内存与缓存目录管理
- CacheData 着色器键值存储与磁盘序列化
- GPUContext 与 Drawing GPU 上下文的桥接
- UniRenderMode 对缓存目录与 GPU 上下文选项的影响

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| render_context.h | `rosen/modules/2d_graphics/src/render_context/render_context.h` | RenderContext 抽象基类 |
| render_context.cpp | `rosen/modules/2d_graphics/src/render_context/render_context.cpp` | `Create()` 工厂与 `ConvertColorGamutToColorSpace` |
| render_context_gl.h | `rosen/modules/2d_graphics/src/render_context/new_render_context/render_context_gl.h` | RenderContextGL：EGL/GLES 实现 |
| render_context_gl.cpp | `rosen/modules/2d_graphics/src/render_context/new_render_context/render_context_gl.cpp` | EGL 初始化、Surface、Damage、ShareContext |
| render_context_vk.h | `rosen/modules/2d_graphics/src/render_context/new_render_context/render_context_vk.h` | RenderContextVK：Vulkan 实现 |
| render_context_vk.cpp | `rosen/modules/2d_graphics/src/render_context/new_render_context/render_context_vk.cpp` | 委托 RsVulkanContext 创建 GPUContext |
| shader_cache.h | `rosen/modules/2d_graphics/src/render_context/shader_cache.h` | ShaderCache：`PersistentCache` 单例 |
| shader_cache.cpp | `rosen/modules/2d_graphics/src/render_context/shader_cache.cpp` | Load/Store/磁盘写入/清理 |
| shader_cache_utils.h | `rosen/modules/2d_graphics/src/render_context/shader_cache_utils.h` | 预加载缓存目录工具（wearable） |
| memory_handler.h | `rosen/modules/2d_graphics/src/render_context/memory_handler.h` | MemoryHandler：配置 GPUContextOptions |
| memory_handler.cpp | `rosen/modules/2d_graphics/src/render_context/memory_handler.cpp` | `ConfigureContext`/`ClearRedundantResources` |
| cache_data.h | `rosen/modules/2d_graphics/src/render_context/cache_data.h` | CacheData：键值存储与序列化 |
| render_context_egl_defines.h | `rosen/modules/2d_graphics/src/render_context/render_context_egl_defines.h` | iOS 临时 EGL 类型占位 |
| render_context_log.h | `rosen/modules/2d_graphics/src/render_context/render_context_log.h` | LOGD/LOGI/LOGW/LOGE |
| rs_vulkan_context.h | `rosen/modules/render_service_base/src/platform/ohos/backend/rs_vulkan_context.h` | RsVulkanContext：按 RenderEngineType 单例 |
| rs_engine_header_ext.h | `rosen/modules/render_service_base/include/platform/ohos/backend/rs_engine_header_ext.h` | `RenderEngineType` 枚举定义 |
| BUILD.gn | `rosen/modules/render_service_base/src/platform/ohos/BUILD.gn` | 按 VK/GL/feature 编译裁剪 |

## 核心模型

### 类层次与后端选择

`RenderContext` 是抽象基类，只声明虚接口与公共字段，由 `Create()` 工厂在运行期决定具体后端：

```
RenderContext (抽象基类, render_context.h)
  ├─ static Create()        → 按 RSSystemProperties::IsUseVulkan() 选择后端
  ├─ 纯虚: Init/AbandonContext/GetType/SetUpGpuContext/
  │         QueryMaxGpuBufferSize/CreateDrawingGPUContext/ReleaseDrawingGPUContext
  ├─ EGL 默认实现 {return nullptr/0}: AcquireSurface/RenderFrame/DamageFrame/
  │         ClearRedundantResources/DestroyShareContext/QueryEglBufferAge/SetRenderContextType
  ├─ 静态 ConvertColorGamutToColorSpace(GraphicColorGamut)
  └─ 公共字段: drGPUContext_/surface_/colorSpace_/pixelFormat_/
              isUniRender_/isUniRenderMode_/mHandler_/shareContextMutex_
        ├─ RenderContextGL (render_context_gl.h/cpp): EGL/GLES 实现
        └─ RenderContextVK (render_context_vk.h/cpp): Vulkan 实现, 委托 RsVulkanContext
```

`RenderContext::Create()`（render_context.cpp:29）后端选择逻辑：

```cpp
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        return std::make_shared<RenderContextVK>();
    }
#endif
    return std::make_shared<RenderContextGL>();
```

- 编译期由 `RS_ENABLE_VK` 控制是否注册 VK 分支；运行期由 `RSSystemProperties::IsUseVulkan()` 决定是否使用 VK，否则回退 GL。
- `RSSystemProperties::IsUseVulkan()` 在 `rs_base_render_engine.cpp`、`rs_render_composer.cpp`、`rs_surface_capture_task.cpp`、`rs_sub_thread.cpp`、`rs_divided_ui_capture.cpp`、`drawing_surface_utils.cpp`、`gpu_context.cpp` 等多处分支中决定后端相关行为。

`RenderEngineType`（rs_engine_header_ext.h:21）按渲染用途区分上下文：

| 取值 | 含义 | 主要使用方 |
| --- | --- | --- |
| `BASIC_RENDER` | 基础渲染（默认） | RSRenderThread 客户端渲染、UniRenderThread |
| `PROTECTED_REDRAW` | 受保护内容重绘 | `RSBaseRenderEngine` 在 `IS_ENABLE_DRM` 时为安全层创建独立上下文 |
| `UNPROTECTED_REDRAW` | 非受保护内容重绘 | Composer 线程重绘 |
| `MAX_INTERFACE_TYPE` | 边界哨兵 | 类型校验 |

### RenderContextGL：EGL/GLES 实现

关键字段（render_context_gl.h）：

| 字段 | 含义 |
| --- | --- |
| `eglDisplay_` / `eglContext_` / `eglSurface_` | EGL 三件套，默认 `EGL_NO_*` |
| `pbufferSurface_` | 1×1 Pbuffer，无窗口时承载离屏 GL 上下文 |
| `eglShareContext_` | 多线程共享上下文 |
| `config_` / `nativeWindow_` | EGLConfig 与原生窗口 |
| `UNIRENDER_CACHE_DIR` | `"/data/service/el0/render_service"` |
| iOS 专属: `color_space_`/`layer_`/`resourceContext`/`framebuffer_`/`colorbuffer_` | iOS 平台 CAEAGLLayer 适配 |

初始化流程 `Init(type, cacheDir)`（render_context_gl.cpp:142）：

1. `IsEglContextReady()` 守护，已就绪直接返回 true。
2. `GetPlatformEglDisplay(EGL_PLATFORM_OHOS_KHR, EGL_DEFAULT_DISPLAY, NULL)` 取 EGLDisplay；优先用 `eglGetPlatformDisplayEXT`（Wayland 扩展可用时），否则退化到 `eglGetDisplay`。
3. `eglInitialize` 初始化主次版本号 → `eglBindAPI(EGL_OPENGL_ES_API)`。
4. `eglChooseConfig` 选 8/8/8/8 RGBA、`EGL_WINDOW_BIT`、`EGL_OPENGL_ES3_BIT` 可渲染配置。
5. `eglCreateContext`（`EGL_CONTEXT_CLIENT_VERSION=2`）创建主上下文。
6. `CreatePbufferSurface()`：仅在未支持 `EGL_KHR_surfaceless_context` 时创建 1×1 Pbuffer 作为离屏 make current 目标。
7. `eglMakeCurrent(display, pbuffer_, pbuffer_, ctx_)`。
8. 委托 `SetUpGpuContext(cacheDir)` 构建 Drawing GPUContext。

`SetUpGpuContext`（render_context_gl.cpp:199）：

- 已创建 `drGPUContext_` 时直接返回。
- 新建 `MemoryHandler`，读取 `glGetString(GL_VERSION)` 作为 identity。
- `isUniRenderMode_` 为真时强制使用 `UNIRENDER_CACHE_DIR`，并把 `isUniRenderMode_` 传入 `GPUContextOptions` 和 `MemoryHandler::ConfigureContext`。
- `Drawing::GPUContext::BuildFromGL(options)` 构建基于 GL 的 GPU 上下文。

EGL 相关辅助 API：

- `CreateEGLSurface(nativeWindow)`：先 `eglMakeCurrent(NO_SURF, NO_SURF, NO_CTX)` 脱离 Pbuffer，再 `eglCreateWindowSurface`。
- `MakeCurrent(surface, ctx)`：surface/ctx 为空时回退到 Pbuffer 和主 context，副作用是同步更新 `eglSurface_`。
- `SwapBuffers(surface)`：带 `RS_TRACE_FUNC()` 的 `eglSwapBuffers`。
- `RenderFrame()`：调用 `surface_->GetCanvas()->Flush()` 提交绘制命令，外层包 `RSTagTracker(TAG_RENDER_FRAME)`。
- `DamageFrame(rects)`：通过 `eglSetDamageRegionKHR`（`EGL_KHR_partial_update`）设置损坏区域；用 `EGL_BUFFER_AGE_KHR` 的 buffer age 配合实现局部更新。
- `QueryEglBufferAge()`：返回 `EGL_BUFFER_AGE_KHR`，无显示/无 surface 时返回 `EGL_UNKNOWN`。
- `ClearRedundantResources()`：`drGPUContext_->Flush()` + `PerformDeferredCleanup(10s)`，回收 10 秒未用的 GPU 资源。
- `CreateShareContext()` / `DestroyShareContext()`：`eglCreateContext(display, config, eglContext_, ...)`，配 `shareContextMutex_` 保护，多线程共享资源。
- `CreateDrawingGPUContext(cacheDir)`：独立创建一份 GPUContext（含独立 ShareContext 与 MemoryHandler），用于需要独立 GPU 上下文的子线程/子模块；与 `SetUpGpuContext` 共用 GL 后端但 GPUContext 实例不同。
- `ReleaseDrawingGPUContext(gpuContext)`：仅 `eglMakeCurrent(NO_SURF, NO_SURF, NO_CTX)` 释放当前线程绑定。
- `QueryMaxGpuBufferSize(maxW, maxH)`：取 `GL_MAX_TEXTURE_SIZE` 和 `GL_MAX_RENDERBUFFER_SIZE` 较小值，作为可渲染最大尺寸。
- `AcquireSurface(width, height)`：根据 `colorSpace_` 选 `Drawing::ColorSpace`，按 `pixelFormat_`（默认 RGBA_8888，`GRAPHIC_PIXEL_FMT_RGBA_1010102` 走 `GL_RGB10_A2`）填充 `Drawing::FrameBuffer`，`surface_->Bind(bufferInfo)` 返回 Drawing Surface。
- iOS 分支：`ResourceMakeCurrent`/`GetResourceContext` 用静态 `resourceContext` 与 `resourceContextMutex`，`UpdateStorageSizeIfNecessary` 调整 `storage_width_/storage_height_`、绑定 `framebuffer_`/`colorbuffer_`。
- ARKUI_X 分支：`AddSurface`/`DeleteSurface`/`DestroySharedSource`/`SetCleanUpHelper`，配合 `surface_count_` 与 `cleanUpHelper_` 管理跨平台窗口生命周期。

析构（render_context_gl.cpp:119）：依次 `eglDestroyContext`、销毁 Pbuffer、`eglMakeCurrent(NO_*)`、`eglTerminate`、`eglReleaseThread`，并将所有 EGL 句柄复位为 `EGL_NO_*`。

### RenderContextVK：Vulkan 实现

RenderContextVK 体积很小，只是 `RsVulkanContext` 的薄封装，所有 GPU 资源创建与查询都按 `contextType_` 路由到对应单例：

```cpp
bool Init(type, cacheDir)          // render_context_vk.cpp:33
  → contextType_ = type;
  → SetUpGpuContext(cacheDir)      // 已有 drGPUContext_ 时直接返回
  → CreateDrawingGPUContext(cacheDir)

bool AbandonContext()              // FlushAndSubmit + PurgeUnlockAndSafeCacheGpuResources
bool QueryMaxGpuBufferSize(...)    → RsVulkanContext::Get(contextType_).QueryMaxGpuBufferSize(...)
CreateDrawingGPUContext(cacheDir)  → RsVulkanContext::Get(contextType_).CreateDrawingGPUContext(...)
ReleaseDrawingGPUContext(ctx)      → RsVulkanContext::Get(contextType_).ReleaseDrawingGPUContext(ctx)
```

- `RsVulkanContext::Get(RenderEngineType)`（rs_vulkan_context.h:35）按 `RenderEngineType` 维护各自的单例；构造参数 `isProtected`/`isHtsEnable` 决定是否走受保护队列、是否启用 HTS。
- VK 不直接持 EGL 资源，所以 `RenderContextGL` 的 EGL 默认实现（`AcquireSurface`/`RenderFrame`/`DamageFrame` 等）在 VK 路径下不会被调用；VK 侧的 Surface 与帧提交由 `RSSurfaceOhosVulkan`、`RSBaseRenderEngine` 的 VK 分支负责。
- `GetType()` 返回 `contextType_`（GL 总是返回 `BASIC_RENDER`），便于上层按类型选择 VK 单例。
- 析构仅置空 `drGPUContext_`；真正的 Vulkan 资源释放由 `RsVulkanContext` 单例与 `AbandonContext()` 完成，避免进程内重复初始化 Vulkan。
- Composer 的 `RSBaseRenderEngine::Init`（rs_base_render_engine.cpp:193）会按 `RenderEngineType` 同时创建并初始化最多两份 RenderContext（`renderContext_` + `protectedRenderContext_`），分别绑定到 `UNPROTECTED_REDRAW` 与 `PROTECTED_REDRAW` 单例。

### ShaderCache：磁盘持久化与失效

`ShaderCache`（shader_cache.h:27）继承 `Drawing::GPUContextOptions::PersistentCache`，单例（`Instance()`），由 Drawing GPU 在编译 shader 时回调 `Load`/`Store`。

容量常量（shader_cache.h）：

| 常量 | 值 | 含义 |
| --- | --- | --- |
| `MAX_KEY_SIZE` | 1024 | 单条 key 上限 |
| `MAX_VALUE_SIZE` | 1 MB | 单条 value 上限 |
| `MAX_TOTAL_SIZE` | 4 MB | 非 Uni 模式总容量上限 |
| `MAX_UNIRENDER_SIZE` | 40 MB | Uni 模式总容量上限（`MAX_VALUE_SIZE * 10`） |
| `DEFAULT_DELAY_SECONDS` | 3 | 非 Uni 模式落盘延迟 |
| `UNI_DELAY_SECONDS` | 900 | Uni 模式落盘延迟（15 分钟） |
| `bufferSize_` | 16 KB | Load 时 malloc 初始缓冲 |

初始化与身份校验 `InitShaderCache(identity, size, isUni)`（shader_cache.cpp:44）：

1. `initialized_` 守护，已初始化直接返回。
2. `filePath_` 为空放弃；按 `isUni` 选 `MAX_UNIRENDER_SIZE` 或 `MAX_TOTAL_SIZE`，并设置对应 `saveDelaySeconds_`。
3. 新建 `CacheData(MAX_KEY_SIZE, MAX_VALUE_SIZE, totalSize, filePath_)`，调用 `ReadFromFile()` 加载磁盘数据。
4. 对 identity（通常是 GLES 版本字符串）做 SHA-256，写入 `idHash_`；用 `ID_KEY=0` 作为 key 读取磁盘上保存的 sha 数组，若不一致则 `cacheData_->Clear()`——这就是版本/身份失效策略：GLES 版本变化 → hash 不匹配 → 缓存整体作废重建。
5. 置 `initialized_ = true`。

`Load`（shader_cache.cpp:98）用 `OptionalLockGuard`（`try_lock`，避免与磁盘写入互斥阻塞 GL 线程）；`Store` 标记 `cacheDirty_`，延迟落盘（`saveDelaySeconds_` 决定）。`CleanAllShaders()` 清空内存与磁盘；`SetMaxUniRenderSize`/`CalMaxUniRenderSize` 运行期可调整 Uni 容量。

`CacheData`（cache_data.h）是底层键值存储：

- `DataPointer` 持有 key/value 原始内存与 `toFree_` 标志；`ShaderPointer` 组合 key/value；`shaderPointers_` 是 `std::vector<ShaderPointer>`，按 key 排序便于二分查找。
- `Rewrite` 写入；`Get` 读取；`IfSizeValidate`/`IfSkipClean`/`RandClean`/`Clean` 实现 LRU-like 随机淘汰（`maxMultipleSize_=2`、`cleanLevel_=2`，超容量时随机清理）。
- `ReadFromFile`/`WriteToFile`/`Serialize`/`DeSerialize` 处理磁盘格式（`Header{numShaders_}` + 多个 `ShaderData{keySize_, valueSize_, data_[]}`）；`IsValidFile` + `CrcGen` 校验完整性；`DumpAbnormalCacheToFile` 把异常 buffer 落盘便于排障。

### MemoryHandler：GPUContextOptions 配置入口

`MemoryHandler`（memory_handler.h）是 RenderContext 与 ShaderCache/Drawing 之间的桥：

- `ConfigureContext(options, identity, size, cacheFilePath, isUni)`（memory_handler.cpp:24）：
  - `options->SetAllowPathMaskCaching(true)`。
  - `GpuApiType::DDGR` 时直接返回——DDGR 后端不走 Skia shader 缓存。
  - 设置 `ShaderCache` 文件路径（空时回退到 `mUniRenderCacheDir`），并 `options->SetStoreCachePath`。
  - `cache.InitShaderCache(identity, size, isUni)`，`options->SetPersistentCache(&cache)`。
- `ClearRedundantResources(gpuContext)`：`Flush()` + `PerformDeferredCleanup(10s)`，与 `RenderContextGL::ClearRedundantResources` 同策略。
- `QuerryShader()` 返回 RAM/数量字符串，`ClearShader()` 调 `CleanAllShaders()`，供 dump 工具或调试接口使用。
- 静态成员 `mUniRenderCacheDir = "/data/service/el0/render_service"`，与 `RenderContextGL::UNIRENDER_CACHE_DIR` 保持一致。

### UniRenderMode 对 RenderContext 行为的影响

`SetUniRenderMode(bool)` 把 `isUniRender_` 置位（render_context.h:72），调用方（如 `RSBaseRenderEngine::Init` 中 `RSUniRenderJudgement::IsUniRender()` 时）再传给 GPUContextOptions 和 MemoryHandler。影响：

- 缓存目录：`SetUpGpuContext` 中 `isUniRenderMode_` 为真时强制使用 `UNIRENDER_CACHE_DIR`，保证统一渲染进程所有线程命中同一份 shader cache。
- 缓存容量：ShaderCache 在 `isUni=true` 时使用 40 MB 上限和 15 分钟延迟落盘（默认仅 4 MB / 3 秒）。
- GPUContextOptions：`options.SetIsUniRender(isUniRenderMode_)`，Drawing/Skia 侧按此调整内部缓存策略。

### 调用方与数据流

主要调用点：

| 调用方 | 路径 | 后端/类型 |
| --- | --- | --- |
| RSRenderThread（客户端渲染线程） | `rosen/modules/render_service_client/core/pipeline/rs_render_thread.cpp:335` | `Create()` + `Init(BASIC_RENDER, cacheDir)` |
| RSCanvasModifiersDraw（modifier 子线程） | `rosen/modules/render_service_client/core/modifier_render_thread/rs_canvas_modifiers_draw.cpp:517` | `Create()` + `Init(BASIC_RENDER, cacheDir_)` |
| RSBaseRenderEngine（Composer 侧） | `rosen/modules/render_service/composer/composer_service/external_depend/engine/rs_base_render_engine.cpp:200` | `Create()` + 按 `UNPROTECTED_REDRAW`/`PROTECTED_REDRAW`/`BASIC_RENDER` 初始化 |
| Drawing NDK GPU 上下文管理 | `rosen/modules/2d_graphics/drawing_ndk/drawing_utils/drawing_gpu_context_manager.cpp:30` | `Create()` + `Init()` |
| RSOffscreenRenderThread | `rosen/modules/render_service_base/src/offscreen_render/rs_offscreen_render_thread.cpp:45` | `Create()` + `Init()` |
| EffectImageChain | `rosen/modules/effect/skia_effectChain/src/effect_image_chain.cpp:230/908` | `Create()` + `Init()` |
| 各类 demo/sample/单测 | `rosen/.../test/`、`samples/2d_graphics/` | 按需 `Create()` + `Init()` |

数据流：

```
RSSystemProperties::IsUseVulkan()
  └─ RenderContext::Create() ──┬─ RenderContextGL  → EGL 初始化 + BuildFromGL
                               └─ RenderContextVK   → RsVulkanContext::Get(type) → BuildFromVk
        ↓
SetUpGpuContext(cacheDir)
  └─ MemoryHandler::ConfigureContext(options, glesVer, len, dir, isUni)
       ├─ ShaderCache::SetFilePath + InitShaderCache(identity, size, isUni)
       │     └─ CacheData::ReadFromFile + SHA-256 身份校验
       └─ options.SetPersistentCache(&ShaderCache)
        ↓
Drawing::GPUContext::BuildFromGL/BuildFromVk(options)
  └─ drGPUContext_ 持有 GPU 上下文，shader 编译时回调 Load/Store
        ↓
AcquireSurface(w, h)  → Drawing::Surface::Bind(FrameBuffer) → 上层 Canvas 绘制
RenderFrame()          → surface_->GetCanvas()->Flush()
ClearRedundantResources() → Flush + PerformDeferredCleanup(10s)
```

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 抽象基类 + 工厂方法 | `RenderContext::Create()` 按 `IsUseVulkan()` 分流 | 上层无感切换 GL/VK，编译期用 `RS_ENABLE_VK` 裁剪 VK 代码 |
| VK 委托 RsVulkanContext 单例 | `RsVulkanContext::Get(contextType_)` | Vulkan 实例/队列昂责，进程内按 `RenderEngineType` 复用，避免重复初始化 |
| RenderEngineType 多实例 | `BASIC_RENDER`/`PROTECTED_REDRAW`/`UNPROTECTED_REDRAW` | Composer 侧按 DRM 安全/非安全隔离上下文，UniRender 走 BASIC |
| EGL Pbuffer 兜底 | `CreatePbufferSurface` 仅在无 `EGL_KHR_surfaceless_context` 时建 1×1 Pbuffer | 兼容不支持 surfaceless 的 EGL 实现，仍能离屏 make current |
| EGL ShareContext | `CreateShareContext`/`DestroyShareContext` + `shareContextMutex_` | 多线程渲染共享 GL 资源（纹理/FBO），子线程 `CreateDrawingGPUContext` 时建立 |
| EGL Partial Update | `DamageFrame` + `QueryEglBufferAge` | 配合 `EGL_KHR_partial_update`/`EGL_BUFFER_AGE_KHR` 做局部更新，减少合成带宽 |
| ShaderCache 磁盘缓存 | `/data/service/el0/render_service` + SHA-256 identity | 首次编译后缓存，加速启动；identity 变化（GLES 版本/驱动）自动失效重建 |
| UniRender 容量/延迟分级 | `MAX_UNIRENDER_SIZE`=40MB / `UNI_DELAY_SECONDS`=900 | 统一渲染进程 shader 多，扩大缓存、降低落盘频率；非 Uni 用小缓存快落盘 |
| CacheData 随机淘汰 | `RandClean`/`Clean` + `maxMultipleSize_/cleanLevel_` | 超容量时随机淘汰避免 LRU 链表开销，适合 shader 场景 |
| ColorGamut 到 ColorSpace 转换 | `ConvertColorGamutToColorSpace` 静态方法 | 统一 Display 色域到 Drawing ColorSpace 的映射；SRGB 走 `CreateSRGB` |
| ARKUI_X 条件编译 | `#ifdef ROSEN_ARKUI_X` 的 `AddSurface`/`DeleteSurface`/`SetCleanUpHelper` | 支持跨平台（iOS/桌面）窗口生命周期与清理回调 |
| iOS EGL 占位 | `render_context_egl_defines.h` + `RenderContextGL` iOS 分支 | iOS 无 EGL，用占位类型与 CAEAGLLayer 适配编译 |
| SmartCache/VMA/Spirv 参数 | `RSBaseRenderEngine::Init` 在 VK 路径下 `SetParam` | Composer 侧按特性开关调优 VK 内存分配与 spirv 缓存 |
| DDGR 短路 | `MemoryHandler::ConfigureContext` 中 `GpuApiType::DDGR` 直接返回 | DDGR 后端不使用 Skia shader 缓存路径 |

## 验证

| 类型 | 入口 |
| --- | --- |
| 单测 | `rosen/test/2d_graphics/unittest/render_context/render_context_test.cpp`（工厂与 EGL Surface） |
| GL 单测 | `rosen/test/2d_graphics/unittest/render_context/render_context_gl_test.cpp` |
| VK 单测 | `rosen/test/2d_graphics/unittest/render_context/render_context_vk_test.cpp` |
| ShaderCache 单测 | `rosen/test/2d_graphics/unittest/render_context/shader_cache_test.cpp` |
| CacheData 单测 | `rosen/test/2d_graphics/unittest/render_context/cache_data_test.cpp` |
| MemoryHandler 单测 | `rosen/test/2d_graphics/unittest/render_context/momory_handler_test.cpp` |
| 构建裁剪 | `rosen/modules/render_service_base/src/platform/ohos/BUILD.gn:85-111`（`graphic_2d_feature_enable_vulkan`/`graphic_2d_feature_enable_opengl`/`rs_enable_gpu`） |
| wearable 预加载 | 同上 `PRELOAD_SHADER_CACHE` 宏与 `shader_cache_utils.cpp` |

## 待补充背景

- RsVulkanInterface 的 Vulkan 实例/队列/扩展初始化细节（在 render_service_base VK 后端）
- VK 受保护队列（protected memory）与 HTS 在 DRM 安全层中的实际触发路径
- iOS ARKUI_X 分支 `framebuffer_`/`colorbuffer_`/`layer_` 的完整生命周期
- `Drawing::GPUContext::BuildFromGL/BuildFromVk` 内部对 PersistentCache 的回调时序
- SmartCache/VMA/SpirvCacheParam 在不同产品下的默认值与调优基线
