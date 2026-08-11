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

## 录制回放链路（以 DrawPath 为例）

RecordingCanvas 继承 NoDrawCanvas，自身不绘制，只把调用记录到 DrawCmdList。
DrawCmdList 有两种 UnmarshalMode，由 RecordingCanvas 构造参数 `addDrawOpImmediate` 决定：

- IMMEDIATE（默认）：构造时即把 ConstructorHandle 写入连续内存 `opAllocator_`，适合录制即传输。
- DEFERRED：先存对象 vector `drawOpItems_`，需要时再 Marshalling 到连续内存，便于录制后再修改（如 TextBlob→Image 缓存替换）。

录制阶段（入口 `RecordingCanvas::DrawPath`，recording_canvas.cpp:170）：

- IMMEDIATE：`CmdListHelper::AddPathToCmdList`（cmd_list_helper.cpp:590）把 `path.Serialize()` 字节流写入 cmdList imageAllocator，返回 `OpDataHandle{offset, size}`；再 `AddDrawOpImmediate<DrawPathOpItem::ConstructorHandle>`（recording_canvas.cpp:804）按 brush/pen 有效性决定生成几个 op（可合并时经 `Paint::CanCombinePaint` 临时改 `PAINT_FILL_STROKE` 出一个 op，否则 brush、pen 各一个），`GenerateHandleFromPaint` 把 ShaderEffect/ColorFilter/PathEffect/MaskFilter/Blender 拆解写入 imageAllocator，最终 `cmdList_->AddDrawOp<ConstructorHandle>` 在 `opAllocator_` 中构造 POD 风格 ConstructorHandle 并更新 `lastOpItemOffset_` 串联链表；`opCount` 上界 40000 防止过多 op。
- DEFERRED：`AddDrawOpDeferred<DrawPathOpItem>(path)`（recording_canvas.cpp:839）按合并策略 `make_shared<DrawPathOpItem>(path, paint)` 后 `drawOpItems_.emplace_back`；后续可 `MarshallingDrawOps`（draw_cmd_list.cpp:197）遍历 vector 调 `DrawPathOpItem::Marshalling`（draw_cmd.cpp:811），再次 `AddPathToCmdList` + `GenerateHandleFromPaint`，经 `cmdList.AddOp<ConstructorHandle>` 写入连续内存，与 IMMEDIATE 形态一致。

反序列化阶段（`DrawCmdList::UnmarshallingDrawOps`，draw_cmd_list.cpp:280）：

- 用 `UnmarshallingPlayer` 按 `nextOpItemOffset` 走链表，读 OpItem 头部 type。
- `UnmarshallingHelper::Instance().GetFuncAndSize(type)`（draw_cmd.cpp:394）查 LUT 取反序列化函数和期望 size；LUT 由各 OpItem 文件顶部 `UNMARSHALLING_REGISTER(DrawPath, DrawOpItem::PATH_OPITEM, DrawPathOpItem::Unmarshalling, sizeof(...ConstructorHandle))` 宏注册（draw_cmd.cpp:797）。
- 命中后 `make_shared<DrawPathOpItem>(cmdList, handle)`：父类 `DrawWithPaintOpItem` 从 Handle 还原 Paint（含 ShaderEffect 等），`path_ = GetPathFromCmdList(cmdList, handle->path)` 从 imageData 反序列化 Path；结果 push 进 `drawOpItems_` vector。

回放阶段（`DrawCmdList::Playback(canvas, rect)`，draw_cmd_list.cpp:363）：

- 若目标 canvas 是 RecordingCanvas（DrawingType::RECORDING），走 `PlaybackToDrawCmdList`（draw_cmd_list.cpp:560），把当前 op vector 和连续内存整体拷到目标 cmdList，实现嵌套录制。
- 否则按 mode 分发：IMMEDIATE 走 `PlaybackByBuffer`（先 `UnmarshallingDrawOpsSimple` 把 buffer 还原成 vector 再遍历），DEFERRED 走 `PlaybackByVector` 直接遍历。
- 对每个 op 调 `op->Playback(&canvas, rect)`，对 DrawPath 即 `DrawPathOpItem::Playback`（draw_cmd.cpp:822-830）：`canvas->AttachPaint(paint_)` 应用 Paint（修改 `CoreCanvas::paintBrush_`/`paintPen_`），`canvas->DrawPath(*path_)` 走 CoreCanvas → Skia 适配层真正绘制；Playback 不调用 `DetachPaint`，pen/brush 状态残留到下一 op 或外层 `AutoCanvasMatrixBrush` 析构时统一还原。

关键设计点：

- ConstructorHandle 是 POD，便于连续内存布局和跨进程序列化；OpItem 是带虚函数的对象，运行时使用。
- Paint 内的 Effect 对象统一拆解到 imageAllocator，PaintHandle 只持偏移引用，避免对象指针跨进程。
- brush+pen 满足 `CanCombinePaint` 时合并为单 op（PAINT_FILL_STROKE），减少 op 数量。
- UnmarshallingHelper 用注册宏代替大 switch-case，便于新增 OpItem 类型。
- 链表式 `nextOpItemOffset` 让反序列化按需遍历连续内存，无需额外索引。

## Paint::CanCombinePaint 合并条件与场景

静态方法 `Paint::CanCombinePaint(const Paint& pen, const Paint& brush)`（paint.cpp:112，
paint.h:35）用于判断同一图元的填充 brush 与描边 pen 是否可合并为一次绘制调用。

合并条件（全部相等）：

| 字段 | 说明 |
| --- | --- |
| `antiAlias_` | 抗锯齿开关 |
| `isHdrColor_` + `hdrColor_`/`color_` | HDR 路径比 `hdrColor_`，SDR 路径比 `color_` |
| `blendMode_` | 混合模式 |
| `hasFilter_` + `filter_` | ColorFilter 是否一致 |
| `colorSpace_` | 颜色空间（指针 `Equals` 比较） |
| `shaderEffect_` | 着色器 |
| `blender_` + `blenderEnabled_` | Blender |
| `blurDrawLooper_` | 模糊阴影绘制层 |

调用场景：

- CoreCanvas 通过宏 `DRAW_API_WITH_PAINT`（core_canvas.cpp:31）和带 looper 的
  `DRAW_API_WITH_PAINT_LOOPER`（core_canvas.cpp:54）统一分发；命中合并时把 pen 的 style
  临时改为 `PAINT_FILL_STROKE`，单次 `impl_->func` 调用同时绘制填充与描边，未命中则 brush、
  pen 分两次 `impl_->func` 调用，两者均无效时用 `defaultPaint_`。
- RecordingCanvas 同样模式（recording_canvas.cpp:819/848/870），命中生成单个
  `PAINT_FILL_STROKE` op，否则生成 brush、pen 两个 op。
- 覆盖 API：`DrawPath`/`DrawRect`/`DrawRoundRect`/`DrawCircle`/`DrawOval`/`DrawArc`/
  `DrawLine`/`DrawPoints`/`DrawBitmap`/`DrawImageRect`/`DrawTextBlob`/`DrawSymbol` 等
  （core_canvas.cpp:227-652，共 27 处）。

关键约束：

- 仅合并"绘制属性"完全相同的 fill+stroke；`style`、stroke 宽度/端点/连接等 stroke 专属属性
  不在比较范围内（这些本就属于 pen，brush 无对应字段，不冲突）。
- 函数签名参数顺序为 `(pen, brush)`，但调用处实参为 `(paintBrush_, paintPen_)`
  （core_canvas.cpp:39、recording_canvas.cpp:819），即第 1 参传 brush、第 2 参传 pen；
  函数内部按字段比较与参数语义无关，不影响结果。

## HDR UIColor 在渲染管线中的完整处理路径

UIColor 类型（`ui_color.h:33`）：在 Color 之外新增 `headroom_`（HDR 亮度比，默认 1.0）字段，
RGB+Alpha 均为 float，可表达超过 [0,1] 范围的亮度。`operator==` 在比较时把 RGB 乘以 headroom
（`ui_color.cpp:158-165`），所以 `(R,G,B,headroom)` 与 `(R*H,G*H,B*H,1.0)` 视为相等。

完整处理路径：

| 阶段 | 文件:行 | 行为 |
| --- | --- | --- |
| 1. 上游 RSColor 携带 headroom | `render_service_base/src/common/rs_color.cpp:128-217`、`include/common/rs_color.h:129` | `RSColor` 持有 headroom，颜色加减乘除按 headroom 比例换算 |
| 2. 转换为 Drawing::UIColor | `render_service_base/src/drawable/rs_property_drawable_background.cpp:299-302` | gamut 是 BT2020/DCIP3 且 `bgColor.GetHeadroom() > 1.0` 时构造 `UIColor(R,G,B,A,headroom)`，否则走 SDR `SetColor` 分支 |
| 3. 写入 Brush/Pen/Paint | `brush.cpp:121-126`、`pen.cpp:68-70`、`paint.cpp:212` | `SetUIColor` 置 `hdrColor_` + `isHdrColor_=true`；`AttachBrush`/`AttachPen` 在 `paint.cpp:129-131/155-157` 同步到 Paint |
| 4. 录制跨进程序列化 | `recording_handle.h:110-195`、`draw_cmd.cpp:297-303` | `PaintHandle` 用 `union{Color; UIColor}` 复用内存，`SetIsUIColor` 把 HDR 标志编码进 `blenderEnabled` 高位 0x80；`GenerateHandleFromPaint` 拷贝 `uiColor` 并 `SetIsUIColor(true)` |
| 5. 反序列化还原 | `draw_cmd.cpp:225-227` | `GetPaintFromHandle` 命中 `IsUIColor()` 调 `paint.SetUIColor(uiColor, colorSpace)`，colorSpace 从 `colorSpaceHandle` 反序列化 |
| 6. Skia 适配层落地 | `skia_adapter/skia_paint.cpp:84-95`（Brush）/`130-141`（Pen）/`186-197`（Paint） | `HasUIColor()` 命中时 `color.SetRgbF(R,G,B,A)`，再 `paint.setColor(SkColor4f::FromColor(...), colorSpace.get())`；colorSpace 携带 BT2020/DCIP3 色域给 Skia |
| 7. HDR headroom 回调注册 | `render_service/core/pipeline/render_thread/rs_uni_render_thread.cpp:204-205`、`render_service/core/feature/hdr/rs_hdr_util.cpp:473-508` | 渲染线程启动时 `UIColor::RegisterHdrCallbackFunc(&RSHdrUtil::HDRColorHeadroomMapping)`；回调内调 `Media::VideoProcessingEngine::AihdrEnhancer::ProcessHDRColor`，按 `expectedHeadroom`（UIColor 携带）与 `actualHeadroom`（`DisplayNits/SdrDisplayNits`）做非线性映射 |

关键约束与缺口：

- 第 6 步只把 RGB+A 写入 SkPaint，**headroom 本身不传给 Skia**；headroom 的预期消费者是第 7 步
  注册的 `HdrRegisterCallback`。
- 本仓内 `UIColor::GetHdrRegisterCallback()` **没有任何调用方**（仅 `ui_color.cpp:74` 自身实现
  和 `ui_color.h:42` 声明），回调注册但消费侧缺失，可能位于其它仓或当前为未启用状态。
- `CanCombinePaint` 比较 `isHdrColor_` 与 `hdrColor_` 全等（`paint.cpp:114-115`），HDR 与 SDR
  不可合并。
- SDR 截图场景下 `RSEffectLuminanceManager::GetCurrentScreenshotType` 为 SDR_* 时，
  `HDRColorHeadroomMapping` 内 `displayHeadroom` 保持 1.0，避免 HDR 增强破坏截屏色彩
  （`rs_hdr_util.cpp:484-491`）。
- `RSColor` 颜色运算（rs_color.cpp:128-217）按 headroom 比例缩放后再加/减，保证不同 headroom
  颜色运算后高光不丢失。

## AutoCanvasMatrixBrush 的使用场景与生命周期约束

RAII 守卫类，声明在 `canvas.h:36-52`，实现在 `canvas.cpp:23-49`；在栈上构造时
快照当前 pen/brush 并 Save/SaveLayer，析构时还原 pen/brush 并 RestoreToCount，
用于在子录制回放期间临时叠加 matrix 与 brush 而不污染外层 canvas 状态。

### 唯一调用场景

`Canvas::DrawRecordCmd`（canvas.cpp:80-93）是本仓内唯一构造点：

```cpp
AutoCanvasMatrixBrush autoCanvasMatrixBrush(this, matrix, brush, recordCmd->GetCullRect());
recordCmd->Playback(this);
```

- 入口先做归一：`matrix->IsIdentity()` 时把 `matrix` 置 nullptr（canvas.cpp:87-89），
  避免对单位矩阵多余 SaveLayer。
- 构造后立刻 `recordCmd->Playback(this)` 回放子录制，对象出栈即清理。
- `RecordingCanvas::DrawRecordCmd`（recording_canvas.cpp:393）只把
  `DrawRecordCmdOpItem` 加入自身 cmdList，不走本守卫；嵌套录制由
  `DrawRecordCmdOpItem::Playback`（draw_cmd.cpp:1615-1619）转发到 `canvas->DrawRecordCmd`
  触发本守卫，所以"录制态 canvas"与"回放态 canvas"是分离的两条路径。

### 构造逻辑（canvas.cpp:23-42）

成员初始化顺序固定，`saveCount_` 与 pen/brush 快照在函数体之前完成：

| 步骤 | 条件 | 动作 | 对应代码 |
| --- | --- | --- | --- |
| 1 | 无条件 | `saveCount_ = canvas->Save()`；`paintPen_`/`paintBrush_` 复制当前 canvas pen/brush | canvas.cpp:25-26 |
| 2a | `brush != nullptr` | 若 `matrix != nullptr` 用 `matrix->MapRect` 把 `bounds` 变换到新 `newBounds`；`SaveLayer(newBounds, brush)` | canvas.cpp:28-34 |
| 2b | `brush == nullptr && matrix != nullptr` | 再 `canvas->Save()` 占一个槽位，与 2a 互补以平衡 Restore | canvas.cpp:35-37 |
| 3 | `matrix != nullptr` | `canvas->ConcatMatrix(*matrix)` 叠加变换 | canvas.cpp:39-41 |

要点：

- 步骤 1 的 `Save()` 与析构的 `RestoreToCount(saveCount_)` 配对，是所有分支的共同锚点。
- `brush != nullptr` 走 `SaveLayer`（额外分配离层 surface，开销大），`brush == nullptr`
  但有 matrix 走普通 `Save`，两者 Restore 数量不同但都由 `saveCount_` 兜底回收。
- `matrix->MapRect(bounds, bounds)` 会就地改写 `newBounds`，源 `bounds` 通过
  `Rect newBounds = bounds` 拷贝隔离，不会影响调用方传入的 `recordCmd->GetCullRect()`。
- 步骤 3 的 `ConcatMatrix` 作用于已 SaveLayer 的栈帧，回放期间绘制的图元都受此矩阵影响。

### 析构逻辑（canvas.cpp:44-49）

```cpp
canvas_->GetMutablePen() = paintPen_;
canvas_->GetMutableBrush() = paintBrush_;
canvas_->RestoreToCount(saveCount_);
```

- 先恢复 pen/brush 快照，再 `RestoreToCount`：避免在 Restore 过程中触发回放
  （如离层 surface 合成）时使用被回放过程污染的 pen/brush。
- `GetMutablePen()/GetMutableBrush()` 返回 `CoreCanvas::paintPen_`/`paintBrush_`
  成员引用（core_canvas.h:797-805），直接赋值绕过 `AttachPen/AttachBrush`
  的副作用，因此不会重置 `paintPen_.SetStyle(PAINT_STROKE)` 之类状态。

### 生命周期约束

- 不可拷贝、不可移动（canvas.h:42-45），仅能栈上构造，生命周期与所在作用域绑定。
- 必须保证构造与析构在同一 canvas 上、同一线程内配对；析构前 canvas 指针需保持有效
  （成员仅 `Canvas* canvas_` 裸指针，无 ref 计数）。
- `saveCount_` 来自构造时的 `canvas->Save()` 返回值；析构 `RestoreToCount(saveCount_)`
  会把栈回退到构造前，期间任何外部代码对该 canvas 的额外 Save/Restore 都必须自行平衡，
  否则会被本守卫一并回退掉。
- 守卫只快照与还原 pen/brush 两个 Paint 成员（恰好覆盖 `DrawPathOpItem::Playback` 中
  `AttachPaint` 改写的 `CoreCanvas::paintBrush_`/`paintPen_`，见 draw_cmd.cpp:828），
  其它栈式状态（matrix、clip、SaveLayer）依赖 `RestoreToCount(saveCount_)` 整体回滚；
  Playback 内若引入非栈式 canvas 成员修改（如直接改 `stencilVal_`），本守卫不会兜底。
- `bounds` 仅在 `brush != nullptr` 时用于 SaveLayer 的裁剪矩形；`brush == nullptr`
  分支忽略 `bounds`，矩阵叠加不受 `bounds` 约束。
- 嵌套录制场景（`DrawRecordCmdOpItem::Playback` → `canvas->DrawRecordCmd`）下
  守卫可重入，每层独立持有自己的 `saveCount_`，栈式 Save/Restore 保证可嵌套。

