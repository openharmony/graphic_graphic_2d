# UIFirst / OPINC / RenderGroup

## 适用范围

- UIFirst（UI 优先渲染）子线程渲染
- OPINC（操作增量缓存）自动缓存
- RenderGroup 渲染组缓存
- 子线程管理和帧率控制
- 后窗遮挡与 UIFirst 联动
- ArkTS 卡片和 Leash 窗口缓存

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSUifirstManager | `rosen/modules/render_service/core/feature/uifirst/rs_uifirst_manager.h` | UIFirst 管理器单例 |
| RSSubThreadManager | `rosen/modules/render_service/core/feature/uifirst/rs_sub_thread_manager.h` | 子线程管理器 |
| RSSubThread | `rosen/modules/render_service/core/feature/uifirst/rs_sub_thread.h` | 子线程封装 |
| RsSubThreadCache | `rosen/modules/render_service/core/feature/uifirst/rs_sub_thread_cache.h` | 子线程缓存 |
| RSUifirstFrameRateControl | `rosen/modules/render_service/core/feature/uifirst/rs_uifirst_frame_rate_control.h` | UIFirst 帧率控制 |
| RSDrawWindowCache | `rosen/modules/render_service/core/feature/uifirst/rs_draw_window_cache.h` | 窗口绘制缓存 |
| RSOpincDrawCache | `rosen/modules/render_service/core/feature/opinc/rs_opinc_draw_cache.h` | OPINC 绘制缓存 |
| RSLayerPartDrawCache | `rosen/modules/render_service/core/feature/opinc/rs_layer_part_draw_cache.h` | OPINC 层部分绘制缓存 |
| RSRenderGroupCacheDrawable | `rosen/modules/render_service/core/feature/render_group/rs_render_group_cache_drawable.h` | 渲染组缓存 Drawable |

## 核心模型

### UIFirst 管理器

`RSUifirstManager` 为单例，管理 UIFirst 的完整生命周期。核心数据流：

1. **收集**：`UpdateUifirstNodes` 判断 SurfaceNode 是否满足子线程渲染条件
2. **投递**：`PostUifirstSubTasks` 将符合条件的节点投递到子线程
3. **执行**：`RSSubThreadManager` 调度到负载最低的子线程执行渲染
4. **完成**：`ProcessSubDoneNode` 处理子线程完成回调
5. **跳过同步**：`CollectSkipSyncNode` 判断节点在 OnSync 阶段是否跳过

关键状态：`uifirstCacheState_`（NodeId → CacheProcessStatus）记录每个节点的缓存状态。

### 子线程管理

`RSSubThreadManager` 管理多个 `RSSubThread`，核心功能：

- `Start`：启动子线程，传入 RenderContext
- `PostTask`：向指定线程投递任务
- `WaitNodeTask` / `NodeTaskNotify`：等待/通知节点任务完成
- `ScheduleRenderNodeDrawable`：调度 Drawable 渲染
- `TryReleaseTextureForIdleThread`：空闲线程释放纹理

### UIFirst 场景判定

支持多种缓存类型 `MultiThreadCacheType`：

- **非焦点窗口**：`IsNonFocusWindowCache`
- **ArkTS 卡片**：`IsArkTsCardCache`
- **Leash 窗口**：`IsLeashWindowCache`

场景识别：

- `isRecentTaskScene_`：最近任务场景
- `isMissionCenterScene_`：任务中心场景
- `isSplitScreenScene_`：分屏场景
- `isSnapshotRotationScene_`：快照旋转场景

### 后窗遮挡与 UIFirst

`pendingNodeBehindWindow_` 记录被后窗遮挡的节点。`BEHIND_WINDOW_RELEASE_TIME = 33ms` 控制释放时间，`BEHIND_WINDOW_TIME_THRESHOLD = 3ms` 为最小帧丢时间。`IsBehindWindowOcclusion` 判断节点是否被后窗遮挡。

### OPINC 绘制缓存

`RSOpincDrawCache` 实现操作增量缓存，核心逻辑：

- `OpincCalculateBefore` / `OpincCalculateAfter`：计算缓存前后处理
- `BeforeDrawCache` / `AfterDrawCache`：绘制缓存前后处理
- `DrawAutoCache`：自动缓存绘制
- `nodeCacheType_`：节点缓存类型（thread_local）
- `recordState_`：记录状态（RECORD_NONE 等）
- `isDrawAreaEnable_`：绘制区域使能状态
- `opListDrawAreas_`：OpList 绘制区域
- 缓存内存管理：`AddOpincCacheMem` / `ReduceOpincCacheMem`

### RenderGroup 缓存

`RSRenderGroupCacheDrawable` 管理渲染组缓存：

- `renderGroupCachedSurface_` / `renderGroupCachedImage_`：缓存的 Surface 和 Image
- `renderGroupCacheThreadId_`：缓存所属线程 ID
- `shouldClipHole_`：是否裁剪空洞
- `drawBlurForCache_` / `drawExcludedSubTreeForCache_`：绘制时标记（thread_local）

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| UIFirst 单例管理器 | `RSUifirstManager::Instance()` | UIFirst 涉及多线程协调，单例保证全局状态一致 |
| 子线程负载均衡 | `RSSubThreadManager` 选择 `minLoadThreadIndex_` | 多子线程场景下均匀分配渲染负载 |
| OPINC thread_local 缓存类型 | `nodeCacheType_` 为 thread_local | 避免多线程间的缓存类型竞争 |
| 后窗遮挡释放时间控制 | `BEHIND_WINDOW_RELEASE_TIME = 33ms` | 后窗遮挡节点延迟释放，避免频繁缓存重建 |
| RenderGroup Cache 线程绑定 | `renderGroupCacheThreadId_` | 缓存只能在创建线程使用，避免 GPU 资源跨线程 |
| UIFirst 事件驱动 | `OnProcessEventResponse` / `OnProcessAnimateScene` | 特定场景（如启动、滚动）触发 UIFirst，非全局开启 |

## 待补充背景

- `UiFirstCcmType::SINGLE` / `MULTI` 模式的差异和切换条件
- 子线程 RenderContext 的创建和生命周期管理
- OPINC 自动缓存命中率和内存阈值策略
- RenderGroup 缓存的触发条件和失效策略
- `RSUiFirstProcessStateCheckerHelper` 的等待/通知机制的完整时序
