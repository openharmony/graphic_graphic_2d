# 服务端 RenderNode

## 适用范围

- RSRenderNode 及其派生类的核心数据模型与生命周期
- 服务端节点树构建、脏区域管理和遍历
- ModifierNG 在服务端的应用流程
- RenderParams 参数管理
- 动画管理器 RSAnimationManager 在节点上的挂载
- 节点缓存（OPINC、RenderGroup、LayerCache）

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| RenderNode 基类 | `rosen/modules/render_service_base/include/pipeline/rs_render_node.h` | RSRenderNode：核心节点类，约 1500 行 |
| RenderNode 实现 | `rosen/modules/render_service_base/src/pipeline/rs_render_node.cpp` | 节点逻辑实现 |
| SurfaceRenderNode | `rosen/modules/render_service_base/include/pipeline/rs_surface_render_node.h` | RSSurfaceRenderNode：Surface 节点，Buffer 管理 |
| ScreenRenderNode | `rosen/modules/render_service_base/include/pipeline/rs_screen_render_node.h` | RSScreenRenderNode：屏幕节点 |
| DisplayRenderNode | `rosen/modules/render_service_base/include/pipeline/rs_display_render_node.h` | RSdisplayRenderNode：逻辑显示节点 |
| RootRenderNode | `rosen/modules/render_service_base/include/pipeline/rs_root_render_node.h` | RSRootRenderNode：根节点 |
| EffectRenderNode | `rosen/modules/render_service_base/include/pipeline/rs_effect_render_node.h` | RSEffectRenderNode：效果节点 |
| CanvasRenderNode | `rosen/modules/render_service_base/include/pipeline/rs_canvas_render_node.h` | RSCanvasRenderNode：画布节点 |
| 渲染参数 | `rosen/modules/render_service_base/include/params/rs_render_params.h` | RSRenderParams：节点渲染参数 |
| 脏区域管理 | `rosen/modules/render_service_base/include/pipeline/rs_dirty_region_manager.h` | RSDirtyRegionManager |
| 动画管理 | `rosen/modules/render_service_base/include/animation/rs_animation_manager.h` | RSAnimationManager |
| ModifierNG 类型 | `rosen/modules/render_service_base/include/modifier_ng/rs_modifier_ng_type.h` | RSModifierType / RSPropertyType 枚举 |

## 核心模型

### 节点继承体系

```
RSRenderNode (enable_shared_from_this)
  ├── RSSurfaceRenderNode  -- 窗口/图层，持有 SurfaceHandler + Buffer
  ├── RSScreenRenderNode   -- 物理屏幕，关联 HdiOutput
  ├── RSdisplayRenderNode  -- 逻辑显示，支持镜像
  ├── RSRootRenderNode     -- 根节点
  ├── RSEffectRenderNode   -- 效果容器，背景模糊
  └── RSCanvasRenderNode   -- 画布，持有 CmdList
```

`RSRenderNodeType` 枚举标识类型（RS_NODE / SURFACE_NODE / SCREEN_NODE 等）。

### 核心数据结构

- **ModifiersNGMap**：`std::map<RSModifierType, ModifierNGContainer>`，存储所有服务端 Modifier。
- **RSRenderParams**：渲染参数快照，每帧 Prepare 时从节点属性计算。
- **RSDirtyRegionManager**：脏区域追踪，支持合并、合并历史和部分渲染。
- **CurFrameInfoDetail**：当前帧信息（prepareSeqNum、vsyncId、subTreeSkipped 等）。

### 帧处理流程

```
Prepare:
  QuickPrepare(visitor)               // 快速准备
  → PrepareSelfNodeForApplyModifiers() // 应用 Modifier 到 staging 属性
  → ApplyModifier()                    // ModifierNG 应用

Process:
  RSRenderNodeDrawable::OnDraw(canvas) // 绘制
  → ApplyRenderParams()                // 应用渲染参数
  → 绘制背景/前景/内容
```

### ModifierNG 应用

1. 服务端 `RSRenderModifier::Apply(canvas, properties)` 将属性变更写入 `RSProperties`。
2. `ApplyLegacyProperty()` 将 Modifier 属性应用到传统 RSProperties 接口。
3. Modifier 按 RSModifierType 排序，保证几何→外观→自定义的执行顺序。

### 脏区域与缓存

- **NodeDirty** 枚举：CLEAN / DIRTY，标记节点是否有内容变更。
- **LayerDrawContent** 位集：SELF / SUBTREE / UPDATE，用于动态 Layer 跳过优化。
- **OPINC**：`RSOpincCache` / `RSLayerPartRenderCache`，节点级渲染缓存。
- **RenderGroup**：`RSRenderGroupCacheDrawable`，子树缓存。

### 动画管理

- `RSAnimationManager` 挂载在 RSRenderNode 上，管理该节点所有活跃动画。
- 每帧 `Animate(timestamp)` 驱动动画估值，更新 RSRenderProperty。
- `RSFrameRateRange` 与 HGM 协商帧率。

### 跨屏节点

- `AddCrossParentChild()` / `AddCrossScreenChild()` 支持节点跨屏挂载。
- `sourceCrossNode_` / `isCloneCrossNode_` 标识克隆关系。
- `hasVisitedCrossNode_` 防止递归遍历。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| enable_shared_from_this | `RSRenderNode : public std::enable_shared_from_this` | 树操作、回调等场景需要安全获取 shared_ptr |
| ModifiersNGMap 排序 | `std::map<RSModifierType, ModifierNGContainer>` | 保证 Modifier 按类型有序应用，几何先于外观 |
| RenderParams 快照 | `RSRenderParams` 独立于 RSRenderNode | 渲染参数在帧内不变，避免读写竞争 |
| NodeDirty 双层标记 | NodeDirty + LayerDrawContent 位集 | 区分"有内容"和"有更新"，支持 Layer 跳过优化 |
| OPINC 缓存 | `RSOpincCache` / `RSLayerPartRenderCache` | 静态子树缓存，跳过无变化节点的完整绘制 |
| SingleFrameComposer | `rs_single_frame_composer.h` | 首帧优化，在主线程提前合成 |

## 待补充背景

- RSRenderParams 的具体 Prepare 算法和脏区域合并策略。
- OPINC 缓存的失效条件和刷新策略。
- RenderGroup 与 OPINC 的关系和使用场景区别。
- SingleFrameComposer 的完整流程和触发条件。
- 节点在服务端被 CleanResources 时的完整资源回收链路。
