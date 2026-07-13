# UI Effect 与 RS Effect

## 适用范围

- RSFilter / RSDrawingFilter 滤镜类型体系及各派生实现
- RSFilterCacheManager 滤镜缓存机制与 RSFilterCacheMemoryController
- RSFilterDrawable 及其派生：背景滤镜、前景滤镜、材质滤镜、合成滤镜等
- NG Shader Drawable：Coverage / Material / Overlay / Background / Foreground
- RSPropertyDrawableUtils 绘制工具函数
- HPAE 滤镜缓存
- RSEffectRenderNode 与 RSEffectRenderNodeDrawable
- UseEffect（BehindWindow 等）与 DynamicLightUp / Binarization / DynamicDim
- VisualEffect 与 Blender

> Effect 模块（effect_common / color_picker / skia_effectChain / effect_ndk / egl）的
> 架构、接口和类型定义见 [effect-common-ndk.md](effect-common-ndk.md)。
> ColorPicker 独立线程模型见 [rs-color-picker-thread.md](rs-color-picker-thread.md)。

## 快速代码地图

### drawable/ — 视效 Drawable

| 文件路径 | 类 | 说明 |
| --- | --- | --- |
| `…/include/drawable/rs_property_drawable.h` | RSPropertyDrawable, RSFilterDrawable | 属性 Drawable 基类；滤镜 Drawable 基类（缓存管理、脏区域标记） |
| `…/include/drawable/rs_property_drawable_background.h` | RSShadowDrawable, RSMaskDrawable, RSBackgroundColorDrawable, RSBackgroundShaderDrawable, RSBackgroundNGShaderDrawable, RSBackgroundFilterDrawable, RSBackgroundEffectDrawable, RSUseEffectDrawable, RSDynamicLightUpDrawable, RSMaterialFilterDrawable, RSBackgroundImageDrawable | 背景侧全部视效 Drawable |
| `…/include/drawable/rs_property_drawable_foreground.h` | RSBinarizationDrawable, RSColorFilterDrawable, RSLightUpEffectDrawable, RSDynamicDimDrawable, RSCompositingFilterDrawable, RSForegroundFilterDrawable, RSForegroundFilterRestoreDrawable, RSForegroundShaderDrawable, RSSpatialEffectDrawable, RSPixelStretchDrawable, RSBorderDrawable, RSOutlineDrawable, RSParticleDrawable | 前景侧全部视效 Drawable |
| `…/include/drawable/rs_property_drawable_utils.h` | RSPropertyDrawableUtils | 静态绘制工具：DrawFilter / DrawBackgroundEffect / DrawColorFilter / 各 RuntimeShader & Blender 构造 / 颜色拾取辅助 / SDF Shape 应用 |
| `…/include/drawable/rs_coverage_ng_shader_drawable.h` | RSCoverageNGShaderDrawable | 光照覆盖 NG Shader：最多 12 光源、Phong/FeatheringBorder/Normal/SDF 多种光照模式、EDR 支持 |
| `…/include/drawable/rs_material_shader_drawable.h` | RSMaterialShaderDrawable | 材质 NG Shader Drawable |
| `…/include/drawable/rs_overlay_ng_shader_drawable.h` | RSOverlayNGShaderDrawable | 叠加 NG Shader Drawable |

> `…` = `rosen/modules/render_service_base`

### render/ — 滤镜类型与缓存

| 文件路径 | 类 | 说明 |
| --- | --- | --- |
| `…/include/render/rs_filter.h` | RSFilter | 滤镜基类：FilterType 枚举、Hash、工厂方法 |
| `…/include/render/rs_drawing_filter.h` | RSDrawingFilter | 绘制用滤镜：包装 ImageFilter + RSRenderFilterParaBase，提供 DrawImageRect |
| `…/include/render/rs_render_filter_base.h` | RSRenderFilterParaBase | 滤镜参数基类：RSUIFilterType 枚举、ProcessImage / GenerateGEVisualEffect 管线 |
| `…/include/render/rs_filter_cache_manager.h` | RSFilterCacheManager | 两级缓存：snapshot + filtered snapshot |
| `…/include/render/rs_filter_cache_memory_controller.h` | RSFilterCacheMemoryController | 缓存内存限制 |

### render/ — 模糊滤镜实现

| 文件路径 | 类 | FilterType |
| --- | --- | --- |
| `…/include/render/rs_blur_filter.h` | RSBlurFilter | BLUR |
| `…/include/render/rs_render_kawase_blur_filter.h` | RSRenderKawaseBlurFilter | KAWASE |
| `…/include/render/rs_render_light_blur_filter.h` | RSRenderLightBlurFilter | LIGHT_BLUR |
| `…/include/render/rs_render_linear_gradient_blur_filter.h` | RSRenderLinearGradientBlurFilter | LINEAR_GRADIENT_BLUR |
| `…/include/render/rs_render_mesa_blur_filter.h` | RSRenderMesaBlurFilter | MESA |
| `…/include/render/rs_motion_blur_filter.h` | RSMotionBlurFilter | MOTION_BLUR |

### render/ — 其他滤镜实现

| 文件路径 | 类 | FilterType |
| --- | --- | --- |
| `…/include/render/rs_material_filter.h` | RSMaterialFilter | MATERIAL |
| `…/include/render/rs_render_aibar_filter.h` | RSRenderAIBarFilter | AIBAR |
| `…/include/render/rs_light_up_effect_filter.h` | RSLightUpEffectFilter | LIGHT_UP_EFFECT |
| `…/include/render/rs_foreground_effect_filter.h` | RSForegroundEffectFilter | FOREGROUND_EFFECT |
| `…/include/render/rs_spherize_effect_filter.h` | RSSpherizeEffectFilter | SPHERIZE_EFFECT |
| `…/include/render/rs_colorful_shadow_filter.h` | RSColorfulShadowFilter | COLORFUL_SHADOW |
| `…/include/render/rs_attraction_effect_filter.h` | RSAttractionEffectFilter | ATTRACTION_EFFECT |
| `…/include/render/rs_render_water_ripple_filter.h` | RSRenderWaterRippleFilter | WATER_RIPPLE |
| `…/include/render/rs_fly_out_shader_filter.h` | RSFlyOutShaderFilter | FLY_OUT |
| `…/include/render/rs_distortion_shader_filter.h` | RSDistortionShaderFilter | DISTORT |
| `…/include/render/rs_hdr_ui_brightness_filter.h` | RSHdrUiBrightnessFilter | HDR_UI_BRIGHTNESS |
| `…/include/render/rs_render_always_snapshot_filter.h` | RSRenderAlwaysSnapshotFilter | ALWAYS_SNAPSHOT |
| `…/include/render/rs_render_grey_filter.h` | RSRenderGreyFilter | GREY |
| `…/include/render/rs_render_maskcolor_filter.h` | RSRenderMaskColorFilter | MASK_COLOR |
| `…/include/render/rs_color_adaptive_filter.h` | RSColorAdaptiveFilter | — |
| `…/include/render/rs_skia_filter.h` | RSSkiaFilter | Skia 滤镜基类 |

### 其他目录

| 文件路径 | 说明 |
| --- | --- |
| `rosen/modules/render_service_base/include/hpae_base/rs_hpae_filter_cache_manager.h` | HPAE 滤镜缓存 |
| `rosen/modules/render_service_base/include/render/rs_colorspace_convert.h` | 颜色空间转换 |
| `rosen/modules/render_service/core/drawable/rs_effect_render_node_drawable.h` | RSEffectRenderNodeDrawable：效果容器节点绘制 |
| `rosen/modules/render_service_base/include/modifier_ng/appearance/rs_use_effect_render_modifier.h` | UseEffect RenderModifier |

## 核心模型

### 滤镜类型体系

```
RSFilter (rs_filter.h)
  ├── FilterType 枚举（30 个值：NONE / BLUR / MATERIAL / AIBAR / … / FROSTED_GLASS_BLUR）
  ├── Hash() — 基于类型+参数的缓存键
  └── RSDrawingFilter (rs_drawing_filter.h)
        ├── 包装 Drawing::ImageFilter
        ├── 可选携带 RSRenderFilterParaBase（NG 滤镜参数）
        │     └── RSUIFilterType 枚举：AIBAR / GREY / MATERIAL / MESA / MASK_COLOR /
        │       KAWASE / LIGHT_BLUR / PIXEL_STRETCH / WATER_RIPPLE / LINEAR_GRADIENT_BLUR /
        │       FLY_OUT / DISTORTION / ALWAYS_SNAPSHOT
        └── DrawImageRect() — 在指定 Canvas 区域绘制滤镜结果
```

**两套枚举的关系**：`RSFilter::FilterType` 是 RSFilter 级别的旧枚举，用于哈希和类型区分；
`RSUIFilterType` 是 RSRenderFilterParaBase 级别的新枚举，用于 NG 滤镜参数管线
（`ProcessImage` / `GenerateGEVisualEffect`）。一个 `RSDrawingFilter` 可同时携带
旧 ImageFilter 和新 RSRenderFilterParaBase 列表。

### 视效 Drawable 体系

所有视效在服务端通过 `DrawableV2` 命名空间下的 Drawable 类执行绘制。
Drawable 继承自 `RSDrawable`，核心生命周期：

```
OnGenerate(node)  → 从节点属性创建 Drawable 实例
OnUpdate(node)    → 属性变化时刷新 staging 数据
OnSync()          → 将 staging_ 字段同步到渲染字段（render thread 读取）
OnDraw(canvas)    → 实际绘制
```

**关键设计：staging/render 双缓冲**。每个 Drawable 维护 `stagingXxx_`（render thread
写入）和 `xxx_`（OnDraw 读取）两份字段。`OnSync()` 将 staging 拷贝到 render 侧，
保证绘制线程读到一致快照。

#### RSFilterDrawable（滤镜 Drawable 基类）

定义于 `rs_property_drawable.h`，是所有滤镜类 Drawable 的基类：

- 持有 `filter_` / `stagingFilter_` 和 `cacheManager_` / `stagingCacheManager_`。
- `MarkFilterRegionChanged()` / `MarkFilterForceClearCache()` 等方法由渲染管线在
  脏区域/遮挡/旋转变化时调用，驱动缓存失效。
- `IsFilterCacheValid()` / `WouldDrawLargeAreaBlur()` 供渲染管线决策是否跳过或降级。
- `GetAbsRenderEffectRect()` 计算滤镜快照/绘制区域（子类可 override）。
- AIBar 专属：`CheckAndUpdateAIBarCacheStatus()` / `ForceReduceAIBarCacheInterval()`，
  根据 HWC damage 交集动态调整 AIBar 缓存间隔。

**派生类**：

| Drawable | 所在文件 | 说明 |
| --- | --- | --- |
| RSBackgroundFilterDrawable | background.h | 背景 `backgroundFilter`；含 BehindWindow 滤镜生成 |
| RSBackgroundEffectDrawable | background.h | 背景 `backgroundEffect`；自定义 EffectRect 计算 |
| RSMaterialFilterDrawable | background.h | 材质滤镜；额外管理 `clipPath_` / `emptyShape_` |
| RSCompositingFilterDrawable | foreground.h | 合成滤镜（前景侧） |
| RSForegroundFilterDrawable | foreground.h | 前景滤镜；独立 boundsRect 管理 |

#### NG Shader Drawable

四种独立 NG Shader Drawable（不继承 RSFilterDrawable，直接继承 RSDrawable），
均通过 `RSNGRenderShaderBase` + `GEVisualEffectContainer` 驱动 NG 效果渲染：

| Drawable | 说明 |
| --- | --- |
| RSBackgroundNGShaderDrawable | 背景 NG Shader；corner radius + shader |
| RSForegroundShaderDrawable | 前景 NG Shader；EDR 支持 |
| RSMaterialShaderDrawable | 材质 NG Shader |
| RSOverlayNGShaderDrawable | 叠加 NG Shader |

共同模式：`stagingShader_`（RSNGRenderShaderBase）+ `visualEffectContainer_`
（GEVisualEffectContainer）+ `stagingCornerRadius_` / `stagingDrawRect_`，
OnDraw 时将 shader 和 container 提交给 Canvas。

#### RSCoverageNGShaderDrawable（光照覆盖）

最复杂的 NG Shader Drawable，实现点光源光照效果：

- 最多 `MAX_LIGHT_SOURCES`（12）个光源，每个携带 `RSLightSource` + 位置 `Vector4f`。
- 按 `IlluminatedType` 分支选择不同 RuntimeShaderBuilder：
  - `Phong` — Phong 光照模型
  - `FeatheringBorder` / `FeatheringBoard` — 边缘羽化光照
  - `Normal` — 普通光照
  - `SDFBorder` / `SDFContent` / `SDFContentAndBorder` — SDF 距离场光照
- 单光源优化路径（`DrawSingle*` 系列方法），使用 `std::array<float, 1>` 避免 12 元素开销。
- EDR 支持：`GetEnableEDR()` 返回 staging 状态，亮度映射通过 Bezier 曲线 `CalcBezierResultY()`。
- CoverageShader 模式：额外携带 `stagingCoverageShader_`（RSNGRenderShaderBase）。

#### 其他视效 Drawable

| Drawable | 所在文件 | 说明 |
| --- | --- | --- |
| RSUseEffectDrawable | background.h | UseEffect（BEHIND_WINDOW / DEFAULT）；持有 EffectRenderNodeDrawable 弱引用 |
| RSDynamicLightUpDrawable | background.h | 动态亮度；构造 RuntimeBlender |
| RSBinarizationDrawable | foreground.h | AI 反色（二值化）；`aiInvert_` 参数 |
| RSColorFilterDrawable | foreground.h | 颜色滤镜；持有 Drawing::ColorFilter |
| RSDynamicDimDrawable | foreground.h | 动态暗化 |
| RSLightUpEffectDrawable | foreground.h | 亮度提升 |
| RSSpatialEffectDrawable | foreground.h | 空间效果；深度遮挡计算、深度图矩阵 |
| RSPixelStretchDrawable | foreground.h | 像素拉伸 |
| RSShadowDrawable | background.h | 阴影；elevation / SDF / colorStrategy |

### RSFilterCacheManager 缓存机制

滤镜计算是 GPU 密集型操作，缓存管理器通过两级缓存优化：

1. **CacheType 枚举**：
   - `CACHE_TYPE_SNAPSHOT`：缓存未应用滤镜的快照。
   - `CACHE_TYPE_FILTERED_SNAPSHOT`：缓存已应用滤镜的结果。
   - `CACHE_TYPE_BOTH`：两级都缓存。

2. **缓存状态更新**：
   - `UpdateCacheStateWithFilterHash()`：滤镜参数变化时失效。
   - `UpdateCacheStateWithFilterRegion()`：滤镜区域超出缓存区域时失效。
   - `UpdateCacheStateWithDirtyRegion()`：脏区域与缓存区域重叠时失效。

3. **绘制流程**（通过 `RSPropertyDrawableUtils::DrawFilter` / `DrawBackgroundEffect`）：
   ```
   DrawFilter(canvas, filter, cacheManager, nodeId)
     → 检查缓存状态
     → 缓存有效：直接绘制 cachedFilteredSnapshot_
     → 缓存无效：
       1. 截取 snapshot（缓存到 cachedSnapshot_）
       2. 应用滤镜
       3. 缓存结果到 cachedFilteredSnapshot_
       4. 绘制结果
   ```

4. **内存管理**：
   - `CompactFilterCache()`：释放其中一级缓存减少内存。
   - `InvalidateFilterCache()`：手动失效缓存。
   - `MarkFilterForceUseCache()` / `MarkFilterForceClearCache()`：强制使用/清除缓存。

### RSEffectRenderNode

- RSEffectRenderNode 是效果容器节点，子节点内容作为效果输入。
- RSEffectRenderNodeDrawable 负责截取子树快照、应用背景效果、绘制子树。
- 支持动态光效（DynamicLightUp）、灰度系数（GreyCoef）等效果。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 两级滤镜缓存 | CACHE_TYPE_SNAPSHOT + CACHE_TYPE_FILTERED_SNAPSHOT | 滤镜参数不变时跳过重计算，区域不变时跳过重截取 |
| 基于哈希的缓存验证 | `UpdateCacheStateWithFilterHash()` | 快速判断滤镜参数是否变化 |
| Drawable staging/render 双缓冲 | 所有 Drawable 的 `stagingXxx_` / `xxx_` + `OnSync()` | render thread 写入和绘制线程读取解耦，避免锁竞争 |
| RSFilterDrawable 集中缓存管理 | `cacheManager_` + `Mark*` 系列 | 脏区域/遮挡/旋转等外部事件统一驱动缓存失效 |
| NG Shader Drawable 独立于 FilterDrawable | Coverage/Material/Overlay/Background/Foreground NGShader | NG 效果走 GEVisualEffectContainer 管线，不走旧 ImageFilter 路径 |
| 单光源优化路径 | `DrawSingle*` 方法 + `std::array<float, 1>` | 常见单光源场景避免 12 元素数组开销 |
| HPAE 缓存 | `RSHpaeFilterCacheManager` | 硬件平台加速引擎缓存，跨帧复用滤波结果 |

## 待补充背景

- HPAE 滤镜缓存与 RSFilterCacheManager 的关系和切换策略。
- 材质滤镜（MaterialFilter）的参数结构和视觉语义。
- AIBar 滤镜的触发条件和缓存间隔动态调整策略。
- RSSpatialEffectDrawable 的深度遮挡算法和深度图矩阵计算。
