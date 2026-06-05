# Single Frame Composer / Power Off Render Skip / VCLD

## 适用范围

- 单帧合成（Single Frame Composer）优化
- 灭屏渲染跳过
- VCLD（Video Corner Layer Display）圆角视频参数

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSSingleFrameComposer | `rosen/modules/render_service_base/include/feature/single_frame_composer/rs_single_frame_composer.h` | 单帧合成器 |
| RSPowerOffRenderController | `rosen/modules/render_service_base/include/feature/power_off_render_skip/rs_power_off_render_controller.h` | 灭屏渲染跳过控制器 |
| RSVcldParam | `rosen/modules/render_service_base/include/feature/vcld/rs_vcld_param.h` | VCLD 参数结构体 |

## 核心模型

### Single Frame Composer

`RSSingleFrameComposer` 优化同一帧内多个 Modifier 的渲染：

1. **单帧 Modifier 管理**：
   - `SingleFrameModifierAddToListNG`：将单帧 Modifier 添加到列表
   - `SingleFrameIsNeedSkipNG`：判断是否需要跳过
   - `SingleFrameAddModifierNG`：添加单帧 Modifier
2. **IPC 线程识别**：
   - `SetSingleFrameFlag(ipcThreadId)`：设置 IPC 线程标志
   - `IsShouldSingleFrameComposer`：是否应该进行单帧合成
3. **应用进程映射**：
   - `AddOrRemoveAppPidToMap(isNodeSingleFrameComposer, pid)`：添加/移除应用进程
   - `IsShouldProcessByIpcThread(pid)`：是否应由 IPC 线程处理

关键数据：

- `singleFrameDrawCmdModifiersNG_`：`map<RSModifierType, vector<shared_ptr<RSRenderModifier>>>` 单帧绘制命令 Modifier
- `ipcThreadIdMap_`：`map<thread::id, uint64_t>` IPC 线程映射（static）
- `appPidMap_`：`map<pid_t, uint64_t>` 应用进程映射（static）
- `visibleWinCount_`：可见窗口计数（atomic static）

### Power Off Render Skip

`RSPowerOffRenderController` 控制灭屏时的渲染跳过：

1. **状态检查**：`CheckScreenPowerRenderControlStatus(nodeMap)` 在主线程计算各屏幕渲染跳过状态
2. **状态查询**：`GetScreenRenderSkipped(screenId)` 查询单屏是否跳过
3. **全局查询**：`GetAllScreenRenderSkipped()` 查询是否所有屏都跳过
4. **状态同步**：`SyncFrom(sourceController)` 从源控制器同步状态

渲染跳过状态枚举 `RenderSkipStatus`：

- `DISABLE_CONTROL`：禁用灭屏渲染跳过控制
- `POWER_OFF_SKIP`：屏幕灭屏，可跳过渲染
- `FORCE_REFRESH`：屏幕灭屏，强制刷新渲染一帧
- `SCREEN_RENDER`：屏幕亮屏，正常渲染

数据：`screenRenderSkipStatus_`：`unordered_map<ScreenId, RenderSkipStatus>` 各屏幕跳过状态。

### VCLD

`RSVcldParam` 定义视频圆角 Layer 显示参数：

- `enable`：是否启用 VCLD
- `radius`：圆角半径，范围 `[1.5, 1/2 * min(img_width, img_height)]`
- `posOffset[4]`：视频目标帧矩形偏移（LEFT/TOP/RIGHT/BOTTOM），值为 0 或 0.5

`operator==` 支持精确比较（使用 `IsScalarAlmostEqual`）。

VCLD 参数通过 `RSSurfaceLayer::SetVcldInfo` / `GetVcldInfo` 设置和获取，最终传递给 HWC Layer。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 单帧合成基于 IPC 线程识别 | `ipcThreadIdMap_` | 同一 IPC 调用链上的 Modifier 可以合并渲染，减少重绘次数 |
| 单帧 Modifier 按类型分组 | `singleFrameDrawCmdModifiersNG_` map | 同类型 Modifier 合并处理，避免重复绘制 |
| 灭屏渲染跳过分级控制 | `RenderSkipStatus` 四级枚举 | 不同场景（灭屏/强制刷新/亮屏）需要不同控制策略 |
| 灭屏强制刷新 | `FORCE_REFRESH` 状态 | 灭屏后某些场景（如亮屏前）需要渲染一帧，避免亮屏闪黑 |
| VCLD 偏移为 0 或 0.5 | `posOffset` 约束 | 视频圆角的偏移为半像素对齐，0 表示无偏移，0.5 表示半像素偏移 |
| VCLD 精确比较 | `IsScalarAlmostEqual` | 浮点比较避免因精度问题导致不必要的更新 |

## 待补充背景

- 单帧合成的完整触发时序和 Modifier 合并策略
- `visibleWinCount_` 对单帧合成的影响
- 灭屏渲染跳过与 RSScreenManager::MarkPowerOffNeedProcessOneFrame 的协调
- VCLD 在 HWC 合成中的完整处理流程
- VCLD radius 的具体计算方式和产品默认值
