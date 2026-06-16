# 截图功能

## 适用范围

- Surface 截图（单个 Surface 节点）
- Display 截图（整个显示节点）
- 统一渲染模式截图
- 并行截图
- HDR 截图（FP16 格式）
- 截图回调与 PixelMap 输出

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSSurfaceCaptureTask | `rosen/modules/render_service/core/feature/capture/rs_surface_capture_task.h` | Surface 截图任务 |
| RSSurfaceCaptureVisitor | 同上文件（内嵌类） | Surface 截图遍历器 |
| RSUniUICapture | `rosen/modules/render_service/core/feature/capture/rs_uni_ui_capture.h` | 统一渲染 UI 截图 |
| RSUniUICaptureVisitor | 同上文件（内嵌类） | 统一渲染截图遍历器 |
| RSSurfaceCaptureTaskParallel | `rosen/modules/render_service/core/feature/capture/rs_surface_capture_task_parallel.h` | 并行 Surface 截图 |
| RSUiCaptureTaskParallel | `rosen/modules/render_service/core/feature/capture/rs_ui_capture_task_parallel.h` | 并行 UI 截图 |
| RSUiCaptureSoloTaskParallel | `rosen/modules/render_service/core/feature/capture/rs_ui_capture_task_parallel.h` | Solo 并行 UI 截图 |

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

### 截图配置

`RSSurfaceCaptureConfig` 封装截图配置参数。

### 数据流

截图请求（IPC）→ RSMainThread → 创建 RSSurfaceCaptureTask → RSSurfaceCaptureVisitor 遍历节点树 → 渲染到 PixelMap 对应的 Surface → CopyDataToPixelMap → 回调返回 PixelMap

### 并行截图

`RSSurfaceCaptureTaskParallel` / `RSUiCaptureTaskParallel` 提供并行截图能力，多节点同时截图提升效率。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 截图通过 Visitor 模式 | `RSSurfaceCaptureVisitor` / `RSUniUICaptureVisitor` | 截图需要遍历节点树，Visitor 模式解构遍历和渲染逻辑 |
| 统一/非统一渲染截图分离 | `ProcessSurfaceRenderNodeWithoutUni` / `ProcessSurfaceRenderNodeWithUni` | 两种渲染模式下 buffer 获取方式不同，分离处理避免逻辑混乱 |
| 截图使用独立 Canvas | `CreateSurface` 创建独立 Drawing Surface | 截图不能影响正常渲染的 Canvas 状态 |
| CPU Surface 模式 | `isUseCpuSurface_` | 部分 GPU 不可用场景需要 CPU 渲染截图 |
| 并行截图 | `*TaskParallel` 系列 | 多窗口截图场景并行化提升性能 |

## 待补充背景

- `RSSurfaceCaptureConfig` 的完整字段定义
- HDR 截图的 FP16 处理流程
- 截图在安全层场景下的处理策略
- 并行截图的线程调度和资源管理
- 截图与 RenderNode 树的同步机制（防止截图时树结构变化）
