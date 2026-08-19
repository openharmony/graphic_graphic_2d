# 遮挡裁剪与可见区域

## 适用范围

改动涉及以下场景时，先读本文，再回到代码确认当前实现：

- Surface/App 级不透明区域、可见区域和窗口可见等级。
- Canvas/Pixel 级 `RSOcclusionHandler`、`OcclusionNode` 或节点/子树裁剪。
- Pixel 级 stencil 遮挡、Leash Window 背景、阴影或 UIFirst cache。
- 可见脏区、Quick Reject、跨屏节点、虚拟屏或安全屏。
- 遮挡参与条件、动画、圆角、Filter、3D 变换、透明度和调试开关。

本文是背景知识和排查路线，不替代代码。脏区域历史、Buffer Age 和 damage 生成见
`docs/knowledge/dirty-region.md`。

## 快速代码地图

以下路径均相对于 `foundation/graphic/graphic_2d/`。

| 方向 | 主要文件 |
| --- | --- |
| Surface 级入口 | `rosen/modules/render_service/core/pipeline/main_thread/rs_uni_render_visitor.cpp` |
| Surface 数据模型 | `rosen/modules/render_service_base/{include,src}/pipeline/rs_surface_render_node.*` |
| Surface 参数 | `rosen/modules/render_service_base/{include,src}/params/rs_surface_render_params.*` |
| 参与条件 | `rosen/modules/render_service/core/feature/dirty/rs_uni_dirty_occlusion_util.{h,cpp}` |
| 轻量树管理 | `rosen/modules/render_service/core/feature/occlusion_culling/rs_occlusion_handler.{h,cpp}` |
| 轻量树节点 | `rosen/modules/render_service/core/feature/occlusion_culling/rs_occlusion_node.{h,cpp}` |
| 可见脏区消费 | `rosen/modules/render_service/core/drawable/rs_surface_render_node_drawable.cpp` |
| Stencil 初始化 | `rosen/modules/render_service/core/drawable/rs_logical_display_render_node_drawable.cpp` |
| 绘制侧 stencil | `rosen/modules/render_service_base/src/drawable/rs_render_node_drawable_adapter.cpp` |
| 配置入口 | `rosen/modules/render_service/core/feature_cfg/feature_param/performance_feature/` |
| 单测 | `rosen/test/render_service/render_service/unittest/feature/occlusion_culling/` |

## 核心模型

当前代码中“遮挡”至少包含三套边界不同的机制：

| 层级 | 主要对象 | 决策结果 |
| --- | --- | --- |
| Surface/App | `RSSurfaceRenderNode`、`RSUniRenderVisitor` | Surface 可见区域与可见等级 |
| Canvas/Pixel | `RSOcclusionHandler`、`OcclusionNode` | 被裁剪节点集合和整棵子树集合 |
| Pixel stencil | `RSPaintFilterCanvas`、Surface stencil 参数 | 绘制命令的像素级 stencil 测试 |

这三层不是同一个算法的三个精度档位：

- Surface 级在主线程遍历窗口时累计上层不透明区域。
- Canvas/Pixel 级为单个 Surface 内的 RS 节点维护轻量树。
- stencil 路径由上层 Surface 的 opaque rect 和 stencil 值驱动具体绘制命令。

修改任一层时，先确认数据由谁产生、经过哪个 Params 同步、最终由哪个 Drawable 消费。

## 设计背景与决策理由

下面的“代码体现”来自当前实现；“设计意图”是从代码路径、注释和模块职责归纳出的解释。

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| Surface 从上层累计不透明区域 | `accumulatedOcclusionRegion_` | 快速跳过完全被上层窗口覆盖的区域 |
| 普通、虚拟和后窗可见区分开保存 | 三个 visible region 字段 | 适配 skip layer、安全屏和 behind-window Filter |
| Surface 内使用轻量树 | `RSOcclusionHandler` 与 `OcclusionNode` | 避免直接复制或反复遍历完整 RS 节点状态 |
| 节点和整棵子树分别记录 | `culledNodes_`、`culledEntireSubtree_` | 区分只跳自身和可以跳过后代的情况 |
| 不透明 coverage 使用 inner rect | `OcclusionNode::ComputeInner()` | 圆角和小数边界下避免过度遮挡 |
| 被测节点使用 outer rect | `ComputeOuter()`、`CheckNodeOcclusion()` | 只有完整覆盖外边界时才裁剪 |
| Filter 会裁小 coverage | `FindMaxDisjointSubRect()` | Filter 影响区域不能继续作为可靠遮挡覆盖 |
| stencil 由 Feature 参数和系统属性控制 | `OcclusionCullingParam`、`RSSystemProperties` | 支持产品配置和运行时覆盖 |

遮挡是正确性敏感优化。无法证明“完整覆盖”时应保守保留绘制，而不是扩大裁剪范围。

## Surface/App 级遮挡

### 可见区域计算

`RSUniRenderVisitor::UpdateNodeVisibleRegion()` 以 `GetSurfaceOcclusionRect(true)` 作为本 Surface
的绘制范围，并在需要重新计算时生成：

- `visibleRegion_`：通常是 self draw region 减去 `accumulatedOcclusionRegion_`。
- `visibleRegionBehindWindow_`：再减去 behind-window 累积区域。
- `visibleRegionInVirtual_`：减去不包含 skip layer 的累积区域。

安全虚拟扩展或独立合成场景会选择不同的累计区域。First-level cross node 还有提前返回和忽略
普通可见性求交的特殊分支，修改时不要只验证单屏主窗口。

### 不透明区域和参与条件

`CalculateOpaqueAndTransparentRegion()` 只继续处理符合条件的主窗口，并执行：

1. 按需重置动画场景中的累计遮挡区域。
2. 调用 `CheckAndUpdateOpaqueRegion()`，考虑屏幕旋转、圆角和容器透明等因素。
3. 通过 `RSUniDirtyOcclusionUtil::IsParticipateInOcclusion()` 判断是否贡献遮挡。
4. 参与时把 `opaqueRegion_` 合入相应累计区域。
5. 收集 WMS 可见等级和顶部遮挡 Surface 信息。

“被计算可见区域”“参与遮挡”“贡献不透明区域”是三个不同问题。透明 Surface 可以被上层遮挡，
但不一定能遮挡下层；主窗口也可能因动画、调试或节点状态不贡献遮挡。

当前参与判断还依赖：

- `RSSurfaceRenderNode::CheckParticipateInOcclusion()` 的节点条件。
- 焦点、祖先动画、特定动画场景和 attraction animation。
- all-surface-visible 调试开关。
- Filter 全屏透明动画场景对累计区域的重置。

不要把 feature 文档中的 Surface 类型对照表当作永久白名单；以当前判断函数为入口追踪。

### 可见等级与回调

WMS 可见等级由 self draw region 和 visible region 计算。Self-drawing node、Ability Component
等还有单独的回调语义。修改渲染裁剪不等于可以同步改变 WMS 对外可见性；涉及回调行为时还要检查
`RSMainThread` 的 occlusion listener 和公开接口兼容性。

## Canvas/Pixel 级遮挡

### 轻量树生命周期

每个开启该能力的 Surface 使用一个 `RSOcclusionHandler`：

- `CollectNode()` 收集或更新单个节点。
- `CollectSubTree()` 在 Prepare 被跳过时维护轻量树与真实 RS 树的一致性。
- `UpdateSkippedSubTreeProp()` 更新静态子树属性。
- `CalculateFrameOcclusion()` 生成本帧裁剪集合并移除 off-tree 节点。
- `TakeCulledNodes()` 和 `TakeCulledEntireSubtree()` 把结果移入 Surface 的 occlusion params。

`RSUniRenderVisitor::CollectSubTreeAndProcessOcclusion()` 在处理到根节点时完成计算；
`RSSurfaceRenderNode::UpdateRenderParams()` 再把集合放入 `RSSurfaceRenderParams`，Drawable 最终设置到 canvas。

### 当前检测边界

`OcclusionNode::DetectOcclusionInner()` 从高 z-order 子节点开始处理。当前实现不是任意 Region 并集算法，
而是围绕 coverage rect 和面积维护候选覆盖：

- 只有满足 `IsOpaque()` 的节点更新全局 coverage。
- `IsOpaque()` 同时要求背景不透明、alpha 不需要混合、子树未忽略且 blend mode 可接受。
- 当前只有 `CANVAS_NODE` 会进入节点或整棵子树的裁剪集合。
- 节点 outer rect 完全落在 coverage rect 内，或完全在根范围外，才可能被裁剪。
- 有子节点超出父范围时，通常只能裁剪自身；没有超出时才可能裁剪整棵子树。
- 带不受支持的裁剪、3D 变换、透明度、Filter 或异常几何时，属性收集可能忽略子树或缩小 coverage。

因此不要把 Canvas/Pixel 级描述成“累计所有不透明节点 Region 后求差”。修改算法时要为圆角、
父子裁剪、children-out-of-rect、Filter、3D、blend mode、off-tree 和静态子树分别补测试。

### 绘制侧消费

Surface 参数中的 `isOcclusionCullingOn_` 决定 Drawable 是否调用 `SetCulledNodesToCanvas()`。
绘制结束后会清空当前线程 canvas 上的裁剪集合。Canvas、Effect 等 Drawable 还会结合
`GetOpDropped()` 和 `QuickReject()` 决定是否记录 `OCCLUSION_SKIP`。

排查“集合生成正确但仍绘制”时，沿 params 同步、Surface Drawable 设置、子 Drawable Quick Reject
三段检查，不要只看 `RSOcclusionHandler`。

## Pixel 级 stencil 遮挡

stencil 路径由 `OcclusionCullingParam::IsStencilPixelOcclusionCullingEnable()` 和系统属性共同决定。
当前主要数据流是：

1. 主线程收集顶部 Surface 的 opaque rect，并为相关 Surface 设置 stencil 值。
2. `RSLogicalDisplayRenderNodeDrawable::ClearCanvasStencil()` 清理屏幕 stencil，按顶部 opaque rect
   写入分层 stencil 值。
3. `RSSurfaceRenderNodeDrawable` 把 Surface 的 stencil 值传给 Leash Window 背景绘制。
4. `DrawLeashWindowBackground()` 只在指定绘制区间设置 stencil；随后恢复 invalid 值。
5. UIFirst cache、阴影路径也会读取 stencil 值并选择 stencil 绘制接口。

修改 stencil 时要同时检查最大值、Surface 顺序、阴影使用的偏移值、无顶部 opaque rect、能力关闭、
UIFirst cache 和 canvas 状态恢复。驱动兼容性与性能收益需要真实设备验证。

## 可见脏区域与脏区交点

`RSSurfaceRenderNodeDrawable::CalculateVisibleDirtyRegion()` 是脏区和遮挡的明确交点：

1. 只处理主窗口或 Leash Window。
2. 离屏路径当前返回一个极大矩形作为保守结果。
3. 遮挡开启且 visible region 为空时，普通节点直接返回空。
4. 从 `GetDirtyRegionForQuickReject()` 合并 dirty region。
5. 普通节点计算 `dirtyRegion AND visibleRegion`；first-level cross node 使用原 dirty region。
6. 结果为空时跳过 Surface 绘制。

Quick Reject 列表由渲染线程生成，不由 `RSDirtyRegionManager::OnSync()` 直接复制。具体历史和 damage
生成流程见 `dirty-region.md`。

本文负责 Quick Reject 的消费和可见区域裁剪，不负责定义 `OnSync()` 中 Quick Reject 的复制、清理或
重建策略。只修改管理器生命周期时优先阅读 `dirty-region.md`；改变 visible dirty 结果时再联合阅读本文。

## 特殊场景边界

### 跨屏和虚拟屏

- First-level cross node 可能跳过普通 Surface 可见区域更新或求交。
- virtual visible region 使用不包含 skip layer 的累计区域。
- 安全虚拟扩展或独立合成会改变 `visibleRegion_` 选择的累计区域。

### 动画和 Filter

- 特定动画场景可以重新允许遮挡参与。
- attraction animation 和祖先动画会影响参与条件。
- 全屏透明 Filter 在特定动画场景可能重置累计遮挡。
- Canvas/Pixel 级 Filter 会裁小 coverage rect，避免把受 Filter 影响区域继续用于遮挡。

### 圆角、alpha 和 blend

Surface opaque region 与 `OcclusionNode` coverage 都会保守处理圆角和透明度。Canvas/Pixel 级还会检查
blend mode；新增 blend 行为时要确认它是否仍能作为可靠 occluder。

## 线程与数据交接

Surface 级可见区域和 Canvas/Pixel 裁剪集合主要在主线程 Prepare/Process 路径产生，随后通过
Render Params 交给 Drawable。渲染线程读取 params、设置 canvas 状态并执行跳过。

`RSOcclusionHandler` 的容器没有内部锁。它依赖调用时序和单线程归属，不应被当作通用线程安全对象。
修改集合所有权时尤其注意 `Take*()` 使用移动语义：读取后源集合的内容不能再被依赖。

## 基于代码路径的现象定位

以下内容根据当前实现的数据流、分支条件和现有单测反向整理，不代表已确认的线上高频故障。
遮挡是正确性敏感优化；定位时先证明完整覆盖和状态交接，再考虑扩大裁剪范围。

### 定位前先确定层级和断点

1. **层级**：Surface/App、Canvas/Pixel 和 Pixel stencil 是三套边界不同的机制。
2. **功能入口**：确认 feature 参数、系统属性、主线程开关和渲染线程参数是否一致。
3. **生产结果**：检查 visible/opaque region，或 `RSOcclusionHandler` 生成的两个裁剪集合。
4. **同步结果**：检查 `OcclusionParams`、staging params 和 render-thread params。
5. **消费结果**：检查 Surface Drawable、子 Drawable、canvas 集合以及绘制后的状态恢复。

不要用某一层的空集合证明全部遮挡失效，也不要把 WMS 可见等级等同于渲染侧是否跳过。

### 本应绘制的 Surface 被跳过或局部漏绘

按以下顺序检查：

- `GetSurfaceOcclusionRect(true)`、opaque region、visible region 和 Quick Reject dirty 是否处于同一
  Screen 坐标空间。
- 上层 Surface 的圆角、容器透明、背景 alpha 和旋转是否生成了过大的 opaque region。
- 当前节点是“被计算可见区域”还是“参与遮挡”；`IsParticipateInOcclusion()` 影响它遮挡下层，
  不直接决定它是否被上层遮挡。
- `CalculateVisibleDirtyRegion()` 是否因非主窗口/非 Leash Window、空 render-thread params、
  空 visible region 或空 Quick Reject dirty 返回空。
- first-level cross node 是否进入提前返回或跳过 visible 求交的分支，当前屏幕是否是首次访问 cross node。
- 安全虚拟屏、virtual visible region、skip layer 和 behind-window 是否选择了正确累计区域。

离屏路径当前返回极大矩形作为保守结果，不应按普通 `dirty AND visible` 的结果判断离屏漏绘。

### 被完全遮挡的 Surface 仍在绘制

优先检查：

- Surface 级遮挡总开关是否开启；`needRecalculateOcclusion_` 是否被节点 dirty、遮挡属性变化、
  behind-window 变化、黑白名单变化或复用判断正确触发。
- 上层节点是否为主窗口，是否通过 `IsParticipateInOcclusion()`，以及 opaque region 是否实际非空。
- all-surface-visible 调试开关、祖先动画、焦点、attraction animation 或全屏透明 Filter 是否让
  累计遮挡被旁路或重置。
- 当前场景应使用 `accumulatedOcclusionRegion_`、`occlusionRegionWithoutSkipLayer_` 还是
  `accumulatedOcclusionRegionBehindWindow_`。
- visible region 是否经过 `RSSurfaceRenderNode::UpdateRenderParams()` 和 params OnSync 到达当前 Drawable。
- 最终仍绘制的是 Surface buffer、Leash 背景、阴影、Filter、UIFirst cache，还是 Surface 内子节点；
  它们不一定共享同一个跳过条件。

如果 visible region 正确为空但仍有绘制，继续沿 Drawable 的专用绘制分支检查，不要重复修改主线程算法。

### WMS 可见等级或遮挡回调与画面不一致

渲染裁剪和 WMS 回调是不同输出：

- `CollectOcclusionInfoForWMS()` 只处理主窗口，并用 self draw region 与 visible region 计算等级。
- Self-drawing node 在实例根可见时、Ability Component 等场景存在强制全可见语义。
- `SurfaceOcclusionCallbackToWMS()` 只在本次结果向量与上次不同时回调。
- 修改 visible dirty、Canvas 裁剪或 stencil 不应自动改变 WMS 对外可见性。

排查时分别记录 WMS visible level、Surface visible region 和渲染侧 draw region，避免比较不同语义的数据。

### cross node、虚拟屏或安全屏结果不一致

优先检查：

- first-level cross node 在非首次访问 cross display 时可能跳过普通 visible region 更新和 opaque 累积。
- first-level cross node 的 visible dirty 当前直接使用 dirty region，不与 visible region 求交。
- 安全虚拟扩展或独立合成计算普通 visible region 时，可能改用不包含 skip layer 的累计区域。
- `visibleRegion_`、`visibleRegionInVirtual_` 和 `visibleRegionBehindWindow_` 是否被消费端混用。
- 跨屏转换前后的屏幕旋转、矩阵和坐标原点是否一致。

单屏主窗口用例不能覆盖这些分支；修改 cross/virtual 逻辑时至少准备普通节点和 first-level cross node
使用相同 dirty、不同 visible region 的区分性输入。

### Canvas 节点误裁剪

先检查轻量树中的属性是否新鲜，再检查几何：

- `CollectNodeProperties()` 是否读取到本帧 alpha、scale、translate、clip、圆角、背景色、blend 和 Filter。
- 父节点、纹理导出、shared transition、建议 OpInc、旋转/透视/skew、3D、特殊 clip、阴影、outline、
  pixel stretch、foreground Filter 和 distortion 是否让子树进入保守忽略。
- 非法 scale、alpha、圆角、bounds、translate、pivot 或 clip rrect 是否把子树标记为忽略。
- `outerRect_`、`innerRect_`、父 clip、frame offset 和绝对坐标转换是否正确。
- `hasChildrenOutOfRect_` 为 `false` 时才可能把整棵子树加入裁剪集合；否则通常只能裁剪自身。
- Filter 节点是否正确裁小 coverage；已被整棵子树遮挡的 Filter 不应再次裁小同一 coverage。

当前实现只对 `CANVAS_NODE` 写入裁剪集合。其他节点类型出现在结果中时，应先检查集合生产或节点类型同步。

### Canvas 节点没有被裁剪

确认当前算法的保守边界：

- 被测节点的 outer rect 必须完整落入一个 coverage rect，或完整位于根范围外。
- 当前全局 coverage 主要保留面积更大的单个矩形，不等价于任意多个 opaque rect 的 Region 并集。
- occluder 需要满足背景不透明、累计 alpha 不需要混合、blend 可接受且子树未忽略。
- 被测节点带 `clipToBounds` 时，当前节点级裁剪还有额外限制。
- Filter 与可靠 coverage 相交时会把 coverage 裁小，可能使下层节点继续绘制。
- 圆角使用 inner rect 贡献 coverage、outer rect 判断被覆盖；边缘保守保留通常是预期行为。

先判断是算法边界还是属性收集错误，再决定是否修改；不能为提高命中率直接把 outer rect 当作 opaque coverage。

### 静态子树、节点删除或换父后结果错误

轻量树必须跟随真实 RS 树：

- Prepare 被跳过不代表子树完全不变；`CollectSubTree()` 仍会检查一级子节点删除，并按需重建后代。
- 已存在且未被忽略的静态子树会进入 `subTreeSkipPrepareNodes_`，随后由
  `UpdateSkippedSubTreeProp()` 递归更新属性。
- 新父节点未被收集时，已存在的子树会从轻量树移除；off-tree 节点在本帧检测后统一处理。
- `isValidInCurrentFrame_` 用于识别未在本帧确认的节点，不能跨帧手工保持为真。

相关改动至少覆盖节点删除、换父、父节点未收集、静态子树新增一级子节点和 ignored 子树传播。

### 裁剪集合生成正确但仍绘制，或源集合突然为空

按所有权移动顺序检查：

1. `RSOcclusionHandler::CalculateFrameOcclusion()` 生成 `culledNodes_` 和 `culledEntireSubtree_`。
2. Visitor 用 `Take*()` 把集合移动到 Surface 的 `OcclusionParams`。
3. `RSSurfaceRenderNode::UpdateRenderParams()` 再用 `Take*()` 移到 staging params；能力关闭时清空集合。
4. params OnSync 把集合移动到渲染线程 params。
5. Surface Drawable 把集合复制到 canvas，子 Drawable 才能执行 Quick Reject。
6. Surface 绘制结束后清空 canvas 集合，避免污染后续 Surface。

每次 `Take*()` 后源集合为空是预期结果。调试代码必须在移动前观察源，或在移动后的下一所有者上观察；
不能把移动后的空集合误判为算法没有生成结果。

如果 params 中集合正确但仍绘制，检查 `isOcclusionCullingOn_`、`SetCulledNodesToCanvas()` 是否执行、
节点 ID 是否一致，以及子 Drawable 是否实际调用了使用 canvas 裁剪集合的 Quick Reject 路径。

### 不同 Surface 之间出现错误裁剪

优先检查：

- Surface Drawable 是否只在 `isOcclusionCullingOn_` 为真时设置集合。
- 普通绘制和 quick-draw 路径结束时是否都清空 `culledNodes` 和 `culledEntireSubtree`。
- 提前返回、离屏、并行 canvas 或异常分支是否绕过了对应的设置或清理点。
- 是否把某个 Surface params 的集合缓存后用于另一个 Surface，或在错误线程共享 handler/canvas 状态。

`RSOcclusionHandler` 本身没有内部锁；不要用加锁猜测替代对主线程生产和渲染线程消费时序的检查。

### stencil 显示异常

优先检查：

- feature 参数、系统属性、主线程收集条件和渲染线程开关是否一致。
- 顶部 opaque rect 的顺序、数量、最大选取逻辑和 stencil 最大值是否匹配。
- Surface、Leash 背景、阴影和 UIFirst cache 使用的 stencil 值及偏移是否一致。
- 无顶部 opaque rect、能力关闭和普通非 stencil 绘制是否保持原行为。
- 设置 stencil 后是否恢复 `Drawing::Canvas::INVALID_STENCIL_VAL`，清屏和 Surface 间是否残留状态。
- 真实设备的 GPU/驱动是否支持当前路径；静态代码和单测不能证明显示效果或性能收益。

## 验证建议

就近单测目标优先看：

- `RSOcclusionHandlerTest`
- `RSOcclusionNodeTest`
- `RSUniDirtyComputeUtilTest`
- `RSSurfaceRenderNodeDrawableTest`
- `RSLogicalDisplayRenderNodeDrawableTest`

Surface occlusion callback 另有 proxy、stub 和接口层 fuzz，但它们主要覆盖 IPC/回调安全，不替代
内部裁剪算法测试。涉及 stencil、GPU 收益、圆角显示、多屏或真实窗口动画时，需要补充真实设备验证。

## 改动检查清单

- 是否先确认修改的是 Surface、Canvas/Pixel 还是 stencil 层？
- 是否区分“被遮挡”“参与遮挡”和“贡献 opaque region”？
- 是否检查圆角、alpha、blend、clip、3D、Filter 和异常几何？
- 是否检查节点裁剪与整棵子树裁剪的不同条件？
- 是否检查静态子树、off-tree、换父节点和移动语义？
- 是否检查 first-level cross node、虚拟屏、安全屏和 skip layer？
- 是否检查 visible region 与 Quick Reject dirty 的交点？
- 是否检查 params 同步、Drawable 消费和 canvas 状态恢复？
- 是否需要 WMS 回调兼容性、单测、fuzz 或真实设备验证？
- 是否避免在每帧遍历中增加大 Region 运算、容器复制或高频日志？

## 待补充背景

以下内容需要模块责任人或设备数据补充，不能仅靠静态扫描确定：

- 各产品启用三层遮挡能力的配置组合和灰度策略。
- Canvas/Pixel coverage 算法的目标场景、已知保守边界和性能基线。
- stencil 在不同 GPU/驱动上的收益、限制和异常回退策略。
- 历史线上误裁剪、漏裁剪、动画闪烁和跨屏问题的典型复现用例。
