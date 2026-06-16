# 2D 文本 / 字体

## 适用范围

- Font 字体属性设置（大小、hinting、edging、subpixel）
- Typeface 字体文件加载与跨进程共享
- TextBlob 文本布局与绘制
- FontMgr 字体管理
- 字体 fallback、主题字体、emoji 支持
- 文本对比度（高对比度模式）控制
- HM Symbol 矢量图标支持

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| font.h | `rosen/modules/2d_graphics/include/text/font.h` | Font 类：大小/hinting/edging/subpixel/fallback |
| typeface.h | `rosen/modules/2d_graphics/include/text/typeface.h` | Typeface：字体文件加载/Ashmem共享/序列化 |
| text_blob.h | `rosen/modules/2d_graphics/include/text/text_blob.h` | TextBlob：文本布局/绘制/高对比度 |
| font_mgr.h | `rosen/modules/2d_graphics/include/text/font_mgr.h` | FontMgr：系统字体管理 |
| font_style.h | `rosen/modules/2d_graphics/include/text/font_style.h` | FontStyle：粗体/斜体/宽度等 |
| font_metrics.h | `rosen/modules/2d_graphics/include/text/font_metrics.h` | FontMetrics：字体度量 |
| text_blob_builder.h | `rosen/modules/2d_graphics/include/text/text_blob_builder.h` | TextBlobBuilder：构建 TextBlob |
| font_types.h | `rosen/modules/2d_graphics/include/text/font_types.h` | TextEncoding / FontEdging / FontHinting 枚举 |
| hm_symbol.h | `rosen/modules/2d_graphics/include/text/hm_symbol.h` | HM Symbol 矢量图标 |
| rs_xform.h | `rosen/modules/2d_graphics/include/text/rs_xform.h` | RSXform：文本变换矩阵 |
| common_utils.h | `rosen/modules/2d_graphics/include/text/common_utils.h` | 文本公共工具 |

## 核心模型

**Font** 描述字体属性：
- 核心属性：Typeface、size、scaleX、skewX
- 渲染控制：`FontEdging`（ALIAS/ANTI_ALIAS/SUBPIXEL_ANTI_ALIAS）、`FontHinting`（NONE/SLIGHT/MEDIUM/FULL）
- 开关：`SetSubpixel`、`SetForceAutoHinting`、`SetEmbeddedBitmaps`、`SetEmbolden`、`SetLinearMetrics`、`SetBaselineSnap`
- 测量：`MeasureText`、`GetWidths`、`GetWidthsBounds`、`GetSpacing`、`GetMetrics`
- 字形：`UnicharToGlyph`、`TextToGlyphs`、`GetPathForGlyph`、`GetTextPath`
- Fallback：`GetFallbackFont(unicode)` 支持按 Unicode 查找后备字体
- 主题字体：`SetThemeFontFollowed` 在未设置 Typeface 时使用系统主题字体
- 特性支持：`DrawingFontFeatures` + `UnicharToGlyphWithFeatures` / `MeasureSingleCharacterWithFeatures`

**Typeface** 表示单个字体文件：
- 创建方式：`MakeDefault`、`MakeFromFile`、`MakeFromStream`、`MakeFromName`、`MakeFromAshmem`
- Ashmem 共享：`SharedTypeface` 结构体用于跨进程传递，包含 pid + uniqueId 组合的 id_
- FontArguments 支持：变体轴（VariationPosition）的 Hash 计算和跨进程传递
- 注册回调：`TypefaceRegisterCallback` / `GetByUniqueIdCallback` 支持字体注册和查找
- 序列化：`Serialize` / `Deserialize`

**TextBlob** 表示已布局的文本：
- 创建：`MakeFromText`、`MakeFromPosText`、`MakeFromString`、`MakeFromRSXform`
- 高对比度：`TextContrast` 枚举（FOLLOW_SYSTEM / DISABLE_CONTRAST / ENABLE_CONTRAST），通过 `ProcessTextConstrast` 单例管理进程级设置
- 速度/质量偏好：`preferSpeedOverQuality` 位控制渲染偏好
- Emoji 标记：`IsEmoji` / `SetEmoji`
- 序列化：`Serialize(ctx)` / `Deserialize(data, size, ctx)`

数据流：Typeface（字体文件）→ Font（字体属性）→ TextBlob（文本布局）→ Canvas::DrawTextBlob 渲染。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| Typeface Ashmem 共享 | `MakeFromAshmem` + `SharedTypeface` 结构体 | 避免跨进程重复加载字体文件，减少内存和启动开销 |
| ProcessTextContrast 单例 | `std::atomic<TextContrast>` 进程级全局设置 | 高对比度模式需要进程统一行为，避免逐 TextBlob 设置不一致 |
| TextBlobRenderOption 位域 | `uint32_t rawData` + `bits` 联合体 | 压缩存储 textContrast 和 preferSpeedOverQuality，减少内存占用 |
| Font 主题字体跟随 | `themeFontFollowed_` 标志 | 支持系统主题切换字体时自动跟随，同时保持显式设置优先 |
| FontArguments Hash | `CalculateHash` + `AssembleFullHash` | 变体轴组合的哈希计算，用于字体缓存键 |
| Typeface 双 Hash 体系 | `hash_`（数据哈希）+ `fullHash_`（含 FontArgs 哈希） | 区分同文件不同变体轴的 Typeface，确保缓存正确性 |

## 待补充背景

- FontMgr 的字体扫描与注册流程
- TextBlob 序列化中 ctx 参数的具体含义（是否关联 GPU 上下文）
- HM Symbol 的完整渲染管线
- GetFallbackFont 的 fallback 链查找策略
- Texgine 文本引擎与 2D Graphics 文本模块的分工边界
