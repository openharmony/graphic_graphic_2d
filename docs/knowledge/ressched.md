# ResSched 资源调度

## 适用范围

- RS 侧资源调度事件监听与上报（ResschedEventListener）
- 帧绘制事件上报（DrawFrameReport）
- 帧率统计与上报（FrameRateStatistics）
- FFRT 高优先级队列异步上报
- 与 ResourceSchedule 子系统的交互（ResSchedClient::ReportData）

## 快速代码地图

| 模块 | 关键文件 | 相对路径 |
| --- | --- | --- |
| ResSched 事件监听器 | `ressched_event_listener.h/.cpp` | `rosen/modules/ressched/include/`、`src/` |
| 单测 | `ressched_event_listener_test.cpp` | `rosen/modules/ressched/test/unittest/` |

## 核心模型

### ResschedEventListener：资源调度事件桥接

ResschedEventListener 继承自 `ResourceSchedule::ResSchedEventListener`，是 RS 进程与资源调度子系统之间的桥接层。全局单例，通过 `sptr` 引用计数管理。

**事件接收**：`OnReceiveEvent(eventType, eventValue, extInfo)` 处理两类事件：

1. **EVENT_DRAW_FRAME_REPORT**：帧绘制事件上报控制
   - `EVENT_VALUE_DRAW_FRAME_REPORT_START`：设置 `isNeedReport_ = true`，启动帧绘制上报
   - `EVENT_VALUE_DRAW_FRAME_REPORT_STOP`：设置 `isNeedReport_ = false`，停止帧绘制上报

2. **EVENT_FRAME_RATE_STATISTICS**：帧率统计
   - `EVENT_VALUE_FRAME_RATE_STATISTICS_START`：开始统计指定 pid 的帧率
   - `EVENT_VALUE_FRAME_RATE_STATISTICS_BREAK`：中断统计
   - `EVENT_VALUE_FRAME_RATE_STATISTICS_END`：结束统计并上报帧率

### 帧绘制上报（ReportFrameToRSS）

当 `isNeedReport_` 为 true 时，在每帧渲染结束时调用。采样间隔 100ms（`SAMPLE_TIME = 100000000` ns）。上报类型为 `RES_TYPE_SEND_FRAME_EVENT`，payload 为空。

首次上报（`isFirstReport_`）立即执行，不受采样间隔限制。

### 帧率统计（FrameRateStatistics）

通过 FFRT 高优先级队列异步执行，避免阻塞渲染线程。

**数据流**：
1. RSS 发送 `EVENT_VALUE_FRAME_RATE_STATISTICS_START` → `HandleFrameRateStatisticsBeginAsync`：记录 pid、type，重置计数器
2. RS 渲染线程每帧调用 `ReportFrameCountAsync(pid)`：计数 `frameCountNum_++`，记录时间戳
3. RSS 发送 `EVENT_VALUE_FRAME_RATE_STATISTICS_END` → `HandleFrameRateStatisticsEndAsync`：计算帧率 = `frameCountNum_ / durationTime`，通过 `ReportFrameRateToRSS` 上报

**上报格式**：`RES_TYPE_FRAME_RATE_REPORT_FROM_RS`，payload 包含 `pid`、`type`、`frameRate`。

**FFRT 队列**：`ffrtHighPriorityQueue_` 使用 `ffrt::qos_user_interactive` QoS 等级，通过双重检查锁懒初始化。

### 线程模型

- `OnReceiveEvent`：在 Binder 线程执行，轻量操作直接处理
- `HandleFrameRateStatistics*Async`：投递到 FFRT 高优先级队列异步执行
- `ReportFrameToRSS`：在渲染线程执行，同步调用 `ResSchedClient::ReportData`
- `ReportFrameRateToRSS`：投递到 FFRT 高优先级队列异步执行

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 继承 ResSchedEventListener | `class ResschedEventListener : public ResourceSchedule::ResSchedEventListener` | 利用资源调度子系统的事件分发机制，解耦 RS 和 RSS |
| sptr 单例 + call_once | `GetInstance()` 使用 `std::call_once` 创建 | 线程安全，避免重复创建 |
| 帧率统计异步执行 | FFRT 高优先级队列 | 不阻塞渲染线程，QoS 保证及时执行 |
| 采样间隔 100ms | `SAMPLE_TIME = 100000000` | 平衡上报精度和开销，避免过于频繁的 IPC |
| 首次上报立即执行 | `isFirstReport_` 标记 | 确保调度器尽快获得第一帧信息，缩短响应延迟 |
| 帧率统计用 duration 计算 | `frameRate = frameCountNum_ / durationTime.count()` | 直接计算实际帧率，而非使用 VSync 频率推算 |
| pid 过滤 | `if (pid != currentPid_.load()) return;` | 仅统计目标进程的帧数，避免其他进程干扰 |

## 待补充背景

- `RES_TYPE_SEND_FRAME_EVENT` 和 `RES_TYPE_FRAME_RATE_REPORT_FROM_RS` 在 RSS 侧的具体调度策略。
- 帧绘制上报 `ReportFrameToRSS` 在 RSMainThread 中的调用时机。
- `ReportFrameCountAsync` 在渲染管线中的调用位置。
- FFRT 高优先级队列在 CPU 紧张场景下的行为（是否会被抢占、延迟多久）。
- `isNeedReport_` 的典型触发场景（哪些 RSS 策略会启动帧绘制上报）。
- 帧率统计 `type` 字段的含义和取值范围。
- 历史线上问题：帧率统计不准、上报延迟导致调度不及时等案例。
