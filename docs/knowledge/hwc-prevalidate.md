# HWC Prevalidate 与 Hpae

## 适用范围

- HWC 预校验（Prevalidate）逻辑修改
- HPAE（Hardware Post-processing Acceleration Engine）模糊合成
- HPAE buffer 管理与状态机
- HPAE 离线渲染流程

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSHwcContext | `rosen/modules/render_service/core/pipeline/hwc/rs_hwc_context.h` | HWC 上下文，sourceTuning / solidLayer 配置 |
| RSUniHwcVisitor | `rosen/modules/render_service/core/pipeline/hwc/rs_uni_hwc_visitor.h` | HWC 遍历器，含 Prevalidate 相关遮挡/使能判断 |
| RSHpaeManager | `rosen/modules/render_service/core/feature/hpae/rs_hpae_manager.h` | HPAE 管理器，控制 HPAE 模糊状态机 |
| RSHpaeBuffer | `rosen/modules/render_service/core/feature/hpae/rs_hpae_buffer.h` | HPAE buffer 封装，管理输入/输出 Surface |
| RSHpaeRenderListener | `rosen/modules/render_service/core/feature/hpae/rs_hpae_render_listener.h` | HPAE 渲染回调监听 |

## 核心模型

### HWC Prevalidate

HWC Prevalidate 的核心逻辑在 `RSUniHwcVisitor` 中。在 Prepare 阶段，遍历器执行以下预校验：

1. **背景 Alpha 检查**：`UpdateHwcNodeEnableByBackgroundAlpha` / `UpdateHardwareStateByHwcNodeBackgroundAlpha` 判断背景透明度是否允许 HWC
2. **Buffer 尺寸检查**：`UpdateHwcNodeEnableByBufferSize` 检查 buffer 尺寸是否符合 HWC 要求
3. **Alpha 检查**：`UpdateHwcNodeEnableByAlpha` 检查节点透明度
4. **旋转检查**：`UpdateHwcNodeEnableByRotate` 检查旋转场景
5. **Filter 矩形检查**：`UpdateHwcNodeEnableByFilterRect` / `UpdateHwcNodeEnableByGlobalFilter` 检查全局模糊与 HWC 的冲突
6. **ColorPicker 检查**：`UpdateHwcNodeEnableByColorPicker` 检查取色任务与 HWC 的冲突
7. **Solid Layer 检查**：`ProcessSolidLayerEnabled/Disabled` 检查纯色层

`RSHwcContext` 中的 `sourceTuningConfig_` 和 `solidLayerConfig_` 提供包名级别的配置覆盖，影响 Prevalidate 的结果。

### HPAE 管理器

`RSHpaeManager` 为单例，管理 HPAE 模糊的完整生命周期。状态机 `HpaeFrameState` 包含 IDLE → ACTIVE → WORKING → DEACTIVE 等状态转换：

- **Prepare 阶段**：`OnUniRenderStart` → `RegisterHpaeCallback` → `IsHpaeBlurNode` 判断是否有 HPAE 模糊节点
- **Process 阶段**：`CheckHpaeBlur` → `InitIoBuffers` → `SetUpHpaeSurface` 准备输入/输出 Surface
- **Buffer 管理**：使用双 buffer（`hpaeBufferIn_` / `hpaeBufferOut_`），各持 `HPAE_BUFFER_SIZE(4)` 个 `RSRenderFrame`

### HPAE Buffer

`RSHpaeBuffer` 封装了 HPAE 输入/输出 buffer 的生命周期：

- `Init`：根据 `BufferRequestConfig` 创建 Surface
- `RequestFrame`：请求渲染帧（buffer），支持按配置或宽高
- `FlushFrame`：提交渲染结果
- `ForceDropFrame`：强制丢弃帧

数据流：RSMainThread Prepare → RSHpaeManager 判断 HPAE 节点 → RSSubThread 渲染到 hpaeBufferIn → HPAE 硬件处理 → hpaeBufferOut → Composer 合成。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| HPAE 状态机驱动 | `HpaeFrameState` 枚举 | HPAE 涉及硬件资源分配/释放，状态机保证资源生命周期正确，避免竞态 |
| HPAE buffer 双端分离 | `hpaeBufferIn_` / `hpaeBufferOut_` | HPAE 硬件需要输入/输出双端 buffer，分离管理便于流水线化 |
| HPAE buffer 4 帧队列 | `HPAE_BUFFER_SIZE = 4` | 4 帧 buffer 平衡内存占用与流水线深度，避免 GPU/HPAE 互相等待 |
| Prevalidate 在 Prepare 阶段执行 | `RSUniHwcVisitor` 在渲染遍历前判断 | 提前确定 HWC 使能，避免 Process 阶段回退 GPU 渲染导致性能损失 |
| sourceTuning 配置动态加载 | `RSHwcContext` 持有可变映射 | 不同应用场景 HWC 策略不同，运行时可调避免硬编码 |

## 待补充背景

- HPAE 硬件的具体能力范围（支持哪些模糊类型和参数）
- `hpaeScaleFactor_` 的计算逻辑和默认值
- HPAE 离线渲染（hpae_offline）的完整流程
- Prevalidate 各检查项的具体阈值和平台差异
- `HpaeStatus` 的详细状态定义和转换条件
