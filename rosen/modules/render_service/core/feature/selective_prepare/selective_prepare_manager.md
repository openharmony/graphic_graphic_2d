# SelectivePrepareOpt 设计文档
 
## 1. 概述
 
SelectivePrepareOpt 是 RS 渲染管线中的低负载优化方案。当只有少量节点做动效时，跳过整棵树的 QuickPrepare 遍历，只对这些节点做小范围 prepare，降低 CPU 底噪。
 
## 2. 激活条件
 
优化仅在以下所有条件同时满足时激活：
 
| # | 条件 | 说明 | 闭环风险 |
|---|------|------|----------|
| 1 | `persist.rosen.graphic.selective_prepare_opt=true` | 功能开关 | — |
| 2 | `hasGpuSurfaceDirty_ == false` | 无 GPU 路径 Surface buffer 更新 | R7, R9, R10, R14 |
| 3 | `hasCommandInFrame_ == false` | 当前帧无事务命令 | R8, R13 |
| 4 | `onTreeAnimatingCount == 1` | 恰好 1 个在树上的动效节点 | R15 |
| 5 | `activeNodeCount == onTreeAnimatingCount` | 所有活跃节点均为该动效节点 | — |
| 6 | 动效节点的 surface 名包含 "aweme" | 白名单过滤 | R11, R12 |
| 7 | 动画仅为旋转属性 | `IsRotationOnlyAnimation()` 检查 ROTATION/ROTATION_X/ROTATION_Y/QUATERNION | R18 |
| 8 | Surface 级聚合标志通过 | `CheckSurfaceFilterAndLight` + `surfaceNode->GetGlobalAlpha()` O(1) 检查 | R1, R3, R4 |
| 9 | 祖先链无 NodeGroup 且 alpha == 1.0 | `CheckAncestorStateToDisplay` 单次遍历到 logicalDisplayNode | R1, R3 |
| 10 | 祖先链圆角由快速路径处理 | `UpdateCurCornerInfo(parent->GetGlobalCornerRadius())` 传播祖先圆角 | R5, R17 |
| 11 | 动画为无限循环 (repeatCount == -1) | `IsSubtreeShallow()` 检查 | — |
| 12 | 子树深度 ≤ 2 且无分叉 | `IsSubtreeShallow()` 检查 | R2 |
| 13 | 动效节点子树全部为 Canvas 节点 | `IsCanvasOnlySubtree()` 检查 CANVAS_NODE / CANVAS_DRAWING_NODE | R2 |
 
## 3. 架构
 
### 3.1 类图
 
```
RSMainThread
  ├── unique_ptr<RSSelectivePrepareManager> selectivePrepareManager_
  │
  │  委托调用:
  │  ├── CheckAndSetup()            ← 主管线 (OnVsync)
  │  ├── PrepareOptNodes()          ← UniRender
  │  ├── SetHasGpuSurfaceDirty()    ← CollectInfoForHardwareComposer
  │  └── SetHasCommandInFrame()     ← ProcessCommand*
  │
  └── 读取 DFX 状态:
      ├── IsActive()                ← Animate 日志
      └── GetHitCount()             ← Animate 能耗统计
```
 
### 3.2 RSSelectivePrepareManager
 
文件位置：`rosen/modules/render_service/core/feature/selective_prepare/`
 
**成员变量：**
 
| 变量 | 类型 | 说明 |
|------|------|------|
| `context_` | `weak_ptr<RSContext>` | 访问 animatingNodeList_ 等 |
| `selectivePrepareOptActive_` | `bool` | 优化是否激活 |
| `selectivePrepareOptNodes_` | `vector<weak_ptr<RSRenderNode>>` | 当前帧的动效节点 |
| `selectivePrepareOptHitCount_` | `uint32_t` | 累计命中次数 |
| `hasGpuSurfaceDirty_` | `bool` | GPU Surface 脏标记 |
| `hasCommandInFrame_` | `bool` | 帧内命令标记 |
| `pendingActivation_` | `bool` | 两帧激活：O(1) 条件首帧通过时标记，次帧检查缓存后激活 |
 
**公开接口：**
 
| 方法 | 说明 |
|------|------|
| `CheckAndSetup()` | 每帧检查激活条件，设置激活状态 |
| `PrepareOptNodes()` | 执行快速路径 prepare，返回 true 表示走了快速路径 |
| `SetHasGpuSurfaceDirty(bool)` | 由 CollectInfoForHardwareComposer 设置 |
| `SetHasCommandInFrame(bool)` | 由 ProcessCommand 设置 |
| `ResetState()` | 重置所有状态 |
| `IsActive()` | 优化是否激活（DFX 用） |
| `HasGpuSurfaceDirty()` | GPU 脏标记（DFX 用） |
| `GetHitCount()` | 累计命中次数（DFX 用） |
| `LogCommandInfo(transactionData)` | DFX：ProcessCommand 后的命令计数与节点 ID 日志 |
| `LogHwcBufferUpdate(surfaceNode, bufferConsumed)` | DFX：HWC buffer 更新追踪日志 |
| `LogAnimatingNodes()` | DFX：Animate 中的动效节点详情日志 |
| `ReportEnergyStats(energy)` | DFX：能耗统计命中次数上报 |
 
**私有方法：**
 
| 方法 | 说明 |
|------|------|
| `SelectivePrepareFastPath(node)` | 单节点快速 prepare（6 步） |
| `CheckSurfaceEligibility(optNode, surfaceNode, debugEnabled)` | 资格检查入口：surface 级 O(1) + 单次祖先遍历 |
| `CheckSurfaceFilterAndLight(surfaceNode, debugEnabled)` | R3/R4：Surface 级 filter/effect/PointLight 聚合标志检查 |
| `CheckAncestorStateToDisplay(optNode, logicalDisplayNodeId)` | R1+R3：单次遍历到 logicalDisplayNode，检查 NodeGroup + 中间 alpha |
| `HandleAlreadyActive(optNode, surfaceNode, debugEnabled)` | 已激活帧的安全网：重检 surface alpha |
| `IsSubtreeShallow(node)` | 检查动画无限循环 + 子树深度≤2 + 无分叉 |
| `IsCanvasOnlySubtree(node)` | 检查子树全部为 CANVAS_NODE 或 CANVAS_DRAWING_NODE |
| `IsRotationOnlyAnimation(node)` | 检查所有运行中的动画是否仅修改旋转属性 |
| `IsRotationProperty(node, propertyId)` | 通过遍历 TRANSFORM modifier 判断是否为旋转属性 |
 
**内部结构体：**
 
| 结构体 | 字段 | 说明 |
|--------|------|------|
| `AncestorCheckResult` | `hasNodeGroup` | 祖先链是否有 NodeGroup（模糊缓存） |
| | `hasAlphaNotOne` | 祖先链中间节点是否有 alpha != 1.0 |
| | `traverseCount` | 遍历的节点数量（DFX trace 用） |
 
## 4. 管线集成
 
### 4.1 帧流程
 
```
OnVsync
  ├── Animate()                          ← DFX: LogAnimatingNodes(), ReportEnergyStats()
  ├── CollectInfoForHardwareComposer()    ← SetHasGpuSurfaceDirty(), LogHwcBufferUpdate()
  ├── CheckAndSetup()                     ← 检查激活条件
  └── UniRender()
       ├── PrepareOptNodes()              ← 快速路径（激活时）
       │   ├── Clear surface/screen dirtyManager
       │   ├── SetAdvancedDirtyRegionType + SetMaxNumOfDirtyRects
       │   ├── SelectivePrepareFastPath() × N
       │   ├── Surface → Screen 脏区传播
       │   └── AddToPendingSyncList
       └── rootNode->QuickPrepare()       ← 正常路径（未激活时）
```
 
### 4.2 RSMainThread 调用点
 
| 调用位置 | 调用 | 说明 |
|----------|------|------|
| 构造函数 | `selectivePrepareManager_ = make_unique<>(context_)` | 初始化 |
| 主管线 OnVsync | `CheckAndSetup()` | 检查激活条件 |
| UniRender | `!PrepareOptNodes()` | 快速路径或正常路径 |
| ProcessCommandForUniRender | `SetHasCommandInFrame(false/true)` + `LogCommandInfo()` | 状态设置 + DFX |
| ProcessCommandForDividedRender | `SetHasCommandInFrame(false/true)` | 状态设置 |
| CollectInfoForHardwareComposer | `SetHasGpuSurfaceDirty(false/true)` + `LogHwcBufferUpdate()` | 状态设置 + DFX |
| Animate | `LogAnimatingNodes()` + `ReportEnergyStats()` | DFX |
 
**侵入性分析**：RSMainThread 中与 SelectivePrepareOpt 相关的代码均为简洁的单行调用，无内联 DFX 逻辑。所有 DFX 逻辑封装在 RSSelectivePrepareManager 中，由调试/功能开关控制。
 
## 5. 资格检查架构
 
### 5.1 整体结构
 
资格检查分为两层，职责清晰分离：
 
```
CheckSurfaceEligibility(optNode, surfaceNode)
  │
  ├── Layer 1: Surface 级 O(1) 聚合检查
  │   ├── CheckSurfaceFilterAndLight(surfaceNode)
  │   │   ├── ChildHasVisibleFilter()        ← 子树 filter 聚合
  │   │   ├── ChildHasVisibleEffect()        ← 子树 effect 聚合
  │   │   ├── GetFilter() / GetBackgroundFilter()  ← surface 自身 filter
  │   │   └── GetChildHasVisibleIlluminated() ← PointLight 聚合
  │   └── surfaceNode->GetGlobalAlpha()      ← root→surface alpha 乘积
  │
  └── Layer 2: 单次祖先遍历 CheckAncestorStateToDisplay(optNode, logicalDisplayNodeId)
      ├── NodeGroup (blur cache)             ← GROUPED_BY_FOREGROUND_FILTER 等
      └── Intermediate alpha                 ← 容器节点 alpha != 1.0
```
 
### 5.2 两层检查的职责划分
 
| 检查项 | 检查层 | 检查方式 | 原因 |
|--------|--------|----------|------|
| 子树 filter/effect | Layer 1 (Surface) | `ChildHasVisibleFilter/Effect` 聚合标志 O(1) | QuickPrepare 向上传播聚合标志，surface 节点即可覆盖整棵子树 |
| Surface 自身 filter | Layer 1 (Surface) | `GetFilter()/GetBackgroundFilter()` O(1) | 直接读 surface 属性 |
| PointLight | Layer 1 (Surface) | `GetChildHasVisibleIlluminated()` O(1) | PointLight 管理器聚合标志 |
| Root→Surface alpha | Layer 1 (Surface) | `GetGlobalAlpha()` O(1) | QuickPrepare 已累积所有祖先 alpha 之积 |
| NodeGroup (模糊缓存) | Layer 2 (遍历) | `GetNodeGroupType()` | 容器 CanvasNode 在 SurfaceNode 之上，可能标有 GROUPED_BY_FOREGROUND_FILTER |
| 中间节点 alpha | Layer 2 (遍历) | `GetAlpha()` | 容器 CanvasNode 可能有非 1.0 alpha，聚合标志无法覆盖 |
 
### 5.3 遍历终点：logicalDisplayNode
 
遍历从 optNode 向上到 logicalDisplayNodeId（不包含），覆盖 SurfaceNode 之上的容器 CanvasNode：
 
```
ScreenNode
  └── DisplayNode (logicalDisplayNode, 遍历终点)
        └── ... (ancestor nodes)
              └── CanvasNode (容器节点，可能带 NodeGroup 或 alpha != 1.0)
                    └── SurfaceNode (Layer 1 检查在此完成)
                          └── ... (intermediate nodes)
                                └── optNode (遍历起点)
```
 
**为什么不能停在 SurfaceNode：** SurfaceNode 之上可能存在容器 CanvasNode，这些容器节点可能标有：
- `GROUPED_BY_FOREGROUND_FILTER`（模糊缓存），导致渲染需要 blur cache
- `alpha != 1.0`，导致快速路径的 alpha 传播不正确
 
**为什么不需要遍历到 ScreenNode：** logicalDisplayNode 是 Display 节点，其上方为 Screen 节点。Screen 和 Display 节点不会携带模糊缓存或非 1.0 alpha，无需检查。
 
### 5.4 CheckAncestorStateToDisplay 实现
 
单次遍历同时检查 NodeGroup 和 alpha，避免多次遍历的开销：
 
```
AncestorCheckResult CheckAncestorStateToDisplay(optNode, logicalDisplayNodeId):
  result = {hasNodeGroup=false, hasAlphaNotOne=false, traverseCount=0}
  current = optNode
  while current != null && current.id != logicalDisplayNodeId:
    if !result.hasNodeGroup && current.GetNodeGroupType() != NONE:
      result.hasNodeGroup = true
    if !result.hasAlphaNotOne && current != optNode && current.GetAlpha() != 1.0:
      result.hasAlphaNotOne = true
    current = current.GetParent()
    result.traverseCount++
  return result
```
 
**关键细节：**
- 跳过 optNode 自身的 alpha 检查（`current != optNode`）：动效节点自身的 alpha 在动画中变化，不应作为拒绝条件
- 跳过 optNode 自身的 NodeGroup 检查同理：动效节点的 NodeGroup 是正常状态
- `traverseCount` 用于 RS_TRACE 输出，便于性能分析
- 早期终止：`hasNodeGroup` 和 `hasAlphaNotOne` 均为 true 时仍继续遍历以记录 traverseCount
 
## 6. 快速路径（SelectivePrepareFastPath）
 
6 步单节点 prepare，对齐正常路径 QuickPrepare 的关键步骤：
 
| 步骤 | 操作 | 对应正常路径 | 闭环风险 |
|------|------|-------------|----------|
| 1 | ApplyModifiers | QuickPrepare::ApplyModifiers | — |
| 2 | UpdateGeometryByParent + UpdateSelfDrawRect | QuickPrepareChildren 内部 | — |
| 3 | Compute absDrawRect (MapRectWithoutRounding + InflateToRectI) | NodePrepare 内部 | — |
| 4 | Dirty region: oldDirty → newDirty → childrenClip | UpdateDirtyRegion | — |
| 5 | NodePostPrepare 子集 | NodePostPrepare | R2, R16, R17 |
| 6 | UpdateRenderParams | UpdateRenderParams | — |
 
步骤 5 的详细操作：
 
| 操作 | 闭环风险 | 说明 |
|------|----------|------|
| `parentNode->ResetChildRelevantFlags()` | R2 | 防止 childrenRect_ 累积膨胀 |
| `stagingParams->SetAlpha(GetAlpha())` | R16 | 动画 alpha 同步到 stagingRenderParams |
| `UpdateCurCornerInfo(parent->GetGlobalCornerRadius(), curCornerRect)` | R5, R17 | 用缓存值传播祖先圆角，更新 globalCornerRadius_ |
| `MapAndUpdateChildrenRect()` | — | 重建 childrenRect_ |
| `UpdateSubTreeInfo()` | — | 更新子树信息 |
| `UpdateLocalDrawRect()` | — | 更新局部绘制区域 |
| `SetAbsDrawRect()` | — | 同步到 stagingRenderParams |
| `SetOldDirty() / SetOldDirtyInSurface()` | — | 保存旧脏区供下帧使用 |
| `ResetChangeState()` | — | 重置变更状态 |
 
## 7. 功能开关
 
| 开关 | 参数 | 默认值 | 说明 |
|------|------|--------|------|
| 功能开关 | `persist.rosen.graphic.selective_prepare_opt` | true | 控制优化启用 |
| 调试开关 | `persist.graphic.graphic.selective_prepare_opt_debug` | false | 控制 DFX 日志输出 |
 
## 8. 白名单
 
当前仅对 surface 名包含 "aweme" 的动效节点生效（覆盖 aweme0、aweme1 等）。白名单检查位于 `CheckNodeEligibility()` 中，在动画类型检查前执行。
 
## 9. 已知限制
 
- 仅支持单个动效节点（`onTreeAnimatingCount == 1`）
- 两个节点的脏区若几何重叠，会合并为一个大 bounding box（与正常路径行为一致）
- HWC buffer 更新通过 `hasGpuSurfaceDirty_` 检测，但 HWC→HWC 路径的普通 buffer 更新不触发 SetContentDirty，需依赖 isHardwareForcedDisabled_ 分支中的额外检测
 
## 10. 性能优化：两帧激活 + 分层资格检查
 
### 10.1 优化原理
 
当 `hasCommandInFrame_ == false` 且 `hasGpuSurfaceDirty_ == false` 时，祖先链属性不会变化。上一帧 QuickPrepare 已将聚合值写入缓存（`globalAlpha_`、`ChildHasVisibleFilter_` 等），可直接 O(1) 读取。无聚合标志覆盖的属性（容器 CanvasNode 的 NodeGroup 和 alpha）则通过短距离遍历检查。
 
### 10.2 两帧激活流程
 
```
Frame N:   O(1) 条件通过 → pendingActivation_ = true → 不激活 → QuickPrepare 运行
                                              ↑ QuickPrepare 更新 globalAlpha_、
                                                ChildHasVisibleFilter 等聚合标志
Frame N+1: O(1) 条件通过 → CheckSurfaceEligibility (O(1) + O(depth)) → 激活
Frame N+2: O(1) 条件通过 → selectivePrepareOptActive_==true → HandleAlreadyActive → O(1)
```
 
**关键设计：** 当 `selectivePrepareOptActive_==true` 时，不再执行 CheckSurfaceEligibility 和 IsSubtreeShallow，仅做 HandleAlreadyActive 安全网检查（surface alpha）。因为 O(1) 条件（hasGpuSurfaceDirty_、hasCommandInFrame_）不变即意味着缓存属性值和祖先链状态仍然有效。若 O(1) 条件变化，优化立即退出。
 
### 10.3 分层检查性能分析
 
| 场景 | Layer 1 (Surface O(1)) | Layer 2 (遍历) | 总开销 |
|------|----------------------|-----------------|--------|
| O(1) 条件不满足 | 不执行 | 不执行 | O(1) |
| Surface 级检查拒绝 | 执行，O(1) | 不执行 | O(1) |
| 祖先链检查拒绝 | 执行，O(1) | 执行，O(depth) | O(depth) |
| 全部通过 | 执行，O(1) | 执行，O(depth) | O(depth) |
| 持续激活帧 | 不执行 | 不执行 | O(1) |
 
**核心收益：**
- 不进入方案场景的底噪开销：O(1)（仅 CheckFastFailConditions）
- Surface 级拒绝：O(1)（聚合标志直接命中，无需遍历）
- 持续激活帧：O(1)（仅 HandleAlreadyActive 检查 surface alpha）
 
### 10.4 R5 祖先圆角处理策略
 
R5（GlobalCornerRadius 未传播）**不通过激活条件排除**，而是通过**代码修复**闭环：
 
- 快速路径步骤 5 调用 `UpdateCurCornerInfo(parent->GetGlobalCornerRadius(), curCornerRect)`
- `parent->GetGlobalCornerRadius()` 返回上帧 QuickPrepare 写入的缓存值（component-wise max 累积）
- `UpdateCurCornerInfo` 计算 `globalCornerRadius_ = max(selfCR, parentCR)`，与正常路径一致
- `globalCornerRect_` 虽可能不精确，但不被 render 阶段消费，不影响渲染
- `hasGlobalCorner_` 由 `UpdateRenderParams` 根据 `globalCornerRadius_.IsZero()` 设置，正确反映当前状态
 
**前提条件：** `hasCommandInFrame_==false` 保证父节点缓存在当前帧不变，值与上帧 QuickPrepare 写入时一致。
 
## 11. 诊断日志
 
所有 RS_LOGI 使用 `%{public}` 格式符确保在 release 构建中可见。日志由调试开关 `persist.graphic.graphic.selective_prepare_opt_debug` 控制（surfaceEligibility 拒绝日志始终输出）。
 
| 日志 | 开关控制 | 说明 |
|------|----------|------|
| `SelectivePrepareOpt: disabled by feature switch` | debug | 功能开关关闭 |
| `SelectivePrepareOpt: reject [gpuDirty=... cmdInFrame=...]` | debug | GPU脏/有命令 |
| `SelectivePrepareOpt: reject [onTreeAnimatingCount=...]` | debug | 动效节点数≠1 |
| `SelectivePrepareOpt: reject [activeNodeCount=...]` | debug | 活跃节点数不匹配 |
| `SelectivePrepareOpt: reject [surface=... not in aweme whitelist]` | debug | 不在白名单 |
| `SelectivePrepareOpt: reject [notRotationOnly]` | debug | 动画非纯旋转 |
| `SelectivePrepareOpt: pending activation [...]` | debug | 两帧激活：首帧通过 |
| `SelectivePrepareOpt: reject [surfaceEligibility] ...` | **始终** | Surface 级或祖先链检查拒绝，含详细属性值 |
| `SelectivePrepareOpt: reject [surfaceChildHasFilter]` | debug | 子树有 filter |
| `SelectivePrepareOpt: reject [surfaceChildHasEffect]` | debug | 子树有 effect |
| `SelectivePrepareOpt: reject [surfaceHasFilter]` | debug | Surface 自身有 filter |
| `SelectivePrepareOpt: reject [surfaceHasBgFilter]` | debug | Surface 自身有背景 filter |
| `SelectivePrepareOpt: reject [surfaceHasPointLight]` | debug | 子树有 PointLight |
| `SelectivePrepareOpt: reject [surfaceAlphaNot1]` | debug | Surface globalAlpha != 1.0 |
| `SelectivePrepareOpt: reject [hasNodeGroup]` | debug | 祖先链有 NodeGroup |
| `SelectivePrepareOpt: reject [intermediateAlphaNot1]` | debug | 祖先链中间 alpha != 1.0 |
| `SelectivePrepareOpt: reject [subtreeNotShallow]` | debug | 子树不浅 |
| `SelectivePrepareOpt: reject [subtreeNotCanvasOnly]` | debug | 子树含非 Canvas 节点 |
| `SelectivePrepareOpt: activated [...]` | debug | 激活成功 |
| `SelectivePrepareOpt: deactivated [surfaceAlpha=...]` | debug | 已激活帧 surface alpha 安全网触发 |
| `SelectivePrepareFastPath` | debug | 快速路径执行 |
| `SelectivePrepareOpt ancestorCheck: traversed=... nodeGroup=... alphaNot1=...` | **始终(trace)** | 祖先遍历深度与结果 |
 
## 12. 测试用例
 
测试文件：`rosen/test/render_service/render_service/unittest/feature/selective_prepare/rs_selective_prepare_manager_test.cpp`
 
### 12.1 快速失败条件
 
| 用例 | 树结构 | 预期结果 | 验证点 |
|------|--------|----------|--------|
| FeatureDisabled | — | 不激活 | `IsActive() == false` |
| GpuDirty | 标准树 + `hasGpuSurfaceDirty_=true` | 不激活 | `IsActive() == false`, `pendingActivation_==false` |
| CommandInFrame | 标准树 + `hasCommandInFrame_=true` | 不激活 | `IsActive() == false`, `pendingActivation_==false` |
 
### 12.2 动画节点计数
 
| 用例 | 树结构 | 预期结果 | 验证点 |
|------|--------|----------|--------|
| MultipleAnimatingNodes | 2 个在树动效节点 | 不激活 | `pendingActivation_==false` |
| ZeroAnimatingNodes | 无动效节点 | 不激活 | `pendingActivation_==false` |
 
### 12.3 Surface 级资格检查
 
| 用例 | 树结构 | 预期结果 | 验证点 |
|------|--------|----------|--------|
| SurfaceNull | optNode 无 surface | 不激活 | `IsActive()==false` |
| SurfaceChildHasFilter | `ChildHasVisibleFilter=true` | 第 2 帧拒绝 | `pending_==true` → `IsActive()==false` |
| SurfaceAlphaNotOne | `globalAlpha!=1.0` | 第 2 帧拒绝 | `pending_==true` → `IsActive()==false` |
| SurfaceHasBackgroundFilter | `ChildHasVisibleEffect=true` | 第 2 帧拒绝 | `pending_==true` → `IsActive()==false` |
 
### 12.4 祖先链遍历（CheckAncestorStateToDisplay）
 
| 用例 | 树结构 | 预期结果 | 验证点 |
|------|--------|----------|--------|
| ContainerNodeGroup | 容器 CanvasNode 标 GROUPED_BY_FOREGROUND_FILTER | 第 2 帧拒绝 | `hasNodeGroup==true` |
| ContainerAlphaNotOne | 容器 CanvasNode alpha=0.5 | 第 2 帧拒绝 | `hasAlphaNotOne==true` |
| TraverseCount | 标准树（optNode→surface→container→display） | `traverseCount==3` | 正确遍历 3 层 |
| SkipsOptNodeOwnAlpha | optNode alpha=0.3 | 不影响 | `hasAlphaNotOne==false` |
 
### 12.5 Canvas-Only 子树（IsCanvasOnlySubtree）
 
| 用例 | 树结构 | 预期结果 | 验证点 |
|------|--------|----------|--------|
| CanvasNodeOnly | 标准树（全部 Canvas） | 通过 | `IsCanvasOnlySubtree()==true` |
| ContainsSurfaceNode | optNode 子树含 SurfaceNode | 拒绝 | `IsCanvasOnlySubtree()==false` |
| EmptySubtree | Canvas 无子节点 | 通过 | `IsCanvasOnlySubtree()==true` |
| NonCanvasRoot | 根节点为 SurfaceNode | 拒绝 | `IsCanvasOnlySubtree()==false` |
| OptNodeNotCanvas | 动效节点为 SurfaceNode | 第 2 帧拒绝 | `IsActive()==false` |
 
### 12.6 子树深度与分支
 
| 用例 | 树结构 | 预期结果 | 验证点 |
|------|--------|----------|--------|
| DeepSubtree | 深度 > MAX_SUBTREE_DEPTH(2) | 第 2 帧拒绝 | `IsActive()==false` |
| BranchingSubtree | optNode 有多个子节点 | 第 2 帧拒绝 | `IsActive()==false` |
| NonInfiniteAnimation | repeatCount != -1 | `IsSubtreeShallow()==false` | 有限动画被排除 |
 
### 12.7 两帧激活流程
 
| 用例 | 操作序列 | 预期结果 |
|------|----------|----------|
| FirstFramePending | CheckAndSetup ×1 | `pending_==true`, `IsActive()==false` |
| SecondFrameActivates | CheckAndSetup ×2 | `IsActive()==true`, `hitCount==1` |
| ThirdFrameStaysActive | CheckAndSetup ×3 | `IsActive()==true`, `hitCount==2` |
| PendingResetByGpuDirty | pending 后 gpuDirty=true | `pending_==false` |
 
### 12.8 已激活帧安全网
 
| 用例 | 操作序列 | 预期结果 |
|------|----------|----------|
| SurfaceAlphaSafetyNet | 激活后 surface alpha 变化 | `IsActive()==false` |
| ActiveDeactivatesOnCommand | 激活后 hasCommandInFrame=true | `IsActive()==false` |