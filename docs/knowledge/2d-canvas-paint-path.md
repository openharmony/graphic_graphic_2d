# 2D Canvas / Paint / Path

## 适用范围

- 使用 Canvas、Paint、Brush、Pen、Path 进行 2D 绘制
- 自定义绘制控件的图形录制与回放
- OverDraw 检测、NoDraw 场景、StateRecord 画布
- SVG 路径解析与构建
- 2D Graphics C++ API 及 NDK 接口开发与调试

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| canvas.h | `rosen/modules/2d_graphics/include/draw/canvas.h` | Canvas 基类及 OverDrawCanvas、NoDrawCanvas、StateRecordCanvas 派生 |
| core_canvas.h | `rosen/modules/2d_graphics/src/drawing/draw/core_canvas.h` | 核心绘制接口：DrawRect/DrawPath/DrawImage 等 |
| paint.h | `rosen/modules/2d_graphics/include/draw/paint.h` | Paint 统一绘制描述（组合 Brush+Pen） |
| brush.h | `rosen/modules/2d_graphics/include/draw/brush.h` | 填充画刷：颜色、ShaderEffect、Filter、Blender |
| pen.h | `rosen/modules/2d_graphics/include/draw/pen.h` | 描边画笔：JoinStyle/CapStyle、宽度、PathEffect |
| path.h | `rosen/modules/2d_graphics/include/draw/path.h` | 路径构建：MoveTo/LineTo/CubicTo/QuadTo/ConicTo/BuildFromSVGString |
| color.h | `rosen/modules/2d_graphics/include/draw/color.h` | Color / Color4f / ColorQuad 颜色类型 |
| surface.h | `rosen/modules/2d_graphics/include/draw/surface.h` | 绘制目标 Surface |
| shadow.h | `rosen/modules/2d_graphics/include/draw/shadow.h` | 阴影绘制参数 |

## 核心模型

Canvas 体系采用继承分层：

```
CoreCanvas (定义 DrawPoint/DrawRect/DrawPath/Save/Restore 等纯绘制接口)
  └─ Canvas (增加 recordingState/offscreen/UICapture/stencil 状态)
       ├─ OverDrawCanvas (DrawingType::OVER_DRAW, GPU 过度绘制检测)
       ├─ NoDrawCanvas (DrawingType::NO_DRAW, 不实际绘制的占位画布)
       ├─ StateRecordCanvas (DrawingType::STATE_RECORD, 仅记录状态)
       └─ RecordingCanvas (Recording 专用, 在 recording/ 头文件中定义)
```

Paint 统一描述绘制样式，内部组合 Brush（填充）和 Pen（描边）：
- `PaintStyle`：`PAINT_NONE` / `PAINT_FILL` / `PAINT_STROKE` / `PAINT_FILL_STROKE`
- 通过 `AttachBrush` / `AttachPen` 分别设置填充和描边参数
- 支持 HDR UIColor（`hdrColor_` + `isHdrColor_`）
- 支持抗锯齿（`antiAlias_`）、BlendMode、ShaderEffect、PathEffect、Blender、BlurDrawLooper

Path 表示矢量路径，核心数据由 `PathImpl`（Skia 适配层）持有：
- 基础动词：MoveTo、LineTo、QuadTo、CubicTo、ConicTo、Close
- 相对动词：RMoveTo、RLineTo、RQuadTo、RCubicTo、RConicTo
- 几何构建：AddRect、AddOval、AddArc、AddCircle、AddRoundRect、AddPoly、AddPath
- 路径运算：Op（DIFFERENCE/INTERSECT/UNION/XOR）、Transform、Offset
- SVG 互操作：`BuildFromSVGString` / `ConvertToSVGString`
- 路径测量：`GetLength`、`GetPositionAndTangent`、`GetSegment`
- 序列化/反序列化：`Serialize` / `Deserialize`

数据流：用户通过 Canvas 调用绘制接口 → Paint 提供样式 → Path/Rect 等提供几何 → 底层通过 `*Impl` 适配到 Skia GPU 或 CPU 后端。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| Paint 合并 Brush+Pen | `Paint` 内含 `PaintStyle` 枚举控制填充/描边组合 | 简化上层调用，一次设置同时控制填充和描边 |
| Canvas 继承分层 | `OverDrawCanvas`/`NoDrawCanvas`/`StateRecordCanvas` 派生 | 不同场景复用 CoreCanvas 接口，避免条件分支 |
| Path 使用 Impl 模式 | `std::shared_ptr<PathImpl> impl_` 隔离 Skia 依赖 | 解耦平台适配层，支持未来非 Skia 后端 |
| HDR UIColor 支持 | `Paint::hdrColor_` + `isHdrColor_` 标志 | HDR 场景需要超过 0-1 范围的亮度表示 |
| SVG 路径互操作 | `BuildFromSVGString` / `ConvertToSVGString` | 支持 ACE 框架 SVG 路径描述到 Path 的转换 |
| AutoCanvasRestore | RAII 类在析构时调用 `RestoreToCount` | 防止 Save/Restore 不匹配导致状态泄漏 |

## 待补充背景

- CoreCanvas 与 RecordingCanvas 的完整录制回放链路说明
- Paint::CanCombinePaint 的合并条件与场景
- HDR UIColor 在渲染管线中的完整处理路径（从 Paint 到 GPU 输出）
- PathImpl 在非 Skia 后端（如 DDGR）下的适配策略
- AutoCanvasMatrixBrush 的使用场景与生命周期约束
