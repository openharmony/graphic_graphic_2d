# 截图功能

## 适用范围

- Surface 截图（单个 Surface 节点）
- Display 截图（整个显示节点）
- 统一渲染模式截图
- 并行截图
- HDR 截图（FP16 格式）
- 截图回调与 PixelMap 输出
- 控件截图（单个 UI 节点，ROOT/CANVAS/CANVAS_DRAWING/SURFACE 类型）
- 控件范围截图（beginNode → endNode 区间截取）
- 分离渲染客户端本地截图（RSDividedUICapture，无跨进程 IPC）
- Solo 节点截图（仅截取节点自身，不含子节点，RSUiCaptureSoloTaskParallel）

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSSurfaceCaptureTask | `rosen/modules/render_service/core/feature/capture/rs_surface_capture_task.h` | Surface 截图任务 |
| RSSurfaceCaptureVisitor | 同上文件（内嵌类） | Surface 截图遍历器 |
| RSUniUICapture | `rosen/modules/render_service/core/feature/capture/rs_uni_ui_capture.h` | 统一渲染 UI 截图 |
| RSUniUICaptureVisitor | 同上文件（内嵌类） | 统一渲染截图遍历器 |
| RSSurfaceCaptureTaskParallel | `rosen/modules/render_service/core/feature/capture/rs_surface_capture_task_parallel.h` | 并行 Surface 截图 |
| RSUiCaptureTaskParallel | `rosen/modules/render_service/core/feature/capture/rs_ui_capture_task_parallel.h` | 并行 UI 截图 |
| RSUiCaptureSoloTaskParallel | `rosen/modules/render_service/core/feature/capture/rs_ui_capture_solo_task_parallel.h` | Solo 并行 UI 截图 |
| RSDividedUICapture + RSDividedUICaptureVisitor | `rosen/modules/render_service_client/core/feature/ui_capture/rs_divided_ui_capture.h` | 分离渲染客户端本地控件截图 |
| RSUiCaptureTaskParallel | `rosen/modules/render_service/core/feature/capture/rs_ui_capture_task_parallel.h` | 统一渲染并行控件截图 |

## 核心模型

### RSSurfaceCaptureTask

`RSSurfaceCaptureTask` 封装单次截图任务：

1. `Run(callback)`：执行截图，返回 PixelMap 给回调
2. `CreatePixelMapBySurfaceNode`：按 SurfaceNode 创建 PixelMap
3. `CreatePixelMapByDisplayNode`：按 DisplayNode 创建 PixelMap
4. `CreateSurface`：为 PixelMap 创建 Drawing Surface

### RSSurfaceCaptureVisitor

继承 `RSNodeVisitor`，遍历节点树执行截图渲染：

- 非统一渲染模式：`ProcessSurfaceRenderNodeWithoutUni` → `CaptureSingleSurfaceNodeWithoutUni` / `CaptureSurfaceInDisplayWithoutUni`
- 统一渲染模式：通过 `ProcessSurfaceRenderNode` 直接处理
- 支持 `isDisplayNode_` 标志区分 Display 截图和 Surface 截图

### RSUniUICapture

统一渲染模式下的截图，内部类 `RSUniUICaptureVisitor` 继承 `RSNodeVisitor`：

- `TakeLocalCapture`：执行本地截图
- 支持 `ExtendRecordingCanvas` 录制画布
- `ProcessSurfaceRenderNodeWithUni`：统一渲染 Surface 处理
- `ProcessSurfaceViewWithUni` / `ProcessSurfaceViewWithoutUni`：SurfaceView 处理
- `isUseCpuSurface_`：CPU Surface 模式标志

### RSDividedUICapture

分离渲染模式下客户端本地控件截图，在 `RSRenderThread` 执行，无跨进程 IPC：

1. `TakeLocalCapture()`：校验 scale → 获取节点 → 不在树上时 ApplyModifiers → CreatePixelMapByNode → CreateSurface → RSPaintFilterCanvas → Visitor 遍历节点树 → 渲染到 PixelMap → CopyDataToPixelMap（Ashmem/heap） → 返回 PixelMap
2. `RSDividedUICaptureVisitor` 继承 `RSNodeVisitor`：
   - `ProcessCanvasRenderNode` / `ProcessRootRenderNode`：设置 relativeMatrix（scale + specifiedAreaRect 偏移 + boundsGeometry 逆矩阵），ApplyAlphaAndBoundsGeometry → ProcessRenderContents → ProcessChildren → ProcessRenderAfterChildren
   - `ProcessSurfaceRenderNode`：通过 RSUIContext 递归调用 `TakeSurfaceCapture`（跨进程获取子 SurfaceNode 的 PixelMap），同步等待最多 MAX_WAIT_TIME(2000ms)，DrawImage 绘制到 canvas
   - `ProcessEffectRenderNode`：ProcessRenderBeforeChildren → ProcessChildren → ProcessRenderAfterChildren
3. `CreateSurface`：GPU 模式 MakeRenderTarget（OpenGL/Vulkan），CPU 模式 MakeRasterDirect
4. 支持 `specifiedAreaRect` 指定区域裁剪

### RSUiCaptureTaskParallel

统一渲染模式下服务端并行控件截图，在 `RSUniRenderThread` 执行：

1. `Capture()` 静态方法入口：创建实例 → UpdateStartAndEndNodeRect → CreateResources → PostTask 到 RSUniRenderThread
2. `CreateResources()`：根据节点类型生成 RSRenderNodeDrawable（SurfaceNode 检查 UIFirst 缓存 / LeashWindow 可复用），创建 PixelMap（支持 HDR RGBA_F16），检查 GPU buffer 尺寸上限
3. `Run()`：CreateSurface → RSPaintFilterCanvas（scale/matrix/HDR/UICapture 标志） → nodeDrawable_->OnCapture(canvas) → CopyDataToPixelMap 或 DMA CreateSurfaceSyncCopyTask → ProcessUiCaptureCallback
4. `CreateSurface`：OpenGL MakeRenderTarget / Vulkan MakeRenderTarget / CPU MakeRasterDirect；HDR 使用 COLORTYPE_RGBA_F16 + 对应 ColorSpace
5. `ProcessUiCaptureCallback`：callback->OnSurfaceCapture → captureCount_-- → RequestNextVSync

### RSUiCaptureSoloTaskParallel

统一渲染模式下 Solo 节点截图，仅截取节点自身绘制内容，不遍历子节点，在 `RSUniRenderThread` 执行：

1. `CaptureSoloNode(id, captureConfig)`：收集根节点所有子节点 ID（CollectAllChildren） → 对每个子节点调用 `CaptureSoloNodePixelMap` → 返回 `vector<pair<NodeId, PixelMap>>`
2. `CaptureSoloNodePixelMap(id, captureConfig)`：创建实例 → CreateResources → PostSyncTask 到 RSUniRenderThread 执行 Run → 返回 PixelMap
3. `CreateResources()`：根据节点类型（SurfaceNode/CanvasNode/EffectNode/CanvasDrawingNode/RootNode）生成 RSRenderNodeDrawable，创建 RGBA_8888 PixelMap，检查 Vulkan GPU buffer 尺寸上限
4. `Run()`：CreateSurface → RSPaintFilterCanvas（scale + relativeMatrix + boundsGeometry 逆矩阵） → nodeDrawable_->OnCapture(canvas)（CaptureParam 中 isSoloNode=true） → DMA CreateSurfaceSyncCopyTask（PostSyncTask 同步等待）或 CopyDataToPixelMap
5. 仅支持统一渲染模式，分离渲染不可用
6. 权限要求：selfCapture || isSystemCalling

### UICaptureInRange 范围截图

- 入口参数：`RSUICaptureInRangeParam{ endNodeId, useBeginNodeSize }`
- `UpdateStartAndEndNodeRect()`：校验 endNode 是 startNode 子树节点 → 计算 startRect/endRect/startMatrix/endMatrix
- 范围截图渲染（`HasEndNodeRect() && !isStartEndNodeSame_`）：
  1. offScreenSurface 渲染 startNode（nodeDrawable_->OnCapture）
  2. offScreenCardSurface 渲染 endNode，effectData 矩阵变换（startMatrix * endMatrixInvert）
  3. GetImageSnapshot → DrawImage 合成到主 canvas
- useBeginNodeSize=true 时直接使用 startNode 尺寸，不做范围裁剪

### UICapture 数据流

- **统一渲染并行路径**：RSInterfaces → RSRenderInterface（设 captureType=UICAPTURE） → RSRenderPipelineClient → IPC(TAKE_SURFACE_CAPTURE) → RSRenderPipelineAgent（权限检查 selfCapture || isSystemCalling） → TakeSurfaceCaptureForUiParallel → RSMainThread::AddUiCaptureTask → RSUiCaptureTaskParallel::Capture → ProcessUiCaptureCallback → IPC 回调 → SurfaceCaptureCallback::OnSurfaceCapture
- **分离渲染路径**：RSInterfaces → RSRenderInterface → TakeSurfaceCaptureForUIWithoutUni → RSRenderThread::PostTask → RSDividedUICapture::TakeLocalCapture → SurfaceCaptureCallback::OnSurfaceCapture（客户端本地，无 IPC）
- **UICaptureInRange**：统一渲染 → RSRenderPipelineClient::TakeUICaptureInRange → IPC(TAKE_UI_CAPTURE_IN_RANGE) → RSRenderPipelineAgent::TakeUICaptureInRange（权限：仅 isSystemCalling） → TakeSurfaceCaptureForUiParallel；分离渲染 → TakeSurfaceCaptureForUIWithoutUni
- **Solo 节点截图**：RSInterfaces::TakeSurfaceCaptureSoloNodeList → RSRenderInterface（设 isSoloNodeUiCapture=true，仅统一渲染） → RSRenderPipelineClient::TakeSurfaceCaptureSoloNode → IPC → RSRenderPipelineAgent::TakeSurfaceCaptureSoloNode（权限：selfCapture || isSystemCalling） → RSUiCaptureSoloTaskParallel::CaptureSoloNode → PostMainThreadSyncTask → CaptureSoloNodePixelMap → PostSyncTask → Run

### 权限模型

| 入口 | 权限要求 | 说明 |
| --- | --- | --- |
| TakeSurfaceCaptureForUI | selfCapture || isSystemCalling | 自截取或系统调用均可 |
| TakeUICaptureInRange | 仅 isSystemCalling | 范围截图涉及跨节点可见性，更严格 |
| TakeSurfaceCaptureSoloNode | selfCapture || isSystemCalling | Solo 截图与普通控件截图权限一致 |

### 截图配置

`RSSurfaceCaptureConfig` 封装截图配置参数。

### 数据流

截图请求（IPC）→ RSMainThread → 创建 RSSurfaceCaptureTask → RSSurfaceCaptureVisitor 遍历节点树 → 渲染到 PixelMap 对应的 Surface → CopyDataToPixelMap → 回调返回 PixelMap

### 并行截图

`RSSurfaceCaptureTaskParallel` / `RSUiCaptureTaskParallel` / `RSUiCaptureSoloTaskParallel` 提供并行截图能力，多节点同时截图提升效率。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 截图通过 Visitor 模式 | `RSSurfaceCaptureVisitor` / `RSUniUICaptureVisitor` | 截图需要遍历节点树，Visitor 模式解构遍历和渲染逻辑 |
| 统一/非统一渲染截图分离 | `ProcessSurfaceRenderNodeWithoutUni` / `ProcessSurfaceRenderNodeWithUni` | 两种渲染模式下 buffer 获取方式不同，分离处理避免逻辑混乱 |
| 截图使用独立 Canvas | `CreateSurface` 创建独立 Drawing Surface | 截图不能影响正常渲染的 Canvas 状态 |
| CPU Surface 模式 | `isUseCpuSurface_` | 部分 GPU 不可用场景需要 CPU 渲染截图 |
| 并行截图 | `*TaskParallel` 系列 | 多窗口截图场景并行化提升性能 |
| 分离渲染客户端本地截图 | RSDividedUICapture 在 RSRenderThread 执行，无 IPC | 分离渲染下节点在客户端，无需跨进程 |
| UICapture 权限分离 | selfCapture vs isSystemCalling | 控件截图可见性比 Surface 截图更敏感 |
| 统一渲染用 Drawable 而非 Visitor | nodeDrawable_->OnCapture(canvas) | Drawable 已持有渲染参数和缓存，直接复用避免重建 |
| UICaptureInRange 矩阵合成 | offScreen + startMatrix/endMatrix | 范围截图需在两个节点间做矩阵变换，保证空间一致性 |
| DMA 异步拷贝 | CreateSurfaceSyncCopyTask → RSBackgroundThread | GPU 渲染后 DMA 拷贝耗时长，异步化避免阻塞渲染线程 |
| Solo 节点不含子节点 | RSUiCaptureSoloTaskParallel::CaptureSoloNodePixelMap + CaptureParam(isSoloNode=true) | 部分场景只需节点自身绘制内容，避免冗余渲染子树 |
| Solo 截图使用同步 PostSyncTask | RSUniRenderThread::PostSyncTask / RSMainThread::PostMainThreadSyncTask | Solo 截图需同步返回 PixelMap（而非异步回调），调用方直接获取结果 |

## 待补充背景

- `RSSurfaceCaptureConfig` 的完整字段定义
- HDR 截图的 FP16 处理流程
- 截图在安全层场景下的处理策略
- 并行截图的线程调度和资源管理
- 截图与 RenderNode 树的同步机制（防止截图时树结构变化）
- UICaptureInRange 的节点区间裁剪算法细节
- DMA/非 DMA PixelMap 模式切换策略和条件
