# Frame Analyzer、Frame Load 与 Frame Report

## 适用范围

- 帧事件采集与可视化（FrameCollector、FramePainter、FrameSaver）
- 帧负载预测与调度（RsFrameDeadlinePredict、RsFrameBlurPredict、RsGameFrameHandler）
- 帧调度事件上报（RsFrameReport 与 FrameUiIntf 联动）
- 渲染管线关键节点打点与 AsyncTrace
- 游戏场景 VSync 偏移调整
- 大面积模糊帧率预测与精确更新
- 帧截止时间（deadline）计算与上报

## 快速代码地图

| 模块 | 关键文件 | 相对路径 |
| --- | --- | --- |
| Frame Analyzer - 帧事件定义 | `frame_info.h` | `rosen/modules/frame_analyzer/export/` |
| Frame Analyzer - 事件采集器 | `frame_collector.h/.cpp` | `rosen/modules/frame_analyzer/export/`、`src/` |
| Frame Analyzer - 帧可视化绘制 | `frame_painter.h/.cpp` | `rosen/modules/frame_analyzer/export/`、`src/` |
| Frame Analyzer - 帧事件落盘 | `frame_saver.h/.cpp` | `rosen/modules/frame_analyzer/src/` |
| Frame Analyzer - 环形队列 | `ring_queue.h` | `rosen/modules/frame_analyzer/export/` |
| Frame Analyzer - 构建 | `BUILD.gn` | `rosen/modules/frame_analyzer/` |
| Frame Load - 帧截止时间预测 | `rs_frame_deadline_predict.h/.cpp` | `rosen/modules/frame_load/include/`、`src/` |
| Frame Load - 模糊帧率预测 | `rs_frame_blur_predict.h/.cpp` | `rosen/modules/frame_load/include/`、`src/` |
| Frame Load - 游戏帧处理 | `rs_game_frame_handler.h/.cpp` | `rosen/modules/frame_load/include/`、`src/` |
| Frame Load - 构建 | `BUILD.gn` | `rosen/modules/frame_load/` |
| Frame Report - 帧调度上报 | `rs_frame_report.h/.cpp` | `rosen/modules/frame_report/include/`、`src/` |
| Frame Report - 构建 | `BUILD.gn` | `rosen/modules/frame_report/` |
| Frame Report - 扩展接口 | `rs_frame_report_ext.h` | `utils/rs_frame_report_ext/include/` |
| 单测 - Frame Analyzer | `frame_collector_test.cpp` 等 | `rosen/test/frame_analyzer/unittest/` |
| 单测 - Frame Load | `game_frame_handler_test.cpp` 等 | `rosen/test/frame_load/unittest/` |
| 单测 - Frame Report | `rs_frame_report_test.cpp` | `rosen/test/frame_report/unittest/` |

## 核心模型

### Frame Analyzer：帧事件采集与可视化

FrameCollector 是全局单例，负责采集渲染管线中 UI 和 RS 两个阶段的关键时间点，存入环形队列（容量 60 帧）。

**数据流**：UI 线程/RS 线程 → `MarkFrameEvent(type, timeNs)` → `ProcessFrameEvent()` → FrameInfoQueue

**关键类型**：

- `FrameEventType`：枚举帧事件类型，UI 阶段包括 HandleInput、Animate、Build、Upload、Layout、Draw（奇偶配对 start/end）；RS 阶段包括 WaitVsync、Release、Flush。起始事件 ID 为奇数，结束事件 ID 为偶数。
- `UIMarks`：UI 阶段时间戳数组，由 UI 线程填充，在 WaitVsyncStart 时合并到 FrameInfo。
- `FrameInfo`：完整一帧的时间戳数组 + 帧号 + skiped 标记，存储在 RingQueue 中。
- `RingQueue<T, cap>`：固定容量环形队列模板，支持正向/反向迭代。

**UI 与 RS 事件合并机制**：UI 事件先写入 `pendingUIMarks_`，当 RS 阶段 WaitVsyncStart 到来时，将 `pendingUIMarks_` 拷贝为 `currentUIMarks_`，并推入 FrameInfoQueue。之后 RS 事件追加到同一 FrameInfo。

**两种输出模式**（通过系统参数 `debug.graphic.frame` 切换）：

- `paint`：启用 FramePainter，在每帧渲染结束时将时间条绘制到屏幕上，以不同颜色表示各阶段耗时，并绘制 60fps/30fps 参考线。帧总时间窗口为 160ms。
- `saver`：启用 FrameSaver，将事件名称和时间戳写入 `/data/frame_render/{pid}.log` 文件。

**AsyncTrace**：FrameCollector 在事件起止时自动调用 `StartAsyncTrace`/`FinishAsyncTrace`（标签 `HITRACE_TAG_GRAPHIC_AGP`），异步 trace 名格式为 `Frame.{index}.{EventName}`。

### Frame Load：帧负载预测与调度

三个独立组件，均使用全局单例模式：

1. **RsFrameDeadlinePredict**：根据当前刷新率和 LTPO 状态计算帧截止时间并上报。120Hz 下，LTPO 开启时根据 VSync 偏移量选择 2.7ms 或 5.4ms 额外保留时间；LTPO 关闭时固定 3ms。非 120Hz 无额外保留。结果通过 `RsFrameReport::ReportFrameDeadline()` 上报。

2. **RsFrameBlurPredict**：预测当前帧是否包含大面积模糊绘制，用于提前调整帧率。分为"预测"（Predictive，在渲染前基于历史信息）和"精确"（Precise，在渲染后确认）两条路径。状态值通过 `RsFrameReport::BlurPredict()` 上报（"0"=无效，"1"=长帧，"2"=短帧）。

3. **RsGameFrameHandler**：监听游戏场景变化，在非 LTPO 且 VSync 偏移已自定义时，切换 RS/App VSync 偏移。游戏场景通过 `FrameReport::HasGameScene()` 查询。统一渲染模式下正常偏移为 5ms，延迟模式为 -3.3ms。

### Frame Report：帧调度事件上报

RsFrameReport 是纯静态工具类，所有方法为静态，通过 `std::call_once` 延迟初始化 `FrameUiIntf`（来自 `frame_aware_sched` 子系统）。

**核心方法与事件映射**：

| 方法 | 上报事件 | 用途 |
| --- | --- | --- |
| `InitDeadline()` | INIT | 初始化调度 |
| `RenderStart(timestamp, skipFirstFrame)` | RS_RENDER_START | 渲染开始 |
| `RenderEnd()` | RS_RENDER_END | 渲染结束 |
| `UniRenderStart/End()` | RS_UNI_RENDER_START/END | 统一渲染起止 |
| `SendCommandsStart()` | RS_SEND_COMMANDS_START | 命令发送开始 |
| `SetFrameParam(requestId, load, schedFrameNum, value)` | SET_FRAME_PARAM | 设置帧参数 |
| `CheckUnblockMainThreadPoint()` | RS_UNBLOCK_MAINTHREAD | 解锁主线程检查点 |
| `CheckPostAndWaitPoint()` | RS_POST_AND_WAIT | PostAndWait 检查点 |
| `CheckBeginFlushPoint()` | RS_BEGIN_FLUSH | 刷新开始检查点 |
| `ReportFrameDeadline(deadline, rate)` | RS_FRAME_DEADLINE | 帧截止时间上报 |
| `ReportBufferCount(count)` | RS_BUFFER_COUNT | 缓冲区数量上报 |
| `ReportComposerInfo(screenId, composerTid)` | RS_COMPOSER_INFO | 合成器信息上报 |
| `ReportUnmarshalData(unmarshalTid, dataSize)` | RS_UNMARSHAL_DATA | 反序列化数据上报 |
| `ReportScbSceneInfo(description, eventStatus)` | GPU_SCB_SCENE_INFO | SCB 场景信息上报 |
| `BlurPredict(payload)` | RS_BLUR_PREDICT | 模糊预测上报 |

**RsFrameReportExt**（`utils/rs_frame_report_ext/`）：通过 dlopen 加载 `libframe_aware_sched.z.so`，提供 `GetEnable`、`HandleSwapBuffer`、`RequestNextVSync`、`ReceiveVSync` 四个函数指针接口，作为 Frame Report 的扩展层。

**Vulkan 窗口信息上报**（`RS_ENABLE_VK` 宏控制）：创建 Vulkan instance/device，获取 `vkSetFrontWindowStatusHUAWEI` 扩展函数指针，上报全屏应用信息。最多重试 3 次。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 帧事件分 UI 和 RS 两段 | `FrameEventType` 枚举 UIMarksStart/End 和 LoopStart/End 分区 | UI 线程和 RS 线程独立打点，在 VSync 边界合并，避免跨线程锁 |
| 环形队列固定 60 帧容量 | `frameQueueMaxSize = 60`、`RingQueue<FrameInfo, 60>` | 覆盖 1 秒@60fps 的历史，内存固定，无需动态分配 |
| 通过系统参数切换模式 | `WatchParameter("debug.graphic.frame", ...)` | 运行时动态开关，不重编译；paint 用于开发阶段可视化，saver 用于离线分析 |
| FrameCollector 双锁 | `pendingMutex_` + `frameQueueMutex_` | pending 保护 UI 事件写入，frameQueueMutex 保护队列和 before/after 指针 |
| FrameReport 静态类 + call_once | 所有方法静态，`std::call_once` 初始化 | 全局唯一入口，延迟初始化避免启动开销 |
| FrameReport 额外上报 Vulkan 前台窗口 | `RS_ENABLE_VK` 条件编译块 | GPU 驱动需要知道前台应用以优化调度，通过 Vulkan 扩展传递 |
| RsFrameBlurPredict 双路径 | `PredictDrawLargeAreaBlur` + `UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely` | 预测路径提前调整帧率，精确路径用于校准，两者配合减少模糊帧卡顿 |
| RsGameFrameHandler 仅在非 LTPO 生效 | `if (isLtpoEnabled || !isVsyncOffsetCustomized) return;` | LTPO 模式下刷新率自适应，不需要手动调整 VSync 偏移 |
| RsFrameDeadlinePredict 120Hz 特殊处理 | `if (currentRate == OLED_120_HZ)` 分支 | 120Hz 帧预算紧张（8.33ms），需要精确的额外保留时间计算 |

## 待补充背景

- FrameCollector 的 `repaint_` 回调在哪些场景下被设置，具体的触发流程。
- FrameAnalyzer 在 `mingw` 平台的简化实现（`src/mingw/`）与设备实现的差异。
- FrameUiIntf（`frame_aware_sched`）的内部调度策略和 RS 侧上报事件的消费方式。
- RsFrameDeadlinePredict 中 `CreateVSyncGenerator()` 的生命周期管理。
- RsFrameBlurPredict 与 HGM（Hyper Graphic Manager）的 RsVariableFrameRate 的交互细节。
- RsGameFrameHandler 的 `handleGameSceneChangedFunc_` 和 `getCoreInfoFunc_` 在 RSMainThread 中的注册时机。
- RsFrameReportExt 加载的 `libframe_aware_sched.z.so` 的具体路径和版本兼容性。
- Vulkan 前台窗口上报 `vkSetFrontWindowStatusHUAWEI` 在不同 GPU 厂商的可用性。
- 历史线上问题：帧事件丢失、AsyncTrace 不配对、saver 文件权限等典型案例。
