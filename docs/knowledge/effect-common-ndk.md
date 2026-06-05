# Effect Common / NDK

## 适用范围

- EffectKit 图像效果处理（模糊、亮度、灰度、反色、颜色矩阵）
- ColorPicker 颜色提取
- FilterCommon 通用效果处理管线
- Effect NDK C 接口（`OH_Filter_*` 系列）
- 高级效果：水波纹（WaterGlass）、竖条纹玻璃（ReededGlass）、水滴过渡（WaterDroplet）、遮罩过渡（MaskTransition）
- 亮度映射颜色（MapColorByBrightness）、伽马校正（GammaCorrection）
- NativeBuffer 输出（GPU 直出）
- SDF 效果

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| effect_filter.h | `rosen/modules/effect/effect_ndk/include/effect_filter.h` | OH_Filter NDK C API |
| effect_types.h | `rosen/modules/effect/effect_ndk/include/effect_types.h` | NDK 类型定义与错误码 |
| filter_common.h | `rosen/modules/effect/effect_common/include/filter_common.h` | FilterCommon 通用效果处理 |
| color_picker.h | `rosen/modules/effect/color_picker/include/color_picker.h` | ColorPicker 颜色提取 |
| color_extract.h | `rosen/modules/effect/color_picker/include/color_extract.h` | ColorExtract 颜色提取核心 |
| effect_type.h | `rosen/modules/effect/color_picker/include/effect_type.h` | 效果类型定义 |
| effect_errors.h | `rosen/modules/effect/color_picker/include/effect_errors.h` | 错误码 |
| effect_utils.h | `rosen/modules/effect/color_picker/include/effect_utils.h` | 工具函数 |
| effect_config.gni | `rosen/modules/effect/effect_config.gni` | 构建配置 |
| skia_effectChain/ | `rosen/modules/effect/skia_effectChain/` | Skia 效果链实现 |
| egl/ | `rosen/modules/effect/egl/` | EGL 上下文管理 |

## 核心模型

**Effect NDK 接口** 采用创建-添加效果-获取结果的管线模式：

```
OH_Filter_CreateEffect(pixelmap, &filter)
  → OH_Filter_Blur(filter, radius)         // 模糊
  → OH_Filter_Brighten(filter, brightness)  // 亮度
  → OH_Filter_GrayScale(filter)             // 灰度
  → OH_Filter_Invert(filter)                // 反色
  → OH_Filter_SetColorMatrix(filter, matrix)// 颜色矩阵
  → OH_Filter_Scale(filter, sx, sy, ...)    // 缩放
  → OH_Filter_MaskTransition(...)           // 遮罩过渡
  → OH_Filter_WaterDropletTransition(...)   // 水滴过渡
  → OH_Filter_WaterGlass(...)               // 水波纹
  → OH_Filter_ReededGlass(...)              // 竖条纹玻璃
  → OH_Filter_MapColorByBrightness(...)     // 亮度映射颜色
  → OH_Filter_GammaCorrection(...)          // 伽马校正
  → OH_Filter_GetEffectPixelMap(filter, &result)  // 获取结果
  → OH_Filter_Release(filter)               // 释放
```

**FilterCommon** 是内部效果处理管线：
- `CreateEffect(pixmap, errorCode)` → 创建效果实例
- 效果链：`effectFilters_` 向量，依次添加各种 `EffectImageFilter`
- `Render(forceCPU)` → 执行渲染
- `GetEffectPixelMap()` → 获取结果 PixelMap

**ColorPicker** 颜色提取：
- 从 PixelMap 提取主色调
- `ColorBrightnessMode`：高饱和亮色 / 低饱和亮色 / 暗色
- `PictureLightColorDegree`：从极亮到极暗的分级
- `PictureShadeDegree`：从极浅到极深的分级

**NDK 类型系统**：
- `EffectErrorCode`：SUCCESS / BAD_PARAMETER / UNSUPPORTED_OPERATION / UNKNOWN_ERROR
- `EffectTileMode`：CLAMP / REPEAT / MIRROR / DECAL
- `EffectMaskType`：LINEAR_GRADIENT_MASK / RADIAL_GRADIENT_MASK
- `OH_Filter_ColorMatrix`：5x4 颜色矩阵（float[20]）
- `OH_Filter_Color`：RGBA 四通道浮点颜色
- `OH_Filter_WaterGlassDataParams`：水波纹参数（30+ 字段）
- `OH_Filter_ReededGlassDataParams`：竖条纹玻璃参数
- `OH_Filter_WaterDropletParams`：水滴过渡参数

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 管线式效果添加 | 多个 OH_Filter_* 可链式调用 | 灵活组合效果，避免为每种组合创建单独 API |
| CPU/GPU 双路径 | `Render(forceCPU)` + `GetEffectNativeBuffer` | GPU 直出 NativeBuffer 更高效，CPU fallback 兼容 |
| NDK C 接口 | `OH_Filter_*` 系列函数 | 供 NAPI/CJ/ANI 等上层语言绑定使用 |
| 复杂效果参数化 | `OH_Filter_WaterGlassDataParams` 30+ 字段 | 精细控制视觉效果，支持动态动画 |
| TileMode 控制 | `EffectTileMode` 枚举 | 模糊等效果边缘处理策略 |
| thread_local FilterCommon | `sConstructor_` 线程局部 | 避免多线程竞争，每个线程独立效果实例 |

## 待补充背景

- EffectImageFilter 的完整效果链执行顺序
- GPU NativeBuffer 输出的完整流程与同步机制
- ColorPicker 算法细节（主色调提取策略）
- skia_effectChain 与 FilterCommon 的关系
- 水波纹/竖条纹玻璃效果的 GPU 着色器实现
