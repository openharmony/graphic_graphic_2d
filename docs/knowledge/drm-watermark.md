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
| RSDrmUtil | `rosen/modules/render_service/core/feature/drm/rs_drm_util.h` | DRM 静态工具类 |
| RSSurfaceWatermarkHelper | `.../feature/watermark/rs_surface_watermark.h` | 水印辅助类 |
| SurfaceWatermarkType | `.../render_service_base/include/common/rs_common_def.h:839-843` | 水印类型与状态码 |
| DRMParam | `.../feature_cfg/feature_param/extend_feature/drm_param.h` | DRM 白/黑名单与 mock 开关 |
| DRMParamParse | `.../feature_cfg/xml_parser/extend_feature_parser/drm_param_parse.h` | XML 配置解析 |

> `...` 代表 `rosen/modules/render_service`（RSDrmUtil/RSSurfaceWatermarkHelper）
> 或 `rosen/modules/render_service_base`（rs_common_def.h），下同。

## 核心模型

### DRM

`RSDrmUtil`（rs_drm_util.h:24，命名空间 `OHOS::Rosen`）非单例，所有方法均为 static，
唯一静态成员是 `inline static drmNodes_`，实质为静态工具类。

public typedef：`DrawablesVec`（:28）=
`vector<tuple<NodeId, NodeId, DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>>`

public 方法（8 个）：

- `ClearDrmNodes()`：每帧起始清空 drmNodes_（rs_main_thread.cpp:2135）
- `AddDrmCloneCrossNode(surfaceNode, DrawablesVec&)`：处理 DRM 克隆跨节点
- `DRMCreateLayer(processor, Drawing::Matrix)`：在 Processor 上创建 DRM Layer
  （h 文件形参名 `hwcMatrix`，cpp 形参名 `canvasMatrix`）
- `MarkBlurIntersectWithDRM(node, drmNodes, curScreenNode)`：3 参数 public 重载
- `IsDRMNodesOnTheTree()`：渲染树中是否有 DRM 节点
- `DealWithDRMNodes(surfaceNode, buffer, clientManager)`：先 CollectDrmNodes 再 PreAlloc
- `HasDRMInVirtualScreen(canvas, surfaceParams)`：虚拟屏是否有 DRM 内容

private 方法（6 个）：`MarkAllBlurIntersectWithDRM`、
`MarkBlurIntersectWithDRM`（2 参数重载）、`IsDRMBelowFilter`、`GetDarkColorMode`、
`CollectDrmNodes`、`PreAllocProtectedFrameBuffers`。

> 注：`CollectDrmNodes` 是 private，被 `DealWithDRMNodes` 调用；原文档将其列为 public 首位有误。

成员变量：
- `drmNodes_`（:55-56）：
  `inline static unordered_map<NodeId, vector<shared_ptr<RSSurfaceRenderNode>>>`，
  键为一级节点 ID，值为 DRM surface node 列表。

关键实现分支：
- `MarkBlurIntersectWithDRM`（3 参，cpp:105-121）：PC 模式
  （`RSSystemProperties::GetSceneBoardIsPcMode()`）或
  `DRMParam::IsMockAllBlurEffectIntersectWithDRMEnable()` 走 `MarkAllBlurIntersectWithDRM`
  （用 `DRMParam::GetBlackList()` 过滤窗口名）；否则走 2 参重载
  （用 `DRMParam::GetWhiteList()` 过滤窗口名）。
- `GetDarkColorMode`（cpp:220-229）：默认取 `RSMainThread::GetGlobalDarkColorMode()`；
  若 appWindowNode 名含 `SCBSmartDock`，按 `GetBgBrightnessSaturation()` 与 1.4f 比较。
- `IsDRMBelowFilter`（cpp:200-218）：遍历 `curScreenNode->GetAllMainAndLeashSurfaces()`，
  返回 `drmNodeIndex > filterNodeIndex`（DRM 在 filter 下方）。

### Watermark

`RSSurfaceWatermarkHelper`（rs_surface_watermark.h:44，命名空间 `OHOS::Rosen`）非单例，
持有在 `RSMainThread` 作 `surfaceWatermarkHelper_` 成员（rs_main_thread.h:913）。

public 方法（7 个 + 1 inline getter）：

- `SetSurfaceWatermark(pid, name, pixelMap, nodeIdList, watermarkType, mainContext, ...)`
  ：设置水印，支持网格（isSystemCalling/rowCount/colCount 有默认值）
- `DrawCommSurfaceWatermark(canvas, params)`：绘制公共水印（静态）
- `ClearSurfaceWatermarkForNodes(pid, name, nodeIdList, mainContext, isSystemCalling=false)`：按节点清除
- `ClearSurfaceWatermark(pid, name, mainContext, isSystemCalling=false, isDeathMonitor=false)`：按名清除
- `ClearSurfaceWatermark(pid, mainContext)`：重载，按 pid 清全部
- `GetSurfaceWatermarks()`：public inline getter
- `GetGridWatermarkEffect()`：网格水印 RuntimeEffect（静态）

> 注：`CheckClearWatermarkPermission` 是 **private inline**（:83），原文档列为 public 有误。
> `DrawWatermark`（private static，:107）是 `DrawCommSurfaceWatermark` 的内部实现。

成员变量：
- `surfaceWatermarks_`（:109）：`unordered_map<string, RSSurfaceWatermarkInfo>`
- `watermarkNameMapNodeId_`（:111-112）：
  `unordered_map<string, pair<unordered_set<NodeId>, SurfaceWatermarkType>>`
- `registerSurfaceWatermarkCount_`（:113）：`uint32_t`，上限 `MAX_LIMIT_SURFACE_WATER_MARK_IMG=1000`（cpp:23）

> `isDeathMonitor` 不是成员变量，只是 `ClearSurfaceWatermark(pid,name,...)` 的参数；
> 死亡监听路径不清 `surfaceWatermarks_` map（保留以便后续），见 cpp:284-310。

### 数据结构

`RSSurfaceWatermarkInfo`（rs_surface_watermark.h:37-42）：
```cpp
struct RSSurfaceWatermarkInfo {
    std::shared_ptr<Drawing::Image> image;   // 字段名 image（非 Image）
    pid_t pid = 0;
    uint32_t rowCount = 0;
    uint32_t colCount = 0;
};
```

`RSSurfaceWatermarkSpecialParam`（:31-35，字段带尾下划线）：
```cpp
struct RSSurfaceWatermarkSpecialParam {
    float maxWidth_ = 0;
    float maxHeight_ = 0;
    bool isWatermarkChange_ = true;
};
```

### 枚举

`SurfaceWatermarkType`（rs_common_def.h:839-843，底层 `uint8_t`）：
```cpp
typedef enum : uint8_t {
    CUSTOM_WATER_MARK = 0,
    SYSTEM_WATER_MARK = 1,
    INVALID_WATER_MARK = 2,
} SurfaceWatermarkType;
```

`SurfaceWatermarkStatusCode`（rs_common_def.h:819-837，共 17 个）：
`WATER_MARK_SUCCESS=0`、`NAME_ERROR`、`RS_CONNECTION_ERROR`、`IMG_ASTC_ERROR`、
`NOT_SUPPORT_ERROR`、`RENDER_SERVICE_NULL`、`WRITE_PARCEL_ERR`、`IPC_ERROR`、
`READ_PARCEL_ERR`、`RS_NOT_FIND_NODE`、`PERMISSION_ERROR`、`IMG_SIZE_ERROR`、
`NODE_NOT_SCREEN`、`PIXELMAP_INVALID`、`NOT_SURFACE_NODE_ERROR`、
`INVALID_WATERMARK_TYPE`、`INVALID_GRID_COUNT`。

## DRM 受保护 buffer 分配与 HWC 处理

### 触发条件

`rs_main_thread.cpp:2242-2253`（`#ifdef RS_ENABLE_VK`）：当 GPU 模式为 VULKAN/DDGR 且
源 Surface buffer 含 `BUFFER_USAGE_PROTECTED` 时，`SetProtectedLayer(true)` 写入
`SpecialLayerType::PROTECTED` 并向 firstLevelNode 聚合 `HAS_PROTECTED`，再调
`RSDrmUtil::DealWithDRMNodes`。

### 分配链路

```
RSDrmUtil::PreAllocProtectedFrameBuffers (rs_drm_util.cpp:244)
  → RSUniRenderThread::AddScreenHasProtectedLayerSet(screenId)
  → clientManager->PreAllocProtectedFrameBuffers(screenId, buffer)
  → RSComposerClient → RSComposerContext → RSRenderToComposerConnection
  → RSRenderComposerAgent::PreAllocProtectedFrameBuffers (ffrt::submit 异步)
  → RSRenderComposer::PreAllocateProtectedBuffer (rs_render_composer.cpp:666)
  → RSSurfaceOhosVulkan::PreAllocateProtectedBuffer (rs_surface_ohos_vulkan.cpp:134)
```

usage flag（rs_render_composer.cpp:708-709）：
```
BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_FB
  | BUFFER_USAGE_PROTECTED | BUFFER_USAGE_DRM_REDRAW
```

### HWC 处理 DRM/protected layer

- `rs_uni_hwc_visitor.cpp:589-595`：HWC 遍历时把含 `PROTECTED` 的 hwcNode
  加入 visitor 的 `drmNodes_`，对 firstLevelNode 设 `SetForceDisableClipHoleForDRM`。
- `rs_uni_hwc_visitor.cpp:730-732`：protected layer 跳过 round corner 强制禁用。
- `rs_uni_hwc_visitor.cpp:1298-1308` `UpdateCrossInfoForProtectedHwcNode`：
  同步 firstLevelNode 的 globalPosition/crossNode。
- `rs_uni_hwc_visitor.cpp:1355-1356`：protected layer 豁免常规强制禁用（保持 HWC 直通）。
- `rs_uni_render_processor.cpp:332-357` `GetForceClientForDRM`：protected layer
  在动画、旋转 90° 倍数、有 cornerRadiusInfo、屏幕旋转或旋转缓存开启时**回退 GPU client**。
- `rs_uni_render_virtual_processor.cpp:526-528`：虚拟屏合成时 protected layer 跳过 fence merge。
- `rs_surface_render_node_drawable.cpp`：protected layer 在 capture（:1420）、
  虚拟屏（:988）、非 protected context（:1582）画黑块。
- `rs_render_composer.cpp:934-939`：redraw 时若 buffer 含 `BUFFER_USAGE_PROTECTED`
  调 `ChangeProtectedState(true)`。
- `rs_multi_screen_util.cpp:536-540`：镜像屏 rebuild 时若含 `HAS_PROTECTED`
  调 `RSDrmUtil::DRMCreateLayer`。

> 注：visitor 的 `drmNodes_`（rs_uni_render_visitor.h:455，`vector<weak_ptr>`）
> 与 `RSDrmUtil::drmNodes_`（`unordered_map`）是两个不同容器，由 HWC visitor 在 :590 填充。

## DRM 与 HDR 交互

有明确交互代码（原"待补充"已结案）：

- `rs_uni_render_visitor.cpp:458`：硬件 HDR 被禁用且 `drmNodes_` 非空时，
  `SetHdrStatus(screenId, NO_HDR)` 强制关闭 HDR。
- `rs_uni_render_visitor.cpp:2910-2930`：有 DRM 节点且该层非 protected 时，
  HWC 被强制禁用，HDR 回退到 uni render。
- 两个 feature 目录（`feature/drm/`、`feature/hdr/`）之间无直接 include 或调用，
  交互发生在 `rs_uni_render_visitor.cpp`。

## 网格水印 shader

`GetGridWatermarkEffect`（rs_surface_watermark.cpp:108-134）使用 SkSL
（`Drawing::RuntimeEffect::CreateForShader`），源码：

```cpp
static constexpr char prog[] = R"(
    uniform shader watermark;
    uniform float2 surfaceSize;
    uniform float2 imageSize;
    uniform float2 gridCount;
    half4 main(float2 coords) {
        float cellW = surfaceSize.x / gridCount.x;
        float cellH = surfaceSize.y / gridCount.y;
        float localX = coords.x - cellW * floor(coords.x / cellW);
        float localY = coords.y - cellH * floor(coords.y / cellH);
        float offsetX = (cellW - imageSize.x) / 2.0;
        float offsetY = (cellH - imageSize.y) / 2.0;
        float sampleX = localX - offsetX;
        float sampleY = localY - offsetY;
        if (sampleX < 0.0 || sampleX > imageSize.x
            || sampleY < 0.0 || sampleY > imageSize.y) {
            return half4(0.0);
        }
        return watermark.eval(float2(sampleX, sampleY));
    }
)";
```

- uniform：`watermark`（image shader child）、`surfaceSize`、`imageSize`、`gridCount`
- 逻辑：surface 坐标按 cellW/cellH 取模得 cell 内局部坐标，居中放置 watermark 图像，
  cell 外返回 `half4(0.0)`（透明）。
- 触发条件（`DrawWatermark` cpp:69-95）：仅当 `rowCount>0 && colCount>0` 且图像小于 cell 时用
  grid shader，否则用 `TileMode::REPEAT` 平铺 imageShader。

## 水印在虚拟屏/镜像屏的处理

- **水印本身无虚拟屏/镜像屏特殊分支**：`DrawCommSurfaceWatermark` 在 surface drawable `OnDraw`
  无条件调用（rs_surface_render_node_drawable.cpp:268），UIFirst 缓存路径也调
  （rs_draw_window_cache.cpp:237、rs_sub_thread_cache.cpp:1260）。
- **DRM 虚拟屏**：`HasDRMInVirtualScreen`（rs_drm_util.cpp:263-284）判断
  `screenProperty.IsVirtual()` 且 surface 含 `SpecialLayerType::PROTECTED`，
  为真则在 `OnDraw` 画黑块（:988-991）。
- **DRM 镜像屏**：`DrawPhysicalMirrorRebuild`（rs_multi_screen_util.cpp:538）
  在 mirrorSourceDisplay 含 `HAS_PROTECTED` 时调 `RSDrmUtil::DRMCreateLayer`。

## DRMParam 配置

`DRMParam`（drm_param.h:22-44）提供 DRM 模糊相交标记的可配置项：

- `IsMockAllBlurEffectIntersectWithDRMEnable()`：是否 mock 所有模糊-DRM 相交
- `GetWhiteList()`：非 PC 路径的窗口名白名单（仅名单内窗口处理模糊-DRM 相交）
- `GetBlackList()`：PC 路径的窗口名黑名单
- 由 `DRMParamParse`（drm_param_parse.h:23）从 XML 配置解析

`MarkBlurIntersectWithDRM` 的 PC/非 PC 分支即用上述名单过滤窗口名。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| DRM 节点按一级节点索引 | `drmNodes_` unordered_map | 按一级节点 ID 快速定位 |
| DRM 与模糊交互标记 | `MarkBlurIntersectWithDRM` | DRM 上方模糊不能 GPU 模糊，标记回退 |
| 受保护 buffer 预分配 | `PreAllocProtectedFrameBuffers` | 需特殊 flag，提前分配避免阻塞 |
| 水印按名称管理 | `watermarkNameMapNodeId_` | 同名水印应用多节点，便于批量操作 |
| 水印清除权限检查 | `CheckClearWatermarkPermission`（private） | 防非设置者清除 |
| 网格水印 RuntimeEffect | `GetGridWatermarkEffect` | SkSL shader 居中平铺，性能优于 CPU |
| 水印数量上限 | `MAX_LIMIT_SURFACE_WATER_MARK_IMG=1000` | 防止水印资源无限增长 |
| protected layer 画黑块 | drawable :988/:1420/:1582 | capture/虚拟屏/非 protected context 安全降级 |

## 测试锚点

| 路径 | 说明 |
| --- | --- |
| `.../feature/drm/rs_drm_util_test.cpp` | DRM 主单测 24 个；覆盖 MarkBlur 系列/IsDRMNodesOnTheTree 等 |
| 同上（未覆盖部分） | **未覆盖** DRMCreateLayer/AddDrmCloneCrossNode/IsDRMBelowFilter/GetDarkColorMode |
| `.../rs_surface_render_node_drawable_test.cpp` | DrawCommSurfaceWatermark / GetGridWatermarkEffect（:2378-2485） |
| `.../main_thread/rs_render_service_connection_test.cpp:252-451` | SetSurfaceWatermark / ClearWatermarkEnabled |
| `.../platform/ohos/rs_render_pipeline_client_test.cpp:598,614` | client 侧 CUSTOM_WATER_MARK |
| `.../rs_interfaces_test.cpp:933-1136` | RSInterfaces watermark 接口，含 INVALID、grid count |
| `.../fuzztest/.../watermarkstub_fuzzer/watermarkstub_fuzzer.cpp` | SET_SURFACE_WATERMARK stub fuzz |
| `.../fuzztest/.../watermark_fuzzer/watermark_fuzzer.cpp` | RSInterfaces watermark fuzz |
| `graphic_test/test/rs_func_feature/watermark/watermark.cpp` | 水印功能测试主入口（1311 行） |
| `rosen/modules/render_service_client/test/rs_surface_watermark_demo.cpp` | CUSTOM 水印 demo |
| `.../rs_surface_watermark_rowcol_demo.cpp` | 行列网格水印 demo |

DRM 侧无 fuzz 测试。

## 待补充背景

- `DRMCreateLayer`、`AddDrmCloneCrossNode`、`IsDRMBelowFilter`、`GetDarkColorMode` 无单测覆盖。
- DRM 侧无 fuzz 测试。
- `DRMParam` 的 XML 配置文件路径与具体字段需在 `feature_cfg` 模块进一步确认。
- protected layer 在 `GetForceClientForDRM` 回退 GPU 合成时的性能影响未量化。
