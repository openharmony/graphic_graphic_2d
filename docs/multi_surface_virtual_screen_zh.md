# 多 Surface 虚拟屏特性说明

## 背景

传统虚拟屏只绑定一个 producer `Surface`。RenderService 在虚拟屏渲染结束后，把整屏内容提交到这个
`Surface`，调用方再从对应 buffer 中消费画面。

本分支把虚拟屏的输出目标从单个 `Surface` 扩展为一组 `SurfaceRegionConfig`：

```cpp
struct SurfaceRegionConfig {
    sptr<Surface> surface = nullptr;
    RectI region;
};
```

其中 `surface` 是输出目标，`region` 是该 surface 对应的虚拟屏区域。`region` 为空时表示该 surface 接收
完整虚拟屏画面；`region` 非空时表示该 surface 只接收虚拟屏坐标系中的指定矩形区域。

这个设计把单 surface 虚拟屏视为多 surface 虚拟屏的特例：

- 单 surface：`GetMultiSurfaceConfigs()` 返回一个配置。
- 多 surface 同画面输出：多个配置，所有 `region` 为空。
- 多 surface 分区输出：多个配置，至少一个 `region` 非空。

## 对外能力

客户端侧新增或扩展了以下能力：

- 创建单 surface 虚拟屏。
- 创建多 surface 虚拟屏。
- 向已有虚拟屏追加 surface。
- 从已有虚拟屏移除 surface。
- 更新某个 surface 的输出区域。
- 一次性替换虚拟屏的全部 surface 配置。

主要接口位于：

- `rosen/modules/render_service_base/include/transaction/rs_render_service_client.h`
- `rosen/modules/render_service_client/core/transaction/rs_interfaces.h`
- `rosen/modules/render_service_base/include/platform/ohos/transaction/zidl/rs_iclient_to_service_connection.h`

核心接口形态如下：

```cpp
ScreenId CreateVirtualScreen(
    const std::string& name,
    uint32_t width,
    uint32_t height,
    const std::vector<SurfaceRegionConfig>& surfaceConfigs,
    ScreenId associatedScreenId = 0,
    int32_t flags = 0,
    std::vector<NodeId> whiteList = {});

int32_t AddVirtualScreenSurface(
    ScreenId id, const std::vector<SurfaceRegionConfig>& surfaceConfigs);
int32_t RemoveVirtualScreenSurface(ScreenId id, const std::vector<sptr<Surface>>& surfaces);
int32_t UpdateVirtualScreenSurfaceRegion(ScreenId id, sptr<Surface> surface, const RectI& region);
int32_t SetVirtualScreenSurfaces(ScreenId id, const std::vector<SurfaceRegionConfig>& surfaceConfigs);
```

## 数据模型

### SurfaceRegionConfig

`SurfaceRegionConfig` 定义在：

- `rosen/modules/render_service_base/include/screen_manager/rs_surface_region_config.h`

它描述一个虚拟屏输出目标：

- `surface`：目标 producer surface。
- `region`：该 surface 需要接收的虚拟屏坐标区域。

`region` 的含义需要结合渲染模式理解：

- 空区域：该 surface 接收完整虚拟屏内容。
- 非空区域：该 surface 接收虚拟屏内容中的 `[x, y, width, height]` 子区域。

### MULTI_SURFACE_CONFIGS

`RSScreenProperty` 新增 `ScreenPropertyType::MULTI_SURFACE_CONFIGS`，用于随屏幕属性在主线程和渲染线程间同步
surface 列表。它替代了原有的 `PRODUCER_SURFACE` 属性类型。

相关文件：

- `rosen/modules/render_service_base/include/screen_manager/rs_screen_property.h`
- `rosen/modules/render_service_base/src/screen_manager/rs_screen_property.cpp`
- `rosen/modules/render_service/screen_manager/rs_screen_thread_safe_property.*`

主线程收到该属性变化后会强制刷新一帧，并标记虚拟 surface 发生变化：

```cpp
if (type == ScreenPropertyType::MULTI_SURFACE_CONFIGS) {
    node->SetVirtualSurfaceChanged(true);
}
```

这样可以让 render thread 在下一帧重新基于新的 surface 配置申请 frame 和 canvas。

## 创建和管理流程

### 创建虚拟屏

`RSScreenManager::CreateVirtualScreen()` 接收 `surfaceConfigs` 后会做基础校验：

- 校验虚拟屏数量和分辨率限制。
- 过滤空 surface。
- 去重同一次创建请求中的重复 surface。
- 检查 surface 是否已经被其他 screen 使用。

通过校验后，配置会写入 `VirtualScreenConfigs::surfaceConfigs`，再创建 `RSScreen`。

### 动态追加 surface

`RSScreenManager::AddVirtualScreenSurface()` 负责跨 screen 唯一性检查，`RSScreen::AddVirtualScreenSurface()`
负责在当前 screen 内做 copy-on-write 更新：

1. 读取旧的 `MultiSurfaceConfigs`。
2. 复制一份新 vector。
3. 跳过空 surface 和当前 screen 内已存在的 surface。
4. 调用 `SetMultiSurfaceConfigs()` 写回属性。

### 动态移除 surface

`RemoveVirtualScreenSurface()` 使用 surface unique id 匹配待删除项。移除后至少要保留一个 surface，否则返回
`INVALID_ARGUMENTS`。这个约束保证虚拟屏不会变成没有输出目标的状态。

### 更新 region

`UpdateVirtualScreenSurfaceRegion()` 根据 surface unique id 找到目标配置，只修改该配置的 `region`，再写回
`MULTI_SURFACE_CONFIGS`。

### 替换全部 surface

`SetVirtualScreenSurfaces()` 用传入配置替换整个列表。它比追加接口更严格：

- 入参不能为空。
- 每个 config 的 surface 不能为空。
- 同一列表内不能有重复 surface。
- surface 不能被其他 screen 占用。

## 渲染路径

多 surface 虚拟屏的渲染核心在：

- `rosen/modules/render_service/core/pipeline/render_thread/rs_uni_render_virtual_processor.h`
- `rosen/modules/render_service/core/pipeline/render_thread/rs_uni_render_virtual_processor.cpp`
- `rosen/modules/render_service/core/drawable/rs_logical_display_render_node_drawable.cpp`

### 初始化 frame

`RSUniRenderVirtualProcessor::RequestFramesForAllSurfaces()` 会遍历
`screenProperty.GetMultiSurfaceConfigs()`，为每个有效 surface 申请 `RSRenderFrame`：

1. 清空旧的 `surfaceFrames_`。
2. 遍历 `SurfaceRegionConfig`。
3. 对每个 surface 复用或创建 `RSSurface`。
4. 调用 render engine `RequestFrame()`。
5. 保存 `frame`、`canvas` 和 `region` 到 `SurfaceFrameConfig`。
6. 第一个成功申请到 frame 的 surface 被记录为 primary surface。

primary surface 是默认绘制目标，也是完整画面复制模式下的源 surface。

### 模式判断

初始化后会根据 region 判断是否需要 offscreen 渲染：

```cpp
needsOffscreenRender_ = false;
for (const auto& frame : surfaceFrames_) {
    if (!frame.region.IsEmpty()) {
        needsOffscreenRender_ = true;
        break;
    }
}
```

因此当前有两条多 surface 路径：

- `needsOffscreenRender_ == false`：所有 surface 接收完整画面（镜像模式）。
- `needsOffscreenRender_ == true`：至少一个 surface 使用 region，进入分区输出模式（扩展模式）。

通过 `IsMultiSurfaceExtendMode()` 判断是否处于扩展模式，只有同时满足多个 surface 和
`needsOffscreenRender_` 时才返回 true。该命名明确表达其语义——仅扩展模式返回 true，镜像模式返回 false。

### 完整画面复制模式（镜像）

当所有 `region` 为空时，渲染仍然先发生在 primary surface 的 canvas 上。`PostProcess()` 进入
`FlushAllSurfaces()` 后，会在 GPU flush 前调用 `CopyToSecondarySurfaces()`：

1. 从 primary canvas 的 surface 获取 snapshot。
2. 遍历 secondary surfaces。
3. 把 snapshot 画到每个 secondary canvas。
4. 后续统一 flush 所有 surface。

这个模式适合多路 consumer 需要相同虚拟屏画面的场景。

### 分区输出模式（扩展）

当至少一个 `region` 非空时，`RSLogicalDisplayRenderNodeDrawable::DrawExpandDisplay()` 会识别
`IsMultiSurfaceExtendMode()`，并切换到 offscreen 渲染：

1. `PrepareOffscreenRender()` 创建离屏画布。
2. `RSRenderNodeDrawable::OnDraw()` 把完整虚拟屏内容画到 offscreen。
3. `GetOffscreenImage()` 获取离屏结果。
4. `RSUniRenderVirtualProcessor::BlitRegionsToSurfaces()` 按每个 config 的 `region` 截取源图区域。
5. 每个 region 被绘制到对应 surface 的完整目标区域。

这个模式适合把一个虚拟屏画面拆分输出到多个 surface 的场景。

## Vulkan 三段式 Flush

多 surface 场景下需要在一个渲染周期内管理多个 surface 的 GPU 提交和 buffer 回收。为此把
`RSSurfaceOhosVulkan` 原有的单步 `FlushFrame()` 拆为三个独立阶段：

1. `FlushGpu` — 每个 surface 独立调用。Skia surface flush 产出 VkSemaphore，中间状态（信号量、
   buffer 指针、回调上下文）存入 `FlushState`，由 `flushStateMutex_` 保护。
2. `SubmitGpu` — 所有 surface 共享同一个 `GrDirectContext`，`mSkContext->Submit()` 只需调用一次
   （在 primary frame 上调用），提交整个 context 上累积的 GPU 命令。
3. `FlushBuffer` — 每个 surface 独立调用。通过 `VkQueueSignalReleaseImageOHOS` 提交信号量，
   `NativeWindowFlushBuffer` 归还 buffer，获取 acquire fence。

`RSRenderFrame` 暴露对应的 `FlushGpu()`/`SubmitGpu()`/`FlushBuffer()` 公共接口，内部跟踪
`flushPhaseActive_`。如果三段式序列未完成就析构，走 `CancelActiveFlush()` 而非 `Flush()`，
避免重复提交。原有 `Flush()` 仍供单 surface 使用。`rs_surface.h` 基类新增三个虚函数声明，
Vulkan 后端覆写实现，其他后端保持默认空实现。

多 surface 处理器的编排顺序：

```cpp
if (!needsOffscreenRender_) { CopyToSecondarySurfaces(); }  // 镜像：先复制
FlushGpu();                         // 所有 surface flush GPU
primaryFrame->SubmitGpu();          // 一次 Submit
FlushBuffer(fences);                // 逐个 flush buffer，收集 fence
MergeMirrorFenceToHardwareEnabledDrawables(MergeAcquireFences(fences));
```

`FlushBuffer` 收集每个成功 flush 的 surface acquire fence，`MergeAcquireFences` 通过
`SyncFence::MergeFence()` 合并成一个 fence。合并后的 fence 传给 mirror 场景的 HWC drawable，
确保等待所有输出 surface 完成后才访问。

## 单 surface 兼容

单 surface 虚拟屏不会走多 surface flush 分支：

```cpp
// PostProcess
if (surfaceFrames_.size() == 1) {
    surfaceFrames_[primarySurfaceIndex_].frame->Flush();  // 传统单步 flush
    sptr<SyncFence> fence = surfaceFrames_[primarySurfaceIndex_].frame->GetAcquireFence();
    if (isMirror_) {
        MergeMirrorFenceToHardwareEnabledDrawables(fence);
    }
    return;
}
FlushAllSurfaces();  // 仅多 surface 路径
```

因此单 surface 行为保持接近原有路径，只是底层 surface 也保存在 `MULTI_SURFACE_CONFIGS` 中。

旧的 `RSVirtualScreenProcessor` 也改为从 `GetMultiSurfaceConfigs()` 读取第一个 surface，保证非 uni-render
路径仍能找到虚拟屏输出目标。

## Surface 生命周期和缓存

`RSScreenRenderNodeDrawable` 为多 surface 增加了按 producer surface unique id 缓存 `RSSurface` 的能力：

```cpp
std::map<uint64_t, std::shared_ptr<RSSurface>> virtualSurfaces_;
```

`RequestFramesForAllSurfaces()` 会优先从 drawable 中取已有 `RSSurface`，没有时才调用
`renderEngine_->MakeRSSurface()` 创建并缓存。

当 `MULTI_SURFACE_CONFIGS` 改变时，主线程标记 `SetVirtualSurfaceChanged(true)`，用于通知渲染侧 surface
配置已经变化，下一帧需要基于新的配置更新 frame。

## 截图和 HWC 节点处理

`RSLogicalDisplayRenderNodeDrawable::OnCapture()` 也改为通过 `GetMultiSurfaceConfigs()` 获取虚拟屏 buffer。
当前它优先使用第一个 surface 的 `GetLastFlushedBuffer()` 作为虚拟屏 buffer 来源。

在存在硬件合成节点时，`DrawHardwareEnabledNodes()` 会用虚拟屏 buffer 和 fence 生成 draw param，再把 HWC
节点按 z-order 绘制到截图 canvas 中。

## IPC 和序列化

为支持跨进程传递 `SurfaceRegionConfig`，本分支扩展了：

- `RSMarshallingHelper` 对 `SurfaceRegionConfig` 和 `RectI` 的 marshalling/unmarshalling。
  `SurfaceRegionConfig` 的 marshalling 委托给已有的 `sptr<Surface>` 序列化方法（使用
  `WriteRemoteObject` / `ReadRemoteObject` 提取 producer），确保与 IPC proxy/stub 的
  序列化格式一致。
- `RSClientToServiceConnectionProxy` 的新请求封装。所有动态管理接口使用 `TF_SYNC` 模式，
  确保能读到服务端返回的 `int32_t` 状态码。
- `RSClientToServiceConnectionStub` 的服务端分发。
- IPC code access verifier 的接口码白名单。

## 错误处理和约束

当前实现的主要约束如下：

- 创建或设置时，一个 surface 不能同时被多个 screen 使用。
- `SetVirtualScreenSurfaces()` 不允许空列表。
- `RemoveVirtualScreenSurface()` 不允许删除最后一个 surface。
- `AddVirtualScreenSurface()` 会跳过空 surface 和重复 surface。
- 多 surface 渲染时，如果某个 surface frame 或 canvas 申请失败，会跳过该 surface。
- 分区输出模式下，region 应由调用方保证有效；无效 region 可能导致该 surface 输出异常或为空。

## 关键文件索引

- API：`rosen/modules/render_service_base/include/transaction/rs_render_service_client.h`
- Client facade：`rosen/modules/render_service_client/core/transaction/rs_interfaces.*`
- IPC interface：`rosen/modules/render_service_base/include/platform/ohos/transaction/zidl/rs_iclient_to_service_connection.h`
- IPC proxy：`rosen/modules/render_service_base/src/platform/ohos/transaction/zidl/rs_client_to_service_connection_proxy.cpp`
- IPC stub：`rosen/modules/render_service/main/render_server/transaction/zidl/rs_client_to_service_connection_stub.cpp`
- 数据结构：`rosen/modules/render_service_base/include/screen_manager/rs_surface_region_config.h`
- 屏幕属性：`rosen/modules/render_service_base/include/screen_manager/rs_screen_property.h`
- Screen 管理：`rosen/modules/render_service/screen_manager/rs_screen_manager.cpp`
- Screen 状态：`rosen/modules/render_service/screen_manager/rs_screen.cpp`
- UniRender 虚拟屏处理器：`rosen/modules/render_service/core/pipeline/render_thread/rs_uni_render_virtual_processor.cpp`
- 逻辑屏绘制：`rosen/modules/render_service/core/drawable/rs_logical_display_render_node_drawable.cpp`
- Vulkan Surface（三段式 flush）：`rosen/modules/render_service_base/src/platform/ohos/backend/rs_surface_ohos_vulkan.cpp`
- RenderFrame（三段式接口）：`rosen/modules/render_service/composer/composer_service/external_depend/engine/rs_base_render_engine.h`

## 后续建议

建议后续补齐以下验证：

- 单 surface 创建、渲染、截图回归。
- 多 surface 全画面复制，验证所有 consumer 收到同一帧。
- 多 surface region 输出，验证每个 surface 的裁剪区域和缩放结果。
- 动态 add/remove/update/set 后下一帧是否正确刷新。
- mirror 场景下 HWC drawable 是否等待所有 surface 的合并 fence。
- IPC 返回值路径是否全部使用同步请求，避免服务端写 reply 但客户端读不到结果。
