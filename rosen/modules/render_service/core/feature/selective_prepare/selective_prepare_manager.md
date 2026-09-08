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
| 8 | 祖先链 alpha == 1.0 | `CheckAncestorStateFromCache()` O(1) 读取 parent 缓存，`!= 1.0f` 拒绝 | R1 |
| 9 | 祖先链圆角由快速路径处理 | `UpdateCurCornerInfo(parent->GetGlobalCornerRadius())` 传播祖先圆角 | R5, R17 |
| 10 | 父节点无 filter/effect | `CheckAncestorStateFromCache()` O(1) 检查 | R3, R4 |
| 11 | 动画为无限循环 (repeatCount == -1) | `IsSubtreeShallow()` 检查 | — |
| 12 | 子树深度 ≤ 2 且无分叉 | `IsSubtreeShallow()` 检查 | R2 |

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
| `CheckAncestorStateFromCache(node)` | R1+R3+R4：O(1) 读取 parent 缓存（globalAlpha_, filter 标志） |
| `IsSubtreeShallow(node)` | 检查动画无限循环 + 子树深度≤2 + 无分叉 |
| `IsRotationOnlyAnimation(node)` | 检查所有运行中的动画是否仅修改旋转属性（ROTATION/ROTATION_X/ROTATION_Y/QUATERNION） |
| `IsRotationProperty(node, propertyId)` | 通过遍历 TRANSFORM modifier 查找 PropertyId 对应的 RSPropertyType，判断是否为旋转属性 |

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

## 5. 快速路径（SelectivePrepareFastPath）

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

## 6. 功能开关

| 开关 | 参数 | 默认值 | 说明 |
|------|------|--------|------|
| 功能开关 | `persist.rosen.graphic.selective_prepare_opt` | true | 控制优化启用 |
| 调试开关 | `persist.graphic.selective_prepare_opt_debug` | false | 控制 DFX 日志输出 |

## 7. 白名单

当前仅对 surface 名包含 "aweme" 的动效节点生效（覆盖 aweme0、aweme1 等）。白名单检查位于 `CheckAndSetup()` 中，在所有退出条件通过后、激活前执行。

## 8. 已知限制

- 仅支持单个动效节点（`onTreeAnimatingCount == 1`）
- 两个节点的脏区若几何重叠，会合并为一个大 bounding box（与正常路径行为一致）
- HWC buffer 更新通过 `hasGpuSurfaceDirty_` 检测，但 HWC→HWC 路径的普通 buffer 更新不触发 SetContentDirty，需依赖 isHardwareForcedDisabled_ 分支中的额外检测

## 9. 性能优化：两帧激活 + 缓存属性读取

### 9.1 优化原理

当 `hasCommandInFrame_ == false` 且 `hasGpuSurfaceDirty_ == false` 时，祖先链属性（alpha、cornerRadius）和父节点 filter 状态不会变化。上一帧 QuickPrepare 已将这些值写入节点缓存（`globalAlpha_`、`globalCornerRadius_`、`childHasVisibleFilter_` 等），可直接读取 O(1) 代替 O(depth) 遍历。

### 9.2 两帧激活流程

```
Frame N:   O(1) 条件通过 → pendingActivation_ = true → 不激活 → QuickPrepare 运行
                                              ↑ QuickPrepare 设置 globalAlpha_、
                                                globalCornerRadius_、filter 标志
Frame N+1: O(1) 条件通过 → CheckAncestorStateFromCache() O(1) → 激活
Frame N+2: O(1) 条件通过 → selectivePrepareOptActive_==true → 直接跳过 → O(1)
```

**关键设计：** 当 `selectivePrepareOptActive_==true` 时，不再检查 CheckAncestorStateFromCache 和 IsSubtreeShallow，因为 O(1) 条件（hasGpuSurfaceDirty_、hasCommandInFrame_）不变即意味着缓存属性值仍然有效。若 O(1) 条件变化，优化立即退出。

### 9.3 CheckAncestorStateFromCache 实现原理

读取父节点上由上一帧 QuickPrepare 写入的缓存值，避免遍历祖先链：

| 检查 | 缓存来源 | 读取方式 | 闭环风险 |
|------|----------|----------|----------|
| R1: 祖先 alpha == 1.0 | `parent->GetGlobalAlpha()` = 所有祖先 alpha 之积 | `>= 1.0f` 则所有祖先 opaque | R1 |
| R5: 祖先 cornerRadius | `parent->GetGlobalCornerRadius()` | 快速路径 `UpdateCurCornerInfo` 传入作为累积初值 | R5, R17 |
| R3/R4: 父无 filter/effect | `parent->ChildHasVisibleFilter()` 等 | 直接读 bool | R3, R4 |

**注意：** R1 使用 `>= 1.0f` 而非 `== 1.0f`，因为 alpha > 1.0 的极端情况与当前行为一致（均允许 alpha >= 1.0）。

### 9.4 R5 祖先圆角处理策略

R5（GlobalCornerRadius 未传播）**不通过激活条件排除**，而是通过**代码修复**闭环：

- 快速路径步骤 5 调用 `UpdateCurCornerInfo(parent->GetGlobalCornerRadius(), curCornerRect)`
- `parent->GetGlobalCornerRadius()` 返回上帧 QuickPrepare 写入的缓存值（component-wise max 累积）
- `UpdateCurCornerInfo` 计算 `globalCornerRadius_ = max(selfCR, parentCR)`，与正常路径一致
- `globalCornerRect_` 虽可能不精确，但不被 render 阶段消费，不影响渲染
- `hasGlobalCorner_` 由 `UpdateRenderParams` 根据 `globalCornerRadius_.IsZero()` 设置，正确反映当前状态

**前提条件：** `hasCommandInFrame_==false` 保证父节点缓存在当前帧不变，值与上帧 QuickPrepare 写入时一致。

### 9.5 每帧开销对比

| 场景 | 旧方案（每帧遍历） | 新方案（两帧激活） |
|------|---------------------|---------------------|
| O(1) 条件满足但无法激活（底噪） | O(depth) 遍历 | O(1) |
| 首次激活 | O(depth) 遍历 | O(1) 读缓存 |
| 持续激活 | O(1)（需 ancestorChainVerified_） | O(1)（parent alpha 安全校验） |
| 退出 | O(1) | O(1) |

核心收益：**不进入方案场景的底噪开销从 O(depth) 降为 O(1)**。

## 10. 诊断日志

所有 RS_LOGI 使用 `%{public}` 格式符确保在 release 构建中可见。日志由调试开关 `persist.graphic.selective_prepare_opt_debug` 控制（ancestorState 拒绝日志始终输出）。

| 日志 | 开关控制 | 说明 |
|------|----------|------|
| `SelectivePrepareOpt: disabled by feature switch` | debug | 功能开关关闭 |
| `SelectivePrepareOpt: reject [gpuDirty=... cmdInFrame=...]` | debug | GPU脏/有命令 |
| `SelectivePrepareOpt: reject [onTreeAnimatingCount=...]` | debug | 动效节点数≠1 |
| `SelectivePrepareOpt: reject [activeNodeCount=...]` | debug | 活跃节点数不匹配 |
| `SelectivePrepareOpt: reject [surface=... not in aweme whitelist]` | debug | 不在白名单 |
| `SelectivePrepareOpt: reject [notRotationOnly]` | debug | 动画非纯旋转 |
| `SelectivePrepareOpt: pending activation [...]` | debug | 两帧激活：首帧通过 |
| `SelectivePrepareOpt: reject [ancestorState] parentAlpha=... parentCR=... childFilter=...` | **始终** | 祖先状态不满足 |
| `SelectivePrepareOpt: reject [subtreeNotShallow]` | debug | 子树不浅 |
| `SelectivePrepareOpt: activated [...]` | debug | 激活成功 |
| `SelectivePrepareFastPath` | debug | 快速路径执行 |
