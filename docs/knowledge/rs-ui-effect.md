# UI Effect 与 RS Effect

## 适用范围

- RSFilter 及其派生类（模糊、颜色滤镜、材质滤镜等）
- RSFilterCacheManager 滤镜缓存机制
- Effect 模块（effect_common、color_picker、skia_effectChain、effect_ndk）
- 背景模糊、前景滤镜、材质滤镜在 RSProperties 中的绘制
- HPAE 滤镜缓存
- ColorPicker 颜色拾取
- VisualEffect 与 Blender

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 滤镜基类 | `rosen/modules/render_service_base/include/render/rs_filter.h` | RSFilter / RSDrawingFilter |
| 滤镜缓存管理 | `rosen/modules/render_service_base/include/render/rs_filter_cache_manager.h` | RSFilterCacheManager：缓存状态管理、DrawFilter |
| 滤镜缓存内存控制 | `rosen/modules/render_service_base/include/render/rs_filter_cache_memory_controller.h` | 缓存内存限制 |
| HPAE 缓存 | `rosen/modules/render_service_base/include/hpae_base/rs_hpae_filter_cache_manager.h` | HPAE 滤镜缓存 |
| 颜色空间 | `rosen/modules/render_service_base/include/render/rs_colorspace_convert.h` | 颜色空间转换 |
| Effect 公共 | `rosen/modules/effect/effect_common/include/filter_common.h` | FilterCommon：公共滤镜参数 |
| ColorPicker | `rosen/modules/effect/color_picker/include/color_picker.h` | ColorPicker：颜色拾取 |
| ColorPicker 公共 | `rosen/modules/effect/color_picker/include/effect_type.h` | 效果类型枚举 |
| 颜色提取 | `rosen/modules/effect/color_picker/include/color_extract.h` | ColorExtract：颜色提取算法 |
| Skia 效果链 | `rosen/modules/effect/skia_effectChain/include/effect_image_chain.h` | EffectImageChain：Skia 效果链 |
| Skia 效果渲染 | `rosen/modules/effect/skia_effectChain/include/effect_image_render.h` | EffectImageRender |
| Effect NDK | `rosen/modules/effect/effect_ndk/include/effect_filter.h` | NDK 滤镜接口 |
| Effect 类型 | `rosen/modules/effect/effect_ndk/include/effect_types.h` | NDK 效果类型定义 |
| EGL 管理 | `rosen/modules/effect/egl/include/egl_manager.h` | EGL 上下文管理 |
| Effect 节点 Drawable | `rosen/modules/render_service/core/drawable/rs_effect_render_node_drawable.h` | RSEffectRenderNodeDrawable |
| Effect 渲染 Modifier | `rosen/modules/render_service_base/include/modifier_ng/appearance/rs_use_effect_render_modifier.h` | UseEffect RenderModifier |

## 核心模型

### 滤镜类型体系

```
RSFilter (基类)
  ├── RSDrawingFilter (绘制用滤镜)
  │     ├── 模糊滤镜 (Blur)
  │     ├── 线性渐变模糊
  │     ├── 运动模糊
  │     ├── 材质滤镜 (Material)
  │     └── AIBar 滤镜
  ├── RSCompositingFilter (合成滤镜)
  └── RSShaderFilter (着色器滤镜)
```

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

3. **绘制流程**：
   ```
   DrawFilter(canvas, filter, nodeId)
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

### Effect 模块架构

Effect 模块独立于 RenderService，提供滤镜和颜色处理能力：

1. **effect_common**：公共滤镜参数和类型定义。
2. **color_picker**：颜色拾取和提取，支持从图像/节点内容提取主色。
3. **skia_effectChain**：基于 Skia 的效果链，支持多滤镜串联。
4. **effect_ndk**：对外暴露的 NDK 滤镜接口。
5. **egl**：EGL 上下文管理，独立 GPU 上下文用于效果处理。

### RSEffectRenderNode

- RSEffectRenderNode 是效果容器节点，子节点内容作为效果输入。
- RSEffectRenderNodeDrawable 负责截取子树快照、应用背景效果、绘制子树。
- 支持动态光效（DynamicLightUp）、灰度系数（GreyCoef）等效果。

### ColorPicker

- `ColorPicker` 从指定区域提取颜色信息。
- 通过 `RSColorPickerModifier` 挂载到节点。
- `SetColorPickerParams()` / `SetColorPickerCallback()` 配置拾取参数和回调。
- 最小间隔 180ms~500ms，避免频繁拾取影响性能。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 两级滤镜缓存 | CACHE_TYPE_SNAPSHOT + CACHE_TYPE_FILTERED_SNAPSHOT | 滤镜参数不变时跳过重计算，区域不变时跳过重截取 |
| 基于哈希的缓存验证 | `UpdateCacheStateWithFilterHash()` | 快速判断滤镜参数是否变化 |
| Effect 独立模块 | effect_common / color_picker / skia_effectChain | 滤镜逻辑与渲染管线解耦，可独立测试和演进 |
| EGL 独立上下文 | `EGLManager` | 效果处理使用独立 GPU 上下文，避免干扰主渲染管线 |
| ColorPicker 回调限流 | `interval` / `notifyThreshold` | 颜色拾取是 GPU 密集操作，限流避免帧率下降 |
| HPAE 缓存 | `RSHpaeFilterCacheManager` | 硬件平台加速引擎缓存，跨帧复用滤波结果 |

## 待补充背景

- HPAE 滤镜缓存与 RSFilterCacheManager 的关系和切换策略。
- Skia 效果链（EffectImageChain）的具体滤镜组合和执行流程。
- 材质滤镜（MaterialFilter）的参数结构和视觉语义。
- AIBar 滤镜的触发条件和渲染逻辑。
- Effect NDK 接口的使用场景和线程模型。
