# RS 进程入口

## 适用范围

- RenderService 进程启动、SA 注册和生命周期管理
- 单进程/多进程渲染模式切换与配置
- RenderProcess 子进程拉起与连接建立
- 屏幕连接/断连事件传递链路
- HGM、Composer、ScreenManager 等核心组件初始化顺序

## 快速代码地图

`rs_main` 前缀 = `rosen/modules/render_service/main/`，`rs_base` 前缀 = `rosen/modules/render_service_base/`。

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 进程入口 main | `rs_main/render_server/main.cpp` | RSRenderService 进程 main 函数，设置 SCHED_FIFO 与优先级 |
| 服务主体 | `rs_main/render_server/rs_render_service.h/.cpp` | 继承 RSRenderServiceStub，Init/Run/SAMgr 注册 |
| init 服务配置 | `graphic.cfg`（仓根目录） | uid/gid/caps/secon、critical 重启策略、bootevent |
| 可执行构建 | `rosen/modules/render_service/BUILD.gn` | `render_service`（约 741 行）与 `render_process`（约 824 行）target |
| SA 接口定义 | `rs_base/include/platform/ohos/transaction/zidl/rs_irender_service.h` | SA 对外接口 |
| Stub 安全校验 | `rs_main/render_server/transaction/zidl/rs_render_service_stub.h` | OnRemoteRequest + 接口安全校验 |
| 多进程管理器 | `rs_main/render_server/rs_render_multi_process_manager.h/.cpp` | GroupId→Pid 映射，DeathRecipient |
| 多进程状态仓库 | `rs_main/render_server/rs_render_multi_process_manager_repository.h/.cpp` | 连接/屏幕输出状态、子进程死亡熔断 |
| 单进程管理器 | `rs_main/render_server/rs_render_single_process_manager.h/.cpp` | 单进程模式直连，不经 IPC |
| 进程管理基类 | `rs_main/render_server/rs_render_process_manager.h` | 定义 GetServiceToRenderConn 等虚接口 |
| RenderProcess 子进程 | `rs_main/render_process/rs_render_process.h/.cpp` | 子进程入口，ConnectToRenderService 连 RenderServer |
| 子进程 main | `rs_main/render_process/rs_render_process_main.cpp` | RenderProcess 子进程 main 函数 |
| 渲染模式配置 | `rs_main/render_server/rs_render_mode_config_parser.h` | 解析多进程/单进程配置 |
| ScreenManager 监听 | `rs_render_service.h` 内部类 `ScreenManagerListener` | 屏幕连接/断连/属性变更转发到 RenderProcessManager |
| 渲染管线代理 | `rosen/modules/render_service/core/rs_render_pipeline.h` | RenderProcess 内管线代理，与主线程和渲染线程交互 |

## 核心模型

### 进程架构

RenderService 有两种部署模式：

1. **单进程模式**（`RSSingleRenderProcessManager`）：RSMainThread、RSUniRenderThread 与 RSRenderService 在同一进程。
   serviceToRenderConnection 直接是本地对象引用。
2. **多进程模式**（`RSMultiRenderProcessManager`）：RenderServer 进程负责 IPC 转发和屏幕管理；
   每个 GroupId 对应一个 RenderProcess 子进程，子进程内运行 RSMainThread + RSUniRenderThread。
   通过 `RSIServiceToRenderConnection` / `RSIConnectToRenderProcess` 跨进程通信。

### init 服务配置（`graphic.cfg`）

render_service 由 init 按 `graphic.cfg` 拉起，关键字段：

- `path`: `/system/bin/render_service`，`uid: graphics`，`gid: system/tp_host/data_reserve/dev_dma_heap/composer_host`。
- `caps: SYS_NICE`（配合 main.cpp 中 `setpriority(-8)` 与 SCHED_FIFO）。
- `critical: [1, 5, 60]`：init 抑制机制，60s 内服务被频繁拉起超过 5 次则重启系统；
  `once: 0` 表示常驻进程，进程退出后由 init 重新拉起。
- `importance: -20`：标准系统服务优先级（取值 [-20, 19]，数值越小优先级越高）；
  `writepid: /dev/memcg/perf_critical/cgroup.procs` 将 pid 写入 perf_critical 内存 cgroup。
- `secon: u:r:render_service:s0`；`bootevents: bootevent.renderservice.ready`。
- on-restart job `services:restartrender_service`：`reset foundation`、`reset allocator_host`、`reset composer_host`，
  即 RS 重启时联动重置依赖它的服务进程。
- 前置 mkdir： `/data/service/el0/render_service`、shader cache 相关目录。

### 启动流程

```
init 按 graphic.cfg 拉起 /system/bin/render_service
main.cpp
  → signal(SIGPIPE, SIG_IGN)
  → setpriority(-8) + sched_setscheduler(SCHED_FIFO, 1)
  → sptr<RSRenderService>::MakeSptr()
  → RSRenderService::Init()
    → SetParameter(bootevent.renderservice.ready, "false")  // 启动前置低
    → EventRunner/EventHandler 创建
    → Watchdog::AddThread("RenderService", handler_, 5000ms)
    → mallopt tcache（受 RSSystemParameters::GetTcacheEnabled 控制）
    → InitRenderServerConfig()   // 解析多进程/单进程配置
    → InitCCMConfig()            // GraphicFeatureParamManager feature 参数解析
    → CoreComponentsInit()       // ScreenManager + VSyncManager + ComposerManager + HgmInit + FeatureComponentInit
    → RenderProcessManagerInit() // 根据配置创建 Single 或 Multi ProcessManager，注册 ScreenManagerListener
    → SAMgrRegister()            // 注册系统能力（见下节）
    → SetParameter(bootevent.renderservice.ready, "true")   // 全部成功后置高
  → RSRenderService::Run()       // 启动 EventRunner 循环
```

注意：`bootevent.renderservice.ready` 在 Init 入口先置 `false`，SAMgr 注册成功后才置 `true`。
仓内实际等待方为 bootanimation：`BootAnimationController::WaitRenderServiceInit()`
（`frameworks/bootanimation/src/boot_animation_controller.cpp`）轮询 `CheckSystemAbility(RENDER_SERVICE)`
和该系统参数，两者均就绪后才退出等待。

### SA 注册

- SA ID 为 `RENDER_SERVICE`，定义在外部仓 samgr 的 `system_ability_definition.h`，值为 **10**；
  IPC descriptor 为 `ohos.rosen.RenderService`（`rs_irender_service.h`）。
- `SAMgrRegister()`（`rs_render_service.cpp`）流程：
  1. `WaitParameter("bootevent.samgr.ready", "true", 5)`：最多等 5 秒等 samgr 就绪，超时则 Init 失败、进程退出。
  2. `SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager()` 获取 samgr 代理。
  3. `samgr->AddSystemAbility(RENDER_SERVICE, this)` 注册。
- RSRenderService **不是 SystemAbility 基类子类**，没有 OnStart/OnStop 回调；
  生命周期即进程生命周期，由 init 的 critical 策略管理。
- SA 对客户端暴露 3 个方法：`CreateConnection`、`RegisterRenderProcessConnection`、`RemoveConnection`；
  其余渲染接口都挂在 CreateConnection 返回的 `RSIClientToServiceConnection` / `RSIClientToRenderConnection` 上。

### IPC 连接模型

- 应用客户端经 `RSRenderServiceConnectHub::GetClientToServiceConnection()` 获取 `RSIClientToServiceConnection`，
  hub 内部调用 SA 的 `CreateConnection` 建立（`rs_render_service_connect_hub.cpp`）。
- 客户端到渲染进程的 `RSIClientToRenderConnection` 由 `RSRenderPipelineClient` 建立：
  通过 `RSIConnectToRenderProcess::CreateRenderConnection` 按渲染进程（tokenMaskId）创建
  （`rs_render_pipeline_client.cpp`、`rs_render_service_connect_hub.cpp`）。
- `RegisterRenderProcessConnection()` 是 RenderProcess 子进程向 RenderServer 注册的内部接口，
  返回 `RSIRenderToServiceConnection`，不面向应用客户端。
- 多进程模式下，RenderServer 持有 `serviceToRenderConnections_` 和 `composerToRenderConnections_`
  （`rs_render_multi_process_manager_repository.h`，按 ProcessUniqueId 索引），
  负责将屏幕事件和 HGM 信息转发到对应 RenderProcess。
- RenderProcess 子进程启动后通过 `ConnectToRenderService()` 建立 `renderToServiceConnection_`。

### 生命周期与异常恢复

| 对象 | 监控/恢复机制 | 代码锚点 |
| --- | --- | --- |
| render_service 进程 | `once:0` 常驻，退出由 init 拉起；critical `[1,5,60]` 抑制频繁拉起；on-restart 联动 reset | `graphic.cfg` |
| render_service 主循环 | Watchdog "RenderService" 线程，5000ms 心跳 | `rs_render_service.cpp` Init |
| RenderProcess 子进程 | `PR_SET_PDEATHSIG` 父死子亡；DeathRecipient 感知 binder 死亡 | `rs_render_multi_process_manager.cpp` |
| 子进程死亡处理 | `HandleRenderProcessDeath` 清理屏幕输出并通知 `OnProcessDisconnected` | 同上 |
| 子进程死亡熔断 | 180s 内子进程死亡超 2 次，RenderServer `_exit(-1)` 交 init 重启 | `rs_render_multi_process_manager_repository.cpp` |

### 屏幕事件流

`RSScreenManager` → `ScreenManagerListener` → `RSRenderProcessManager`（分发到对应 RenderProcess）→
`RSMainThread::OnScreenConnected` → 创建 ScreenNode。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| SCHED_FIFO + 优先级 -8 | `main.cpp` 中 `sched_setscheduler` + `setpriority` | 保证渲染服务实时调度，减少帧延迟抖动 |
| 单进程/多进程可配 | `RSRenderModeConfigParser` + `RenderProcessManagerInit()` | 车机等多屏场景需要独立 RenderProcess 隔离，手机单进程节省资源 |
| GroupId→进程映射 | repository 的 `groupIdToRenderProcessUniqueId_` | 多屏分组渲染，同组屏幕共享一个 RenderProcess |
| Watchdog 注册 | `Watchdog::GetInstance().AddThread("RenderService", handler_)` | 防止主线程死锁导致系统无响应 |
| SA 注册前等待 samgr ready | `SAMgrRegister()` 中 `WaitParameter("bootevent.samgr.ready", ...)` | 等不到 samgr 就失败退出，交 init 重启 |
| 不继承 SystemAbility 基类 | 直接继承 RSRenderServiceStub 并 `AddSystemAbility` | 生命周期由 init critical 托管，无需 samgr 按需拉起语义 |
| 子进程死亡熔断 180s/2 次 | `CheckAndHandleSubprocessDeathOverflow` | 防止子进程反复崩溃导致屏幕事件风暴，整体重启更可控 |
| DeathRecipient | `RenderProcessDeathRecipient` | 子进程异常退出时回收资源、重建连接 |
| IPC Persistence | `RSIpcPersistenceManager` | 子进程重启后可恢复历史 IPC 数据，减少重连开销 |

## 行为限制和规格边界

| 限制项 | 当前值/行为 | 代码锚点 |
| --- | --- | --- |
| 等 samgr 就绪 | 最多 5 秒，超时 Init 失败进程退出 | `rs_render_service.cpp` `SAMgrRegister` |
| 子进程死亡熔断 | 180s 内 >2 次则 RenderServer 退出 | `rs_render_multi_process_manager_repository.cpp` |
| Watchdog 周期 | 5000ms（RenderService 与 RenderProcess 相同） | 两处 `WATCHDOG_TIMEVAL = 5000` |

## 验证建议

- 模块构建：`//foundation/graphic/graphic_2d/rosen/modules/render_service:render_service`（含 librender_service）。
- 单测：`rosen/test/render_service/render_service/unittest/transaction/rs_render_service_stub_test.cpp`、
  `unittest/pipeline/rs_render_service_test.cpp`。
- 设备验证缺口：critical 重启联动等 init 行为需要真实设备/镜像验证，本仓无法静态确认。

## 待补充背景

- 多进程模式下 RenderProcess 子进程的具体拉起机制（由谁 fork、参数传递）。
- RenderProcessManager 的 GroupId 划分策略和产品配置方法。
- IPC Persistence 的具体数据类型和恢复流程。
- 车机多屏场景下典型 GroupId 配置示例。
- 历史线上子进程 Crash 恢复案例和超时策略。
