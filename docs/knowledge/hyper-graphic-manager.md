# Hyper Graphic Manager 知识路由

## 适用场景

改动涉及以下任一内容时，必须先读本文：

- LTPO、LTPS、fps、frame rate、refresh rate、刷新率切换、刷新率拆分。
- 刷新率投票、软 VSync、DVSync、AdaptiveVsync、触控提频、空闲降频、多应用帧率策略。
- `rosen/modules/hyper_graphic_manager/` 或 Render Service 中 HGM 初始化、IPC、回调和 per-frame 同步。

## 模块定位

HGM 是 OpenHarmony 图形子系统中的 Hyper Graphic Manager 模块，编译为
`libhyper_graphic_manager` 共享库。
它通过 XML 策略和投票框架动态决策显示刷新率，典型目标包括并不限于 30Hz、60Hz、90Hz 和 120Hz。
本文所有代码路径均以仓库根目录为基准。

## 目录结构

```text
rosen/modules
├── hyper_graphic_manager/
│   ├── core/
│   │   ├── frame_rate_manager/                                           # HgmFrameRateManager 和各类帧率子管理器
│   │   ├── hgm_screen_manager/                                           # HgmCore、HgmScreen、HgmScreenInfo
│   │   ├── config/                                                       # XML 解析、日志、IPC 回调管理、用户自定义配置
│   │   ├── utils/                                                        # 公共类型、投票器、状态机、timer、LRU cache
│   │   ├── soft_vsync_manager/                                           # 应用维度软 VSync 分频
│   │   └── native_display_soloist/                                       # OH_DisplaySoloist 应用帧率控制 C API
│   └── frame_rate_vote/                                                  # RSFrameRateVote、RSVideoFrameRateVote
├── render_service/
│   ├── core/feature/hyper_graphic_manager/                               # 模块间交互，包括IPC事件输入，渲染端数据输入，刷新率数据输出
│   └── composer/composer_service/external_depend/hyper_graphic_manager/  # 切换屏幕刷新率档位和设置送显模式，跟硬件相关的业务
├── render_service_base/src/
│   ├── transaction/rs_hgm_config_data.cpp                                # 应用端动画速度对应的帧率值配置数据
│   ├── transaction/rp_hgm_config_data.cpp                                # 渲染端动画速度对应的帧率值配置数据
│   └── feature/hyper_graphic_manager/                                    # frameRateLinker管理器
└── render_service_client/core/
    └── feature/hyper_graphic_manager/                                    # 应用端的刷新率业务包括对外接口及UI动画速度帧率计算等
```

## 运行链路

### 服务端初始化

Render Service 启动时，`RSRenderService::Init()` 只执行一次，并调用 `RSRenderService::HgmInit()`：

1. `HgmCore::Instance()` 首次触发单例构造。
2. `InitXmlConfig()` 读取 HGM XML 策略配置。
3. 创建 `hgmFrameRateMgr_`，其构造过程会设置 timer 和 callback。
4. 注册 `persist.sys.mode` 系统属性监听，用于刷新率模式变化。
5. `SetLtpoConfig()` 将 XML 中的 LTPO 配置落到成员变量。

随后创建 `hgmContext_` 并初始化 `HgmTaskHandleThread`。线程任务会设置强制刷新回调、
调用 `frameRateManager->Init()` 初始化子模块，并注册 HGM 配置更新和自适应 VSync 更新回调。
这些回调只在数据实际变化时通过 `hgmDataChangeTypes_` 标记同步到渲染侧，发送后清理标记。

`RSRenderService::RenderProcessManagerInit()` 中会把服务端帧率决策回调
`HgmContext::ProcessHgmFrameRate()` 传给 `renderProcessManager_`。只有单进程模式会真正串起该回调：

- 单进程：`RSSingleRenderProcessManager` 将回调设置到 `RSRenderServiceAgent::hgmProcessCallback_`。
- 多进程：渲染侧帧率决策回调不会执行；
  相关 IPC 因规格禁用而直接返回，这是当前预期行为。

### 渲染端初始化

`RSMainThread::Init()` 负责渲染端 HGM 初始化：

1. 初始化每帧执行的 `mainLoop_`。
2. 创建 `hgmRenderContext_`，传入渲染端到服务端的连接。
3. 调用 `hgmRenderContext_->InitHgmConfig()` 初始化渲染端成员变量。

注意：渲染端 `InitHgmConfig()` 会独立读取 HGM XML 配置。
初始化阶段服务端和渲染端各读一次配置；
运行时配置数据通过每帧返回的 `HgmServiceToProcessInfo` 从服务端同步到渲染端。

### 每帧运行

每次 VSync 触发 `mainLoop_()`，随后调用 `hgmRenderContext_->NotifyRpHgmFrameRate()`：

1. 渲染端填充 `HgmProcessToServiceInfo` 中仅渲染侧可见的数据。
2. `renderToServiceConnection_->NotifyRpHgmFrameRate()` 将数据送到服务端。
3. 服务端执行 `HgmContext::ProcessHgmFrameRate()`。
4. 服务端填充 `HgmServiceToProcessInfo`，判断本帧是否需要刷新帧率决策。
5. 服务端投递任务到 `HgmTaskHandleThread`，调用 `frameRateManager->UniProcessDataForLtpo()`。
6. 服务端组合外部输入计算下一帧刷新率，并把结果保存到下一帧通过NotifyRpHgmFrameRate接口回传给渲染端。
7. 调用返回后，渲染端通过 `SetServiceToProcessInfo()` 同步服务端返回的数据。
8. 渲染端获得决策的刷新率数据随帧传递给服务端的composer模块，composer模块调用屏幕管理接口给硬件层设置新的刷新率。

当 `HgmServiceToProcessInfo` 中 `hgmDataChangeTypes.test(HGM_CONFIG_DATA)` 为真时，
渲染端配置更新回调 `rpFrameRatePolicy_->HgmConfigUpdateCallback()` 才会执行。

### 刷新率投票决策树

修改刷新率逻辑时，优先按下面顺序定位，不要只看最终 `CalcRefreshRate()`：

```text
外部事件 / per-frame 数据
  -> HgmContext 投递到 HgmTaskHandleThread
  -> HgmFrameRateManager 收集场景和 linker
  -> HgmFrameVoter 刷新率合并 min/max/preferred
  -> HgmFrameRateManager::ProcessLtpoVote()
  -> HgmFrameRateManager::CalcRefreshRate()
  -> HgmVSyncGeneratorController / 硬件刷新率设置
  -> HgmSoftVSyncManager::CollectFrameRateChange()
  -> HgmVoter 软VSync合并 min/max/preferred
```

决策树检查点：

1. **输入是否本帧生效**：
   触控、动画、视频、pointer、idle、multi-app、RS linker 和 app linker 进入 manager 的时机不同。
   新输入要说明是立即影响本帧，还是进入下一帧合并。
2. **投票范围是否合理**：
   `FrameRateRange` 中 min、max、preferred 必须符合屏幕支持模式。
   preferred 超出范围时，要明确按上限、下限还是默认策略处理。
3. **优先级是否正确**：
   新 voter 要说明与 touch、video、animation、idle、power saving 的优先级关系。
   修改 `HgmFrameVoter::ProcessVote` 或 `HgmVoter` 时要覆盖冲突投票。
4. **LTPO 和 LTPS 是否分叉**：
   LTPO 侧关注 `ProcessLtpoVote()`、脉冲和 soft VSync；
   LTPS 侧关注固定周期、rate discount 和连接分发。
5. **软 VSync 是否需要同步**：
   目标刷新率变化后，`HgmSoftVSyncManager::CollectFrameRateChange()` 要能通知 app 侧分频变化。
6. **硬件是否真的切换**：
   `CalcRefreshRate()` 只代表策略结果，不等于设备已完成 mode 切换。
   涉及硬件验证时必须看 Composer/HdiBackend 或设备日志。

### 每帧同步边界

渲染端和服务端每帧交换的数据不是普通全量配置同步：

- `HgmRenderContext::NotifyRpHgmFrameRate()` 创建 `HgmProcessToServiceInfo`。
- `RSRenderToServiceConnection::NotifyRpHgmFrameRate()` 把 timestamp、vsyncId 和渲染侧信息送到服务端。
- `HgmContext::HandleHgmProcessInfo()` 只接收渲染侧可见的数据，例如 linker 和 UI 状态。
- `HgmContext::SetServiceToProcessInfo()` 只在变化位标记后把服务端数据回填给渲染端。
- `hgmDataChangeTypes_` 是运行时同步的节流开关；新增字段必须决定是否加入变化位。

新增字段时要同时回答四个问题：

1. 字段来源在渲染端、服务端还是外部 IPC。
2. 字段是每帧更新、按事件更新，还是只在配置变化时更新。
3. 字段缺省值对旧进程、单进程和多进程模式是否安全。
4. 字段同步失败时，下一帧能否恢复，是否需要保留上一帧值。

每帧链路中不要直接跨端访问对象。
渲染端只通过 `RSRenderToServiceConnection` 返回结构体拿服务端结果；
服务端只通过 `HgmContext` 和 `HgmTaskHandleThread` 串行处理策略输入。

### 外部输入

HGM 还会接收外部 IPC 输入参与帧率决策，典型链路如下：

1. `rs_interfaces.cpp` 接收外部调用。
2. 分发到同名 `rs_render_service_client.cpp` 接口。
3. 分发到同名 `rs_client_to_service_connection.cpp` 接口。
4. 通过 `renderServiceAgent_->GetHgmContext()` 获取服务端 `hgmContext_`。
5. 进入 `hgm_context.cpp` 中同名函数。

多数 `hgm_context.cpp` 函数会把工作投递到 `HgmTaskHandleThread`，确保 HGM 逻辑串行执行，
避免帧率决策和外部输入之间出现并发竞态。

## 优先查看位置

- 新增投票源：
  看 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_frame_rate_manager.h`。
  增加 `Handle*Event` 和 `rosen/modules/hyper_graphic_manager/core/utils/hgm_voter.h` 中的 voter。
- 修改投票合并逻辑：
  看 `rosen/modules/hyper_graphic_manager/core/utils/hgm_frame_voter.cpp`，关注 `ProcessVote` 和
  `ProcessVoteIter`。
- 新增 XML 配置项：
  看 `rosen/modules/hyper_graphic_manager/core/utils/hgm_command.h` 和
  `rosen/modules/hyper_graphic_manager/core/config/xml_parser.cpp`。
  先改配置结构体，再改解析。
- 修改多应用策略：
  看 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_multi_app_strategy.cpp`。
  关注 `UseMax`、`FollowFocus` 等策略。
- 修改触控空闲行为：
  看 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_idle_detector.cpp` 和
  `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_touch_manager.cpp`。
  关注保底策略和状态机。
- 调整 LTPO/DVSync：
  看 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_frame_rate_manager.cpp`。
  关注 `ProcessLtpoVote` 和 DVSync task。
- 修改软 VSync：
  看 `rosen/modules/hyper_graphic_manager/core/soft_vsync_manager/hgm_soft_vsync_manager.cpp`。
  关注 `CollectFrameRateChange`。
- 新增或修改屏幕配置：
  看 `rosen/modules/hyper_graphic_manager/core/hgm_screen_manager/hgm_core.cpp`。
  关注 `Init`、`AddScreen`、`SetLtpoConfig`。
- 修改刷新率计算：
  看 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_frame_rate_manager.cpp`，
  关注 `CalcRefreshRate`。
- 修改 IPC 回调注册：
  看 `rosen/modules/hyper_graphic_manager/core/config/hgm_config_callback_manager.cpp`，关注三类 callback。
- 新增测试：
  看 `rosen/test/hyper_graphic_manager/unittest/`。
  复用 `HgmTestBase` 和 mock visitor。

## 关键类型地图

- `HgmCore`：
  位置 `rosen/modules/hyper_graphic_manager/core/hgm_screen_manager/hgm_core.h`。
  HGM 顶层入口，持有 manager、parser 和 screen。
- `HgmFrameRateManager`：
  位置 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_frame_rate_manager.h`。
  帧率决策中枢，持有多个子组件。
- `HgmFrameVoter`：
  位置 `rosen/modules/hyper_graphic_manager/core/utils/hgm_frame_voter.h`。负责投票聚合和 LTPO 合并。
- `HgmVoter`：
  位置 `rosen/modules/hyper_graphic_manager/core/utils/hgm_voter.h`。负责底层投票递送和优先级合并。
- `VoteInfo`：
  位置 `rosen/modules/hyper_graphic_manager/core/utils/hgm_voter.h`。
  单次投票信息，包含 name、min、max、pid。
- `HgmMultiAppStrategy`：
  位置 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_multi_app_strategy.h`。
  负责多应用冲突策略。
- `HgmIdleDetector`：
  位置 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_idle_detector.h`。
  负责 surface 和动画空闲检测。
- `HgmTouchManager`：
  位置 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_touch_manager.h`。负责触控状态机。
- `HgmPointerManager`：
  位置 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_pointer_manager.h`。负责指针状态机。
- `HgmSoftVSyncManager`：
  位置 `rosen/modules/hyper_graphic_manager/core/soft_vsync_manager/hgm_soft_vsync_manager.h`。
  负责应用维度 VSync 分频。
- `HgmVSyncGeneratorController`：
  位置 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_vsync_generator_controller.h`。
  负责 VSync 频率和 offset 控制。
- `HgmEnergyConsumptionPolicy`：
  位置 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/hgm_energy_consumption_policy.h`。
  负责节能降帧策略。
- `PolicyConfigData`：
  位置 `rosen/modules/hyper_graphic_manager/core/utils/hgm_command.h`。XML 策略配置总结构。
- `PolicyConfigVisitor`：
  位置 `rosen/modules/hyper_graphic_manager/core/utils/hgm_command.h`。配置查询接口。
- `XMLParser`：
  位置 `rosen/modules/hyper_graphic_manager/core/config/xml_parser.h`。解析 `hgm_policy_config.xml`。
- `HgmScreen`：
  位置 `rosen/modules/hyper_graphic_manager/core/hgm_screen_manager/hgm_screen.h`。屏幕抽象和 mode 管理。

## 编码约定

- 文件命名：`hgm_*.h`、`hgm_*.cpp`。
- 类命名：`Hgm*`。
- 枚举：UPPER_SNAKE_CASE。
- 成员变量：优先使用尾部 `_`；`HgmCore` 中保留部分历史 `m` 前缀。
- 命名空间：常见为 `OHOS::Rosen`，老文件可能使用嵌套 namespace 写法。
- 日志：使用 `rosen/modules/hyper_graphic_manager/core/config/hgm_log.h` 中的 `HGM_LOGD/I/W/E`，
  格式化输出注意 `%{public}s`。
- 错误返回：`int32_t` 常用 `HgmErrCode`，例如 `EXEC_SUCCESS` 和 `HGM_ERROR`；也可能返回 `bool`。
- 智能指针：IPC/RefBase 对象使用 `sptr<>` 和 `wptr<>`；内部对象使用 `shared_ptr` 或 `unique_ptr`。
- 线程安全：无锁状态优先 `std::atomic<T>`；临界区使用 `std::mutex` 和 `std::lock_guard`。

## 禁止模式

- 不要在 `rosen/modules/hyper_graphic_manager/core/frame_rate_manager/` 中直接访问
  `PolicyConfigData` 字段，优先使用 `PolicyConfigVisitor`。
- 不要从 `render_service` 直接调用 `HgmFrameRateManager`，应通过 `HgmContext` 或 `HgmRenderContext`。
- 不要在 `RenderProcessManager` 相关的实现中 使用HgmCore这个单例。
- 不要直接访问对侧对象；服务端和渲染端只能通过 `renderToServiceConnection_` 或
  `RSRenderToServiceConnection` 交换运行时数据。
- 不要把多进程模式下帧率决策回调不执行误判为普通 bug；这是当前规格禁用路径。

## 验证建议

文档或知识路由变更只需检查路径和锚点。

涉及真实刷新率、LTPO、HWC、功耗或显示效果时，需要补充真实设备验证。
记录至少包含产品名、
设备形态、系统版本或镜像标识、输入场景、触发命令或 API、期望结果和实际结果。

没有真实设备时，不默认阻塞文档、静态验证或可本地验证的实现推进。
最终回复或 PR 中必须写清未执行设备验证的原因和风险；
不得把刷新率、功耗或显示效果描述为“完整验证”。
如果任务目标就是证明设备行为或性能功耗结论，需要人工确认或设备补测后才能关闭。

真实设备验证建议最少覆盖：

- **静态模式**：锁定 60Hz、90Hz、120Hz 或产品支持的固定模式，确认策略不会越过硬件能力。
- **触控提频**：滑动、点击、长按后进入 touch voter，释放后能按 idle 策略降频。
- **动画场景**：系统动画、应用动画和窗口动画能上报 `RSFrameRateRange`，结束后回落。
- **视频/多应用**：视频、焦点窗口、多窗口并存时，
  确认 multi-app 策略和视频 voter 优先级。
- **LTPO/soft VSync**：同屏不同 app 分频时，确认实际 callback 周期和目标刷新率一致。
- **异常回退**：屏幕灭屏、热插拔、HGM XML 缺项、DVSync 关闭或 HWC 不支持时不崩溃。

设备记录不要只写“肉眼正常”。至少保留 hilog/hitrace、目标刷新率、实际回调周期、
屏幕 mode、触发 API、测试时长和是否发生掉帧或频繁抖动。

## 额外注意

- `HgmCore` 是 HGM 单例根，外部访问通常从 `HgmCore::Instance()` 开始。
- 每帧集成链路是 `mainLoop_()`、`HgmRenderContext::NotifyRpHgmFrameRate()`、连接回调、
  `HgmContext::ProcessHgmFrameRate()`。
- IPC callback 主要有 `OnHgmConfigChanged`、`OnHgmRefreshRateModeChanged`、
  `OnHgmRefreshRateUpdate` 三类，通过 `RSIHgmConfigChangeCallback` 分发。
