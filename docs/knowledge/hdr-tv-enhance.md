# HDR / TV Metadata / Image Enhance

## 适用范围

- HDR 显示与 HDR Cast
- TV 元数据（PQ Metadata）采集与传递
- 图像增强场景配置
- 像素格式协商（FP16/RGBA1010102）
- SDR 亮度映射与 EDR

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSHdrUtil | `rosen/modules/render_service/core/feature/hdr/rs_hdr_util.h` | HDR 工具类 |
| RSBaseHdrUtil | `rosen/modules/render_service/composer/composer_service/external_depend/hdr/rs_base_hdr_util.h` | 合成侧 HDR 基础工具 |
| RSTvMetadataManager | `rosen/modules/render_service/core/feature/tv_metadata/rs_tv_metadata_manager.h` | TV 元数据管理器 |
| RSTvMetadataUtil | `rosen/modules/render_service/composer/composer_service/external_depend/tv_metadata/rs_tv_metadata_util.h` | TV 元数据工具 |
| ImageEnhanceManager | `rosen/modules/render_service/core/feature/image_enhance/image_enhance_manager.h` | 图像增强管理器 |

## 核心模型

### HDR

`RSHdrUtil` 提供静态方法处理 HDR 相关逻辑：

1. **HDR Surface 判断**：`CheckIsHdrSurface` 检查 SurfaceNode 是否为 HDR 内容
2. **AI HDR Metadata**：`CheckHasSurfaceWithAiHdrMetadata` 检查 AI HDR 元数据
3. **线性矩阵**：`UpdateSurfaceNodeLayerLinearMatrix` 更新 Layer 线性矩阵
4. **像素格式**：`UpdatePixelFormatAfterHwcCalc` / `CheckPixelFormatWithSelfDrawingNode` HWC 计算后更新像素格式
5. **亮度映射**：`UpdateSurfaceNodeNit` / `SetHDRParam` / `UpdateSelfDrawingNodesNit` 更新 nit 值
6. **HDR Cast**：`IsHDRCast` / `UpdateHDRCastProperties` / `HDRCastProcess` / `SetHDRCastShader` 处理 HDR 投射
7. **HDR 截图**：`NeedUseF16Capture` 判断是否需要 FP16 截图
8. **Color Headroom**：`HDRColorHeadroomMapping` HDR 色域头映射
9. **Metadata**：`EraseHDRMetadataKey` / `SetMetadata` 设置/清除 buffer 的 HDR 元数据

HDR Cast 颜色空间：
- 输入：sRGB primaries + sRGB transfunc + BT601 matrix + Full range
- 输出：BT2020 primaries + HLG transfunc + BT2020 matrix + Full range

### TV Metadata

`RSTvMetadataManager` 为单例，管理 TV PQ 元数据的采集和传递：

- `RecordAndCombineMetadata`：记录并合并元数据
- `CopyTvMetadataToSurface`：将元数据复制到 Surface
- `UpdateTvMetadata` / `RecordTvMetadata`：更新/记录 TV 元数据
- `CollectTvMetadata`：收集 SurfaceSize、ColorPrimaries、HdrType 等信息
- `IsSdpInfoAppId`：判断是否为 SDP 信息应用

关键数据结构 `TvPQMetadata`，包含 `sceneTag`（场景标签）、颜色基色、HDR 类型等。`GetPriority` 方法计算元数据优先级。

缓存机制：`cachedSurfaceNodeId_` / `cachedMetadata_` 缓存上一个 Surface 的元数据。

### Image Enhance

`ImageEnhanceManager` 管理图像增强场景配置：

- `CheckPackageInConfigList`：检查包名是否在增强配置列表
- `mImageEnhanceScene_`：图像增强场景集合
- `pkgs_`：包名配置列表

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| HDR Cast 独立颜色空间定义 | `RSHDRUtilConst` 中的输入/输出颜色空间 | HDR Cast 涉及 SDR→HDR 转换，明确颜色空间避免色彩错误 |
| TV 元数据单例管理 | `RSTvMetadataManager::Instance()` | TV 元数据需要在合成管线中传递，单例保证全局一致性 |
| TV 元数据优先级机制 | `GetPriority` 计算优先级 | 多个 Surface 可能同时产出元数据，优先级决定最终使用哪个 |
| Image Enhance 包名配置 | `CheckPackageInConfigList` | 不同应用的图像增强策略不同，按包名配置实现差异化 |
| HDR 截图 FP16 判断 | `NeedUseF16Capture` | HDR 内容截图需要更高精度格式，FP16 保留 HDR 信息 |

## 待补充背景

- `HdrStatus` 的完整枚举值和判断逻辑
- `TvPQMetadata` 的完整字段定义
- HDR Cast 的 `USE_VIDEO_PROCESSING_ENGINE` 条件编译的完整处理
- AI HDR Metadata 的具体格式和来源
- Image Enhance 的场景配置参数和产品差异
