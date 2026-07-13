# Behind Window Filter / Dynamic Layer Skip

## 适用范围

- 后窗模糊滤镜使能控制
- `RSUseEffectDrawable` 与 `UseEffectType::BEHIND_WINDOW` 的绘制消费
- 后窗模糊数据在父 Surface 生成、子节点消费的两阶段流程
- 动态 Layer 跳过控制
- 全屏遮挡判断
- 渲染节点绘制内容检测

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSBehindWindowFilterManager | `rosen/modules/render_service_base/include/feature/behind_window_filter/rs_behind_window_filter_manager.h` | 后窗滤镜 CCM 全局开关 |
| RSUseEffectDrawable | `rosen/modules/render_service_base/include/drawable/rs_property_drawable_background.h` | UseEffect Drawable（含 BEHIND_WINDOW 消费端） |
| RSUseEffectDrawable 实现 | `rosen/modules/render_service_base/src/drawable/rs_property_drawable_background.cpp` | OnGenerate/OnUpdate/OnSync/OnDraw 实现 |
| UseEffectType 枚举 | `rosen/modules/render_service_base/include/property/rs_properties_def.h` | EFFECT_COMPONENT / BEHIND_WINDOW 枚举定义 |
| DrawUseEffect 工具函数 | `rosen/modules/render_service_base/src/drawable/rs_property_drawable_utils.cpp` | 读取 behind-window 缓存数据并绘制 |
| RSFilterDrawable（后窗生成端） | `rosen/modules/render_service_base/src/drawable/rs_property_drawable.cpp` | OnDraw 中生成 CacheBehindWindowData |
| NeedDrawBehindWindow | `rosen/modules/render_service_base/src/pipeline/rs_surface_render_node.cpp` | 父 Surface 是否需要绘制后窗模糊的判定 |
| Drawable 槽位注册 | `rosen/modules/render_service_base/src/drawable/rs_drawable.cpp` | USE_EFFECT 槽位 → RSUseEffectDrawable::OnGenerate |
| RSDrawableSlot 枚举 | `rosen/modules/render_service_base/include/drawable/rs_drawable.h` | USE_EFFECT 槽位定义（位于 BACKGROUND_FILTER 之后） |
| RSDynamicLayerSkipController | `rosen/modules/render_service_base/include/feature/dynamic_layer_skip/rs_dynamic_layer_skip_controller.h` | 动态 Layer 跳过控制器 |
| LayerSkipContext | 同上文件（配套结构体） | Layer 跳过上下文 |

## 核心模型

### Behind Window Filter

`RSBehindWindowFilterManager` 为单例，提供后窗模糊滤镜的 CCM（Client Composition Manager）开关：

- `IsBehindWindowFilterEnabledByCCM`：查询 CCM 是否启用后窗滤镜
- `SetBehindWindowFilterEnabledByCCM`：设置 CCM 后窗滤镜使能
- `isBehindWindowFilterEnabledCCM_`：atomic bool，默认为 true

这是一个简单的全局开关，由 CCM 配置控制。后窗模糊是指在窗口后方应用模糊效果（如磨砂玻璃），CCM 可以动态关闭此效果以节省性能。

CCM 开关的配置来源：启动时由 `BehindWindowFilterParamParse::BehindWindowFilterCCMInit()` 从
XML 性能配置键 `"BehindWindowBlur"` 读取并初始化；运行时可通过 IPC
（`SET_BEHIND_WINDOW_FILTER_ENABLED` / `GET_BEHIND_WINDOW_FILTER_ENABLED`）动态修改。

### UseEffectType 枚举

`UseEffectType` 定义于 `rs_properties_def.h`，区分两种效果消费模式：

| 值 | 含义 | 数据来源 |
| --- | --- | --- |
| `EFFECT_COMPONENT`（默认） | 参与父 `RSEffectRenderNode` 的效果合成 | `canvas->GetEffectData()` |
| `BEHIND_WINDOW` | 消费窗口后方内容的模糊缓存 | `canvas->GetBehindWindowData()` |

客户端通过 `RSNode::SetUseEffectType` → `RSUseEffectModifier` →
`RSUseEffectRenderModifier` → `RSProperties::SetUseEffectType` 设置，
服务端用 `std::clamp` 限制在 `[0, MAX]` 范围内。

### RSUseEffectDrawable

`RSUseEffectDrawable` 注册在 `RSDrawableSlot::USE_EFFECT` 槽位（位于 `BACKGROUND_FILTER`
之后、`BACKGROUND_STYLE` 之前），在一个节点上仅生成一个实例，但通过 `UseEffectType`
区分两种行为路径。

#### 生命周期

| 阶段 | 方法 | 线程 | 说明 |
| --- | --- | --- | --- |
| 生成 | `OnGenerate` | UI | `GetUseEffect()` 为 false 时返回 nullptr；`BEHIND_WINDOW` 时仅存类型，`EFFECT_COMPONENT` 时向上查找 `RSEffectRenderNode` 并存其 drawable 弱引用 |
| 更新 | `OnUpdate` | UI | 读取 `GetUseEffectType()` 写入 staging；`GetUseEffect()` 为 false 时返回 false 触发擦除 |
| 同步 | `OnSync` | UI→RT | 将 `stagingUseEffectType_` 拷贝到 `useEffectType_` |
| 绘制 | `OnDraw` | RT | 按 `useEffectType_` 分支消费效果数据 |

#### BEHIND_WINDOW 绘制分支

`OnDraw` 中 `useEffectType_ == BEHIND_WINDOW` 的处理逻辑：

1. 前置守卫：`RSSystemProperties::GetEffectMergeEnabled()` 且
   `RSFilterCacheManager::isCCMEffectMergeEnable_` 均为 true 时才进入；
   否则直接 `return`，不执行任何绘制。
2. 截屏/缓存模式：当 canvas 处于 window-freeze-capture 或 drawing-cache 模式时，
   - 若 `GetIsDrawingCache() && !GetCacheBehindWindowData()`（drawing-cache 但无后窗缓存数据），
     直接 `return`，不清空画布。
   - 否则清空画布为透明（`Clear(COLOR_TRANSPARENT)`），使后窗模糊区域透出底层已渲染的模糊内容，
     然后提前返回。
3. 正常绘制路径：调用 `RSPropertyDrawableUtils::DrawUseEffect(canvas, BEHIND_WINDOW)`，
   内部通过 `canvas->GetBehindWindowData()` 获取后窗模糊缓存数据并绘制。

#### EFFECT_COMPONENT 绘制分支

当 canvas 的 `effectData` 为空时，回退到直接绘制父 `RSEffectRenderNode` 的背景滤镜
（通过持有的 `effectRenderNodeDrawableWeakRef_` 调用 `DrawImpl`），
再调用 `DrawUseEffect`。

### RSUseEffectDrawable 与 RSBehindWindowFilterManager 的关系

两者分属后窗模糊流水线的不同层次，协作关系如下：

```
RSBehindWindowFilterManager（全局门控）
    │
    ▼
RSSurfaceRenderNode::NeedDrawBehindWindow()  ← 5 个条件全部满足才为 true
    │                                           其中条件之一：CCM 开关为 true
    ▼
RSBackgroundFilterDrawable::OnDraw（生成端）
    │  对父 Surface 执行模糊 + 缓存
    │  结果存入 canvas->SetCacheBehindWindowData()
    ▼
RSUseEffectDrawable::OnDraw（消费端，子节点）
    │  useEffectType_ == BEHIND_WINDOW
    │  读取 canvas->GetBehindWindowData()
    ▼
RSPropertyDrawableUtils::DrawUseEffect → 绘制模糊结果
```

**门控层**：`RSBehindWindowFilterManager` 是原子布尔单例，只控制"后窗模糊流水线是否激活"。
它被 `RSSurfaceRenderNode::NeedDrawBehindWindow()` 中的一个条件检查——
只有 CCM 开启时，父 Surface 才会生成后窗模糊数据（`RSBackgroundFilterDrawable`）。

**消费层**：`RSUseEffectDrawable` 不直接检查 `RSBehindWindowFilterManager`。
它只依赖 `UseEffectType::BEHIND_WINDOW` 标志和 canvas 上的 `BehindWindowData` 缓存。
如果 CCM 关闭导致父 Surface 未生成后窗模糊数据，则 `GetBehindWindowData()` 返回空，
`RSUseEffectDrawable::OnDraw` 不会绘制后窗模糊内容。

两层之间的隐含约定：**父 Surface 生成、子节点消费**。子节点通过
`ProcessBehindWindowOnTreeStateChanged` / `ProcessBehindWindowAfterApplyModifiers`
注册到父节点的 `childrenBlurBehindWindow_` 集合中，使父 Surface 的
`NeedDrawBehindWindow()` 返回 true。`RSBehindWindowFilterManager` 管门，
`RSUseEffectDrawable` 管画，中间通过 canvas 的 `BehindWindowData` 缓存传递数据。

### Dynamic Layer Skip

> 本节与 Behind Window Filter 属于不同特性，暂放在同一文档中。
> 长远看可拆分到独立的"节点合并绘制"文档。

`RSDynamicLayerSkipController` 提供动态 Layer 跳过判断：

1. **初始化**：`Init(screenRect, globalDisabled)` 设置屏幕矩形和全局禁用标志
2. **节点检查**：`CheckNodeDrawProperty` 检查节点绘制属性
3. **子树绘制内容标记**：`MarkParentSubTreeHasDrawContent` 标记父节点子树有绘制内容
4. **Drawable 检查**：`HasDrawContentDrawableInRange` 检查指定 Drawable 范围内是否有绘制内容
5. **节点遍历**：`VisitRenderNode` 遍历渲染节点
6. **全屏遮挡检测**：
   - `DetectScreenLayerValidity`：检测屏幕 Layer 有效性
   - `DetectScreenLayerValidityInner`：内部检测逻辑，维护 `targetArea` 和 `totalTargetCount`
   - `HasFullScreenSelfDrawingSurface`：判断是否有全屏自绘 Surface
7. **验证**：`VerifyScreenLayerValidity` 验证屏幕 Layer 有效性

`LayerSkipContext` 配套结构体：

- `screenLayerInvalid_`：屏幕 Layer 是否无效
- `relevantSurfaceNodeIds_`：相关 Surface 节点 ID 列表
- `SyncFrom`：从控制器同步状态
- `Reset`：重置状态

内部状态：

- `screenRect_`：屏幕矩形
- `globalOccluderDetected_`：全局遮挡器已检测
- `occluderInstanceRootNodeId_`：遮挡器实例根节点 ID
- `targetSelfDrawingSurface_`：目标自绘 Surface 列表
- `globalDisabled_`：全局禁用标志
- `visitedRenderNodeCount_`：已访问渲染节点计数

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 后窗滤镜 CCM 开关独立 | `RSBehindWindowFilterManager` 单例 | CCM 可动态控制后窗模糊，独立开关避免修改 UI 框架代码 |
| 后窗开关默认启用 | `isBehindWindowFilterEnabledCCM_ = true` | 后窗模糊是默认行为，CCM 可在性能不足时关闭 |
| 生成端与消费端分离 | `RSBackgroundFilterDrawable` 生成 / `RSUseEffectDrawable` 消费 | 父 Surface 生成模糊缓存，子节点透明留白或消费缓存，避免重复模糊 |
| BEHIND_WINDOW 不存父节点弱引用 | `OnGenerate` 中 BEHIND_WINDOW 分支仅存类型 | 后窗模糊数据来自 canvas 缓存而非父节点 drawable，无需向上查找 |
| 截屏/缓存模式下清空透明 | `OnDraw` 中 `Clear(COLOR_TRANSPARENT)` | 让底层已渲染的模糊内容透出，避免在缓存快照中重复合成；drawing-cache 无后窗数据时直接返回不清空 |
| Layer 跳过基于遮挡检测 | `DetectScreenLayerValidity` | 被全屏遮挡的 Layer 无需渲染，跳过可节省 GPU 资源 |
| 全局禁用标志 | `globalDisabled_` | 某些场景（如调试/性能测试）需要全局禁用 Layer 跳过 |
| 自绘 Surface 特殊处理 | `targetSelfDrawingSurface_` | 自绘 Surface（如视频/相机）可能不需要模糊后窗，特殊识别 |

## 待补充背景

- Layer 跳过在统一渲染管线中的调用时序
- 遮挡检测的算法细节和性能开销
- `globalOccluderDetected_` 的触发条件和重置机制
