# 代理合成（Delegate Composite）逻辑梳理

> 本文档梳理 `foundation/graphic/graphic_2d` 仓内"代理合成"特性（Web 端
> WebProxyComposer 模式）的代码全貌、调用链与数据流，便于后续做特性隔离。
> 路径均为本仓相对路径。

## 1. 适用范围

"代理合成"在仓内特指 `delegate_composite` 特性：让 Web 进程（WebProxyComposer）
承担 SurfaceNode 的部分合成工作，Render Service 通过 IPC 回调通知 Web 端
"完成"，并由客户端 BufferManager 将"待合成 SurfaceBuffer + 目标/源矩形 +
合成模式"等参数投递到服务端，服务端在合适的时机触发 OnComplete 回调。

该特性在仓内被以下产品形态使用：

- Web 页面（WebProxyComposer 进程内的 `RosenWeb` SurfaceNode）。
- 任何走代理合成路径的 Ability / Web 渲染场景。

## 2. 快速代码地图

### 2.1 客户端（render_service_client）

| 文件 | 行数级别 | 作用 |
| --- | --- | --- |
| `rosen/modules/render_service_client/core/feature/delegate_composite/rs_delegate_composite_buffer_manager.h/cpp` | 110 / 323 | 每个 SurfaceNode 持有一个 `RSDelegateCompositeBufferManager`，负责 `SetBuffer` / `ReleaseBuffer` / `CleanBuffer` / `SetDamageRegion` / `SetBufferTransform` / `SetDelegateDstRect` / `SetDelegateSrcRect` / `SetDelegateMode` 等 |
| `rosen/modules/render_service_client/core/feature/delegate_composite/rs_delegate_composite_listener.h/cpp` | 71 / 202 | `SurfaceTransactionListener`（每个 listenerId 一个） + `SurfaceNodeBufferReleaseListener`（单例）持有 IPC Stub 并注册到 `RSRenderPipelineClient` |

### 2.2 服务端 / 公共（render_service_base）

| 文件 | 行数级别 | 作用 |
| --- | --- | --- |
| `rosen/modules/render_service_base/include/feature/delegate_composite/rs_delegate_composite_callback_manager.h` | 92 | 单例 `RsDelegateCompositeCallbackManager`，对外提供 `SetInfo` / `GetInfo` / `RegisterSurfaceTransactionListener` / `NotifySurfaceTransactionListener` / `ProcessDelegateCompositeCommand` / `PrepareDelegateCompositeCommand` 等 |
| `rosen/modules/render_service_base/src/feature/delegate_composite/rs_delegate_composite_callback_manager.cpp` | 690 | 上述单例实现，含 4 张互斥保护的 map |
| `rosen/modules/render_service_base/include/feature/delegate_composite/rs_delegate_composite_params.h/cpp` | 45 / 66 | 每个 SurfaceRenderNode 一个 `RsDelegateCompositeParams`，存 dstRect / srcRect / isSetDelegateMode_（atomic）/ name / nodeId |
| `rosen/modules/render_service_base/include/command/rs_delegate_composite_command.h` | — | 命令枚举 `RSDelegateCompositeCommandType`：`TRANSACTION_BUFFER=0`、`SURFACE_TRANSACTION_CMD=1`；以及 `TransactionBufferCommand`、`SurfaceTransactionCommand` |
| `rosen/modules/render_service_base/src/command/rs_delegate_composite_command.cpp` | — | 命令的 Marshalling / Unmarshalling |
| `rosen/modules/render_service_base/include/ipc_callbacks/rs_delegate_composite_callback.h` | — | IPC 接口 `RSISurfaceTransactionListener` / `RSISurfaceNodeBufferReleaseCallback`（IRemoteBroker） |
| `rosen/modules/render_service_base/include/ipc_callbacks/rs_delegate_composite_callback_interface_code.h` | — | IPC 接口码 |
| `rosen/modules/render_service_base/src/ipc_callbacks/rs_delegate_composite_callback_proxy.h/cpp` | 46 / ~150 | 客户端代理 `RSWebProxyComposerCallbackProxy` / `SurfaceNodeBufferReleaseCallbackProxy` |
| `rosen/modules/render_service_base/src/ipc_callbacks/rs_delegate_composite_callback_stub.h/cpp` | — | 服务端 Stub `RSWebProxyComposerCallbackStub` / `SurfaceNodeBufferReleaseCallbackStub`（实现 OnComplete 派发） |

### 2.3 集成点（必须配合改动的位置）

| 文件 | 位置 | 集成点 |
| --- | --- | --- |
| `rosen/modules/render_service_client/core/ui/rs_surface_node.h` | L57, L390–401, L489 | 前向声明 `RSDelegateCompositeBufferManager`；SetBuffer/ReleaseBuffer/CleanBuffer/SetDesiredPresentTime/SetDamageRegion/SetBufferTransform/SetDelegateDstRect/SetDelegateSrcRect/SetDelegateMode 接口；成员 `delegateCompositeBufMgr_`（已包在 `#ifndef ROSEN_CROSS_PLATFORM` 内） |
| `rosen/modules/render_service_client/core/ui/rs_surface_node.cpp` | L44–45, L855–857, L1519–1583, ~L1592 | 包含 BufferManager 头；析构置空 `delegateCompositeBufMgr_`；9 个 Set* 包装方法；构造时 `make_shared` 初始化 |
| `rosen/modules/render_service_base/src/pipeline/rs_surface_render_node.cpp` | L4390–4455 | `delegateCompositeParams_` 创建 + SetDelegateDstRect / GetDelegateDstRect / SetDelegateSrcRect / GetDelegateSrcRect / GetDelegateMode / SetDelegateMode 包装 |
| `rosen/modules/render_service_base/src/params/rs_surface_render_params.cpp` | L766, L1018–1029 | 复制/设置 `isWebProxyComposerNode_` |
| `rosen/modules/render_service_base/src/transaction/transaction.cpp` | L30 | `#include "feature/delegate_composite/rs_delegate_composite_callback_manager.h"` |
| `rosen/modules/render_service_base/src/platform/ohos/rs_render_pipeline_client.cpp` | L1177–1210 | `RegisterSurfaceTransactionListener` / `UnRegisterSurfaceTransactionListener` / `RegisterSurfaceNodeBufferReleaseListener` |
| `rosen/modules/render_service_base/src/platform/windows/rs_render_pipeline_client.cpp` | L319–337 | 上述三个方法的 Windows 平台桩 |
| `rosen/modules/render_service_base/src/platform/darwin/rs_render_pipeline_client.cpp` | L303+ | 上述方法的 Darwin 平台桩 |
| `rosen/modules/render_service_base/src/platform/ohos/transaction/zidl/rs_client_to_render_connection_proxy.h/.cpp` | L195–197, L2128–2190 | 上述方法在 IPC Proxy 里的实现 |
| `rosen/modules/render_service_base/include/platform/common/rs_system_properties.h` | L421 | `static bool GetRsDelegateCompositeCleanCacheDfxEnable();` |
| `rosen/modules/render_service_base/src/platform/ohos/rs_system_properties.cpp` | L1874 | 读取 `persist.graphic.enable_delegate_composite_dfx` |
| `rosen/modules/render_service_base/src/platform/windows/rs_system_properties.cpp` | L887 | 桩 |
| `rosen/modules/render_service_base/src/platform/darwin/rs_system_properties.cpp` | L893 | 桩 |
| `rosen/modules/render_service_client/BUILD.gn` | L192–193 | 2 个 `delegate_composite` 源文件 |
| `rosen/modules/render_service_base/BUILD.gn` | 见 §6 | 多个 `delegate_composite` 源文件 + IPC 源文件 |

## 3. 核心模型

### 3.1 客户端：BufferManager

`RSDelegateCompositeBufferManager` 与一个 `RSSurfaceNode` 一一对应（节点
持 `shared_ptr`），绑定 `RSUIContext` / `Surface` / `NodeId` / 名称。BufferManager
把每次"设置 Buffer / 矩形 / 模式"打包成 `TransactionBufferCommand`，并把
SurfaceBuffer / Rect / DelegateMode 序列化为 Parcel 数据，通过
`RSTransactionProxy` 投递到服务端；同一个 Parcel 里还塞一个
`SurfaceTransactionCommand` 标记"本帧事务结束"。

```text
SetBuffer → 序列化 SurfaceBuffer 到 Parcel
         → TransactionBufferCommand::SET_BUFFER   (subCmd)
         → TransactionBufferCommand               (主命令)
         → SurfaceTransactionCommand              (事务结束)
```

释放时则走 `ReleaseBuffer`（带 sequence + SyncFence）和 `CleanBuffer`（清理
整帧）。

### 3.2 客户端：Listener

两个 Listener 类：

- `SurfaceTransactionListener`（per listenerId）：构造时 new 一个
  `RSWebProxyComposerCallbackStub` 实例，调用
  `RSRenderPipelineClient::RegisterSurfaceTransactionListener(stub, uniqueId_)`。
  Stub 接收到 `OnComplete(timestamp, srcId, seqNums)` 后通过
  `RsDelegateCompositeCallbackManager::NotifySurfaceTransactionListener`
  派发到用户回调。析构时 `UnRegisterSurfaceTransactionListener(uniqueId_)`。
- `SurfaceNodeBufferReleaseListener`（singleton）：new 一个
  `SurfaceNodeBufferReleaseCallbackStub` 全局注册到
  `RegisterSurfaceNodeBufferReleaseListener`，接收 `OnBufferComplete` 通知。

### 3.3 服务端：CallbackManager

`RsDelegateCompositeCallbackManager` 是单例（`Instance()`），内含 4 张 map：

- `webProxyComposerTid_` + 1 张按 `webProxyComposerTid` 索引的"buffer 释放序列"
  信息表（`OnBufferComplete` 时使用）。
- 1 张按 `NodeId` 索引的 `RSISurfaceTransactionListener` 注册表（WebProxyComposer
  调 `RegisterSurfaceTransactionListener` 时注册）。
- 1 张按 `NodeId` 索引的"待通知 transaction 序列号"表（在 transaction 流末尾
  `NotifySurfaceTransactionListener` 时被消费）。
- 1 张按 `NodeId` 索引的 `RSISurfaceNodeBufferReleaseCallback` 注册表。

对外接口：

- `SetInfo(NodeId, ...)` / `GetInfo(NodeId)`：在 transaction 流解析过程中
  把"每个节点当帧的 listener / seqNums"暂存起来。
- `RegisterSurfaceTransactionListener` / `UnRegisterSurfaceTransactionListener`
  / `NotifySurfaceTransactionListener` / `RegisterReleaseListener`。
- `ProcessDelegateCompositeCommand(Parcel&)`：在 transaction 反序列化时
  解析 `RSDelegateCompositeCommandType` 两种命令。
- `PrepareDelegateCompositeCommand(Parcel&, std::function)`：构造端把 Buffer /
  Rect / Mode 写进 Parcel。

### 3.4 命令对象

- `RSDelegateCompositeCommandType::TRANSACTION_BUFFER=0`：主携带 SET_BUFFER /
  SET_RECT 两种子命令的 `TransactionBufferCommand`。
- `RSDelegateCompositeCommandType::SURFACE_TRANSACTION_CMD=1`：
  `SurfaceTransactionCommand`，仅带 NodeId、seqNums 队列、timestamp 等元信息，
  告诉服务端"本事务完成"。

## 4. 调用链与数据流

### 4.1 SetBuffer 流（Web 端 → RS 服务端）

```text
RosenWeb
  └─ RSSurfaceNode::SetBuffer(buffer, fence, cb)
        └─ delegateCompositeBufMgr_->SetBuffer(...)
              ├─ RSTransaction::Begin/...
              ├─ TransactionBufferCommand(SET_BUFFER + SurfaceBuffer)
              ├─ SurfaceTransactionCommand(seqNums, timestamp)  // 事务结束
              └─ RSTransaction::Flush → IPC → RSTransactionProxy → RS
                                                                  ↓
                                  RsDelegateCompositeCallbackManager
                                    ├─ ProcessDelegateCompositeCommand
                                    │   ├─ TRANSACTION_BUFFER → 解析 SET_BUFFER
                                    │   └─ SURFACE_TRANSACTION_CMD → SetInfo(seqNums)
                                    └─ 一帧结束 → NotifySurfaceTransactionListener
                                          └─ 调 RSISurfaceTransactionListener::OnComplete
                                                └─ Web 端 RSWebProxyComposerCallbackStub 收到
                                                      └─ SurfaceTransactionListener 派发用户回调
```

### 4.2 释放 Buffer 流（Web 端请求归还 / 同步释放）

```text
Buffer release
  └─ RSSurfaceNode::ReleaseBuffer(seq, fence) / CleanBuffer(...)
        └─ delegateCompositeBufMgr_->ReleaseBuffer / CleanBuffer
              └─ RSTransaction → IPC → RsDelegateCompositeCallbackManager
                                                ↓
                              标记/清理 webProxyComposerTid_ 对应缓冲
                                                ↓
                          SurfaceNodeBufferReleaseCallback::OnBufferComplete
                                                ↓
                SurfaceNodeBufferReleaseListener 单例派发
```

### 4.3 监听器注册流

```text
RosenWeb 第一次 SetBuffer 之前
  └─ SurfaceTransactionListener::SurfaceTransactionListener(...)
        ├─ new RSWebProxyComposerCallbackStub()
        └─ RSRenderPipelineClient::RegisterSurfaceTransactionListener(stub, uniqueId_)
              └─ IPC → RSClientToRenderConnectionProxy::RegisterSurfaceTransactionListener
                    └─ RS 端把 stub 加入 RsDelegateCompositeCallbackManager 的注册表

SurfaceNodeBufferReleaseListener::SurfaceNodeBufferReleaseListener()
  └─ new SurfaceNodeBufferReleaseCallbackStub()
  └─ RSRenderPipelineClient::RegisterSurfaceNodeBufferReleaseListener(stub)
        └─ 全局单例，进程生命周期内只注册一次
```

## 5. 关键决策点（设计背景）

1. **为什么把 Listener 做成 per-uniqueId**：每个 WebView 实例都对应一个
   `uniqueId`（同 listenerId 内不互相干扰），析构时 UnRegister 干净。
2. **为什么 `RsDelegateCompositeParams` 同时放在 Server 与 Client 两侧的
   SurfaceNode**：Client 侧维护 `delegateCompositeBufMgr_`（写入），Server 侧
   `delegateCompositeParams_`（读取/同步给 renderThread / drawable），数据流
   跨进程需要两端都有 holder。
3. **为什么用 `RSWebProxyComposerCallback*` 命名**：这套 IPC 接口本意就是给
   "Web 端代理合成器"调用，外部进程以 WebProxyComposer 身份注册
   OnComplete 监听并接收 buffer 释放通知。
4. **为什么 `isSetDelegateMode_` 是 atomic**：该标志从 Client 流向 Server
   SurfaceRenderParams，需要在多线程场景下无锁读。
5. **为什么有 `CleanCacheDfx` 开关**：DUMP 调试时主动清空缓存
   （`persist.graphic.enable_delegate_composite_dfx`）。

## 6. 构建配置现状

### 6.1 `graphic_config.gni` 中的 CCM 锚点（line 238–244）

仓内已存在一个空壳的 `graphic_2d_delegator_configs`：

```gni
graphic_2d_delegator_configs = {
}
if (path_exists("//foundation/graphic/graphic_2d_ext/delegator/config.gni")) {
  graphic_2d_delegator_configs = {
    import("//foundation/graphic/graphic_2d_ext/delegator/config.gni")
  }
}
```

当 `graphic_2d_ext/delegator/config.gni` 存在时，会被 `import` 进
`graphic_2d_delegator_configs`；否则保持空。**目前此 gni 变量在本仓内还没有
被任何 BUILD.gn 引用**，是预留给厂商扩展的插槽。

### 6.2 DDGR 参考模式

DDGR 走的是 `path_exists + graphic_2d_ext_feature_enable_ddgr + ENABLE_DDGR_OPTIMIZE`
的"硬开关 + 软开关"两层模型；本仓的 `RS_ENABLE_PREFETCH` 走的是"在
`graphic_config.gni` 直接定义 + 推到 `gpu_defines`"的单开关模型。
两种都可以参考，但鉴于代理合成已经是"大特性 + 跨模块"形态，**建议沿用 DDGR
双层模型**：在 `graphic_config.gni` 增加 `RS_ENABLE_DELEGATE_COMPOSITE`
作为开/关硬开关，并保留 `graphic_2d_delegator_configs` 插槽给 ext。

## 7. 隔离建议（与代码改动清单配套）

> 完整改动见任务输出，下文只描述方向。详细隔离方案另见随本 md 同步输出的
> 代码 diff 注释。

- **新增** `graphic_2d_feature_enable_delegate_composite`（默认 `true`，保持
  现有行为）。
- **新增** `RS_ENABLE_DELEGATE_COMPOSITE` 宏到 `gpu_defines`，由上述开关控制。
- **包夹** 所有 §2.1、§2.2 列出的源/头文件：`#ifdef RS_ENABLE_DELEGATE_COMPOSITE` …
  `#endif`。
- **包夹** §2.3 列出的所有集成点（rs_surface_node.h/cpp、rs_surface_render_node.cpp、
  rs_surface_render_params.cpp、transaction.cpp、3 个 platform 的
  rs_render_pipeline_client.cpp、rs_client_to_render_connection_proxy.h/.cpp、
  3 个 platform 的 rs_system_properties.cpp/h）。
- **条件** 改造 `render_service_client/BUILD.gn` 与 `render_service_base/BUILD.gn`
  的 `sources`，把 §2.1 / §2.2 文件包在 `if (graphic_2d_feature_enable_delegate_composite)` 里。
- **保留** `graphic_2d_delegator_configs` 作为 ext 厂商配置插槽，不在本仓强制引用。

## 8. 静态验证清单

- `git diff --check` 检查无空白/换行异常。
- `rg "delegate_composite|DelegateComposite|RSWebProxyComposer|delegateComposite"` 在新宏关闭
  路径下应只出现在 `#ifdef RS_ENABLE_DELEGATE_COMPOSITE` 块内。
- `BUILD.gn` 中 `graphic_2d_feature_enable_delegate_composite = false` 时，
  上述 sources 应被排除；`true` 时保持现状。
- 不在本仓范围内：实际 GN 构建 + 真机验证，留待用户在集成环境中确认。
