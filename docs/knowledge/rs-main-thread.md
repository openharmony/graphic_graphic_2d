# RS 主线程

## 适用范围

### 框架范围

- 单例生命周期与初始化（Instance / Init）
- VSync 驱动帧循环（OnVsync → mainLoop_）
- TransactionData 接收、分类、合并与执行
- IPC 连接管理（ApplicationAgent / connections / SendCommands）
- RSContext 节点树上下文
- 焦点窗口信息与屏幕事件
- 任务投递（PostTask / PostSyncTask / ScheduleTask）
- 内存清理框架（时机触发，具体策略见各特性）

### 特性范围

- 遮挡计算与可见性回调（WMS + 应用层）
- HWC 与 DirectComposition
- 控件截图 / 窗口截图
- UIFirst 子线程预渲染
- DVSync 延迟 VSync
- HGM 刷新率策略 / LTPO / VSync Rate Reduce
- FastCompose / AdaptiveCompose
- 系统动画场景
- 水印（全局 + Surface）
- HDR 亮度与色温
- Tunnel 层 / Delegate 合成
- Buffer 回收 / Single Frame Composer
- DRM 受保护内容
- 幕帘屏 / 无障碍 / Dark Mode
- UIExtension
- StatusBar Dirty-Only / LPP Video
- 丢帧 / Jank 优化 / Surface FPS

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 主线程头文件 | `rosen/modules/render_service/core/pipeline/main_thread/rs_main_thread.h` | 单例，Init/OnVsync/mainLoop_ |
| 主线程实现 | `rosen/modules/render_service/core/pipeline/main_thread/rs_main_thread.cpp` | 帧循环主逻辑（6188 行） |
| 渲染监听 | `rs_render_service_listener.h` / `rs_uni_render_listener.h` | VSync 回调入口 |
| 渲染访问器 | `rs_render_service_visitor.h` / `rs_uni_render_visitor.h` | 遍历节点树（分离/统一两条路径） |
| RSContext | `rosen/modules/render_service_base/include/pipeline/rs_context.h` | 主线程持有的节点树上下文 |
| VSync 接收 | `platform/common/rs_vsync_client.h` | VSync 信号接收 |
| 事务数据 | `transaction/rs_transaction_data.h` | 客户端→服务端属性/命令数据 |
| UIFirst | `rosen/modules/render_service/core/feature/uifirst/rs_uifirst_manager.h` | 子线程预渲染管理 |
| HGM | `rosen/modules/hyper_graphic_manager/core/hgm_render_context.h` | 刷新率策略与 LTPO |
| Tunnel | `rosen/modules/render_service/core/feature/tunnel/rs_tunnel_layer_manager.h` | Tunnel 层管理 |
| DirectComposition | `rosen/modules/render_service/core/feature/hwc/rs_direct_composition_helper.h` | DirectComposition 辅助 |

## 框架通路

### 帧循环主流程

RSMainThread 是单例，运行在 RenderService 进程的主 EventHandler 上。`Init()` 构造 `mainLoop_` lambda，每帧由 VSync 驱动 `OnVsync()` 调用：

```
OnVsync(timestamp)
  → MergeToEffectiveTransactionDataMap(cachedTransactionDataMap_)  // 合并 IPC 线程数据
  → mainLoop_():
      1. RenderFrameStart                          // trace/frame report 开始
      2. ProcessDelegateCompositeCommand           // ← Delegate 合成
      3. ConsumeAndUpdateAllNodes                  // ← UIFirst/丢帧/Buffer回收/Tunnel/DRM/HGM/SurfaceFPS/LPP
      4. WaitUntilUnmarshallingTaskFinished        // ← DVSync 同步屏障
      5. ProcessCommand                            // 事务命令执行（框架核心）
      6. Animate(timestamp)                        // ← HGM 帧率通知
      7. CollectInfoForHardwareComposer             // ← HWC/截图/幕帘屏
      8. Render()                                  // ← 遮挡/HDR亮度/DirectComposition
      9. OnUniRenderDraw                           // 渲染线程投递
      10. UIExtensionNodesTraverseAndCallback       // ← UIExtension
      11. SendCommands                             // IPC 命令回送
      12. Cleanup                                  // ← 内存清理/节点 GC
```

### TransactionData 处理链路

1. 客户端通过 `RSClientToRenderConnection` 发送 `RSTransactionData`。
2. `RecvRSTransactionData()` 接收后存入 `cachedTransactionDataMap_`（统一渲染）或进入 `ClassifyRSTransactionData()`（分离渲染）。
3. 同步事务存入 `syncTransactionData_`，异步事务存入 `cachedTransactionDataMap_`。
4. `OnVsync` 中 `MergeToEffectiveTransactionDataMap()` 合并去重。
5. `ProcessCommandForUniRender()` / `ProcessCommandForDividedRender()` 执行命令。
6. `ProcessSplitTransactionCommands()` 处理分帧重建事务（时间预算执行）。
7. ← Single Frame Composer：IPC 线程立即执行，跳过 VSync 等待。

### IPC 连接管理

- `RegisterApplicationAgent(pid, app)` / `UnRegisterApplicationAgent(pid)`：注册/注销客户端应用代理。
- `connections_`：维护所有 `RSIClientToRenderConnection` IPC 连接。
- `SendCommands()`：每帧末尾通过 `applicationAgentMap_` 将命令回送客户端。

### 任务投递机制

- `PostTask(task)` / `PostTask(task, name, delay, priority)`：异步投递到主线程 EventHandler。
- `PostSyncTask(task)`：同步投递，阻塞等待完成。
- `ScheduleTask<T>(task)`：模板封装，返回 `future<T>`，外部线程可安全投递并获取结果。
- `ForceRefreshForUni()`：跳过 VSync 等待，立即触发下一帧（← FastCompose / Single Frame Composer）。

### 焦点与屏幕事件

- `SetFocusAppInfo(info)`：WMS 设置焦点应用 PID/UID/bundle/ability/nodeId → `focusNodeId_`。
- `OnScreenConnected(property)`：创建 ScreenNode，更新刷新率。
- `OnScreenDisconnected(screenId)`：销毁 ScreenNode。
- `OnScreenPropertyChanged()`：分发屏幕刷新率、电源、物理模式等属性变更。

### 内存清理框架

- `ClearMemoryCache(moment, deeply, pid)`：根据时机（APP 切后台、内存压力、进程退出）清理 CPU/GPU 缓存。
- `HandleOnTrim(level)`：响应系统内存水位，订阅 `rsAppStateListener_`。
- `ReleaseSurface()` / `ReleaseImageMem()`：帧间隙通过 RSBackgroundThread 释放 Drawing::Surface 和图片缓存。
- `CleanResources(pid)`：进程退出时清理节点树、事务数据、回调、水印等全部资源。

## 特性梳理

### 遮挡计算

- **功能**：计算 ScreenNode 下各 SurfaceRenderNode 的可见区域，通知 WMS 和应用层可见性变化。
- **挂入帧循环**：步骤 8 Render() 中调用 `CalcOcclusion()`（分离渲染路径）；统一渲染路径由 `RSUniRenderVisitor` 处理。
- **关键入口**：`CalcOcclusion()`, `RegisterOcclusionChangeCallback(pid, cb)`, `RegisterSurfaceOcclusionChangeCallback(id, pid, cb, partitionPoints)`
- **关键成员**：`occlusionListeners_`（WMS 按pid）、`surfaceOcclusionListeners_`（应用按NodeId）
- **交互**：系统动画场景影响遮挡优先级；幕帘屏遮挡整个屏幕。

### HWC 与 DirectComposition

- **功能**：收集硬件合成节点，尝试 DirectComposition 跳过 GPU 渲染，节省功耗。
- **挂入帧循环**：步骤 7 `CollectInfoForHardwareComposer()` + 步骤 8 Render() 中 `DoDirectComposition()`。
- **关键入口**：`CollectInfoForHardwareComposer()`, `DoDirectComposition()`, `CheckIfHardwareForcedDisabled()`
- **关键成员**：`hardwareEnabledNodes_`, `isHardwareForcedDisabled_`, `directComposeHelper_`
- **禁用条件**：动画进行中、HDR 内容、DRM 受保护层、多屏/折叠切换、节点有 shadow/filter、UIFirst 跳帧需补帧、幕帘屏、水印。

### 截图（控件截图 / 窗口截图）

- **功能**：控件截图截取节点子树；窗口截图利用 UIFirst 缓存加速。
- **挂入帧循环**：步骤 5 ProcessCommand 后 → `PrepareUiCaptureTasks()`（等待 cmds 执行）→ 步骤 7-8 间 `ProcessUiCaptureTasks()`。
- **关键入口**：`AddUiCaptureTask(id, task)`, `AddWindowCapTask(id, task)`, `IsSnapshotPendingThisFrame()`
- **关键成员**：`pendingUiCaptureTasks_`, `uiCaptureTasks_`, `pendingWindowCapTasks_`
- **交互**：截图任务 pending 时禁用 DirectComposition；窗口截图优先使用 UIFirst 缓存。

### UIFirst

- **功能**：子线程预渲染 UI 内容，降低首帧延迟。
- **挂入帧循环**：步骤 3 ConsumeAndUpdateAllNodes 中检查 UIFirst 参数；步骤 7 CollectInfoForHWC 中检查跳帧补帧。
- **关键入口**：`SetUifirstScale(scale)`, RSUifirstManager 各 Prepare/Process 方法
- **关键成员**：`uifirstScale_`
- **交互**：UIFirst 跳帧需补帧时禁用 DirectComposition；窗口截图可复用 UIFirst 缓存。

### DVSync

- **功能**：延迟 VSync 机制，命令处理与 VSync 时序解耦，提升 UI 响应性。
- **挂入帧循环**：OnVsync 前 `DVSyncUpdate()`；步骤 4 `WaitUntilUnmarshallingTaskFinished()` 同步屏障。
- **关键入口**：`DVSyncUpdate()`, `SetForceRsDVsync(sceneId)`, `NotifyUnmarshalTask(uiTimestamp)`
- **关键成员**：`rsVsyncManagerAgent_`, `unmarshalBarrierTask_`, `needWaitUnmarshalFinished_`
- **交互**：DVSync 时间偏移影响 FastCompose 时序判断。

### HGM / LTPO / VSync Rate Reduce

- **功能**：刷新率策略（HGM）、LTPO 动态刷新率、按 Surface fps 投票和降频。
- **挂入帧循环**：Init 中初始化 `hgmRenderContext_`；步骤 3 更新 Surface fps 数据；步骤 6 Animate 中通知帧率；步骤 8 UniRender 中重置帧率参数。
- **关键入口**：`GetRSVsyncRateReduceManager()`, `hgmRenderContext_->NotifyRpHgmFrameRate()`
- **关键成员**：`hgmRenderContext_`, `rsVsyncRateReduceManager_`

### FastCompose / AdaptiveCompose

- **功能**：buffer 到达时 mid-VSync 渲染（跳过下一 VSync 等待）；游戏自适应合成。
- **挂入帧循环**：RecvRSTransactionData 中 `CheckFastCompose()` 触发 `ForceRefreshForUni()`。
- **关键入口**：`CheckFastCompose()`, `CheckAdaptiveCompose()`, `ForceRefreshForUni()`
- **关键成员**：`lastFastComposeTimeStamp_`, `vsyncRsTimestamp_`

### 系统动画场景

- **功能**：追踪系统级过渡动画（任务中心、三指手势），影响遮挡优先级和渲染调度。
- **挂入帧循环**：mainLoop_ 中 `CheckSystemSceneStatus()` 清理过期场景。
- **关键入口**：`SetSystemAnimatedScenes(scene, isRegular)`, `SetAnimationOcclusionInfo(sceneId, isStart)`
- **关键成员**：`systemAnimatedScenesList_`, `threeFingerScenesList_`
- **交互**：影响遮挡计算优先级；动画进行中禁用 DirectComposition。

### 水印（全局 + Surface）

- **功能**：全局水印叠加在 DisplayNode 上；Surface 水印叠加在指定 SurfaceNode 上。
- **挂入帧循环**：Set → `SetDirtyFlag()` + `RequestNextVSync()` → renderThreadParams 同步到渲染线程。
- **关键入口**：`SetWatermark(pid, name, img)`, `SetSurfaceWatermark(pid, name, img, nodeList, type)`
- **关键成员**：`watermarkImg_`, `watermarkFlag_`, `surfaceWatermarkHelper_`
- **交互**：水印存在时禁用 DirectComposition。

### HDR 亮度与色温

- **功能**：HDR 亮度控制（dimming post-process）、色温调节、HDR 状态检测。
- **挂入帧循环**：步骤 8 Render() 中 `UpdateLuminanceAndColorTemp()`。
- **关键入口**：`SetLuminanceChangingStatus(id, changed)`, `ExchangeLuminanceChangingStatus(id)`
- **关键成员**：`displayLuminanceChanged_`, `isHdrSwitchChanged_`, `isColorTemperatureOn_`
- **交互**：HDR 内容禁用 DirectComposition；HDR 状态影响 HWC 收集。

### Tunnel 层 / Delegate 合成

- **功能**：Tunnel 层让 Web 内容直达 DSS 减少延迟；Delegate 合成让 WebView 由 HWC 直接合成跳过 GPU。
- **挂入帧循环**：步骤 3 ConsumeAndUpdateAllNodes 中 `tunnelRouteArbiter_->ArbitrateAndClaim()` + 多处 `RefreshGlobalTriggerSnapshot()`；步骤 2 `ProcessDelegateCompositeCommand()`。
- **关键入口**：`GetTunnelLayerStateHandler()`, `ProcessDelegateCompositeCommand()`, `TraverseNodeForDelegateMode()`
- **关键成员**：`tunnelLayerManager_`, `tunnelRouteArbiter_`, `isWebCommandOnly_`

### Buffer 回收 / Single Frame Composer

- **功能**：回收离树 Web SurfaceNode 的末帧 buffer 释放内存；IPC 线程立即执行事务降低延迟。
- **挂入帧循环**：步骤 3 ConsumeAndUpdateAllNodes 中 `PostTryReclaimLastBuffer()`；RecvRSTransactionData 中 `IsNeedProcessBySingleFrameComposer()` → `ProcessDataBySingleFrameComposer()`。
- **关键入口**：`IsNeedProcessBySingleFrameComposer()`, `ProcessDataBySingleFrameComposer()`
- **关键成员**：`curFrameBufferReclaimCount_`

### DRM 受保护内容

- **功能**：DRM 保护内容必须走 HWC 直通路径，不允许 GPU 渲染。
- **挂入帧循环**：步骤 3 ConsumeAndUpdateAllNodes 中 `RSDrmUtil::DealWithDRMNodes()`；步骤 7 HWC 强制禁用判断。
- **关键入口**：`SetHasSurfaceLockLayer(has)`, `HasDRMOrSurfaceLockLayer()`
- **关键成员**：`hasProtectedLayer_`, `hasSurfaceLockLayer_`

### 幕帘屏 / 无障碍 / Dark Mode

- **功能**：幕帘屏（盲人隐私保护）；无障碍配置变更追踪（色弱反转高对比）；全局暗色模式。
- **挂入帧循环**：Set → `SetDirtyFlag()` + `RequestNextVSync()` → 步骤 8 Render() 中 `isCurtainScreenUsingStatusChanged_` 强制重渲染。
- **关键入口**：`SetCurtainScreenUsingStatus(isOn)`, `SetAccessibilityConfigChanged()`, `SetGlobalDarkColorMode(isDark)`
- **关键成员**：`isCurtainScreenOn_`, `isAccessibilityConfigChanged_`, `accessibilityObserver_`, `isGlobalDarkColorMode_`

### UIExtension

- **功能**：UI 扩展组件（嵌入其他应用的 Ability 组件），收集并分发布局/可见性数据到宿主应用。
- **挂入帧循环**：步骤 10 `UIExtensionNodesTraverseAndCallback()`。
- **关键入口**：`RegisterUIExtensionCallback(pid, userId, cb)`, `UnRegisterUIExtensionCallback(pid)`
- **关键成员**：`uiExtensionListenners_`, `uiExtensionCallbackData_`

### StatusBar Dirty-Only / LPP Video

- **功能**：仅状态栏变化时跳过完整 GPU 渲染，DirectComposition 处理；LPP Video 低功耗播放 buffer 消费。
- **挂入帧循环**：步骤 8 Render() 中 `IfStatusBarDirtyOnly()` 判断；步骤 3 ConsumeAndUpdateAllNodes 中 `lppVideoHandler_` 消费。
- **关键入口**：`IfStatusBarDirtyOnly()`, `GetLppVideoHander()`
- **关键成员**：`ifStatusBarDirtyOnly_`, `lppVideoHandler_`

### 丢帧 / Jank 优化 / Surface FPS

- **功能**：按 PID 丢帧降低 CPU 负载；动画过渡期间丢弃 Jank 统计帧避免误报；Surface fps 操作列表管理。
- **挂入帧循环**：步骤 3 ConsumeAndUpdateAllNodes 配置 `dropFrameConfig`；步骤 1/12 Jank 打点；步骤 3 收集 `addSurfaceFpsOpMap_`。
- **关键入口**：`AddPidNeedDropFrame(pidList, level)`, `SetDiscardJankFrames(flag)`, `AddSurfaceFpsOp(op)`
- **关键成员**：`surfacePidNeedDropFrame_`, `discardJunkFrames_`, `addSurfaceFpsOpMap_`

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| VSync 驱动主循环 | `OnVsync()` → mainLoop_ 12 步 | 确保帧处理与显示刷新同步，避免撕裂 |
| 同步/异步事务分离 | `syncTransactionData_` vs `cachedTransactionDataMap_` | 同步事务需等待对端完成，异步可延迟合并 |
| ScheduleTask 模板 | `Detail::ScheduledTask` + `PostTask` | 外部线程安全向主线程投递任务并获取 future |
| mainLoop_ lambda | `Init()` 中一次性构造 | 12 步流程集中定义，避免分散在 OnVsync 调用链中 |
| 分帧重建事务 | `ProcessSplitTransactionCommands()` + 时间预算 | 大事务拆分执行，避免单帧超时 |
| 遮挡回调双通道 | `occlusionListeners_`(按pid) + `surfaceOcclusionListeners_`(按NodeId) | WMS 需全局可见性，应用需单 Surface 可见性 |
| HWC 禁用条件集中 | `CheckIfHardwareForcedDisabled()` | 多种禁用条件（动画/HDR/DRM/多屏/filter/水印/幕帘屏）集中判断 |
| FastCompose mid-VSync | `CheckFastCompose()` + `ForceRefreshForUni()` | buffer 到达时立即渲染，跳过 VSync 等待 |
| StatusBar Dirty Only | `ifStatusBarDirtyOnly_` | 仅状态栏变化时 DirectComposition 处理 |
| DVSync 解耦 | `DVSyncUpdate()` / `WaitUntilUnmarshallingTaskFinished()` | 命令处理时序与 VSync 解耦 |
| SingleFrameComposer | `ProcessDataBySingleFrameComposer()` | IPC 线程立即执行，极致降延迟 |
| 帧循环特性挂入点标注 | 各步骤中特性方法调用 | 框架与特性解耦，特性按步骤挂入而非修改主流程 |

## 待补充背景

- DVSync 的具体时序逻辑和帧率匹配算法。
- FastCompose 与 PartialRender 的关系和切换条件。
- `systemAnimatedScenesList_` 的完整场景枚举和优先级策略。
- `tunnelLayerManager_` 的 tunnel 层创建和仲裁逻辑。
- Delegate 合成的完整 Z-order 调整和 DirectComposition 标志更新流程。
- SingleFrameComposer 的适用条件（无动画、单窗口）完整枚举。
- 帧循环各步骤的耗时分布和性能瓶颈定位方法。
- 历史线上主线程卡顿典型案例和排查路线。
