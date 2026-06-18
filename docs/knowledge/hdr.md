# HDR 行为模型与修改指南

## 文档信息

| 项 | 内容 |
| --- | --- |
| 文档等级 | L3 |
| 适用模块 | `rosen/modules/render_service/core/feature/hdr/`, `rosen/modules/render_service/render/rs_colorspace_convert.cpp` |
| 主要风险 | 设备、显示效果、性能 |
| 可信度 | 已代码核对 |
| 最后核对 | 2026-06-18 |

## 适用范围

改动涉及以下场景时，先读本文，再回到代码确认当前实现：

- HDR 图片显示与 Tone Mapping 处理
- HDR 视频显示与亮度映射
- HDR 投屏（SDR→HDR 颜色空间转换）
- 像素格式协商（FP16/RGBA1010102/RGBA1010108）
- F16 离屏渲染
- VPE（VideoProcessingEngine）颜色空间转换
- HDR 元数据与传递函数检查

## 不适用范围

以下场景不要只依赖本文：

- 颜色空间通用定义，参考 `colorspace.md`

## 快速代码地图

| 方向 | 主要文件 |
| --- | --- |
| HDR 工具类 | `rosen/modules/render_service/core/feature/hdr/rs_hdr_util.h`, `rosen/modules/render_service/core/feature/hdr/rs_hdr_util.cpp` |
| 亮度控制 | `rosen/modules/render_service_base/include/display_engine/rs_luminance_control.h`, `rosen/modules/render_service_base/src/display_engine/rs_luminance_control.cpp` |
| 颜色空间转换 | `rosen/modules/render_service/render/rs_colorspace_convert.h`, `rosen/modules/render_service/render/rs_colorspace_convert.cpp` |
| 图片绘制 | `rosen/modules/render_service_base/src/render/rs_image.cpp` |
| 离屏渲染 | `rosen/modules/render_service/core/drawable/rs_logical_display_render_node_drawable.cpp` |
| 主线程处理 | `rosen/modules/render_service/core/pipeline/main_thread/rs_uni_render_visitor.cpp` |
| 视频节点绘制 | `rosen/modules/render_service/core/drawable/rs_surface_render_node_drawable.cpp` |
| 渲染引擎 | `rosen/modules/render_service/render/rs_uni_render_engine.h`, `rosen/modules/render_service/composer/composer_service/external_depend/engine/rs_base_render_engine.cpp` |

## 核心模型

HDR 显示涉及三个核心组件：

1. **RSLuminanceControl**：亮度控制模块，负责 HDR 决策和亮度映射计算
2. **RSColorSpaceConvert**：颜色空间转换模块，调用 VPE 进行 Tone Mapping 和色域转换
3. **RSHdrUtil**：HDR 工具类，提供像素格式协商、HDR 投屏等静态方法

**HDR 显示流程：**
主线程收集 HDR 状态 → 亮度控制决策 → 渲染线程根据决策选择离屏格式 → 图片绘制时进行 Tone Mapping → 输出显示

**HDR 投屏流程：**
虚拟屏模式判断 → 色域判断 → 固定 10bit 格式 → VPE 颜色空间转换 → 输出到虚拟屏

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 条件编译 VPE 路径 | `#ifdef USE_VIDEO_PROCESSING_ENGINE` | VPE（VideoProcessingEngine）是可选模块，条件编译保证跨平台兼容性 |
| F16 离屏渲染 | `PrepareOffscreenRender` 中 `hdrOrScRGB` 为 true 时使用 F16 | HDR 内容需要更高精度格式，F16 保留 HDR 信息 |
| 固定 10bit 格式 | `SetFixVirtualBuffer10Bit(true)` 当 `ColorGamut == BT2100_HLG` | HDR 投屏场景下，虚拟屏需要固定 10bit 格式避免格式切换 |
| RGBA1010108 门控 | `GetRGBA1010108Enabled()` 运行时判断 | 部分硬件/驱动不支持 1010108 格式，运行时门控 |
| 亮度映射决策 | `RSLuminanceControl::IsHdrOn(screenId)` | 统一的 HDR 开启决策，考虑硬件能力和系统配置 |

## 数据流 / 调用链

### HDR 图片显示流程

主线程收集 → 渲染线程处理 → 图片绘制：

```text
主线程：HDR Status 收集
  RSCanvasNode::SetHDRPresent（客户端）
  → RSCommand/Modifier 传递
  → RSCanvasRenderNode::SetHDRPresent（服务端）
  → RSUniRenderVisitor::QuickPrepareSurfaceRenderNode
  → RSHdrUtil::CheckPixelFormatForHdrEffect
  → screenNode->SetHasUniRenderHdrSurface(true)
  → RSUniRenderVisitor::QuickPrepareScreenRenderNode
  → RSHdrUtil::UpdatePixelFormatAfterHwcCalc
  → RSUniRenderVisitor::HandlePixelFormat
  → RSLuminanceControl::SetHdrStatus(screenId, displayHdrStatus)
  → RSLuminanceControl::IsHdrOn(screenId) 决策
  → 决策结果传递给渲染线程

渲染线程：离屏渲染与像素格式
  RSScreenRenderNodeDrawable::OnDraw
  → RSLogicalDisplayRenderNodeDrawable::OnDraw
  → 判断 needOffscreen = GetNeedOffscreen() || isHdrOn || isScRGBEnable
  → RSLogicalDisplayRenderNodeDrawable::PrepareOffscreenRender
  → if (hdrOrScRGB) 使用 F16 离屏 surface
  → RSRenderNodeDrawable::OnDraw（在离屏 surface 上绘制）
  → FinishOffscreenRender（离屏内容绘制回主 canvas）

图片绘制：Tone Mapping 与色域转换
  RSExtendImageObject::Playback（或 DrawImageWithParmOpItem::Playback）
  → RSImage::CanvasDrawImage
  → RSImage::HDRConvert
  → 检查 isHDRPixelMap || isEDRSurface || isHDRUiCapture
  → 创建 ImageShader
  → RSColorSpaceConvert::ColorSpaceConvertor
  → colorSpaceConverterDisplay_->Process
  → VPE（VideoProcessingEngine）Tone Mapping、亮度映射、色域转换
  → 生成输出 shader
  → canvas.DrawRect 使用生成的 shader 绘制
```

### HDR 投屏流程

```text
HDRCast 流程：
  1. HandleVirtualScreenHDRStatus 判断虚拟屏类型
     - MIRROR_COMPOSITE：镜像模式，检查源屏 HDR 状态
     - EXPAND_COMPOSITE：扩展模式，检查当前屏 HDR 状态
  2. 判断条件：源屏 HDR 开启 && 虚拟屏色域 == BT2100_HLG
  3. UpdateHDRCastProperties 设置固定 10bit 格式
  4. IsHDRCast 返回 true，设置 GRAPHIC_PIXEL_FMT_RGBA_1010102
  5. HDRCastProcess 执行颜色空间转换
  6. SetHDRCastShader 设置着色器进行 SRGB→BT2020_HLG 转换
```

### HDR 视频显示流程

HDR 视频有两种处理路径：直通（Direct Composition）和 Prepare（离屏渲染）

**HDR 视频状态收集：**

```text
CollectInfoForHardwareComposer
  → 遍历 surfaceNode
  → CheckIsHdrSurface 检查 buffer 元数据与传递函数是否符合 HDR 视频标准
  → CheckIsHDRSelfProcessingBuffer 判断是否符合非标准 HDR 视频（希望提亮的图层）
  → 收集到的 HDR 视频状态保存到 surfaceNode 中
```

**UpdateSurfaceNodeNit 亮度映射函数：**

UpdateSurfaceNodeNit 被直通和 Prepare 两种路径共同调用，负责 HDR 视频的亮度映射计算：

```text
UpdateSurfaceNodeNit 处理流程：
  1. 检查 SurfaceNode 的 GetVideoHdrStatus()
  2. NO_HDR 时设置 SDR nit 值和默认矩阵
  3. HDR 时：
     - 获取静态元数据（HdrStaticMetadata）包含 maxContentLightLevel
     - 获取动态元数据（如有）
     - 计算 scaler = CalScaler(maxContentLightLevel, ...)
     - 应用亮度因子（GetHDRBrightnessFactor()）
     - 应用 HDR 调光因子（HdrDimmingProcess）
     - 计算 layerNits = std::clamp(sdrNits * scaler, sdrNits, displayNits)
     - 计算 brightnessRatio = std::pow(layerNits / displayNits, 1.0f / GAMMA2_2)
  4. 自处理 buffer 调用 ProcessEDR
  5. 更新色温补偿矩阵（UpdateSurfaceNodeLayerLinearMatrix）
  6. 亮度信息保存到 surfaceNode（SetDisplayNit/SetSdrNit/SetBrightnessRatio）
```

**HDR 视频直通路径：**

```text
RSMainThread::DoDirectComposition
  → 将 surfaceNode 的 HDR 视频状态传递给 screenNode
  → 调用 UpdateSurfaceNodeNit 计算亮度映射
  → 亮度信息传递到 surfaceNodeParam, RSUniRenderProcessor::CreateLayer 设置给 RSRenderSurfaceLayer给 DSS 使用
  → 将收集到的 HDR status 传递给 luminance
```

**HDR 视频 Prepare 路径：**

```text
主线程：
  RSUniRenderVisitor::QuickPrepareScreenRenderNode
  → RSHdrUtil::UpdateSelfDrawingNodesNit
     → 遍历 selfDrawingNodes
     → 调用 UpdateSurfaceNodeNit 计算亮度映射
     → 更新 headroomMap
  → RSHdrUtil::UpdatePixelFormatAfterHwcCalc
  → HandlePixelFormat
     → RSLuminanceControl::SetHdrStatus 设置 HDR 状态
     → RSLuminanceControl::IsHdrOn 获取 HDR 开启决策

渲染线程离屏部分：与 HDR 图片通路一致

渲染线程绘制部分：
  RSSurfaceRenderNodeDrawable::OnDraw
  → RSSurfaceRenderNodeDrawable::OnGeneralProcess
  → RSSurfaceRenderNodeDrawable::DealWithSelfDrawingNodeBuffer
  → RSSurfaceRenderNodeDrawable::DrawSelfDrawingNodeBuffer
  → RSUniRenderEngine::DrawSurfaceNodeWithParams
  → RSBaseRenderEngine::DrawImage
  → RSBaseRenderEngine::ColorSpaceConvertor（VPE 颜色空间转换）
```

**关键判断：**
- `CheckIsHdrSurface`：检查 buffer 元数据与传递函数是否符合 HDR 视频标准
- `CheckIsHDRSelfProcessingBuffer`：判断是否符合非标准 HDR 视频（希望提亮的图层）
- `GetVideoHdrStatus()`：获取 SurfaceNode 的 HDR 视频状态

**HWC 禁用逻辑与渲染线程绘制：**

HDR 视频渲染线程绘制部分只在需要走 GPU 绘制时执行，包括两种情况：
1. HWC 被禁用时
2. HWC 合成失败需要 redraw 时

如果 HDR 视频可以走 HWC，则不需要使用离屏渲染，渲染线程也不需要 GPU 绘制。

HWC 禁用的触发条件（详见 `hwc-prevalidate.md`）：
- HDR 内容需要走 GPU 绘制时会禁用 HWC，例如 HDR 图片
- 此时 `SetHasUniRenderHdrSurface` 会被设置为 true
- HWC 逻辑中通过 `GetHasUniRenderHdrSurface()` 来判断是否禁用 HWC

因此 HDR 视频的渲染行为取决于是否能够走 HWC：
- 可走 HWC：直接硬件合成，无需离屏和 GPU 绘制
- 不可走 HWC：需要离屏渲染和 GPU 绘制

## 线程 / 进程边界

| 操作 | 线程/进程 | 同步方式 | 注意事项 |
| --- | --- | --- | --- |
| HDR Status 收集 | 主线程 | 直接调用 | 客户端通过 Modifier 传递到服务端 |
| HDR 决策 | 主线程 | 直接调用 RSLuminanceControl | 决策结果写入 RenderParams |
| 离屏渲染 | 渲染线程 | 直接调用 | 根据 isHdrOn 选择离屏格式 |
| VPE 处理 | 渲染线程 | 同步调用 | 阻塞等待 shader 生成 |

## 状态、配置和特性开关

**HDR Status 位掩码（HdrStatus）：**
- `NO_HDR = 0x0000`：非 HDR
- `HDR_PHOTO = 0x0001`：HDR 照片
- `HDR_VIDEO = 0x0010`：HDR 视频
- `AI_HDR_VIDEO_GTM = 0x0100`：AI HDR GTM
- `HDR_EFFECT = 0x1000`：HDR 效果
- `AI_HDR_VIDEO_GAINMAP = 0x10000`：AI HDR GainMap
- `HDR_UICOMPONENT = 0x100000`：HDR UI 组件
- `HDR_COLOR = 0x1000000`：HDR 色彩
- `AI_HDR_VIDEO_AI2020 = 0x10000000`：AI HDR AI2020

**HDRType 枚举：**
- `DEFAULT = 0`：默认类型
- `AIHDR = 1`：AI HDR 类型

**系统属性：**
- `RSSystemProperties::GetHdrImageEnabled()`：HDR 图片功能开关
- `RSSystemProperties::GetHdrVideoEnabled()`：HDR 视频功能开关

**条件编译：**
- `USE_VIDEO_PROCESSING_ENGINE`：VPE 模块开关

**关键常量：**
- `DEFAULT_HDR_RATIO = 1.0f`：默认 HDR 亮度比
- `DEFAULT_SCALER = 1000.0f / 203.0f`：默认缩放因子（203 nits 为 SDR 参考亮度）
- `GAMMA2_2 = 2.2f`：Gamma 2.2 校正

## 行为限制和规格边界

| 限制项 | 当前值/行为 | 代码锚点 | 说明 |
| --- | --- | --- | --- |
| SDR 参考亮度 | 203 nits | `DEFAULT_SCALER = 1000.0f / 203.0f` | 用于计算 HDR→SDR 亮度映射 |
| Gamma 校正 | 2.2 | `GAMMA2_2 = 2.2f` | 符合人眼感知特性 |
| 像素格式 F16 | COLORTYPE_RGBA_F16 | `PrepareOffscreenRender` | HDR 离屏渲染使用 |
| 像素格式 10bit | RGBA_1010102 或 RGBA_1010108 | `GetRGBA1010108Enabled()` | 大部分机型用 1010102 |
| HDR 投屏格式 | GRAPHIC_PIXEL_FMT_RGBA_1010102 | `IsHDRCast` | 固定 10bit 格式 |
| layerNits 边界 | std::clamp(sdrNits * scaler, sdrNits, displayNits) | `UpdateSurfaceNodeNit` | 限制在 SDR 到显示亮度之间 |
| 亮度比计算 | std::pow(layerNits / displayNits, 1.0f / GAMMA2_2) | `UpdateSurfaceNodeNit` | Gamma 2.2 校正 |

## 常见故障排查

### HDR 视频显示异常

优先检查：

- `CollectInfoForHardwareComposer` 是否正确遍历 surfaceNode
- `CheckIsHdrSurface` 检查 buffer 元数据与传递函数是否符合标准
- `CheckIsHDRSelfProcessingBuffer` 是否正确识别非标准 HDR 视频
- `UpdateSurfaceNodeNit` 亮度映射计算是否正确
- 静态/动态元数据是否成功读取（maxContentLightLevel）
- scaler 计算和 layerNits 是否在合理范围内
- 自处理 buffer 是否正确调用 `ProcessEDR`
- 亮度信息是否正确传递给 DSS（RSRenderSurfaceLayer）

### HDR 图片显示异常

优先检查：

- 主线程是否正确收集 HDR 状态（`GetHDRImagePresent()` / `GetHDRUIPresent()`）
- `RSLuminanceControl::IsHdrOn(screenId)` 是否返回 true
- 渲染线程是否正确创建 F16 离屏 surface
- VPE 模块是否可用（`USE_VIDEO_PROCESSING_ENGINE`）
- `RSImage::HDRConvert` 判断条件是否满足

### HDR 投屏失败

优先检查：
- 虚拟屏色域是否为 BT2100_HLG
- 源屏或当前屏 HDR 状态是否正确
- `SetFixVirtualBuffer10Bit(true)` 是否设置
- 像素格式是否为 RGBA_1010102
- VPE 颜色空间转换是否成功

### 离屏渲染异常

优先检查：

- `hdrOrScRGB` 判断是否正确
- F16 surface 创建是否成功
- 硬件是否禁用 FP16（`IsHardwareHdrDisabled`）
- canvas 可替换条件（`canvasReplacable`）

## 修改指南

### 新增 HDR 类型

1. 在 `rs_luminance_control.h` 中添加 `HdrStatus` 枚举值
2. 在 `CheckPixelFormat` 中添加对应的检查逻辑
3. 在 `HandlePixelFormat` 中添加状态收集逻辑
4. 在 `CollectInfoForHardwareComposer` 中添加 HDR 视频检查
5. 补充 `rs_hdr_util_test.cpp` 单测
6. 真实设备验证显示效果

### 修改 HDR 视频亮度映射

1. 修改 `UpdateSurfaceNodeNit` 亮度计算逻辑
2. 调整 `CalScaler` 计算参数
3. 检查静态/动态元数据读取路径
4. 确认亮度信息传递给 DSS 的完整路径
5. 真实设备验证不同亮度下的效果

1. 在 `rs_luminance_control.h` 中添加 `HdrStatus` 枚举值
2. 在 `CheckPixelFormat` 中添加对应的检查逻辑
3. 在 `HandlePixelFormat` 中添加状态收集逻辑
4. 补充 `rs_hdr_util_test.cpp` 单测
5. 真实设备验证显示效果

### 修改 Tone Mapping 策略

1. 修改 `RSColorSpaceConvert::ColorSpaceConvertor` 参数
2. 调整 VPEParameter 设置
3. 检查 `RSImage::HDRConvert` 判断逻辑
4. 补充不同 HDR 类型的测试用例
5. 真实设备验证不同亮度下的效果

### 修改像素格式协商

1. 修改 `GetRGBA1010108Enabled()` 判断条件
2. 检查 `PrepareOffscreenRender` 格式选择逻辑
3. 确认 HWC 像素格式设置
4. 不同机型验证格式兼容性

## 验证建议

构建命令从 OpenHarmony 源码根目录执行：

```sh
./build.sh --product-name <product-name> --build-target graphic_2d --ccache
```

就近测试目标：
- 单测：`rs_hdr_util_test`
- 真实设备验证：需要支持 HDR 的设备

涉及 VPE、Tone Mapping、颜色空间转换时，必须真实设备验证。

## 改动检查清单

- 是否确认 HdrStatus 位掩码操作正确性？
- 是否检查主线程到渲染线程的状态传递完整性？
- 是否确认离屏渲染格式选择逻辑？
- 是否检查 VPE 参数设置？
- 是否确认像素格式协商的完整路径？
- 是否确认 HDR 视频直通和 Prepare 两种路径的完整性？
- 是否检查 HDR 视频亮度映射计算和传递？
- 是否确认 SetHasUniRenderHdrSurface 与 HWC 禁用逻辑的一致性？
- 是否检查渲染线程绘制部分的条件判断（HWC 禁用/失败时才需要 GPU 绘制）？
- 是否需要真实设备验证？

## 相关文档

- `colorspace.md`：颜色空间定义与转换
- `hwc-prevalidate.md`：HWC 预校验与禁用逻辑

## 待补充背景

这些内容需要模块责任人后续补充：

- 各 HdrStatus 类型的完整判断逻辑和触发场景
- RGBA1010108 格式的硬件支持列表和运行时判断逻辑
- 历史线上问题典型案例
