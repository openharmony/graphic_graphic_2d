# Render Context 渲染上下文模块介绍

## 1. 概述

Render Context模块是Rosen 2D图形子系统的核心模块，负责GPU渲染上下文的创建、管理和Shader缓存。该模块通过抽象工厂模式支持OpenGL ES和Vulkan两种渲染后端，为上层渲染管线提供统一的渲染上下文接口。

## 2. 模块职责

### 2.1 核心职责
1. **渲染上下文管理**
    - 提供统一的渲染上下文抽象接口（RenderContext）
    - 支持OpenGL ES和Vulkan两种后端的动态选择
    - 管理GPU上下文（Drawing::GPUContext）的生命周期

2. **EGL环境管理**（GL后端）
    - 管理EGL Display、Context、Surface的创建与销毁
    - 支持Window Surface和Pbuffer Surface
    - 提供共享上下文机制用于多线程渲染

3. **Shader缓存管理**
    - 提供持久化的Shader编译缓存，避免重复编译
    - 基于文件系统的缓存存储与加载
    - 支持缓存大小限制和自动清理策略

4. **GPU资源配置**
    - 配置GPU上下文选项（Shader缓存注入）
    - 管理颜色空间转换
    - 查询GPU最大缓冲区尺寸

### 2.2 后端职责划分

#### OpenGL ES后端（RenderContextGL）
- **EGL环境**：完整管理EGL Display/Context/Surface生命周期
- **Surface管理**：支持Window Surface和Pbuffer Surface的创建与切换
- **渲染操作**：提供MakeCurrent、SwapBuffers、DamageFrame等渲染控制
- **共享上下文**：支持创建和销毁EGL共享上下文，用于资源跨线程共享
- **GPU信息查询**：通过OpenGL接口查询最大纹理尺寸等硬件限制
- **颜色空间转换**：将Drawing颜色类型转换为OpenGL格式

#### Vulkan后端（RenderContextVK）
- **Vulkan初始化**：通过RsVulkanContext单例获取Vulkan设备
- **受保护渲染**：支持受保护内容（Protected Content）的渲染状态管理
- **上下文类型**：支持基础渲染、受保护重绘、非受保护重绘等多种上下文类型
- **GPU上下文设置**：配置Vulkan GPU上下文并初始化Drawing模块

## 3. 模块文件结构

### 3.1 核心渲染上下文文件

#### 3.1.1 render_context.h/cpp
**功能**: 渲染上下文的抽象基类，定义所有渲染后端的统一接口。

**位置**: `render_context.h` / `render_context.cpp`

**主要职责**:
- 定义渲染上下文的纯虚接口（Init、AbandonContext、SetUpGpuContext等）
- 提供EGL相关操作的虚函数默认实现
- 提供工厂方法Create()，根据系统属性动态选择GL或VK后端
- 管理Drawing::GPUContext和Drawing::Surface

**工厂方法**:
```cpp
std::shared_ptr<RenderContext> RenderContext::Create()
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        return std::make_shared<RenderContextVK>();
    }
#endif
    return std::make_shared<RenderContextGL>();
}
```

**核心成员**:
```cpp
class RenderContext {
protected:
    std::shared_ptr<Drawing::GPUContext> drGPUContext_;
    std::shared_ptr<Drawing::Surface> surface_;
    std::mutex shareContextMutex_;
    bool isUniRender_ = false;
    bool isUniRenderMode_ = false;
    std::string cacheDir_;
    std::shared_ptr<MemoryHandler> mHandler_;
    int32_t pixelFormat_ = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    GraphicColorGamut colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
};
```

**后端选择对比**:
| 特性 | RenderContextGL | RenderContextVK |
|------|----------------|-----------------|
| 渲染API | OpenGL ES 3.0+ | Vulkan |
| 选择条件 | 默认后端 | RSSystemProperties::IsUseVulkan() 为true |
| 编译宏 | 始终编译 | RS_ENABLE_VK |
| EGL依赖 | 是 | 否 |

---

#### 3.1.2 render_context_gl.h/cpp
**功能**: OpenGL ES后端的渲染上下文实现。

**位置**: `new_render_context/render_context_gl.h` / `new_render_context/render_context_gl.cpp`

**主要职责**:
- 实现完整的EGL环境管理（Display、Config、Context、Surface）
- 提供OpenGL ES渲染操作接口
- 支持共享上下文创建用于多线程资源分享
- 通过OpenGL接口查询GPU硬件限制

**核心成员**:
```cpp
class RenderContextGL : public RenderContext {
private:
    EGLConfig config_;
    EGLNativeWindowType nativeWindow_;
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLContext eglContext_ = EGL_NO_CONTEXT;
    EGLSurface eglSurface_ = EGL_NO_SURFACE;
    EGLSurface pbufferSurface_ = EGL_NO_SURFACE;
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
    const std::string UNIRENDER_CACHE_DIR = "/data/service/el0/render_service";
};
```

**初始化流程**:
1. **获取Display**: 通过`eglGetPlatformDisplay()`获取EGL Display
2. **初始化EGL**: 调用`eglInitialize()`初始化EGL
3. **选择Config**: 根据属性选择合适的EGL Framebuffer Config
4. **创建Context**: 使用OpenGL ES 3.0属性创建EGL Context
5. **创建Pbuffer**: 创建Pbuffer Surface用于离屏渲染
6. **设置GPU上下文**: 配置Drawing GPUContext并关联Shader缓存

---

#### 3.1.3 render_context_vk.h/cpp
**功能**: Vulkan后端的渲染上下文实现。

**位置**: `new_render_context/render_context_vk.h` / `new_render_context/render_context_vk.cpp`

**主要职责**:
- 实现Vulkan渲染上下文的初始化
- 支持受保护内容渲染状态管理
- 通过Vulkan Context单例获取设备信息

**核心成员**:
```cpp
enum class RenderContextVKType : uint8_t {
    BASIC_RENDER = 0,
    PROTECTED_REDRAW,
    UNPROTECTED_REDRAW,
    MAX_INTERFACE_TYPE,
};

class RenderContextVK : public RenderContext {
private:
    std::atomic<bool> isProtected_{false};
};
```

### 3.2 Shader缓存管理文件

#### 3.2.1 shader_cache.h/cpp
**功能**: Shader缓存管理器，继承自`Drawing::GPUContextOptions::PersistentCache`，为GPU上下文提供持久化Shader缓存。

**主要职责**:
- 实现GPU上下文的Shader缓存回调接口（Load/Store）
- 管理缓存的初始化、文件路径和身份校验
- 提供延迟写入机制，减少磁盘IO频率
- 提供缓存大小和数量的查询接口

**核心成员**:
```cpp
class ShaderCache : public Drawing::GPUContextOptions::PersistentCache {
private:
    bool initialized_ = false;
    std::unique_ptr<CacheData> cacheData_;
    std::string filePath_;
    std::vector<uint8_t> idHash_;          // SHA256哈希，用于身份校验
    mutable std::mutex mutex_;
    bool savePending_ = false;
    unsigned int saveDelaySeconds_ = 900;  // 延迟写入时间（15分钟）
    size_t bufferSize_ = 16 * 1024;       // 16KB缓冲区
    bool cacheDirty_ = false;
    int maxUniRenderSize_ = 0;

    static constexpr size_t MAX_KEY_SIZE = 1024;        // 1KB
    static constexpr size_t MAX_VALUE_SIZE = 1024 * 1024; // 1MB
    static constexpr size_t MAX_TOTAL_SIZE = 4 * 1024 * 1024;  // 4MB
    static constexpr size_t MAX_UNIRENDER_SIZE = 10 * 1024 * 1024; // 10MB
};
```

**线程安全设计**:
```cpp
struct OptionalLockGuard {
    explicit OptionalLockGuard(std::mutex& m): mtx(m), status(m.try_lock()) {}
    ~OptionalLockGuard() { if (status) { mtx.unlock(); } }
    std::mutex& mtx;
    bool status = false;
};
```
采用try_lock非阻塞锁策略，避免Store操作阻塞渲染线程。

---

#### 3.2.2 cache_data.h/cpp
**功能**: 底层缓存数据存储，负责文件IO和缓存条目管理。

**主要职责**:
- 提供基于排序数组的Key-Value缓存存储
- 实现缓存的序列化和反序列化（文件读写）
- 提供缓存大小限制和随机淘汰清理策略
- CRC校验确保缓存文件完整性

**核心成员**:
```cpp
class CacheData {
private:
    size_t totalSize_ = 0;
    std::vector<ShaderPointer> shaderPointers_;  // 按Key排序的Shader条目
    size_t maxKeySize_;
    size_t maxValueSize_;
    size_t maxTotalSize_;
    std::string cacheDir_;
};
```

**内部类**:
- **DataPointer**: 管理缓存数据的内存，支持占用模式（引用外部内存）和拷贝模式
- **ShaderPointer**: 封装Key-Value对的DataPointer，支持排序比较

**缓存清理策略**:
当缓存总量超过`maxTotalSize_`时触发随机清理（`RandClean`），随机选择条目进行淘汰，直到总大小回到阈值以下。

---

#### 3.2.3 shader_cache_utils.h/cpp
**功能**: Shader缓存路径管理工具类。

**主要职责**:
- 管理预加载Shader缓存的目录路径
- 提供路径的获取和设置接口

**核心接口**:
```cpp
class ShaderCacheUtils {
public:
    static std::string GetPreloadCacheDir();
    static void SetPreloadCacheDir(const std::string dir);
private:
    static const std::string preloadShaderCacheDir_;
};
```

### 3.3 辅助文件

#### 3.3.1 memory_handler.h/cpp
**功能**: GPU上下文配置处理器，提供静态方法用于配置GPU上下文和清理资源。

**主要职责**:
- 将ShaderCache注入到GPU上下文选项中
- 清理GPU冗余资源
- 查询和清除Shader缓存

**核心接口**:
```cpp
class MemoryHandler {
public:
    static void ConfigureContext(Drawing::GPUContextOptions* context,
        const char* identity, const size_t size,
        const std::string& cacheFilePath = mUniRenderCacheDir, bool isUni = true);
    static void ClearRedundantResources(Drawing::GPUContext* gpuContext);
    static std::string QuerryShader();
    static std::string ClearShader();
private:
    inline static const std::string mUniRenderCacheDir = "/data/service/el0/render_service";
};
```

#### 3.3.2 render_context_log.h
**功能**: 模块日志工具，定义LOGD/LOGI/LOGW/LOGE宏。

**主要职责**:
- 基于OHOS HiLog框架提供分级日志输出
- 在非OHOS平台上提供空实现

#### 3.3.3 render_context_egl_defines.h
**功能**: Harmony平台EGL类型占位定义。

**主要职责**:
- 为Harmony平台提供EGL基本类型的void*定义
- 提供EGL_NO_CONTEXT等常量宏

## 4. 设计模式

1. **抽象工厂模式**: `RenderContext::Create()`根据系统属性动态创建GL或VK后端实例
2. **单例模式**: `ShaderCache::Instance()`提供全局唯一的Shader缓存访问点
3. **策略模式**: RenderContextGL和RenderContextVK作为不同的渲染策略实现同一接口
4. **模板方法模式**: ShaderCache定义缓存操作的统一流程，具体存储由CacheData实现
