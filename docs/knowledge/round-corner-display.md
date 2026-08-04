# Round Corner Display（圆角显示，RCD）

## 适用范围

- 圆角屏幕上下区域渲染（Top/Bottom Surface）
- 刘海/Notch 状态与圆角资源选择
- 屏幕旋转方向与圆角资源切换
- 硬件合成（HardwareComposer）圆角 Layer Buffer 预备
- RCD 脏区域标记与处理
- RCD 独立消息总线通信
- RCD XML 配置解析与 LCD 型号匹配
- RCD Surface 渲染节点生命周期管理

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RoundCornerDisplayManager | `rosen/modules/render_service/core/feature/round_corner_display/rs_round_corner_display_manager.h` | RCD 全局管理器（单例），管理 RCD 模块与 Layer 映射 |
| RoundCornerDisplay | `rosen/modules/render_service/core/feature/round_corner_display/rs_round_corner_display.h` | RCD 核心类，管理资源选择、脏区域和硬件信息 |
| RoundCornerConfig (RCDConfig) | `rosen/modules/render_service/core/feature/round_corner_display/rs_round_corner_config.h` | XML 配置解析：LCDModel / ROG / SurfaceConfig / HardwareComposerConfig |
| RSRcdSurfaceRenderNode | `rosen/modules/render_service/core/feature/round_corner_display/rs_rcd_surface_render_node.h` | RCD Surface 渲染节点，继承 RSRenderNode + RSSurfaceHandler |
| RSRcdRenderVisitor | `rosen/modules/render_service/core/feature/round_corner_display/rs_rcd_render_visitor.h` | RCD 渲染访问器，处理 Buffer 获取与硬件资源写入 |
| RSRcdRenderManager | `rosen/modules/render_service/core/feature/round_corner_display/rs_rcd_render_manager.h` | RCD 渲染管理器（单例），管理 Top/Bottom 渲染节点与渲染任务调度 |
| RSRcdRenderListener | `rosen/modules/render_service/core/feature/round_corner_display/rs_rcd_render_listener.h` | RCD Buffer 消费监听器，OnBufferAvailable 增加 Buffer 计数 |
| RsMessageBus | `rosen/modules/render_service/core/feature/round_corner_display/rs_message_bus.h` | RCD 专用消息总线，基于 Topic + 类型擦除的分发机制 |
| RsAny | `rosen/modules/render_service/core/feature/round_corner_display/rs_any.h` | 类型擦除容器，支撑 RsMessageBus 的泛型消息存储与调用 |

## 核心模型

### RoundCornerDisplayManager

全局管理器，通过 `RSSingleton` 使用，管理多个屏幕的 RCD 模块：

1. **RCD 模块生命周期**：
   - `AddRoundCornerDisplay(id)`：为 DisplayNode 创建 `RoundCornerDisplay` 实例并调用 `InitOnce()`
   - `RemoveRCDResource(id)`：同时清除 rcdMap 和 rcdlayerMap 中对应数据
2. **Layer 映射**：
   - `AddLayer(name, id, type)`：以 Surface 名称注册 Layer（TOP/BOTTOM），供 硬件合成模块 合成识别
   - `CheckLayerIsRCD(name)`：判断某 Layer 是否属于 RCD
   - `GetLayerPair(name)`：获取 Layer 对应的 NodeId 和 RCDLayerType
3. **参数更新**（委托给 `RoundCornerDisplay`）：
   - `SendRcdMessage(id, screenProperty)`：从 ScreenProperty 提取 left/top/width/height 和 Notch 状态，调用 `UpdateDisplayParameter` 和 `UpdateNotchStatus`
   - `UpdateOrientationStatus(id, orientation)`：旋转方向变化
   - `UpdateHardwareResourcePrepared(id, prepared)`：硬件资源预备完成
4. **脏区域**：`HandleRoundCornerDirtyRect(id, dirtyRect, type)` 委托给对应 `RoundCornerDisplay`
5. **硬件任务**：`RunHardwareTask(id, task)` 和 `GetHardwareInfo(id, preparing)` 委托给 `RoundCornerDisplay`
6. **使能判断**：
   - `CheckRcdRenderEnable(screenProperty)`：仅 `ScreenState::HDI_OUTPUT_ENABLE` 时启用
   - `GetRcdEnable()`：检查系统属性 `RSScreenRoundCornerEnable` 和配置文件中 LCDModel 的 top/bottom surface support
   - 配置未加载时默认支持（`!rcdCfg.IsDataLoaded()`）

Layer 类型枚举 `RCDLayerType`：INVALID / TOP / BOTTOM。

### RoundCornerDisplay

每个屏幕对应一个 RCD 核心实例：

1. **初始化**：
   - `InitOnce()` → `Init()`：加载配置 → 选择 LCD 型号 → 按分辨率加载图片资源
   - `LoadConfigFile()`：解析 `/sys_prod/etc/display/RoundCornerDisplay/config.xml`
   - `SeletedLcdModel("default")`：从 `RCDConfig` 获取 LCDModel，确定 topSurface/bottomSurface/hardwareComposer 是否支持
   - `LoadImgsbyResolution(w, h)`：从 LCDModel 中按分辨率获取 ROGSetting，加载 Portrait/Landscape 图片
2. **资源选择**：
   - `RcdChooseTopResourceType()`：根据 `curOrientation_` 和 `notchStatus_` 选择顶部资源类型：
     - ROTATION_0/180 + NOTCH_DEFAULT → TOP_PORTRAIT
     - ROTATION_0/180 + NOTCH_HIDDEN → TOP_HIDDEN
     - ROTATION_90/270 + NOTCH_DEFAULT → TOP_PORTRAIT
     - ROTATION_90/270 + NOTCH_HIDDEN → TOP_LADS_ORIT
   - `RcdChooseRSResource()`：将 `curTop_`/`curBottom_` 指向对应 `Drawing::Image`
   - `RcdChooseHardwareResource()`：将 `hardInfo_.topLayer`/`bottomLayer` 指向 `RoundCornerLayer` + Bitmap
3. **参数更新与脏标记**：
   - `UpdateDisplayParameter(left, top, width, height)`：分辨率变化时重新加载图片，设置 `RCD_DIRTY_ALL`
   - `UpdateNotchStatus(status)`：刘海状态变化时设置 `RCD_DIRTY_TOP`
   - `UpdateOrientationStatus(orientation)`：旋转变化时设置 `RCD_DIRTY_TOP`
   - `UpdateHardwareResourcePrepared(prepared)`：硬件资源预备完成后更新 `resourceChanged`/`resourcePreparing`
4. **脏区域处理**：
   - `HandleTopRcdDirty(dirtyRect)`：若 `RCD_DIRTY_TOP` 标记存在，将 `displayRect_` 顶部区域与 dirtyRect 合并
   - `HandleBottomRcdDirty(dirtyRect)`：若 `RCD_DIRTY_BOTTOM` 标记存在，将 `displayRect_` 底部区域与 dirtyRect 合并
   - 资源未变化时（`!hardInfo_.resourceChanged`）清除对应脏标记位
5. **硬件信息**：`RoundCornerHardware` 结构体包含 `resourceChanged`/`resourcePreparing`/`displayRect`/`topLayer`/`bottomLayer`
6. **并发保护**：`resourceMut_`（shared_mutex），读操作用 `shared_lock`，写操作用 `unique_lock`

`RoundCornerDirtyType` 使用位掩码：RCD_DIRTY_NONE(0x00) / RCD_DIRTY_TOP(0x01) / RCD_DIRTY_BOTTOM(0x02) / RCD_DIRTY_ALL(0x03)。

`WindowNotchStatus`：WINDOW_NOTCH_DEFAULT(0) / WINDOW_NOTCH_HIDDEN(1)。

`ShowTopResourceType`：TOP_PORTRAIT / TOP_LADS_ORIT / TOP_HIDDEN。

### RCDConfig（配置系统）

XML 配置解析系统，配置路径 `/sys_prod/etc/display/RoundCornerDisplay/config.xml`：

1. **结构层次**：
   - `RCDConfig` → `LCDModel` → `SurfaceConfig` + `SideRegionConfig` + `HardwareComposerConfig` + `ROGSetting[]`
   - `ROGSetting` → `RogPortrait`(layerUp/layerDown/layerHide) + `RogLandscape`(layerUp)
   - `RoundCornerLayer`：fileName / offsetX / offsetY / binFileName / bufferSize / cldWidth / cldHeight / curBitmap
   - `SupportConfig`：support(bool) + mode(int)
2. **配置加载**：
   - `RCDConfig::Load(configFile)`：使用 libxml2 解析，一次性加载并缓存（`isLoadData` 标志）
   - `LCDModel::GetRog(w, h)`：按分辨率精确匹配 ROGSetting
3. **资源路径**：
   - 图片资源：`/sys_prod/etc/display/RoundCornerDisplay/<fileName>`
   - 硬件 Bin 资源：`/sys_prod/etc/display/RoundCornerDisplay/<binFileName>`
4. **硬件合成配置**：`HardwareComposer.support` 标识是否支持 硬件合成模块 圆角合成

### RSRcdSurfaceRenderNode

继承 `RSRenderNode` + `RSSurfaceHandler`，为 RCD Layer 提供渲染和 Buffer 管理：

1. **节点类型**：`RCDSurfaceType` 区分 TOP / BOTTOM / INVALID
2. **Surface 创建**：`CreateSurface(listener)` → 创建 `IConsumerSurface`（命名含 rendertargetId）→ 注册到 `RoundCornerDisplayManager` 的 Layer 映射 → 创建 Producer Surface
3. **硬件资源预备**：
   - `PrepareHardwareResourceBuffer(layerInfo)`：设置 `cldLayerInfo`/`layerBitmap`/Buffer 尺寸/srcRect/dstRect/GlobalZOrder
   - Top Surface ZOrder = `0x7FFFFFFF`，Bottom Surface ZOrder = `0x7FFFFEFF`
4. **硬件 Buffer 写入**：
   - `SetHardwareResourceToBuffer()`：将 layerBitmap 写入 SurfaceBuffer → `FillHardwareResource` 写入 CLD Bin 数据和 `CldInfo` → `SetRCDMetaData` 设置 Buffer 元数据（ATTRKEY_CLD_METADATA）
   - `FillHardwareResource`：在 Buffer 末尾追加 `CldInfo` 结构体和 `.bin` 文件内容
   - `CldInfo`：cldDataOffset / cldSize / cldWidth / cldHeight / cldStride / exWidth / exHeight / baseColor
5. **Buffer 配置**：`GetHardenBufferRequestConfig()` 设置 DMA Buffer + `PRIV_USAGE_FBC_CLD_LAYER` + RGBA_8888 格式
6. **PixelMap 生成**：`CreatePixelMapFromBitmap` 从 Drawing::Bitmap 创建 DMA PixelMap（RGBA_8888/BGRA_8888）

### RSRcdRenderManager

渲染管理器，管理 Top/Bottom 渲染节点和渲染任务调度：

1. **初始化**：`InitInstance()` 通过 `RsMessageBus` 注册四个 Topic：
   - `TOPIC_RCD_DISPLAY_SIZE` → `UpdateDisplayParameter`
   - `TOPIC_RCD_DISPLAY_ROTATION` → `UpdateOrientationStatus`
   - `TOPIC_RCD_DISPLAY_NOTCH` → `UpdateNotchStatus`
   - `TOPIC_RCD_DISPLAY_HWRESOURCE` → `UpdateHardwareResourcePrepared`
2. **节点管理**：
   - `GetTopRenderNode(id)` / `GetBottomRenderNode(id)`：懒创建 RCD Surface 渲染节点
   - NodeId 通过 `GenerateRoundCornerDisplayID(screenID, rcdTypeId)` 生成（PID_T << 32 | SkChecksum hash）
   - `CheckRenderTargetNode(context)`：定期检查 DisplayNode 是否存在，不存在则移除 RCD 资源
3. **渲染任务**：
   - `DoProcessRenderTask(id, info)`：创建 `RSRcdRenderVisitor`，处理 Bottom 和 Top Surface，硬件资源预备成功后发送 `TOPIC_RCD_DISPLAY_HWRESOURCE` 消息
   - `DoProcessRenderMainThreadTask(id, info)`：主线程路径，资源未变化时直接 `ProcessRcdSurface`

### RSRcdRenderVisitor

渲染访问器，负责 Buffer 获取和硬件资源写入：

1. **ProcessRcdSurfaceRenderNode**：
   - 资源未变化且已有 Buffer → 直接 `ProcessRcdSurface`
   - 资源变化 → 创建 Surface → `PrepareHardwareResourceBuffer` → `RequestFrame` + `Flush` → `ConsumeAndUpdateBuffer` → `ProcessRcdSurface`
2. **ConsumeAndUpdateBuffer**：AcquireBuffer → `SetHardwareResourceToBuffer`（写入 CLD 数据到 Buffer）→ ReleaseBuffer
3. **ProcessRcdSurfaceRenderNodeMainThread**：仅资源未变化时通过 `uniProcessor_->ProcessRcdSurface(node)` 处理

### RsMessageBus + RsAny

RCD 专用消息总线，基于 Topic + 类型擦除的分发机制：

1. `RegisterTopic<Args>(topic, object, member)`：注册成员函数到 Topic，以 Topic + 参数类型签名作为 Key
2. `SendMsg<Args>(topic, args...)`：按 Key 查找并调用所有注册的处理函数
3. `RsAny`：类型擦除容器，内部使用 `Derived<T>` 模板存储 `std::function<void(Args...)>`，`Send` 方法调用时加锁

`RsNotCopyable`：辅助类，禁止拷贝和赋值。

## 数据流

### RCD 初始化流程

RSMainThread → `RSRcdRenderManager::InitInstance()` → RsMessageBus 注册 Topic → `RoundCornerDisplayManager::AddRoundCornerDisplay(id)` → `RoundCornerDisplay::Init()` → LoadConfigFile + SeletedLcdModel + LoadImgsbyResolution

### RCD 参数更新流程

ScreenProperty 变化 → RsMessageBus `SendMsg<NodeId, ...>(TOPIC, id, params)` → `RoundCornerDisplayManager::Update*` → `RoundCornerDisplay::Update*` → 设置 rcdDirtyType_ + updateFlag_

### RCD 渲染流程（资源变化）

`RSRcdRenderManager::DoProcessRenderTask` → 创建 RSRcdRenderVisitor → GetTopRenderNode / GetBottomRenderNode → `ProcessRcdSurfaceRenderNode` → CreateSurface → PrepareHardwareResourceBuffer → RequestFrame + Flush → ConsumeAndUpdateBuffer（SetHardwareResourceToBuffer） → uniProcessor_->ProcessRcdSurface → `RsMessageBus::SendMsg(TOPIC_RCD_DISPLAY_HWRESOURCE)` 通知资源预备完成

### RCD 渲染流程（资源未变化）

`RSRcdRenderManager::DoProcessRenderMainThreadTask` → `ProcessRcdSurfaceRenderNodeMainThread` → `uniProcessor_->ProcessRcdSurface(node)` 直接合成已有 Buffer

### RCD 脏区域处理流程

RSMainThread 渲染 → `HandleRoundCornerDirtyRect(id, dirtyRect, TOP/BOTTOM)` → `RoundCornerDisplay::HandleTopRcdDirty / HandleBottomRcdDirty` → 将圆角区域与 dirtyRect 合并

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| Layer 分 TOP/BOTTOM | `RCDLayerType` / `RCDSurfaceType` | 圆角分上下两部分，Top 覆盖刘海/圆角区域，Bottom 覆盖底部圆角；硬件合成模块 可能分别合成 |
| 独立消息总线 | `RsMessageBus` + `RsAny` | RCD 状态更新来自主线程/渲染线程等多个来源，需要异步 Topic 分发解耦 |
| 位掩码脏区域类型 | `RoundCornerDirtyType` (0x01/0x02/0x03) | TOP 和 BOTTOM 脏区域独立标记，支持按位操作合并/清除 |
| ZOrder 分层 | Top=`0x7FFFFFFF`, Bottom=`0x7FFFFEFF` | Top 圆角层级最高（覆盖刘海），Bottom 略低但仍高于普通窗口 |
| 配置驱动 LCD 型号 | `RCDConfig` → `LCDModel` → `ROGSetting` | 不同屏幕型号有不同的圆角参数和资源，配置文件实现产品差异化 |
| DMA Buffer + CLD 元数据 | `PRIV_USAGE_FBC_CLD_LAYER` / `ATTRKEY_CLD_METADATA` | 硬件合成模块 需要 CLD 硬件压缩数据，通过 Buffer 元数据传递给 DSS |
| shared_mutex 保护资源 | `resourceMut_` | 资源读多写少，读写锁比互斥锁并发性更好 |
| 懒创建 Surface 渲染节点 | `GetTopRenderNode` / `GetBottomRenderNode` 按需创建 | RCD 不是所有屏幕都需要，懒创建避免不必要资源分配 |
| 配置未加载默认支持 | `!rcdCfg.IsDataLoaded()` → rcdSupport=true | 配置文件可能在启动后期才可用，默认支持避免早期帧缺失圆角 |
| 注册式 Topic 分发 | `RegisterTopic` + `SendMsg` | 参数更新来自不同触发源（屏幕属性变化、旋转、Notch 设置），Topic 注册解耦触发与处理 |
| 渲染线程预备 + 主线程复用 | `DoProcessRenderTask`（资源变化）/ `DoProcessRenderMainThreadTask`（复用） | 资源变化需要 GPU 渲染新 Buffer（耗时长，在渲染线程），无变化时主线程直接复用已有 Buffer |

## 测试锚点

| 测试文件 | 路径 | 覆盖 |
| --- | --- | --- |
| rs_round_corner_display_test | `rosen/test/render_service/render_service/unittest/pipeline/rs_round_corner_display_test.cpp` | RoundCornerDisplay 核心逻辑 |
| rs_rcd_render_params_test | `rosen/test/render_service/render_service_base/unittest/params/rs_rcd_render_params_test.cpp` | RCD 渲染参数 |
| rs_render_surface_rcd_layer_test | `rosen/test/render_service/render_service/unittest/composer/composer_service/render_layer/rs_render_surface_rcd_layer_test.cpp` | RCD Layer 合成 |
| rs_surface_rcd_layer_test | `rosen/test/render_service/render_service/unittest/composer/composer_client/layer/rs_surface_rcd_layer_test.cpp` | Surface RCD Layer |
| rs_rcd_surface_render_node_drawable_test | `rosen/test/render_service/render_service/unittest/drawable/rs_rcd_surface_render_node_drawable_test.cpp` | RCD Surface Drawable |

## 待补充背景

- `RSRenderSurfaceRCDLayer` 的完整实现和 硬件合成模块 合成流程
- CLD 硬件压缩数据格式和 DSS 解压细节
- 多屏场景下 RCD 的完整生命周期管理
- `SideRegionConfig`（侧边区域）的使用场景和渲染逻辑
- RCD 与安全层/TUI 的交互策略
- Buffer 超时（500ms）和 RequestFrame 失败的恢复机制
