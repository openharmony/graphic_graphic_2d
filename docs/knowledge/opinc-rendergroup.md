# OPINC / RenderGroup

## 适用范围

- OPINC（操作增量缓存）自动缓存
- RenderGroup 渲染组缓存

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSOpincDrawCache | `rosen/modules/render_service/core/feature/opinc/rs_opinc_draw_cache.h` | OPINC 绘制缓存 |
| RSLayerPartDrawCache | `rosen/modules/render_service/core/feature/opinc/rs_layer_part_draw_cache.h` | OPINC 层部分绘制缓存 |
| RSRenderGroupCacheDrawable | `rosen/modules/render_service/core/feature/render_group/rs_render_group_cache_drawable.h` | 渲染组缓存 Drawable |

## 核心模型

### OPINC 绘制缓存

`RSOpincDrawCache` 实现操作增量缓存，核心逻辑：

- `OpincCalculateBefore` / `OpincCalculateAfter`：计算缓存前后处理
- `BeforeDrawCache` / `AfterDrawCache`：绘制缓存前后处理
- `DrawAutoCache`：自动缓存绘制
- `nodeCacheType_`：节点缓存类型（thread_local）
- `recordState_`：记录状态（RECORD_NONE 等）
- `isDrawAreaEnable_`：绘制区域使能状态
- `opListDrawAreas_`：OpList 绘制区域
- 缓存内存管理：`AddOpincCacheMem` / `ReduceOpincCacheMem`

### RenderGroup 缓存

`RSRenderGroupCacheDrawable` 管理渲染组缓存：

- `renderGroupCachedSurface_` / `renderGroupCachedImage_`：缓存的 Surface 和 Image
- `renderGroupCacheThreadId_`：缓存所属线程 ID
- `shouldClipHole_`：是否裁剪空洞
- `drawBlurForCache_` / `drawExcludedSubTreeForCache_`：绘制时标记（thread_local）

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| OPINC thread_local 缓存类型 | `nodeCacheType_` 为 thread_local | 避免多线程间的缓存类型竞争 |
| RenderGroup Cache 线程绑定 | `renderGroupCacheThreadId_` | 缓存只能在创建线程使用，避免 GPU 资源跨线程 |

## 待补充背景

- OPINC 自动缓存命中率和内存阈值策略
- RenderGroup 缓存的触发条件和失效策略
