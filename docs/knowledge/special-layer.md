# 特殊层（安全图层 / 截屏跳过图层 / 跳过图层 / 黑白名单）

## 适用范围

- 安全图层、跳绘层（SKIP）等特殊层的识别与渲染控制
- 安全屏（Security Display）场景的黑/白遮罩与跳绘决策
- 截屏跳过图层（SNAPSHOT_SKIP、CAPTURE_WINDOW）
- 虚拟屏黑/白名单与全局黑名单（接口、IPC、生效链路）
- 镜像屏与特殊层的交集检测

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSSpecialLayerManager / ScreenSpecialLayerInfo | `rosen/modules/render_service_base/include/common/rs_special_layer_manager.h` | 节点级特殊层状态 + 屏幕级全局登记表 |
| RSSpecialLayerUtils | `rosen/modules/render_service/core/feature/special_layer/rs_special_layer_utils.h` | 特殊层渲染决策与状态聚合 |
| RSScreenManager（黑名单部分） | `rosen/modules/render_service/screen_manager/rs_screen_manager.h` | 全局/虚拟屏黑名单存储与变更通知 |
| RSScreenManagerAgent | `rosen/modules/render_service/screen_manager/public/rs_screen_manager_agent.h` | 黑名单接口参数校验层 |
| RSInterfaces（黑名单部分） | `rosen/modules/render_service_client/core/transaction/rs_interfaces.h` | 客户端公开入口 |
| C2S IPC 命令码 | `rosen/modules/render_service_base/include/platform/ohos/transaction/rs_iclient_to_service_connection_ipc_interface_code.h` | 虚拟屏/特殊层段 `0x002xxx` |
| Stub 分发与 token 校验 | `rosen/modules/render_service/main/render_server/transaction/zidl/rs_client_to_service_connection_stub.cpp` | `descriptorCheckList` + `OnRemoteRequest` |
| 权限校验 | `rosen/modules/render_service/core/transaction/rs_iclient_to_service_connection_ipc_interface_code_access_verifier.cpp` | `IsSystemCalling` 注册 |
| 全局黑名单跨进程同步 | `rosen/modules/render_service/main/render_process/transaction/zidl/rs_service_to_render_connection_proxy.cpp`（`OnGlobalBlacklistChanged`） | Service → Render Process（S2R）分发 |
| 全局黑名单生效 | `rosen/modules/render_service/core/rs_render_pipeline_agent.cpp`（`OnGlobalBlacklistChanged`） | 主线程任务更新渲染侧状态 |
| 渲染线程参数快照 | `rosen/modules/render_service_base/include/params/rs_render_thread_params.h`（`ScreenSpecialLayerParam`） | 全局黑名单/白名单矩形的渲染线程侧副本 |

## 核心模型

### SpecialLayerType 位掩码

`SpecialLayerType` 是位掩码枚举（`SpecialLayerBitmask = uint32_t`），低 10 位为实例标志
（节点自身"是"什么），高位为存在标志（子树中"含有"什么，由实例标志左移
`SPECIAL_TYPE_NUM = 10` 得到）：

| 实例标志 | 值 | 含义 |
| --- | --- | --- |
| `SECURITY` | `0x00000001` | 安全图层（敏感内容） |
| `SKIP` | `0x00000002` | 跳绘层（安全屏/截屏下不渲染，勿与 dynamic_layer_skip 的跳帧混淆） |
| `PROTECTED` | `0x00000004` | DRM 保护层级 |
| `SNAPSHOT_SKIP` | `0x00000008` | 截屏时跳过 |
| `IS_BLACK_LIST` | `0x00000080` | 黑名单层 |
| `IS_WHITE_LIST` | `0x00000100` | 白名单层 |

对应的 `HAS_*` 标志（如 `HAS_SECURITY = SECURITY << 10`）在自底向上合并子树状态时使用。
常用组合常量：`VIRTUALSCREEN_IS_SPECIAL = 0x7`（security/skip/protected）、
`SNAPSHOT_IS_SPECIAL = 0xF`（再加 snapshot_skip）、`HAS_SCREEN_SPECIAL = 0x60000`
（黑白名单存在标志）。单类名单条目上限 `MAX_SPECIAL_LAYER_NUM = 1024`。

### 渲染行为矩阵

| 层类型 | 普通显示 | 安全屏（`IsSecurityDisplay()`） | 截屏（`isSnapshot_`） |
| --- | --- | --- | --- |
| SECURITY | 正常绘制 | 整屏 `DRAW_BLACK`（安全豁免可覆盖） | 多 surface `DRAW_BLACK`；单 surface 且无 blur/自截屏时 `DRAW_WHITE` |
| SKIP | 正常绘制 | 整屏 `SKIP_DRAW` | 整屏 `SKIP_DRAW`（`CAPTURE_WINDOW_NAME` 有特殊处理） |
| PROTECTED | 正常绘制 | 正常绘制（DRM 在别处处理） | 正常绘制 |
| SNAPSHOT_SKIP | 正常绘制 | 正常绘制 | 整屏 `SKIP_DRAW` |
| BLACK_LIST | 正常绘制 | 逐 surface 查黑名单，命中跳绘 | 命中跳绘 |
| WHITE_LIST | 正常绘制 | 逐 surface `SkipDrawByWhiteList()` 判定 | 正常绘制 |

决策入口在 `rs_surface_render_node_drawable.cpp`：

```text
drawType = isSnapshot_ ? RSSpecialLayerUtils::GetDrawTypeInSnapshot(surfaceParams)
                       : RSSpecialLayerUtils::GetDrawTypeInSecurityDisplay(surfaceParams, uniParams);
```

截屏路径早退条件：`captureParam.needCaptureSpecialLayer_` 为真时
`GetDrawTypeInSnapshot` 直接返回 `NONE`（本次截屏需要保留特殊层内容，不做任何跳绘/遮罩）。

`DrawType` 枚举：`NONE`（正常绘制）/ `DRAW_WHITE` / `DRAW_BLACK` / `SKIP_DRAW`。
`DisplaySpecialLayerState` 枚举：`NO_SPECIAL_LAYER` / `HAS_SPECIAL_LAYER` / `CAPTURE_WINDOW`。

### SkipDrawByWhiteList 决策

安全屏下 canvas/effect 类 drawable 在绘制前调用
`RSRenderNodeDrawable::SkipDrawByWhiteList(canvas)`（调用点：
`rs_canvas_render_node_drawable.cpp`、`rs_canvas_drawing_render_node_drawable.cpp`、
`rs_effect_render_node_drawable.cpp`），逻辑要点：

1. 并行绘制（parallel canvas）场景直接不跳过；
2. 虚拟屏白名单为空则不跳过；节点是白名单节点则不跳过
   （判据：白名单根 ID 栈 `RSSpecialLayerManager::whiteListRootIds_` 非空，
   即祖先或自身已被记录为白名单根）；栈为空按命中处理；
3. 命中跳绘时仅绘制子节点（`DrawChildren`）、跳过父节点本体；
4. 其余情况跳绘。

白名单根 ID 栈在每帧屏幕 drawable `PrepareForDraw`（`OnDraw` 第一步）开头
`ClearWhiteListRootIds()`，绘制过程中由 `SetWhiteListRootId` / `ResetWhiteListRootId`
维护；`AutoSpecialLayerStateRecover` 是其 RAII 恢复封装。

### 组件职责

- **RSSpecialLayerManager**（render_service_base，每个渲染节点一个实例）：
  记录节点自身与子树的特殊层位掩码和 NodeId 集合；
  `Set/Find/Get/AddIds/RemoveIds/GetIds` 及带屏幕维度的 `*WithScreen` 变体；
  `FindScreenHasType` 反查拥有某类特殊层的屏幕集合；`MergeChildren` 合并子树状态。
- **ScreenSpecialLayerInfo**（render_service_base，静态全局登记表）：
  按"特殊层类型 → NodeId → 屏幕集合"登记；`Update` / `ClearByScreenId` /
  `ClearEmptyInfo` / `QueryEnableScreen` / `ExistEnableScreen` / `QueryNodeIdsByType`；
  持有主线程侧全局黑名单登记表 `SetGlobalBlackList` / `GetGlobalBlackList`；
  维护镜像屏→源屏映射（`UpdateScreenMirrorSourceMap` 等）。
- **ScreenSpecialLayerParam**（render_service_base，随 `RSRenderThreadParams` 传递的参数快照）：
  持有渲染线程侧全局黑名单副本与白名单矩形
  （`SetGlobalBlackList` / `GetGlobalBlackList`、`AddWhiteListRect` /
  `GetWhiteListRectByScreenId` / `ClearWhiteListRect`）；
  由主线程每帧填充（`RSUniRenderVisitor` 拷贝全局黑名单、`RSMainThread::AddWhiteListRect`
  收集白名单矩形），渲染线程只读，避免跨线程访问 `ScreenSpecialLayerInfo`。
- **RSSpecialLayerUtils**（render_service，静态工具类）：
  绘制类型决策（`GetDrawTypeInSecurityDisplay` / `GetDrawTypeInSnapshot`）、
  镜像屏交集检测（`CheckSpecialLayerIntersectMirrorDisplay`、`NeedProcessSecLayerInDisplay`、
  `HasMirrorDisplay`）、状态聚合（`GetSpecialLayerStateInVisibleRect` /
  `GetSpecialLayerStateInSubTree`、`DealWithSpecialLayer`、`UpdateScreenSpecialLayer`）、
  黑名单合并（`GetMergeBlackListInMainThread` / `GetMergeBlackListInRenderThread`、
  `UpdateInfoWithGlobalBlackList`）、白名单矩形（`CollectWhiteListRect` /
  `SetWhiteListRectToMetaData`，矩形经 Metadata 传给 HWC）、调试 dump（`DumpScreenSpecialLayer`）。

## 黑白名单管理

### 存储

| 数据 | 位置 | 说明 |
| --- | --- | --- |
| 全局黑名单（管理侧） | `RSScreenManager::globalBlackList_` | `globalBlackListMutex_` 保护，变更后通知 |
| 全局黑名单（主线程登记表） | `ScreenSpecialLayerInfo::globalBlackList_` | 主线程任务更新的登记表 |
| 全局黑名单（渲染线程快照） | `RSRenderThreadParams::screenSpecialLayerParam_`（`ScreenSpecialLayerParam`） | `RSUniRenderVisitor` 每帧从登记表拷贝，渲染线程只读 |
| 单屏黑/白名单 | `RSScreen` → `RSScreenThreadSafeProperty` | 仅影响单个虚拟屏；`RSScreenManager` 的 Add 黑/白名单操作由 `specialLayerListMutex_` 保护 |
| 类型黑名单 | `RSScreen` → `RSScreenThreadSafeProperty`（`GetTypeBlackList`） | 按节点类型过滤，单屏 |

### 接口与 IPC 链

黑/白名单接口为 Client → Service 单跳 IPC（C2S 通道，命令码段 `0x002xxx`）：

```text
RSInterfaces::Set/Add/RemoveVirtualScreenBlackList            (render_service_client)
  → RSRenderServiceClient                                     (连接判空 → RENDER_SERVICE_NULL)
    → RSClientToServiceConnectionProxy                        (WriteInterfaceToken + WriteUInt64Vector, TF_ASYNC)
      → IPC codes: SET/ADD/REMOVE_VIRTUAL_SCREEN_BLACKLIST = 0x002003/0x002004/0x002005
        → RSClientToServiceConnectionStub::OnRemoteRequest
            ├─ descriptorCheckList 命中 → ReadInterfaceToken 校验
            ├─ securityManager_.IsInterfaceCodeAccessible（access verifier）
            └─ case 分发
          → RSClientToServiceConnection → RSScreenManagerAgent
            （MAX_SPECIAL_LAYER_NUM 上限 / Add/Remove 空表返回 BLACKLIST_IS_EMPTY，Set 空表仅告警 /
              判空 SCREEN_NOT_FOUND）
              → RSScreenManager
```

`RSScreenManager::Set/Add/RemoveVirtualScreenBlackList` 中
`id == INVALID_SCREEN_ID` 的分支会转发到私有的 `Set/Add/RemoveGlobalBlackList`
（全局黑名单语义寄生在虚拟屏接口之下）；具体屏幕则走 `RSScreen` 的单屏名单。
全局黑名单变更经 `callbackMgr_->NotifyGlobalBlacklistChanged` 通知。

族内接口形态约定（同类新增接口必须遵循）：Set 类为 fire-and-forget
（`int32_t` 返回、无 `repCode`；客户端包装层返回的是传输层 `ERR_OK`，
不是服务端状态）；Add/Remove 类为 `ErrCode` + `repCode` 出参
（`TF_ASYNC` 下回读 repCode，见"已知模式风险"）。不得在同一组接口内混用两种形态。

权限：三个虚拟屏黑名单命令码均在 access verifier 中注册 `IsSystemCalling`。
注意 `IsExclusiveVerificationPassed` 默认 `hasPermission = true`，
**未注册的命令码默认放行**，敏感接口必须显式注册。

### 全局黑名单生效链

```text
RSScreenManager::Set/Add/RemoveGlobalBlackList（globalBlackListMutex_ 内变更）
  → RSScreenCallbackManager::NotifyGlobalBlacklistChanged
    → RSRenderService::ScreenManagerListener::OnGlobalBlacklistChanged   (rs_render_service.cpp)
      → RSRenderProcessManager::OnGlobalBlacklistChanged
          （遍历 GetServiceToRenderConns()，逐 Render Process 分发）
        → RSServiceToRenderConnectionProxy::OnGlobalBlacklistChanged
            [S2R IPC, ON_GLOBAL_BLACKLIST_CHANGED, TF_ASYNC]
          → RSServiceToRenderConnection::OnGlobalBlacklistChanged   (Render Process)
            → RSRenderPipelineAgent::OnGlobalBlacklistChanged
              → PostMainThreadTask:
                  ScreenSpecialLayerInfo::SetGlobalBlackList(globalBlackList)
                  + RSSpecialLayerUtils::UpdateInfoWithGlobalBlackList(nodeMap)

每帧渲染（主线程）：
RSUniRenderVisitor → renderThreadParams->GetMutableScreenSpecialLayerParam()
    .SetGlobalBlackList(ScreenSpecialLayerInfo::GetGlobalBlackList())
（白名单矩形同理：CollectWhiteListRect → RSMainThread::AddWhiteListRect → param，
  每帧开头 ClearWhiteListRect）
```

渲染合并点按线程分为两个变体（`GetMergeBlackList` 已拆分）：

- `GetMergeBlackListInMainThread`：主线程消费（如 `UpdateInfoWithGlobalBlackList`），
  全局黑名单取自 `ScreenSpecialLayerInfo::GetGlobalBlackList()`。
- `GetMergeBlackListInRenderThread`：渲染线程消费
  （`rs_multi_screen_util.cpp`、`rs_uni_dirty_compute_util.cpp` 等），
  全局黑名单取自 `RSRenderThreadParamsManager` 当前参数的
  `ScreenSpecialLayerParam::GetGlobalBlackList()`，避免渲染线程跨线程读登记表。

两者均仅当 `screenProperty.EnableSkipWindow()` 为真时才把全局黑名单并入单屏黑名单，
否则只用单屏黑名单。全局黑名单只影响开启了跳窗能力的虚拟屏。

### 新增 IPC 接口集成清单

为黑/白名单族（或任何 C2S 接口）新增命令码时，以下注册点**缺一不可**。
KB 对比实验中曾出现遗漏 stub token 校验注册（第 4 项）导致新接口运行时不可用的案例：

1. `rs_iclient_to_service_connection_ipc_interface_code.h` 追加命令码
   （历史取值全局唯一，不得复用；按段追加，如 `0x002xxx` 段尾部）
2. `zidl/rs_iclient_to_service_connection.h` 新增纯虚接口
   （grep 所有实现类补齐实现，本族为 Proxy + 服务端 connection 两处）
3. `zidl/rs_client_to_service_connection_proxy.h/.cpp` Proxy 实现
   （`WriteInterfaceToken` → 写参数 → `SendRequest`）
4. `zidl/rs_client_to_service_connection_stub.cpp`：
   **加入 `descriptorCheckList`**（Proxy 写了 token 而 stub 不读，会导致后续参数
   解析整体错位、接口运行时失败）+ `OnRemoteRequest` 新增 case
5. `rs_iclient_to_service_connection_ipc_interface_code_access_verifier.cpp`
   注册权限（敏感操作注册 `IsSystemCalling`；不注册默认放行）
6. 服务端实现 `rs_client_to_service_connection.h/.cpp` → agent → manager
7. safuzz：`rs_irender_service_connection_ipc_interface_code_utils.cpp` 码名表 +
   `configs/test_case_config.json` fuzz 用例
8. 命令码固化测试：
   `rosen/test/render_service/render_service_profiler/unittest/standard/platform/ohos/rs_irender_service_connection_ipc_interface_code_test.cpp`
   的 `FixEnumValue003`（C2S 段）中为新命令码追加 `EXPECT_EQ` 断言；
   遗漏会导致命令码值变更缺少回归锚点（KB 对比实验中两个 Agent 均遗漏此项）
9. 语义收窄/接口拆分时：除迁移生产与 demo 调用方外，还需全量枚举以旧参数
   断言旧行为的既有用例并同步改造（见"已知风险"第 1 条清单）；
   只迁移调用点、不迁移断言，旧用例仍会失败
10. 测试锚点见下节

已知模式风险：

- 黑/白名单族 Proxy 在 `TF_ASYNC` 下仍 `reply.ReadInt32()` 读 repCode，
  异步语义下回读可靠性依赖既有实现；新增接口沿用该模式时保持与同族接口一致即可，
  不要在单个接口上混用不同的 reply 约定。
- 客户端包装层（`RSRenderServiceClient`）Set 类接口单测断言返回 `0`，
  断言的是传输层 `ERR_OK`，不代表服务端状态；
  不要据此推断服务端分支行为，也不要在改造时误改这类断言的语义。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 实例/存在双层位掩码 | `IS_*` 低 10 位 + `HAS_*` 左移 10 位 | 自底向上合并子树状态时区分"自身是"与"子树含有" |
| 全局黑名单独立于单屏黑名单 | `globalBlackList_` + `ScreenSpecialLayerInfo` 登记表 | 全局黑名单适用所有（开启跳窗的）虚拟屏，管理侧与渲染侧分离，经主线程任务同步 |
| 渲染线程黑名单快照 | `ScreenSpecialLayerParam`（随 `RSRenderThreadParams` 传递） | 主线程每帧拷贝全局黑名单/白名单矩形，渲染线程只读快照，避免跨线程访问登记表 |
| 全局黑名单仅对 `EnableSkipWindow` 屏生效 | `GetMergeBlackListIn*` 分支 | 跳窗能力是虚拟屏内容过滤的前提，未开启的屏不做名单过滤 |
| 白名单根 ID 用栈 | `whiteListRootIds_` + `AutoSpecialLayerStateRecover` | 绘制过程嵌套进出白名单子树，RAII 保证状态恢复 |
| 截屏单 surface 用白遮罩 | `DRAW_WHITE`（单 surface 无 blur/自截屏） | 与多 surface 黑遮罩区分，保持单窗口截屏的视觉语义 |
| 白名单矩形经 Metadata 传 HWC | `SetWhiteListRectToMetaData` | 虚拟屏白名单区域需要硬件合成侧感知 |
| 黑名单变更异步生效 | `PostMainThreadTask` 更新主线程登记表 | 名单变更在服务端 IPC 线程发起，渲染状态必须在主线程更新 |

## 已知风险

- **旧接口双语义**：`Set/Add/RemoveVirtualScreenBlackList(INVALID_SCREEN_ID, ...)`
  实际执行全局黑名单操作；若未来拆分为独立的全局黑名单接口，需同步迁移既有调用方
  与断言旧行为的用例，并处理旧语义返回码变化。调用方/用例清单：
  - `graphic_test/test/rs_func_feature/multi_screen/multi_screen_test.cpp` 多处
    `Set/Add/RemoveVirtualScreenBlackList(INVALID_SCREEN_ID, ...)`；
  - `rosen/modules/render_service_client/test/screen_special_layer_demo_utils.h` 的
    `MODIFY_LIST_FUNC_MAP`：`SetBlackList/AddBlackList/RemoveBlackList` 三个 lambda，
    `Params.screenId` 默认即 `INVALID_SCREEN_ID`，默认入参命中全局语义
    （注意：同文件 `EnableGlobalBlackList` 只调 `SetCastScreenEnableSkipWindow`，
    不受拆分影响，勿混淆）；
  - `rosen/modules/render_service_client/test/Interfacetest/rs_interface_stable_demo.cpp`
    的 `SetVirtualScreenBlackList(-1, ...)` 边界调用（语义收窄后返回非 SUCCESS）；
  - `rosen/test/render_service/render_service/unittest/pipeline/main_thread/rs_render_service_connection_test.cpp`
    的 `ModifyVirtualScreenBlackList001/003`：以 `INVALID_SCREEN_ID` 直连
    connection 并断言 `SUCCESS/ERR_OK`，依赖被移除的转发语义，拆分后必然失败，
    必须与调用方一并迁移（KB 对比实验中两个 Agent 均遗漏此文件）。
- **TF_ASYNC + repCode**：见"新增 IPC 接口集成清单"末尾说明。
- **默认放行**：access verifier 未注册命令码默认 `hasPermission = true`。

## 测试锚点

| 测试 | 路径 |
| --- | --- |
| 特殊层工具单测 | `rosen/test/render_service/render_service/unittest/feature/special_layer/rs_special_layer_utils_test.cpp` |
| 特殊层管理器单测 | `rosen/test/render_service/render_service_base/unittest/common/rs_special_layer_manager_test.cpp` |
| 客户端接口单测 | `rosen/modules/render_service_client/test/unittest/rs_interfaces_test.cpp`（`*VirtualScreenBlackList*` 用例） |
| 服务端连接层单测 | `rosen/test/render_service/render_service/unittest/pipeline/main_thread/rs_render_service_connection_test.cpp`（`ModifyVirtualScreenBlackList*`，`INVALID_SCREEN_ID` 双语义用例） |
| 命令码固化测试 | `rosen/test/render_service/render_service_profiler/unittest/standard/platform/ohos/rs_irender_service_connection_ipc_interface_code_test.cpp`（`FixEnumValue003`，C2S 段） |
| 多屏设备测试 | `graphic_test/test/rs_func_feature/multi_screen/multi_screen_test.cpp` |
| 手工 demo | `rosen/modules/render_service_client/test/screen_special_layer_demo.cpp` |
