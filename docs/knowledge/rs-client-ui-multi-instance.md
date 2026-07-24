# 客户端多实例、窗口管理以及事务机制

## 适用范围

- 窗口管理以及事务提交
- 客户端多实例支持上下文
- 客户端节点树构建以及上下树操作

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| RSUIDirector | `rs_ui_director.h/.cpp` | 窗口管理以及事务提交 |
| RSUIContext | `rs_ui_context.h/.cpp` | 多实例上下文 |
| RSUIContextManager | `rs_ui_context_manager.h/.cpp` | 多实例上下文管理、注册与查找 |
| RSNodeMapV2 | `rs_node_map_v2.h/.cpp` | 本实例节点注册与查找 |
| RSTransactionHandler | `rs_transaction_handler.h/.cpp` | 事务缓存与提交 |

## 设计背景与决策理由
应用 stage 模型下，支持一个进程多个窗口示例，因此存在多线程多实例调用 rs_client 侧接口的情况。
单实例 rs_client 容易出现以下情况：
- 渲染节点可以在任意客户端使，用并发使用不安全：多线程并发读写同一个节点/变量时容易发生踩内存crush。
- 多个客户端共享大量全局变量，线程时序无法保证：每个client触发flush message都会把所有事务发送到rs服务端，容器发生消息队列拆分以及数据不同步。
- 节点注册的动画结束回调需要等待rs执行结束后抛任务回到对应的线程/实例，抛错会导致无法执行回调。

rs客户端采用多实例，数据隔离来实现并行化：
- 去除全局变量（RSNodeMap、RSTransactionProxy、RSAnimation）各个客户端数据隔离。
- 每个客户端对象增加上下文机制（RSUIContext），当前上下文的数据仅能在当前上下文使用。

## 核心模型

### 多实例架构

多实例架构主要由以下几个类协作完成，关系如下（创建方向自上而下，持有方向为成员或 friend）：

```
RSUIContextManager (进程级单例)
  │  rsUIContextMap_: unordered_map<token, shared_ptr<RSUIContext>>
  │  friend: RSUIDirector / RSUIContext
  ├─ CreateRSUIContext()  → new RSUIContext(token, connectToRenderRemote)
  ├─ GetRSUIContext(token)
  └─ DestroyContext(token)
          │
          ▼
RSUIContext (实例上下文, 继承 enable_shared_from_this)
  │  构造私有，只能由 RSUIContextManager / RSUIDirector (friend) 创建
  │  token_ : 实例唯一标识
  ├─ RSNodeMapV2                               nodeMap_                  (值成员, 本实例节点注册表)
  ├─ shared_ptr<RSTransactionHandler>          rsTransactionHandler_     (命令事务)
  ├─ shared_ptr<RSSyncTransactionHandler>      rsSyncTransactionHandler_ (同步事务)
  ├─ shared_ptr<RSRenderInterface>             rsRenderInterface_        (渲染管线)
  ├─ unordered_map<pid_t, RSImplicitAnimator>  rsImplicitAnimators_      (隐式动画, 按线程)
  ├─ shared_ptr<RSModifierManager>             rsModifierManager_        (modifier 管理)
  └─ TaskRunner / requestVsyncCallback_                                  (UI 线程任务投递与 vsync)
          │
          ▼
RSNodeMapV2 (final)
  │  nodeMapNew_: unordered_map<NodeId, weak_ptr<RSBaseNode>>
  │  构造私有，friend: RSUIContext
  └─ RegisterNode / UnregisterNode / GetNode<T> / TraversalNodes

RSTransactionHandler (继承 enable_shared_from_this)
  │  friend: RSUIDirector
  │  token_ / renderPipelineClient_ / renderThreadClient_
  ├─ implicitRemoteTransactionData_   (发往 RenderService 的命令缓冲)
  ├─ implicitCommonTransactionData_   (本地/RT 命令缓冲)
  └─ AddCommand / FlushImplicitTransaction / CommitSyncTransaction
```

**RSUIDirector**（`final` 类）是帧驱动与窗口生命周期管理器，持有 `std::shared_ptr<RSUIContext> rsUIContext_`，是上述三类的 friend。创建路径 ：
- `RSUIDirector::Create(connectToRenderRemote, rsUIContext)` → `RSUIDirector::Init`：
  - 若传入 `rsUIContext` 非空，则 `rsUIContext_ = rsUIContext`、`skipDestroyUIContext_ = true`（子窗复用父实例 context，多个 ArkUI 子窗共享同一上下文）；
  - 否则 `rsUIContext_ = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote)` 新建。
- `RSUIContext` 构造函数一次性创建 `RSRenderInterface`、`RSTransactionHandler(token, renderPipelineClient)`、`RSSyncTransactionHandler`，并 `rsSyncTransactionHandler_->SetTransactionHandler(rsTransactionHandler_)` 互相关联。
- `RSUIContextManager::CreateRSUIContext` 用 `tid<<32 | counter` 生成 token，按 token 缓存到 `rsUIContextMap_`；分离渲染或非 SceneBoard 下进程内只保留一个 context（命中即复用）。

**RSUIContextManager**（`final` 类，进程级单例）是多实例上下文的注册中心，通过 `RSUIContextManager::Instance`/`RSUIContextManager::MutableInstance` 获取单例。核心成员 `rsUIContextMap_`（`token → shared_ptr<RSUIContext>`），按 token 索引各实例。主要接口：`RSUIContextManager::CreateRSUIContext` 创建并缓存 `RSUIContext`（`GenerateToken` 用 `tid<<32 | counter` 生成唯一 token），`RSUIContextManager::GetRSUIContext` 按 token 查找，`RSUIContextManager::DestroyContext` 销毁。复用策略由 `isMultiInstanceOpen_`、`isDividedRender_`、`hasCreateRSUIContext_` 控制——分离渲染或非 SceneBoard 下进程内只保留一个 context。`RSUIContextManager::CloseAllSyncTransaction` 批量关闭所有 context 的同步事务，`RSUIContextManager::StartCloseSyncTransactionFallbackTask` 提供超时兜底。

**RSUIContext**（继承 `enable_shared_from_this`）是多实例上下文容器，构造函数私有，只能由 `RSUIContextManager`/`RSUIDirector`（friend）创建。持有 `token_`（实例唯一标识），值持有 `RSNodeMapV2 nodeMap_`，共享持有 `RSTransactionHandler`、`RSSyncTransactionHandler`、`RSRenderInterface`。动画侧持有 `rsImplicitAnimators_`（按线程懒创建的 `RSImplicitAnimator`）、`rsModifierManager_`/`rsModifierManagerMap_`（`RSModifierManager`，uniRender 单个、分离渲染按线程）、`animations_`（`AnimationId → RSAnimation`）与 `animatingPropertyNum_`。主要接口：`RSUIContext::GetMutableNodeMap`/`RSUIContext::GetNodeMap` 取节点注册表，`RSUIContext::GetRSTransaction` 取事务处理器，`RSUIContext::GetRSImplicitAnimator`/`RSUIContext::GetRSModifierManager` 取动画与 modifier 管理器，`RSUIContext::AnimationCallback`/`RSUIContext::AddAnimationInner`/`RSUIContext::RemoveAnimationInner` 管理动画生命周期，`RSUIContext::PostTask`/`RSUIContext::PostDelayTask`/`RSUIContext::SetUITaskRunner` 投递 UI 线程任务，`RSUIContext::SetRebuildState`/`RSUIContext::WaitForRebuildNormal` 控制节点树重建状态。

**RSNodeMapV2**（`final` 类）是本实例的节点注册表，作为值成员直接持有于 `RSUIContext`，构造私有，friend 为 `RSUIContext`。核心成员 `nodeMapNew_`（`NodeId → weak_ptr<RSBaseNode>`），用弱引用避免延长节点生命周期。主要接口：`RSNodeMapV2::RegisterNode` 注册节点（重复注册打 WARN），`RSNodeMapV2::UnregisterNode` 注销节点，`RSNodeMapV2::GetNode<T>` 模板取节点（通过 `ReinterpretCast` 安全转型），`RSNodeMapV2::TraversalNodes` 遍历所有节点（供 `RSUIDirector::ReleaseRenderNode` 等批量操作使用），`RSNodeMapV2::RegisterNodeInstanceId` 关联节点与实例 ID。每个实例拥有独立的 `RSNodeMapV2`，实现节点树的实例间隔离。

**RSTransactionHandler**（继承 `enable_shared_from_this`）是命令事务处理器，friend 为 `RSUIDirector`，由 `RSUIContext` 构造时创建并持有 `token_`、`renderPipelineClient_`、`renderThreadClient_`。核心缓冲区：`implicitRemoteTransactionData_`（发往 RenderService 的命令）与 `implicitCommonTransactionData_`（本地/RT 命令），另有两个栈支持同步事务嵌套。主要接口：`RSTransactionHandler::AddCommand` 收集 UI 线程产生的 `RSCommand`（按 `isRenderServiceCommand` 分流到 remote/common 缓冲），`RSTransactionHandler::FlushImplicitTransaction` 帧末打包成 `RSTransactionData` 发往 RenderService，`RSTransactionHandler::Begin`/`RSTransactionHandler::Commit` 管理事务起止，`RSTransactionHandler::CommitSyncTransaction`/`RSTransactionHandler::StartSyncTransaction`/`RSTransactionHandler::CloseSyncTransaction` 支持同步事务，`RSTransactionHandler::ExecuteSynchronousTask` 执行同步任务，`RSTransactionHandler::MoveCommandByNodeId` 按节点迁移命令（供 context 切换场景），`RSTransactionHandler::SetFlushEmptyCallback`/`RSTransactionHandler::SetCommitTransactionCallback` 设置回调。

**协作关系**：

```
属性变更（在 animateTo 会话内）
  → RSImplicitAnimator::CreateImplicitAnimation  生成 RSAnimation
  → RSUIContext::AddAnimationInner  登记到 animations_
  → 动画驱动 modifier 变更 → RSModifierManager::AddModifier/AddAnimation
  → 帧驱动 RSModifierManager::Animate(time) 推进渲染动画
  → 服务端回调 RSUIContext::AnimationCallback 处理 FINISHED 等事件
```

`RSUIContext` 自身还持有 `animations_`（`AnimationId → RSAnimation`）与 `animatingPropertyNum_`（按 PropertyId 计数活跃动画），`RSUIContext::AnimationCallback` 接收服务端回调并触发 finish/repeat 回调，动画完成时经 `RSUIContext::RemoveAnimationInner` 清理并在计数归零时 `SetPropertyOnAllAnimationFinish`。这套机制使每个实例的隐式动画上下文、动画注册表与 modifier 驱动各自独立，避免跨实例动画串扰。

#### 实例隔离要点：
- 每个 `RSUIContext` 拥有独立的 `nodeMap_`、`rsTransactionHandler_`、`rsSyncTransactionHandler_`、隐式动画器和 modifier 管理器，不同实例的节点树、命令队列、动画状态互不干扰。
- `RSNode` 通过成员 `rsUIContext_` 绑定到具体实例，所有 `AddCommand` 与 `GetToken()` 均走该 context。

#### 命令投递数据流（实例隔离的关键）：

```
RSNode 属性变更 / 上下树操作 ...
  → RSNode::AddCommand
  → RSNode::AddCommandInner
  → RSNode::GetRSTransaction() = rsUIContext_->GetRSTransaction()
  → RSTransactionHandler::AddCommand
  → 缓存到 implicitRemote/CommonTransactionData_
  → RSUIDirector 帧末调 SendMessages()->GetRSTransaction()->FlushImplicitTransaction()
  → 打包成 RSTransactionData 发往 RenderService
```

（已废弃）无 context 回退路径：`RSNode` 构造函数未传 `rsUIContext_` 时，`RSNode::GetRSTransaction` 返回 `nullptr`，`RSNode::AddCommandInner` 改走进程级 `RSTransactionProxy::GetInstance()`，对应单实例旧链路。

### 窗口管理以及RSUIDirector状态机轮转

#### RSUIDirector生命周期状态枚举

`RSUIDirectorLifecycleState`（客户端-服务端同步用）定义 6 个状态：

| 状态 | 含义 |
| --- | --- |
| CREATE | 已创建，尚未就绪 |
| RESUME | 回前台，重建服务端渲染节点 |
| FOREGROUND | 前台活跃，正在渲染 |
| BACKGROUND | 后台，停止渲染但保留资源 |
| STOP | 退后台，释放服务端渲染节点 |
| DESTROYED | 已销毁，终态 |

#### 状态轮转

每个 `Go*` 方法遵循同一模式（自 `RSUIDirector::GoCreate` 起）：先检查源状态是否合法，非法则 `return` 不改状态；合法则执行 `ExecuteGo*` 副作用 + `AddUIDirectorCommand<RSUIDirectorGo*>` 投递命令，最后把 `currentUIDirectorState_` 置为目标状态。`RSUIDirector::Init` 末尾会自动串联 `GoCreate()` → `GoResume()` → `GoForeground()`。

```
            GoCreate            GoResume            GoForeground
  init ───────────► CREATE ──────────► RESUME ──────────► FOREGROUND
                       │                 │                     │
                       │     GoResume    │      GoStop         │ GoBackground
                       │   (from STOP)   ▼                     ▼
                       │             ┌─ STOP ◄────────── BACKGROUND
                       │             │   │                     │
                       │             │   │ GoResume             │ GoForeground
                                         │   (to RESUME)        │
                                         └──────────────────────┘
  Destroy ──► DESTROYED  （可从任意状态进入，终态）
```

合法/非法源状态对照表（✅ 允许转换并执行副作用，❌ 拒绝转换不改状态，— 自转换或不产生副作用的静默迁移）：

| 源状态 \ 目标状态 | CREATE | RESUME | FOREGROUND | BACKGROUND | STOP | DESTROYED |
| :-: | :-: | :-: | :-: | :-: | :-: | :-: |
| **CREATE** | — | ✅ | ❌ | ❌ | — | ✅ |
| **RESUME** | ✅ | — | ✅ | ❌ | ✅ | ✅ |
| **FOREGROUND** | ✅ | ❌ | — | ✅ | ❌ | ✅ |
| **BACKGROUND** | ✅ | ❌ | ✅ | ❌ | ✅ | ✅ |
| **STOP** | ✅ | ✅ | ❌ | ❌ | — | ✅ |
| **DESTROYED** | ✅ | ❌ | ❌ | ❌ | ❌ | — |

> 注：
> - `RSUIDirector::GoCreate`/`RSUIDirector::Destroy` 不做源状态校验，无条件执行 `Execute` + 命令投递，故对各源状态均为 ✅（自转换除外）。
> - `RSUIDirector::GoStop` 即使源状态合法，`ExecuteGoStop` 与命令投递仅在 `IsRenderNodeRebuildEnabled() && RebuildDebugEnabled()` 时才执行，但状态仍会置为 STOP。
> - `CREATE → STOP` 标为 —：`GoStop` 未将 CREATE 列入非法源，但也未列入合法执行分支，状态会被置为 STOP 但不执行副作用、不投递命令。
> - 各方法的自转换（如 `RESUME → RESUME`）标为 —：方法不拒绝但跳过 `Execute` + 命令，状态不变。

#### ExecuteGo* 副作用

| 方法 | 关键副作用 |
| --- | --- |
| `RSUIDirector::ExecuteGoCreate` | 无 |
| `RSUIDirector::ExecuteGoResume` | 重建服务端节点树 `RebuildNodeTree`，设置 context RebuildState |
| `RSUIDirector::ExecuteGoForeground` | `isActive_=true`；`UpdateWindowStatus(true)`；root `SetEnableRender(true)`；surface `MarkUIHidden(false)` + `SetAbilityState(FOREGROUND)` |
| `RSUIDirector::ExecuteGoBackground` | `isActive_=false`；`UpdateWindowStatus(false)`；`ReportUiSkipEvent`；root `SetEnableRender(false)`；surface `MarkUIHidden(true)` + `SetAbilityState(BACKGROUND)`；非 uniRender/非 textureExport 时清 bufferQueue 缓存与冗余资源 |
| `RSUIDirector::ExecuteGoStop` | 释放服务端节点树 `ReleaseRenderNode`：遍历当前实例 `nodeMap_` ，释放UI节点对应的服务端渲染节点（如果存在） `ReleaseInRender()`（跳过LAZY_LOAD 节点、AppWindow 节点、textureExport 节点） |
| `RSUIDirector::ExecuteGoDestroy` | root `RemoveFromTree` + `rootNode_.reset()`；销毁 `RSUIContext`（`skipDestroyUIContext_` 为真时跳过，保留子窗共享 context）；清理 `uiTaskRunners_`/`requestVsyncCallbacks_` |

#### 客户端-服务端状态镜像

状态变更通过命令同步到服务端，形成双端状态机：

```
客户端 RSUIDirector::GoXxx
  → RSUIDirector::AddUIDirectorCommand<RSUIDirectorGoXxx>
  → 构造命令 (nodeId, pid, token)
  → rsUIContext_->GetRSTransaction()->AddCommand(command, true)
  → FlushImplicitTransaction 打包发往 RenderService

服务端 RSUIDirectorCommandHelper::GoXxx
  → RSContext::GetUIRenderDirector(pid, token)
       ├─ GoCreate 时若不存在则 CreateUIRenderDirector
       └─ GoDestroy 时 DestroyUIRenderDirector
  → RSUIRenderDirector::OnStateSync(state)
```

`RSUIRenderDirector` 是服务端镜像，按 `token` 标识，持有 `currentState_`、`appWindowNode_` 与注册的 `renderNodes_`，在 `GoCreate` 时按需创建、`GoDestroy` 时从 `RSContext` 移除。`RSUIDirectorCommandHelper::GoStop` 在服务端会额外 `DestroyTokenNode(pid, token)` 清理该 token 的节点。

#### 窗口管理

`RSUIDirector` 持有 `std::weak_ptr<RSSurfaceNode> surfaceNode_` 与 `std::weak_ptr<RSRootNode> rootNode_`，窗口绑定通过三者协作完成：

- `RSUIDirector::SetRSSurfaceNode(surfaceNode)`：保存 surface 后调 `RSUIDirector::AttachSurface`。
- `RSUIDirector::SetRSRootNode(rootNode)` / `RSUIDirector::SetRoot(root)`：保存 root 后调 `RSUIDirector::AttachSurface`。
- `RSUIDirector::AttachSurface`：当 root 与 surface 均就绪时，`rootNode->AttachRSSurfaceNode(surfaceNode)` 把绘制输出 Surface 绑定到根节点；任一未就绪则打 debug 日志等待下次调用。

窗口状态随状态机联动：`ExecuteGoForeground`/`ExecuteGoBackground` 分别把 `RSSurfaceNodeAbilityState` 置为 `FOREGROUND`/`BACKGROUND`，并 `MarkUIHidden` 控制可见性；`SetAbilityBGAlpha`、`SetContainerWindow` 等外观接口直接转发给 `surfaceNode_`。

### 事务提交

RSNode 的属性变更不会立即同步到服务端，而是通过 `RSTransactionHandler` 缓冲命令，在 `RSUIDirector` 帧末尾统一提交。以下按命令分流、双缓冲、帧末提交、显式事务、同步事务、命令迁移和线程安全展开。

#### 命令分流

`RSTransactionHandler::AddCommand` 按 `isRenderServiceCommand` 将命令分流到不同缓冲：

- `isRenderServiceCommand=true` 且 `renderPipelineClient_` 非空 → `RSTransactionHandler::AddRemoteCommand`，命令进入 `implicitRemoteTransactionData_`（发往 RenderService）。
- `isRenderServiceCommand=false` → `RSTransactionHandler::AddCommonCommand`，命令进入 `implicitCommonTransactionData_`（本地/渲染线程处理）。
- `AddRemoteCommand`/`AddCommonCommand` 内部判断同步事务栈是否非空：若栈非空，命令进入栈顶 `RSTransactionData`；否则进入隐式缓冲。

渲染线程产生的命令走独立入口 `RSTransactionHandler::AddCommandFromRT`，进入 `implicitTransactionDataFromRT_` 缓冲，加 `mutexForRT_` 保护，与 UI 线程缓冲隔离。

#### 双缓冲与双客户端

`RSTransactionHandler` 持有三组命令缓冲与两个客户端：

| 缓冲 | 用途 | 提交客户端 | 锁 |
| --- | --- | --- | --- |
| `implicitRemoteTransactionData_` | UI 线程发往 RenderService 的命令 | `renderPipelineClient_` | `mutex_` |
| `implicitCommonTransactionData_` | UI 线程本地/RT 命令 | `renderThreadClient_` | `mutex_` |
| `implicitTransactionDataFromRT_` | RT 线程命令，必定发往 RenderService | `renderPipelineClient_` | `mutexForRT_` |

另有两个栈 `implicitCommonTransactionDataStack_`/`implicitRemoteTransactionDataStack_` 用于显式事务和同步事务嵌套场景。`renderPipelineClient_` 是发往 RenderService 的 IPC 客户端，`renderThreadClient_` 是本地渲染线程客户端，二者在 `RSUIDirector::Init` 中通过 `RSTransactionHandler::SetRenderThreadClient` 设置。

#### 隐式事务

`RSUIDirector::SendMessages` 在帧末调用 `RSTransactionHandler::FlushImplicitTransaction`，传入 `timestamp_`、`abilityName_`、`dvsyncUpdate_`、`dvsyncTime_`：

```
RSUIDirector::SendMessages
  → rsUIContext_->GetRSTransaction()->FlushImplicitTransaction(timestamp, abilityName, dvsyncUpdate, dvsyncTime)
```

`RSTransactionHandler::FlushImplicitTransaction` 的处理顺序：

1. 若同步事务栈非空且 `needSync_` 为真，打错误日志返回（同步事务未关闭时不允许隐式提交）。
2. **提交 common**：`implicitCommonTransactionData_` 非空时，设置 `timestamp_`/`token_`/`tid_` 后调 `renderThreadClient_->CommitTransaction`，然后重建空缓冲；为空时调 `flushEmptyCallback_`（用于空帧回调）。
3. **提交 remote**：`implicitRemoteTransactionData_` 非空时，`std::swap` 取出数据（避免拷贝），设置 `timestamp_`/`token_`/`tid_`，若 `commitTransactionCallback_` 非空走回调（hybrid render 场景），否则调 `renderPipelineClient_->CommitTransaction`，最后记录 `transactionDataIndex_`。

`RSTransactionHandler::FlushImplicitTransactionFromRT` 是 RT 线程的独立提交入口，加 `mutexForRT_` 保护，提交 `implicitTransactionDataFromRT_` 后重建空缓冲。

#### 显式事务（Begin/Commit）

显式事务通过栈实现嵌套，供窗口事务等场景使用：

- `RSTransactionHandler::Begin(syncId)`：向 common/remote 栈各压入一个新 `RSTransactionData`；若 `needSync_ && syncId > 0`，标记栈顶 `MarkNeedSync`。若栈顶已标记 `IsNeedSync()` 且当前为同步事务，不重复压栈。
- 命令在 Begin/Commit 之间时，`AddRemoteCommand`/`AddCommonCommand` 将命令写入栈顶而非隐式缓冲。
- `RSTransactionHandler::Commit(timestamp)`：若栈顶 `IsNeedSync()`，不弹出（等同步提交）；否则弹出 remote 栈顶，非空时经 `renderPipelineClient_->CommitTransaction` 提交，同时弹出 common 栈顶。

#### 同步事务

同步事务确保多个实例的命令在同一帧到达服务端，通过 `needSync_` 标志和 `syncId` 协调：

- `RSTransactionHandler::StartSyncTransaction`/`RSTransactionHandler::CloseSyncTransaction`：设置/清除 `needSync_`。
- `RSTransactionHandler::CommitSyncTransaction(syncId, timestamp, abilityName)`：用 `ProcessSyncTransactionStack` 翻转栈后按 FIFO 顺序提交 common 栈（经 `renderThreadClient_`）和 remote 栈（经 `renderPipelineClient_`），每帧数据设置 `syncId`。
- `RSTransactionHandler::MarkTransactionNeedSync`：标记栈顶需要同步。`RSTransactionHandler::MarkTransactionNeedCloseSync` 同时标记需要关闭同步并设置事务数量。`RSTransactionHandler::SetSyncTransactionNum` 设置栈顶的同步事务数量。
- `RSTransactionHandler::MergeSyncTransaction`：将本 handler 栈顶命令合并到目标 handler 栈顶（`MoveAllCommand`），弹出本 handler 栈顶，并调目标 handler 的 `CloseSyncTransaction`，用于多实例合并同步事务。

`RSSyncTransactionHandler` 是上层同步事务控制器（由 `RSUIContext` 持有），通过 `RSSyncTransactionHandler::SetTransactionHandler` 关联到 `RSTransactionHandler`，提供 `OpenSyncTransaction`/`CloseSyncTransaction` 的对外接口，持有 `RSTransaction` 对象管理同步语义。`RSUIContextManager::CloseAllSyncTransaction` 可批量关闭所有实例的同步事务，`RSUIContextManager::StartCloseSyncTransactionFallbackTask` 提供 5 秒超时兜底，防止同步事务未关闭导致阻塞。

#### 命令迁移

`RSTransactionHandler::MoveCommandByNodeId` 按节点 ID 把命令从本 handler 迁移到目标 handler（同时迁移 remote 和 common），供 context 切换场景使用（如节点从一个实例迁到另一个实例）。`RSTransactionHandler::MoveCommandByNodeIdExcludeTreeCommands` 同上但排除树结构命令（`AddChild`/`RemoveChild` 等），通过临时翻转栈遍历每一层 `RSTransactionData` 处理后恢复栈顺序。`RSUIContext::MoveModifier` 在 context 间迁移 modifier 时会调用此机制。

#### 同步任务

`RSTransactionHandler::ExecuteSynchronousTask` 按 `isRenderServiceTask` 分流：为真时经 `renderPipelineClient_->ExecuteSynchronousTask` 发往 RenderService，为假时经 `renderThreadClient_->ExecuteSynchronousTask` 在本地渲染线程同步执行。

#### 线程安全

- `mutex_`：保护 UI 线程的 `implicitRemoteTransactionData_`、`implicitCommonTransactionData_` 及两个栈，`AddCommand`、`FlushImplicitTransaction`、`Begin`、`Commit`、`CommitSyncTransaction`、`MoveCommandByNodeId` 等均加此锁。
- `mutexForRT_`：独立保护 RT 线程的 `implicitTransactionDataFromRT_`，`AddCommandFromRT` 和 `FlushImplicitTransactionFromRT` 加此锁，与 UI 线程缓冲互不阻塞。
- `MergeSyncTransaction` 使用 `std::lock(selfLock, otherLock)` 同时锁两个 handler 的 `mutex_`，避免死锁。
