# UIFirst

## 适用范围

- UIFirst子线程并行渲染
- 子线程管理与负载均衡
- 缓存 Surface 生命周期与复用
- 帧率控制（含遮挡降频）
- SkipSync 同步跳过机制
- HDR / 颜色空间 / 脏区优化

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSUifirstManager | `rosen/modules/render_service/core/feature/uifirst/rs_uifirst_manager.h` | UIFirst 管理器单例，主/渲染/子线程协调 |
| RSSubThreadManager | `rosen/modules/render_service/core/feature/uifirst/rs_sub_thread_manager.h` | 子线程池管理器，负载均衡 |
| RSSubThread | `rosen/modules/render_service/core/feature/uifirst/rs_sub_thread.h` | 单个子线程封装（含共享 GPU 上下文） |
| RsSubThreadCache | `rosen/modules/render_service/core/feature/uifirst/rs_sub_thread_cache.h` | 子线程缓存管理（DrawableV2 命名空间） |
| RSUifirstFrameRateControl | `rosen/modules/render_service/core/feature/uifirst/rs_uifirst_frame_rate_control.h` | UIFirst 帧率控制 |
| RSDrawWindowCache | `rosen/modules/render_service/core/feature/uifirst/rs_draw_window_cache.h` | 首帧窗口内容离屏缓存 |
| RSUIFirstRenderParams / RSUIFirstNodeState | `rosen/modules/render_service_base/include/feature/uifirst/rs_uifirst_params.h` | 节点 UIFirst 参数与状态 |
| RSFrameControlTool | `rosen/modules/render_service_base/include/feature/uifirst/rs_frame_control.h` | 正在刷新窗口标识，配合帧率控制 |
| RSDrawableUpdater | `rosen/modules/render_service_base/include/feature/uifirst/rs_drawable_updater.h` | Save/Restore Drawable 更新工具 |
| 各枚举 | `rosen/modules/render_service_base/include/common/rs_common_def.h` | MultiThreadCacheType、CacheProcessStatus、UiFirstCcmType 等 |

## 核心模型

### 1. UIFirst 管理器（RSUifirstManager）

单例，管理 UIFirst 完整生命周期。核心数据流：

1. **收集**（主线程）：`UpdateUifirstNodes` 判断 SurfaceNode 是否满足子线程渲染条件，
   命中 `IsLeashWindowCache` / `IsNonFocusWindowCache` / `IsArkTsCardCache` 之一。
2. **投递**（渲染线程）：`PostUifirstSubTasks` 先 `PurgePendingPostNodes` 剔除不需要的节点，
   再 `SortSubThreadNodesPriority` 优先级排序，逐节点 `PostSubTask`。
3. **执行**（子线程）：`RSSubThreadManager::ScheduleRenderNodeDrawable` 调度到负载最低的子线程执行。
4. **完成**（渲染线程）：`ProcessSubDoneNode` 处理子线程完成回调（`AddProcessDoneNode`）。
5. **跳过同步**（渲染线程）：`CollectSkipSyncNode` 判断节点在 OnSync 阶段是否跳过。

关键集合（成员均见 `rs_uifirst_manager.h`）：

- `pendingPostNodes_` / `pendingPostCardNodes_`：主线程收集、渲染线程消费的待提交节点。
- `pendingResetNodes_`：退出 UIFirst 后待重置的节点。
- `subthreadProcessingNode_`：正在子线程处理的节点（Drawable 引用）。
- `subthreadProcessDoneNode_` / `subthreadProcessSkippedNode_` / `purgedNode_`：子线程、渲染线程通过锁交换的结果。
- `uifirstCacheState_`（NodeId → CacheProcessStatus）：渲染线程 OnSync 期间记录的 UIFirst 根节点缓存状态。

### 2. 子线程管理器（RSSubThreadManager）

单例，管理多个 `RSSubThread`，核心功能：

- `Start`：启动子线程池，传入共享 `RenderContext`。
- `ScheduleRenderNodeDrawable`：按负载均衡（`minLoadThreadIndex_`）调度节点渲染。
- `WaitNodeTask` / `NodeTaskNotify`：按 nodeId 等待/通知节点任务完成（`nodeTaskState_` + `cvParallelRender_`）。
- `ScheduleReleaseCacheSurfaceOnly`：提交缓存释放任务。
- `TryReleaseTextureForIdleThread` / `ForceReleaseResource`：纹理/资源释放。
- `GetGPUCacheManagerFunc`：GPU 缓存管理器回调注入（避免单例耦合）。

### 3. 子线程（RSSubThread）

每个子线程持有共享 EGL/Vulkan 上下文：

- `PostTask` / `PostSyncTask`：异步/同步投递任务。
- `DrawableCache`：子线程渲染入口，先 `CheckValid`（必要时 `CreateShareGrContext`），再调用 `DrawableCacheWithSkImage`。
- `DrawableCacheWithSkImage`：执行离屏渲染并生成缓存纹理。
- `ReleaseCacheSurfaceOnly` / `AddToReleaseQueue` / `ResetGrContext`：Surface 与 GPU 资源管理。
- `doingCacheProcessNum_`（atomic）：当前处理中的缓存任务数。

### 4. 子线程缓存（RsSubThreadCache，DrawableV2）

管理 UIFirst 缓存 Surface 的生命周期。核心结构：

- `UIFirstParams`：`submittedSubThreadIndex_`、`cacheProcessStatus_`（atomic）、`isNeedSubmitSubThread_`。
- `CacheSurfaceInfo`：已处理 Surface/节点数、alpha、是否含阴影、颜色空间、`opaqueRegion`、`absDrawRect`、`vsyncId` 等。
- 双缓冲：`cacheSurface_`（当前）与 `cacheCompletedSurface_`（完成），`UpdateCompletedCacheSurface` 做 swap。

关键接口：

- 生命周期：`InitCacheSurface`、`UpdateCompletedCacheSurface`、`ClearCacheSurfaceInThread`、`ClearCacheSurfaceOnly`、`HasCachedTexture`。
- 绘制：`DealWithUIFirstCache`（总入口）、`DrawUIFirstCache*WithStarting`、`DrawCacheSurface`、`SubDraw`（子线程核心绘制）。
- 脏区优化（仅 MULTI 模式 + `GetUIFirstDirtyEnabled`）：`UpdateUifirstDirtyManager`、`MergeUifirstAllSurfaceDirtyRegion`、`UifirstDirtyRegionDfx`。
- HDR：`SetTargetColorGamut`、`SetHDRPresent`。
- 遮挡：`SetCacheBehindWindowData`、`DrawBehindWindowBeforeCache`。

### 5. 帧率控制（RSUifirstFrameRateControl）

单例，控制子线程渲染频率，支持场景见 `SceneId` 枚举（图标启动、Dock 启动、滑动回桌面、最近任务、清除最近任务、AOD 唤醒、锁屏解锁等）。

`SubThreadFrameDropDecision` 跳帧决策：

```
inAnimation = JudgeStartAnimation() || JudgeStopAnimation() || JudgeMultiTaskAnimation()
hasMultipleSubSurfaces = node.GetChildren()->size() > 1
canDropFrame   = GetUifirstFrameDropInterval(frameInterval)      // callCount % (interval+1) != 0
isNeedFrameControl = !RSFrameControlTool::Instance().CheckAppWindowNodeId(node.GetFirstLevelNodeId())

return inAnimation && ((forceRefreshOnce_ && isNeedFrameControl)
                    || (!hasMultipleSubSurfaces && canDropFrame))
```

### 6. 首帧窗口缓存（RSDrawWindowCache）

在渲染线程首帧把窗口内容离屏绘制并缓存为 `Drawing::Image`，之后复用：

- `DrawAndCacheWindowContent`：首帧绘制并缓存窗口内容。
- `DealWithCachedWindow`：使用已缓存内容绘制（首帧 RT 缓存复用）。
- `ClearCache` / `HasCache`：缓存清理/判断。
- `DrawCache`（static）：把缓存 Image 绘制到目标 Canvas。

### 7. 处理状态检查辅助（RSUiFirstProcessStateCheckerHelper）

用于子线程绘制时检查和等待 UIFirst 处理状态，避免前一级节点未完成造成数据竞争：

- 构造时记录当前线程的 `curFirstLevelNodeId_` 与 `curUifirstRootNodeId_`（均为 `thread_local`），析构时清除。
- `CheckMatchAndWaitNotify`：检查匹配或等待前一级节点完成通知（超时 `TIME_OUT`，默认 500ms）。
- `NotifyAll`：状态变为 DONE/SKIPPED 时通知所有等待者（`notifyCv_`）。

### 8. 帧控制工具（RSFrameControlTool）

标识正在刷新的应用窗口，配合帧率控制避免对该窗口跳帧：

- `SetAppWindowNodeId` / `CheckAppWindowNodeId`：设置/检查正在刷新窗口节点 ID。

## 关键数据结构

### MultiThreadCacheType

```cpp
enum class MultiThreadCacheType : uint8_t {
    NONE = 0,            // 未启用 UIFirst
    LEASH_WINDOW,        // Leash 窗口缓存
    ARKTS_CARD,          // ArkTS 卡片缓存
    NONFOCUS_WINDOW,     // 非焦点窗口缓存
};
```

### CacheProcessStatus

```cpp
enum class CacheProcessStatus : uint8_t {
    WAITING = 0,   // 等待子线程处理
    DOING,         // 子线程处理中
    DONE,          // 子线程处理完成
    SKIPPED,       // 本帧跳过处理，等新数据
    UNKNOWN,       // 未知/初始状态
};
```

### UiFirstCcmType / UiFirstModeType

```cpp
enum class UiFirstCcmType : uint8_t {
    SINGLE = 1,   // 单窗口
    MULTI = 2,    // 多窗口
    HYBRID = 3,   // 混合
};

enum class UiFirstModeType : uint8_t {
    SINGLE_WINDOW_MODE,  // 单窗口模式（手机）
    MULTI_WINDOW_MODE,   // 多窗口模式（PC/平板）
};
```

`GetUiFirstMode()` 映射：`SINGLE → SINGLE_WINDOW_MODE`；`MULTI → MULTI_WINDOW_MODE`；
`HYBRID →` 依 `isFreeMultiWindowEnabled_` 决定。

### RSUIFirstSwitch

```cpp
enum class RSUIFirstSwitch {
    NONE = 0,                  // 遵循 RS 规则
    MODAL_WINDOW_CLOSE = 1,    // 模态窗口动画时关闭 UIFirst
    FORCE_DISABLE = 2,         // 强制关闭 UIFirst
    FORCE_ENABLE = 3,          // 强制开启 UIFirst
    FORCE_ENABLE_LIMIT = 4,    // 强制开启但有限制
    FORCE_DISABLE_NONFOCUS = 5, // 仅在非焦点窗口强制关闭
    FORCE_DISABLE_CARD = 6,     // 在卡片上强制关闭
};
```

### RSUIFirstRenderParams

存储在节点中供子线程渲染使用（按内存对齐排列，见 `rs_uifirst_params.h` 注释）：

```cpp
struct RSUIFirstRenderParams {
    RectI childrenDirtyRect;                          // 子节点合并脏区矩形
    RectI visibleFilterRect;                          // 可见滤镜区域矩形
    NodeId startingWindowId = INVALID_NODEID;         // 启动窗口节点 ID
    Vector2f lastCacheSize = {0.f, 0.f};             // 上帧缓存尺寸
    MultiThreadCacheType cacheType = MultiThreadCacheType::NONE; // 当前缓存类型
    Gravity frameGravity = Gravity::TOP_LEFT;         // 首帧 Gravity 对齐
    bool parentEnabled = false;                       // 父节点是否启用 UIFirst
    bool leashAllEnabled = false;                     // LeashAll 模式（含阴影）
    bool isPartialSynced = false;                     // 是否已执行部分同步
};
```

### RSUIFirstNodeState

跟踪节点的 UIFirst 启用/禁用状态：

```cpp
struct RSUIFirstNodeState {
    int64_t uifirstStartTime = -1;                    // UIFirst 启用时间戳
    MultiThreadCacheType lastFrameCacheType = MultiThreadCacheType::NONE; // 上帧缓存类型
    RSUIFirstSwitch switchMode = RSUIFirstSwitch::NONE; // 外部控制开关模式
    std::atomic<bool> isWaitFirstFrame {false};       // 是否等待 UIFirst 首帧完成
    bool isEnabled = false;                           // UIFirst 是否启用
    bool hasAppWindow = false;                        // 是否包含应用窗口
    bool forceUIFirst = false;                        // 是否强制启用
    bool forceStateChanged = false;                   // 强制状态是否变更
    bool forceUpdate = false;                         // 是否需要强制更新
    bool isForceMarked = false;                       // 是否被强制标记（旧分支，将废弃）
    bool forceDrawWithSkipped = false;                // 被跳过后是否强制重绘
    bool contentDirty = false;                        // 内容是否有脏区
    bool selfAndParentShouldPaint = true;             // 自身和父节点是否需要绘制
    bool needSync = false;                            // 是否需要同步
    bool skipPartialSync = false;                     // 是否跳过部分同步
    bool isTargetDfxEnabled = false;                  // 目标节点 DFX 是否启用
    bool subThreadAssignable = false;                 // 是否可分配到子线程
};
```

### EventInfo

用于追踪场景动画的起止时间：

```cpp
struct EventInfo {
    int64_t startTime = 0;        // 事件开始时间（ms）
    int64_t stopTime = 0;         // 事件结束时间（ms，0 表示未结束）
    int64_t uniqueId = 0;         // 事件唯一 ID
    int32_t appPid = -1;          // 应用进程 PID
    std::string sceneId;          // 场景 ID（如 APP_LIST_FLING）
    std::set<NodeId> disableNodes;// 该事件中被禁用的节点集合
};
```

## 核心工作流程

### 1. 节点启用 UIFirst（UpdateUifirstNodes）

```
UpdateUifirstNodes(node, ancestorHasAnimation)
│
├─ 1. ForceUpdateUifirstNodes(node)
│     优先处理强制开关：总开关关闭 / 不支持 / FORCE_DISABLE / 有保护层(Protected) / PID 重建
│     → 强制 NONE；FORCE_ENABLE 且为 Leash 窗口 → 按模式设 LEASH/NONFOCUS
│
├─ 2. IsLeashWindowCache(node, animation)
│     仅 SINGLE_WINDOW_MODE；非一级节点/Tree 重建/卡片白名单/有动画窗口直接返回 false
│     Leash 窗口 + 无透明/滤镜 + IsCacheSizeValid
│     动画驱动（recent 场景需含主窗口的缩放），旋转/截图旋转/SCB/自绘制窗口排除
│
├─ 3. IsNonFocusWindowCache(node, animation)
│     仅 MULTI_WINDOW_MODE；超窗口数阈值/焦点/模态动画/共享转场节点排除
│     最终 QuerySubAssignable（透明+滤镜、旋转、保护层）
│
├─ 4. IsArkTsCardCache(node, animation)
│     仅 SINGLE_WINDOW_MODE + 卡片开关；ABILITY_COMPONENT_NODE +
│     ARKTSCARDNODE_NAME + 卡片白名单(桌面/负一屏) + 非 FORCE_DISABLE_CARD
│
├─ 5. 满足 → ProcessFirstFrameCache(node, cacheType)
│       首帧：UifirstStateChange(NONE) + 等待首帧缓存
│       非首帧：UifirstStateChange(node, cacheType) → AddPendingPostNode
│
└─ 6. 不满足 → UifirstStateChange(node, NONE) → AddPendingResetNode
```

`UifirstStateChange` 内部做 disable→enable / enable→disable / keep 的状态迁移，并维护
`uifirstStartTime`、`subThreadAssignable`、窗口计数与 `pendingResetNodes_`。

### 2. 子线程任务提交（PostUifirstSubTasks）

```
PostUifirstSubTasks()  [渲染线程]
│
├─ 1. PurgePendingPostNodes()
│     DoPurgePendingPostNodes：按遮挡/静态内容/帧率控制/背窗剔除节点
│     ShouldAutoCleanCache：缓存复用达阈值时标记清理
│
├─ 2. SortSubThreadNodesPriority()
│     SetNodePriority：焦点最高优先，其次 video/高/低优先级
├─ 3. MarkPostNodesPriority()
├─ 4. 遍历 sortedSubThreadNodeIds_ → PostSubTask(id)
│       → RSSubThreadManager::ScheduleRenderNodeDrawable
│       → RSSubThread::DrawableCache
├─ 5. UifirstCurStateClear()：清空当前帧缓存状态
└─ 6. 无任务时 ClearSubthreadRes()：连续 CLEAR_RES_THRESHOLD 帧无节点释放子线程资源
```

### 3. 子线程渲染（RSSubThread::DrawableCache）

```
DrawableCache(nodeDrawable)
│
├─ 1. CheckValid()：校验/创建共享 GrContext，校验 drawable
├─ 2. 帧号匹配：已有缓存纹理且任务帧号 != 当前帧 → SKIPPED + AddProcessSkippedNode + 请求下一 VSync
├─ 3. RSUiFirstProcessStateCheckerHelper 记录状态，cacheProcessStatus → DOING
├─ 4. DrawableCacheWithSkImage()
│       InitCacheSurface → RSPaintFilterCanvas → SubDraw()
│         └ SubDraw：合并脏区 / 裁剪 / DrawAllUifirst 或 DrawUifirstContentChildren
│       FlushAndSubmit → UpdateCacheSurfaceInfo → UpdateBackendTexture
│       → SetCacheBehindWindowData
├─ 5. cacheProcessStatus → DONE，SetCacheSurfaceNeedUpdated(true)，ResetSurfaceSkipCount
└─ 6. NodeTaskNotify(nodeId) + RequestNextVSync + AddProcessDoneNode(nodeId)
```

### 4. 缓存完成处理（ProcessSubDoneNode）

```
ProcessSubDoneNode()  [渲染线程]
│
├─ 1. ProcessSubThreadSkippedNode()：尝试重调被跳过的节点
├─ 2. ProcessPurgedNode()：尝试重调被剔除节点
├─ 3. ProcessDoneNode() — ProcessDoneNodeInner()
│       UpdateCompletedCacheSurface()（swap 完成缓存）
│       RenderGroupUpdate() / NotifyUIStartingWindow / erase subthreadProcessingNode_
│       处理 captured、pendingReset、过期 processingNode（超时加入跳过）
├─ 4. UpdateSkipSyncNode()：更新 跳过/部分同步 集合
└─ 5. RestoreSkipSyncNode()：恢复不再需要跳过的节点
```

### 5. 缓存使用（渲染线程合成）

```
DealWithUIFirstCache()
│
├─ DealWithCachedWindow()：首帧 RT 缓存复用
├─ 应用 Alpha / Matrix / 全局定位偏移
└─ 选择绘制:
     ├─ DrawUIFirstCacheWithStarting()：有启动窗口
     └─ DrawUIFirstCache()—DrawCacheSurface()
          ├─ HasCachedTexture → DrawCacheSurface（Drawing::Image）
          ├─ !HasCachedTexture && canSkipWait → 不绘制（用旧缓存）
          └─ !HasCachedTexture && !canSkipWait → WaitNodeTask + UpdateCompletedCacheSurface
```

## 线程模型与同步机制

| 线程 | 职责 | 关键操作 |
| --- | --- | --- |
| Main Thread | 遍历节点树，判断并收集节点 | `UpdateUifirstNodes`、`AddPendingPostNode`、`ProcessForceUpdateNode` |
| Render Thread | 合成帧、提交与消费子线程任务 | `PostUifirstSubTasks`、`ProcessSubDoneNode`、`DealWithUIFirstCache` |
| Sub Thread | 离屏渲染生成缓存纹理 | `DrawableCache`、`DrawableCacheWithSkImage`、`SubDraw` |

同步要点：

- `childrenDrawableMutex_` 保护 `subthreadProcessDoneNode_`；`skippedNodeMutex_` 保护
  `subthreadProcessSkippedNode_`；`purgedNodeMutex_` 保护 `purgedNode_`。
- `globalFrameEventMutex_` 保护 `globalFrameEvent_`。
- `completeResourceMutex_`（recursive）保护完整缓存资源。
- `RSSubThreadManager` 用 `nodeTaskState_` + `cvParallelRender_` 实现节点任务等待/通知。
- `RSUiFirstProcessStateCheckerHelper` 用 `notifyMutex_` + `notifyCv_` 做前一级节点等待（超时 `TIME_OUT`）。
- 大量 `std::atomic`（`cacheProcessStatus_`、`isNeedSubmitSubThread_`、`isTextureValid_`、各场景标志、动画状态、`callCount_` 等）实现无锁状态同步。

### SkipSync 机制

节点在子线程渲染期间，主线程 OnSync 可能造成数据竞争，需跳过：

```
CollectSkipSyncNode(node)
│  1. 在 pendingPostNodes_/pendingPostCardNodes_ → 标记 needSync
│  2. CollectSkipSyncNodeWithDrawableState()
│     - 根缓存状态 DOING/WAITING → skip；DONE/UNKNOWN/SKIPPED → 不跳过
│     - 前一级节点正在处理 → 延迟
│  3. 卡片白名单 + processingCardNodeSkipSync_
│  4. processingNodePartialSync_（Leash 部分同步）
│  5. processingNodeSkipSync_（子 Surface 完全跳过）
└─ 都不命中 → 不跳过
```

## 缓存管理机制

### 缓存初始化（InitCacheSurface）

- 清理旧缓存 Surface（`clearCacheSurfaceFunc_`）。
- 尺寸：LeashAll 取 `localDrawRect`，否则取缓存尺寸。
- 后端：OpenGL `MakeRenderTarget`；Vulkan `MakeFromBackendTexture`（含 FP16/HDR/颜色空间）；
  Raster `MakeRasterN32Premul`。

### 缓存更新（UpdateCompletedCacheSurface）

`completeResourceMutex_` 保护下 swap 当前与完成缓存（Surface、线程索引、Info、BehindWindowData、Rect、BackendTexture、CleanupHelper、isCacheValid_），并 `isTextureValid_ = true`、清 `needUpdated`。

### 缓存清理（ClearCacheSurfaceInThread / ClearCacheSurfaceOnly）

`ClearCacheSurfaceInThread()` 调用清理回调释放 Surface 后重置；`ClearCacheSurfaceOnly()` 仅清当前缓存、保留完成缓存。

### 自动清理（ShouldAutoCleanCache）

仅 `MULTI_WINDOW_MODE` 且 `clearCacheThreshold_ > 0` 且系统属性开启时生效；缓存复用次数达阈值后 `AddMarkedClearCacheNode`，由 `ProcessMarkedNodeSubThreadCache` 统一清理。

### 脏区优化（Dirty Region）

仅 `IsUIFirstDirtyEnabled()`（MULTI 模式 + 系统属性）时：

- `UpdateUifirstDirtyManager`：合并全局脏区、自身脏区、所有子 Surface 脏区。
- `SubDraw` 中 `MergeUifirstAllSurfaceDirtyRegion` 合并后 `ClipRegion` 裁剪，只重绘变化区域。

## 帧率控制策略

### 子线程帧率控制（SubThreadControlFrameRate）

```
需满足：GetSubThreadControlFrameRate() 开关
      + HasCachedTexture() && alpha != 0
      + processedNodes > SUBTHREAD_CONTROL_FRAMERATE_NODE_LIMIT
      + 且为 LEASH_WINDOW 类型
      + RSUifirstFrameRateControl::NeedRSUifirstControlFrameDrop(node)
      + 不在 subthreadProcessingNode_ && !IsSubThreadSkip()
```

### 遮挡降频（Behind Window）

仅 `MULTI_WINDOW_MODE` + `GetUIFirstBehindWindowEnabled()`：

```
NeedPurgeByBehindWindow：purgeEnable_ && hasTexture
    && lastFrameCacheType == NONFOCUS_WINDOW && IsBehindWindowOcclusion(node)

HandlePurgeBehindWindow：距上次剔除 >= PURGE_BEHIND_WINDOW_TIME 或首次 → 不剔除并更新时间，否则剔除
PURGE_BEHIND_WINDOW_TIME = BEHIND_WINDOW_RELEASE_TIME(33) - BEHIND_WINDOW_TIME_THRESHOLD(3) = 30ms
```

## 场景适配与白名单

### 强制转子线程的动画（toSubByAppAnimation_）

```cpp
const std::vector<std::string> toSubByAppAnimation_ = {
    { "WINDOW_TITLE_BAR_MINIMIZED" },   // 窗口标题栏最小化
    { "LAUNCHER_APP_LAUNCH_FROM_DOCK" },// 从 Dock 启动应用
};
```

### 禁止场景（LAYER_PART_RENDER_DISABLE_ANIMATION）

```cpp
const std::unordered_set<std::string_view> LAYER_PART_RENDER_DISABLE_ANIMATION = {
    "APP_LIST_FLING",      // 应用列表滑动
    "WEB_LIST_FLING",      // Web 列表滑动
    "SCROLLER_ANIMATION",  // 滚动动画
};

// 另一个禁止场景（CheckIfAppWindowHasAnimation 中使用）
constexpr std::string_view ABILITY_OR_PAGE_SWITCH = "ABILITY_OR_PAGE_SWITCH";
```

### 特殊场景标志

```cpp
std::atomic<bool> isRecentTaskScene_;      // 最近任务（手机）
std::atomic<bool> isMissionCenterScene_;   // 任务中心（PC）
std::atomic<bool> isSplitScreenScene_;     // 分屏（PC）
std::atomic<bool> isSnapshotRotationScene_;// 快照旋转（平板）
```

由 `OnProcessAnimateScene(SystemAnimatedScenes)` 切换：
`ENTER_RECENTS`/`EXIT_RECENTS`、`ENTER_MISSION_CENTER`/`EXIT_MISSION_CENTER`、
`ENTER_SPLIT_SCREEN`/`EXIT_SPLIT_SCREEN`、`SNAPSHOT_ROTATION`、`OTHERS`（重置 PC 场景标志）。

## 配置参数

### 核心成员（rs_uifirst_manager.h）

| 参数 | 类型 | 默认值 | 说明 |
| --- | --- | --- | --- |
| `isUiFirstOn_` | bool | false | UIFirst 总开关 |
| `isUiFirstSupportFlag_` | bool | false | UIFirst 支持标志（`GetUiFirstSwitch` 为二者与） |
| `isCardUiFirstOn_` | bool | false | ArkTS 卡片 UIFirst 开关 |
| `uifirstType_` | UiFirstCcmType | SINGLE | 1=SINGLE, 2=MULTI, 3=HYBRID |
| `isFreeMultiWindowEnabled_` | bool | false | 自由多窗口开关（影响 HYBRID 映射） |
| `uifirstWindowsNumThreshold_` | int | 0 | 窗口数阈值（0 不限制） |
| `clearCacheThreshold_` | int | 0 | 缓存复用清理阈值（仅 MULTI_WINDOW_MODE） |
| `sizeChangedThreshold_` | float | 0.1f | 尺寸变化阈值 |
| `purgeEnable_` | bool | false | 剔除节点开关 |
| `isUIFirstLeashAllEnable_` | bool | false | 所有 Leash 窗口启用 UIFirst（含阴影） |
| `allScreenPowerOffNeedPurge_` | bool | false | 所有屏幕关闭时剔除 |
| `curUifirstWindowNums_` | int | 0 | 当前 UIFirst 窗口数 |

### 读取来源（ReadUIFirstCcmParam / RefreshUIFirstParam）

- `ReadUIFirstCcmParam`：从 `UIFirstParam`（XML 配置）读取开关、类型、窗口阈值、清理阈值、尺寸阈值、LeashAll；模拟器（`ROSEN_EMULATOR`）强制关闭。
- `RefreshUIFirstParam`：读取 `purgeEnable_`、`isUiFirstSupportFlag_`、`allScreenPowerOffNeedPurge_`。

### 系统属性（运行时）

| 属性访问器 | 说明 |
| --- | --- |
| `RSSystemProperties::GetSubThreadControlFrameRate()` | 子线程帧率控制开关 |
| `RSSystemProperties::GetSubThreadDropFrameInterval()` | 子线程跳帧间隔 |
| `RSSystemProperties::GetUIFirstBehindWindowEnabled()` | 背窗遮挡帧率控制开关 |
| `RSSystemProperties::GetUIFirstBehindWindowFilterEnabled()` | 后台窗口滤镜开关 |
| `RSSystemProperties::GetUIFirstAutoClearCacheEnabled()` | 自动清理缓存开关 |
| `RSSystemProperties::GetUIFirstDirtyEnabled()` | 脏区优化开关 |
| `RSSystemProperties::GetUIFirstDirtyDebugEnabled()` | 脏区 DFX 开关 |
| `RSSystemProperties::GetUIFirstPurgeEnabled()` | 剔除节点开关 |
| `RSSystemProperties::GetCacheOptimizeRotateEnable()` | 旋转缓存优化开关 |
| `RSSystemParameters::GetUIFirstStartingWindowCacheEnabled()` | 启动窗口缓存开关 |
| `RSSystemParameters::IsNeedScRGBForP3()` | ScRGB for P3 颜色空间开关 |

### 时间/阈值常量

| 常量 | 值 | 说明 |
| --- | --- | --- |
| `CLEAR_RES_THRESHOLD` | 3 | 连续无 UIFirst 节点后清理资源帧数阈值 |
| `BEHIND_WINDOW_RELEASE_TIME` | 33 ms | 背窗最短帧时间（约 30fps） |
| `BEHIND_WINDOW_TIME_THRESHOLD` | 3 ms | 背窗最小帧丢时间 |
| `PURGE_BEHIND_WINDOW_TIME` | 30 ms (33-3) | 背窗剔除时间间隔 |
| `UIFIRST_TASKSKIP_PRIO_THRESHOLD` | 3 | 任务跳过优先级阈值 |
| `SUBTHREAD_CONTROL_FRAMERATE_NODE_LIMIT` | 5 | 帧率控制节点数限制 |
| `TIME_OUT`（CheckMatchAndWaitNotify） | 500 ms | 状态检查等待超时 |

## DFX 与调试

### Trace 打点

```cpp
RS_TRACE_NAME_FMT("PostUifirstSubTasks %zu", sortedSubThreadNodeIds_.size());
RS_TRACE_NAME_FMT("post UpdateCacheSurface %" PRIu64, id);
RS_OPTIONAL_TRACE_NAME_FMT("sub done %" PRIu64"", id);
RS_TRACE_NAME_FMT("sub skipped %" PRIu64, id);
RS_TRACE_NAME_FMT("RSSubThread::DrawableCache [%s] id:[%" PRIu64 "]", ...);
RS_TRACE_NAME_FMT("UIFirst_switch disable -> enable %" PRIu64, node.GetId());
```

### 日志输出

```cpp
RS_LOGI("uifirst disable -> enable. %{public}s id:%{public}" PRIu64, ...);
RS_LOGI("uifirst enable -> disable. %{public}s id:%{public}" PRIu64, ...);
RS_LOGI("ReadUIFirstCcmParam isUiFirstOn=%{public}d isCardUiFirstOn=%{public}d ...");
RS_LOGW("[%{public}s] cachedSize invalid ...");
```

### 内存调试

```cpp
void RSSubThread::DumpMem(DfxString& log, bool isLite = false);
void RSSubThread::DumpGpuMem(DfxString& log, const std::vector<std::pair<NodeId, std::string>>& nodeTags);
float RSSubThread::GetAppGpuMemoryInMB();
void RSSubThreadManager::DumpMem(DfxString& log, bool isLite = false);
void RSSubThreadManager::DumpGpuMem(DfxString& log, const std::vector<std::pair<NodeId, std::string>>& nodeTags);
float RSSubThreadManager::GetAppGpuMemoryInMB();
```

### 缓存图像 Dump

```cpp
RSBaseRenderUtil::WriteCacheImageRenderNodeToPng(cacheSurface, nodeDrawable->GetName());
RSBaseRenderUtil::WriteCacheImageRenderNodeToPng(cacheImage, "cacheImage");
```

## 测试锚点

- `rosen/test/render_service/render_service/unittest/feature/uifirst/`：
  `rs_uifirst_manager_test.cpp`、`rs_uifirst_manager_test2.cpp`、`rs_uifirst_frame_rate_control_test.cpp`、
  `rs_sub_thread_test.cpp`、`rs_sub_thread_manager_test.cpp`、`rs_sub_thread_cache_test.cpp`、
  `rs_draw_window_cache_test.cpp`。
- `rosen/test/render_service/render_service_base/unittest/feature/uifirst/`：`rs_frame_control_test.cpp`。