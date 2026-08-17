# 服务端 RenderNode

## 适用范围

- RSRenderNode 及其派生类的核心数据模型与生命周期
- 服务端节点树构建、脏区域管理和遍历
- ModifierNG 在服务端的应用流程
- RenderParams 参数管理
- 动画管理器 RSAnimationManager 在节点上的挂载
- 节点缓存（OPINC、RenderGroup、LayerCache）

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| RenderNode 基类 | `rosen/modules/render_service_base/include/pipeline/rs_render_node.h` | RSRenderNode：核心节点类 |
| RenderNode 实现 | `rosen/modules/render_service_base/src/pipeline/rs_render_node.cpp` | 节点逻辑实现 |
| SurfaceRenderNode | `rosen/modules/render_service_base/include/pipeline/rs_surface_render_node.h` | RSSurfaceRenderNode：Surface 节点，Buffer 管理 |
| ScreenRenderNode | `rosen/modules/render_service_base/include/pipeline/rs_screen_render_node.h` | RSScreenRenderNode：屏幕节点 |
| LogicalDisplayRenderNode | `rosen/modules/render_service_base/include/pipeline/rs_logical_display_render_node.h` | RSLogicalDisplayRenderNode：逻辑显示节点 |
| RootRenderNode | `rosen/modules/render_service_base/include/pipeline/rs_root_render_node.h` | RSRootRenderNode：根节点 |
| EffectRenderNode | `rosen/modules/render_service_base/include/pipeline/rs_effect_render_node.h` | RSEffectRenderNode：效果节点 |
| CanvasRenderNode | `rosen/modules/render_service_base/include/pipeline/rs_canvas_render_node.h` | RSCanvasRenderNode：画布节点 |
| 渲染参数 | `rosen/modules/render_service_base/include/params/rs_render_params.h` | RSRenderParams：节点渲染参数 |
| 脏区域管理 | `rosen/modules/render_service_base/include/pipeline/rs_dirty_region_manager.h` | RSDirtyRegionManager |
| 动画管理 | `rosen/modules/render_service_base/include/animation/rs_animation_manager.h` | RSAnimationManager |
| ModifierNG 类型 | `rosen/modules/render_service_base/include/modifier_ng/rs_modifier_ng_type.h` | RSModifierType / RSPropertyType 枚举 |

## 核心模型

### 节点继承体系

```
RSRenderNode (enable_shared_from_this)
  ├── RSSurfaceRenderNode        -- 窗口/图层，同时继承 RSSurfaceBufferInterface，持有 Surface Handler + Buffer
  ├── RSScreenRenderNode         -- 物理屏幕，关联 HdiOutput
  ├── RSLogicalDisplayRenderNode -- 逻辑显示，支持镜像
  ├── RSEffectRenderNode         -- 效果容器，背景模糊
  ├── RSProxyRenderNode          -- 代理节点，转发操作
      └── RSCanvasRenderNode         -- 画布，持有 CmdList
          ├── RSRootRenderNode       -- 根节点（一棵渲染内容树的根，复用画布绘制能力）
          ├── RSUnionRenderNode      -- 合并绘制节点
          └── RSCanvasDrawingRenderNode -- Canvas 绘制节点
```

> 注：`RSRootRenderNode` 的 "Root" 指**一棵渲染内容树的根节点**（通常挂在一个 Surface 下），
> 并非继承体系的最顶层基类。它派生自 `RSCanvasRenderNode` 以复用画布/CmdList 绘制能力，
> 同样情况的还有 `RSUnionRenderNode`、`RSCanvasDrawingRenderNode`。

> 另有 `RSWindowKeyFrameRenderNode`（窗口关键帧）位于 `feature/window_keyframe/` 下。

`RSRenderNodeType` 枚举标识类型（RS_NODE / SURFACE_NODE / SCREEN_NODE / CANVAS_NODE / UNION_NODE 等）。

### 核心数据结构

- **ModifiersNGMap**：`std::map<ModifierNG::RSModifierType, ModifierNGContainer>`，存储所有服务端 Modifier。
- **RSRenderParams**：渲染参数快照，每帧 Prepare 时从节点属性计算。
- **RSDirtyRegionManager**：脏区域追踪，支持合并、合并历史和部分渲染。
- **CurFrameInfoDetail**：当前帧信息（prepareSeqNum、vsyncId、subTreeSkipped 等）。

### 帧处理流程

```
Prepare:
  QuickPrepare(visitor)               // 快速准备
  → PrepareSelfNodeForApplyModifiers() // 应用 Modifier 到 staging 属性
  → ApplyModifiers()                    // ModifierNG 应用

Process:
  RSRenderNodeDrawable::OnDraw(canvas) // 绘制
  → ApplyRenderParams()                // 应用渲染参数
  → 绘制背景/前景/内容
```

### ModifierNG 应用

1. 服务端 `RSRenderModifier::Apply(canvas, properties)` 将属性变更写入 `RSProperties`。
2. `ApplyLegacyProperty()` 将 Modifier 属性应用到传统 RSProperties 接口。
3. Modifier 按 RSModifierType 排序，保证几何→外观→自定义的执行顺序。

### 脏区域与缓存

- **NodeDirty** 枚举：CLEAN / DIRTY，标记节点是否有内容变更。
- **LayerDrawContent** 位集：SELF / SUBTREE / UPDATE / SUBTREE_UPDATE，用于动态 Layer 跳过优化。
- **OPINC**：`RSOpincCache` / `RSLayerPartRenderCache`，节点级渲染缓存。
- **RenderGroup**：`RSRenderGroupCacheDrawable`，子树缓存。

### 动画管理

- `RSAnimationManager` 挂载在 RSRenderNode 上，管理该节点所有活跃动画。
- 每帧 `Animate(timestamp)` 驱动动画估值，更新 RSRenderProperty。
- `RSFrameRateRange` 与 HGM 协商帧率。

### 跨屏节点

- `RSRenderNode::AddCrossParentChild()` / `AddCrossScreenChild()` 支持节点跨屏挂载。
- 跨屏克隆相关成员定义在 `RSSurfaceRenderNode` 上：`sourceCrossNode_` / `isCloneCrossNode_` 标识克隆关系，`hasVisitedCrossNode_` 防止递归遍历。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| enable_shared_from_this | `RSRenderNode : public std::enable_shared_from_this` | 树操作、回调等场景需要安全获取 shared_ptr |
| ModifiersNGMap 排序 | `std::map<ModifierNG::RSModifierType, ModifierNGContainer>` | 保证 Modifier 按类型有序应用，几何先于外观 |
| RenderParams 快照 | `RSRenderParams` 独立于 RSRenderNode | 渲染参数在帧内不变，避免读写竞争 |
| NodeDirty 双层标记 | NodeDirty + LayerDrawContent 位集 | 区分"有内容"和"有更新"，支持 Layer 跳过优化 |
| OPINC 缓存 | `RSOpincCache` / `RSLayerPartRenderCache` | 静态子树缓存，跳过无变化节点的完整绘制 |
| SingleFrameComposer | `rosen/modules/render_service_base/include/feature/single_frame_composer/rs_single_frame_composer.h` | RSSingleFrameComposer：首帧优化，在主线程提前合成 |

## 关键机制补充

### RSRenderParams Prepare 与脏区域合并

Prepare 链路：

- `RSRenderServiceVisitor::PrepareChildren` 递归遍历，每个节点 `Prepare` 先 `ApplyModifiers()` 再递归子树；快速路径 `QuickPrepare` 末尾补 `UpdateRenderParams()` + `AddToPendingSyncList()`。
- `ApplyModifiers` 按 `dirtyTypesNG_` 对每种 modifier 先 reset 再 `ApplyLegacyProperty` 写回 `RSProperties`，随后 `RSProperties::OnApplyModifiers` 做帧偏移、像素拉伸、filter 推导。
- 矩阵：`UpdateDrawRect` → `UpdateGeometryByParent` → `RSObjAbsGeometry::UpdateMatrix`，把父矩阵与自身 transform（pivot/scale/rotate/translate 等）合成出 `absMatrix_` 并 `SetAbsRect`。
- 快照同步：`UpdateRenderParams` 把几何/alpha/filter 等逐项 Set 进 `stagingRenderParams_`；帧末 `OnSync` 调 `RSRenderParams::OnSync` 把 staging 拷到 render params（matrix 走 Swap），保证渲染线程读到帧内不变的快照。

脏区域合并（`RSDirtyRegionManager`）：

- 当前帧 `MergeDirtyRect` 用 `JoinRect` 取包围并集，同时维护多矩形 advanced 列表，超过阈值回退成单矩形。
- 历史合并 `UpdateDirty` 把当前帧入环形队列（`HISTORY_QUEUE_MAX_SIZE = 10`），按 `bufferAge` 取最近 N 帧与当前帧并集，适配多 buffer backbuffer 复用。
- 对齐裁剪：`ClipDirtyRectWithinSurface` 裁到 surface，`GetPixelAlignedRect`（默认 32 像素对齐）对齐当前帧与历史以减少碎块。
- 部分渲染：`SetPartialRenderEnabled` 开关；节点经 `UpdateAbsDirtyRegion` / `UpdateDirtyRegion` 把脏区汇入 manager，Layer 级通过 render params 下发当前帧脏区。

### OPINC 缓存失效与刷新

- 状态分散在三处：`RSOpincCache`（子树标志）、`RSOpincRootCache`（稳定状态机）、`RSOpincManager`（单例总开关与内存）。
- 可生成前提：`subTreeSupportFlag` 自底向上累积（父 = 父 && 子 support && 子非 opinc 根 && 非 group）；含 filter/effect/spherize/attraction/colorBlend/useEffect 等属性的节点本帧不支持缓存。
- 失效触发：节点/子树 dirty、内容 dirty、成为 group、或 ArkUI 重新 `MarkSuggestOpincNode` 时，`RSOpincRootCache` 状态机重置为 `STATE_CHANGE`，清零 `unchangeCount_` 并设 `waitCount_` 为 60 帧。
- 生成命中：节点连续稳定进入 unchange 状态且 supportFlag 成立时置 `isOpincRootFlag_`；`RSOpincDrawCache` 走 `RECORD_NONE → CALCULATE → CACHING → CACHED`，受屏内、尺寸、单根（`OPINC_ROOT_TOTAL_MAX = 1`）和总内存（屏宽 × 高 × 2）限制。
- LayerPartRender：连续 3 帧（`MIN_UNCHANGE_COUNT`）不变进入局部稳态更新；material 节点会向上传染，策略为 `CACHE_DISABLE` 或存在 UIFirst/动画子节点时退出。

### RenderGroup 与 OPINC 的关系

- RenderGroup 由 ArkUI 标记 `NodeGroupType::GROUPED_BY_USER` 触发，以 group 根为界的整子树离屏整图（`RSRenderGroupCacheDrawable` 持有 surface/Image）。
- OPINC 是运行期自动识别稳定子树的复用机制，落在 `RSOpincDrawCache` 上，按 opList drawArea 的 unionRect 做局部命中。
- 二者**共用同一份离屏纹理**（同一个 `renderGroupCacheDrawable_` 槽位），以 `opincCachedMark` 区分内存 tag（`TAG_OPINC` / `TAG_RENDER_GROUP`）并仲裁优先级，不是两套独立缓存。
- 适用场景：RenderGroup 适合 ArkUI 显式标记、内容静态整块可复用的子树；OPINC 适合未被标记但运行期稳定可复用的节点，是自动兜底。
- 互斥：RenderGroup 节点（group > NONE）会令其子树 `subTreeSupportFlag` 失效，OPINC 不在 group 子树内单独生根；OPINC 主动放弃时回退 RenderGroup 或不缓存。

### SingleFrameComposer 流程与触发条件

- 总开关 `persist.sys.graphic.singleFrameComposer`（默认关），另设 `singleFrameComposerCanvasNode` 放行 `hwstylusfeature` 节点。
- 触发需同时满足：统一渲染、非窗口动画期、非多窗口（`GetVisibleWinCount < 2`）、目标 pid 已登记或开关打开。
- 流程：`CommitTransaction` 命中后，主线程当下即 `ProcessDataBySingleFrameComposer`，`SetSingleFrameFlag` 标记当前线程，仅处理 `CANVAS_NODE_UPDATE_RECORDING` 命令，随后 `ForceRefreshForUni` 立即刷新。
- 旁路缓存：`AddModifier` 在三条件满足时走 `SingleFrameAddModifierNG`，custom modifier 存入 `singleFrameDrawCmdModifiersNG_` 并 `SetDirty`，提前 return 不进常规 `modifiersNG_`。
- 绘制合入：渲染阶段 `ApplyDrawCmdModifier` 把单帧 Modifier 合回列表并用 `SingleFrameIsNeedSkipNG` 跳过旧 Modifier，实现“提前一帧”应用以减少首帧延迟；代价是只覆盖单窗口、非动画、custom 绘制场景。

### 节点离树资源回收链路

> 说明：`RSRenderNode` 本身没有 `CleanResources` 方法，该名字属于 `RSMainThread::CleanResources(pid)` 的进程级清理入口；单节点资源回收由离树钩子 + `RSRenderNodeGC` 桶式延迟释放串联。

- 离树：`RemoveChild` → `ResetParent` → `SetIsOnTheTree(false)` → `OnTreeStateChanged`（标记 drawable 清理、强制失效 filter 缓存、`HandleNodeRemovedFromTree` 清 cmdlist/children 列表）。
- Drawable 释放：`ReleaseNodeMem` 置 `released_`、`renderDrawable_.reset()` + `RemoveDrawableFromCache`、`drawableVec_.reset()`；`ClearDrawableVec2` 在 `OnSync` 按槽位清理并置 dirty。
- 派生类扩展：`RSSurfaceRenderNode` 离树时 `MarkNeedPurge` 驱动 GPU purge、leash 窗口清图缓存；`RSCanvasDrawingRenderNode` 在 `AfterSync` 清 `drawCmdListsNG`（cmdlist）。
- GC 回收：析构由 `RSRenderNodeGC::NodeDestructor` 入桶，VSync 间 `ReleaseNodeBucket` 分批 `delete`；后台离树 canvas 节点由 `ReleaseNodeMemNotOnTree` 在空闲时释放。
- 总链路：`RemoveChild` → `OnTreeStateChanged`/`OnResetParent`（即时清缓存/通知）→ `ReleaseNodeMem`（GC 释放 drawable/vec）→ 析构（动画回退到 fallback 根）→ 入桶延迟 `delete`。
