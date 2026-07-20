# 客户端 UI 节点

## 适用范围

- RSNode 及其派生类（RSCanvasNode、RSSurfaceNode、RSDisplayNode 等）的创建、属性设置和树操作
- DrawOnNode 自定义绘制
- 隐式/显式动画在节点上的使用

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 基础节点 | `rosen/modules/render_service_client/core/ui/rs_node.h/.cpp` | RSNode：属性设置、树操作、动画接口 |
| Surface 节点 | `rosen/modules/render_service_client/core/ui/rs_surface_node.h/.cpp` | RSSurfaceNode：窗口 Surface 绑定、Buffer 管理 |
| Canvas 节点 | `rosen/modules/render_service_client/core/ui/rs_canvas_node.h/.cpp` | RSCanvasNode：自定义绘制录制 |
| Canvas 绘制节点 | `rs_canvas_drawing_node.h/.cpp` | RSCanvasDrawingNode：直接绘制模式 |
| Display 节点 | `rs_display_node.h/.cpp` | RSDisplayNode：显示节点 |
| Root 节点 | `rs_root_node.h/.cpp` | RSRootNode：根节点 |
| Effect 节点 | `rs_effect_node.h/.cpp` | RSEffectNode：效果节点 |
| Proxy 节点 | `rs_proxy_node.h/.cpp` | RSProxyNode：代理节点 |
| Depth 节点 | `rs_depth_node.h/.cpp` | RSDepthNode：深度节点 |
| Union 节点 | `rs_union_node.h/.cpp` | RSUnionNode：联合节点 |
| 属性提取器 | `rosen/modules/render_service_client/core/modifier/rs_modifier_extractor.h` | RSModifierExtractor：staging 属性读取 |
| 属性冻结器 | `rs_showing_properties_freezer.h` | RSShowingPropertiesFreezer：showing 属性只读访问 |

## 核心模型

### 节点继承体系

```
RSNode (enable_shared_from_this)
  ├── RSCanvasNode       -- 自定义绘制（录制模式）
  ├── RSCanvasDrawingNode -- 自定义绘制（直接模式）
  ├── RSSurfaceNode      -- 窗口 Surface，绑定 Consumer/Producer
  ├── RSDisplayNode      -- 显示输出
  ├── RSRootNode         -- 根节点
  ├── RSEffectNode       -- 效果容器
  ├── RSDepthNode        -- 深度排序
  ├── RSProxyNode        -- 代理/跨进程节点
  └── RSUnionNode        -- 联合节点
```

每个节点有 `RSUINodeType` 枚举标识类型，支持 `IsInstanceOf<T>()` + `ReinterpretCastTo<T>()` 安全转型。

### 属性设置与 Modifier

RSNode 的属性通过 ModifierNG 体系设置：

1. `SetBounds()` / `SetFrame()` 等属性设置方法最终调用 ModifierNG 的 `Setter()`。
2. Modifier 持有 `RSProperty<T>` 对象，属性变更时 MarkDirty。
3. Dirty Modifier 通过 Transaction 传递到服务端。
### 树操作

#### 基础树操作

RSNode 提供标准的树操作接口，均定义在 `rs_node.cpp` 中。每个操作既维护客户端本地 `children_`/`parent_`，又通过 `RSCommand` 同步到服务端。

**`RSNode::AddChild(child, index=-1)`** — 添加子节点：

- 入参校验：`child` 为空返回；`child` 的 parent 已是本节点则跳过；不允许把 `DISPLAY_NODE` 作为子节点添加。
- 纹理导出分支：若父非纹理导出节点而子是纹理导出节点，走 `RSNode::AddCompositeNodeChild` 合成层路径。
- 按 `shadowNodeFlag_` 决定加载 `child` 还是其 shadowNode 的渲染节点（`LoadRenderNodeIfNeed`）。
- 最终调 `RSNode::AddChildInner` 执行挂接。

**`RSNode::AddChildInner(child, index)`** — 内部挂接（private）：

1. 若 `child` 已有旧 parent，先调 `RSNode::RemoveFromTree` 脱离原树。
2. 按 `index` 插入 `children_`（`index < 0` 或越界则 `push_back`）。
3. `SetParent(weak_from_this)`；若 `isTextureExportNode_` 不一致则 `SyncTextureExport`。
4. 触发 `child->OnAddChildren()` 回调 + `child->MarkDirty(APPEARANCE)`。
5. 用 `child->GetHierarchyCommandNodeId()` 构造 `RSBaseNodeAddChild` 命令，经 `AddCommand` 投递。
6. `child->SetIsOnTheTree(isOnTheTree_)` 同步树归属标记。

**`RSNode::RemoveChild(child)`** — 移除子节点：

- 校验 `child` 为空或 parent 不是本节点则返回。
- 调 `RSNode::RemoveChildByNode` 从 `children_` 擦除 → `child->OnRemoveChildren()` → `child->parent_.reset()` → `child->MarkDirty(APPEARANCE)`。
- 构造 `RSBaseNodeRemoveChild` 命令投递 → `child->SetIsOnTheTree(false)`。

**`RSNode::RemoveChildByNode(child)`** — 按 node 从 `children_` 擦除（private）：`std::find_if` + `ROSEN_EQ` 匹配后 `erase`，含 `CheckMultiThreadAccess` 线程检查。

**`RSNode::RemoveChildByNodeSelf(WeakPtr child)`** — 弱引用包装，`lock` 成功后转调 `RemoveChild`，用于不确定 child 是否仍存活的场景。

**`RSNode::MoveChild(child, index)`** — 调整子节点顺序：

- 校验 `child` 是本节点子节点（parent 检查 + `find_if` 查找）。
- `erase` 原位置 → 按 `index` `insert` 或 `push_back`。
- 构造 `RSBaseNodeMoveChild` 命令投递 → `SetIsOnTheTree`。
- 不改父子关系、不触发 `OnAddChildren`/`OnRemoveChildren`、不 `MarkDirty`——仅改同层顺序。

**`RSNode::RemoveFromTree()`** — 节点自删：

- `MarkDirty(APPEARANCE)` → 让父节点 `RemoveChildByNode(shared_from_this)` → `OnRemoveChildren()` → `parent_.reset()`。
- 用自身 `GetHierarchyCommandNodeId()` 构造 `RSBaseNodeRemoveFromTree` 命令投递（始终发送）。
- `SetIsOnTheTree(false)`。含 `CheckMultiThreadAccess`。

**`RSNode::ClearChildren()`** — 清空所有子节点：

- 遍历 `children_`：每个 child `SetIsOnTheTree(false)` + `parent_.reset()` + `MarkDirty(APPEARANCE)`。
- `children_.clear()` → 构造 `RSBaseNodeClearChild` 命令投递。

**`RSNode::SetParent(WeakPtr parent)`** — 仅赋值 `parent_`，无副作用、无命令。

#### 跨父节点与跨屏操作

窗口跨屏场景下，一个子节点可能挂载到多个父节点，使用以下接口（仅 `RSDisplayNode` 支持跨父操作）：

**`RSNode::AddCrossParentChild(child, index)`** — 跨父添加（不从旧父移除）：

- 仅 `RSDisplayNode` 支持，否则打错误日志返回。
- 直接插入 `children_`，不调 `RemoveFromTree` 脱离旧父。
- `SetParent` + `OnAddChildren` + `MarkDirty` → 构造 `RSBaseNodeAddCrossParentChild` 命令投递。

**`RSNode::RemoveCrossParentChild(child, newParent)`** — 跨父移除并设置新父：

- 仅 `RSDisplayNode` 支持。`RemoveChildByNode` → `SetParent(newParent)` → 构造 `RSBaseNodeRemoveCrossParentChild(parentId, childId, newParentId)` 命令投递。含 `CheckMultiThreadAccess`。

**`RSNode::AddCrossScreenChild(child, index, autoClearCloneNode)`** — 跨屏添加克隆：

- `child` 必须是 `RSSurfaceNode`。
- 不修改本地 `children_`，仅用 `GenerateId()` 生成克隆 ID，构造 `RSBaseNodeAddCrossScreenChild` 命令，服务端据此创建克隆节点。

**`RSNode::RemoveCrossScreenChild(child)`** — 跨屏移除克隆：

- `child` 必须是 `RSSurfaceNode`。构造 `RSBaseNodeRemoveCrossScreenChild` 命令投递。

#### RSDisplayNode 特有树操作

`RSDisplayNode` 作为显示输出节点，提供独立的上下树与屏幕绑定接口（均定义在 `rs_display_node.cpp`）：

**`RSDisplayNode::AddDisplayNodeToTree`** — 将 DisplayNode 及其子树上树：

**`RSDisplayNode::RemoveDisplayNodeFromTree`** — 将 DisplayNode 及其子树下树：

> 注：上述两个方法用 `LCOV_EXCL_START`/`LCOV_EXCL_STOP` 排除覆盖率统计，用于特定显示管理场景。

**`RSDisplayNode::SetScreenId(screenId)`** — 设置 DisplayNode 关联的物理屏幕 ID：此操作会将服务端的 `RSLogicalDisplayRenderNode` 及其子树整体迁移到另一个屏幕节点`RSScreenRenderNode`上

#### RSSurfaceNode 特有树操作

`RSSurfaceNode` 提供两组显示/容器挂载接口，分别走直接命令和 CmdModifier 属性通道：

**`RSSurfaceNode::AttachToDisplay(screenId)`** — 将 SurfaceNode 挂载到指定 Display：

- 构造 `RSSurfaceNodeAttachToDisplay(GetId(), screenId)` 命令，经 `RSSurfaceNode::AddCommand(command, IsRenderServiceNode())` 直接投递。
- 走 `RSCommand` 即时通道，打 INFO 日志和 trace。

**`RSSurfaceNode::DetachToDisplay(screenId)`** — 将 SurfaceNode 从指定 Display 分离：

- 构造 `RSSurfaceNodeDetachToDisplay(GetId(), screenId)` 命令，经 `RSSurfaceNode::AddCommand(command, IsRenderServiceNode())` 直接投递。
- 走 `RSCommand` 即时通道，打 INFO 日志和 trace。

**`RSSurfaceNode::AttachToWindowContainer(screenId)`** — 将 SurfaceNode 挂载到窗口容器：

- 通过 `SetRSCmdProperty<AttachToWindowContainerCmdModifier>` 走 ModifierNG 属性通道，dirty modifier 在帧末统一提交。
- 与 `AttachToDisplay` 的区别：走 CmdModifier 属性系统而非直接 `RSCommand`，支持动画、去重和脏标记。

**`RSSurfaceNode::DetachFromWindowContainer(screenId)`** — 将 SurfaceNode 从窗口容器分离：

- 通过 `SetRSCmdProperty<DetachFromWindowContainerCmdModifier>` 走 ModifierNG 属性通道，与 `AttachToWindowContainer` 对称。

> 注：`AttachToDisplay`/`DetachToDisplay` 走直接 `RSCommand`（即时投递），`AttachToWindowContainer`/`DetachFromWindowContainer` 走 `SetRSCmdProperty`（CmdModifier 属性通道，帧末统一提交）。两组接口适用于不同的挂载场景：前者面向 Display 级别，后者面向窗口容器级别。

#### 回调与脏标记

树操作通过以下机制联动动画与重绘：

- **`RSNode::OnAddChildren`** / **`RSNode::OnRemoveChildren`**：子节点添加/移除时触发。若节点持有 `transitionEffect_` 且当前可见（`GetStagingProperties().GetVisible()`），调 `NotifyTransition(effect, isTransitionIn)` 启动转场动画。
- **`MarkDirty(NodeDirtyType::APPEARANCE)`**：树操作后标记节点外观脏，驱动后续重绘。
- **`SetIsOnTheTree`**：递归同步子树的是否在主树上的标记，影响渲染调度与命令投递路径。

#### 节点树重建

应用退后台超过1s后会释放服务端节点，进前台时重建:
**`RSNode::RebuildTree`** — 在节点从 `STOP` 状态恢复时重建服务端渲染节点树：

- 仅 `RSNodeState::INACTIVE` 状态执行；textureExport 节点提前返回。
- 设置 `RSUIContext::SetRebuildState(Rebuilding)` → `RSNode::_RebuildTreeInternal`：先 `ReCreateNodeInRender` 恢复自身，再 BFS 逐层（`_RebuildTreeLevel`）恢复所有子节点的渲染节点。
- `RSNode::ReCreateNodeInRender` 跳过 `LAZY_LOAD` 节点自身重建但恢复子树；`ACTIVE` 节点不重建；其余节点 `CreateRenderNode` + `SetUIContextToken` + `DoFlushModifier` 恢复属性。

#### RSDisplayNode 特有


#### RSRootNode 特有

#### 通用模式

所有树操作共享以下设计约定：

- **命令投递**：每个操作构造对应 `RSCommand`（`RSBaseNodeAddChild`/`RSBaseNodeRemoveChild`/`RSBaseNodeMoveChild`/`RSBaseNodeRemoveFromTree`/`RSBaseNodeClearChild` 等），经 `RSNode::AddCommand` → `RSNode::AddCommandInner` → `RSTransactionHandler::AddCommand` 投递到事务缓冲，帧末统一提交。
- **`GetHierarchyCommandNodeId`**：命令中的节点 ID 使用 `GetHierarchyCommandNodeId()` 而非 `GetId()`，以正确处理代理节点和影子节点的层级映射。
- **`IsRenderServiceNode` + `GetFollowType`**：`AddCommand` 的参数决定命令走 remote（发往 RenderService）还是 common（本地渲染线程）通道。
- **线程安全**：`RemoveChildByNode`、`RemoveFromTree`、`RemoveCrossParentChild` 含 `CheckMultiThreadAccess` 检查，确保多线程调用的安全性。

### RSSurfaceNode 特有

- `RSSurfaceNodeConfig`：节点名、SurfaceId、同步模式、窗口类型等配置。
- `CreateSurfaceNode()` 工厂方法。
- 绑定 `IConsumerSurface` 用于接收应用侧 buffer。
- `SetTakeSurfaceForUIFlag()` 标记 UI 侧取 Surface。

### RSCanvasNode 特有

- `BeginRecording()` / `FinishRecording()` — 录制绘制命令到 CmdList。
- `DrawOnNode()` — 在指定 Modifier 类型上绘制自定义内容。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| enable_shared_from_this | `RSNode : public std::enable_shared_from_this<RSNode>` | 节点树使用 shared_ptr 管理，需要安全获取自身指针 |
| RSUINodeType + IsInstanceOf | 枚举类型 + 模板转型 | 替代 dynamic_cast，编译期确定类型，无 RTTI 开销 |
| 属性设置走 Modifier | `SetBounds()` → ModifierNG::Setter | 统一属性变更通道，支持动画、去重、脏标记 |
| 跨父节点子节点 | `AddCrossParentChild()` | 窗口跨屏场景，一个子节点挂载到多个父节点 |
| Freeze 机制 | `SetFreeze()` | 冻结节点内容，避免无效重绘，节省 GPU 开销 |
| RSUIContext 多实例 | `RSUIContext` + `RSUIContextManager` | 多窗口/多实例场景下隔离节点树和动画状态 |

## 待补充背景

- RSProxyNode 的跨进程代理机制和生命周期。
- RSCanvasDrawingNode 与 RSCanvasNode 的具体差异和使用场景。
- RSUnionNode 的设计意图和使用场景。
- RSUIDirector 的帧驱动与 VSync 的关系。
- Freeze 机制在 UniRender 下的具体生效条件。
