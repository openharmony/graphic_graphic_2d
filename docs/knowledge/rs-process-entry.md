# RS 进程入口

## 适用范围

- RenderService 进程启动、SA 注册和生命周期管理
- 单进程/多进程渲染模式切换与配置
- RenderProcess 子进程拉起与连接建立
- 屏幕连接/断连事件传递链路
- HGM、Composer、ScreenManager 等核心组件初始化顺序

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 进程入口 main | `rosen/modules/render_service/main/render_server/main.cpp` | RSRenderService 进程 main 函数，设置 SCHED_FIFO 调度策略与优先级 |
| 服务主体 | `rosen/modules/render_service/main/render_server/rs_render_service.h/.cpp` | 继承 RSRenderServiceStub，Init/Run/SAMgr 注册 |
| 多进程管理器 | `rosen/modules/render_service/main/render_server/rs_render_multi_process_manager.h` | 管理多 RenderProcess 子进程，GroupId→Pid 映射，DeathRecipient |
| 单进程管理器 | `rosen/modules/render_service/main/render_server/rs_render_single_process_manager.h` | 单进程模式下 serviceToRender/composerToRender 直连 |
| 进程管理基类 | `rosen/modules/render_service/main/render_server/rs_render_process_manager.h` | 定义 GetServiceToRenderConn 等虚接口 |
| RenderProcess 子进程 | `rosen/modules/render_service/main/render_process/rs_render_process.h/.cpp` | 子进程入口，ConnectToRenderService 建立到 RenderServer 的 IPC |
| 子进程 main | `rosen/modules/render_service/main/render_process/rs_render_process_main.cpp` | RenderProcess 子进程 main 函数 |
| 渲染模式配置 | `rosen/modules/render_service/main/render_server/rs_render_mode_config_parser.h` | 解析多进程/单进程配置 |
| ScreenManager 监听 | `rs_render_service.h` 内部类 `ScreenManagerListener` | 屏幕连接/断连/属性变更转发到 RenderProcessManager |
| 渲染管线代理 | `rosen/modules/render_service/main/render_process/rs_render_pipeline.h` | RenderProcess 内管线代理，与主线程和渲染线程交互 |

## 核心模型

### 进程架构

RenderService 有两种部署模式：

1. **单进程模式**（`RSSingleRenderProcessManager`）：RSMainThread、RSUniRenderThread 与 RSRenderService 在同一进程。serviceToRenderConnection 直接是本地对象引用。
2. **多进程模式**（`RSMultiRenderProcessManager`）：RenderServer 进程负责 IPC 转发和屏幕管理；每个 GroupId 对应一个 RenderProcess 子进程，子进程内运行 RSMainThread + RSUniRenderThread。通过 `RSIServiceToRenderConnection` / `RSIConnectToRenderProcess` 跨进程通信。

### 启动流程

```
main.cpp
  → sptr<RSRenderService>::MakeSptr()
  → RSRenderService::Init()
    → InitRenderServerConfig()   // 解析多进程/单进程配置
    → InitCCMConfig()            // Feature 参数解析
    → CoreComponentsInit()       // ScreenManager + VSyncManager + ComposerManager + HgmInit
    → RenderProcessManagerInit() // 根据配置创建 Single 或 Multi ProcessManager
    → SAMgrRegister()            // 注册系统能力
  → RSRenderService::Run()       // 启动 EventRunner 循环
```

### IPC 连接模型

- 客户端通过 `RegisterRenderProcessConnection()` 获取 `RSIClientToServiceConnection` + `RSIClientToRenderConnection`。
- 多进程模式下，RenderServer 持有 `serviceToRenderConnections_`（按 pid 索引）和 `composerToRenderConnections_`，负责将屏幕事件和 HGM 信息转发到对应 RenderProcess。
- RenderProcess 子进程启动后通过 `ConnectToRenderService()` 建立 `renderToServiceConnection_`。

### 屏幕事件流

`RSScreenManager` → `ScreenManagerListener` → `RSRenderProcessManager`（分发到对应 RenderProcess）→ `RSMainThread::OnScreenConnected` → 创建 ScreenNode。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| SCHED_FIFO + 优先级 -8 | `main.cpp` 中 `sched_setscheduler` + `setpriority` | 保证渲染服务实时调度，减少帧延迟抖动 |
| 单进程/多进程可配 | `RSRenderModeConfigParser` + `RenderProcessManagerInit()` | 车机等多屏场景需要独立 RenderProcess 隔离，手机单进程节省资源 |
| GroupId→Pid 映射 | `RSMultiRenderProcessManager::groupIdToRenderProcessPid_` | 多屏分组渲染，同组屏幕共享一个 RenderProcess |
| Watchdog 注册 | `Watchdog::GetInstance().AddThread("RenderService", handler_)` | 防止主线程死锁导致系统无响应 |
| DeathRecipient | `RenderProcessDeathRecipient` | 子进程异常退出时回收资源、重建连接 |
| IPC Persistence | `RSIpcPersistenceManager` | 子进程重启后可恢复历史 IPC 数据，减少重连开销 |

## 待补充背景

- 多进程模式下 RenderProcess 子进程的具体拉起机制（由谁 fork、参数传递）。
- RenderProcessManager 的 GroupId 划分策略和产品配置方法。
- IPC Persistence 的具体数据类型和恢复流程。
- 车机多屏场景下典型 GroupId 配置示例。
- 历史线上子进程 Crash 恢复案例和超时策略。
