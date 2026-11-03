# HWC 主线程处理

## 适用范围

- 主线程 HWC 节点收集（`CollectInfoForHardwareComposer` / `hardwareEnabledNodes_`）
- 帧级 HWC 强制禁用判定（`CheckIfHardwareForcedDisabled`）
- DirectComposition（直通）进入与退出条件
- 主线程对每个 HWC 候选节点的 srcRect / dstRect 计算
- 节点级 HWC 禁用原因（`HwcDisabledReasons` 枚举与 DFX 统计）
- 主线程与渲染线程之间的 HWC 参数同步

> 本文聚焦主线程（RSMainThread + RSUniRenderVisitor + RSUniHwcVisitor）一侧的 HWC 处理；硬件线程侧的合成细节请参考 `hwc-hardware-thread.md`，预协商策略请参考 `hwc-prevalidate.md`。

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSMainThread | `rosen/modules/render_service/core/pipeline/main_thread/rs_main_thread.h` / `.cpp` | 单例，持有 `hardwareEnabledNodes_`、`isHardwareForcedDisabled_`、`doDirectComposition_` 等状态；提供 `CollectInfoForHardwareComposer` / `CheckIfHardwareForcedDisabled` / `DoDirectComposition` / `ResetHardwareEnabledState` |
| RSUniRenderVisitor | `rosen/modules/render_service/core/pipeline/main_thread/rs_uni_render_visitor.h` / `.cpp` | 主线程节点树遍历器，持有 `hwcVisitor_`；负责 z-order、prepareClip、跨屏、HDR/DRM、TopLayer 等节点级 HWC 判定 |
| RSUniHwcVisitor | `rosen/modules/render_service/core/pipeline/hwc/rs_uni_hwc_visitor.h` / `.cpp` | HWC 专用遍历器，负责 srcRect/dstRect 计算与节点级使能判定（Alpha/Rotate/BufferSize/BackgroundAlpha/Filter/ColorPicker/NodeBelow 等） |
| RSUniHwcComputeUtil | `rosen/modules/render_service/core/feature/hwc/rs_uni_hwc_compute_util.h` / `.cpp` | HWC 矩形/变换计算工具：`UpdateHwcNodeProperty`、`UpdateRealSrcRect`、`CalcSrcRectByBufferFlip`、`GetLayerTransform` |
| RSHwcContext | `rosen/modules/render_service/core/pipeline/hwc/rs_hwc_context.h` / `.cpp` | sourceTuning / solidLayer 配置，按包名/窗口模式调整 HWC 策略 |
| RSDirectCompositionHelper | `rosen/modules/render_service/core/pipeline/hwc/rs_direct_composition_helper.h` | `isLastFrameDirectComposition_` / `lastFrameDidGpuRender_` / `consecutiveDoCompSuccessCount_` |
| HwcDisabledReasonCollection | `rosen/modules/render_service_base/include/info_collection/rs_hardware_compose_disabled_reason_collection.h` | 单例；定义 `HwcDisabledReasons` 25 个枚举值，统计每个 NodeId 的禁用原因 |
| RSRenderThreadParams | `rosen/modules/render_service_base/include/params/rs_render_thread_params.h` | 主线程 → 渲染线程参数载体，包含 `hardwareEnabledTypeDrawables_` |

## 核心模型

### 帧循环中的 HWC 挂入点

```
OnVsync → mainLoop_():
  3. ConsumeAndUpdateAllNodes
       └── ResetHardwareEnabledState(isUniRender)        // 每帧重置 HWC 状态
  5. ProcessCommand                                      // 应用层 SetHardwareEnabled 生效
  6. Animate
       └── UpdateDirectCompositionByAnimate              // 动画期间禁用 DirectComposition
  7. CollectInfoForHardwareComposer                      // ★ HWC 主入口
       ├── CheckIfHardwareForcedDisabled                 // 帧级强制禁用判定
       ├── TraverseSurfaceNodes                          // 收集 HWC 候选节点
       │     ├── IsHardwareEnabledType() 过滤
       │     ├── 检查 GetDoDirectComposition / IsOnTheTree / HDR
       │     ├── 入 hardwareEnabledNodes_ / hardwareEnabledDrwawables_
       │     └── 设置 ContentDirty / HwcDelayDirtyFlag
       └── TraverseProtectiveSolidNodes                  // 收集保护性纯色节点
  8. Render() → UniRender()
       ├── MarkHardwareForcedDisabled (if needed)
       ├── doDirectComposition_ 复核（ UIFirst / PowerOff / Dirty 等）
       ├── willGoDirectComposition 评估 → DoDirectComposition  ← 直通尝试
       └── RSUniRenderVisitor 遍历 → RSUniHwcVisitor 节点级判定
  9. UpdateRenderParams → needPostAndWait_               // doDirectComposition_ 决定是否同步渲染线程
```

### 每帧状态重置（`ResetHardwareEnabledState`）

在 `ConsumeAndUpdateAllNodes` 中调用，仅在统一渲染（`isUniRender_`）路径生效：

- `isHardwareForcedDisabled_` ← `!RSSystemProperties::GetHardwareComposerEnabled()`（全局开关）
- `directComposeHelper_.isLastFrameDirectComposition_` ← 上一帧的 `doDirectComposition_`
- `doDirectComposition_` ← `RSSystemProperties::GetDoDirectCompositionEnabled()`（若已被强制禁用则为 false）
- `isHardwareEnabledBufferUpdated_` = false
- `hasProtectedLayer_` / `hasSurfaceLockLayer_` = false
- `hardwareEnabledNodes_` / `hardwareEnabledDrwawables_` / `protectiveSolidNodes_` 等容器 clear

### HWC 候选节点筛选

`RSSurfaceRenderNode::IsHardwareEnabledType()` 决定节点是否能进入 HWC：

```cpp
return (nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE && isHardwareEnabledNode_) || IsLayerTop();
```

- `isHardwareEnabledNode_` 由应用层调用 `SetHardwareEnabled(...)` 设置；XCOMPONENT、视频、SurfaceView 等通过 `SelfDrawingNodeType` 区分。
- `IsLayerTop()` 表示顶部图层（如状态栏等顶层 Surface）。
- 只有 `IsHardwareEnabledType()` 返回 true 的节点，才会被主线程 `hardwareEnabledNodes_` 收集，并触发 `RSUniHwcVisitor::UpdateHwcNodeInfo` 计算矩形。

### HWC 节点收集（`CollectInfoForHardwareComposer`）

入口在 `rs_main_thread.cpp:2290`，仅在 `isUniRender_` 下执行。流程：

1. **预协商前置处理**：TV Overlay 显示预处理；`hasProtectedLayer_ = RSDrmUtil::IsDRMNodesOnTheTree()`。
2. **帧级强制禁用判定**：调用 `CheckIfHardwareForcedDisabled()` 写入 `isHardwareForcedDisabled_`。
3. **截图任务屏蔽**：`!pendingUiCaptureTasks_.empty()` → `doDirectComposition_ = false`。
4. **HDR / 色温状态更新**：`isHdrSwitchChanged_` / `isColorTemperatureOn_`。
5. **`TraverseSurfaceNodes` 遍历**（lambda 内逐节点）：
   - 截图窗口 / HDR 切换 → `SetContentDirty`；
   - Clone 跨屏节点 → 加入 `hardwareEnabledDrwawables_`；
   - 提交了 buffer 的节点 → 加入 `selfDrawables_`、设置硬光标信息；
   - `!GetDoDirectComposition()` → 关闭本帧 `doDirectComposition_` 并复位标志；
   - 不在树上但本帧消费了 buffer → `UpdateHardwareDisabledState(true)` + 关闭 DirectComposition；
   - 更新合成类型、HDR 状态、SDR metadata、HeteroHDR；
   - 硬件 HDR 被禁用 + 节点为 HDR 且非保护层 → 关闭 DirectComposition；
   - **`!IsHardwareEnabledType()` 直接 return**（不收集）；
   - 新挂树节点 → 把对应 appNode 加入 `context_->AddActiveNode`；
   - **正式收集**：有 buffer 的 HWC 候选节点 `hardwareEnabledNodes_.emplace_back(surfaceNode)`，同时构建 `hardwareEnabledDrwawables_`；
   - **状态过渡 dirty 处理**：
     - 强制禁用态 + (buffer 更新 / 上一帧 HWC) → ContentDirty；
     - 上一帧非 HWC + buffer 更新 → ContentDirty + 关闭 DirectComposition；非 buffer 更新走 `HwcDelayDirtyFlag`；
     - HWC → HWC 且 `HwcDelayDirtyFlag` 或旋转角度非 90 倍数 → ContentDirty + 关闭 DirectComposition；
   - `GetIntersectWithFilterNode() && buffer consumed` → ContentDirty + 关闭 DirectComposition；
   - buffer 更新 → `isHardwareEnabledBufferUpdated_ = true`。
6. **`TraverseProtectiveSolidNodes`**：收集 `protectiveSolidNodes_` / `protectiveSolidDrawables_`。

收集结果（`hardwareEnabledNodes_` 等）通过 `RSRenderThreadParams` 同步到渲染线程，并在 `DoDirectComposition` 中复用。

### Visitor 遍历阶段的 HWC 收集（`allHwcNodeAndFilterNode_`）

`RSRenderNode::allHwcNodeAndFilterNode_`（`rs_render_node.h:1383`，类型 `std::deque<WeakPtr>`）是 **节点树自底向上归并** 的 HWC 候选 + 滤镜节点列表，与主线程 `hardwareEnabledNodes_`（按 SurfaceNode 遍历顺序的扁平列表）不同，它保留了完整的 z-order，是 `RSUniHwcVisitor` 与跳过子树判定时的关键数据源。

#### 数据结构

| 类型 | 字段 | 说明 |
| --- | --- | --- |
| `std::deque<WeakPtr>` | `RSRenderNode::allHwcNodeAndFilterNode_` | 每个节点一份；按子树遍历顺序记录后代中所有 HWC 候选节点 + 滤镜节点；同时包含 `weak_from_this()` 自身 |
| `std::vector<std::weak_ptr<RSSurfaceRenderNode>>` | `RSScreenRenderNode::childHwcNodes_` | 屏节点派生列表：仅过滤出 HWC 类型，由 `UpdateChildHwcNode` 从 `allHwcNodeAndFilterNode_` 派生 |
| `std::vector<WeakPtr>` | `RSSurfaceRenderNode::childHardwareEnabledNodes_` | Surface 节点派生列表：由 `UpdateChildHardwareEnabledNode` 从 `allHwcNodeAndFilterNode_` 派生 |

访问器：`GetAllHwcNodeAndFilterNode()` / `ClearAllHwcNodeAndFilterNode()`（`rs_render_node.h:166-168`）。

#### 每帧生命周期

**清空**：`RSRenderNode::FilterRectCacheReset()` 以及 `RSUniRenderVisitor` 各 `Prepare*` 入口（`QuickPrepareScreenRenderNode` / `QuickPrepareLogicalDisplayRenderNode` / `QuickPrepareSurfaceRenderNode` / `QuickPrepareDepthRenderNode` / `QuickPrepareUnionRenderNode` / `QuickPrepareEffectRenderNode` / `QuickPrepareCanvasRenderNode` / `PrepareRootRenderNode` 等）都会先调用 `node.ClearAllHwcNodeAndFilterNode()`。

**收集**：分两条路径在 `Prepare*` 阶段自底向上填充：

1. **`CollectHwcAndFilterNodesToParent`**（`rs_uni_render_visitor.cpp:1182`）：常规路径，由 `PostPrepare`（`rs_uni_render_visitor.cpp:3837`）在节点准备好之后调用，也用于 `QuickPrepareSurfaceRenderNode` 的 QuickSkip 分支（`rs_uni_render_visitor.cpp:1279`）。
   - 若 `isBlendNeedFilter || node.IsHardwareEnabledType()`：把 `node.weak_from_this()` 插到自身列表**头部**（`begin()`），保证自身在该子树列表里最靠前；
   - 把自身列表整体合并到父节点列表：
     - `isParentPrepareInReverseOrder == true` → 父列表 `begin()` 处插入（反向遍历场景）；
     - 否则 → 父列表 `end()` 处追加。
   - 该合并策略保证兄弟子树之间的相对 z-order 与 Visitor 遍历方向一致。

2. **`CollectHwcAndFilterNodesInSkippedSubTree`**（`rs_uni_render_visitor.cpp:1168`）：**当子树被跳过** 时使用，仅把子节点列表合并到当前节点，不插入自身。在 `QuickPrepare*` 各分支判定 `isSubTreeNeedPrepare == false` 时调用（`rs_uni_render_visitor.cpp:1234 / 1411 / 1558 / 2013 / 2218 / 4261`），并在 QuickSkip 路径里再额外调用一次 `CollectHwcAndFilterNodesToParent` 把自身加入（`rs_uni_render_visitor.cpp:1278-1280`）。

**派生**：
- 屏节点：`PrepareScreenNode` 末尾 `node.UpdateChildHwcNode()`（`rs_uni_render_visitor.cpp:835`）→ 过滤出 HWC 类型写入 `childHwcNodes_`；
- Surface 节点（含 LeashWindow）：`node.UpdateChildHardwareEnabledNode()`（`rs_uni_render_visitor.cpp:1282 / 1416`）→ 写入 `childHardwareEnabledNodes_`。

**重置时机**：除每帧 `Prepare*` 入口的 `ClearAllHwcNodeAndFilterNode()` 外，`ResetChildHwcNodes()` / `ResetChildHardwareEnabledNodes()` 配合调用以清空派生列表。

#### 消费方

| 消费方 | 输入来源 | 作用 |
| --- | --- | --- |
| `RSUniHwcVisitor::UpdateHwcNodeEnableByFilterIntersection`（`rs_uni_hwc_visitor.cpp:961`） | `curScreenNode_->GetAllHwcNodeAndFilterNode()` | **反向遍历**（`rbegin→rend`）屏级列表：滤镜节点累积到 `filterNodes`，遇到 HWC 节点调用 `CheckHwcNodeFilterIntersection` 检查是否被上方滤镜覆盖；命中即 `DISABLED_BY_FLITER_RECT`；AIBar / HveBlur 走缓存优化分支 |
| `RSUniHwcVisitor::CheckHwcNodeFilterIntersection`（`rs_uni_hwc_visitor.cpp:1001`） | 上一步累积的 `filterNodes` | 把 HWC 节点的 `absRect` 与每个滤镜 rect `IntersectRect`，命中则禁用并打 `DISABLED_BY_FLITER_RECT`；与 AIBar 相交时注册到 `aibarNodes_` 并根据缓存状态决定是否禁用 |
| `RSUniHwcVisitor::UpdateHwcNodeRectInSkippedSubTree`（`rs_uni_hwc_visitor.cpp:1092`） | `rootNode.GetAllHwcNodeAndFilterNode()`（被跳过的子树根） | 即使子树整体被 QuickSkip，仍然要为其中的 HWC 节点重算 `UpdateDstRect` / `UpdateHwcNodeInfo`（matrix/clipRect），保证跳过帧里 HWC 直通的矩形正确 |
| `RSUniRenderVisitor::DisableOccludedHwcNodeInSkippedSubTree`（`rs_uni_render_visitor.cpp:3769`） | `node.GetAllHwcNodeAndFilterNode()` | 跳过子树且当前 SurfaceNode 被完全遮挡时，把列表中所有 HWC 节点强制禁用 |
| `RSUniHwcVisitor::UpdateHwcNodeEnable`（`rs_uni_hwc_visitor.cpp:560`） | `curScreenNode_->GetChildHwcNodes()`（派生列表） | 屏级 HWC 节点的统一遍历入口，串联 `UpdateHwcNodeProperty` / `UpdateHwcNodeEnableByAlpha` / `UpdateHwcNodeEnableByRotate` / `UpdateHwcNodeEnableByHwcNodeBelowSelfInApp` 等 |
| `RSUniHwcVisitor::UpdateHwcNodeEnableByNodeBelow`（`rs_uni_hwc_visitor.cpp:655`） | `surfaceNode->GetChildHardwareEnabledNodes()`（派生列表） | 在 Leash/Main 窗口内部按 App 维度做累计 dirty、透明 HWC、backgroundAlpha 等批量判定 |
| `RSUniRenderVisitor::PrepareScreenNode` 末尾 `node.UpdateChildHwcNode()` | 屏节点 `allHwcNodeAndFilterNode_` | 为后续 `UpdateHwcNodeEnable` / `UpdateHwcNodeDirtyRegionAndCreateLayer` 提供 HWC 节点视图 |
| `RSRenderNode` dump（`rs_render_node.cpp:1262`） | 自身列表 | DFX dump 输出 `allHwcAndFilterNode` 节点 ID |

#### 关键不变量

- **z-order 严格保留**：自身插头部 + 子列表按遍历方向合并到父，保证 `reverse` 遍历屏级列表时是从顶层向底层扫描；`UpdateHwcNodeEnableByFilterIntersection` 依赖此顺序才能让每个 HWC 节点正确感知「上方所有滤镜」。
- **类型混合**：列表中同时包含 HWC 候选节点（`IsHwcLayerType()` true）与滤镜节点（`IsBlendNeedFilter` true），通过 `IsHwcLayerType()` 反向区分。派生列表 `childHwcNodes_` / `childHardwareEnabledNodes_` 只保留 HWC 类型，避免下游消费方重复过滤。
- **跳过子树不丢失**：子树被 QuickSkip 时通过 `CollectHwcAndFilterNodesInSkippedSubTree` + `UpdateHwcNodeRectInSkippedSubTree` 保证矩形与使能状态与正常路径一致；仅在「子树跳过 + 当前窗口被完全遮挡」时通过 `DisableOccludedHwcNodeInSkippedSubTree` 整体禁用。
- **生命周期与 Prepare 一一对应**：每帧每个 `Prepare*` 入口都会 `ClearAllHwcNodeAndFilterNode()`，避免上一帧残留；列表只在本帧 Visitor 阶段有效，`DoDirectComposition` 与渲染线程消费的仍是 `hardwareEnabledNodes_` / `hardwareEnabledDrwawables_`。

### 帧级强制禁用判定（`CheckIfHardwareForcedDisabled`）

入口 `rs_main_thread.cpp:2492`。核心逻辑：

```
isHardwareForcedDisabled_ =
    (!hasProtectedLayer_ &&
        (isHardwareForcedDisabled_                  // 已被禁用，保持
         || isFoldScreenSwitching                   // 折叠屏切换中
         || (isMultiDisplay && (isExpandScreenOrWiredProjectionCase
                                || !enableHwcForMirrorMode))  // 多屏展开/投屏/镜像不支持
         || hasColorFilter))                        // 颜色反转/三色色弱等无障碍滤镜
    || CheckOverlayDisplayEnable();                 // TV Overlay
```

要点：
- **DRM 保护层例外**：`hasProtectedLayer_` 为 true 时**不**进入上述大多数禁用分支（保护内容必须走 HWC）。
- **镜像模式复用 DisplayNode buffer**，默认禁用 HWC，由 `GetHardwareComposerEnabledForMirrorMode()` 控制。
- **多屏 + HWC 启用**：`CheckIfHardwareForcedDisabled` 末尾如果 `isMultiDisplay && !isHardwareForcedDisabled_`，单独把 `doDirectComposition_` 置 false（HWC 可走硬件线程，但跳过直通合成）。

### 节点级 HWC 使能链（`RSUniHwcVisitor::UpdateHwcNodeInfo`）

每个 HWC 候选节点进入 `UpdateHwcNodeInfo`（`rs_uni_hwc_visitor.cpp:1385`）后，按顺序执行：

1. **复位 + 基础参数检查**：`SetHardwareForcedDisabledState(false)`；若不满足动态使能、屏不可见、无 buffer、扩展屏禁用等任一条件（且非 PROTECTED layer），则 `DISABLED_BY_INVALID_PARAM`。
2. **`UpdateSrcRect`**：buffer/consumer transform + Gravity（RESIZE）+ totalMatrix 逆变换。
3. **`UpdateHwcNodeEnableByGlobalPosition`**。
4. **`UpdateHwcNodeEnableByBackgroundAlpha`** → 命中纯色层 → `ProcessSolidLayerEnabled` / 否则 `ProcessSolidLayerDisabled`。
5. **`UpdateHwcNodeByTransform`**：调用 `RSUniHwcComputeUtil` 完成 `DealWithNodeGravity` / `DealWithScalingMode` / `LayerCrop` / `CalcSrcRectByBufferFlip`。
6. **`UpdateDstRectByGlobalPosition`**：把 dstRect 加上屏的 `offsetX/offsetY`。
7. **`UpdateHwcNodeEnableByBufferSize`**：XCOMPONENT / Web 在缩放场景下 buffer 尺寸与 bounds 不匹配 → `DISABLED_BY_BUFFER_NONMATCH`。

随后在 `RSUniRenderVisitor::UpdateHwcNodeEnableForScreen`（screen 节点遍历末尾）统一调用：

8. **`UpdateHwcNodeEnable`**（`rs_uni_hwc_visitor.cpp:560`）：
   - `UpdateHwcNodeEnableByFilterIntersection`（HVE 滤镜相交）；
   - 反向遍历所有 HWC 节点 → `UpdateHwcNodeProperty`（向上归并 alpha / matrix / drawing cache / 圆角相交 AABB）、`UpdateHwcNodeEnableByAlpha`（累计 alpha 非 1 → `DISABLED_BY_ACCUMULATED_ALPHA`）、`UpdateHwcNodeEnableByRotate`（非 90 倍数旋转或含非 Z 轴旋转 → `DISABLED_BY_ROTATION`）、`UpdateHwcNodeEnableByHwcNodeBelowSelfInApp`；
   - `UpdateHwcNodeEnableByColorPicker`（取色任务相交 → 禁用）；
   - `uniRenderVisitor_.PrevalidHwcNode()`（预协商失败 → `DISABLED_BY_PREVALIDATE`）；
   - `UpdateHwcNodeEnableByNodeBelow`（背景 alpha、被上层 HWC 覆盖、transparent HwcNode → `DISABLED_BY_TRANSPARENT_NODE` / `DISABLED_BY_HWC_NODE_ABOVE` 等）；
   - `UpdateAncoNodeHWCDisabledState`（ANCO 窗口内任一 HWC 节点禁用则整组禁用 → `DISABLED_BY_ANCO_HAS_GPU`）；
   - `UpdateScreenHdrForceHwcState`（HDR 强制 HWC 名单）。
9. **`UpdateHwcNodeDirtyRegionAndCreateLayer`**（`rs_uni_render_visitor.cpp:2876`）：
   - 屏级 HDR / DRM / 指纹存在且非保护层 → `DISABLED_BY_RENDER_HDR_SURFACE`；
   - 计算 `RSUniHwcComputeUtil::GetLayerTransform`，调用 `hwcNodePtr->UpdateHwcNodeLayerInfo(transform)` 创建/更新硬件层信息；
   - 受 SurfaceLock 标记影响 `SetHasSurfaceLockLayer`。
10. **TopLayer 处理**：TopLayer 在 `DISABLED_BY_TOP_LAYERS`、`DISABLED_BY_POINT_WINDOW`（硬光标）、`DISABLED_BY_RENDER_HDR_SURFACE` 等条件下被禁用，或根据 `GPU_OFFLINE_DEVICE` 准备状态决定。

### srcRect 与 dstRect 计算

| 步骤 | 函数 | 输入 | 作用 |
| --- | --- | --- | --- |
| 入口 srcRect | `RSUniHwcVisitor::UpdateSrcRect` | totalMatrix、consumer/buffer、bounds | 由 dstRect 经 inverse(totalMatrix) 与 inverse(gravity) 反推；处理 90/270 旋转时宽高 swap；再过 `CalcSrcRectByBufferRotation` |
| Gravity / Scaling / Crop | `RSUniHwcComputeUtil::DealWithNodeGravity` / `DealWithScalingMode` / `LayerCrop` | screenProperty、totalMatrix、gravityMatrix、scalingModeMatrix | API18+ / SolidLayer / PROTECTED / RenderFitSurface 走新版本，否则 `DealWithNodeGravityOldVersion`；按 scalingMode 调整 dstRect 与 srcRect |
| Buffer Flip | `RSUniHwcComputeUtil::CalcSrcRectByBufferFlip` | bufferTransformType | 水平/垂直翻转：`GRAPHIC_FLIP_H` / `GRAPHIC_FLIP_V` 重算 srcRect.left_/top_ |
| 真实 srcRect 修正 | `RSUniHwcComputeUtil::UpdateRealSrcRect` | absRect vs dstRect | buffer 尺寸 ≠ bounds 时按比例缩放 srcRect；按 bufferRect 裁剪；90/270 旋转 swap；最终过 `SrcRectRotateTransform` |
| 入口 dstRect | `RSUniHwcVisitor::UpdateDstRect` | absRect、clipRect、screenInfo | delegateMode 屏内 → 直通；否则与 screenRect/prepareClipRect/curSurfaceNode_->GetDstRect() 依次 intersect；HardwareEnabledTopSurface 时按 ROG 宽高比缩放 |
| 全局位置修正 | `RSUniHwcVisitor::UpdateDstRectByGlobalPosition` | screenProperty.offsetX/offsetY | `GetHwcGlobalPositionEnabled()` 为 true 时把屏偏移加到 dstRect |
| 矩阵归并 | `RSUniHwcComputeUtil::UpdateHwcNodeProperty` | hwcNode 及所有祖先 | 自底向上累乘 totalMatrix / alpha / absRotation；计算圆角 AABB 相交；判定 drawing cache / needBlend → `DISABLED_BY_DRAWING_CACHE` |
| Transform | `RSUniHwcComputeUtil::GetLayerTransform` | 节点 srcRect/dstRect 与 buffer/旋转 | 输出 GraphicTransformType 给 `UpdateHwcNodeLayerInfo` 用于创建硬件层 |

关键不变量：
- srcRect 始终在 buffer 坐标系内（含旋转/翻转修正），dstRect 始终在屏幕坐标系内。
- delegateMode 节点 dstRect 不与 prepareClipRect 相交，保留原始矩形用于 DSS 直通。
- 受 ROG（Render On Grid）影响时仅 HardwareEnabledTopSurface 走宽高比缩放。

### DirectComposition（直通）进入条件

`doDirectComposition_` 在帧内被多次复核，最终同时满足下列全部条件才会真正进入 `DoDirectComposition`：

**前置（ResetHardwareEnabledState 阶段）**
- 全局 `RSSystemProperties::GetHardwareComposerEnabled()` 为 true；
- 全局 `RSSystemProperties::GetDoDirectCompositionEnabled()` 为 true。

**`CollectInfoForHardwareComposer` 阶段**（任一不满足即关闭）
- 无 pending 截图任务（`pendingUiCaptureTasks_.empty()`）；
- 所有 HWC 候选节点 `GetDoDirectComposition()` 都为 true；
- 不存在「不在树但消费了 buffer」的 HWC 节点；
- 无硬件 HDR 禁用 + 非 HDR Surface；
- 没有 HWC 节点处于「GPU→HWC 切换且 buffer 已更新」或 `HwcDelayDirtyFlag=true`；
- 没有节点 `IntersectWithFilterNode && buffer consumed`；
- 非多屏（`isMultiDisplay` 在 `CheckIfHardwareForcedDisabled` 末尾单独关闭）。

**`UniRender` 阶段**（`rs_main_thread.cpp:2987` 起）
- `!isHardwareForcedDisabled_`；
- `!RSUifirstManager::NeedNextDrawForSkippedNode()`（UIFirst 不需要补帧）；
- `!GetContext().GetPowerOffRenderController().GetAllScreenRenderSkipped()`（非灭屏渲染跳过）；
- `UpdateDirectCompositionByAnimate`：动画运行帧或动画最后一帧 → 关闭；
- `willGoDirectComposition = doDirectComposition_ && !isDirty_ && !isAccessibilityConfigChanged_ && !isCachedSurfaceUpdated_ && pointerSkip`；
- `isHardwareEnabledBufferUpdated_` 为 true 才真正调用 `DoDirectComposition(rootNode)`。

**`DoDirectComposition` 内部**（`rs_main_thread.cpp:3226`）返回 false 即回退到正常 GPU 渲染：
- 根节点存在子节点；
- 找到 `RSScreenRenderNode` 且 `CompositeType == UNI_RENDER_COMPOSITE`；
- 屏状态为 `HDI_OUTPUT_ENABLE`；
- AI Bar 节点不需要更新缓存（`CheckReduceIntervalForAIBarNodesIfNeeded`）；
- `RSProcessorFactory::CreateProcessor` 成功，`processor->Init` 成功；
- `RSAncoManager::AncoOptimizeScreenNode` 未触发接管。

成功路径：
1. 通过 `PostSyncTask` 把后处理投到 `RSUniRenderThread`：`ResetVideoHeadroomInfo`、按 HWC 节点更新 Nit/Headroom、`SetHasSurfaceLockLayer`；
2. 调用 `RSProcessor::Process` 走硬件合成路径；
3. `needTraverseNodeTree = !DoDirectComposition(...)`：成功则**跳过节点树遍历**，把 `hardwareEnabledDrwawables_` 等直接同步给渲染线程；
4. `needPostAndWait_ = !doDirectComposition_ && needDrawFrame_`：直通成功时主线程不同步等待渲染线程，降低延迟。

退出 DirectComposition（回到 GPU 渲染）时：
- `aibarNodes_.clear()`、`lastFrameDidGpuRender_ = true`、`ResetConsecutiveDoCompSuccessCount()`；
- `RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot()` 触发 tunnel 仲裁刷新；
- `doDirectComposition_ = false`。

### HWC 禁用原因（DFX）

`HwcDisabledReasons` 枚举（共 25 项）覆盖主线程所有禁用路径，由 `HwcDisabledReasonCollection` 单例按 NodeId 维度统计，便于线上回退问题定位：

| 编号 | 枚举 | 触发位置 |
| --- | --- | --- |
| 0 | `DISABLED_BY_FLITER_RECT` | `UpdateHwcNodeEnableByFilterIntersection`：与滤镜区域相交 |
| 3 | `DISABLED_BY_ACCUMULATED_ALPHA` | `UpdateHwcNodeEnableByAlpha`：归并后 alpha ≠ 1 |
| 4 | `DISABLED_BY_ROTATION` | `UpdateHwcNodeEnableByRotate`：非 90° 倍数或含非 Z 轴旋转 |
| 5 | `DISABLED_BY_HWC_NODE_ABOVE` | `UpdateHwcNodeEnableByHwcNodeBelowSelf`：被上层 HWC 覆盖 |
| 6 | `DISABLED_BY_BACKGROUND_ALPHA` | `ProcessSolidLayerDisabled` / 背景非纯色 |
| 7 | `DISABLED_BY_INVALID_PARAM` | `UpdateHwcNodeInfo` 入口：无 buffer / 不可见 / 动态禁用 |
| 8 | `DISABLED_BY_PREVALIDATE` | `PrevalidHwcNode` 预协商失败 |
| 10 | `DISABLED_BY_BUFFER_NONMATCH` | `UpdateHwcNodeEnableByBufferSize`：buffer 尺寸与 bounds 不匹配 |
| 11 | `DISABLED_BY_RENDER_HDR_SURFACE` | `UpdateHwcNodeDirtyRegionAndCreateLayer`：屏级 HDR/DRM/指纹存在 |
| 12 | `DISABLED_BY_SOLID_BACKGROUND_ALPHA` | SolidLayer 候选但 alpha 条件不满足 |
| 13 | `DISABLED_BY_HWC_NODE_BELOW_SELF_IN_APP` | 同 App 内被下方 HWC 节点关系影响 |
| 14 | `DISABLED_BY_UIFIRST` | UIFirst 相关禁用 |
| 15 | `DISABLED_BY_DRAWING_CACHE` | `UpdateHwcEnableByProperty`：父节点处于静态缓存 / NodeGroup |
| 16 | `DISABLED_BY_ABOVED_BOUND_NE_DST_RECT` | `UpdateHardwareStateByBoundNEDstRectInApps` |
| 17 | `DISABLED_BY_TRANSPARENT_NODE` | 上层透明 HWC 节点影响 |
| 18 | `DISABLED_BY_CROSS_NODE` | Clone 跨屏节点 |
| 19 | `DISABLED_BY_SCREEN_NODE_ROTATION` | ScreenNode `IsNeedClientCompose()` |
| 20 | `DISABLED_BY_ANCO_HAS_GPU` | ANCO 窗口内已有 GPU 合成节点 |
| 21 | `DISABLED_BY_TOP_LAYERS` | TopLayer 处理阶段命中禁用条件 |
| 22 | `DISABLED_BY_IS_CLONE_NODE` | SurfaceRenderNode 自身的 Clone 节点判定 |
| 23 | `DISABLED_BY_POINT_WINDOW` | 指针窗口（硬光标）节点 |

### 主线程 → 渲染线程的 HWC 参数同步

`RSRenderThreadParams` 承载以下 HWC 相关字段：

- `hardwareEnabledTypeDrawables_` ← `hardwareEnabledDrwawables_`（每个 entry 包含 screenNodeId、logicalDisplayNodeId、RenderDrawable）；
- `selfDrawables_` / `canvasDrawingSelfDrawables_` ← 自绘节点；
- `protectiveSolidDrawables_` ← 保护性纯色节点；
- `hardCursorDrawableVec_` ← `RSPointerWindowManager::GetHardCursorDrawableVec()`。

当 `doDirectComposition_` 为 true 时主线程**跳过 `drawFrame_.PostAndWait()`**，渲染线程不再做 GPU 合成，仅由 `DoDirectComposition` 内的 `PostSyncTask` 完成必要的 Nit/Headroom 更新和 processor 处理。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 帧级与节点级禁用分层 | `CheckIfHardwareForcedDisabled`（全局）+ `UpdateHwcNodeEnable*`（节点） | 全局条件（折叠/多屏/颜色滤镜）一次性判定避免对每个节点重复计算；节点级条件保留细粒度控制 |
| `hasProtectedLayer_` 例外 | `(!hasProtectedLayer_ && (...))` 包裹多数禁用分支 | DRM 保护内容必须走 HWC，即便处于折叠/多屏等场景也保留硬件路径 |
| 候选节点筛选前置 | `IsHardwareEnabledType()` 在 `TraverseSurfaceNodes` 中早退 | 大量非自绘节点无需进入 HWC 计算，避免无效矩形运算 |
| DirectComposition 多次复核 | 从 `CollectInfoForHardwareComposer` 到 `UniRender` 每个阶段都可能关闭 `doDirectComposition_` | 直通跳过整个 GPU 渲染，风险高；任一不利条件都需即时回退 |
| `HwcDelayDirtyFlag` | GPU→HWC 切换首帧若 buffer 未更新，延后一帧置 dirty | 避免上帧 GPU 内容残留造成闪烁，同时尽量保留直通机会 |
| 矩阵归并放 `UpdateHwcNodeProperty` | 自底向上 `TraverseParentNodeAndReduce` | alpha / matrix / 圆角 AABB 需要全路径归并，集中计算避免重复 |
| DFX 25 个禁用原因 | `HwcDisabledReasonCollection` + `HwcDisabledReasons` | 线上 HWC 回退问题定位困难，按 NodeId 累计原因可快速定位根因 |
| 直通成功跳过 PostAndWait | `needPostAndWait_ = !doDirectComposition_ && needDrawFrame_` | 直通场景下渲染线程无需绘制，主线程不等同步信号，降低端到端延迟 |
| 双套收集列表分工 | `hardwareEnabledNodes_`（主线程扁平遍历）+ `allHwcNodeAndFilterNode_`（Visitor 自底向上归并） | 前者用于直通 / 渲染线程同步；后者保留 z-order 与滤镜关系，支撑滤镜相交、跳过子树等复杂判定 |
| `allHwcNodeAndFilterNode_` 混合类型 | 同一 deque 内并存 HWC 候选与滤镜节点 | 反向遍历即可一次性得到「上方所有滤镜 + 当前 HWC」视图，避免再维护一份滤镜列表；派生列表过滤掉非 HWC 后供下游消费 |
| 跳过子树仍保留矩形更新 | `CollectHwcAndFilterNodesInSkippedSubTree` + `UpdateHwcNodeRectInSkippedSubTree` | QuickSkip 是性能优化，不能让 HWC 直通的矩形信息丢失，否则下一帧直通会用过期坐标 |
| ROG 缩放只对 TopSurface | `IsHardwareEnabledTopSurface()` 分支内乘 widthRatio/heightRatio | 普通子 Surface 的 dstRect 已在父坐标内，避免重复缩放 |
| DoDirectComposition 内 PostSyncTask | 把 Nit/Headroom 更新与 Processor 处理同步到渲染线程 | 这些操作依赖渲染线程上下文（EGL/Vulkan 状态），但又必须在主线程决定直通后立即执行 |

## 待补充背景

- `willGoDirectComposition` 中 `isCachedSurfaceUpdated_` / `pointerSkip` 的具体语义和触发场景。
- `UpdateHwcNodeEnableByNodeBelow` 中 `RsCommonHook::GetHardwareEnabledByBackgroundAlphaFlag` 等开关的默认值与产品差异。
- `RSDirectCompositionHelper::consecutiveDoCompSuccessCount_` 的累计阈值与 tunnel 激活的关系。
- AI Bar 缓存间隔调优（`CheckReduceIntervalForAIBarNodesIfNeeded` / `ForceReduceAIBarCacheInterval`）的完整算法。
- 各 HwcDisabledReasons 在现网的占比与典型回退路径（线上数据）。
- `allHwcNodeAndFilterNode_` 在 `isParentPrepareInReverseOrder` 各取值下与 Visitor 实际遍历方向的精确对应关系（屏展开 / 投屏 / 镜像场景）。
- `UpdateHwcNodeEnableByFilterIntersection` 中 AIBar / HveBlur / `IsTransparent` 等缓存优化分支的触发条件与命中率。
- 多屏 + 镜像模式下 HWC 与 DirectComposition 的精细化差异（为什么 HWC 可走、DirectComposition 必关）。
- SurfaceLockLayer (`SetHasSurfaceLockLayer` / `GetFixRotationByUser`) 与 DRM 联动对 HWC 的完整影响。
