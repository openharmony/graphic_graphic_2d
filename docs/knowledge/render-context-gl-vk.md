# Render Context GL / VK

## 适用范围

- RenderContext 渲染上下文基类
- RenderContextGL 基于 EGL/OpenGL 的渲染上下文
- RenderContextVK 基于 Vulkan 的渲染上下文
- ShaderCache 着色器缓存
- MemoryHandler 内存管理
- GPUContext 与 Drawing GPU 上下文的桥接

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| render_context.h | `rosen/modules/2d_graphics/src/render_context/render_context.h` | RenderContext 基类 |
| render_context_gl.h | `rosen/modules/2d_graphics/src/render_context/new_render_context/render_context_gl.h` | RenderContextGL：EGL/GLES 实现 |
| render_context_vk.h | `rosen/modules/2d_graphics/src/render_context/new_render_context/render_context_vk.h` | RenderContextVK：Vulkan 实现 |
| shader_cache.h | `rosen/modules/2d_graphics/src/render_context/shader_cache.h` | ShaderCache：着色器磁盘缓存 |
| shader_cache_utils.h | `rosen/modules/2d_graphics/src/render_context/shader_cache_utils.h` | ShaderCache 工具函数 |
| memory_handler.h | `rosen/modules/2d_graphics/src/render_context/memory_handler.h` | MemoryHandler：GPU 内存管理 |
| cache_data.h | `rosen/modules/2d_graphics/src/render_context/cache_data.h` | CacheData 缓存数据 |
| render_context.cpp | `rosen/modules/2d_graphics/src/render_context/render_context.cpp` | RenderContext 工厂方法与公共实现 |

## 核心模型

**RenderContext** 是渲染上下文的抽象基类：

```
RenderContext (抽象基类)
  ├─ Init() / AbandonContext()
  ├─ SetUpGpuContext(shared_ptr<GPUContext>)
  ├─ GetShaderCacheSize() / CleanAllShaderCache()
  ├─ QueryMaxGpuBufferSize()
  ├─ ConvertColorGamutToColorSpace() (静态)
  └─ EGL 专用虚函数: AcquireSurface / RenderFrame / DamageFrame / CreateShareContext / ...

RenderContextGL (EGL/GLES 实现)
  ├─ EGLDisplay / EGLContext / EGLSurface / EGLConfig
  ├─ PbufferSurface 离屏渲染
  ├─ ShareContext 多线程共享
  └─ ShaderCache 磁盘缓存 (/data/service/el0/render_service)

RenderContextVK (Vulkan 实现)
  ├─ RenderContextVKType: BASIC_RENDER / PROTECTED_REDRAW / UNPROTECTED_REDRAW
  ├─ isProtected_ 原子标志（安全内容渲染）
  └─ 同样使用 Drawing::GPUContext
```

**RenderContextGL** 关键流程：
1. `Init()` → 创建 EGLDisplay → 选择 EGLConfig → 创建 EGLContext
2. `AcquireSurface(width, height)` → 创建 Drawing::Surface
3. `RenderFrame()` → 渲染当前帧
4. `DamageFrame(rects)` → 设置 EGL 损坏区域（`EGL_KHR_partial_update`）
5. `CreateShareContext()` / `DestroyShareContext()` → 多线程共享上下文
6. `QueryEglBufferAge()` → 查询缓冲区年龄用于局部更新

**RenderContextVK** 类型区分：
- `BASIC_RENDER`：基础渲染
- `PROTECTED_REDRAW`：受保护内容重绘
- `UNPROTECTED_REDRAW`：非受保护内容重绘

**公共字段**：
- `drGPUContext_`：Drawing::GPUContext 共享指针
- `surface_`：Drawing::Surface
- `colorSpace_`：GraphicColorGamut（默认 SRGB）
- `pixelFormat_`：像素格式（默认 RGBA_8888）
- `isUniRender_` / `isUniRenderMode_`：统一渲染模式标志

**ShaderCache**：将编译后的 GPU 着色器持久化到磁盘，避免每次启动重新编译，缓存目录为 `/data/service/el0/render_service`。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 抽象基类 + 工厂方法 | `RenderContext::Create()` 工厂方法 | 根据运行时配置选择 GL 或 VK 后端 |
| EGL ShareContext | `CreateShareContext` / `DestroyShareContext` | 多线程渲染需要共享 GL 上下文和资源 |
| VK Protected 状态 | `std::atomic<bool> isProtected_` | 安全视频内容需要受保护的 Vulkan 队列 |
| ShaderCache 磁盘缓存 | `/data/service/el0/render_service` | 首次编译后缓存，加速后续启动 |
| ColorGamut 到 ColorSpace 转换 | `ConvertColorGamutToColorSpace` 静态方法 | 统一 Display 色域到 Drawing ColorSpace 的映射 |
| PbufferSurface | `CreatePbufferSurface()` | 无窗口时的离屏渲染目标 |
| ARKUI_X 条件编译 | `#ifdef ROSEN_ARKUI_X` 跨平台分支 | 支持跨平台（iOS/桌面）渲染上下文 |

## 待补充背景

- RenderContext::Create 工厂方法的后端选择逻辑
- RenderContextVK 的完整 Vulkan 初始化流程
- ShaderCache 的版本管理与缓存失效策略
- MemoryHandler 的 GPU 内存回收机制
- UniRenderMode 对 RenderContext 行为的影响
