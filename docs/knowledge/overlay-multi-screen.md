# Overlay Display / Multi Screen

## 适用范围

- Overlay 显示模式（叠加以太等后处理效果）
- 多屏渲染路径（镜像/扩展/有线镜像）
- 镜像显示绘制策略
- 虚拟扩展屏渲染

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSOverlayDisplayManager | `rosen/modules/render_service/core/feature/overlay_display/rs_overlay_display_manager.h` | Overlay 显示管理器 |
| RSMultiScreenUtil | `rosen/modules/render_service/core/feature/multi_screen/rs_multi_screen_util.h` | 多屏渲染工具 |

## 核心模型

### Overlay Display

`RSOverlayDisplayManager` 为单例，通过动态加载 so 实现叠加以太等后处理效果：

- `SetOverlayDisplayMode`：设置 Overlay 显示模式
- `PostProcFilter`：渲染后处理滤镜
- `ExpandDirtyRegion`：扩展脏区域（后处理可能扩大影响范围）
- `PreProcForRender`：渲染前预处理，判断模式是否变化
- `IsOverlayDisplayEnableForCurrentVsync`：当前 VSync 是否启用 Overlay

动态加载机制：`LoadLibrary` / `CloseLibrary` / `LoadSymbol` 加载外部 so，通过函数指针 `OverlayPostProcFunc` 和 `OverlayExpandDirtyRegionFunc` 调用后处理逻辑。

### Multi Screen

`RSMultiScreenUtil` 提供静态方法，处理多屏渲染路径选择：

关键枚举 `RenderStrategy`：

- `DRAW_AS_MAIN_SCREEN`：按主屏绘制
- `DRAW_VIRTUAL_EXTEND`：虚拟扩展显示
- `DRAW_VIRTUAL_MIRROR_FROM_CACHE`：虚拟镜像从缓存复制
- `DRAW_VIRTUAL_MIRROR_REBUILD`：虚拟镜像重建
- `DRAW_PHYSICAL_MIRROR_FROM_CACHE`：物理镜像从缓存复制
- `DRAW_PHYSICAL_MIRROR_REBUILD`：物理镜像重建

核心方法：

- `HandleMirrorDisplay`：处理镜像显示（逻辑显示节点），按 `CompositeType` 分发到物理或虚拟镜像路径
- `HandleMirrorScreen`：处理镜像屏幕（屏幕节点）
- `HandleVirtualExtendDisplay`：处理虚拟扩展显示
- `HandleVirtualExtendScreen`：处理虚拟扩展屏幕
- `DumpRenderStrategy`：DFX 打印绘制策略变更

内部分发：

- `DrawPhysicalMirrorDisplay`：物理镜像显示入口，根据 `isRedraw` 条件分发：
  - `isRedraw`（debug 模式、cacheImage 为空、或有线屏 onDraw 且有特殊层）→ `DrawPhysicalMirrorRebuild`
  - 否则 → `DrawPhysicalMirrorFromCache`
- `DrawVirtualMirrorDisplay`：虚拟镜像显示入口，根据 `rebuildReasonFlag` 位集分发：
  - 任一标志置位（特殊层、HDR、cacheImage 为空、静屏、屏幕关闭、色彩过滤）→ `DrawVirtualMirrorRebuild`
  - 否则 → `DrawVirtualMirrorFromCache`
- `DrawVirtualExtend`：虚拟扩展绘制

`MultiScreenParams` 结构体持有屏幕和逻辑显示的 Drawable/Params 对，用于多屏渲染参数传递。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| Overlay 通过动态 so 加载 | `LoadLibrary` / `soHandle_` | Overlay 后处理逻辑闭源或产品差异化，动态加载解耦编译依赖 |
| 多屏绘制路径枚举化 | `RenderStrategy` 枚举 | 多屏场景绘制方式多样，枚举清晰表达路径，便于 DFX 和调试 |
| 镜像区分 FromCache 和 Rebuild | `DRAW_PHYSICAL_MIRROR_FROM_CACHE` / `REBUILD`、`DRAW_VIRTUAL_MIRROR_FROM_CACHE` / `REBUILD` | FromCache 直接复制缓存像素，Rebuild 重新渲染整帧，根据场景选择性能最优路径 |
| 物理镜像入口统一分发 | `DrawPhysicalMirrorDisplay` → `DrawPhysicalMirrorFromCache` / `DrawPhysicalMirrorRebuild` | 入口统一判断 isRedraw 条件后分发，避免外部多处重复判断 |
| 虚拟镜像入口位集分发 | `DrawVirtualMirrorDisplay` → `rebuildReasonFlag` → `FromCache` / `Rebuild` | 用 bitset 累积多个 rebuild 原因，任一标志触发即走 Rebuild，逻辑清晰可扩展 |
| Overlay 脏区域扩展 | `ExpandDirtyRegion` | 后处理效果可能影响原始脏区域外的像素，需要扩展脏区域保证显示正确 |

## 待补充背景

- Overlay so 的具体接口定义和加载时机
- 虚拟扩展屏的完整渲染流程
- Overlay 显示模式的模式值定义
