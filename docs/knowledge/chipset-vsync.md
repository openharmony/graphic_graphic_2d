# Chipset VSync / Frame Stability

## 适用范围

- 芯片级 VSync 控制
- 帧稳定性检测与回调
- 帧稳定性目标注册与结果查询

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RsChipsetVsync | `rosen/modules/render_service/core/feature/chipset_vsync/rs_chipset_vsync.h` | 芯片级 VSync 控制 |
| RSFrameStabilityManager | `rosen/modules/render_service/core/feature/frame_stability/rs_frame_stability_manager.h` | 帧稳定性管理器 |
| RSFrameStabilityState | `rosen/modules/render_service/core/feature/frame_stability/rs_frame_stability_state.h` | 帧稳定性状态定义 |

## 核心模型

### Chipset VSync

`RsChipsetVsync` 为单例，通过动态加载 so 实现芯片级 VSync 控制：

- `LoadLibrary` / `CloseLibrary`：动态加载/关闭芯片级 VSync 库
- `InitChipsetVsync`：初始化芯片级 VSync
- `SetVsync(timeStamp, curTime, period, allowFramerateChange)`：设置 VSync 参数

函数指针类型：
- `InitChipsetVsyncFunc`：初始化函数签名 `int32_t (*)()`
- `SetVsyncFunc`：设置 VSync 函数签名 `int32_t (*)(int64_t, uint64_t, int64_t, bool)`

关键参数 `allowFramerateChange`：是否允许帧率变化，用于 HGM 与芯片级 VSync 的协调。

线程安全：`mutex_` 保护 so 句柄，`funcMutex_` 保护函数指针。

### Frame Stability

`RSFrameStabilityManager` 为单例，提供帧稳定性检测能力：

**核心 API**：

- `RegisterFrameStabilityDetection`：注册帧稳定性检测，传入目标（`FrameStabilityTarget`）、配置（`FrameStabilityConfig`）和回调（`RSIFrameStabilityCallback`）
- `UnregisterFrameStabilityDetection`：取消注册
- `StartFrameStabilityCollection`：开始采集
- `GetFrameStabilityResult`：获取稳定性结果
- `UpdateFrameStabilityDetection`：更新检测目标
- `RecordCurrentFrameDirty`：记录当前帧脏区域

**资源清理**：

- `CleanResourcesByPid`：进程退出时清理
- `CleanResourcesByScreenId`：屏幕节点销毁时清理

**内部状态**：

- `DetectorContext`：检测上下文，包含配置、回调、检测状态（INIT/STABLE/NOTSTABLE）、累积脏区域、起始时间、屏幕面积
- `CollectorContext`：采集上下文，包含配置、采集脏区域、屏幕面积

**稳定性判断**：

- `RecordDirtyToDetector`：将脏区域记录到检测器
- `RecordDirtyToCollector`：将脏区域记录到采集器
- `CalculateRegionPercentage`：计算区域占比
- `HandleStabilityTimeout`：处理稳定性超时
- `TriggerCallback`：触发稳定性回调

连接数限制：`MAX_FRAME_STABILITY_CONNECTION_NUM = 10`

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| Chipset VSync 动态加载 | `LoadLibrary` / `chipsetVsyncLibHandle_` | 芯片级 VSync 能力因平台而异，动态加载避免编译依赖 |
| Chipset VSync 双锁 | `mutex_` + `funcMutex_` | so 句柄和函数指针生命周期不同，分别保护避免死锁 |
| Frame Stability 检测/采集分离 | `DetectorContext` / `CollectorContext` | 检测需要回调通知，采集只需数据记录，职责分离 |
| 帧稳定性连接数限制 | `MAX_FRAME_STABILITY_CONNECTION_NUM = 10` | 限制并发检测数量，避免资源过度占用 |
| 允许帧率变化参数 | `allowFramerateChange` in SetVsync | HGM 调帧率时芯片级 VSync 可能需要同步，此参数控制是否允许 |
| 脏区域占比判断稳定性 | `CalculateRegionPercentage` | 帧稳定性本质是判断画面是否稳定，脏区域占比是直接指标 |

## 待补充背景

- 芯片级 VSync so 的具体平台和版本
- `FrameStabilityTarget` 和 `FrameStabilityConfig` 的完整字段定义
- 稳定性判断的阈值和超时时间
- `RSIFrameStabilityCallback` 的 IPC 传递机制
- 芯片级 VSync 与 HGM 的协调时序
