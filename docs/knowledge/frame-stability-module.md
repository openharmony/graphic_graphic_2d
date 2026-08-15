# 稳帧检测模块

## 适用范围

改动涉及以下场景时，先读本文，再回到代码确认当前实现：

- 稳帧检测的注册、注销、回调触发和资源清理。
- 稳帧采集的启动、脏区累积和结果查询。
- 已注册上下文的参数配置动态更新（`UpdateFrameStabilityConfig` 类需求）。
- `FrameStabilityTargetType`（SCREEN/WINDOW）的选择和 target.id 的含义。
- `FrameStabilityConfig` 中 `stableDuration`、`changePercent` 的阈值行为。
- 脏区信息收集路径：统一渲染、直写模式、多屏扩展。
- 回调 IPC 跨进程传输和 `CustomFrameStabilityCallback` 适配。
- 进程退出或 Screen 节点销毁时的资源自动清理。
- 新增 IPC 接口码时需要同步注册的文件（Access Verifier、DoS 模拟器等）。

本文是背景知识和排查路线，不替代代码。修改前仍需读取对应头文件、实现文件和测试。

## 快速代码地图

| 方向 | 主要文件 |
| --- | --- |
| 类型与错误码 | `rosen/modules/render_service_base/include/transaction/rs_frame_stability_types.h` |
| 状态定义 | `rosen/modules/render_service/core/feature/frame_stability/rs_frame_stability_state.h` |
| 管理器主实现 | `rosen/modules/render_service/core/feature/frame_stability/rs_frame_stability_manager.h`, `rosen/modules/render_service/core/feature/frame_stability/rs_frame_stability_manager.cpp` |
| 回调 IPC 接口 | `rosen/modules/render_service_base/include/ipc_callbacks/rs_iframe_stability_callback.h`, `rosen/modules/render_service_base/include/ipc_callbacks/rs_iframe_stability_callback_ipc_interface_code.h` |
| 回调 IPC Proxy | `rosen/modules/render_service_base/src/ipc_callbacks/rs_frame_stability_callback_proxy.h`, `rosen/modules/render_service_base/src/ipc_callbacks/rs_frame_stability_callback_proxy.cpp` |
| 回调 IPC Stub | `rosen/modules/render_service_base/src/ipc_callbacks/rs_frame_stability_callback_stub.h`, `rosen/modules/render_service_base/src/ipc_callbacks/rs_frame_stability_callback_stub.cpp` |
| 客户端公开 API | `rosen/modules/render_service_client/core/transaction/rs_interfaces.h`, `rosen/modules/render_service_client/core/transaction/rs_interfaces.cpp` |
| 客户端旧接口层 | `rosen/modules/render_service_client/core/transaction/rs_render_interface.h`, `rosen/modules/render_service_client/core/transaction/rs_render_interface.cpp` |
| 客户端平台实现 | `rosen/modules/render_service_base/include/transaction/rs_render_pipeline_client.h`, `rosen/modules/render_service_base/src/platform/ohos/rs_render_pipeline_client.cpp` |
| IPC Proxy | `rosen/modules/render_service_base/src/platform/ohos/transaction/zidl/rs_client_to_render_connection_proxy.h`, `rosen/modules/render_service_base/src/platform/ohos/transaction/zidl/rs_client_to_render_connection_proxy.cpp` |
| IPC Stub | `rosen/modules/render_service/core/transaction/zidl/rs_client_to_render_connection_stub.cpp` |
| 服务端连接层 | `rosen/modules/render_service/core/transaction/rs_client_to_render_connection.h`, `rosen/modules/render_service/core/transaction/rs_client_to_render_connection.cpp` |
| 服务端管线代理 | `rosen/modules/render_service/core/rs_render_pipeline_agent.h`, `rosen/modules/render_service/core/rs_render_pipeline_agent.cpp` |
| 统一渲染脏区收集 | `rosen/modules/render_service/core/feature/dirty/rs_uni_dirty_compute_util.cpp` |
| 主屏 Drawable 脏区收集 | `rosen/modules/render_service/core/drawable/rs_screen_render_node_drawable.cpp` |
| 直写模式脏区收集 | `rosen/modules/render_service/core/pipeline/main_thread/rs_main_thread.cpp` |
| 多屏扩展脏区收集 | `rosen/modules/render_service/core/feature/multi_screen/rs_multi_screen_util.cpp` |
| IPC 接口码定义 | `rosen/modules/render_service_base/include/platform/ohos/transaction/rs_iclient_to_render_connection_ipc_interface_code.h` |
| IPC 安全注册 | `rosen/modules/render_service/core/transaction/rs_iclient_to_render_connection_ipc_interface_code_access_verifier.cpp`, `rosen/modules/safuzz/rs_ipc_dos_simulator/ipc/rs_irender_service_connection_ipc_interface_code_utils.cpp` |
| 单测 | `rosen/test/render_service/render_service/unittest/feature/frame_stability/rs_frame_stability_manager_test.cpp` |

## 核心模型

模块同时承载两类独立上下文：

- **检测上下文（DetectorContext）**：通过 `RegisterFrameStabilityDetection` 创建，维护状态机、累计脏区、计时器、延迟任务和 IPC 回调。状态变化时通过 `OnFrameStabilityChanged` 实时通知调用方。
- **采集上下文（CollectorContext）**：通过 `StartFrameStabilityCollection` 创建，仅累积脏区面积。调用方通过 `GetFrameStabilityResult` 一次性获取结果，获取后上下文自动销毁。

两类上下文以 `target.id`（screenId 或 surfaceNodeId）为 key，分别存储在 `detectorContexts_` 和 `collectorContexts_` 中，用独立的 `detectorMutex_` 和 `collectorMutex_` 保护。同一个 target.id 可以同时注册检测和采集，互不影响。

脏区面积计算使用 `Occlusion::Region` 做矩形合并去重，避免重叠区域被重复计算。百分比 = `Region.Area() / screenArea`。

## 设计背景与决策理由

下面的"代码体现"来自当前实现；"设计意图"包含代码路径、注释和模块责任人补充的背景。

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 双模式（检测 + 采集）分离 | `DetectorContext` 和 `CollectorContext` 分别存储，各自独立计数和互锁 | 检测模式需要实时回调，采集模式只需要一次性结果；分离后回调逻辑不影响采集路径，也避免同一个 map 上两类生命周期互相干扰 |
| 脏区驱动而非帧率驱动 | `RecordCurrentFrameDirty` 以脏区面积占比判断稳定性，不依赖帧率统计 | 脏区面积直接反映视觉内容变化，比帧率统计更精确；帧率高但画面不变时仍判定为稳定 |
| 注册后立即投递延迟任务 | `RegisterFrameStabilityDetection` 末尾 `PostTask(task, ..., stableDuration)`，`HandleStabilityTimeout` 在无脏区时将状态转为 STABLE | 无 VSync 信号时没有脏区记录触发，延迟任务确保"无变化即稳定"的语义能按时回调 |
| 脏区超阈值时重置延迟任务 | `RecordDirtyToDetector` 中 `percentage >= changePercent` 时 `RemoveTask` + `PostTask` 重新计时 | 每次不稳定判定后重新开始计时，确保从最后一次变化起算 stableDuration |
| PID 鉴权 | `UnregisterFrameStabilityDetection`、`GetFrameStabilityResult` 和所有操作已注册内容的接口中校验 `detectorContext->pid != pid` | 防止跨进程操作对方上下文，确保只有注册进程才能注销、查询或更新。新增操作已注册内容的接口时必须包含 PID 校验，不一致返回 `PERMISSION_DENIED` |
| `UpdateFrameStabilityDetection` 空实现 | `rs_frame_stability_manager.cpp` 中 `UpdateFrameStabilityDetection` 直接返回 `SUCCESS` | 预留接口，当前未实现 target ID 更新逻辑；调用方不应依赖此接口做实际功能 |
| `RSInterfaces::RegisterFrameStabilityDetection` 返回 0 | `rs_interfaces.cpp` 中 `RegisterFrameStabilityDetection` 直接 `return 0` | 当前 RSInterfaces 层未接入实际 IPC 调用，仅作为占位；实际调用走 `RSRenderInterface` 或 `RSRenderPipelineClient` |
| 同一状态不重复回调 | `RecordDirtyToDetector` 中 `if (detectorContext->state != DetectionState::NOTSTABLE)` 和 `if (detectorContext->state != DetectionState::STABLE)` 的条件判断 | 状态机设计的核心约束：只有状态真正发生变化时才触发回调。如果状态已经是 NOTSTABLE，脏区继续超阈值不会再触发 `isStable=false`；如果已经是 STABLE，脏区持续低于阈值也不会再触发 `isStable=true`。动态更新 config 时必须重置 state 为 INIT，因为配置变更后检查条件发生变化，旧状态判定不再有效，需要由新的脏区数据重新驱动状态机流转 |
| Detector 与 Collector 上下文独立更新 | `detectorContexts_` 和 `collectorContexts_` 分别存储，用独立的 `detectorMutex_` 和 `collectorMutex_` 保护 | 同一个 target.id 可以只注册检测或只注册采集，两类上下文独立注册、独立销毁。动态更新 config 时应分别处理两个 map，任一成功更新即可。Collector 不存在不算错误，Detector 不存在但 Collector 存在时也应允许更新 Collector |

## IPC 链路

### 注册/注销/采集/查询（客户端 → 服务端）

```
外部调用者
  │
  ▼
RSInterfaces                          ← render_service_client
   │
   ▼
RSRenderInterface                     ← render_service_client (旧接口层)
   │
   ▼
RSRenderPipelineClient                ← render_service_base (平台实现)
   │  ┌─ CustomFrameStabilityCallback   ← 将 std::function 转为 IPC sptr<RSIFrameStabilityCallback>
   │  └─ RSClientToRenderConnectionProxy::RegisterFrameStabilityDetection  ← IPC Proxy
   ▼
════════════════ IPC 跨进程 ═══════════════
   │  MessageParcel: target.id(Uint64) + target.type(Uint32) + config.stableDuration(Uint32) + config.changePercent(Float) + callback(RemoteObject)
   ▼
RSClientToRenderConnectionStub        ← render_service_base (Stub 反序列化)
   │
   ▼
RSClientToRenderConnection            ← render_service (连接层)
   │
   ▼
RSRenderPipelineAgent                 ← render_service (投递到主线程)
   │  PostMainThreadSyncTask → ValidateTargetId → RSFrameStabilityManager
   ▼
RSFrameStabilityManager               ← render_service (核心单例)
```

### IPC 模式选择指南

新增接口时，IPC 模式（`TF_SYNC` / `TF_ASYNC`）应根据上层是否需要感知操作结果来决定：

| 模式 | 适用场景 | 判断标准 |
| --- | --- | --- |
| `TF_SYNC` | 上层需要感知操作是否成功 | 接口返回值包含操作结果（如 `TARGET_ID_NOT_REGISTERED`、`PERMISSION_DENIED`、`INVALID_CONFIG` 等），调用方依赖返回值决定后续行为 |
| `TF_ASYNC` | 上层只需发起请求，不关心结果 | 接口仅触发服务端动作，调用方不依赖返回值（如回调通知由 `OnFrameStabilityChanged` 异步发送） |

注意点：

- 当前所有稳帧 C2R 接口（`Register/Unregister/Start/Get/Update`）均使用 `TF_SYNC` 发送，上层需要感知操作结果。
- `UpdateFrameStabilityConfig` 类接口应使用 `TF_SYNC`：上层需要感知配置是否更新成功（target.id 未注册、PID 不匹配、config 参数无效等），必须同步返回结果。
- `CustomFrameStabilityCallback` 继承 `RSFrameStabilityCallbackStub`，将用户传入的 `std::function<void(bool)>` 包装为 IPC 可传输的 `sptr<RSIFrameStabilityCallback>`。
- `RSInterfaces` 层当前是占位实现，实际调用走 `RSRenderInterface`。

### 回调（服务端 → 客户端）

```
RSFrameStabilityManager::TriggerCallback
   │
   ▼
RSIFrameStabilityCallback::OnFrameStabilityChanged(bool isStable)
   │
   ▼
RSFrameStabilityCallbackProxy         ← IPC Proxy (服务端持有，调用客户端)
   │  TF_ASYNC 方式发送，Parcel: isStable(Bool)
   ▼
════════════════ IPC 跨进程 ═══════════════
   ▼
RSFrameStabilityCallbackStub          ← IPC Stub (客户端)
   │
   ▼
CustomFrameStabilityCallback          ← 用户注册的 std::function<void(bool)>
```

注意点：

- 回调使用 `TF_ASYNC` 发送，不阻塞渲染线程。如果客户端进程已退出，`SendRequest` 会失败但不会卡住服务端。
- 回调 IPC 接口码只有一个：`ON_FRAME_STABILITY_CHANGED = 0`。

## IPC 接口码与安全注册

### IPC 接口码分配

当前稳帧检测相关接口码定义在 `rs_iclient_to_render_connection_ipc_interface_code.h`：

| 接口码 | 枚举值 | 说明 |
| --- | --- | --- |
| `REGISTER_FRAME_STABILITY_DETECTION` | `0X16000` | 注册检测 |
| `UNREGISTER_FRAME_STABILITY_DETECTION` | `0X16001` | 注销检测 |
| `START_FRAME_STABILITY_COLLECTION` | `0X16002` | 启动采集 |
| `GET_FRAME_STABILITY_RESULT` | `0X16003` | 获取采集结果 |
| `UPDATE_FRAME_STABILITY_DETECTION` | `0X16004` | 更新检测目标（当前空实现） |

新增接口码应紧跟 `0X16004`，如 `0X16005`。

### 新增 IPC 接口码时必须同步注册的文件

新增稳帧检测相关 IPC 接口码时，除了标准的 Proxy/Stub/Connection/Agent/Manager 链路外，还必须在以下文件中同步注册：

| 文件 | 注册内容 | 说明 |
| --- | --- | --- |
| `rs_iclient_to_render_connection_ipc_interface_code_access_verifier.cpp` | 新增 case，`hasPermission = IsSystemCalling(...)` | 当前所有稳帧接口均使用 `IsSystemCalling` 校验，新增接口应保持一致。未注册时 `hasPermission` 默认为 `true`，任何调用方均可访问 |
| `rs_irender_service_connection_ipc_interface_code_utils.cpp`（safuzz） | 新增 `DECLARE_RENDER_INTERFACE_CODE_NAME(NEW_CODE)` | DoS 模拟器依赖此注册，未注册时 IPC 安全测试覆盖率不完整 |

注意点：

- 当前所有稳帧接口的 Access Verifier 策略均为 `IsSystemCalling`，即只允许系统调用。新增接口应保持一致，除非有明确的产品需求降低权限。
- `UpdateFrameStabilityDetection`（`0X16004`）已在 Access Verifier 中注册，但 DoS 模拟器中未注册（`rs_irender_service_connection_ipc_interface_code_utils.cpp` 只到 `0X16003`），这是已有的遗漏。
- 未注册 Access Verifier 时，`hasPermission` 默认为 `true`，不会拒绝调用，但与同类型接口的安全策略不一致。

### 防御性编程模式

新增对 `DetectorContext` 或 `CollectorContext` 的操作时，应遵循以下防御性编程模式（与 `UnregisterFrameStabilityDetection` 和 `GetFrameStabilityResult` 保持一致）：

1. 从 map 中查找 target.id，不存在则返回 `TARGET_ID_NOT_REGISTERED`。
2. 检查获取到的 `shared_ptr` 是否为 nullptr。为 nullptr 时先 erase 该条目，再返回 `TARGET_ID_NOT_REGISTERED`。这防止了 map 中存入空指针的异常情况。
3. 校验 PID 一致性，不匹配则返回 `PERMISSION_DENIED`。**所有操作已注册内容的接口都必须校验 PID**，不仅是注销和查询，还包括更新配置等操作。不同进程操作对方已注册的内容属于鉴权失败。
4. 操作完成后记录 INFO 日志和 TRACE，包含 pid、type、id 等关键参数。

## 检测模式算法

检测模式维护一个 `DetectorContext`，包含状态机、累计脏区、计时器和延迟任务。

### 状态机

```
INIT ──→ NOTSTABLE ──→ STABLE ──→ NOTSTABLE ──→ ...
              │                        ↑
              └───── stableDuration ───┘
```

### 每帧脏区记录逻辑 (`RecordDirtyToDetector`)

```
1. 将当前帧脏区矩形列表合并到 accumulatedDirtyRegion（Region.Or 去重合并）
2. 计算 percentage = accumulatedDirtyRegion.Area() / screenArea
3. 判断：
   a. percentage >= changePercent（脏区占比超过阈值）
      → 判定"不稳定"，触发回调(isStable=false)
      → 重置 accumulatedDirtyRegion、startTime
      → 取消旧延迟任务，投递新延迟任务（stableDuration 后触发 HandleStabilityTimeout）
   b. percentage < changePercent 且 elapsed >= stableDuration
      → 判定"稳定"，触发回调(isStable=true)
      → 重置 accumulatedDirtyRegion、startTime
   c. percentage < changePercent 且 elapsed < stableDuration
      → 不触发回调，继续累积
```

### 延迟任务 (`HandleStabilityTimeout`)

当注册后无 VSync 信号（即无脏区记录），`stableDuration` 后延迟任务触发，将状态转为 STABLE 并回调 `isStable=true`。这是"无变化即稳定"的兜底逻辑。

注册时立即投递延迟任务：注册后立即投递一个 `stableDuration` 的延迟任务，确保在完全没有帧渲染的情况下也能在指定时间后回调"稳定"。

注意点：

- 同一状态不会重复回调。例如状态已经是 `NOTSTABLE` 时，脏区继续超阈值不会再触发 `isStable=false`。
- 延迟任务通过 `RSUniRenderThread::Instance().PostTask` 投递到渲染线程 looper，任务名称为 `RSFrameStability + targetId`。
- `HandleStabilityTimeout` 中 `hasPendingStabilityTask == false` 时直接返回，防止过期任务误触发。

### 动态更新配置时的重置策略

当需要动态更新已注册检测的 `FrameStabilityConfig`（如 `UpdateFrameStabilityConfig` 类需求）时，必须遵循以下重置策略：

| 字段 | 是否重置 | 原因 |
| --- | --- | --- |
| `accumulatedDirtyRegion` | **必须重置** | 旧脏区按旧 `changePercent` 累积，新阈值下无意义，需从更新时刻重新开始累积 |
| `startTime` | **必须重置** | `stableDuration` 可能变化，需从更新时刻重新计时 |
| `state` | **必须重置为 INIT** | 配置变更后检查条件发生变化，旧状态判定不再有效：例如原来 STABLE 是基于旧 `changePercent` 判定的，降低 `changePercent` 后同样的脏区占比可能已超过新阈值，应判定为 NOTSTABLE；重置为 INIT 后由下一帧脏区数据按新 config 重新驱动状态机流转 |
| 延迟任务 | **必须重新投递** | `stableDuration` 变了，旧延迟任务使用旧时长，需按新时长重新投递。旧的 `HandleStabilityTimeout` 调用会因为 `hasPendingStabilityTask == false` 而直接返回，不会误触发 |

对于 Collector 的 `collectionDirtyRegion` 和 `screenArea`，**不应重置**：采集语义是累积的，`changePercent` 只在最终 `GetFrameStabilityResult` 时使用，更新阈值不影响已采集数据。

## 采集模式算法

采集模式维护一个 `CollectorContext`，仅做脏区累积，不做实时判断。

```
1. StartFrameStabilityCollection → 创建 CollectorContext
2. 每帧 RecordDirtyToCollector → 合并脏区到 collectionDirtyRegion，更新 screenArea
3. GetFrameStabilityResult → 计算 percentage = collectionDirtyRegion.Area() / screenArea
   → result = (percentage <= changePercent)  // 稳定
   → 销毁 CollectorContext
```

采集模式是一次性的：调用 `GetFrameStabilityResult` 后自动销毁上下文，不可重复查询。如需再次采集，需重新调用 `StartFrameStabilityCollection`。

注意点：

- `changePercent` 在采集模式中只用于最终判断，不影响中间累积过程。
- `screenArea` 在每次 `RecordDirtyToCollector` 时更新，取最后一次的值。

## 渲染线程脏区收集

渲染线程是脏区数据的产生者，通过 `RecordCurrentFrameDirty` 将每帧变化信息注入 `RSFrameStabilityManager`。

### 收集参数

| 参数 | 来源 | 说明 |
| --- | --- | --- |
| `id` (NodeId) | screenId 或 surfaceNode 的 instanceRootNodeId | 标识脏区所属的 screen/window |
| `damageRegionRects` | 脏区计算模块 | 当前帧的脏区矩形列表 |
| `screenArea` | screenProperty.Width * Height | 屏幕面积，用于计算百分比 |

### 不同渲染路径的收集方式

统一渲染路径 (UniRender)：

- `rs_uni_dirty_compute_util.cpp`：按 surfaceNode 粒度收集可见脏区（含 HWC 脏区），id 为 surfaceNode 的 id
- `rs_screen_render_node_drawable.cpp`：主绘制路径，收集全屏级脏区 + HWC 脏区，id 为 screenId
- `rs_screen_render_node_drawable.cpp`：硬件合成 Surface 的 dstRect，id 为 surfaceNode 的 instanceRootNodeId
- `rs_screen_render_node_drawable.cpp`：HWC 脏区汇总，id 为 screenId

直写模式 (DirectCompose)：

- `rs_main_thread.cpp`：自绘制 Surface 的 dstRect，id 为 instanceRootNodeId
- `rs_main_thread.cpp`：所有 refreshRects 汇总 + HWC 脏区，id 为 screenId

多屏扩展：

- `rs_multi_screen_util.cpp`：扩展屏脏区 + HWC 脏区，id 为 paramScreenId

注意点：

- 不同路径的脏区来源不同（有的用 `dstRect`，有的用 `damageRegionRects`，有的用 `HwcDirtyRegion`），排查时要先确认命中的是哪条路径。
- `RecordCurrentFrameDirty` 内部同时调用 `RecordDirtyToDetector` 和 `RecordDirtyToCollector`，一次调用覆盖两种模式。

## 资源清理

| 场景 | 调用点 | 说明 |
| --- | --- | --- |
| 进程退出 | `rs_render_pipeline_agent.cpp`，通过 `PostUniRenderThreadSyncTask` 投递到渲染线程 | `CleanResourcesByPid`，遍历删除 pid 匹配的 Detector 和 Collector 上下文 |
| Screen 节点销毁 | `rs_screen_render_node_drawable.cpp` | `CleanResourcesByScreenId`，按 nodeId 精确删除 |

注意点：

- `CleanResourcesByPid` 会同时清理检测和采集上下文，且清理在渲染线程执行，与脏区记录在同一线程，避免并发问题。
- `CleanResourcesByPid` 使用 `EraseIf` 遍历，不会遗漏任何 pid 匹配的上下文。
- `CleanResourcesByScreenId` 中 `screenId` 被强转为 `uint64_t` 作为 map key，与注册时 `target.id` 的类型一致。

## 规格限制

| 限制项 | 当前值 | 代码锚点 | 说明 |
| --- | --- | --- | --- |
| 最大连接数（检测） | `MAX_FRAME_STABILITY_CONNECTION_NUM = 10` | `rs_frame_stability_manager.h` | `detectorContexts_` 独立计数 |
| 最大连接数（采集） | `MAX_FRAME_STABILITY_CONNECTION_NUM = 10` | `rs_frame_stability_manager.h` | `collectorContexts_` 独立计数，与检测互不影响 |
| `stableDuration` 范围 | 100ms ~ 5000ms | `rs_frame_stability_types.h` | `MIN_STABLE_DURATION` ~ `MAX_STABLE_DURATION` |
| `changePercent` 范围 | 0.0 ~ 1.0 | `rs_frame_stability_types.h` | `MIN_CHANGE_PERCENT` ~ `MAX_CHANGE_PERCENT` |
| 目标类型 | SCREEN / WINDOW | `rs_frame_stability_types.h` | `FrameStabilityTargetType` 枚举，目前仅支持这两种 |
| PID 鉴权 | 注册进程 PID | `rs_frame_stability_manager.cpp` | 所有操作已注册内容的接口（注销、查询、更新配置等）均需校验 PID 一致性，不匹配则返回 `PERMISSION_DENIED`。新增操作已注册内容的接口时必须包含 PID 校验 |
| 采集模式一次性 | `GetFrameStabilityResult` 后自动销毁 | `rs_frame_stability_manager.cpp` | 不可重复查询，需重新 `StartFrameStabilityCollection` |
| 回调 IPC 模式 | TF_ASYNC | `rs_frame_stability_callback_proxy.cpp` | `OnFrameStabilityChanged` 使用异步发送，不阻塞渲染线程 |
| C2R 接口 IPC 模式 | TF_SYNC | `rs_client_to_render_connection_proxy.cpp` | 所有稳帧 C2R 接口（Register/Unregister/Start/Get/Update）均使用同步发送，上层需感知操作结果 |
| `UpdateFrameStabilityDetection` | 当前空实现 | `rs_frame_stability_manager.cpp` | 仅返回 `SUCCESS`，预留接口，调用方不应依赖此接口做实际功能 |

## 错误码

| 错误码 | 值 | 含义 |
| --- | --- | --- |
| `SUCCESS` | 0 | 成功 |
| `NULL_CALLBACK` | 1 | 注册时回调为空 |
| `TARGET_ID_ALREADY_REGISTERED` | 2 | 目标 ID 已注册 |
| `TARGET_ID_NOT_REGISTERED` | 3 | 目标 ID 未注册 |
| `CONNECTIONS_OCCUPIED` | 4 | 连接数已满 |
| `INVALID_ID` | 5 | 目标 ID 无效（节点不存在） |
| `PERMISSION_DENIED` | 6 | PID 不匹配 |
| `UNKNOWN` | 7 | 未知错误 |

新增接口时，如果现有错误码无法精确描述失败原因，应补充专用错误码。例如 `UpdateFrameStabilityConfig` 中 config 参数校验失败，现有错误码无法区分"参数无效"和"target.id 未注册"，应新增 `INVALID_CONFIG = 8`。错误码应定义在 `rs_frame_stability_types.h` 中，与现有错误码保持连续编号。

## 线程模型

```
┌──────────────────────────────────────────────────────────┐
│ 外部进程 (App)                                            │
│  RSInterfaces → RSRenderInterface → RSRenderPipelineClient│
│  ┌─ CustomFrameStabilityCallback (Stub, 接收回调)        │
└──────────────┬ IPC ──────────────────────────────────────┘
               │
┌──────────────▼───────────────────────────────────────────┐
│ Render Service 进程                                       │
│                                                           │
│  主线程:                                                   │
│    RSClientToRenderConnectionStub → RSRenderPipelineAgent │
│    → ValidateTargetId → RSFrameStabilityManager          │
│    (注册/注销/采集/查询)                                   │
│                                                           │
│  渲染线程:                                                 │
│    RecordCurrentFrameDirty (每帧脏区注入)                  │
│    HandleStabilityTimeout (延迟任务触发)                   │
│    TriggerCallback → RSIFrameStabilityCallback            │
│    → RSFrameStabilityCallbackProxy (IPC 回调到 App)       │
│                                                           │
│  合成线程:                                                 │
│    (无直接参与)                                            │
└──────────────────────────────────────────────────────────┘
```

关键点：

- 注册/注销/查询通过 `PostMainThreadSyncTask` 同步投递到主线程，确保与渲染管线同步。
- 脏区记录在渲染线程每帧调用。
- 延迟任务和回调触发也在渲染线程，通过 `RSUniRenderThread::Instance().PostTask` 投递。
- `detectorMutex_` 和 `collectorMutex_` 分别保护两套上下文 map，支持渲染线程和主线程并发访问。
- `CleanResourcesByPid` 通过 `PostUniRenderThreadSyncTask` 投递到渲染线程，与脏区记录在同一线程。

## 常见故障排查

### 注册后没有收到回调

优先检查：

- target.id 是否正确。SCREEN 类型传 screenId，WINDOW 类型传 surfaceNodeId。
- `changePercent` 是否设得过大。如果设为 1.0，则脏区占比永远不会超过阈值，检测模式不会触发"不稳定"回调；需要等 `stableDuration` 后通过 `RecordDirtyToDetector` 的 `elapsed >= stableDuration` 分支或 `HandleStabilityTimeout` 触发"稳定"回调。
- 是否有 VSync 信号和脏区记录。如果完全没有帧渲染，依赖 `HandleStabilityTimeout` 延迟任务触发。
- 回调 IPC 是否通畅。客户端进程可能已退出，`SendRequest` 失败但服务端不会重试。

### 采集结果不符合预期

优先检查：

- `changePercent` 和 `screenArea` 的值。`screenArea` 取最后一次 `RecordDirtyToCollector` 传入的值，如果中间有变化，最终判断用的是最后一次。
- 是否在采集期间有脏区记录。如果 `RecordCurrentFrameDirty` 对应的 target.id 不匹配，脏区不会被累积。
- `GetFrameStabilityResult` 是否被多次调用。采集模式是一次性的，第一次调用后上下文已销毁，第二次调用会返回 `TARGET_ID_NOT_REGISTERED`。

### 注销返回 PERMISSION_DENIED

优先检查：

- 注销时的 pid 是否与注册时一致。`RSClientToRenderConnection` 自动传入 `remotePid_`，调用方通常不需要手动传 pid，但如果绕过标准 IPC 路径直接调用，可能出现 pid 不匹配。

### 延迟任务未触发

优先检查：

- `RSUniRenderThread` 的 looper 是否正常运行。延迟任务通过 `PostTask` 投递，如果渲染线程 looper 停止，任务不会执行。
- 是否在延迟任务触发前调用了 `UnregisterFrameStabilityDetection`。注销时会 `RemoveTask`，延迟任务不会执行。
- `hasPendingStabilityTask` 是否为 true。如果脏区超阈值时已重置了延迟任务，旧的 `HandleStabilityTimeout` 调用会因为 `hasPendingStabilityTask == false` 而直接返回。

### 动态更新配置后状态未重新判定

优先检查：

- 是否在更新 config 时未重置 state 为 INIT。更新 config 后检查条件发生变化，旧状态判定不再有效，**必须重置 state 为 INIT**，由下一帧脏区数据按新 config 重新驱动状态机流转。例如原来 STABLE 是基于旧 `changePercent=0.5` 判定的，降低为 `changePercent=0.1` 后同样的脏区占比可能已超过新阈值，应判定为 NOTSTABLE。
- 是否在更新 config 后重新投递了延迟任务。`stableDuration` 变化后，旧延迟任务使用旧时长，必须按新时长重新投递。

### 新增 IPC 接口后 Access Verifier 未注册

优先检查：

- `rs_iclient_to_render_connection_ipc_interface_code_access_verifier.cpp` 中是否新增了对应的 case。未注册时 `hasPermission` 默认为 `true`，任何调用方均可访问，与同类型接口的 `IsSystemCalling` 安全策略不一致。
- `rs_irender_service_connection_ipc_interface_code_utils.cpp`（safuzz）中是否新增了 `DECLARE_RENDER_INTERFACE_CODE_NAME`。未注册时 DoS 模拟器测试覆盖率不完整。

## 验证建议

单测目标：

```text
rosen/test/render_service/render_service/unittest/feature/frame_stability/rs_frame_stability_manager_test.cpp
```

接口测试：

```text
rosen/modules/render_service_client/test/Interfacetest/rs_interface_frame_stability_demo.cpp
```

压力测试：

```text
rs_interface_frame_stability_stress.cpp
```

IPC 回调单测：

```text
rosen/test/render_service/render_service_base/unittest/ipc_callbacks/rs_frame_stability_callback_stub_test.cpp
rosen/test/render_service/render_service_base/unittest/ipc_callbacks/rs_frame_stability_callback_proxy_test.cpp
```

构建命令从 OpenHarmony 源码根目录执行：

```sh
hb build graphic_2d -t --skip-download --build-target //foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/feature/frame_stability:RSFrameStabilityManagerTest
```

## 改动检查清单

- 是否读过 `rs_frame_stability_types.h` 和 `rs_frame_stability_manager.h`，再读实现文件，而不是只改内部实现？
- 是否确认 target.id 的类型和含义（SCREEN 用 screenId，WINDOW 用 surfaceNodeId）？
- 是否检查 `stableDuration` 和 `changePercent` 的有效范围？
- 是否确认检测模式的状态机转换不会遗漏回调或重复回调？
- 是否确认动态更新 config 时必须重置 state 为 INIT（配置变更后检查条件变化，旧判定无效），同时重置 accumulatedDirtyRegion、startTime 和延迟任务？
- 是否确认采集模式 `GetFrameStabilityResult` 后上下文自动销毁，调用方不会再重复查询？
- 是否确认 Detector 和 Collector 上下文独立更新，任一不存在不算错误？
- 是否同步 IPC Proxy/Stub 的 Parcel 序列化和反序列化？
- 是否确认回调 IPC 使用 TF_ASYNC，C2R 接口使用 TF_SYNC？
- 是否在 Access Verifier 中注册了新增 IPC 接口码（`IsSystemCalling` 校验）？
- 是否在 DoS 模拟器中注册了新增 IPC 接口码（`DECLARE_RENDER_INTERFACE_CODE_NAME`）？
- 是否遵循防御性编程模式（map 查找 → nullptr 检查 → PID 鉴权 → 操作 → 日志）？所有操作已注册内容的接口都必须校验 PID 一致性，不一致返回 `PERMISSION_DENIED`。
- 是否确认新增接口的 IPC 模式选择正确？上层需要感知操作结果的接口应使用 `TF_SYNC`，仅发起请求不关心结果的接口可使用 `TF_ASYNC`。
- 是否为新增接口补充了必要的错误码？现有错误码无法精确描述失败原因时，应在 `rs_frame_stability_types.h` 中补充专用错误码，用于指导错误类型。
- 是否检查 `CleanResourcesByPid` 和 `CleanResourcesByScreenId` 在进程退出和节点销毁时被正确调用？
- 是否确认新增脏区收集点传入的 target.id 和 screenArea 与注册时一致？
- 是否需要补充单测，覆盖新增或修改的路径？

## 待补充背景

这些内容需要模块责任人后续补充，不能仅靠静态扫描完全确定：

- 稳帧检测的实际使用方（如相机、录屏等）的调用方式和预期行为。
- `UpdateFrameStabilityDetection` 的预期实现和使用场景。
- `WINDOW` 类型 target.id 的实际使用案例和验证结果。
- 历史线上问题，例如回调未触发、采集结果不准确、延迟任务误触发等典型案例。
- 真实设备上不同刷新率（60Hz/90Hz/120Hz）对稳帧检测行为的影响。
- `UpdateFrameStabilityDetection`（`0X16004`）在 DoS 模拟器中未注册的遗漏是否需要补齐。
