# Frame Analyzer、Frame Load 与 Frame Report

## 适用范围

- 帧事件采集与可视化（FrameCollector、FramePainter、FrameSaver）
- 帧负载预测与调度（RsFrameDeadlinePredict、RsFrameBlurPredict、RsGameFrameHandler）
- 帧调度事件上报（RsFrameReport 与 FrameUiIntf 联动）
- 渲染管线关键节点打点与 AsyncTrace
- 游戏场景 VSync 偏移调整
- 大面积模糊帧率预测与精确更新
- 帧截止时间（deadline）计算与上报
- Vulkan 前台窗口信息上报（RS_ENABLE_VK）

## 快速代码地图

### Frame Analyzer

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| `frame_info.h` | `rosen/modules/frame_analyzer/export/` | 帧事件类型枚举、FrameInfo/UIMarks 结构体、颜色映射 |
| `frame_collector.h` | `rosen/modules/frame_analyzer/export/` | 全局单例采集器接口定义 |
| `frame_collector.cpp` | `rosen/modules/frame_analyzer/src/` | 采集器实现：MarkFrameEvent、ProcessFrameEvent、双锁机制 |
| `frame_painter.h/.cpp` | `rosen/modules/frame_analyzer/export/`、`src/` | 帧可视化绘制：时间条、参考线 |
| `frame_saver.h/.cpp` | `rosen/modules/frame_analyzer/src/` | 帧事件落盘：`/data/frame_render/{pid}.log` |
| `ring_queue.h` | `rosen/modules/frame_analyzer/export/` | 固定容量环形队列模板 |
| `BUILD.gn` | `rosen/modules/frame_analyzer/` | 构建配置：libframe_analyzer |

### Frame Load

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| `rs_frame_deadline_predict.h/.cpp` | `rosen/modules/frame_load/include/`、`src/` | 120Hz 帧截止时间计算与上报 |
| `rs_frame_blur_predict.h/.cpp` | `rosen/modules/frame_load/include/`、`src/` | 大面积模糊预测（Predictive/Precise 双路径） |
| `rs_game_frame_handler.h/.cpp` | `rosen/modules/frame_load/include/`、`src/` | 游戏场景 VSync 偏移调整 |
| `BUILD.gn` | `rosen/modules/frame_load/` | 构建配置：libframe_load |

### Frame Report

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| `rs_frame_report.h/.cpp` | `rosen/modules/frame_report/include/`、`src/` | 静态工具类，所有上报方法静态 |
| `BUILD.gn` | `rosen/modules/frame_report/` | 构建配置：libframe_report |

### 扩展与测试

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| `rs_frame_report_ext.h` | `utils/rs_frame_report_ext/include/` | dlopen 扩展接口 |
| `frame_collector_test.cpp` | `rosen/test/frame_analyzer/unittest/` | FrameCollector 单测 |
| `game_frame_handler_test.cpp` | `rosen/test/frame_load/unittest/` | RsGameFrameHandler 单测 |
| `rs_frame_report_test.cpp` | `rosen/test/frame_report/unittest/` | RsFrameReport 单测 |

## 核心模型

### Frame Analyzer：帧事件采集与可视化

#### FrameCollector 单例架构

FrameCollector 是全局单例，负责采集渲染管线中 UI 和 RS 两个阶段的关键时间点，存入环形队列（容量 60 帧）。

**数据流**：
```
UI 线程/RS 线程 → MarkFrameEvent(type, timeNs) → ProcessFrameEvent() → FrameInfoQueue
```

#### 帧事件类型定义（`frame_info.h`）

```cpp
enum class FrameEventType : int32_t {
    // UI Event (UIMarksStart ~ UIMarksEnd-1)
    HandleInputStart = 0,  HandleInputEnd,
    AnimateStart,          AnimateEnd,
    BuildStart,            BuildEnd,
    UploadStart,           UploadEnd,
    LayoutStart,           LayoutEnd,
    DrawStart,             DrawEnd,  // UIMarksEnd-1

    // RS Event (WaitVsync ~ Flush)
    WaitVsyncStart,        WaitVsyncEnd,
    ReleaseStart,          ReleaseEnd,
    FlushStart,            FlushEnd,

    Max,  // 边界
};
```

**枚举规则**：
- 起始事件 ID 为偶数（0、2、4...），结束事件 ID 为奇数（1、3、5...）
- `UIMarksStart = HandleInputStart = 0`，`UIMarksEnd = DrawEnd + 1 = 12`
- `LoopStart = HandleInputStart = 0`，`LoopEnd = Max = 18`

#### 数据结构

```cpp
struct UIMarks {
    int32_t frameNumber = 0;
    std::array<int64_t, UIMarksLen> times = {};  // 12 个元素
};

struct FrameInfo {
    int32_t frameNumber = 0;
    bool skiped = false;
    std::array<int64_t, LoopLen> times = {};  // 18 个元素
};

using FrameInfoQueue = RingQueue<FrameInfo, 60>;  // 固定容量 60 帧
```

#### UI 与 RS 事件合并机制

1. **UI 阶段**：UI 线程调用 `MarkFrameEvent(HandleInputStart/.../DrawEnd, timeNs)`
   - 获取 `pendingMutex_` 锁
   - 写入 `pendingUIMarks_.times[index]`

2. **RS 阶段**：RS 线程调用 `MarkFrameEvent(WaitVsyncStart, timeNs)`
   - 获取 `pendingMutex_` + `frameQueueMutex_` 双锁
   - 将 `pendingUIMarks_` 拷贝为 `currentUIMarks_`
   - 调用 `frameQueue_.Push(FrameInfo())` 创建新帧
   - 将 `currentUIMarks_` 合并到新帧的 `times` 数组
   - 设置 `pbefore_` 指向新帧

3. **后续 RS 事件**：直接追加到 `pafter_->times[index]`

#### 双锁机制

```cpp
std::mutex pendingMutex_;      // 保护 pendingUIMarks_
std::mutex frameQueueMutex_;    // 保护 frameQueue_ 和 pbefore_/pafter_
```

#### 输出模式（系统参数 `debug.graphic.frame`）

- `paint`：启用 FramePainter，每帧渲染结束时绘制时间条到屏幕
- `saver`：启用 FrameSaver，写入 `/data/frame_render/{pid}.log`
- `disable`：关闭输出

#### AsyncTrace 打点

```cpp
// frame_collector.cpp:126-127, 140-142
StartAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, 
    GetAsyncNameByFrameEventType(index), frameNumber);
FinishAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, 
    GetAsyncNameByFrameEventType(index), frameNumber);

// Trace 名称格式："Frame.{index}.{EventName}"
// 例如："Frame.0.Animate", "Frame.1.Animate"
```

### Frame Load：帧负载预测与调度

#### RsFrameDeadlinePredict：120Hz 帧截止时间计算

**核心逻辑**（`rs_frame_deadline_predict.cpp:33-66`）：

```cpp
void ReportRsFrameDeadline(uint32_t rate, int64_t period, 
                           bool ltpoEnabled, bool forceRefreshFlag) {
    int64_t extraReserve = 0;
    int64_t vsyncOffset = 0;
    
    if (currentRate == OLED_120_HZ) {
        if (ltpoEnabled) {
            vsyncOffset = CreateVSyncGenerator()->GetVSyncOffset();
            if (vsyncOffset > SINGLE_SHIFT && vsyncOffset <= DOUBLE_SHIFT) {
                extraReserve = SINGLE_SHIFT;  // 2.7ms
            } else if (vsyncOffset > DOUBLE_SHIFT && vsyncOffset < idealPeriod) {
                extraReserve = DOUBLE_SHIFT;  // 5.4ms
            }
        } else {
            extraReserve = FIXED_EXTRA_DRAWING_TIME;  // 3ms
        }
    }
    
    int64_t drawingTime = forceRefreshFlag ? idealPeriod : idealPeriod + extraReserve;
    RsFrameReport::ReportFrameDeadline(drawingTime, currentRate);
}
```

**常量定义**：
```cpp
constexpr int64_t FIXED_EXTRA_DRAWING_TIME = 3000000;  // 3ms
constexpr int64_t SINGLE_SHIFT = 2700000;              // 2.7ms
constexpr int64_t DOUBLE_SHIFT = 5400000;              // 5.4ms
```

#### RsFrameBlurPredict：大面积模糊预测

**双路径机制**：

1. **Predictive 路径**（渲染前）：
   ```cpp
   void AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively();
   void PredictDrawLargeAreaBlur(RSRenderNode& node);
   ```
   - 在渲染前遍历节点树，调用 `node.NodeDrawLargeAreaBlur()`
   - 累积 `predictDrawLargeAreaBlur_.first` 结果
   - 上报 "0"(无效)、"1"(长帧)、"2"(短帧)

2. **Precise 路径**（渲染后）：
   ```cpp
   void UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely();
   ```
   - 渲染后确认是否真的有大面积模糊
   - 用于校准预测结果

**状态值**（`rs_frame_blur_predict.h:42-44`）：
```cpp
const std::string RS_BLUR_PREDICT_INVALID = "0";  // 无效
const std::string RS_BLUR_PREDICT_LONG = "1";     // 长帧（有大面积模糊）
const std::string RS_BLUR_PREDICT_SHORT = "2";    // 短帧（无大面积模糊）
```

#### RsGameFrameHandler：游戏场景 VSync 偏移调整

**构造函数**（`rs_game_frame_handler.h:29-30`）：
```cpp
RsGameFrameHandler(
    const HandleGameSceneChangedFunc& handleGameSceneChangedFunc,
    const GetCoreInfoFunc& getCoreInfoFunc);
```

**核心方法**：
```cpp
void HandleGameSceneChanged();  // 处理游戏场景变化
```

**回调类型**：
```cpp
using HandleGameSceneChangedFunc = std::function<void(int64_t, int64_t)>;
using GetCoreInfoFunc = std::function<void(bool&, bool&, bool&, int64_t&, int64_t&)>;
```

**条件限制**：
- 仅在非 LTPO 模式下生效
- VSync 偏移必须已自定义

### Frame Report：帧调度事件上报

#### 静态工具类设计

```cpp
class RsFrameReport final {
public:
    RsFrameReport() = delete;
    ~RsFrameReport() = delete;
    
    static void InitDeadline();
    static void RenderStart(uint64_t timestamp, int skipFirstFrame = 0);
    static void RenderEnd();
    // ...
    
private:
    static void InitSched();
    static std::once_flag initFlag_;
    static bool inited;
};
```

#### 延迟初始化

```cpp
void RsFrameReport::InitDeadline() {
    std::call_once(initFlag_, &RsFrameReport::InitSched);
    GraphReportSchedEvent(OHOS::RME::FrameSchedEvent::INIT, {});
}

void RsFrameReport::InitSched() {
    OHOS::RME::FrameUiIntf::GetInstance().Init();
    inited = true;
}
```

#### 核心上报方法与事件映射

| 方法 | 上报事件 | Payload |
| --- | --- | --- |
| `InitDeadline()` | `INIT` | `{}` |
| `RenderStart(timestamp, skipFirstFrame)` | `RS_RENDER_START` | `{"vsyncTime", "skipFirstFrame"}` |
| `RenderEnd()` | `RS_RENDER_END` | `{}` |
| `UniRenderStart/End()` | `RS_UNI_RENDER_START/END` | `{}` |
| `SendCommandsStart()` | `RS_SEND_COMMANDS_START` | `{}` |
| `SetFrameParam(requestId, load, schedFrameNum, value)` | `SET_FRAME_PARAM` | `{"requestId", "load", "schedFrameNum", "value"}` |
| `CheckUnblockMainThreadPoint()` | `RS_UNBLOCK_MAINTHREAD` | `{}` |
| `CheckPostAndWaitPoint()` | `RS_POST_AND_WAIT` | `{}` |
| `CheckBeginFlushPoint()` | `RS_BEGIN_FLUSH` | `{}` |
| `ReportFrameDeadline(deadline, rate)` | `RS_FRAME_DEADLINE` | 内部调用 FrameUiIntf |
| `ReportBufferCount(count)` | `RS_BUFFER_COUNT` | `{}` |
| `ReportComposerInfo(screenId, composerTid)` | `RS_COMPOSER_INFO` | `{}` |
| `ReportUnmarshalData(unmarshalTid, dataSize)` | `RS_UNMARSHAL_DATA` | `{}` |
| `ReportScbSceneInfo(description, eventStatus)` | `GPU_SCB_SCENE_INFO` | `{}` |
| `BlurPredict(payload)` | `RS_BLUR_PREDICT` | `payload` |

#### Vulkan 前台窗口信息上报（RS_ENABLE_VK）

**实现细节**（`rs_frame_report.cpp`）：

1. **dlopen 加载 Vulkan**：
   ```cpp
   const std::string LIB_VULKAN_PATH = "/system/lib64/libvulkan.so";
   vkhandle = dlopen(LIB_VULKAN_PATH.c_str(), RTLD_LAZY);
   ```

2. **获取函数指针**：
   ```cpp
   static PFN_vkSetFrontWindowStatusHUAWEI mSetFrontWindowStatusHUAWEI;
   static PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
   static PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
   // ...
   ```

3. **创建 Instance/Device**：
   ```cpp
   static bool CreateVulkanInstance();
   static bool CreateVulkanDevice();
   static bool GetSetFrontWindowStatusHUAWEI();
   ```

4. **上报前台窗口信息**：
   ```cpp
   static void ReportWindowInfo(bool isSingleFullScreenApp, 
                                const char* firstFrontBundleName);
   ```

5. **重试机制**：
   ```cpp
   const uint32_t MAX_INITIALIZATION_COUNT = 3;  // 最多重试 3 次
   ```

## 调用链路与触发时机

### FrameCollector 调用链

#### RSRenderThread → WaitVsync 事件

```
RSRenderThread::OnVsync()
  └─> SendFrameEvent(start=true)
      └─> FrameCollector::GetInstance().MarkFrameEvent(WaitVsyncStart)
  └─> ... 渲染处理 ...
  └─> SendFrameEvent(start=false)
      └─> FrameCollector::GetInstance().MarkFrameEvent(WaitVsyncEnd)
```

**代码位置**：`rosen/modules/render_service_client/core/pipeline/rs_render_thread.cpp:104-109`

#### RSRenderThreadVisitor → Release/Flush 事件

```
RSRenderThreadVisitor::ProcessRootSurface()
  └─> rsSurface->RequestFrame() 前
      └─> FrameCollector::GetInstance().MarkFrameEvent(ReleaseStart)
  └─> ... RequestFrame 失败 ...
      └─> FrameCollector::GetInstance().MarkFrameEvent(ReleaseEnd)
  └─> ... 渲染完成 ...
  └─> rsSurface->FlushFrame() 前
      └─> FrameCollector::GetInstance().MarkFrameEvent(FlushStart)
  └─> rsSurface->FlushFrame() 后
      └─> FrameCollector::GetInstance().MarkFrameEvent(FlushEnd)
```

**代码位置**：
- ReleaseStart：`rosen/modules/render_service_client/core/render_thread/rs_render_thread_visitor.cpp:443`
- ReleaseEnd：`rosen/modules/render_service_client/core/render_thread/rs_render_thread_visitor.cpp:455, 571`
- FlushStart/End：`rosen/modules/render_service_client/core/render_thread/rs_render_thread_visitor.cpp:572, 610`

### RsFrameBlurPredict 调用链

#### RSUniRenderVisitor → 大面积模糊预测

```
RSUniRenderVisitor::CheckMergeFilterDirtyWithPreDirty()
  └─> filterNode->PostPrepareForBlurFilterNode()
  └─> RsFrameBlurPredict::GetInstance().PredictDrawLargeAreaBlur(*filterNode)
```

**代码位置**：`rosen/modules/render_service/core/pipeline/main_thread/rs_uni_render_visitor.cpp:3207`

**触发时机**：在处理 Filter 节点脏区域合并时，遍历每个 Filter 节点进行大面积模糊预测。

### RsFrameDeadlinePredict 调用链

#### RSMainThread → 帧截止时间上报

```
RSMainThread::CalSyncSetup()
  └─> RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(
        currentRate, idealPeriod, ltpoEnabled, forceRefreshFlag)
```

**代码位置**：`rosen/modules/render_service/core/pipeline/main_thread/rs_main_thread.cpp:5973`

**触发时机**：在 VSync 到来、计算同步参数时调用，根据当前刷新率和 LTPO 状态计算帧截止时间。

### RsFrameReport 调用链

#### 初始化与渲染起止上报

```
RSUIDirector::Init()
  └─> RsFrameReport::InitDeadline()                                    // 初始化调度

RSRenderThread::RenderCache()
  └─> RsFrameReport::RenderStart(timestamp)                            // 客户端渲染开始

RSMainThread::Render()
  └─> RsFrameReport::RenderStart(timestamp, skipFirstFrame)            // 服务端渲染开始

RSMainThread::RenderFrameFinish() / RenderDirectFinish()
  └─> RsFrameReport::RenderEnd()                                        // 渲染结束
```

**代码位置**：
- InitDeadline：`rosen/modules/render_service_client/core/ui/rs_ui_director.cpp:119`、`rosen/modules/render_service/core/pipeline/main_thread/rs_main_thread.cpp:691`
- RenderStart：`rosen/modules/render_service_client/core/pipeline/rs_render_thread.cpp:596`、`rosen/modules/render_service/core/pipeline/main_thread/rs_main_thread.cpp:5291`
- RenderEnd：`rosen/modules/render_service/core/pipeline/main_thread/rs_main_thread.cpp:3500, 3519, 3532`

#### 统一渲染模式上报

```
RSDrawFrame::DrawFrame()
  └─> RsFrameReport::UniRenderStart()                                  // 统一渲染开始
  └─> ... 渲染处理 ...
  └─> RsFrameReport::UniRenderEnd()                                    // 统一渲染结束
```

**代码位置**：`rosen/modules/render_service/core/pipeline/render_thread/rs_draw_frame.cpp:94, 137`

#### 模糊预测上报

```
RsFrameBlurPredict::GetCurrentFrameDrawLargeAreaBlurPredictively()
  └─> RsFrameReport::BlurPredict(param)                                // 预测路径上报

RsFrameBlurPredict::GetCurrentFrameDrawLargeAreaBlurPrecisely()
  └─> RsFrameReport::BlurPredict(param)                                // 精确路径上报
```

**代码位置**：`rosen/modules/frame_load/src/rs_frame_blur_predict.cpp:88, 97, 111`

### 调用链总结图

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              UI 线程                                        │
│  RSUIDirector::Init() → RsFrameReport::InitDeadline()                      │
└─────────────────────────────────────────────────────────────────────────────┘
                                     │
                                     ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                            RSRenderThread                                    │
│  OnVsync() → MarkFrameEvent(WaitVsyncStart/End)                             │
│  RenderCache() → RenderStart()                                              │
└─────────────────────────────────────────────────────────────────────────────┘
                                     │
                                     ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          RSRenderThreadVisitor                               │
│  ProcessRootSurface() → MarkFrameEvent(ReleaseStart/End, FlushStart/End)    │
└─────────────────────────────────────────────────────────────────────────────┘
                                     │
                                     ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                              RSMainThread                                    │
│  CalSyncSetup() → ReportRsFrameDeadline()                                   │
│  Render() → RenderStart() → RenderEnd()                                     │
└─────────────────────────────────────────────────────────────────────────────┘
                                     │
                                     ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          RSUniRenderVisitor                                  │
│  CheckMergeFilterDirtyWithPreDirty() → PredictDrawLargeAreaBlur()           │
└─────────────────────────────────────────────────────────────────────────────┘
                                     │
                                     ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          RsFrameBlurPredict                                  │
│  PredictDrawLargeAreaBlur() → BlurPredict() (预测路径)                       │
│  UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely() → BlurPredict()    │
└─────────────────────────────────────────────────────────────────────────────┘
                                     │
                                     ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          RSDrawFrame (统一渲染)                              │
│  DrawFrame() → UniRenderStart() → UniRenderEnd()                            │
└─────────────────────────────────────────────────────────────────────────────┘
                                     │
                                     ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                              FrameUiIntf                                     │
│  (frame_aware_sched 子系统，通过 FrameSchedEvent 上报)                       │
└─────────────────────────────────────────────────────────────────────────────┘
```

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 帧事件分 UI 和 RS 两段 | `FrameEventType` 枚举 UIMarksStart/End 和 LoopStart/End 分区 | UI 线程和 RS 线程独立打点，在 VSync 边界合并，避免跨线程锁 |
| 环形队列固定 60 帧容量 | `frameQueueMaxSize = 60`、`RingQueue<FrameInfo, 60>` | 覆盖 1 秒@60fps 的历史，内存固定，无需动态分配 |
| 通过系统参数切换模式 | `WatchParameter("debug.graphic.frame", ...)` | 运行时动态开关，不重编译；paint 用于开发阶段可视化，saver 用于离线分析 |
| FrameCollector 双锁 | `pendingMutex_` + `frameQueueMutex_` | pending 保护 UI 事件写入，frameQueueMutex 保护队列和 before/after 指针 |
| FrameReport 静态类 + call_once | 所有方法静态，`std::call_once` 初始化 | 全局唯一入口，延迟初始化避免启动开销 |
| RsFrameBlurPredict 双路径 | `PredictDrawLargeAreaBlur` + `UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely` | 预测路径提前调整帧率，精确路径用于校准，两者配合减少模糊帧卡顿 |
| RsGameFrameHandler 仅在非 LTPO 生效 | `if (isLtpoEnabled || !isVsyncOffsetCustomized) return;` | LTPO 模式下刷新率自适应，不需要手动调整 VSync 偏移 |
| RsFrameDeadlinePredict 120Hz 特殊处理 | `if (currentRate == OLED_120_HZ)` 分支 | 120Hz 帧预算紧张（8.33ms），需要精确的额外保留时间计算 |
| Vulkan 前台窗口上报 | `RS_ENABLE_VK` 条件编译块 | GPU 驱动需要知道前台应用以优化调度，通过 Vulkan 扩展传递 |
| FrameCollector 帧号管理 | `currentFrameNumber_` 在 ProcessUIMarkLocked 中递增 | 同步 UI 和 RS 事件的帧号，支持 AsyncTrace 关联 |
| 帧总时间窗口 160ms | `frameTotalMs = 160` | 绘制模式下的时间轴窗口，覆盖约 10 帧@60fps |
