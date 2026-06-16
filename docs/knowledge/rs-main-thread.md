# RS 主线程与连接管理

## 适用范围

- RSMainThread 的初始化、VSync 驱动主循环、帧调度
- TransactionData 接收、分类与合并
- 遮挡计算与可见性回调
- 内存清理策略与时机
- 硬件合成（HWC）决策和 DirectComposition
- 客户端 IPC 连接管理

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 主线程头文件 | `rosen/modules/render_service/core/pipeline/main_thread/rs_main_thread.h` | 单例，Init/OnVsync/ProcessCommand/Animate/Render |
| 主线程实现 | `rosen/modules/render_service/core/pipeline/main_thread/rs_main_thread.cpp` | 帧循环主逻辑 |
| 渲染监听 | `rs_render_service_listener.h` / `rs_uni_render_listener.h` | VSync 回调入口 |
| 渲染访问器 | `rs_render_service_visitor.h` / `rs_uni_render_visitor.h` | 遍历节点树 |
| RSContext | `rosen/modules/render_service_base/include/pipeline/rs_context.h` | 主线程持有的节点树上下文 |
| VSync 接收 | `platform/common/rs_vsync_client.h` | VSync 信号接收 |
| 事务数据 | `transaction/rs_transaction_data.h` | 客户端→服务端属性/命令数据 |

## 核心模型

### 帧循环主流程

RSMainThread 是单例，运行在 RenderService 进程的主 EventHandler 上。每帧由 VSync 驱动：

```
OnVsync(timestamp)
  → ProcessCommand()           // 处理所有待执行的 RSCommand
  → Animate(timestamp)         // 驱动动画更新
  → ConsumeAndUpdateAllNodes() // 消费节点 buffer、更新脏区域
  → CalcOcclusion()            // 遮挡计算
  → Render()                   // 分发到 RSUniRenderThread 或本地渲染
  → CollectInfoForHardwareComposer() // 收集 HWC 信息
```

### TransactionData 处理链路

1. 客户端通过 `RSClientToRenderConnection` 发送 `RSTransactionData`。
2. `RecvRSTransactionData()` 接收后进入 `ClassifyRSTransactionData()`。
3. 同步事务存入 `syncTransactionData_`，异步事务存入 `cachedTransactionDataMap_`。
4. `MergeToEffectiveTransactionDataMap()` 合并去重。
5. `ProcessCommandForUniRender()` / `ProcessCommandForDividedRender()` 执行命令。

### 遮挡计算

- `CalcOcclusion()` 遍历 ScreenNode 下所有 SurfaceRenderNode。
- 使用 Region 裁剪算法计算可见区域，结果通过 `SurfaceOcclusionChangeCallback` 回调给 WMS。
- `surfaceOcclusionListeners_` 按 NodeId 注册 `RSISurfaceOcclusionChangeCallback`。

### 内存管理

- `ClearMemoryCache(moment, deeply, pid)` 根据不同时机（APP 切后台、内存压力等）清理缓存。
- `HandleOnTrim()` 响应系统内存水位。
- `ReleaseSurface()` / `ReleaseImageMem()` 在帧间隙释放 GPU 资源。

### HWC 与 DirectComposition

- `CollectInfoForHardwareComposer()` 收集 `hardwareEnabledNodes_`。
- `DoDirectComposition()` 尝试直接合成，跳过 GPU 渲染。
- `isHardwareForcedDisabled_` 当节点有 shadow/filter 时强制禁用 HWC。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| VSync 驱动主循环 | `OnVsync()` → ProcessCommand → Animate → Render | 确保帧处理与显示刷新同步，避免撕裂 |
| 同步/异步事务分离 | `syncTransactionData_` vs `cachedTransactionDataMap_` | 同步事务需等待对端完成，异步可延迟合并 |
| ScheduleTask 模板 | `Detail::ScheduledTask` + `PostTask` | 外部线程安全向主线程投递任务并获取 future |
| 遮挡回调双通道 | `occlusionListeners_`（按 pid）+ `surfaceOcclusionListeners_`（按 NodeId） | WMS 需要全局可见性，应用需要单个 Surface 可见性 |
| FastCompose | `IsFastComposeAllow()` / `CheckFastCompose()` | 无脏区域时跳过完整渲染，节省功耗 |
| StatusBar Dirty Only | `ifStatusBarDirtyOnly_` | 仅状态栏变化时只重绘状态栏区域 |
| DVSync | `DVSyncUpdate()` / `NeedConsumeDVSyncCommand()` | 动画帧率与屏幕刷新率解耦，支持可变帧率 |

## 待补充背景

- DVSync 的具体时序逻辑和帧率匹配算法。
- FastCompose 与 PartialRender 的关系和切换条件。
- `systemAnimatedScenesList_` 的完整场景枚举和优先级策略。
- `tunnelLayerManager_` 的 tunnel 层创建和仲裁逻辑。
- 历史线上主线程卡顿典型案例和排查路线。
