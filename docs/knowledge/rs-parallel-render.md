# 并行渲染与 SLR

## 适用范围

- 并行渲染任务调度与执行
- SuperRenderTask 的子任务拆分与合并
- SLR（Super Low Resolution）降分辨率渲染
- SLR 的 Laplace 滤波和恢复算法
- 渲染负载均衡

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 渲染任务基类 | `rosen/modules/render_service/core/pipeline/parallel_render/rs_render_task.h` | RSRenderTaskBase / RSRenderTask / RSSuperRenderTask / RSCompositionTask |
| 渲染任务实现 | `rosen/modules/render_service/core/pipeline/parallel_render/rs_render_task.cpp` | 任务执行逻辑 |
| SLR 降分辨率 | `rosen/modules/render_service/core/pipeline/slr_scale/rs_slr_scale.h` | RSSLRScaleFunction：CanvasScale/ProcessCacheImage/ProcessOffscreenImage |
| SLR 实现 | `rosen/modules/render_service/core/pipeline/slr_scale/rs_slr_scale.cpp` | SLR 算法实现 |

## 核心模型

### 并行渲染任务模型

并行渲染基于任务分解模型：

1. **RSRenderTaskBase**：所有渲染任务基类，持有 `node_`（目标节点）和 `loadId_`（负载标识）。
2. **RSRenderTask**：单个 SurfaceRenderNode 的渲染子任务，具有 `RenderNodeStage` 阶段：
   - `PREPARE`：准备阶段
   - `PROCESS`：处理阶段
   - `CACHE`：缓存阶段
   - `CALC_COST`：计算开销
3. **RSSuperRenderTask**：一个 ScreenRenderNode 对应的超级任务，内部包含 `queue<RSRenderTask>` 子任务队列。由 `AddTask()` 添加子任务，`GetNextRenderTask()` 逐个取出执行。
4. **RSCompositionTask**：合成任务，对应一个 ScreenRenderNode。

### 任务调度流程

```
RSMainThread::DoParallelComposition(rootNode)
  → 为每个 ScreenNode 创建 RSSuperRenderTask
  → 遍历子节点，每个 SurfaceRenderNode 创建 RSRenderTask(PREPARE/PROCESS)
  → 通过线程池并行执行 RSRenderTask
  → 所有子任务完成后执行 RSCompositionTask
```

### SLR 降分辨率渲染

SLR（Super Low Resolution）在渲染负载过高时降低渲染分辨率，再通过滤波算法恢复：

1. **触发条件**：当渲染负载超过阈值（`SLR_SCALE_THR_HIGH = 0.79f`）。
2. **CanvasScale**：通过 `scaleMatrix_` 对画布进行缩放，降低实际渲染分辨率。
3. **恢复过程**：
   - `ProcessCacheImage()`：对缓存图像用 SLR Shader 恢复。
   - `ProcessOffscreenImage()`：对离屏渲染图像恢复。
4. **算法**：
   - SLR Shader：基于 RuntimeEffect 的自定义着色器，对降分辨率图像进行上采样和锐化。
   - Laplace Shader：拉普拉斯滤波器增强边缘。
   - `kernelSize_` 控制滤波核大小，`alpha_` 控制混合系数。

### SLR 颜色空间

- `CheckOrRefreshColorSpace()` 根据屏幕 ColorGamut 调整颜色空间。
- `imageColorSpace_` 确保 SLR 处理前后颜色一致。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| SuperRenderTask 分解模型 | RSSuperRenderTask 包含多个 RSRenderTask | 一帧内多个 SurfaceNode 可并行处理，充分利用多核 |
| RenderNodeStage 阶段划分 | PREPARE/PROCESS/CACHE/CALC_COST | 分阶段执行，PREPARE 和 PROCESS 可流水线化 |
| SLR 降分辨率 | RSSLRScaleFunction + RuntimeShader | GPU 负载过高时降低分辨率保帧率，再通过滤波恢复画质 |
| Laplace 滤波恢复 | `laplaceFilterShader_` + `slrFilterShader_` | 两级滤波：拉普拉斯锐化 + 混合，在低分辨率下保持边缘清晰 |
| 阈值常量 | `SLR_SCALE_THR_HIGH = 0.79f` | 经验阈值，平衡画质损失与帧率提升 |
| saveCount 机制 | `saveCount_` + Canvas Save/Restore | 确保 SLR 缩放状态正确恢复，不影响后续绘制 |

## 待补充背景

- 并行渲染线程池的具体配置（线程数、优先级）。
- RSMainThread 中 `DoParallelComposition()` 的完整调度策略和回退逻辑。
- SLR 的实际触发场景和产品配置方法。
- SLR 对 HDR 内容的处理策略。
- 并行渲染与 OPINC/RenderGroup 缓存的交互。
