# EffectKit API（CAPI / JS）

## 适用范围

本文档覆盖 EffectKit 模块的公开接口层，包括：
- **NDK C API**（`OH_Filter_*` 系列）：图像效果处理管线，供 NAPI/CJ/ANI 等上层语言绑定
- **FilterCommon JS/CJ API**：同一效果管线的内部 JS/CJ 层入口，含 SDF 等仅内部暴露的效果
- **ColorPicker API**：颜色提取接口，主色调/沉浸色/莫兰迪色等提取策略

本文档聚焦 API 语义与使用模型，内部渲染管线（skia_effectChain）和 GPU 着色器细节作为 API 行为的背景补充。

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| effect_filter.h | `rosen/modules/effect/effect_ndk/include/effect_filter.h` | OH_Filter NDK C API 声明 |
| effect_types.h | `rosen/modules/effect/effect_ndk/include/effect_types.h` | NDK 类型定义与错误码 |
| effect_filter.cpp | `rosen/modules/effect/effect_ndk/src/effect_filter.cpp` | OH_Filter NDK C API 实现 |
| filter.h | `rosen/modules/effect/effect_ndk/src/filter/filter.h` | NDK 内部 Filter 类，桥接 C API 到效果管线 |
| filter_common.h | `rosen/modules/effect/effect_common/include/filter_common.h` | FilterCommon JS/CJ 层入口 |
| color_picker.h | `rosen/modules/effect/color_picker/include/color_picker.h` | ColorPicker 颜色提取 API |
| color_extract.h | `rosen/modules/effect/color_picker/include/color_extract.h` | ColorExtract 颜色量化核心（Median Cut） |
| effect_image_render.h | `rosen/modules/effect/skia_effectChain/include/effect_image_render.h` | EffectImageFilter 基类 + 11 子类 |
| effect_image_chain.h | `rosen/modules/effect/skia_effectChain/include/effect_image_chain.h` | EffectImageChain 低层渲染 |
| render_context.h | `rosen/modules/2d_graphics/src/render_context/render_context.h` | RenderContext / RenderContextGL（OpenGL 路径） |
| effect_config.gni | `rosen/modules/effect/effect_config.gni` | 构建配置（仅 `effect_enable_gpu` 开关） |

## API 分层架构

```
┌─────────────────────────────────────────────────────────────────┐
│  NDK C API (effect_filter.h)    │  CJ/JS API (filter_common.h) │
│  OH_Filter_* 函数               │  FilterCommon::static 方法    │
│  · 全部效果 + NativeBuffer 输出  │  · 全部效果 + CreateSDF       │
└──────────────┬──────────────────┴──────────────┬────────────────┘
               │  Filter 类 (filter.h)            │  sConstructor_
               │  effectFilters_ 向量              │  AddNextFilter
               └──────────────┬───────────────────┘
                              │
                              ▼
               EffectImageRender (effect_image_render.h)
               创建 EffectImageChain → 逐个 Apply → Draw
                              │
                              ▼
               EffectImageChain (effect_image_chain.h)
               管理 Drawing::Surface、Canvas、image_、filters_
               两种组合策略：惰性 ImageFilter 组合 / 即时 GPU 着色器
                              │
                              ▼
               Drawing::GE*ShaderFilter / GEShaderFilter 子类
               GPU 着色器执行（Skia RuntimeEffect / SkSL）
```

两个 API 入口共享同一效果管线（`EffectImageRender` → `EffectImageChain`），区别仅在于参数转换层和效果覆盖范围。

## Filter API

### NDK C API：OH_Filter_* 管线模式

采用创建-添加效果-获取结果的管线模式：

```
OH_Filter_CreateEffect(pixelmap, &filter)
  → OH_Filter_Blur(filter, radius)                // 模糊（CPU/GPU 双路径）
  → OH_Filter_Brighten(filter, brightness)         // 亮度
  → OH_Filter_GrayScale(filter)                    // 灰度
  → OH_Filter_Invert(filter)                       // 反色
  → OH_Filter_SetColorMatrix(filter, matrix)       // 颜色矩阵
  → OH_Filter_Scale(filter, sx, sy, ...)           // 缩放
  → OH_Filter_MaskTransition(...)                  // 遮罩过渡（GPU only）
  → OH_Filter_WaterDropletTransition(...)          // 水滴过渡（GPU only）
  → OH_Filter_WaterGlass(...)                      // 水波纹（GPU only）
  → OH_Filter_ReededGlass(...)                     // 竖条纹玻璃（GPU only）
  → OH_Filter_MapColorByBrightness(...)            // 亮度映射颜色（GPU only）
  → OH_Filter_GammaCorrection(...)                 // 伽马校正（GPU only）
  → OH_Filter_GetEffectPixelMap(filter, &result)   // 获取 PixelMap 结果
  → OH_Filter_GetEffectNativeBuffer(filter, buf, &fence, release)  // GPU 直出 NativeBuffer
  → OH_Filter_Release(filter)                      // 释放
```

### FilterCommon JS/CJ API

与 NDK C API 共享效果管线，额外提供 `CreateSDF`（无 NDK 对应接口）：

- `CreateEffect(pixmap, errorCode)` → 创建效果实例，设置 `sConstructor_`（线程局部）
- 效果链：`effectFilters_` 向量，通过 `AddNextFilter` 追加（上限 `MAX_FILTER_COUNT = 1000`）
- `Render(forceCPU)` → 委托 `EffectImageRender::Render`
- `GetEffectPixelMap()` → 获取结果 PixelMap
- `CreateSDF(spreadFactor, generateDerivs)` → SDF 效果（仅 FilterCommon 暴露）

### 效果分类

| 效果 | NDK C API | JS/CJ API | 渲染路径 | CPU 支持 |
|------|-----------|-----------|----------|----------|
| Blur | `OH_Filter_Blur` | `Blur` | CPU/GPU 双路径 | 是 |
| Brighten | `OH_Filter_Brighten` | `Brightness` | CPU（DrawingFilter） | 是 |
| GrayScale | `OH_Filter_GrayScale` | `Grayscale` | CPU（DrawingFilter） | 是 |
| Invert | `OH_Filter_Invert` | `Invert` | CPU（DrawingFilter） | 是 |
| SetColorMatrix | `OH_Filter_SetColorMatrix` | `SetColorMatrix` | CPU（DrawingFilter） | 是 |
| Scale | `OH_Filter_Scale` | `Scale` | GPU 着色器 | 否 |
| MapColorByBrightness | `OH_Filter_MapColorByBrightness` | `MapColorByBrightness` | GPU 着色器 | 否 |
| GammaCorrection | `OH_Filter_GammaCorrection` | `GammaCorrection` | GPU 着色器 | 否 |
| SDF | — | `CreateSDF` | GPU 着色器 | 否 |
| MaskTransition | `OH_Filter_MaskTransition` | `MaskTransition` | GPU 着色器 | 否 |
| WaterDropletTransition | `OH_Filter_WaterDropletTransition` | `WaterDropletTransition` | GPU 着色器（闭源） | 否 |
| WaterGlass | `OH_Filter_WaterGlass` | `WaterGlass` | GPU 着色器（闭源） | 否 |
| ReededGlass | `OH_Filter_ReededGlass` | `ReededGlass` | GPU 着色器（闭源） | 否 |

### NDK 类型系统

| 类型 | 定义位置 | 用途 |
|------|----------|------|
| `EffectErrorCode` | `effect_types.h` | SUCCESS / BAD_PARAMETER / UNSUPPORTED_OPERATION / UNKNOWN_ERROR |
| `EffectTileMode` | `effect_types.h` | CLAMP / REPEAT / MIRROR / DECAL（模糊边缘处理） |
| `EffectMaskType` | `effect_types.h` | LINEAR_GRADIENT_MASK / RADIAL_GRADIENT_MASK |
| `OH_Filter_ColorMatrix` | `effect_types.h` | 5x4 颜色矩阵（float[20]） |
| `OH_Filter_Color` | `effect_types.h` | RGBA 四通道浮点颜色 |
| `OH_Filter_WaterGlassDataParams` | `effect_types.h` | 水波纹参数（25 字段：速度、折射、波形、光照、遮罩等） |
| `OH_Filter_ReededGlassDataParams` | `effect_types.h` | 竖条纹玻璃参数（17 字段：折射、网格光/影、点光源等） |
| `OH_Filter_WaterDropletParams` | `effect_types.h` | 水滴过渡参数（12 字段：位置、半径、扭曲、噪声、光照） |
| `OH_Filter_MapColorByBrightnessParams` | `effect_types.h` | 亮度映射颜色参数（颜色数组 + 位置数组，最多 5 对） |

### 效果链执行顺序

`AddNextFilter` 将 `EffectImageFilter` 追加到 `effectFilters_` 向量尾部（`emplace_back`），保持调用顺序。

`EffectImageRender::Render` 执行步骤：

```
1. effectImage->Prepare(srcPixelMap, forceCPU)  // 创建 Surface、转换像素
2. for (const auto& filter : effectFilters_)    // 从 index 0 到末尾，顺序执行
     filter->Apply(effectImage)                 // 每个效果依次作用于链
3. effectImage->Draw()                          // 最终刷出并读回
```

**先添加的效果先执行（最内层），后添加的效果后执行（最外层）。** 后续效果处理前序效果的输出。

### 两种组合策略

**策略 A：惰性 ImageFilter 组合**（CPU 路径滤镜）

适用于 Brighten、GrayScale、Invert、SetColorMatrix 和 CPU 路径 Blur。

- 通过 `CreateComposeImageFilter(newFilter, existingFilters_)` 将新滤镜包裹为外层
- 实际执行延迟到 `Draw()` 时统一渲染
- `DrawOnFilter()` 一次性绘制所有组合后的 ImageFilter

**策略 B：即时 GPU 着色器应用**

适用于 GPU 路径 Blur、MapColorByBrightness、GammaCorrection、SDF、WaterGlass、ReededGlass、MaskTransition、WaterDropletTransition、Scale。

- 先调用 `UpdateImage()` 刷出已累积的惰性 ImageFilter（如有）
- 然后立即调用 `GEShaderFilter::ProcessImage()` 处理 `image_`，原地替换

**混合链示例**：`Blur(CPU) → Invert → MapColorByBrightness`
1. `Blur(CPU)`：惰性，存入 `filters_`
2. `Invert`：惰性，`CreateComposeImageFilter` 与 `filters_` 组合
3. `MapColorByBrightness`：先 `UpdateImage()` 刷出 Blur+Invert，再即时 GPU 处理

### GPU NativeBuffer 输出

`OH_Filter_GetEffectNativeBuffer` 提供不经过 PixelMap 的 GPU 直出路径：

```
OH_Filter_GetEffectNativeBuffer(filter, nativeBuffer, &syncFenceFd, releaseGpuContext)
  → Filter::RenderNativeBuffer(false, nativeBuffer, syncFenceFd, releaseGpuContext)
    → EffectImageRender::RenderNativeBuffer(...)
      → EffectImageChain::PrepareNativeBuffer(srcPixelMap, dstNativeBuffer)
      → filter->Apply(effectImage)  // 逐个效果
      → EffectImageChain::DrawNativeBuffer()
```

**PrepareNativeBuffer**：仅 Vulkan 路径（`RS_ENABLE_VK`），OpenGL/EGL 不支持。

1. `RsVulkanContext::Get(...)` — 获取 Vulkan 上下文单例
2. `NativeBufferUtils::CreateSurfaceFromNativeBuffer()` — 将 `OH_NativeBuffer` 导入为 VkImage
   - `GetNativeBufferFormatProperties` — 查询格式属性
   - `CreateVkImage` — 从外部 buffer 创建 VkImage
   - `AllocateDeviceMemory` — 通过 `VkImportNativeBufferInfoOHOS` 导入 NativeBuffer 内存
   - `BindImageMemory` — 绑定 VkImage + VkDeviceMemory
   - `Drawing::Surface::MakeFromBackendTexture` — 创建渲染目标 Surface

GPU 直接渲染到 `OH_NativeBuffer`，无需中间 PixelMap 拷贝。

**同步机制**：

```
1. DrawOnFilter()                           // 绘制滤镜效果到 canvas
2. CreateVkSemaphore()                      // 创建可导出为 sync_fd 的 VkSemaphore
3. surface_->Flush(&flushInfo)              // 提交 GPU 命令 + 信号量
4. gpuContext_->Submit()                    // 提交 GPU 工作
5. GetFenceFdFromSemaphore()                // 从 VkSemaphore 提取 sync_fd
6. *syncFenceFd = effectImage->GetfenceId() // 返回给调用者
```

- `VkExportSemaphoreCreateInfo.handleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT`
- 调用者使用 sync_fd 等待 GPU 完成后再读取 NativeBuffer
- `DestroySemaphoreInfo` 使用原子引用计数（初始 2），一个由显式调用减少，一个由 GPU 完成回调减少

**GPU 上下文生命周期**：`releaseGpuContext` 参数控制：
- `false`：保留 GPU 上下文，可复用于后续渲染
- `true`：`EffectImageChain` 析构时调用 `gpuContext_->ReleaseResourcesAndAbandonContext()`，适用于一次性渲染场景

## ColorPicker API

### 接口概览

| 方法 | 策略 | 代码位置 |
|------|------|----------|
| `GetMainColor` | 缩放至 1x1 像素（双线性平均） | `color_picker.cpp:150` |
| `GetLargestProportionColor` | 返回特征色中像素数最多的 | `color_picker.cpp:219` |
| `GetHighestSaturationColor` | 遍历特征色，取 HSV 饱和度最高 | `color_picker.cpp:228` |
| `GetAverageColor` | 按像素数加权平均所有特征色 | `color_picker.cpp:247` |
| `GetImmersiveBackgroundColor` | `GetDominantColor` + `ColorBrightnessMode` 调整 | `color_picker.cpp:610` |
| `GetImmersiveForegroundColor` | 反转背景色的亮度模式 | `color_picker.cpp:647` |
| `GetMorandiBackgroundColor` | 强制 S=9, V=84 保留主色调 | `color_picker.cpp:408` |
| `GetReverseColor` | 亮图返回黑色，暗图返回白色 | `color_picker.cpp:375` |

### 核心算法：Median Cut（VBox）颜色量化

`ColorExtract` 实现与 Android `Palette` 相同的 Median Cut 算法：

| 步骤 | 组件 | 关键代码位置 |
|------|------|-------------|
| 1. 缩放至 100x100 | `CreateScaledPixelMap` | `color_picker.cpp:51-67` |
| 2. 提取非透明像素 | `InitColorValBy8888Color` / `InitColorValBy1010102Color` | `color_extract.cpp:90-150` |
| 3. 计算灰度方差 & 对比度 | `CalcGrayMsd`, `CalcContrastToWhite` | `color_extract.cpp:314-331, 373-388` |
| 4. 量化为每通道 5 位（15 位总计，32768 色） | `QuantizeFromRGB888` | `color_extract.cpp:221-227` |
| 5. 构建直方图（32768 bins） | `GetNFeatureColors` | `color_extract.cpp:396-409` |
| 6. 收集非零颜色 | `GetNFeatureColors` | `color_extract.cpp:411-424` |
| 7. Median Cut：按体积最大 VBox 分裂 | `QuantizePixels` → `SplitBoxes` | `color_extract.cpp:250-274` |
| 8. 沿最长维度中位数分裂 | `VBox::FindSplitPoint` | `color_extract.h:244-265` |
| 9. 每个 VBox 计算加权平均色 | `VBox::GetAverageColor` | `color_extract.h:296-327` |
| 10. 近似回 8 位 RGB | `ApproximateToRGB888` | `color_extract.cpp:237-248` |
| 11. 按像素数降序排列 | `GetNFeatureColors` | `color_extract.cpp:437` |

默认输出 20 个特征色（`specifiedFeatureColorNum_ = 20`），可通过 `SetFeatureColorNum` 配置。

### ColorBrightnessMode 分类

`DiscriminateDarkOrBrightColor`（`color_picker.cpp:514`）基于 HSV 判定：

| 条件 | 模式 |
|------|------|
| V ≤ 80 | `DARK_COLOR` |
| H ∈ (20,50] 且 S > 60 | `HIGH_SATURATION_BRIGHT_COLOR` |
| H ∈ (20,50] 且 S ≤ 60 | `LOW_SATURATION_BRIGHT_COLOR` |
| H ∈ (50,269] 且 S > 40 | `DARK_COLOR`（冷色高饱和视为暗色） |
| H ∈ (50,269] 且 S ≤ 40 | `LOW_SATURATION_BRIGHT_COLOR` |
| 其余 S > 50 | `HIGH_SATURATION_BRIGHT_COLOR` |
| 其余 S ≤ 50 | `LOW_SATURATION_BRIGHT_COLOR` |

### PictureLightColorDegree 判定

基于灰度方差（`grayMsd_`）和对比白度（`contrastToWhite_`）：

| 条件 | 级别 |
|------|------|
| 灰度方差 ≥ 6000 | `EXTREMELY_FLOWERY_PICTURE` |
| contrastToWhite < 1.5 | `EXTREMELY_LIGHT_COLOR_PICTURE` |
| contrastToWhite < 1.9 | `LIGHT_COLOR_PICTURE` |
| contrastToWhite ≤ 7.0 | `FLOWERY_PICTURE` |
| 灰度方差 ≥ 3000 | `DARK_COLOR_PICTURE` |
| 其余 | `EXTREMELY_DARK_COLOR_PICTURE` |

### PictureShadeDegree 判定

仅基于 `contrastToWhite_`，阈值依次为 1.5 / 1.9 / 3.0 / 7.0 / 14.0，对应从极浅到极深 6 级。

## 内部实现：skia_effectChain 管线

`FilterCommon` 是高层 API 入口，`skia_effectChain` 是低层渲染引擎：

```
FilterCommon (effect_common/)       -- 高层 API，存储 effectFilters_、srcPixelMap_
    │                                    使用 thread_local sConstructor_ 模式
    ▼
EffectImageRender (skia_effectChain/) -- 编排器：创建 EffectImageChain → 逐个 Apply → Draw
    │
    ▼
EffectImageChain (skia_effectChain/) -- 低层渲染：管理 Drawing Surface、Canvas、image_
    │                                    两种组合策略（惰性/即时）
    ▼
EffectImageFilter 子类 (skia_effectChain/) -- 多态 Apply() 接口
    │    EffectImageDrawingFilter:  亮度/灰度/反色/颜色矩阵
    │    EffectImageBlurFilter:     模糊
    │    EffectImageMapColorByBrightnessFilter
    │    EffectImageGammaCorrectionFilter
    │    EffectImageSdfFromImageFilter
    │    EffectImageWaterGlassFilter
    │    EffectImageReededGlassFilter
    │    EffectImageMaskTransitionFilter
    │    EffectImageWaterDropletTransitionFilter
    │    EffectImageScaleFilter
    │    EffectImageEllipticalGradientBlurFilter
    ▼
Drawing::GE*ShaderFilter            -- GPU 着色器执行
```

文件依赖：`filter_common.h` → `effect_image_render.h` → `effect_image_chain.h`

### OpenGL 路径 Surface 创建

`EffectImageChain::CreateSurface(forceCPU=false)` 在 OpenGL 路径：
1. `RenderContext::Create()` → 创建 `RenderContextGL` 实例
2. `renderContext_->Init()` → 内部初始化 EGL：`eglGetDisplay` + `eglInitialize` + 创建 PBuffer Surface + `eglCreateContext`（GLES 3.x）+ `eglMakeCurrent`
3. `renderContext_->GetSharedDrGPUContext()` → 获取共享 GPU 上下文
4. `Drawing::Surface::MakeRenderTarget(gpuContext, false, imageInfo)` — 创建渲染目标

## 内部实现：GPU 着色器

### 着色器来源

| 效果 | 着色器来源 | 代码位置 |
|------|-----------|----------|
| WaterGlass | `libgraphics_effect_ext.z.so`（闭源，运行时动态加载） | `effect_image_chain.cpp:953` |
| ReededGlass | `libgraphics_effect_ext.z.so`（闭源，运行时动态加载） | `effect_image_chain.cpp:971` |
| WaterDropletTransition | `libgraphics_effect_ext.z.so`（闭源，运行时动态加载） | `effect_image_chain.cpp:628` |
| MaskTransition | 开源 SkSL（`ge_mask_transition_shader_filter.cpp`） | `graphic_graphics_effect` 仓库 |
| WaterRipple | 开源 SkSL（4 种变体，`ge_water_ripple_filter.h`） | `graphic_graphics_effect` 仓库 |

### 动态加载机制

`GEExternalDynamicLoader` 通过 `dlopen` 加载 `libgraphics_effect_ext.z.so`：

```
GEExternalDynamicLoader::CreateGEXObjectByType(filterType, sizeof(Params), params)
  → dlsym("CreateGEXObjectByType") from libgraphics_effect_ext.z.so
  → 返回 GEShaderFilter*（闭源）
```

- `filterType`：`GEFilterType` 枚举值（WATER_GLASS、REEDED_GLASS、WATER_DROPLET_TRANSITION，定义在 `graphics_effect` 外部组件）
- `sizeof(Params)`：类型安全校验
- 系统属性 `rosen.graphic.gex.enable` 控制是否启用（默认 true）

### MaskTransition 着色器（开源）

SkSL 着色器核心逻辑（`ge_mask_transition_shader_filter.cpp:96-112`）：

```glsl
uniform shader alphaMask;    // 遮罩（线性/径向渐变）
uniform shader topLayer;     // 过渡图
uniform shader bottomLayer;  // 原始图
uniform float factor;        // 混合比例
uniform float inverseFlag;   // 反转标志

vec4 main(float2 fragCoord) {
    float alpha = alphaMask.eval(fragCoord).a * factor;
    alpha = mix(alpha, 1.0 - alpha, inverseFlag);
    vec4 topColor = topLayer.eval(fragCoord);
    vec4 bottomColor = bottomLayer.eval(fragCoord);
    return mix(bottomColor, topColor, 1.0 - alpha);
}
```

MaskTransition 同时作为 NGEffect 滤镜注册（`rs_render_filter_def.in`），可在 Render Service 渲染管线中使用。

### WaterRipple 着色器（开源）

4 种 SkSL 变体（`ge_water_ripple_filter.h`）：

| 变体 | 常量 | 用途 |
|------|------|------|
| `shaderStringSMsend` | SMALL2MEDIUM_SEND=1 | 小→中涟漪（发送方向） |
| `shaderStringSMrecv` | SMALL2MEDIUM_RECV=0 | 小→中涟漪（接收方向） |
| `shaderStringSSmutual` | SMALL2SMALL=2 | 小→小双向涟漪 |
| `shaderStringMiniRecv` | MINI_RECV=3 | 迷你接收涟漪 |

核心着色器技术：
- `smoothstep` 控制波纹包络 + `sin` 生成波形
- 数值微分（`calcWave(dis+h) - calcWave(dis-h) / 2h`）计算波面法线
- UV 位移模拟折射
- 法线与光照方向点积计算高光

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 管线式效果添加 | 多个 OH_Filter_* 可链式调用 | 灵活组合效果，避免为每种组合创建单独 API |
| CPU/GPU 双路径 | `Render(forceCPU)` + `GetEffectNativeBuffer` | GPU 直出 NativeBuffer 更高效，CPU fallback 兼容 |
| NDK C 接口 | `OH_Filter_*` 系列函数 | 供 NAPI/CJ/ANI 等上层语言绑定使用 |
| 复杂效果参数化 | `OH_Filter_WaterGlassDataParams` 25 字段 | 精细控制视觉效果，支持动态动画 |
| TileMode 控制 | `EffectTileMode` 枚举 | 模糊等效果边缘处理策略 |
| thread_local FilterCommon | `sConstructor_` 线程局部 | 避免多线程竞争，每个线程独立效果实例 |
| 惰性 + 即时双组合策略 | CPU 滤镜惰性组合，GPU 着色器即时处理 | CPU 滤镜可批量优化，GPU 着色器需即时消费 |
| 闭源着色器动态加载 | `GEExternalDynamicLoader` + `libgraphics_effect_ext.z.so` | WaterGlass/ReededGlass 等商业效果闭源分发 |
| NativeBuffer 仅 Vulkan | `PrepareNativeBuffer` 依赖 `RS_ENABLE_VK` | Vulkan 支持外部内存导入，OpenGL 不支持 |
| 原子引用计数信号量 | `DestroySemaphoreInfo`（mRefs=2） | GPU 完成回调与显式调用共同管理信号量生命周期 |

## 测试锚点

| 测试文件 | 路径 | 覆盖范围 |
|----------|------|----------|
| effect_filter_unittest.cpp | `rosen/modules/effect/test/unittest/` | 全部 OH_Filter_* NDK API（含 NativeBuffer） |
| effect_image_render_unittest.cpp | `rosen/modules/effect/test/unittest/` | EffectImageFilter 工厂方法 + Render/RenderNativeBuffer |
| effect_image_chain_unittest.cpp | `rosen/modules/effect/test/unittest/` | EffectImageChain 各 Apply* 方法 |
| filter_common_unittest.cpp | `rosen/modules/effect/test/unittest/` | FilterCommon（CJ/JS 层）含 CreateSDF、多效果链 |
| color_picker_unittest.cpp | `rosen/modules/effect/test/unittest/` | ColorPicker 颜色提取 |
| effectfilter_fuzzer.cpp | `rosen/test/render_service/render_service/fuzztest/effectfilter_fuzzer/` | 全部 18 个 OH_Filter_* NDK API 模糊测试 |

GPU 依赖测试仅在 `effect_enable_gpu = true` 时编译（由 `effect_config.gni` 控制）。
