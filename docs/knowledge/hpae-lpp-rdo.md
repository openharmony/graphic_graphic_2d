# HPAE / LPP / RDO

## 适用范围

- HPAE（Hardware Post-processing Acceleration Engine）模糊合成
- LPP（Low Power Path）低功耗视频通路
- RDO（Render Data Optimization）渲染数据优化

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSHpaeManager | `rosen/modules/render_service/core/feature/hpae/rs_hpae_manager.h` | HPAE 管理器单例 |
| RSHpaeBuffer | `rosen/modules/render_service/core/feature/hpae/rs_hpae_buffer.h` | HPAE Buffer 封装 |
| RSHpaeRenderListener | `rosen/modules/render_service/core/feature/hpae/rs_hpae_render_listener.h` | HPAE 渲染监听 |
| LppVideoHandler | `rosen/modules/render_service/core/feature/lpp/render_process/lpp_video_handler.h` | LPP 视频处理器 |
| LppLayerCollector | `rosen/modules/render_service/composer/composer_service/external_depend/lpp/render_server/lpp_layer_collector.h` | LPP Layer 收集器 |
| RSRdo | `rosen/modules/render_service/core/feature/rdo/rs_rdo.h` | RDO 模块 |

## 核心模型

### HPAE

`RSHpaeManager` 为单例，管理 HPAE 模糊合成的完整生命周期。详见 hwc-prevalidate.md 中 HPAE 部分。状态机 `HpaeFrameState`：IDLE → ACTIVE → WORKING → DEACTIVE → SWITCH_BLUR → CHANGE_CONFIG → ALLOC_BUF → WAITING。

核心数据流：

1. Prepare：`OnUniRenderStart` → `RegisterHpaeCallback` → `IsHpaeBlurNode` 判断 HPAE 节点
2. Sync：`OnSync(isHdrOn)` 同步 HDR 状态
3. Process：`CheckHpaeBlur` → `InitIoBuffers` → `SetUpHpaeSurface` 准备输入输出 Surface
4. Buffer 管理：`hpaeBufferIn_` / `hpaeBufferOut_` 各持 4 个 RSRenderFrame

`RSHpaeBuffer` 封装输入/输出 buffer：

- `Init`：根据 BufferRequestConfig 创建 Surface
- `RequestFrame`：请求渲染帧（支持按配置或宽高）
- `FlushFrame`：提交帧
- `ForceDropFrame`：强制丢帧
- `hpaeScaleFactor_`：HPAE 缩放因子

### LPP

LPP 提供低功耗视频直通通路，绕过 GPU 渲染。

**LppVideoHandler**（渲染侧）：

- `JudgeRequestVsyncForLpp`：判断是否需要为 LPP 请求 VSync
- `ConsumeAndUpdateLppBuffer`：消费并更新 LPP buffer
- `JudgeLppLayer`：在硬件线程判断 LPP Layer
- `HasLppVideo`：是否有 LPP 视频
- `lppConsumerMap_`：`<vsyncId, vector<wptr<IConsumerSurface>>>` 映射
- `lppShbState_` / `lppRsState_`：LPP 状态（UNKOWN / LPP_LAYER / UNI_RENDER）

**LppLayerCollector**（合成侧）：

- `AddLppLayerId`：从 Layer 列表添加 LPP Layer ID
- `RemoveLayerId`：移除 Layer ID
- `GetLppLayerId`：获取 LPP Layer ID 集合
- LPP Layer 属性：`TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT`

最大 SurfaceNode 数量：`LPP_SURFACE_NODE_MAX_SIZE = 10`

### RDO

`RSRdo` 提供渲染数据优化能力，当前为条件编译：

- `EnableRSCodeCache`：启用 RS 代码缓存
- `HelperThreadforBinXO`：BinXO 辅助线程

编译宏 `RS_ENABLE_RDO` 控制是否启用。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| HPAE buffer 4 帧队列 | `HPAE_BUFFER_SIZE = 4` | 平衡内存占用与流水线深度 |
| HPAE 双端 buffer 分离 | `hpaeBufferIn_` / `hpaeBufferOut_` | HPAE 硬件需要输入/输出双端，分离管理便于流水线化 |
| HPAE buffer 分配异步 | `hpaeBufferAllocating_` atomic | Buffer 分配耗时长，异步避免阻塞渲染 |
| LPP 渲染/合成双侧管理 | `LppVideoHandler` + `LppLayerCollector` | 渲染侧判断 LPP 状态，合成侧收集 Layer，职责分离 |
| LPP ConsumerSurface 映射 | `lppConsumerMap_` 按 vsyncId | LPP buffer 消费需要与 VSync 对齐，按 vsyncId 管理保证时序 |
| RDO 条件编译 | `RS_ENABLE_RDO` | RDO 依赖特定平台能力，条件编译避免不可用时的编译错误 |

## 待补充背景

- HPAE 硬件模糊的具体类型和支持的参数范围
- `hpaeScaleFactor_` 的计算逻辑
- HPAE 离线渲染（hpae_offline）的完整流程
- LPP 的完整使用场景（视频播放/视频通话等）
- RDO 的具体优化策略和 BinXO 的含义
