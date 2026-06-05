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

## 待补充背景

- RuntimeEffect 的 SkSL 编译与缓存策略
- ImageFilter Compose 的链式应用顺序与性能影响
- ParticleEffect 的完整生命周期管理
- ShaderEffectLazy 与 ImageFilterLazy 的延迟创建触发时机
- BlendMode 与 Blender 的使用优先级和性能差异
