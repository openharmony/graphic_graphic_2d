# Drawing NDK

## 适用范围

- C 语言 NDK 接口开发（`OH_Drawing_*` 系列）
- Canvas、Paint、Path、Bitmap、Image 等 2D 绘制的 C API
- 字体与文本绘制 NDK 接口
- GPU Context 与 Surface NDK 接口
- 滤镜、着色器、颜色空间 NDK 接口
- RecordCmd 录制 NDK 接口
- 跨语言绑定（NAPI/CJ/ANI/Taihe）的基础层

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| drawing_canvas.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_canvas.h` | OH_Drawing_Canvas C API |
| drawing_path.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_path.h` | OH_Drawing_Path C API |
| drawing_bitmap.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_bitmap.h` | OH_Drawing_Bitmap C API |
| drawing_pen.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_pen.h` | OH_Drawing_Pen C API |
| drawing_brush.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_brush.h` | OH_Drawing_Brush C API |
| drawing_font.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_font.h` | OH_Drawing_Font C API |
| drawing_typeface.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_typeface.h` | OH_Drawing_Typeface C API |
| drawing_text_blob.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_text_blob.h` | OH_Drawing_TextBlob C API |
| drawing_image.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_image.h` | OH_Drawing_Image C API |
| drawing_surface.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_surface.h` | OH_Drawing_Surface C API |
| drawing_gpu_context.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_gpu_context.h` | OH_Drawing_GPUContext C API |
| drawing_shader_effect.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_shader_effect.h` | OH_Drawing_ShaderEffect C API |
| drawing_color_filter.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_color_filter.h` | OH_Drawing_ColorFilter C API |
| drawing_image_filter.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_image_filter.h` | OH_Drawing_ImageFilter C API |
| drawing_filter.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_filter.h` | OH_Drawing_Filter C API |
| drawing_mask_filter.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_mask_filter.h` | OH_Drawing_MaskFilter C API |
| drawing_path_effect.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_path_effect.h` | OH_Drawing_PathEffect C API |
| drawing_matrix.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_matrix.h` | OH_Drawing_Matrix C API |
| drawing_rect.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_rect.h` | OH_Drawing_Rect C API |
| drawing_round_rect.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_round_rect.h` | OH_Drawing_RoundRect C API |
| drawing_color.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_color.h` | OH_Drawing_Color C API |
| drawing_color_space.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_color_space.h` | OH_Drawing_ColorSpace C API |
| drawing_pixel_map.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_pixel_map.h` | OH_Drawing_PixelMap C API |
| drawing_record_cmd.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_record_cmd.h` | OH_Drawing_RecordCmd C API |
| drawing_types.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_types.h` | 基础类型定义 |
| drawing_error_code.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_error_code.h` | 错误码定义 |
| drawing_sampling_options.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_sampling_options.h` | 采样选项 |
| drawing_region.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_region.h` | 区域裁剪 |
| drawing_lattice.h | `rosen/modules/2d_graphics/drawing_ndk/include/drawing_lattice.h` | 九宫格绘制 |
| array_mgr.h | `rosen/modules/2d_graphics/drawing_ndk/include/array_mgr.h` | 数组管理 |
| drawingndk_blocklist.txt | `rosen/modules/2d_graphics/drawing_ndk/drawingndk_blocklist.txt` | NDK 符号导出白名单 |

## 核心模型

Drawing NDK 采用 **C 接口 + 不透明指针** 模式：

```
OH_Drawing_Canvas*  ← 实际指向 Drawing::Canvas
OH_Drawing_Paint*   ← 实际指向 Drawing::Paint (或 Pen/Brush)
OH_Drawing_Path*    ← 实际指向 Drawing::Path
OH_Drawing_Bitmap*  ← 实际指向 Drawing::Bitmap
OH_Drawing_Font*    ← 实际指向 Drawing::Font
...
```

命名规范：
- 类型：`OH_Drawing_<Type>`（如 `OH_Drawing_Canvas`）
- 函数：`OH_Drawing_<Type>_<Action>`（如 `OH_Drawing_Canvas_DrawRect`）
- 错误码：`OH_DRAWING_SUCCESS` / `OH_DRAWING_ERROR_*`

典型使用流程：
1. `OH_Drawing_BitmapCreate` + `OH_Drawing_BitmapBuild` 创建位图
2. `OH_Drawing_CanvasCreate` + `OH_Drawing_CanvasBindBitmap` 绑定画布
3. `OH_Drawing_PenCreate` / `OH_Drawing_BrushCreate` 创建画笔/画刷
4. `OH_Drawing_CanvasAttachPen` / `AttachBrush` 设置绘制样式
5. `OH_Drawing_Path*` 构建路径
6. `OH_Drawing_CanvasDrawPath` 绘制
7. 释放所有对象

## 对外行为和 ABI 边界

Drawing NDK 是公开 C ABI，修改时优先保持以下不变量：

- 公开类型仍是 `OH_Drawing_*` 不透明指针，不能暴露 C++ 类型或 STL 类型。
- 新函数命名必须保持 `OH_Drawing_<Type>_<Action>`。
- Create/Destroy、Copy/Destroy、CreateFrom*/Destroy 的所有权要在头文件注释中写清。
- 输入指针允许为空还是必须非空，需要和返回值或错误码一致。
- enum、struct 字段、错误码数值、函数签名属于兼容面，改动前要人工确认。
- 新导出符号要检查 `drawingndk_blocklist.txt` 和构建出的 `libnative_drawing.so`。

常见实现模式：

```text
OH_Drawing_* public C API
  -> CastTo*() / Helper::CastTo()
  -> Drawing::* C++ object
  -> 参数校验、执行、返回值或 g_drawingErrorCode
```

新增 API 时，要同步检查：

- `rosen/modules/2d_graphics/drawing_ndk/include/` 的头文件声明和 since/syscap 注释。
- `rosen/modules/2d_graphics/drawing_ndk/src/` 的实现。
- `rosen/modules/2d_graphics/drawing_ndk/BUILD.gn` 是否包含新增源文件。
- `rosen/test/2d_graphics/unittest/ndk/` 是否有单测。
- `rosen/test/2d_graphics/fuzztest/ndk/` 是否需要新增或扩展 fuzzer。

## 错误码模型

错误码定义在 `drawing_error_code.h`，当前包括：

- `OH_DRAWING_SUCCESS`
- `OH_DRAWING_ERROR_NO_PERMISSION`
- `OH_DRAWING_ERROR_INVALID_PARAMETER`
- `OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE`
- `OH_DRAWING_ERROR_ALLOCATION_FAILED`
- `OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH`
- `OH_DRAWING_ERROR_INCORRECT_PARAMETER`

实现里存在两类错误返回模式：

1. **直接返回 `OH_Drawing_ErrorCode`**：
   新接口优先使用这种方式，调用方不需要再读全局错误码。
2. **设置全局 `g_drawingErrorCode`**：
   旧接口常见于返回 `void`、对象指针、`bool` 或数值的函数。
   调用方通过 `OH_Drawing_ErrorCodeGet()` 读取，必要时调用 `OH_Drawing_ErrorCodeReset()`。

新增或修改错误码时要遵守：

- 空指针一般返回或设置 `OH_DRAWING_ERROR_INVALID_PARAMETER` 或既有同类接口使用的错误码。
- 数值超范围用 `OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE`。
- 分配失败用 `OH_DRAWING_ERROR_ALLOCATION_FAILED`，不能静默返回空对象不置错。
- 同一头文件中的同类 API 错误码风格要一致；不要把新接口随意换成另一种模式。
- 旧接口错误码语义属于应用可见行为，修改前需要兼容确认和 XTS 预期。

## 输入校验重点

NDK 所有参数都来自应用进程，按不可信输入处理：

- 指针：校验对象指针、输出指针、数组指针和嵌套对象指针。
- 数组：校验 `count`、`offset`、`byteLength`，避免乘法溢出和越界写。
- 浮点：处理 NaN、Inf、负尺寸、极大矩阵和不可逆矩阵。
- 文本：校验编码、字节长度、glyph 数量和 fallback font 场景。
- PixelMap：确认 Media PixelMap 互操作的 ownership、格式、stride、row bytes 和空对象。
- GPUContext/Surface：确认 GPU 开关、上下文生命周期、Surface 绑定和释放顺序。
- RecordCmd：确认录制命令的 replay 对象仍然有效。

Destroy 函数要么允许空指针并直接返回，要么按既有同类接口设置错误码。
同一对象重复 Destroy 不应被文档描述为合法用法，
测试重点是避免正常路径泄漏和空指针崩溃。

## fuzz / 单测 / XTS

### 单测

最近单测位置：

```text
rosen/test/2d_graphics/unittest/ndk/
```

新增 API 至少覆盖：

- 成功路径。
- 空指针输入。
- 输出指针为空。
- 边界值、越界值、NaN/Inf。
- Create/Destroy 成对释放。
- 错误码 Get/Reset 或直接返回码。

### Fuzz

NDK fuzz 位置：

```text
rosen/test/2d_graphics/fuzztest/ndk/
```

已有示例包括 GPUContext 和 Canvas Utils fuzzer。新增解析、数组、文本、PixelMap、GPU 或 RecordCmd
入口时，优先补 fuzzer；只改普通 setter 且已有同类 fuzzer 覆盖时，可扩展现有 fuzzer。

### XTS

涉及公开行为时需要同步确认 XTS：

- 新增公开 API、错误码、enum、struct 字段、syscap 或 since 标签。
- 修改空指针、越界、异常输入的返回语义。
- 修改绘制结果、文本测量、PixelMap 互操作或 GPU surface 行为。
- 修改 `libnative_drawing.so` 导出符号。

本仓通常无法单独完成 XTS；PR 中要写清目标用例、预期行为和未执行原因。

## 修改检查清单

- 头文件、实现、BUILD、导出符号、单测是否同步。
- 错误码风格是否与同类接口一致。
- 所有应用输入是否校验空指针、长度、范围和浮点异常。
- Create/Destroy ownership 是否清楚，失败路径是否泄漏。
- 是否需要扩展 `rosen/test/2d_graphics/fuzztest/ndk/`。
- 是否需要 XTS 或跨语言绑定同步。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| C 接口 + 不透明指针 | 所有 `OH_Drawing_*` 类型均为前向声明指针 | ABI 稳定，隐藏 C++ 实现细节 |
| Create/Destroy 配对 | `OH_Drawing_CanvasCreate` / `OH_Drawing_CanvasDestroy` | 明确生命周期，支持 RAII 之外的语言绑定 |
| 符号导出控制 | `drawingndk_blocklist.txt` 白名单 | 限制导出符号，防止内部 API 泄露 |
| 错误码返回 | `OH_Drawing_ErrorCode` 枚举 | C 接口无异常，通过返回码报告错误 |
| 按类型拆分头文件 | 34 个 `drawing_*.h` 文件 | 按需包含，减少编译依赖 |
| syscap 标注 | 头文件中 `@syscap SystemCapability.Graphic.Graphic2D.NativeDrawing` | 支持产品裁剪按系统能力选择 |

## 待补充背景

- NDK 与上层 NAPI/CJ/ANI/Taihe 绑定的映射关系
- `drawing_utils/` 目录中的辅助工具
- GPUContext NDK 接口在 GPU 渲染场景下的完整用法
- RecordCmd NDK 接口的录制回放完整流程
- PixelMap NDK 接口与 Media PixelMap 的互操作
