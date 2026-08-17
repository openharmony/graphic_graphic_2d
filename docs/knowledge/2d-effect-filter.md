# 2D Effect / Filter

## 适用范围

- ShaderEffect 着色器效果（线性/径向/锥形渐变、运行时着色器）
- ColorFilter 颜色过滤（矩阵、混合、LRGB 转 SRGB）
- ImageFilter 图像过滤（模糊、偏移、颜色过滤、着色器）
- MaskFilter 遮罩过滤
- PathEffect 路径效果（虚线、角效果、复合）
- Blender 自定义混合
- RuntimeEffect / RuntimeShaderBuilder 运行时着色器
- Filter 统一过滤描述
- BlurDrawLooper 模糊绘制循环器
- ParticleEffect 粒子效果

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| filter.h | `rosen/modules/2d_graphics/include/effect/filter.h` | Filter 聚合：ColorFilter + ImageFilter + MaskFilter + FilterQuality |
| shader_effect.h | `rosen/modules/2d_graphics/include/effect/shader_effect.h` | ShaderEffect 渐变/着色器基类 |
| color_filter.h | `rosen/modules/2d_graphics/include/effect/color_filter.h` | ColorFilter 颜色矩阵/混合/LRGB 转换 |
| image_filter.h | `rosen/modules/2d_graphics/include/effect/image_filter.h` | ImageFilter 模糊/偏移/颜色/着色器过滤 |
| mask_filter.h | `rosen/modules/2d_graphics/include/effect/mask_filter.h` | MaskFilter 遮罩模糊 |
| path_effect.h | `rosen/modules/2d_graphics/include/effect/path_effect.h` | PathEffect 虚线/角效果/复合 |
| blender.h | `rosen/modules/2d_graphics/include/effect/blender.h` | Blender 自定义混合 |
| runtime_effect.h | `rosen/modules/2d_graphics/include/effect/runtime_effect.h` | RuntimeEffect 运行时着色器 |
| runtime_shader_builder.h | `rosen/modules/2d_graphics/include/effect/runtime_shader_builder.h` | RuntimeShaderBuilder 构建运行时着色器 |
| blur_draw_looper.h | `rosen/modules/2d_graphics/include/effect/blur_draw_looper.h` | BlurDrawLooper 模糊绘制循环器 |
| particle_effect.h | `rosen/modules/2d_graphics/include/effect/particle_effect.h` | ParticleEffect 粒子效果 |
| color_matrix.h | `rosen/modules/2d_graphics/include/effect/color_matrix.h` | ColorMatrix 5x4 颜色矩阵 |
| color_space.h | `rosen/modules/2d_graphics/include/effect/color_space.h` | Drawing 层 ColorSpace |
| blend_mode.h | `rosen/modules/2d_graphics/include/draw/blend_mode.h` | BlendMode 枚举 |

## 核心模型

Effect 体系以 **Filter** 为统一入口，聚合三种子效果：

```
Filter
  ├─ ColorFilter  → 颜色变换（矩阵/混合/LRGB→SRGB）
  ├─ ImageFilter  → 图像级效果（模糊/偏移/颜色过滤/着色器过滤）
  └─ MaskFilter   → 遮罩级效果（模糊遮罩）
  + FilterQuality → NONE/LOW/MEDIUM/HIGH
```

**ShaderEffect** 层次：
- 静态渐变：`MakeLinearGradient`、`MakeRadialGradient`、`MakeConicalGradient`、`MakeSweepGradient`
- 图片着色器：`MakeShaderImage`
- 颜色着色器：`MakeColorShader`
- 运行时：通过 `RuntimeEffect` + `RuntimeShaderBuilder` 构建
- Lazy 版本：`ShaderEffectLazy` 延迟创建

**ImageFilter** 类型（通过 `ImageFilter::FilterType` 枚举区分）：
- Blur、Offset、ColorFilter、Shader、Compose（组合）
- Lazy 版本：`ImageFilterLazy` 延迟创建

**ColorFilter** 类型：
- Matrix（5x4 颜色矩阵）、Blend（与指定颜色混合）、LinearToSRGB、SRGBToLinear

**PathEffect** 类型：
- Dash（虚线）、Corner（角效果）、Compose（组合）、Sum（求和）

**Blender** 替代 BlendMode 用于更灵活的混合：
- `RuntimeBlenderBuilder` 构建运行时自定义混合

数据流：Paint.SetFilter → Filter 持有 ColorFilter/ImageFilter/MaskFilter → Canvas 绘制时 Skia 按链路应用效果。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| Filter 聚合三类子效果 | `Filter` 同时持有 ColorFilter/ImageFilter/MaskFilter | Paint 只需设置一个 Filter 即可组合多种效果 |
| Lazy 版本 Shader/ImageFilter | `ShaderEffectLazy` / `ImageFilterLazy` | 延迟到实际绘制时创建，减少不必要的 GPU 资源 |
| RuntimeEffect | `RuntimeEffect` + `RuntimeShaderBuilder` | 支持用户自定义 SkSL 着色器，扩展效果能力 |
- ParticleBuilder | `particle_builder.h` | 粒子效果构建器 | 运行时着色器粒子效果 |
| BlurDrawLooper | `BlurDrawLooper` 关联到 Paint | 多次绘制带模糊的循环效果（如阴影扩散） |
| ColorMatrix 5x4 | `ColorMatrix` 20 元素数组 | 标准 5x4 颜色变换矩阵，兼容 Skia 和 CSS filter |

## 补充背景

### RuntimeEffect 的 SkSL 编译与缓存

- 入口：`RuntimeEffect::CreateForShader / CreateForES3Shader / CreateForBlender`，构造期同步编译。
- Drawing 接受 GLSL 风格输入，`SkiaRuntimeEffect::GlslToSksl` 用正则改写为 SkSL：
  `uniform shader <n>;` 保留、`<child>(coord)` → `<child>.eval(coord)`、`uniform sampler2D` → `uniform shader`、`texture(...)` → `.eval(...)`、去除浮点 `f` 后缀。
- 编译：`SkRuntimeEffect::MakeForShader / MakeForBlender`，ES3 走 `SkRuntimeEffectPriv::ES3Options()`；产物 `sk_sp<SkRuntimeEffect>` 由 `SkiaRuntimeEffect` 持有。
- 缓存：Drawing 层不缓存 SkRuntimeEffect 实例；GPU 程序缓存由 `ShaderCache` 兜底落盘
  （`memory_handler.cpp` 将其作为 `PersistentCache` 装入 GPU context），统一管理 SkSL/程序缓存。
- Options：`forceNoInline`（USE_M133_SKIA 映射为 `forceUnoptimized`）、`useAF`、`useHighpLocalCoords`、`needDrawingslToSksl`。

### ImageFilter Compose 链式应用与性能

- `CreateComposeImageFilter(f1, f2)`：f1 为外层、f2 为内层，按 `f1(f2(input))` 求值，即 f2 先执行。
- 组合类另有 `BLEND`（两输入按 BlendMode 合成）、`ARITHMETIC`（四系数 `k1..k4` 混合前景/背景，`enforcePMColor` 可钳制 RGB 到 alpha）。
- 性能：每层 Compose 产生一份中间纹理，深度越大中间纹理与全画面处理次数越多；
  `cropRect` 默认 `noCropRect`（无穷大）即不裁剪、全画面处理，必要时显式设 cropRect 缩小处理范围。

### ParticleEffect 生命周期

- 构建：`ParticleBuilder` 经 `AddConfigData(name, configStr, typeSize)` / `AddConfigImage(name, image, option)`
  / `SetUpdateCode(code)` 收集着色器代码与配置，`MakeParticleEffect(maxParticleSize)` 产出 `ParticleEffect`。
- 运行：`UpdateConfigData` / `UpdateConfigImage` 按 name 增量更新数据与贴图，实现下沉到 `ParticleEffectImpl`。
- 持久化：支持 `Serialize` / `Deserialize`。
- 头文件版权 2026，属新增能力，实现细节仍在 `ParticleBuilderImpl` / `ParticleEffectImpl` 内演化。

### ShaderEffectLazy / ImageFilterLazy 延迟创建

- 两者均 `IsLazy() == true`，类型分别为 `LAZY_SHADER`、`LAZY_IMAGE_FILTER`。
- 持有描述性 Obj（`ShaderEffectObj` / `ImageFilterObj`）+ 物化缓存（`shaderEffectCache_` / `imageFilterCache_`，非 lazy）。
- 物化入口 `Materialize()`：调用时按 Obj 构建真实 ShaderEffect/ImageFilter 并缓存，后续调用复用缓存对象。
- 触发时机：Canvas 实际绘制阶段按需物化，配置阶段不创建 GPU 资源。
- 序列化限制：`Serialize/Deserialize` 被禁用（log error），跨进程走 `Marshalling/Unmarshalling`（基于 `ExtendObject`/`Object`）。
- `ShaderEffectLazy` 另支持 `CreateFromShaderEffectObj`（从已构造 Obj 创建）与 `CreateForUnmarshalling`（占位待反序列化）。

### BlendMode 与 Blender 优先级与性能

- BlendMode 为固定枚举，Skia 内建、开销最小；`Blender::CreateWithBlendMode(mode)` 可将其包成 Blender。
- 自定义混合：`RuntimeEffect::CreateForBlender(sl)` 编译 SkSL，再由 `RuntimeBlenderBuilder::MakeBlender()` 产出 Blender，统一经 `BlenderImpl` 落到 Skia。
- 优先级：Paint 同时设置 BlendMode 与 Blender 时，以 Blender（`SetBlender`）为准，BlendMode 被覆盖。
- 性能：固定 BlendMode 走内建路径无编译开销；RuntimeBlender 需 SkSL 编译（同 RuntimeEffect 编译与缓存路径），编译阶段产生额外开销，运行期按 SkSL 执行，灵活但开销高于内建模式。

