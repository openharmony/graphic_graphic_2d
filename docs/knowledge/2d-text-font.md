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

## 补充背景

### FontMgr 的字体扫描与注册流程

- 两种实例：`CreateDefaultFontMgr`（系统默认）、`CreateDynamicFontMgr`（动态）。
- 动态注册：`LoadDynamicFont(familyName, data, dataLength)` 或 `LoadDynamicFont(familyName, Typeface)` 按家族名加载。
- 主题字体：`LoadThemeFont(familyName, themeName, data)` / `LoadThemeFont(themeName, Typeface)`。
- 系统扫描：`ParseInstallFontConfig(configPath, fontPathVec)` 解析安装配置文件取字体路径列表，按 `FontCheckCode`（SUCCESSED / ERROR_PARSE_CONFIG_FAILED / ERROR_TYPE_OTHER）返回；`GetFontFullName(fontFd, fullnameVec)` 从 fd 读取字体全名（`FontByteArray` UTF-16BE）。
- 枚举/查询：`CountFamilies` / `GetFamilyName` / `CreateStyleSet`、`MatchFamily(name)→FontStyleSet`、`MatchFamilyStyle(name, style)→Typeface`。

### TextBlob 序列化 ctx 参数的含义

- ctx 是 `TextBlob::Context*`（非 GPU 上下文），持有 `typeface_` 与 `isCustomTypeface_`。
- 实现路径：Serialize/Deserialize 经 `SkSerialProcs/SkDeserialProcs.fTypefaceCtx` 透传给 `SkiaTypeface::SerializeTypeface/DeserializeTypeface`。
- Serialize：对 customTypeface 调 `textblobCtx->SetTypeface(customTypeface)` 记录自定义字体，本体仍走 `typeface->serialize()`。
- Deserialize：若 ctx 携带 typeface 则直接复用（避免重新反序列化字体文件），否则回退 `SkTypeface::MakeDeserialize`。
- 设计意图：跨进程传递 TextBlob 时携带自定义 Typeface，避免重复加载/反序列化字体。

### HM Symbol 渲染管线

- 数据：`DrawingHMSymbolData`（symbolGlyphId + layers + renderGroups + path_ + symbolId）。
- 配置：`DrawingSymbolLayersGroups`（layers + 按 `DrawingSymbolRenderingStrategy`(SINGLE/MULTIPLE_COLOR/MULTIPLE_OPACITY) 分组的 renderModeGroups + animationSettings）。
- 动画：`DrawingEffectStrategy`（SCALE/VARIABLE_COLOR/APPEAR/DISAPPEAR/BOUNCE/PULSE/REPLACE_APPEAR/REPLACE_DISAPPEAR/DISABLE/QUICK_REPLACE_*，TEXT_FLIP=100）+ `DrawingPiecewiseParameter`（curveType SPRING/LINEAR/FRICTION/SHARP + curveArgs + duration + delay）。
- 工具：`DrawingHMSymbol::PathOutlineDecompose`（路径分解）、`MultilayerPath`（多层展开）、`SetGetGroupParametersCallback`（静态回调 + `shared_mutex` 保护，供动画参数查询）。
- 流程：symbolGlyphId → layers 分组 → renderGroups（带 `DrawingSColor`）→ 路径展开 → 按动画策略驱动渲染。

### GetFallbackFont 的 fallback 链查找策略

- 单字符：`Font::GetFallbackFont(unicode)` 返回可绘制该 Unicode 的后备 Font。
- 系统级：`FontMgr::MatchFamilyStyleCharacter(familyName, fontStyle, bcp47[], bcp47Count, character)`，按 BCP47（ISO 639+15924+3166-1）locale 与字符匹配。
- 批量两阶段（`GetFallbacksForString`）：Phase 1 用 prior Typeface 经 `UnicharToGlyph` 探测所有码点（非零即命中）；Phase 2 剩余码点走 `MatchFamilyStyleCharacter`，每个新 Typeface 继续批量探测剩余码点；连续同 Typeface 合并为 run（`FontFallbackInfo` 含 typeface + glyphIds），未匹配输出 typeface=nullptr/glyphId=0。
- 路径回退：`Font::GetTextPathWithFallback` 对非 GLYPH_ID 编码自动 fallback；GLYPH_ID 不支持 fallback，直接走 `GetTextPath`。

### Texgine 与 2D Graphics 文本模块的分工边界

- 位置：Texgine 在 `frameworks/text/service/texgine/`；2D Graphics 文本模块在 `rosen/modules/2d_graphics/include/text/`（Font/Typeface/TextBlob 等）。
- 双引擎架构：texgine + skia_txt 共存，Texgine 自研引擎逐步替换 SkiaTxt（见 `docs/knowledge/text-framework.md`）。
- 分工：Texgine 负责复杂文本布局（段落、复杂脚本、emoji 排版），输出 Glyph/RSXform；2D Graphics 文本模块作为底座，负责字形加载、度量、TextBlob 构建与 Canvas 绘制。
- 衔接：Texgine 调用 Typeface/Font 加载字形，用 TextBlobBuilder 构建可绘制 TextBlob，再交由 Canvas 绘制。
