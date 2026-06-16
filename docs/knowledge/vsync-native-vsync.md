# VSync 与 Native VSync

## 适用范围

- VSync 信号生成、分发与采样
- DVSync（显示 VSync）延迟机制
- LTPO/LTPS 刷新率模式切换
- Adaptive Sync 自适应同步
- VSync 连接管理与 QoS 帧率控制
- Native VSync 客户端接收

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| VSyncGenerator | `rosen/modules/composer/vsync/include/vsync_generator.h` | VSync 信号生成器，计算下一次 VSync 时间 |
| VSyncController | `rosen/modules/composer/vsync/include/vsync_controller.h` | VSync 控制器，管理使能和相位偏移 |
| VSyncDistributor | `rosen/modules/composer/vsync/include/vsync_distributor.h` | VSync 分发器，管理连接和事件分发 |
| VSyncConnection | 同上（内嵌类） | VSync 连接，每个客户端一个 |
| VSyncSampler | `rosen/modules/composer/vsync/include/vsync_sampler.h` | VSync 采样器，从硬件 VSync 采样计算周期 |
| DVSyncLibManager | `rosen/modules/composer/vsync/include/dvsync_lib_manager.h` | DVSync 动态库管理器 |
| IVSyncConnection | `rosen/modules/composer/vsync/include/ivsync_connection.h` | VSync 连接 IPC 接口 |
| VSyncConnectionStub | `rosen/modules/composer/vsync/include/vsync_connection_stub.h` | VSync 连接 Stub |
| VSyncConnectionProxy | `rosen/modules/composer/vsync/include/vsync_connection_proxy.h` | VSync 连接 Proxy |
| Native VSync fuzz | `rosen/modules/composer/native_vsync/test/fuzztest/` | Native VSync fuzz 测试 |

## 核心模型

### VSync 管线

VSync 信号的处理链路为：硬件 VSync → `VSyncSampler`（采样计算 period/phase/referenceTime）→ `VSyncGenerator`（根据采样结果生成软件 VSync）→ `VSyncController`（控制使能和相位）→ `VSyncDistributor`（分发到各连接）→ `VSyncConnection`（通过 socketpair 发送到客户端）。

### VSyncGenerator

核心单例，运行在独立线程（支持 ffrt 线程）。关键逻辑：

- `UpdateMode`：更新 period/phase/referenceTime
- `ChangeGeneratorRefreshRateModel`：根据各连接的刷新率需求，选择合适的生成频率
- 支持 LTPO 模式（按脉冲分发）和 LTPS 模式（按周期分发）
- DVSync 支持：`AddDVSyncListener` / `RemoveDVSyncListener` 独立监听
- Adaptive Sync：`CheckSampleIsAdaptive` / `SetAdaptive` 自适应同步

### VSyncDistributor

管理 `VSyncConnection` 集合，核心功能：

- `RequestNextVSync`：客户端请求下一个 VSync
- `SetVSyncRate`：设置连续 VSync 速率（LTPS 模式）
- `SetQosVSyncRate`：按 windowNodeId 设置 QoS 帧率
- `CollectConnections` / `CollectConnectionsLTPO`：收集需要触发的连接
- DVSync 集成：`SetUiDvsyncSwitch` / `SetNativeDVSyncSwitch` / `InitDVSync`

### VSyncConnection

每个客户端一个连接，通过 `LocalSocketPair` 发送 VSync 事件。关键属性：

- `rate_`：LTPS 速率控制
- `id_`：连接 ID（用于 LTPO）
- `windowNodeId_`：窗口节点 ID（用于 QoS）
- `vsyncPulseFreq_` / `referencePulseCount_`：LTPO 脉冲参数
- `requestVsyncTimestamp_`：精确 VSync 请求时间戳集合

### VSyncSampler

从硬件 VSync 或 PresentFence 采样，计算 period/phase/referenceTime。关键参数：

- `MAX_SAMPLES = 32`：最大采样数
- `MIN_SAMPLES_FOR_UPDATE = 6`：最小更新采样数
- `isAdaptive_`：Adaptive Sync 标志

### DVSyncLibManager

动态加载 `libdvsync.z.so`，提供 DVSync 延迟决策的函数指针集合。支持包名事件通知、触摸事件、buffer 信息等输入，输出 VSync 跳过/延迟决策。

## Native VSync 外部链路

Native VSync 是应用侧 C API 到 RS VSync 分发器的入口：

```text
OH_NativeVSync_Create()
  -> CreateAndInitVSyncReceiver()
  -> VSyncReceiver::Init()
  -> VSyncDistributor::AddConnection()
  -> OH_NativeVSync_RequestFrame()
  -> VSyncReceiver::RequestNextVSync()
  -> VSyncConnection::RequestNextVSync()
  -> VSyncDistributor::RequestNextVSync()
  -> socketpair callback to app
```

关键函数：

- `OH_NativeVSync_Create()` / `OH_NativeVSync_Create_ForAssociatedWindow()`。
- `OH_NativeVSync_RequestFrame()` / `OH_NativeVSync_RequestFrameWithMultiCallback()`。
- `OH_NativeVSync_GetPeriod()`。
- `OH_NativeVSync_DVSyncSwitch()`。
- `OH_NativeVSync_SetExpectedFrameRateRange()`。

NDK 层输入要按公共 API 处理：name 长度、callback 空指针、`period` 输出指针、
expected range 的 min/max/preferred、windowId 和关联窗口生命周期都要检查。

## VSync / HWC / HDI 设备链路

VSync 的真实设备链路要同时看采样、软件生成和硬件刷新率：

```text
硬件 VSync / PresentFence
  -> VSyncSampler::AddSample() / AddPresentFenceTime()
  -> VSyncSampler::UpdateModeLocked()
  -> VSyncGenerator::UpdateMode()
  -> VSyncController phase/enable
  -> VSyncDistributor::OnVSyncEvent()
  -> VSyncConnection callback
  -> RS main loop / app callback
```

和 Composer/HGM 的关系：

- HGM 决策目标刷新率后，Composer/HdiBackend 负责切换屏幕 mode。
- `VSyncSampler::SetPendingPeriod()` 可记录待切换周期，不能等同于硬件已切换。
- `VSyncDistributor::CollectConnections()` 处理 LTPS 连接分发。
- `VSyncDistributor::CollectConnectionsLTPO()` 处理 LTPO pulse 连接分发。
- `DVSyncLibManager` 只影响延迟/跳帧策略，不替代硬件 VSync 能力判断。

涉及 HWC/HDI 时要同时读 `docs/knowledge/composer-hdi.md`。

## Fallback 和异常路径

需要明确覆盖以下 fallback：

- 硬件 VSync 关闭或采样不足时，软件 VSync 继续生成稳定 callback。
- PresentFence 时间异常、倒退或抖动时，`VSyncSampler` 应拒绝异常 sample。
- Native VSync 未初始化时，`RequestNextVSync` 返回错误，不触发 callback。
- callback 为空、receiver 为空、period 输出指针为空时，NDK 层直接失败。
- DVSync 动态库加载失败或开关关闭时，回退普通 VSync 分发。
- 屏幕灭屏、热插拔、折叠屏切换时，采样屏幕 id 和 enable 状态要更新。

新增分发策略时，至少覆盖：

- 单 callback 和 multi callback。
- LTPS rate 分频和 LTPO pulse 分发。
- native DVSync 开关和 UI DVSync 开关。
- HGM 刷新率变化后 app callback 周期是否同步变化。

## 验证建议

代码修改优先选最近目标：

```sh
prebuilts/build-tools/linux-x86/bin/ninja -C out/<product-name> \
  //foundation/graphic/graphic_2d/rosen/modules/composer/vsync:libvsync
prebuilts/build-tools/linux-x86/bin/ninja -C out/<product-name> \
  //foundation/graphic/graphic_2d/rosen/modules/composer/native_vsync:libnative_vsync
prebuilts/build-tools/linux-x86/bin/ninja -C out/<product-name> \
  //foundation/graphic/graphic_2d/rosen/modules/composer/native_vsync/test/fuzztest/nativeVsyncFuzzer:fuzztest
```

真实设备验证要记录硬件 mode、目标 period、callback timestamp 间隔、DVSync 开关、
屏幕电源/热插拔状态和是否发生连续丢帧。只跑单测不能证明 HWC/HDI mode 已切换。

没有真实设备时，不默认阻塞文档、静态验证或可本地验证的实现推进。
最终回复或 PR 中必须写清未执行设备验证的原因和风险；
不得把 VSync/HWC/HDI mode 切换描述为“完整验证”。
如果任务目标就是证明设备 callback 周期或 mode 切换，需要人工确认或设备补测后才能关闭。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 软件 VSync 生成 | `VSyncGenerator` 独立线程计算 | 硬件 VSync 不一定持续可用，软件生成保证 VSync 信号稳定 |
| 采样器独立 | `VSyncSampler` 单独类 | 采样和生成分离，采样可从硬件 VSync 或 PresentFence 获取 |
| DVSync 动态库 | `DVSyncLibManager` 加载 so | DVSync 策略闭源，动态加载解耦编译依赖 |
| SocketPair 分发 | `LocalSocketPair` 发送事件 | 跨进程 VSync 通知，socketpair 比 Binder 轻量 |
| QoS 帧率控制 | `SetQosVSyncRate` 按 windowNodeId | 不同窗口帧率需求不同，按窗口粒度控制避免全局影响 |
| Adaptive Sync | `isAdaptive_` / `CheckSampleIsAdaptive` | 根据硬件 VSync 间隔自适应调整，节省功耗 |

## 待补充背景

- DVSync 动态库的具体策略逻辑和配置参数
- LTPO 模式下脉冲分发的完整时序
- VSync 连接的死亡通知和资源回收机制
- Native VSync 客户端的完整 API 和使用流程
- PresentFence 采样的触发条件和回退策略
