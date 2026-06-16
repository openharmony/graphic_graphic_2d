# Color / HDR Capability

## 适用范围

- ColorSpace 色彩空间定义与转换（SRGB / P3 / BT2020 / AdobeRGB / HLG / PQ 等）
- Color 颜色值表示与跨色域转换
- ColorSpaceConvertor 色域转换矩阵计算
- TransferFunc 传递函数（SRGB / HLG / PQ / Linear / Log）
- ColorSpacePrimaries 三基色与白点
- HDR 色域支持（BT2020_HLG / BT2020_PQ / P3_HLG / P3_PQ）
- 有限范围色域（*_LIMIT）
- NDK 色彩空间管理接口

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| color_space.h | `utils/color_manager/export/color_space.h` | ColorSpace 类：色域名/三基色/传递函数/ToLinear/ToNonLinear/ToSkColorSpace |
| color_space_convertor.h | `utils/color_manager/export/color_space_convertor.h` | ColorSpaceConvertor：色域转换矩阵与 Convert 方法 |
| color.h | `utils/color_manager/export/color.h` | Color 类：RGBA 颜色值 + 色域名 + Convert |
| native_color_space_manager.cpp | `utils/color_manager/ndk/native_color_space_manager.cpp` | NDK 色彩空间管理实现 |
| src/ | `utils/color_manager/src/` | ColorSpace / ColorSpaceConvertor / Color 实现 |
| test/ | `utils/color_manager/test/` | 单元测试 |

## 核心模型

**ColorSpace** 描述一个完整的色彩空间：

```
ColorSpace
  ├─ colorSpaceName : ColorSpaceName (37 种预定义 + CUSTOM)
  ├─ toXYZ : Matrix3x3 (RGB→XYZ 变换矩阵)
  ├─ whitePoint : array<float, 2> (白点 xy 坐标)
  └─ transferFunc : TransferFunc (传递函数参数)

关键预定义色域:
  SRGB / DISPLAY_P3 / ADOBE_RGB / BT709 / BT601_EBU / BT601_SMPTE_C
  BT2020 / BT2020_HLG / BT2020_PQ / P3_HLG / P3_PQ
  LINEAR_SRGB / LINEAR_P3 / LINEAR_BT2020
  *_LIMIT 版本 (有限范围)
  NTSC_1953 / PRO_PHOTO_RGB / ACES / ACESCG / CIE_LAB / CIE_XYZ
  EXTENDED_SRGB / LINEAR_EXTENDED_SRGB / SMPTE_C / H_LOG
```

**TransferFunc** 传递函数参数（g/a/b/c/d/e/f）：
- 特殊 g 值标识 HDR：`HLG_G = -3.0f`、`PQ_G = -2.0f`、`LOG_G = -100.0f`
- 标准 SRGB：g=2.4, a=1/1.055, b=0.055/1.055, c=1/12.92, d=0.04045
- 线性：g=1.0

**ColorSpaceConvertor** 色域转换：
- 构造：`ColorSpaceConvertor(srcColorSpace, dstColorSpace, mappingMode)`
- `Convert(Vector3)` → 非线性空间转换
- `ConvertLinear(Vector3)` → 线性空间转换
- 预计算转换矩阵：`transferMatrix`
- 静态实例：sRGB↔P3↔BT2020 六个方向

**Color** 颜色值：
- `r, g, b, a` 四通道浮点值（0.0~1.0+，HDR 可超过 1.0）
- `srcName` 所属色域名
- `Convert(ColorSpaceName)` / `Convert(ColorSpace)` → 跨色域转换
- `PackValue()` → 打包为 uint64

**GamutMappingMode** 色域映射模式：
- `GAMUT_MAP_CONSTANT` / `GAMUT_MAP_EXPENSION`
- `GAMUT_MAP_HDR_CONSTANT` / `GAMUT_MAP_HDR_EXPENSION`

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 特殊 g 值标识 HDR | `HLG_G = -3.0f`、`PQ_G = -2.0f` | 统一 TransferFunc 结构，通过 g 值区分 SDR/HDR 传递函数 |
| Matrix3x3 + Bradford | 色适应使用 Bradford 矩阵 | 业界标准色适应算法，确保跨白点色域转换精度 |
| Skia 互操作 | `ToSkColorSpace()` / SkColorSpace 构造 | 与 Skia 渲染管线无缝对接 |
| 预计算静态转换器 | 6 个静态 ColorSpaceConvertor | 避免每次转换重复计算矩阵 |
| 有限范围色域 | `*_LIMIT` 枚举值 | 视频/广播领域使用 16-235 有限范围 |
| skcms 依赖 | `#include skcms.h` | 使用 Skia 的色彩管理系统保证一致性 |

## 待补充背景

- HDR 亮度元数据（maxCLL/maxFALL）的处理
- GamutMappingMode 四种模式的具体差异与使用场景
- ColorSpace 自定义（CUSTOM）的创建与校验流程
- NDK 色彩空间管理接口的完整 API 列表
- 色域转换在 GPU 着色器中的实现方式
