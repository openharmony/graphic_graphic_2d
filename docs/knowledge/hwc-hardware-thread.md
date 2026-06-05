# 硬件线程与 HWC

## 适用范围

- 硬件合成（HWC）节点判断与使能逻辑修改
- 实时刷新率统计与显示
- HWC 上下文配置（sourceTuning / solidLayer）调整
- HWC 禁用原因统计与 DFX
- 节点遮挡判断、背景 Alpha 对 HWC 使能的影响

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSRealtimeRefreshRateManager | `rosen/modules/render_service/core/pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h` | 实时刷新率统计与显示管理 |
| RSHwcContext | `rosen/modules/render_service/core/pipeline/hwc/rs_hwc_context.h` | HWC 上下文，持有 sourceTuning / solidLayer 配置 |
| RSUniHwcVisitor | `rosen/modules/render_service/core/pipeline/hwc/rs_uni_hwc_visitor.h` | 统一渲染 HWC 遍历器，决定节点 HWC 使能 |
| RSDirectCompositionHelper | `rosen/modules/render_service/core/pipeline/hwc/rs_direct_composition_helper.h` | 直投合成辅助状态 |

## 核心模型

### 实时刷新率管理

`RSRealtimeRefreshRateManager` 为单例，在硬件线程侧统计每个 ScreenId 的实时帧率。支持两种模式：`COLLECT`（仅采集）和 `SHOW`（采集 + 显示）。每帧通过 `CountRealtimeFrame` 累计帧计数，以 250ms 为周期计算实际刷新率，低于 `IDLE_FPS_THRESHOLD(8)` 视为空闲。最终通过 `screenRefreshRateMap_` 和 `currRealtimeRefreshRateMap_` 维护逻辑刷新率与实时刷新率。

### HWC 上下文

`RSHwcContext` 持有两组配置映射：`sourceTuningConfig_` 和 `solidLayerConfig_`。`CheckPackageInConfigList` 用于判断特定包名是否在配置列表中，影响背景 Alpha 方案的 HWC 使能。HwcContext 由 `RSUniRenderVisitor` 创建并传递给 `RSUniHwcVisitor`。

### HWC 遍历器

`RSUniHwcVisitor` 是核心 HWC 判断逻辑，负责：

1. **坐标更新**：`UpdateSrcRect` / `UpdateDstRect` / `UpdateHwcNodeByTransform` 计算源/目标矩形和矩阵
2. **遮挡判断**：`CheckNodeOcclusion` / `CheckSubTreeOcclusion` 判断节点是否被遮挡
3. **HWC 使能判定**：按优先级依次检查 `UpdateHwcNodeEnableByBackgroundAlpha` → `UpdateHwcNodeEnableByBufferSize` → `UpdateHwcNodeEnableByAlpha` → `UpdateHwcNodeEnableByRotate` → `UpdateHwcNodeEnableByFilterRect` → `UpdateHwcNodeEnableByColorPicker`
4. **Solid Layer**：`ProcessSolidLayerEnabled/Disabled` 处理纯色层
5. **DRM**：`UpdateCrossInfoForProtectedHwcNode` 处理受保护内容
6. **DFX**：`HwcDisabledReasonCollection` 统计禁用原因

数据流：RSUniRenderVisitor → 创建 RSUniHwcVisitor → 遍历节点树 → 判定每个 SurfaceRenderNode 的 HWC 使能 → 结果写入节点属性 → 合成阶段消费。

### 直投辅助

`RSDirectCompositionHelper` 仅记录 `isLastFrameDirectComposition_` 标志，用于直投合成状态跟踪。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 实时刷新率独立于 HGM 统计 | `RSRealtimeRefreshRateManager` 单独计数帧而非查询 HGM | HGM 返回的是策略刷新率，而实时刷新率需要反映真实合成频率，两者语义不同 |
| HWC 遍历器与渲染遍历器分离 | `RSUniHwcVisitor` 持有 `RSUniRenderVisitor&` 引用但独立类 | HWC 判断逻辑复杂度高，独立封装便于维护和测试，避免渲染遍历器膨胀 |
| HWC 禁用原因统计 | `HwcDisabledReasonCollection` 单例 | 线上 HWC 回退问题难以定位，统计禁用原因可以快速定位根因 |
| sourceTuning / solidLayer 配置外置 | `RSHwcContext` 持有可变配置映射 | 不同产品对 HWC 使能策略不同，配置外置便于产品差异化而无需改代码 |
| Solid Layer 特殊处理 | `ProcessSolidLayerEnabled/Disabled` 分支 | 纯色背景层可由 HWC 直接合成，跳过 GPU 渲染以节省带宽 |

## 待补充背景

- `sourceTuningConfig_` 和 `solidLayerConfig_` 的具体配置项及各产品默认值
- HWC 使能判断的完整优先级链和各条件的性能影响量化
- `isHardwareForcedDisabled_` 的触发场景和恢复机制
- HDR 强制 HWC 节点 (`CollectHdrForceHwcNodes`) 的完整处理流程
- 实时刷新率在多屏场景下的采集策略
