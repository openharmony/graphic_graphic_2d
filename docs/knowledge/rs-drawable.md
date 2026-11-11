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

## 补充背景

### RSPropertyDrawable 的具体子类和绘制链路

#### 分类和继承体系

RSPropertyDrawable 体系分为三层：

1. **基类层**
   - `RSPropertyDrawable`：基础属性绘制类，使用 `DrawCmdList` 记录和回放绘制命令。
   - `RSFilterDrawable`：滤镜绘制基类，继承自 `RSDrawable`，包含 `RSFilterCacheManager` 缓存机制。
   - `RSDrawable`：所有 Drawable 的顶层基类，定义 `OnDraw`、`OnSync`、`OnUpdate` 等核心接口。

2. **背景属性 Drawable**（`rs_property_drawable_background.h/cpp`）
   - `RSShadowDrawable`：阴影绘制。支持三种阴影模式：
     - 普通阴影（elevation > 0）：使用 `DrawShadow` 绘制。
     - MaskFilter 阴影（radius > 0）：使用 `DrawShadowMaskFilter` 绘制。
     - SDF 阴影：通过 `GE_SHADER_SDF_SHADOW` 着色器绘制，支持自定义形状。
   - `RSMaskDrawable`：遮罩绘制，使用 `DrawCmdList` 记录遮罩路径。
   - `RSBackgroundColorDrawable`：背景色绘制。
   - `RSBackgroundShaderDrawable`：背景着色器绘制（渐变等）。
   - `RSBackgroundNGShaderDrawable`：背景 NG 着色器绘制，使用 `GEVisualEffectContainer`。
   - `RSBackgroundImageDrawable`：背景图片绘制，支持 PixelMap 和 ASTC 压缩纹理。
   - `RSBackgroundFilterDrawable`：背景滤镜绘制（毛玻璃、模糊等），继承 `RSFilterDrawable`。
   - `RSBackgroundEffectDrawable`：背景效果绘制，继承 `RSFilterDrawable`。
   - `RSUseEffectDrawable`：使用效果节点绘制，引用 `EffectRenderNode` 的 drawable。
   - `RSDynamicLightUpDrawable`：动态亮度调节绘制，使用自定义 Blender。
   - `RSMaterialFilterDrawable`：材质滤镜绘制，支持空形状裁剪。

3. **前景属性 Drawable**（`rs_property_drawable_foreground.h/cpp`）
   - `RSBinarizationDrawable`：二值化绘制（AI 反色效果）。
   - `RSColorFilterDrawable`：颜色滤镜绘制。
   - `RSLightUpEffectDrawable`：亮度效果绘制。
   - `RSDynamicDimDrawable`：动态变暗绘制。
   - `RSCompositingFilterDrawable`：合成滤镜绘制（前景），继承 `RSFilterDrawable`。
   - `RSForegroundFilterDrawable`：前景滤镜绘制。
   - `RSForegroundFilterRestoreDrawable`：前景滤镜恢复绘制，与 `RSForegroundFilterDrawable` 配对使用。
   - `RSForegroundColorDrawable`：前景色绘制。
   - `RSForegroundShaderDrawable`：前景着色器绘制，支持 EDR 效果。
   - `RSBorderDrawable`：边框绘制。支持 SDF 边框（`GE_SHADER_SDF_BORDER`）和普通边框两种模式。
   - `RSOutlineDrawable`：轮廓绘制。支持 SDF 轮廓（`GE_SHADER_SDF_OUTLINE`）和普通轮廓两种模式。
   - `RSParticleDrawable`：粒子特效绘制。
   - `RSPixelStretchDrawable`：像素拉伸绘制。

4. **其他属性 Drawable**（`rs_property_drawable.h/cpp`）
   - `RSFrameOffsetDrawable`：帧偏移绘制，通过 Translate 命令调整子节点位置。
   - `RSClipToBoundsDrawable`：裁剪到边界绘制。支持五种裁剪类型：
     - `CLIP_PATH`：使用自定义 Path 裁剪。
     - `CLIP_RRect`：使用圆角矩形裁剪，支持 `ClipRRectOptimization` 优化。
     - `CLIP_RECT`：使用矩形裁剪。
     - `CLIP_IRECT`：使用整数矩形裁剪（用于 AppWindow 旋转缓存）。
     - `CLIP_SDF`：使用 SDF 着色器裁剪（`GE_SHADER_SDF_CLIP`）。
   - `RSClipToFrameDrawable`：裁剪到帧绘制。

5. **其他 Drawable**（`rs_misc_drawable.h/cpp`）
   - `RSChildrenDrawable`：子节点绘制（详见下节）。
   - `RSCustomModifierDrawable`：自定义修饰器绘制，回放 `SimpleDrawCmdList`。
   - `RSSaveDrawable`/`RSRestoreDrawable`：Canvas 状态保存/恢复。
   - `RSCustomSaveDrawable`/`RSCustomRestoreDrawable`：自定义状态保存/恢复。
   - `RSEnvFGColorDrawable`：环境前景色绘制。
   - `RSEnvFGColorStrategyDrawable`：环境前景色策略绘制（反色背景色策略）。
   - `RSCustomClipToFrameDrawable`：自定义裁剪到帧绘制。
   - `RSBeginBlenderDrawable`/`RSEndBlenderDrawable`：Blender 开始/结束绘制。

#### 绘制链路

RSPropertyDrawable 的绘制链路分为三阶段：

1. **Update 阶段**（主线程或服务端线程）
   ```
   RSRenderNode::UpdateDrawable()
     → RSPropertyDrawable::OnUpdate(node)
       → 从 node.GetRenderProperties() 读取属性值
       → 写入 stagingXXX_ 成员变量
       → 使用 RSPropertyDrawCmdListUpdater 记录绘制命令到 stagingDrawCmdList_
       → 设置 needSync_ = true
   ```
   
   - `RSPropertyDrawCmdListUpdater` 在构造时创建 `ExtendRecordingCanvas`。
   - 在析构时将 `DrawCmdList` 移动到 `target_->stagingDrawCmdList_`。
   - 这种 RAII 模式确保绘制命令一定会被记录。

2. **Sync 阶段**（渲染线程）
   ```
   RSRenderNodeDrawable::OnSync()
     → RSPropertyDrawable::OnSync()
       → std::swap(drawCmdList_, stagingDrawCmdList_)
       → 交换其他 staging/render 成员变量
       → needSync_ = false
   ```
   
   - Sync 使用 swap 而非拷贝，避免数据复制开销。
   - 旧 `drawCmdList_` 会通过 `AddToClearCmdList` 延迟释放。

3. **Draw 阶段**（渲染线程）
   ```
   RSRenderNodeDrawable::Draw(canvas)
     → RSPropertyDrawable::OnDraw(canvas, rect)
       → drawCmdList_->Playback(*canvas)
   ```
   
   - 直接回放录制好的 `DrawCmdList`。
   - 对于 `RSFilterDrawable`，会调用 `RSPropertyDrawableUtils::DrawFilter` 处理滤镜缓存。

#### 滤镜绘制链路

`RSFilterDrawable` 的绘制链路更复杂：

```
OnUpdate(node)
  → 从 properties 获取 filter
  → stagingFilter_ = filter
  → stagingCacheManager_->UpdateFilterInfo(filter)

OnSync()
  → filter_ = std::move(stagingFilter_)
  → stagingCacheManager_->SwapDataAndInitStagingFlags(cacheManager_)

OnDraw(canvas, rect)
  → GetAbsRenderEffectRect() 计算 snapshot 和 draw 区域
  → RSPropertyDrawableUtils::DrawFilter(canvas, filter_, cacheManager_, ...)
    → cacheManager_->IsFilterCacheValid() 检查缓存
    → 若缓存有效：直接使用缓存图像
    → 若缓存失效：重新绘制滤镜效果并更新缓存
```

滤镜缓存管理（`RSFilterCacheManager`）支持：
- 区域变化时清除缓存（`MarkFilterRegionChanged`）。
- 大面积模糊时强制使用缓存（`MarkFilterRegionIsLargeArea`）。
- 跳帧时清除缓存（`MarkForceClearCacheWithLastFrame`）。
- AIBar 滤镜的缓存间隔调整（`ForceReduceAIBarCacheInterval`）。

#### SDF 着色器绘制链路

SDF（Signed Distance Field）相关的 Drawable 使用 `GEVisualEffectContainer`：

```
RSShadowDrawable/RSClipToBoundsDrawable::OnUpdate()
  → 从 properties 获取 SDFShape
  → sdfShape->GenerateGEVisualEffect() 生成 GEVisualEffect
  → geVisualEffect->GenerateShaderShape() 生成 GEShaderShape
  → 创建 GEVisualEffectContainer 并添加 chained filter

OnDraw(canvas, rect)
  → geContainer_->SetGeometry(matrix, rect, ...)
  → GraphicsEffectEngine::GERender::DrawShaderEffect(canvas, *geContainer_, rect)
```

支持的 SDF 着色器类型：
- `GE_SHADER_SDF_SHADOW`：SDF 阴影。
- `GE_SHADER_SDF_CLIP`：SDF 裁剪。
- `GE_SHADER_SDF_BORDER`：SDF 边框。
- `GE_SHADER_SDF_OUTLINE`：SDF 轮廓。

### RSChildrenDrawable 的实现和子节点绘制顺序控制

#### 实现机制

`RSChildrenDrawable` 负责绘制 `RSRenderNode` 的所有子节点，关键成员：

```cpp
class RSChildrenDrawable : public RSDrawable {
private:
    std::vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> childrenDrawableVec_;
    std::vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> stagingChildrenDrawableVec_;
    bool childrenHasSharedTransition_ = false;
};
```

#### 更新流程

```
OnUpdate(node)
  → children = node.GetSortedChildren()  // 获取排序后的子节点列表
  → stagingChildrenDrawableVec_.clear()
  → 遍历 children：
    → 若有 SharedTransition：调用 OnSharedTransition(child) 处理
    → childDrawable = RSRenderNodeDrawableAdapter::OnGenerate(child)
    → stagingChildrenDrawableVec_.push_back(childDrawable)
```

#### ShadowBatching 模式

当 `node.GetRenderProperties().GetUseShadowBatching()` 为 true 时：

```
遍历 children：
  → shadowDrawable = RSRenderNodeDrawableAdapter::OnGenerateShadowDrawable(child, childDrawable)
  → stagingChildrenDrawableVec_.push_back(shadowDrawable)
  → pendingChildren.push_back(childDrawable)
→ 合并两向量：
  → stagingChildrenDrawableVec_.insert(end(), pendingChildren.begin(), pendingChildren.end())
```

此模式将所有阴影先绘制，再绘制所有子节点内容，减少 Canvas 状态切换次数，提升阴影批量绘制性能。

#### SharedTransition 处理

SharedTransition（共享转场动画）用于两个节点间的过渡动画：

```
OnSharedTransition(node)
  → pairedNode = sharedTransitionParam->GetPairedNode(nodeId)
  → 若 pairedNode 不存在或不在树上：返回 false（不跳过）
  → 若未配对（paired_ = false）：返回 false（可能绘制两次）
  → 若无 Relation：SetNeedGenerateDrawable(true)，返回 true（跳过）
  → 若 IsLower(nodeId)：返回 true（跳过，由高层节点绘制）
  → 若 IsHigher(nodeId)：
    → 先添加 pairedNode drawable
    → SetNeedGenerateDrawable(false)
    → 返回 false（继续绘制当前节点）
```

SharedTransition 确保：
- 低层节点跳过绘制，避免重复。
- 高层节点先绘制配对节点（低层），再绘制自己。
- 配对节点在动画期间以正确顺序和位置绘制。

#### 同步和绘制

```
OnSync()
  → std::swap(stagingChildrenDrawableVec_, childrenDrawableVec_)
  → RSRenderNodeDrawableAdapter::AddToClearDrawables(stagingChildrenDrawableVec_)

OnDraw(canvas, rect)
  → 遍历 childrenDrawableVec_：
    → drawable->Draw(*canvas)
```

绘制时使用 `__builtin_prefetch` 预取后续 drawable，减少 cache miss：

```cpp
for (size_t i = 0; i < childrenDrawableVec_.size(); i++) {
    size_t prefetchIndex = i + 2;
    if (prefetchIndex < childrenDrawableVec_.size()) {
        __builtin_prefetch(&(childrenDrawableVec_[prefetchIndex]), 0, 1);
    }
    childrenDrawableVec_[i]->Draw(*canvas);
}
```

#### 子节点排序

子节点顺序由 `RSRenderNode::GetSortedChildren()` 决定，排序规则：

- Z-index：低 Z-index 子节点先绘制。
- TreeId：相同 Z-index 时按树 ID 排序。
- 添加顺序：作为稳定排序的最后依据。

修改子节点添加逻辑、Z-index 或 SharedTransition 时，要检查 `GetSortedChildren`、`OnSharedTransition` 和 ShadowBatching 模式的一致性。

---
