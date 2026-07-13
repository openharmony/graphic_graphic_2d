# RS ColorPicker 独立线程特性

## 适用范围

- RS（Render Service）进程内独立线程颜色提取的全链路
- `RSColorPickerDrawable` 的生命周期与状态机
- `RSColorPickerThread` 的 GPU 共享上下文与 fence 同步
- `RSColorPickerManager`（CONTRAST 策略）与 `ColorPickAltManager`（CLIENT_CALLBACK 策略）的差异
- RS ColorPicker 与应用进程 `ColorPicker`（`rosen/modules/effect/color_picker`）的区分

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| IColorPickerManager | `rosen/modules/render_service_base/include/feature/color_picker/i_color_picker_manager.h` | 抽象接口 |
| RSColorPickerManager | `rosen/modules/render_service_base/include/feature/color_picker/rs_color_picker_manager.h` | CONTRAST 策略实现 |
| ColorPickAltManager | `rosen/modules/render_service_base/include/feature/color_picker/color_pick_alt_manager.h` | CLIENT_CALLBACK 策略实现 |
| RSColorPickerThread | `rosen/modules/render_service_base/include/feature/color_picker/rs_color_picker_thread.h` | 独立线程单例 |
| RSColorPickerUtils | `rosen/modules/render_service_base/include/feature/color_picker/rs_color_picker_utils.h` | GPU fence 同步、快照、ExecColorPick |
| RSHeteroColorPicker | `rosen/modules/render_service_base/include/feature/color_picker/rs_hetero_color_picker.h` | MHC/HPS 硬件加速路径 |
| RSColorPickerDrawable | `rosen/modules/render_service_base/include/drawable/rs_color_picker_drawable.h` | Drawable 槽位适配器 |
| RSColorPickerDrawable 实现 | `rosen/modules/render_service_base/src/drawable/rs_color_picker_drawable.cpp` | OnGenerate/OnPrepare/OnSync/OnDraw |
| ColorPick 参数定义 | `rosen/modules/render_service_base/include/property/rs_color_picker_def.h` | ColorPickerParam、ColorPickStrategyType |
| Placeholder 映射表 | `rosen/modules/render_service_base/include/feature/color_picker/rs_color_placeholder_mapping_def.in` | 占位符到亮/暗色的 X-macro 表 |
| Drawable 槽位注册 | `rosen/modules/render_service_base/src/drawable/rs_drawable.cpp` | COLOR_PICKER 槽位注册 |
| 主线程集成 | `rosen/modules/render_service/core/pipeline/main_thread/rs_main_thread.cpp` | 回调注册、InitRenderContext、SendColorPickerCallback |
| Visitor 集成 | `rosen/modules/render_service/core/pipeline/main_thread/rs_uni_render_visitor.cpp` | PrepareColorPickers、PostColorPickerStateTask |
| HWC 集成 | `rosen/modules/render_service/core/pipeline/hwc/rs_uni_hwc_visitor.cpp` | UpdateHwcNodeEnableByColorPicker |
| 单测 | `rosen/test/render_service/render_service_base/unittest/feature/color_picker/` | 4 个测试文件（manager/thread/utils/hetero） |
| 源码内流程图 | `rosen/modules/render_service_base/src/feature/color_picker/README.md` | 架构图、状态机图、执行流程图 |

## 核心模型

### 与应用进程 ColorPicker 的区分

| 方面 | RS ColorPicker（本文档） | 应用进程 ColorPicker |
| --- | --- | --- |
| 运行进程 | Render Service 进程 | 应用进程 |
| 运行线程 | RSColorPickerThread + 主线程 + 渲染线程 | 调用方线程（同步） |
| 输入 | GPU 后端纹理（fence 同步的实时渲染表面） | `Media::PixelMap`（已解码位图） |
| 提取算法 | GPU readback + 算术平均；或 MHC/HPS 硬件路径 | 直方图/量化：最大占比色、平均色、最高饱和度色、莫兰迪色等 |
| 输出 | 单个 `ColorQuad`（对比黑/白动画，或亮度区间） | 丰富的颜色分析 API |
| 用途 | 自适应 UI 主题（对比文字/图标色）；通知应用背景亮度变化 | 图片颜色提取，用于图片编辑/沉浸式场景 |
| 复用关系 | 调用 `render_service_base/include/render/rs_color_picker.h` 的 `RSColorPicker::GetAverageColorDirect` 做像素平均 | 独立完整 |

应用进程 ColorPicker 位于 `rosen/modules/effect/color_picker/include/color_picker.h`，
继承 `ColorExtract`，提供 `GetLargestProportionColor`、`GetAverageColor` 等同步 API。
RS ColorPicker 使用的是 `render_service_base` 内部的 `RSColorPicker`（`render/rs_color_picker.h`，
继承 `RSColorExtract`），调用其静态方法 `GetAverageColorDirect` 做最终的像素均值计算。

### 策略模式

`IColorPickerManager` 定义抽象接口，两种策略在 `RSColorPickerDrawable::OnGenerate` 时按
`ColorPickStrategyType` 选择：

```
IColorPickerManager（抽象接口）
├── RSColorPickerManager        ← strategy != CLIENT_CALLBACK
│   生成黑/白对比色，带 133ms 平滑动画
│   结果通过 atomic + dirty 回调驱动渲染线程重绘
│
└── ColorPickAltManager         ← strategy == CLIENT_CALLBACK
    按亮度分类（DARK/LIGHT/NEUTRAL），区域变化时通知
    结果通过 IPC RSColorPickerCallback 命令投递到应用进程
```

### 四线程协作模型

| 线程 | 职责 |
| --- | --- |
| **主线程（RSMainThread）** | 状态机驱动（PREPARING→SCHEDULED→COLOR_PICK_THIS_FRAME），dirty 判定，发送 IPC 回调 |
| **渲染线程** | `OnDraw`：读取插值色并应用到 canvas；若 `needColorPick_` 则快照并调度异步提取 |
| **ColorPickerThread** | `ExecColorPick`：等 GPU fence → 重建纹理 → 读像素 → 计算均值 → `HandleColorUpdate` |
| **应用进程** | 接收 IPC 回调（仅 CLIENT_CALLBACK 策略） |

### RSColorPickerThread

单例线程，基于 `AppExecFwk::EventRunner`（名为 `"RSColorPickerThread"`），任务以
`EventQueue::Priority::IMMEDIATE` 投递。

关键能力：
- **独立 GPU 共享上下文**：通过 `renderContext_->CreateShareContext()`（GL）或
  `RsVulkanContext::GetSingleton().CreateDrawingContext()`（Vulkan）创建。
  启动时由 `RSMainThread::InitRenderContext` 传入渲染引擎的 RenderContext 进行初始化。
- **PostTask(task, delayTime)**：投递任务到独立线程。
- **NotifyNodeDirty(nodeId)**：通过回调（注册于 `RSMainThread`）标记节点 dirty 并请求 vsync。
- **NotifyClient(nodeId, luminance)**：通过回调通知主线程发送 IPC。

### RSColorPickerDrawable 状态机

```
PREPARING  ──ScheduleColorPickIfReady──>  SCHEDULED
SCHEDULED  ──PostColorPickerStateTask──>  COLOR_PICK_THIS_FRAME
COLOR_PICK_THIS_FRAME ──OnPrepare─────>  PREPARING
```

- `ScheduleColorPickIfReady`：根据 `params.interval` 计算冷却延迟，转换 PREPARING→SCHEDULED。
- `PostColorPickerStateTask`（在 visitor 中）：投递延迟任务到主线程，设置状态并标记 dirty；
  进入 COLOR_PICK_THIS_FRAME 时请求 vsync。
- `SetState`：校验状态转换合法性（如不能从 PREPARING 直接跳到 COLOR_PICK_THIS_FRAME）。
- `OnPrepare`：仅计算 `stagingNeedColorPick_`（状态是否为 COLOR_PICK_THIS_FRAME）
  和暗色模式变化；状态转换 COLOR_PICK_THIS_FRAME → PREPARING 由 visitor 的
  `PrepareColorPickers` 在 `PrepareColorPicker()` 返回 true 后执行。

### 完整生命周期

| 阶段 | 方法 | 线程 | 说明 |
| --- | --- | --- | --- |
| 生成 | `OnGenerate` | UI | 读 `GetColorPicker()` 参数，按策略创建 manager |
| 预备 | `OnPrepare` | 主线程 | 计算 `stagingNeedColorPick_`（状态是否为 COLOR_PICK_THIS_FRAME）和暗色模式变化 |
| 同步 | `OnSync` | UI→RT | `stagingNeedColorPick_`→`needColorPick_`，传播暗色模式 |
| 绘制 | `OnDraw` | RT | `GetColorPick()` 取插值色设到 canvas；若 `needColorPick_` 调用 `ScheduleColorPick` |
| 异步提取 | `ExecColorPick` | ColorPickerThread | 等 fence（30ms）→ 重建纹理 → ReadPixels → 均值计算 → `HandleColorUpdate` |
| 结果投递 | `HandleColorUpdate` | ColorPickerThread | CONTRAST：原子写 + NotifyNodeDirty；CLIENT_CALLBACK：NotifyClient → IPC |

### 异步提取流程（GPU fence 同步）

1. 渲染线程调用 `RSColorPickerUtils::ExtractSnapshotAndScheduleColorPick`。
2. 裁剪 canvas 到提取区域，取图像快照（`GetImageSnapshot` 或 `HveFilter::SampleLayer`）。
3. 先尝试 `RSHeteroColorPicker::Instance().GetColor()`（MHC/HPS 硬件路径），命中则直接返回。
4. 未命中则走 `ScheduleColorPickWithSemaphore`：
   - 创建 Vulkan semaphore，surface flush 时携带该 semaphore。
   - 提取 fence fd。
   - 投递任务到 ColorPickerThread：`ExecColorPick`。
5. `ExecColorPick`（仅 Vulkan）：
   - `WaitFence`（30ms 超时）。
   - `image->BuildFromTexture`（共享 GPU 上下文）。
    - `RSPropertyDrawableUtils::PickColor` → GPU 缩放 + `ReadPixels` + `GetAverageColorDirect`。
   - 调用 `manager->HandleColorUpdate(colorPicked)`。

### 结果投递

**CONTRAST 策略**（`RSColorPickerManager::HandleColorUpdate`）：

1. 将提取色转为对比色（黑/白），使用滞后阈值（HIGH=220, LOW=150）避免抖动。
2. 原子写 `prevColor_`、`colorPicked_`、`animStartTime_`。
3. `RSColorPickerThread::Instance().NotifyNodeDirty(nodeId)` → 标记节点 dirty + 请求 vsync。
4. 下帧渲染时 `GetColorPick()` 返回插值色（133ms 动画），动画期间持续 NotifyNodeDirty 驱动帧。

**CLIENT_CALLBACK 策略**（`ColorPickAltManager::HandleColorUpdate`）：

1. 计算亮度，按阈值分类（DARK/LIGHT/NEUTRAL）。
2. 区域变化时 `RSColorPickerThread::Instance().NotifyClient(nodeId, luminance)`。
3. → `RSMainThread::SendColorPickerCallback` → 构造 `RSColorPickerCallback` IPC 命令 → 应用进程。

### HWC 联动

当 picker 处于 COLOR_PICK_THIS_FRAME 状态时：
- `HandleColorPickerHwcDisable`（visitor）：记录 picker 区域。
- `UpdateHwcNodeEnableByColorPicker`（HWC visitor）：强制相交的 HWC surface 节点
  `SetHardwareNeedMakeImage`，确保被提取的内容由 GPU 合成（而非 HWC 直接送显），
  使 picker 能读取到像素。

### 特殊节点处理

- `IsColorPickerOnlyNode`：有 ColorPickerDrawable 但无实际 filter 的节点，
  在 dirty/visitor 管线中跳过 filter 缓存清除、脏区域合并和 filter 信息收集逻辑，
  避免无意义模糊工作。ColorPicker 自身的 HWC 禁用在跳过之前已执行，不受影响。
- 脱离渲染树时（`rs_render_node.cpp`）：调用 `ResetColorMemory`，
  并将最后的暗色等效模式持久化到节点属性。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 独立线程执行 GPU readback | `RSColorPickerThread` 单例 | 读像素耗时且阻塞 GPU，不能放在渲染线程 |
| 独立 GPU 共享上下文 | `CreateShareGPUContext` | ColorPickerThread 需要访问渲染线程产生的纹理，但不能共享命令流 |
| fence 同步 | `ScheduleColorPickWithSemaphore` | 确保渲染线程的 GPU 命令完成后 ColorPickerThread 才读像素 |
| 状态机冷却 | `params.interval` + PREPARING/SCHEDULED 状态 | 避免每帧都做颜色提取，降低功耗 |
| 滞后阈值 | THRESHOLD_HIGH=220, THRESHOLD_LOW=150 | 对比色在边界值附近抖动时不会频繁翻转 |
| HWC 联动 | `SetHardwareNeedMakeImage` | HWC 直接合成的内容不在 GPU 可读纹理中，需要强制 GPU 合成 |
| MHC/HPS 硬件路径优先 | `RSHeteroColorPicker::GetColor` | HPS 将统计缩小到 1×1 纹理后仍需 `BuildFromTexture` + `ReadPixels` 读取结果，减少大图 readback，功耗更低 |
| Placeholder 映射表 | `rs_color_placeholder_mapping_def.in` | 语义化占位符到具体颜色的映射支持暗色模式 |

## 待补充背景

- Placeholder 映射的完整语义和 ArkUI 使用方式
- MHC/HPS 硬件加速路径的触发条件和芯片依赖
