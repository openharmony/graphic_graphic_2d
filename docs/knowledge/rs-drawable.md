# Drawable 绘制执行

## 适用范围

- Drawable 体系架构和与 RenderNode 的映射关系
- 各类 Drawable 的绘制流程（OnDraw/OnCapture）
- Drawable 的自动注册工厂机制
- OPINC 绘制缓存与 Layer 部分渲染
- RenderGroup 缓存绘制
- 并行渲染在 Drawable 层的体现

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| Drawable 基类适配器 | `rosen/modules/render_service_base/include/drawable/rs_render_node_drawable_adapter.h` | RSRenderNodeDrawableAdapter：Draw/OnGenerate 工厂 |
| RenderNode Drawable | `rosen/modules/render_service/core/drawable/rs_render_node_drawable.h/.cpp` | RSRenderNodeDrawable：基础节点绘制 |
| Surface Drawable | `rs_surface_render_node_drawable.h/.cpp` | RSSurfaceRenderNodeDrawable：Surface buffer 绘制 |
| Screen Drawable | `rs_screen_render_node_drawable.h/.cpp` | RSScreenRenderNodeDrawable：屏幕帧提交 |
| Canvas Drawable | `rs_canvas_render_node_drawable.h/.cpp` | RSCanvasRenderNodeDrawable：Canvas CmdList 回放 |
| CanvasDrawing Drawable | `rs_canvas_drawing_render_node_drawable.h/.cpp` | RSCanvasDrawingRenderNodeDrawable：直接绘制 |
| Effect Drawable | `rs_effect_render_node_drawable.h/.cpp` | RSEffectRenderNodeDrawable：效果节点 |
| LogicalDisplay Drawable | `rs_logical_display_render_node_drawable.h/.cpp` | RSLogicalDisplayRenderNodeDrawable：逻辑显示 |
| Root Drawable | `rs_root_render_node_drawable.h/.cpp` | RSRootRenderNodeDrawable：根节点 |
| Union Drawable | `rs_union_render_node_drawable.h/.cpp` | RSUnionRenderNodeDrawable：联合节点 |
| 属性 Drawable | `rosen/modules/render_service_base/include/drawable/rs_property_drawable.h` | RSPropertyDrawable：属性绘制（阴影、边框等） |
| 通用 Drawable | `rosen/modules/render_service_base/include/drawable/rs_drawable.h` | RSDrawable：基础 Drawable |
| DFX | `rosen/modules/render_service/core/drawable/dfx/` | 绘制调试相关 |

## 核心模型

### Drawable 与 RenderNode 的映射

Drawable 体系采用**工厂自动注册**模式：

1. `RSRenderNodeDrawableAdapter` 定义 `OnGenerate` 静态方法和 `Registrar` 静态成员。
2. 每个 Drawable 子类通过 `RenderNodeDrawableRegistrar<RSRenderNodeType::XXX, OnGenerate>` 自动注册到工厂表。
3. 当 RenderNode 需要创建 Drawable 时，工厂表根据节点类型查找对应的 `OnGenerate` 函数创建实例。

```
RSRenderNode::GetRenderNodeDrawableAdapter()
  → 工厂表查找 RSRenderNodeType
  → 调用对应 Drawable::OnGenerate(node)
  → 返回 RSRenderNodeDrawable::Ptr
```

### 绘制流程

```
RSUniRenderProcessor::Process()
  → 遍历节点树
  → 对每个节点调用 drawable->Draw(canvas)
    → RSRenderNodeDrawable::Draw(canvas)
      → ShouldPaint() 检查是否需要绘制
      → OnDraw(canvas) 执行实际绘制
```

`OnDraw()` 是虚函数，各 Drawable 子类实现具体绘制逻辑：

- **RSRenderNodeDrawable**：应用 RenderParams，绘制背景→内容→前景→子节点。
- **RSSurfaceRenderNodeDrawable**：获取 Surface buffer，绘制到画布。
- **RSScreenRenderNodeDrawable**：遍历子 Drawable，提交到 Composer。
- **RSCanvasRenderNodeDrawable**：回放 CmdList。

### OnCapture 模式

`OnCapture()` 是截图专用路径，跳过部分优化（如脏区域裁剪），确保截取完整内容。

### OPINC 绘制缓存

- `RSOpincDrawCache`：节点级绘制缓存，存储上一帧的渲染结果。
- `RSLayerPartDrawCache`：Layer 级部分渲染缓存。
- `GenerateCacheIfNeed()`：当前帧与缓存一致时直接使用缓存图像，跳过绘制。

### RenderGroup 缓存

- `RSRenderGroupCacheDrawable`：子树级缓存，整棵子树内容不变时复用缓存图像。
- 配合 `SetDrawBlurForCache()` / `SetDrawExcludedSubTreeForCache()` 控制缓存内容。

### 并行渲染支持

- `subTreeParallel_` 标记 Drawable 是否来自并行渲染节点。
- 并行渲染时，子 Drawable 可在不同线程同时执行 `OnDraw()`。

### Surface Drawable 特殊逻辑

- `OffscreenRotationInfo`：支持离屏旋转，临时创建 offscreen surface 绘制后旋转合并。
- `RsSubThreadCache` / `RSDrawWindowCache`：UIFirst 子线程缓存绘制。
- `CheckIfSurfaceSkipInMirrorOrScreenshot()`：镜像/截图时跳过特定 Surface。

## RenderNode 到 Drawable 的完整流程

### 生成

Drawable 生成通常发生在节点加入渲染上下文或 pipeline agent 同步节点时：

```text
RSRenderNode / RSSurfaceRenderNode / RSLogicalDisplayRenderNode
  -> DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node)
  -> RenderNodeDrawableRegistrar 按 RSRenderNodeType 查表
  -> XXXRenderNodeDrawable::OnGenerate(node)
  -> node 记录 drawable adapter
```

关键锚点：

- `rosen/modules/render_service/core/rs_render_pipeline_agent.cpp`：
  同步 render node 时调用 `RSRenderNodeDrawableAdapter::OnGenerate()`。
- `rosen/modules/render_service_base/src/pipeline/rs_surface_render_node.cpp`：
  surface node 生成 drawable。
- `rosen/modules/render_service/core/drawable/*_drawable.cpp`：
  各节点类型的 `OnGenerate()` 和 `Registrar`。

新增 RenderNode 类型时，不能只新增节点类，还要补对应 Drawable、Registrar、params 和测试。

### 参数与缓存

Drawable 绘制时依赖两类状态：

- `RSRenderParams`：当前帧从 RenderNode 同步来的可绘制参数。
- Drawable 内部缓存：RenderGroup、OPINC、UIFirst 子线程缓存、Surface buffer 相关缓存。

缓存相关的主要路径：

- `RSRenderNodeDrawable::GenerateCacheIfNeed()`：生成 RenderGroup/OPINC 缓存。
- `RSRenderNodeDrawable::CheckCacheTypeAndDraw()`：判断用缓存、重画还是跳过。
- `RSCanvasRenderNodeDrawable::OnDraw()`：处理 OPINC 前后置逻辑。
- `RSRenderNodeDrawable::ClearRenderGroupResource()`：释放 RenderGroup 缓存资源。

缓存 key 或失效条件变化时，要同时检查 bounds、clip、filter、shadow、HDR、color space、
backface、child blacklist 和并行绘制标记。

### 绘制执行

普通绘制：

```text
drawable->Draw(canvas)
  -> Draw() 判断 capture / ShouldPaint / params 有效性
  -> OnDraw(canvas)
  -> 处理 properties、children、content、foreground、cache
```

截图绘制：

```text
drawable->Draw(canvas)
  -> OnCapture(canvas)
  -> 跳过或调整部分 dirty/skip 优化
  -> 保证 capture 结果完整
```

Surface 绘制还要关注：

- 当前 SurfaceBuffer 是否存在，acquire fence 是否可等待。
- mirror、screenshot、virtual screen 场景是否应跳过某些 Surface。
- UIFirst 子线程缓存是否命中，未命中时是否回主线程绘制。
- HWC/device composition 与 GPU/client composition 的分支是否一致。

### 释放和失效

Drawable 释放不是简单析构：

- RenderNode 销毁时应断开 drawable adapter，避免迟到绘制访问旧节点。
- RenderGroup/OPINC 缓存要通过 clear 路径减少内存统计并释放 surface/image。
- SurfaceBuffer 绘制完成后由 finish callback 和 release fence 控制归还时机。
- 并行渲染、UIFirst 和子线程缓存需要在对应线程或 context 下释放 GPU 资源。

修改释放路径时，要覆盖窗口关闭、节点移除、缓存失效、截图、屏幕旋转
和后台进程冻结。

## 修改检查清单

- 新节点类型是否补齐 Drawable、Registrar、RenderParams、OnDraw、OnCapture。
- Draw 和 OnCapture 是否都处理空 params、空 canvas、空 buffer 和异常状态。
- 缓存命中条件变化是否同步处理 cache clear、内存统计和线程 id。
- OPINC、RenderGroup、UIFirst、并行渲染的分支是否保持一致。
- SurfaceBuffer/fence 相关路径是否有有界等待和 release callback。
- 真实显示、截图、镜像、虚拟屏或 HWC 相关变化需要设备验证。
- 没有真实设备时，记录设备验证缺口，不默认阻塞文档、静态验证或可本地验证的小修。
  但不能把真实显示、截图、镜像或 HWC 行为描述为完整设备验证。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 工厂自动注册 | `RenderNodeDrawableRegistrar` + 静态 `Registrar instance_` | 新增 Drawable 类型只需定义 OnGenerate 和 Registrar，无需修改工厂代码 |
| Draw/OnDraw 分离 | `Draw()` 做前置检查，`OnDraw()` 做实际绘制 | 统一 ShouldPaint、Occlusion 等前置逻辑 |
| OnCapture 独立路径 | `OnCapture()` 虚函数 | 截图需要完整内容，不受脏区域优化影响 |
| OPINC + LayerPart 双层缓存 | `RSOpincDrawCache` + `RSLayerPartDrawCache` | 节点级缓存处理整体不变，Layer 级缓存处理部分变化 |
| subTreeParallel 标记 | `subTreeParallel_` 布尔 | 并行渲染的 Drawable 需要特殊处理线程安全 |
| offscreen rotation | `OffscreenRotationInfo` | 屏幕旋转时通过离屏渲染避免 buffer 尺寸不匹配 |

## 待补充背景

- RSPropertyDrawable 的具体子类和绘制链路（阴影、边框、滤镜等）。
- RSChildrenDrawable 的实现和子节点绘制顺序控制。
- DFX 目录下的调试 Drawable 功能和使用方法。
- RenderGroup 缓存的创建/失效/更新完整生命周期。
- Drawable 与 RSBaseRenderEngine 的交互方式。
