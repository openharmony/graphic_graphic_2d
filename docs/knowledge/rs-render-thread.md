# RS 渲染线程

## 适用范围

- RSUniRenderThread 初始化、渲染帧循环和资源管理
- RenderEngine 的创建和 GPU 上下文管理
- 物理屏/虚拟屏渲染处理器选择
- 帧间隙内存回收与 GPU 缓存管理
- Buffer 管理与 Fence 同步
- 渲染帧数据从 RSMainThread 同步到渲染线程的流程

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 统一渲染线程 | `rosen/modules/render_service/core/pipeline/render_thread/rs_uni_render_thread.h/.cpp` | 单例，Start/RenderFrames/Sync/ClearMemoryCache |
| 帧绘制 | `rs_draw_frame.h/.cpp` | RSDrawFrame 封装单帧 GPU 绘制流程 |
| 渲染引擎 | `rs_render_engine.h/.cpp` | RSBaseRenderEngine 子类创建（GPU/软件） |
| 物理屏处理器 | `rs_physical_screen_processor.h/.cpp` | 物理屏幕渲染流程 |
| 虚拟屏处理器 | `rs_virtual_screen_processor.h/.cpp` | 虚拟屏幕渲染流程 |
| 统一渲染处理器 | `rs_uni_render_processor.h/.cpp` | UniRender 模式下单帧处理主逻辑 |
| 统一渲染虚拟处理器 | `rs_uni_render_virtual_processor.h/.cpp` | UniRender 模式下虚拟屏处理 |
| Composer 适配 | `rs_composer_adapter.h` / `rs_uni_render_composer_adapter.h` | 对接 Composer 提交帧 |
| 渲染工具 | `rs_uni_render_util.h/.cpp` / `rs_base_surface_util.h` | 渲染辅助函数 |
| 分裂渲染工具 | `rs_divided_render_util.h/.cpp` | DividedRender 模式辅助 |
| Buffer 管理器 | `rosen/modules/render_service/core/pipeline/buffer_manager/rs_buffer_manager.h` | 帧间 Buffer 生命周期和 Fence 管理 |

## 核心模型

### 渲染线程与主线程协作

RSUniRenderThread 是独立线程，运行在自己的 EventRunner 上。每帧流程：

```
RSMainThread::OnVsync()
  → Render() / OnUniRenderDraw()
  → RSMainThread::UniRender(rootNode) 或 RSMainThread::DoParallelComposition()
  → RSUniRenderThread::Sync(stagingRenderThreadParams)  // 主线程→渲染线程同步参数
  → RSUniRenderThread::RenderFrames()
    → Render()
      → RSUniRenderProcessor::Process(rootNodeDrawable)  // 遍历 Drawable 树执行绘制
      → RSUniRenderComposerAdapter::CommitFrame()         // 提交到 Composer
```

### 同步机制

- `RSRenderThreadParams` 封装一帧渲染所需的全部参数（节点树快照、脏区域、水印、屏幕信息等）。
- `Sync()` 在主线程调用，将 staging 参数 move 到渲染线程。
- 渲染线程从 `RSRenderThreadParamsManager` 获取当前帧参数。

### 渲染引擎

- `uniRenderEngine_` 是 `RSBaseRenderEngine` 的子类实例（GPU 或软件渲染）。
- GPU 模式下使用 Skia/Drawing GPU 后端，`InitGrContext()` 初始化 GPU 上下文。
- 渲染线程持有独立的 `Drawing::GPUContext`，与主线程的 GPU 上下文分离。

### 内存管理

- `PurgeCacheBetweenFrames()` 在帧间隙清理 GPU 缓存。
- `ClearMemoryCache(moment, deeply)` 根据不同清理力度回收资源。
- `MemoryManagementBetweenFrames()` / `SuppressGpuCacheBelowCertainRatioBetweenFrames()` 控制 GPU 缓存比例。
- `PostImageReleaseTask()` 延迟释放图片资源。

### Buffer 管理与 Fence

- `RSBufferManager` 管理 SurfaceBuffer 的获取、释放和 Fence 同步。
- `BufferManagerGuard` 在一帧绘制开始/结束时管理 acquireFence。
- `WaitUntilScreenNodeBufferReleased()` 等待屏幕节点 buffer 就绪，防止 buffer 竞争。

### 屏幕节点 Buffer 等待

- `ScreenNodeBufferCond` 使用 condition_variable 阻塞渲染线程，直到 ScreenNode 有空闲 buffer。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 主线程/渲染线程分离 | RSMainThread + RSUniRenderThread 独立 EventRunner | VSync 处理和 GPU 绘制并行，减少帧延迟 |
| Sync 参数传递 | `RSRenderThreadParams` + move 语义 | 避免帧间数据竞争，主线程准备→渲染线程消费 |
| BufferManagerGuard | RAII 管理 Fence | 保证 Fence 在帧结束时正确设置，避免 buffer 泄漏 |
| 帧间隙内存回收 | `PurgeCacheBetweenFrames()` / `MemoryManagementBetweenFrames()` | 降低常驻内存，在帧空闲时主动回收 |
| Vma 优化 | `vmaOptimizeFlag_` / `SetVmaCacheStatus()` | 控制虚拟内存分配器缓存策略，平衡性能与内存 |
| ScreenNodeBufferCond | `screenNodeBufferReleasedCond` | 背压机制，渲染线程等待 buffer 就绪，避免覆盖正在显示的 buffer |

## 待补充背景

- RSUniRenderProcessor 的具体 Prepare/Process 两阶段流程。
- DividedRender 模式与 UniRender 模式的切换条件和差异。
- 渲染线程与 Composer 的具体提交时序和 Fence 传递链路。
- GPU 上下文丢失（Vulkan Error）时的恢复流程。
- 历史线上渲染线程卡顿或 buffer 竞争典型案例。
