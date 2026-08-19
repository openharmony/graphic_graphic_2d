# 脏区管理和局部刷新

## 适用范围

改动涉及以下场景时，先读本文，再回到代码确认当前实现：

- `RSDirtyRegionManager` 的脏区域合并、裁剪、历史记录和 Buffer Age。
- Surface、Display、HWC、UIFirst、Filter 或 advanced dirty region。
- EGL damage、GPU tile 对齐、Quick Reject 或部分渲染开关。
- 虚拟屏、镜像屏、扩展屏的脏区域累积、映射和清理。
- 主线程向渲染线程同步脏区域，或 DFX 脏区域显示异常。

本文是背景知识和排查路线，不替代代码。遮挡裁剪和可见区域计算见
`docs/knowledge/occlusion-culling.md`。

## 快速代码地图

以下路径均相对于 `foundation/graphic/graphic_2d/`。

| 方向 | 主要文件 |
| --- | --- |
| 管理器接口 | `rosen/modules/render_service_base/include/pipeline/rs_dirty_region_manager.h` |
| 管理器实现 | `rosen/modules/render_service_base/src/pipeline/rs_dirty_region_manager.cpp` |
| 屏幕脏区工具 | `rosen/modules/render_service/core/feature/dirty/rs_uni_dirty_compute_util.{h,cpp}` |
| 遮挡参与判断 | `rosen/modules/render_service/core/feature/dirty/rs_uni_dirty_occlusion_util.{h,cpp}` |
| Filter 收集器 | `rosen/modules/render_service_base/{include,src}/dirty_region/rs_filter_dirty_collector.*` |
| 渲染线程消费 | `rosen/modules/render_service/core/pipeline/render_thread/rs_uni_render_util.{h,cpp}` |
| Surface/Screen 同步 | `rosen/modules/render_service_base/src/pipeline/rs_{surface,screen}_render_node.cpp` |
| 多屏消费 | `rosen/modules/render_service/core/drawable/rs_logical_display_render_node_drawable.cpp` |
| DFX | `rosen/modules/render_service/core/drawable/dfx/rs_dirty_rects_dfx.{h,cpp}` |
| 单测 | `rosen/test/render_service/render_service_base/unittest/pipeline/rs_dirty_region_manager_test.cpp` |

## 核心模型

`RSDirtyRegionManager` 同时维护四类数据：

- 当前帧：`currentFrameDirtyRegion_` 是包围矩形，
  `currentFrameAdvancedDirtyRegion_` 保存多个矩形。
- 历史结果：`dirtyHistory_` 和 `advancedDirtyHistory_` 保存最近帧；
  `dirtyRegion_`、`advancedDirtyRegion_` 是按 Buffer Age 合并后的结果。
- 专用分支：`hwcDirtyRegion_`、`typeHwcDirtyRegion_`、`uifirstFrameDirtyRegion_`、
  `filterCollector_` 和虚拟扩展屏累积列表。
- 调试与快速剔除：节点级 DFX 数据、`mergedDirtyRegions_` 和
  `dirtyRegionForQuickReject_`。

最重要的区分是：

- `GetCurrentFrameDirtyRegion()` 返回当前帧包围矩形。
- `GetDirtyRegion()` 返回 `UpdateDirty()` 后按历史合并的包围矩形。
- `GetAdvancedDirtyRegion()` 返回历史合并后的多矩形结果；是否保留多矩形由
  `AdvancedDirtyRegionType` 决定。
- `GetDirtyRegionForQuickReject()` 不是普通历史结果的别名，它由渲染线程的
  `RSUniRenderUtil::SetDrawRegionForQuickReject()` 单独填充。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 同时维护包围矩形和多矩形 | `currentFrameDirtyRegion_` 与 advanced vectors | 在低管理成本和精细 damage 之间切换 |
| 历史队列固定为 10 帧 | `HISTORY_QUEUE_MAX_SIZE = 10` | 给 Buffer Age 和 UIFirst 提供有界历史 |
| 无有效历史时回退全 Surface | `MergeHistory()` 在 age 无效时合入 `surfaceRect_` | 不确定 buffer 内容时优先保证正确性 |
| Filter 独立收集和同步 | `RSFilterDirtyCollector` | 模糊和滤镜会扩展影响范围，不能只看节点边界 |
| Display 管理器额外保存原始矩形 | `isDisplayDirtyManager_`、`mergedDirtyRegions_` | 为多矩形 damage 和 DFX 保留更细粒度信息 |
| 多矩形过多时退化为包围矩形 | `UpdateCurrentFrameAdvancedDirtyRegion()` | 限制 region 管理与合并成本 |
| OnSync 传结果而非共享管理器 | `RSDirtyRegionManager::OnSync()` | 主线程生产、渲染线程消费，减少共享可变状态 |

不要把这些意图当成外部规格。阈值、开关和合并策略变化时，应重新核对代码和设备表现。

## 当前帧脏区域

### 合并和裁剪

常用修改接口：

- `MergeDirtyRect(rect)`：把有效矩形合入当前帧包围矩形，并更新 advanced dirty。
- `MergeDirtyRectIfIntersect(rect)`：只有 `rect` 与当前帧脏区相交时才扩展包围矩形。
- `IntersectDirtyRect(rect)`：同时裁剪当前帧包围矩形和 advanced dirty。
- `ClipDirtyRectWithinSurface()`：把结果限制在 active surface 或 surface 范围内。
- `MergeDirtyRectAfterMergeHistory(rect)`：历史合并完成后再扩展 `dirtyRegion_`。
- `ResetDirtyAsSurfaceSize()`：把当前帧结果、历史合并结果和 Quick Reject 设置为
  `activeSurfaceRect_`（非空时）或 `surfaceRect_`，但不改写历史队列。

`MergeDirtyRect()` 对空矩形直接返回。Display 管理器还会把输入矩形追加到
`mergedDirtyRegions_`；普通 Surface 管理器不会自动保留这份列表。`MergeDirtyRectIfIntersect()`
不能用来写入第一个矩形，因为当前帧脏区为空时相交判断为 `false`。`IntersectDirtyRect()` 会把
advanced 列表中的矩形原位裁剪，但不会删除裁剪后为空的列表项。

### DirtyRegionType

`DirtyRegionType` 用于节点级 DFX 归因，不等同于独立的渲染算法：

| 类型 | 典型来源 |
| --- | --- |
| `UPDATE_DIRTY_REGION` | 常规节点更新 |
| `OVERLAY_RECT` | Overlay 影响区域 |
| `FILTER_RECT` | Filter 影响区域 |
| `SHADOW_RECT` | 阴影影响区域 |
| `PREPARE_CLIP_RECT` | Prepare 裁剪区域 |
| `REMOVE_CHILD_RECT` | 子节点移除 |
| `RENDER_PROPERTIES_RECT` | 渲染属性变化 |
| `CANVAS_NODE_SKIP_RECT` | Canvas 节点跳过 |
| `OUTLINE_RECT` | Outline 影响区域 |
| `SUBTREE_SKIP_RECT` | 子树跳过 |
| `SUBTREE_SKIP_OUT_OF_PARENT_RECT` | 超出父节点范围的子树跳过 |

新增或调整枚举时，要同步检查构造函数中的容器尺寸、边界校验、DFX 绘制和相关测试。

## Buffer Age 和历史合并

### 当前代码语义

`UpdateDirty()` 的顺序是：

1. 按需对齐当前帧脏区，并在首次开启对齐时对齐历史。
2. `PushHistory(currentFrameDirtyRegion_)`。
3. `MergeHistory(bufferAge_, currentFrameDirtyRegion_)`。
4. `MergeAdvancedDirtyHistory(bufferAge_)`。

虽然当前帧先进入历史队列，但 `MergeHistory()` 从 `historyHead_` 的前一项开始读取。
因此历史帧充足时，`bufferAge_ = N` 表示以当前帧为基础，再合并此前 N 帧；不是总共合并 N 帧。

| `bufferAge_` | 当前实现 |
| --- | --- |
| `1` | 合并当前帧和上一帧；还没有上一帧时只得到当前帧 |
| `2` | 历史充足时合并当前帧和此前两帧 |
| `N` | 合并当前帧和最多 `min(N, historySize_ - 1)` 个此前帧 |
| `0` | 视为历史不可用，合入 `surfaceRect_`，结果至少覆盖完整 Surface |
| 大于 `historySize_` | 与 0 类似，普通结果合入完整 Surface |

当 `N == historySize_` 时，循环最后还会读到已入队的当前帧；因为 Region 合并具有幂等性，
不会重复扩大结果。`MergeAdvancedDirtyHistory()` 使用相同的历史索引，但无效 age 时直接把结果设置为
`{surfaceRect_}`，与普通路径的 `currentFrameDirtyRegion_ JOIN surfaceRect_` 写法不完全相同。

`SetBufferAge(age)` 对负数返回 `false`，并把内部值重置为 0。当前 `UpdateDirtyValid` 用例让 age
随历史数量一起增长，没有直接隔离“已有上一帧时 age=1”的语义。修改 Buffer Age 时优先为
`0`、`1`、历史不足、`N == historySize_`、超过历史数量、队列回绕和负数补区分性测试。

### Advanced dirty

advanced dirty 用 `Occlusion::Region` 合并多矩形：

- `AdvancedDirtyRegionType::DISABLED` 时，历史结果退化为一个 bound。
- 启用时，`GetRegionRectIs()` 可以保留多个矩形。
- 当前帧矩形数量超过 `maxNumOfDirtyRects_` 时，会立即退化为当前帧包围矩形。
- `UpdateMaxNumOfDirtyRectByState()` 对 Display 和 Surface 使用不同的启用条件。

不要只修改 `dirtyRegion_` 路径而遗漏 advanced history、对齐和 HWC 合并。

## 对齐、damage 和 Quick Reject

`GetPixelAlignedRect()` 和 `UpdateDirtyByAligned()` 默认按 `ALIGNED_BITS = 32` 调整边界。
是否使用对齐由上层开关和 GPU tile 信息共同决定，不应把 32 当作所有 GPU 的固定 tile 规格。
当前实现使用整数除法计算左、上边界；对负坐标的结果受 C++ 向零截断语义影响，修改对齐逻辑时要
单独覆盖负坐标，不能只依据正坐标用例假设一定向外扩展。

`RSUniDirtyComputeUtil::DIRTY_REGION_COUNT_THRESHOLD` 当前为 1。调用点根据矩形数量、
脏区对齐开关和 GPU tile 有效性选择单矩形裁剪或多矩形处理。修改阈值时要检查：

- `rs_uni_render_util.cpp` 的 damage 生成。
- `rs_screen_render_node_drawable.cpp` 的提交路径。
- `rs_multi_screen_util.cpp` 的多屏路径。
- EGL 左下角坐标转换：`GetDirtyRegionFlipWithinSurface()`。

Quick Reject 的数据流容易误判：

1. `RSDirtyRegionManager::OnSync()` 会把目标管理器的 `dirtyRegionForQuickReject_` 清空。
2. 渲染线程随后在 `RSUniRenderUtil::SetDrawRegionForQuickReject()` 中，根据合并后的 damage
   为 Surface 管理器设置矩形列表。
3. `RSSurfaceRenderNodeDrawable::CalculateVisibleDirtyRegion()` 再读取该列表并与可见区域求交。

因此 Quick Reject 为空时，要同时检查 OnSync 之后的渲染线程生成步骤，而不是只查主线程历史。

### Quick Reject 同步契约

本文负责 Quick Reject 的产生、同步、清理和重建生命周期。修改 visible region 求交、first-level crossnode 或 Offscreen 行为时，再补读`occlusion-culling.md`。

| 操作 | 源管理器 | 目标管理器 | 其他脏区 |
| --- | --- | --- | --- |
| `OnSync(target)` | Quick Reject 不复制 | 原有 Quick Reject 必须清空 | 按现有字段策略复制或保留 |
| 渲染线程重建 | 不参与 | 写入本帧 Quick Reject | 不应被连带修改 |
| 建议新增的独立清理接口 | 不适用 | 只清理 Quick Reject | current-frame、历史、advanced、HWC、UIFirst 不变 |

前两行描述当前实现；当前类没有 Quick Reject 独立清理 API，第三行是新增此类接口时应保持的约束。

修改 Quick Reject 接口或 `OnSync()` 时遵守以下约束：

- 清理操作只能影响 `dirtyRegionForQuickReject_`，不能用全量 `Clear()` 代替。
- `OnSync()` 应清空目标管理器的 Quick Reject，不能把源 Quick Reject 复制到目标，也不能单独清空
  源 Quick Reject。源管理器仍按既有流程执行 `Clear()`。
- 不改变其他同步字段的赋值、清理时机和先后关系。
- 不增加矩形遍历、容器复制或日志。

### Quick Reject 代码与测试锚点

| 目的 | 优先定位 |
| --- | --- |
| getter/setter | `GetDirtyRegionForQuickReject()`、`SetDirtyRegionForQuickReject()` |
| 同步字段策略 | `RSDirtyRegionManager::OnSync()` |
| 渲染线程生产 | `RSUniRenderUtil::SetDrawRegionForQuickReject()` |
| Drawable 消费 | `RSSurfaceRenderNodeDrawable::CalculateVisibleDirtyRegion()` |
| 接口直接测试 | `RSDirtyRegionManagerTest.SetDirtyRegionForQuickReject` |
| 普通 current-frame 同步基线 | `RSDirtyRegionManagerTest.OnSyncCurrentFrameDirtyState001` |
| Quick Reject 的 OnSync 清理 | 当前管理器单测未直接覆盖，修改时应新增或扩展同步测试 |

### 区分性测试输入

验证 `OnSync()` 的“复制、清理、保留”策略时，源和目标应预置不同的非空值，避免默认空值让错误实现通过。

| 测试目的 | 源对象初始值 | 目标对象初始值 | 预期结果 |
| --- | --- | --- | --- |
| 建议新增的独立清理 | 普通 dirty=C，Quick Reject=A | 不适用 | Quick Reject 为空，普通 dirty 仍为 C |
| 同步目标清理 | Quick Reject=A | Quick Reject=B，且 A 不等于 B | 目标 Quick Reject 为空 |
| 保持正常同步 | current-frame dirty=C | current-frame dirty=D | 目标得到 C，其他现有清理语义不变 |
| 空目标保护 | 任意非空状态 | `nullptr` | 源状态不因空目标调用而改变 |

如果新增独立清理接口，至少同时覆盖“目标字段被清空”和“一个代表性非目标字段保持不变”。
如果改动接入 `OnSync()`，优先扩展已有同步测试，保留其中对普通脏区复制与源端清理的断言。

## Filter、HWC 和专用脏区域

### Filter

Filter 脏区由两层状态共同管理：

- `RSDirtyRegionManager` 保存 `cacheableFilterRects_` 和 `RSFilterDirtyCollector`。
- `RSUniFilterDirtyComputeUtil` 生成 `FilterDirtyRegionInfo`，并在 Screen 或 Surface damage
  中处理 Filter 传播。

Filter 可能扩大影响范围。排查时同时检查 Filter 的可见矩形、缓存是否允许部分渲染、
collector 是否经过 `OnSync()`，并按各字段的实际消费点执行 `Clear()`、
`ClearPureCleanFilterDirtyRegion()` 或 `ClearPendingPurgeFilterRegion()`。

### HWC 和 UIFirst

- `MergeHwcDirtyRect()` 按默认类型写入 `hwcDirtyRegion_`，按具体 Surface 类型写入
  `typeHwcDirtyRegion_`；它还会把 HWC 矩形并入 `advancedDirtyRegion_`。
- `uifirstFrameDirtyRegion_` 单独保存 UIFirst 本帧结果，并在 OnSync 后清空源数据。
- Layer partial render 还会使用独立的 `RSDirtyRegionManager`，不能默认所有 dirty 都属于
  Screen 或主 Surface 管理器。

## 多屏和虚拟屏

多屏场景除了普通历史结果，还包含以下状态：

- `MergeDirtyHistoryInVirtual(age)` 计算 `mergedDirtyInVirtualScreen_`。
- `AccumulateVirtualExpandScreenDirtyRegions()` 保存扩展屏跨帧矩形。
- `RSUniDirtyComputeUtil` 负责累积、合并、清理和 skip 判断。
- `RSLogicalDisplayRenderNodeDrawable` 读取镜像源的普通 dirty、HWC dirty 和按类型 HWC dirty，
  再映射到目标坐标空间。

排查镜像或扩展屏问题时，不要只看源屏 `dirtyRegion_`。还要检查 canvas matrix、屏幕旋转、
采样缩放、HWC 类型过滤、累积列表清理和安全屏分支。

## 主线程与渲染线程交接

`RSScreenRenderNode` 和 `RSSurfaceRenderNode` 持有主线程 `dirtyManager_`；对应 Drawable 持有
`syncDirtyManager_`。`RSDirtyRegionManager::OnSync()` 当前会：

- 复制 Surface 范围、当前帧结果、历史合并结果、advanced、HWC、UIFirst 和 `debugRect_`；
  节点级 DFX map 与 `mergedDirtyRegions_` 只在 dirty debug 开启时复制。
- 调用 `filterCollector_.OnSync()`。
- 清空目标的 Quick Reject 列表，等待渲染线程重建。
- 调用源管理器的 `Clear()`；随后再单独清空源端 `uifirstFrameDirtyRegion_`。

`Clear()` 不是“清除管理器全部内容”，主要字段边界如下：

| `Clear()` 当前清理 | `Clear()` 当前保留 |
| --- | --- |
| 普通和 advanced 的当前帧/合并结果 | `dirtyHistory_`、`advancedDirtyHistory_` 及其索引 |
| HWC 和按类型 HWC dirty | Quick Reject、UIFirst 本帧 dirty |
| visited、`mergedDirtyRegions_`、cacheable Filter rect | 虚拟屏合并结果和虚拟扩展累积列表 |
| 节点级 DFX map、DFX target 标记 | `debugRect_`、debug enable 状态 |
| Filter 两个列表和 pending-purge region | `pureCleanFilterDirtyRegion_` |
| `hasUifirstChild_` | Surface/active rect、Buffer Age、partial-render 配置和 offset |

头文件中虽然存在 `isSync_`，当前实现没有用它保护 OnSync。不要据此声称 OnSync 是“原子复制”
或管理器可以被任意线程并发访问；应按调用链确认生产和消费线程的时序约束。

## 基于代码路径的现象定位

以下内容根据当前实现的数据流、分支条件和现有单测反向整理，不代表已确认的线上高频故障。
先用代码和 DFX 确认断点；产品配置、显示效果和性能问题仍需结合设备数据判断。

### 定位前先确定四个维度

1. **管理器归属**：确认对象是 Screen、Surface、cache surface、layer partial render，还是跨屏临时管理器。
2. **处理阶段**：区分当前帧、历史合并后、OnSync 目标、渲染线程重建后和最终 damage。
3. **坐标空间**：区分节点局部、Surface、Screen、EGL 左下角和虚拟屏映射后的坐标。
4. **字段族**：区分普通 dirty、advanced、HWC、UIFirst、Filter、Quick Reject 和 DFX 数据。

不要只打印一个 `RectI` 就下结论。同一帧的这些对象可能有意保存不同阶段、不同坐标或不同粒度的结果。

### 局部区域未刷新

按生产到消费的顺序检查：

- 变化是否写入当前正在使用的管理器。跨屏首层节点可能使用临时 `RSDirtyRegionManager`，
  layer partial render 也有独立实例。
- 输入矩形是否为空或无效；`MergeDirtyRect()` 对空矩形直接返回。
- 输入是否已经转换到管理器的 Surface 或 Screen 坐标，offset、阴影和 cache surface 原点是否一致。
  当前 `SetOffset()` 只写 `offsetX_`/`offsetY_`，不更新 `hasOffset_`；`HasOffset()` 返回 `false`
  不能证明 offset 没有生效，应直接检查 `GetOffsetedDirtyRegion()` 的结果。
- `IntersectDirtyRect()` 或 `ClipDirtyRectWithinSurface()` 是否把结果裁空；普通包围矩形优先使用
  `activeSurfaceRect_`，advanced rect 当前使用 `surfaceRect_` 裁剪。
- Filter、阴影、outline、子节点移除和超出父范围的旧区域是否同时合入。
- 是否先完成 `UpdateDirty()`，再执行 `OnSync()`；Drawable 是否读取对应的 `syncDirtyManager_`。
- 最终漏刷发生在普通 dirty、advanced damage，还是 `dirty AND visible` 的交点。

### 刷新晚一帧、隔帧变化或结果抖动

优先检查生命周期和历史索引：

- 当前帧顺序应是生产 current-frame dirty、`UpdateDirty()`、`OnSync()`，然后由渲染线程生成 damage
  和 Quick Reject。把 OnSync 提前会把旧结果交给 Drawable。
- `UpdateDirty()` 会先把当前帧压入历史，再按 Buffer Age 合并；不要按“历史不包含当前帧”计算索引。
- OnSync 后源对象的当前帧和普通合并结果被 `Clear()` 清空，但历史队列、Quick Reject 等字段仍保留。
  此时只检查源对象的某一个字段不能证明同步成功或失败，应按上面的字段边界同时检查目标对象。
- `surfaceRect_`、`activeSurfaceRect_` 或 partial render 开关在相邻帧变化时，确认上层是否选择全量兜底。
- 如果只有 UIFirst、HWC 或 Filter 抖动，先按各自生命周期检查，不要用普通 dirty 的清理规则套用。

### 意外全屏刷新或 damage 明显过大

优先检查：

- `bufferAge_` 为 0 或大于 `historySize_` 时，`MergeHistory()` 会合入 `surfaceRect_`；负数输入会被
  `SetBufferAge()` 重置为 0，并返回 `false`。
- 刚建立历史或 Buffer Age 跳变时，当前历史数量是否足以满足请求。
- `surfaceRect_` 和 `activeSurfaceRect_` 是否正确，是否误调用 `ResetDirtyAsSurfaceSize()`、
  `MergeSurfaceRect()` 或其他全量兜底入口。
- 上层调用 `SetPartialRenderEnabled()` 造成状态变化时，是否通过 `GetEnabledChanged()` 触发
  `ResetDisplayDirtyRegion()` 的全量兜底。Filter 的
  `forceDisablePartialRender_` 会禁止对应 Filter 使用缓存部分渲染，并在相交时合入 Filter dirty，
  不等同于直接设置全屏 dirty。
- advanced dirty 矩形数量是否超过 `maxNumOfDirtyRects_`，从而退化为一个较大的包围矩形。
- GPU tile 对齐是否扩大了当前帧和既有历史；首次开启对齐会执行 `AlignHistory()`。

### 普通 dirty 正确但 advanced damage 不正确

两条路径不是所有接口都同步更新：

- `MergeDirtyRect()` 同时更新普通 current-frame 和 advanced current-frame。
- `MergeDirtyRectIfIntersect()` 当前只扩展普通包围矩形，不追加 advanced rect。
- `MergeDirtyRectAfterMergeHistory()` 当前只扩展历史合并后的 `dirtyRegion_`。
- `MergeHwcDirtyRect()` 会更新 HWC 字段，并把矩形合入 `advancedDirtyRegion_`。
- `AdvancedDirtyRegionType::DISABLED`、矩形数量上限和无效 Buffer Age 都可能让多矩形退化。
- `ClipDirtyRectWithinSurface()` 对普通包围矩形和 advanced rect 使用的裁剪范围当前不完全相同。

修改其中一个接口时，先确认这种字段差异是调用契约还是遗漏，再决定是否让两条路径保持一致。

### damage、Quick Reject 或 visible dirty 为空

沿三个对象分别检查：

- **damage**：`UpdateDirty()` 后的普通或 advanced 历史结果是否为空，flip、旋转、采样缩放和 tile
  对齐后的矩形是否仍有效。
- **Quick Reject**：OnSync 会主动清空目标管理器的旧列表；清空后到
  `RSUniRenderUtil::SetDrawRegionForQuickReject()` 重建前为空是预期中间状态。
- **visible dirty**：普通节点还要计算 `Quick Reject dirty AND visibleRegion`；继续按
  `occlusion-culling.md` 的 Surface 类型、cross node 和 Offscreen 分支检查。

`SetDrawRegionForQuickReject()` 只给有效的主窗口和 Leash Window Drawable 写入列表，并跳过空 params
或空管理器。排查时确认当前检查对象实际满足这些条件。OnSync 不复制源 Quick Reject；渲染侧读取的是
目标管理器，因此源对象保留的旧值也不能证明本帧重建成功。

### Quick Reject 残留上一帧或各 Surface 的 visible dirty 同时异常

优先检查：

- 本帧是否执行了 OnSync，把目标旧列表清空。
- 渲染线程传给 `SetDrawRegionForQuickReject()` 的 `mergedDirtyRects` 是否属于当前屏幕和当前帧。
- `allSurfaceDrawables` 是否属于当前屏幕，是否混入旧 Drawable，或漏掉当前主窗口/Leash Window。
- 调用之后是否有代码再次覆盖目标管理器，或错误读取了主线程源管理器。

当前实现会把同一屏幕的 `mergedDirtyRects` 写入列表中的所有有效主窗口和 Leash Window；不同 Surface
得到相同 Quick Reject 列表本身不是错误，Surface 差异在后续与各自 visible region 求交时产生。

不要用全量 `Clear()` 修复 Quick Reject 残留；它当前不清理 Quick Reject，却会清理普通、advanced、
HWC、Filter 和 DFX 等其他状态。

### UIFirst 本帧脏区丢失或残留

当前实现需要同时观察源和目标：

- `OnSync()` 先把 `uifirstFrameDirtyRegion_` 复制到目标，再在源端单独清空。
- `RSDirtyRegionManager::Clear()` 当前不负责清空 UIFirst 字段；不要把单独调用 `Clear()` 等同于
  UIFirst 生命周期结束。
- OnSync 后源为空、目标非空是正常结果；同步前源为空则应回溯 `SetUifirstFrameDirtyRect()` 的生产点。
- `hasUifirstChild_` 属于 layer partial render 状态，`Clear()` 会把它恢复为 `false`，不要与
  `uifirstFrameDirtyRegion_` 混为同一字段。

### Filter 区域漏合、过度扩大或缓存状态异常

`RSFilterDirtyCollector` 内的字段不是统一同步、统一清理：

- `CollectFilterDirtyRegionInfo(info, syncToRT)` 根据参数写入不同列表；只有需要渲染线程消费的
  `pureCleanFilters_` 由 `OnSync()` 复制到目标。
- `filtersWithBelowDirty_`、`pendingPurgeFilterRegion_` 和 `pureCleanFilterDirtyRegion_` 有不同用途，
  不能通过检查任意一个字段代表全部 Filter 状态。
- collector 的 `Clear()` 会清理两个列表和 pending-purge region；`pureCleanFilterDirtyRegion_`
  当前由 `ClearPureCleanFilterDirtyRegion()` 独立清理。
- 同时检查 `cacheableFilterRects_`、`isFilterCacheRectValid_`、可见 Filter rect 和
  `forceDisablePartialRender_`。

Filter 问题应沿“主线程收集、OnSync 字段选择、渲染线程 damage 扩大、缓存有效性恢复”四段定位。

### HWC、虚拟屏或镜像屏残影

优先检查：

- 默认类型 HWC dirty 写入 `hwcDirtyRegion_`，其他 Surface 类型写入 `typeHwcDirtyRegion_`；消费端
  是否读取了正确类型并同时处理必要的 advanced dirty。
- 源屏历史是否用正确 Buffer Age 合并，镜像目标是否读取了正确源 Drawable 的同步管理器。
- canvas matrix、屏幕旋转、采样缩放和平移是否与目标坐标一致。
- 安全屏、skip layer、镜像和虚拟扩展是否选择了正确的 dirty/visible 分支。
- `virtualExpandScreenAccumulatedDirtyRegions_` 是否过早清除或长期未清除；该列表使用独立清理接口，
  不由普通 `Clear()` 负责。

### DFX 与实际 damage 不一致

节点级 `DirtyRegionType`、Display 的 `mergedDirtyRegions_`、普通历史结果、advanced dirty、Quick Reject
和最终 EGL damage 不是同一份数据。先确认 DFX 模式展示的对象、坐标和处理阶段，再比较对应生成点。

还要注意：

- `UpdateDirtyRegionInfoForDfx()` 会忽略非法类型和空矩形。
- OnSync 只在 dirty debug 开启时复制节点级 DFX 和 `mergedDirtyRegions_`。
- `IsCurrentFrameDirty()` 会排除只用于调试的 `debugRect_`，因此 DFX 有矩形不代表业务 dirty 非空。

## 验证建议

就近单测目标优先看：

- `RSDirtyRegionManagerTest`
- `RSUniDirtyComputeUtilTest`
- `RSUniRenderUtilDirtyRegionTest`

脏区管理器 fuzz 入口为 `rsdirtyregionmanager_fuzzer`。涉及 HWC、GPU tile、镜像、扩展屏、
Filter 显示效果或性能收益时，还需要真实设备验证；没有设备时只记录本地验证结果和缺口。

## 改动检查清单

- 是否区分当前帧、历史合并、advanced、HWC、UIFirst 和 Quick Reject？
- 是否覆盖 Buffer Age 为 0、1、队列边界、越界和负数？
- 是否同步包围矩形与多矩形路径？
- 是否检查 Surface、Display、cache surface 和 layer partial render 使用的不同管理器？
- 是否检查 Filter 扩大区域和 collector 同步？
- 是否检查虚拟屏、镜像屏、扩展屏以及 HWC dirty 映射？
- 是否检查 OnSync 的复制字段、清理时机和渲染线程重建步骤？
- 修改 Quick Reject 时，是否用不同的源/目标非空值区分“复制”和“清空”？
- 若新增独立清理接口，是否确认至少一个非目标脏区字段保持不变？
- 是否需要 DFX、单测、fuzz 或真实设备验证？
- 是否避免在每帧路径增加大容器复制、全量扫描或高频日志？

## 待补充背景

以下内容需要模块责任人或设备数据补充，不能仅靠静态扫描确定：

- 各产品实际 Buffer Age 来源、buffer queue 策略和常见异常值。
- 不同 GPU 和产品采用的 tile 尺寸、对齐收益及退化边界。
- Filter、HWC、多屏场景的团队常用设备用例和性能基线。
- 历史线上残影、全刷、漏刷问题的典型日志与复现条件。
