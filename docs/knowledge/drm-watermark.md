# DRM / Watermark

## 适用范围

- DRM（Digital Rights Management）受保护内容处理
- 安全层与模糊的交互
- Surface 水印设置与绘制
- 水印在虚拟屏/镜像屏的行为
- 受保护 buffer 预分配

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSDrmUtil | `rosen/modules/render_service/core/feature/drm/rs_drm_util.h` | DRM 工具类 |
| RSSurfaceWatermarkHelper | `rosen/modules/render_service/core/feature/watermark/rs_surface_watermark.h` | 水印辅助类 |

## 核心模型

### DRM

`RSDrmUtil` 提供静态方法处理 DRM 受保护内容：

1. **DRM 节点收集**：`CollectDrmNodes` 收集 DRM SurfaceNode，存入 `drmNodes_`（`map<NodeId, vector<shared_ptr<RSSurfaceRenderNode>>>`，按一级节点 ID 索引）
2. **DRM 节点处理**：`DealWithDRMNodes` 处理 DRM 节点，含受保护 buffer 预分配 `PreAllocProtectedFrameBuffers`
3. **克隆跨节点**：`AddDrmCloneCrossNode` 处理 DRM 克隆跨节点
4. **DRM Layer 创建**：`DRMCreateLayer` 在 Processor 上创建 DRM Layer
5. **模糊与 DRM 交互**：`MarkBlurIntersectWithDRM` 标记与 DRM 相交的模糊节点
6. **DRM 树判断**：`IsDRMNodesOnTheTree` 检查渲染树中是否有 DRM 节点
7. **虚拟屏 DRM**：`HasDRMInVirtualScreen` 检查虚拟屏是否有 DRM 内容

内部方法：

- `MarkAllBlurIntersectWithDRM`：标记所有与 DRM 相交的模糊
- `IsDRMBelowFilter`：判断 DRM 是否在 Filter 下方
- `GetDarkColorMode`：获取暗色模式
- `PreAllocProtectedFrameBuffers`：预分配受保护帧 buffer

### Watermark

`RSSurfaceWatermarkHelper` 管理 Surface 水印：

1. **设置水印**：`SetSurfaceWatermark(pid, name, pixelMap, nodeIdList, watermarkType, ...)` 设置水印，支持普通水印和网格水印，可指定行列数
2. **绘制水印**：`DrawCommSurfaceWatermark` 绘制公共 Surface 水印（静态方法）
3. **清除水印**：`ClearSurfaceWatermark` / `ClearSurfaceWatermarkForNodes` 按条件清除
4. **权限检查**：`CheckClearWatermarkPermission` 检查清除权限（仅设置者或系统可清除）
5. **网格水印效果**：`GetGridWatermarkEffect` 获取网格水印 RuntimeEffect

关键数据结构：

- `RSSurfaceWatermarkInfo`：水印信息（Image、pid、rowCount、colCount）
- `RSSurfaceWatermarkSpecialParam`：特殊参数（maxWidth、maxHeight、isWatermarkChange）
- `watermarkNameMapNodeId_`：`map<string, pair<set<NodeId>, SurfaceWatermarkType>>` 水印名到节点映射
- `surfaceWatermarks_`：`map<string, RSSurfaceWatermarkInfo>` 所有水印

水印类型 `SurfaceWatermarkType` 包含 `INVALID_WATER_MARK` 等。

进程退出时通过 `isDeathMonitor` 参数快速清除该进程的所有水印。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| DRM 节点按一级节点索引 | `drmNodes_` map 结构 | DRM 内容通常以应用窗口为单位，按一级节点索引快速定位 |
| DRM 与模糊交互标记 | `MarkBlurIntersectWithDRM` | DRM 内容上方的模糊需要特殊处理（不能用 GPU 模糊），标记保证正确回退 |
| 受保护 buffer 预分配 | `PreAllocProtectedFrameBuffers` | 受保护 buffer 分配需要特殊标志，提前分配避免渲染时阻塞 |
| 水印按名称管理 | `watermarkNameMapNodeId_` | 同一名称的水印可能应用于多个节点，按名称管理便于批量操作 |
| 水印清除权限检查 | `CheckClearWatermarkPermission` | 防止非设置者清除水印，保护水印不被恶意移除 |
| 网格水印 RuntimeEffect | `GetGridWatermarkEffect` | 网格水印使用 GPU shader 绘制，性能优于 CPU 绘制 |

## 待补充背景

- DRM 受保护 buffer 的分配标志和 HWC 处理方式
- `SurfaceWatermarkType` 的完整枚举定义
- 水印在虚拟屏和镜像屏场景下的完整处理流程
- DRM 与 HDR 的交互
- 网格水印 shader 的具体实现
