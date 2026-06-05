# 屏幕管理

## 适用范围

- 物理屏幕热插拔与连接管理
- 虚拟屏幕创建/销毁/配置
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

支持丰富的屏幕属性设置：分辨率/模式、色彩空间、HDR 格式、像素格式、VCP 特性、背光、校正、电源状态等。通过 `RSScreenPreprocessor` 在属性变更前进行预处理。

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
| 折叠屏独立管理器 | `RSFoldScreenManager` | 折叠屏状态复杂（展开/折叠/半展），独立管理器封装产品差异 |
| 电源状态单独跟踪 | `screenPowerStatus_` map | 多屏各自独立电源状态，支持单屏灭屏 |
| VSync 使能屏幕单选 | `UpdateVsyncEnabledScreenId` | 硬件 VSync 资源有限，同一时刻只有一个屏幕启用硬件 VSync |

## 待补充背景

- `RSScreen` 的完整内部结构和 HDI 交互方式
- 虚拟屏多 Surface 的使用场景和配置规则
- `ScreenConstraintType` 的具体类型和使用场景
- 折叠屏的完整状态转换和双屏显示逻辑
- `SetScreenSecurityMask` / `SetVirtualScreenSecurityExemptionList` 的安全模型
