# 屏幕管理

## 适用范围

- 物理屏幕热插拔与连接管理
- 虚拟屏幕创建、销毁、配置
- 屏幕属性查询与设置（分辨率、色彩空间、HDR、背光）
- 折叠屏管理
- 屏幕电源状态控制
- 屏幕黑/白名单管理
- VSync 使能屏幕选择

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSScreenManager | `rosen/modules/render_service/screen_manager/rs_screen_manager.h` | 屏幕管理器核心类 |
| RSScreen | `rosen/modules/render_service/screen_manager/rs_screen.h` | 屏幕抽象 |
| RSScreenPreprocessor | `rosen/modules/render_service/screen_manager/rs_screen_preprocessor.h` | 屏幕预处理 |
| RSScreenThreadSafeProperty | `rosen/modules/render_service/screen_manager/rs_screen_thread_safe_property.h` | 线程安全屏幕属性 |
| RSFoldScreenManager | `rosen/modules/render_service/screen_manager/product_feature/fold/rs_fold_screen_manager.h` | 折叠屏管理 |
| RSCallbackManager | `rosen/modules/render_service/screen_manager/callback/rs_screen_callback_manager.h` | 屏幕回调管理 |
| TouchScreen | `rosen/modules/render_service/screen_manager/touch_screen.h` | 触摸屏 |
| Agent 接口 | `rosen/modules/render_service/screen_manager/public/` | 屏幕管理 Agent IPC 接口 |

## 核心模型

### RSScreenManager

`RSScreenManager` 继承 `RefBase`，由 `RSRenderService` 创建，管理所有物理和虚拟屏幕。核心数据结构：

- `screens_`：`map<ScreenId, shared_ptr<RSScreen>>`，所有屏幕实例
- `defaultScreenId_`：默认屏幕 ID
- `virtualScreenCount_` / `currentVirtualScreenNum_`：虚拟屏幕计数
- `pendingConnectedIds_`：待连接屏幕 ID 列表
- `screenPowerStatus_`：各屏幕电源状态
- `screenBacklight_` / `screenCorrection_`：背光和校正
- `globalBlackList_`：全局黑名单
- `foldScreenManager_`：折叠屏管理器

### 物理屏幕管理

- **连接**：`ProcessScreenConnected` 处理热插拔连接，创建 `RSScreen` 并注册
- **断连**：`ProcessScreenDisConnected` 清理屏幕资源
- **默认屏幕**：`HandleDefaultScreenDisConnected` 处理默认屏幕断连

### 虚拟屏幕管理

- **创建**：`CreateVirtualScreen` → `CreateAndRegisterVirtualScreen`，支持多 Surface 配置
- **销毁**：`RemoveVirtualScreen`，回收 ID
- **ID 管理**：`GenerateVirtualScreenId` / `freeVirtualScreenIds_`，复用已释放 ID
- **多 Surface**：`AddVirtualScreenSurface` / `RemoveVirtualScreenSurface` 支持多 Surface 虚拟屏
- **黑/白名单**：`SetVirtualScreenBlackList` / `AddVirtualScreenWhiteList` 控制虚拟屏显示内容

### 属性管理

支持丰富的屏幕属性设置：分辨率、模式、色彩空间、HDR 格式、像素格式、VCP 特性、背光、校正、电源状态等。通过 `RSScreenPreprocessor` 在属性变更前进行预处理。

### 折叠屏

`RSFoldScreenManager` 管理折叠屏状态，`RSScreenManager` 维护 `foldScreenIds_` 映射，记录每个折叠屏的连接和上电状态。

### VSync 使能

`UpdateVsyncEnabledScreenId` / `JudgeVSyncEnabledScreenWhilePowerStatusChanged` 选择 VSync 使能的屏幕，多屏场景下只有一个屏幕启用硬件 VSync。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 虚拟屏幕 ID 复用 | `freeVirtualScreenIds_` 队列 | 虚拟屏频繁创建销毁，ID 复用避免 ID 无限增长 |
| 热插拔延迟处理 | `pendingConnectedIds_` + `hotPlugAndConnectMutex_` | HDI 回调和屏幕初始化可能不同步，延迟处理保证一致性 |
| 全局黑名单独立于虚拟屏黑名单 | `globalBlackList_` + `SetVirtualScreenBlackList` | 全局黑名单适用于所有虚拟屏，虚拟屏黑名单仅影响单个 |
| 折叠屏独立管理器 | `RSFoldScreenManager` | 折叠屏状态复杂（展开、折叠、半展），独立管理器封装产品差异 |
| 电源状态单独跟踪 | `screenPowerStatus_` map | 多屏各自独立电源状态，支持单屏灭屏 |
| VSync 使能屏幕单选 | `UpdateVsyncEnabledScreenId` | 硬件 VSync 资源有限，同一时刻只有一个屏幕启用硬件 VSync |

## RSScreen 内部结构与 HDI 交互

### 双构造路径

`RSScreen` 提供两条构造路径，物理屏和虚拟屏走不同的初始化流程：

- **物理屏**：`RSScreen(ScreenId id)` → 调用 `PhysicalScreenInit()`。
  创建 `HdiScreen::CreateHdiScreen(ScreenPhysicalId(id))`，通过 HDI 获取屏幕硬件信息。
- **虚拟屏**：`RSScreen(const VirtualScreenConfigs& configs)` → 调用 `VirtualScreenInit()`。
  不创建 `hdiScreen_`，属性全部由配置传入。

### 核心成员

| 成员 | 类型 | 说明 |
| --- | --- | --- |
| `hdiScreen_` | `unique_ptr<HdiScreen>` | 物理屏 HDI 接口；虚拟屏为 nullptr |
| `supportedModes_` | `vector<GraphicDisplayModeInfo>` | HDI 返回的分辨率和刷新率模式列表 |
| `capability_` | `GraphicDisplayCapability` | HDI 返回的屏幕能力（接口类型、物理尺寸等） |
| `hdrCapability_` | `GraphicHDRCapability` | HDI 返回的 HDR 能力 |
| `property_` | `RSScreenThreadSafeProperty` | 线程安全属性封装，内部持有 `sptr<RSScreenProperty>` |
| `onPropertyChange_` | `OnPropertyChangeCallback` | 属性变更回调，注册后委托 `RSScreenPreprocessor` 通知 |
| `onBackLightChange_` | `function<void(RsScreenBrightnessData)>` | 背光变更回调，委托 `RSScreenManager::OnScreenBacklightChanged` |
| `backlightLevel_` | `atomic<int32_t>` | 背光级别缓存，初始化时从 HDI 读取 |

### HDI 交互接口

物理屏所有硬件操作都通过 `hdiScreen_` 完成，虚拟屏直接返回或操作 `property_`。
以下按阶段列出物理屏使用的 HDI 接口：

**初始化阶段**（`PhysicalScreenInit` / `ScreenCapabilityInit`）：

| HDI 调用 | 用途 |
| --- | --- |
| `HdiScreen::CreateHdiScreen` + `Init` | 创建 HDI 屏幕对象并初始化 |
| `GetScreenSupportedModes` | 获取所有分辨率和刷新率模式 |
| `GetHDRCapabilityInfos` | 获取 HDR 能力信息 |
| `SetScreenPowerStatus(ON)` | 初始上电（仅 MIPI 或 ID=0 的屏幕） |
| `GetScreenPowerStatus` | 读取初始电源状态 |
| `GetScreenCapability` | 读取屏幕能力 |
| `GetScreenConnectionType` | 读取连接类型（内部/外部） |
| `GetScreenSupportedColorGamuts` | 读取支持的色彩空间 |
| `GetScreenBacklight` | 读取初始背光级别 |
| `GetDisplayPropertyForHardCursor` | 读取硬件光标支持 |

**运行时属性设置**：

| HDI 调用 | 对应 `RSScreen` 方法 | 说明 |
| --- | --- | --- |
| `SetScreenMode` | `SetActiveMode` | 切换分辨率和刷新率模式 |
| `SetScreenPowerStatus` | `SetPowerStatus` | 设置电源状态 |
| `SetScreenColorGamut` | `SetScreenColorGamut` | 设置色彩空间 |
| `SetScreenGamutMap` / `GetScreenGamutMap` | `SetScreenGamutMap` / `GetScreenGamutMap` | 色调映射设置/获取 |
| `SetScreenOverlayResolution` | `SetRogResolution` | ROG 分辨率缩放 |
| `SetScreenConstraint` | `SetScreenConstraint` | 帧时间约束 |
| `SetScreenVsyncEnabled` | `SetScreenVsyncEnabled` | VSync 使能控制 |
| `SetScreenVCPFeature` / `GetScreenVCPFeature` | `SetScreenVCPFeature` / `GetScreenVCPFeature` | VCP 特性设置/获取，仅用于扩展屏，当前只操作屏幕亮度 |
| `GetPanelPowerStatus` | `GetPanelPowerStatus` | 面板电源状态 |
| `SetDisplayProperty` | `SetDualScreenState` | 双屏状态设置 |
| `GetDisplayIdentificationData` | `GetDisplayIdentificationData` | EDID 数据读取 |
| `SetScreenBacklight` | `SetScreenBacklight` | 背光设置（非 HDI 调用，通过回调 + 缓存实现） |

### 属性变更通知机制

所有属性修改通过 `UPDATE_PROPERTY` 宏统一处理：

```c++
#define UPDATE_PROPERTY(name, value)            \
    do {                                        \
        auto prop = property_.Set##name(value); \
        NotifyScreenPropertyChange(prop);       \
    } while (0)
```

通知链路：

```text
RSScreen 属性变更
  │ UPDATE_PROPERTY 宏
  │
  ├─① property_.SetXxx(value)           ← 写入线程安全属性，返回变更类型和值
  │
  ├─② RSScreen::NotifyScreenPropertyChange()
  │     └─ onPropertyChange_ 回调
  │
  ├─③ RSScreenPreprocessor::NotifyScreenPropertyChanged()
  │     └─ 区分物理屏/虚拟屏，分发到回调管理器
  │
  └─④ RSScreenCallbackManager::NotifyScreenPropertyUpdated()
        └─ 通知 AgentListener（DMS、SCB 等外部模块）
```

背光变更走独立回调 `onBackLightChange_`，不经过上述链路，直接由
`RSScreenManager::OnScreenBacklightChanged` 处理。

## 虚拟屏多 Surface 配置

### SurfaceRegionConfig

```c++
struct SurfaceRegionConfig {
    sptr<Surface> surface = nullptr;   // 生产者 Surface
    RectI region;                       // 渲染区域（x, y, width, height）
};
```

一个虚拟屏可绑定多个 Surface，每个 Surface 有独立的渲染区域。
典型场景：投屏镜像、扩展模式、多窗口分屏投送。

### 配置规则

- **创建时配置**：`VirtualScreenConfigs.surfaceConfigs` 传入初始 Surface 列表。
  `surfaceConfigs` 为空 → 屏幕状态 `DISABLED`；非空 → `PRODUCER_SURFACE_ENABLE`。
- **运行时增删**：
  - `SetMultiSurfaceConfigs`：整体替换，同时更新屏幕状态。
  - `AddSurfaceConfigs`：追加 Surface 配置，追加后无 Surface 则转为 `DISABLED`。
  - `RemoveSurfaceConfigs`：按 `surfaceId` 集合删除，删除后无 Surface 则转为 `DISABLED`。
- **Surface 唯一性**：`CollectVirtualScreenSurfaceIds` 检查所有虚拟屏的 Surface ID，
  同一个 `Surface` 不允许被多个虚拟屏共用。
- **容量限制**：
  - 最大虚拟屏数量：`MAX_VIRTUAL_SCREEN_NUM = 64`。
  - 最大尺寸：65536 × 65536。
  - 黑/白名单最大条目数：`MAX_SPECIAL_LAYER_NUM`。
- **ID 复用**：虚拟屏销毁后 ID 进入 `freeVirtualScreenIds_` 队列，
  新建虚拟屏优先复用已释放 ID，避免 64 位 ID 无限增长。

### 创建流程

`CreateVirtualScreen` → `ValidateVirtualScreenLimits` → 构建 `SurfaceRegionConfig` →
`CreateAndRegisterVirtualScreen` → `GenerateVirtualScreenId` → `new RSScreen(configs)` →
注册到 `screens_` → `NotifyVirtualScreenConnected` → 返回新 ID。

## ScreenConstraintType

`ScreenConstraintType` 定义帧在屏幕上显示的时间约束方式，
用于精确控制帧呈现时机，由 `SetScreenConstraint(frameId, timestamp, type)` 设置：

| 枚举值 | 含义 | 说明 |
| --- | --- | --- |
| `CONSTRAINT_NONE` (0) | 无约束 | 不对帧呈现时间做任何要求 |
| `CONSTRAINT_ABSOLUTE` | 绝对时间戳 | 帧必须在给定的绝对时间点呈现 |
| `CONSTRAINT_RELATIVE` | 相对时间戳 | 帧在相对于某个参考点的时间偏移后呈现 |
| `CONSTRAINT_ADAPTIVE` | 自适应 VSync | 帧根据自适应 VSync 模式呈现 |

使用规则：

- 仅作用于物理屏；虚拟屏调用直接返回 `SUCCESS`，不传 HDI。
- 调用链：`RSScreenManager::SetScreenConstraint` → `RSScreen::SetScreenConstraint` →
  `hdiScreen_->SetScreenConstraint(frameId, timestamp, static_cast<uint32_t>(type))`。
- `frameId` 由 `RSScreenManager::frameId_`（atomic）递增生成，
  用于关联约束与具体帧。
- HGM 刷新率决策链路中使用 `CONSTRAINT_ADAPTIVE` 类型配合 LTPO 帧调度。

## 折叠屏状态转换与双屏逻辑

### FoldState 枚举

```c++
enum class FoldState : uint32_t {
    UNKNOW,   // 未知
    FOLDED,   // 折叠（angle ≤ 25°）
    EXPAND    // 展开（angle > 25°）
};
```

### 状态转换规则

`TransferAngleToScreenState` 将姿态传感器角度转换为屏幕状态：

- 角度 ≤ `OPEN_HALF_FOLDED_MIN_THRESHOLD`（25°）→ `FOLDED`
- 角度 > 25° → `EXPAND`

### 双屏选择逻辑

`HandleSensorData(angle, abAngle)` 根据两个角度和屏幕可用性决定活跃屏幕：

```text
if (angle → FOLDED) && (abAngle → FOLDED) && (externalScreenId != INVALID)
    → targetScreenId = externalScreenId（外屏）
else
    → targetScreenId = innerScreenId（内屏，固定为 0）
```

- **折叠态**：使用外屏（externalScreenId），内屏不可见。
- **展开态**：使用内屏（innerScreenId = 0），外屏作为辅助。
- `abAngle` 是 A-B 铰角，默认为 0（未折叠）。两者同时 FOLDED 才切换到外屏。

### 姿态传感器

- 传感器类型：`SENSOR_TYPE_ID_POSTURE`。
- 采样间隔：`POSTURE_INTERVAL = 4000000` ns（4 ms）。
- 注册/注销流程：`RegisterSensorCallback`（Subscribe + SetBatch + Activate）→
  `HandlePostureData` → 投递到 mainHandler → `HandleSensorData`。
- 开机完成后注销传感器：监听 `bootevent.boot.completed` 属性 → `UnRegisterSensorCallback`。

### RSScreenManager 折叠屏跟踪

| 数据 | 说明 |
| --- | --- |
| `foldScreenIds_` | `map<ScreenId, FoldScreenStatus>`，每个折叠屏记录 `isConnected` 和 `isPowerOn` |
| `ORIGINAL_FOLD_SCREEN_AMOUNT = 2` | 内屏 + 外屏 |
| `isFoldScreenFlag_` | 从 `const.window.foldscreen.type` 系统属性读取 |
| `foldScreenManager_` | `RSFoldScreenManager` 实例，仅在 `isFoldScreenFlag_` 为 true 时创建 |

### VSync 选择与折叠屏

`JudgeVSyncEnabledScreenWhilePowerStatusChanged` 在折叠屏电源状态变化时选择 VSync 使能屏幕：

- 上电（POWER_STATUS_ON）：标记 `isPowerOn = true`，若当前 VSync 屏幕未上电则切换到新上电屏幕。
- 下电（POWER_STATUS_OFF）：标记 `isPowerOn = false`，若下电的不是当前 VSync 屏幕则保持不变；
  否则在剩余折叠屏中找已连接且已上电的屏幕。

## 虚拟屏安全模型

虚拟屏内容渲染受多层安全机制控制，确保敏感内容不被投送到非信任屏幕。

### 安全层级

```text
1. 全局黑名单（globalBlackList_）
   └─ 适用于所有虚拟屏，存储在 RSScreenManager

2. 单屏黑名单（per-screen blackList）
   └─ 仅影响特定虚拟屏，存储在 RSScreenThreadSafeProperty

3. 类型黑名单（typeBlackList）
   └─ 按节点类型（uint8_t）过滤，仅影响特定虚拟屏

4. 白名单（whiteList）
   └─ 明确允许在特定虚拟屏上渲染的节点，按 NodeId

5. 安全豁免列表（SecurityExemptionList）
   └─ 按NodeId 豁免安全过滤，允许特定节点绕过安全策略渲染到虚拟屏

6. 安全遮罩（SecurityMask）
   └─ PixelMap 图像遮罩，在敏感区域叠加视觉遮盖
```

### 安全豁免列表（SetVirtualScreenSecurityExemptionList）

- 仅作用于虚拟屏（`RSScreenManager` 校验 `IsVirtual()`）。
- 参数：`vector<uint64_t>`，包含允许绕过安全过滤的 NodeId。
- 场景：投屏时需要显示某些安全窗口（如系统通知），
  这些窗口默认会被安全策略拦截，豁免列表允许其通过。
- 存储：`RSScreenThreadSafeProperty::SetSecurityExemptionList`，
  线程安全写入，变更通知通过 `UPDATE_PROPERTY` 宏触发。

### 安全遮罩（SetScreenSecurityMask）

- 作用于任意屏幕（物理屏和虚拟屏均可）。
- 参数：`shared_ptr<Media::PixelMap>`，遮罩图像。
- 场景：对屏幕上的敏感区域叠加遮盖（如密码输入框遮挡），
  遮罩图像在渲染时覆盖指定区域。
- 存储：`RSScreenThreadSafeProperty::SetSecurityMask`，
  线程安全写入，变更通知通过 `UPDATE_PROPERTY` 宏触发。

### 判断优先级

渲染侧在处理虚拟屏内容时按以下优先级判断节点是否可见：

1. 节点在 SecurityExemptionList 中 → 允许渲染，跳过后续检查。
2. 节点在全局黑名单中 → 禁止渲染。
3. 节点在单屏黑名单中 → 禁止渲染。
4. 节点类型在类型黑名单中 → 禁止渲染。
5. 节点在白名单中 → 允许渲染。
6. 未命中以上规则 → 按 `CastScreenEnableSkipWindow` 和安全标志决定。

### 其他安全相关设置

| 设置 | 方法 | 说明 |
| --- | --- | --- |
| 投屏跳窗 | `SetCastScreenEnableSkipWindow` | 控制投屏是否跳过特定窗口 |
| 安全层选项 | `VirtualScreenConfigs.flags` → `SetVirtualSecLayerOption` | 创建虚拟屏时的安全层标志 |
| 可见区域 | `SetMirrorScreenVisibleRect` | 限制镜像屏只渲染主屏指定矩形区域 |
| 渲染控制禁用 | `SetDisablePowerOffRenderControl` | 禁止灭屏时的渲染控制
