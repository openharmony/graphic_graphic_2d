# Composer 与 HDI 后端

## 适用范围

- 合成管线（Composer Pipeline）修改
- HDI 后端（HdiBackend）适配
- Layer 事务处理与 buffer 提交
- 合成帧率控制与延迟计算
- 颜色空间/像素格式协商

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSRenderComposer | `rosen/modules/render_service/composer/composer_service/pipeline/rs_render_composer.h` | 合成器主类，管理合成 Prepare/Process |
| RSRenderComposerContext | `rosen/modules/render_service/composer/composer_service/pipeline/rs_render_composer_context.h` | 合成器上下文，Layer 映射管理 |
| RSRenderComposerAgent | `rosen/modules/render_service/composer/composer_service/pipeline/rs_render_composer_agent.h` | 合成器代理，线程消息转发 |
| RSRenderComposerManager | `rosen/modules/render_service/composer/composer_service/pipeline/rs_render_composer_manager.h` | 合成器管理，屏幕连接/断连 |
| RSSurfaceLayer | `rosen/modules/render_service/composer/composer_client/layer/rs_surface_layer.h` | Surface Layer 实现，封装 HDI Layer 属性 |
| RSSurfaceRCDLayer | `rosen/modules/render_service/composer/composer_client/layer/rs_surface_rcd_layer.h` | RCD（圆角显示）Layer |
| RSLayer | `rosen/modules/render_service/composer/composer_service/render_layer/rs_layer.h` | Layer 基类 |
| LppLayerCollector | `rosen/modules/render_service/composer/composer_service/external_depend/lpp/render_server/lpp_layer_collector.h` | LPP Layer 收集器 |
| HDR util | `rosen/modules/render_service/composer/composer_service/external_depend/hdr/` | 合成侧 HDR 工具 |
| TV Metadata util | `rosen/modules/render_service/composer/composer_service/external_depend/tv_metadata/` | 合成侧 TV 元数据 |
| HGM | `rosen/modules/render_service/composer/composer_service/external_depend/hyper_graphic_manager/` | 合成侧 HGM 集成 |
| RCD | `rosen/modules/render_service/composer/composer_service/external_depend/rcd/` | 合成侧圆角显示 |

## 核心模型

### 合成管线

`RSRenderComposer` 运行在独立 EventRunner 上，每个屏幕一个实例。核心流程：

1. **ComposerPrepare**：调用 HDI Prepare，确定各 Layer 合成类型（Client/HWC），计算当前刷新率和延迟时间
2. **ComposerProcess**：根据 Prepare 结果处理 Client 合成（GPU 重绘），提交 HWC 合成 Layer
3. **OnPrepareComplete**：HDI 回调，处理 HWC 合成完成事件

### Layer 生命周期

`RSSurfaceLayer` 继承 `RSLayer`，封装一个 HDI Layer 的全部属性：Alpha、Zorder、Transform、CompositionType、CropRect、ColorTransform、TunnelLayer、HDR Metadata 等。通过 `RSRenderLayerCmd` 模式进行属性更新，避免直接 setter。

`RSRenderComposerContext` 维护 `layersMap_`（RSLayerId → RSLayer 映射），负责 Layer 的增删和查询。

### 合成帧率控制

`RSRenderComposer` 通过 HGM 集成实现帧率控制：

- `IsDelayRequired` / `CalculateDelayTime` / `UpdateDelayTime` 计算帧延迟
- `SetHardwareTaskNumCallback` / `SetTaskEndWithTimeCallback` 回调通知硬件任务数和完成时间
- `AdaptiveModeStatus` 判断自适应模式状态

### Tunnel Layer 提交

`CommitTunnelLayerBySurfaceId` 支持直投 Layer 的 buffer 提交，绕过正常合成管线。

### LPP Layer 收集

`LppLayerCollector` 在合成侧收集 LPP（Low Power Path）Layer ID，属性为 `TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT`。

## Layer 到 HDI 的设备链路

Composer 的设备提交链路分为客户端 Layer 事务和服务端 HDI 后端两段：

```text
RenderService / drawable / HWC visitor
  -> RSSurfaceLayer 设置 buffer、fence、visible、dirty、composition type
  -> RSRenderLayerCmd 记录属性变更
  -> RSLayerTransactionHandler::CommitRSLayerTransaction()
  -> RSRenderToComposerConnection::CommitLayers()
  -> RSRenderComposerAgent
  -> RSRenderComposer
  -> HdiBackend / HdiOutput / HdiLayer
  -> display HDI prepare / commit / present fence
```

关键代码锚点：

- `composer_client/layer/rs_surface_layer.cpp`：
  `SetCompositionType()`、`SetBuffer()`、`SetAcquireFence()`、HDR、tunnel、dirty 等属性。
- `composer_service/transaction/rs_layer_transaction_handler.cpp`：
  批量提交 `RSLayerTransactionData`。
- `composer_service/connection/rs_render_to_composer_connection.cpp`：
  RenderService 到 ComposerService 的连接入口。
- `composer_service/layer_backend/hdi_backend.cpp`、`hdi_output.cpp`、`hdi_layer.cpp`：
  HDI 后端和每屏输出。
- `composer_service/render_layer/rs_render_surface_layer.cpp`：
  ComposerService 侧 Layer 状态。

## HWC / HDI fallback

HWC 不是固定开启路径。以下场景会导致 client/GPU composition 或其它回退：

- 全局强制 client composition：`RSSystemProperties::IsForceClient()`。
- Filter、圆角、阴影、复杂旋转、特殊裁剪或不支持的 blend/color space。
- HWC prevalidate 返回不支持，或 `RequestCompositionType` 不满足 device composition。
- buffer、fence、visible region、dirty region、crop、transform 不满足 HDI 限制。
- HDR、DRM/protected layer、tunnel layer、LPP、RCD 等能力和设备不匹配。
- 多屏、虚拟屏、折叠屏切换时当前 output 不支持目标 layer 配置。

fallback 判断要同时看两层：

1. RenderService 侧 HWC visitor/compute util 是否决定使用 DEVICE、CLIENT 或 OFFLINE_DEVICE。
2. ComposerService / HDI prepare 后 `hdiCompositionType_` 是否和请求一致。

不能只看 `RSSurfaceLayer::SetCompositionType()` 的请求值。真实提交以 HDI prepare/commit 的结果为准。

## Buffer / Fence / Present 生命周期

Layer 提交时必须同时维护 buffer 和 fence：

- `RSSurfaceLayer::SetBuffer()` 记录当前 `SurfaceBuffer`。
- `RSSurfaceLayer::SetAcquireFence()` 记录 acquire fence。
- HDI commit 后返回 present/release fence，决定 buffer 何时可复用。
- `SetPreBuffer()` 保留上一帧 buffer，避免回退或延迟提交时悬空。

修改 buffer 提交流程时，要覆盖：

- acquire fence 为空、超时、已 signal。
- present fence 创建失败或 release fence 未返回。
- 同一 layer 连续提交相同 buffer。
- tunnel layer 直投和普通 layer 同时存在。
- HWC fallback 后 GPU 重绘是否仍正确释放 buffer。

## 真实设备验证

Composer/HDI 改动必须记录真实设备信息，尤其是以下场景：

- 单屏、虚拟屏、多屏或折叠屏切换。
- HWC 成功和 HWC fallback 各至少一个场景。
- HDR/SDR、不同 pixel format、不同 color space。
- 旋转、缩放、圆角、filter、shadow 和 dirty region 变化。
- tunnel/LPP/DRM/protected layer 场景。

记录项建议包括：产品、镜像、屏幕 id、目标 mode、layer 数、各 layer composition type、
HDI prepare/commit 返回、present fence、hitrace/hilog 和肉眼结果。
没有设备时不能写“已完成显示验证”。

没有真实设备时，不默认阻塞文档、静态验证或可本地验证的实现推进。
最终回复或 PR 中必须写清未执行设备验证的原因和风险；
不得把 HWC fallback、HDI prepare/commit 或显示效果描述为“完整验证”。
如果任务目标就是证明设备合成行为，需要人工确认或设备补测后才能关闭。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 每屏幕独立 Composer 实例 | `RSRenderComposer` 绑定 `HdiOutput` | 多屏场景各屏独立合成，避免锁竞争和状态串扰 |
| Composer 运行在独立 EventRunner | `runner_` / `handler_` | 合成操作耗时长，独立线程避免阻塞主线程 |
| Layer 属性通过 Cmd 更新 | `RSRenderLayerCmd` 模式 | 批量属性更新减少 HDI 调用次数，降低 IPC 开销 |
| external_depend 模式集成 HDR/TV/LPP | `composer_service/external_depend/` | 第三方/设备差异功能通过外部依赖隔离，主流程不受影响 |
| Delay 机制控制帧提交时机 | `CalculateDelayTime` / `delayTime_` | 高刷新率下延迟提交避免空跑帧，节省功耗 |

## 待补充背景

- HDI 后端的具体接口版本和能力差异
- `ComposerScreenInfo` 的完整字段含义
- GPU 重绘（Redraw）的完整触发条件和资源管理
- AFBC（Arm Frame Buffer Compression）的使能条件和平台支持
- 颜色空间协商（`ComputeTargetColorGamut` / `ComputeTargetPixelFormat`）的完整决策逻辑
