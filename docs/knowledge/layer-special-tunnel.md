# Layer / Tunnel Layer

特殊层（安全图层/跳过图层/截屏跳过图层/黑白名单）内容见 `special-layer.md`。

## 适用范围

- 合成 Layer 管理
- Tunnel Layer 直投通道
- Layer 缓存管理
- Tunnel 路由仲裁

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSLayerCacheManager | `rosen/modules/render_service/core/feature/layer/rs_layer_cache_manager.h` | Layer 缓存管理 |
| RSTunnelLayerManager | `rosen/modules/render_service/core/feature/tunnel_layer/rs_tunnel_layer_manager.h` | Tunnel Layer 管理器 |
| RSTunnelLayerHelper | `rosen/modules/render_service/core/feature/tunnel_layer/rs_tunnel_layer_helper.h` | Tunnel Layer 辅助 |
| RSTunnelRouteArbiter | `rosen/modules/render_service/core/feature/tunnel_layer/rs_tunnel_route_arbiter.h` | Tunnel 路由仲裁器 |
| RSTunnelLayerStateHandler | `rosen/modules/render_service/core/feature/tunnel_layer/rs_tunnel_layer_state_handler.h` | Tunnel Layer 状态处理 |
| RSSurfaceLayer | `rosen/modules/render_service/composer/composer_client/layer/rs_surface_layer.h` | 合成客户端 Layer |
| RSSurfaceRCDLayer | `rosen/modules/render_service/composer/composer_client/layer/rs_surface_rcd_layer.h` | RCD Layer |

## 核心模型

### Tunnel Layer

Tunnel Layer 提供直投通道，绕过 RS 渲染管线直接将 buffer 提交到 HWC。

**RSTunnelLayerManager**：管理 Tunnel Layer 生命周期：

- `UpdateTunnelLayerState`：更新 Tunnel Layer 状态
- `HandleLayerStateChanged`：处理 Layer 状态变化回调
- `ClearRuntimeStateByPid`：进程退出时清理运行时状态

**RSTunnelLayerHelper**：提供静态辅助方法：

- `ResolveTunnelLayerInfo`：从 ConsumerSurface 解析 tunnelLayerId 和 property
- `AcquirePendingBuffer`：获取 pending buffer
- `TryCommitBufferDirect`：尝试直接提交 buffer
- `BeginTunnelBuilding`：开始建立 Tunnel
- `HandleListenerBuffer`：处理 Listener buffer 回调，返回 `ListenerHandleResult`

**RSTunnelRouteArbiter**：路由仲裁器，决定走 Tunnel 直投还是正常渲染：

- `ArbitrateAndClaim`：仲裁并声明路由（NOT_TUNNEL_ACTIVE / GO_NORMAL / KEEP_DIRECT）
- `AbandonNormalClaim`：放弃正常路由声明
- `OnRenderCommitDone`：渲染提交完成通知
- `globalRouteForcedNormal_`：全局强制走正常路由标志
- `ComputeGlobalForbiddenCause`：计算全局禁止直投原因

### Layer 缓存

`RSLayerCacheManager` 管理 Layer 的缓存，减少 Layer 创建/销毁开销。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| Tunnel 路由仲裁器独立 | `RSTunnelRouteArbiter` | Tunnel/正常路由切换逻辑复杂，独立封装便于测试和维护 |
| 全局强制正常路由 | `globalRouteForcedNormal_` atomic | 特定场景（如截图、安全要求）需要全局禁用直投，atomic 保证线程安全 |
| Tunnel buffer 提交失败处理 | `HandleTunnelCommitFailure`（在 Composer） | 直投提交可能失败（如 HWC 不支持），需要回退到正常渲染 |

## 待补充背景

- `TunnelLayerProperty` 的完整标志位定义
- Tunnel Layer 与 LPP 的交互流程
- `LayerStateChange` 的完整状态枚举和转换图
- Layer 缓存的具体策略和淘汰机制
