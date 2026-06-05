# Dirty Region（脏区域）

## 适用范围

- 脏区域计算与合并
- 虚拟扩展屏脏区域累积
- Filter（模糊）脏区域处理
- 遮挡与脏区域交互
- GPU Tile 对齐策略

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSUniDirtyComputeUtil | `rosen/modules/render_service/core/feature/dirty/rs_uni_dirty_compute_util.h` | 统一渲染脏区域计算工具 |
| RSUniDirtyOcclusionUtil | `rosen/modules/render_service/core/feature/dirty/rs_uni_dirty_occlusion_util.h` | 脏区域遮挡工具 |
| RSUniFilterDirtyComputeUtil | 同上文件（内嵌类） | Filter 脏区域计算 |

## 核心模型

### 脏区域计算

`RSUniDirtyComputeUtil` 提供静态方法，核心流程：

1. `GetCurrentFrameVisibleDirty`：获取当前帧可见脏区域，与屏幕信息取交集
2. `ScreenIntersectDirtyRects`：脏区域与屏幕区域取交集
3. `GetFilpDirtyRects` / `FilpRects`：翻转脏区域（适配旋转）
4. `ClipRegion`：将脏区域裁剪到 Canvas

关键阈值 `DIRTY_REGION_COUNT_THRESHOLD = 1`：当脏区域数量 ≤ 1 时使用裁剪矩形，超过时切换到 GPU Tile 对齐。

### 虚拟扩展屏脏区域

虚拟扩展屏场景下脏区域需要跨帧累积：

- `AccumulateVirtualExpandScreenDirtyRegions`：累积脏区域
- `MergeVirtualExpandScreenAccumulatedDirtyRegions`：合并累积脏区域
- `ClearVirtualExpandScreenAccumulatedDirtyRegions`：清除累积
- `CheckVirtualExpandScreenSkip`：判断是否可跳过虚拟屏渲染
- `DirtyStatusAutoUpdate`：RAII 封装，自动管理虚拟屏脏区域使能状态

### Filter 脏区域

`RSUniFilterDirtyComputeUtil` 处理模糊/滤镜的脏区域传播：

- `DealWithFilterDirtyRegion`：入口方法，处理 Filter 节点的脏区域
- `GenerateFilterDirtyRegionInfo`：生成 Filter 脏区域信息
- `GetVisibleFilterRect`：获取 Filter 节点可见矩形
- 支持部分渲染缓存：`FilterCachePartialRenderEnabled`
- `dirtyAlignEnabled_`：脏区域对齐开关

### 遮挡与脏区域

`RSUniDirtyOcclusionUtil` 处理遮挡对脏区域的影响：

- `SetAnimationOcclusionInfo`：设置动画遮挡信息
- `CheckResetAccumulatedOcclusionRegion`：检查并重置累积遮挡区域
- `IsParticipateInOcclusion`：判断节点是否参与遮挡计算

### GPU Tile 对齐

`damageRegionGpuTile_` 控制 GPU Tile 对齐策略。`SetDamageRegionGpuTile` 设置 tile 尺寸，`IsDamageRegionGpuTileValid` 检查有效性。当脏区域数量超过阈值时，使用 GPU Tile 对齐提升渲染效率。

### 可见等级

`GetRegionVisibleLevel` 计算区域可见等级，影响脏区域是否需要渲染。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 脏区域阈值切换策略 | `DIRTY_REGION_COUNT_THRESHOLD = 1` | 少量脏区域用矩形裁剪精确，大量脏区域用 GPU Tile 对齐避免细碎区域 |
| 虚拟屏脏区域跨帧累积 | `AccumulateVirtualExpandScreenDirtyRegions` | 虚拟扩展屏可能不在每帧都渲染，累积保证不丢脏区域 |
| Filter 脏区域独立处理 | `RSUniFilterDirtyComputeUtil` | 模糊效果会放大脏区域影响范围，需要特殊处理 |
| 遮挡信息与动画联动 | `isAnimationOcclusion_` 记录时间戳 | 动画期间遮挡关系频繁变化，需要特殊标记避免误判 |
| RAII 管理虚拟屏脏区域状态 | `DirtyStatusAutoUpdate` | 虚拟屏脏区域使能状态需要自动恢复，避免状态泄漏 |

## 待补充背景

- `Occlusion::Region` 的具体实现和合并算法
- GPU Tile 对齐在不同 GPU 架构下的 tile 尺寸选择
- `FilterDirtyRegionInfo` 的完整字段含义和生成逻辑
- 虚拟扩展屏脏区域的实际使用场景和性能影响
- `RSVisibleLevel` 的等级定义和使用方式
