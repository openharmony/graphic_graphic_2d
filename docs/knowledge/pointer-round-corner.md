# Pointer Window Manager / Round Corner Display

## 适用范围

- 鼠标指针窗口管理
- 硬件光标（Hard Cursor）HWC 合成
- 圆角显示（RCD）渲染
- 圆角硬件合成 buffer 预备
- 非显示区域（Notch/刘海）处理
- TUI（Trusted UI）与指针

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSPointerWindowManager | `rosen/modules/render_service/core/feature/pointer_window_manager/rs_pointer_window_manager.h` | 指针窗口管理器 |
| RoundCornerDisplayManager | `rosen/modules/render_service/core/feature/round_corner_display/rs_round_corner_display_manager.h` | 圆角显示管理器 |
| RoundCornerDisplay | `rosen/modules/render_service/core/feature/round_corner_display/rs_round_corner_display.h` | 圆角显示核心类 |
| RSCRSurfaceRenderNode | `rosen/modules/render_service/core/feature/round_corner_display/rs_rcd_surface_render_node.h` | RCD Surface 渲染节点 |
| RSCRRenderManager | `rosen/modules/render_service/core/feature/round_corner_display/rs_rcd_render_manager.h` | RCD 渲染管理器 |
| RSCRRenderVisitor | `rosen/modules/render_service/core/feature/round_corner_display/rs_rcd_render_visitor.h` | RCD 渲染访问器 |
| RSCRRenderListener | `rosen/modules/render_service/core/feature/round_corner_display/rs_rcd_render_listener.h` | RCD 渲染监听 |
| RoundCornerConfig | `rosen/modules/render_service/core/feature/round_corner_display/rs_round_corner_config.h` | 圆角配置 |
| RSMessageBus | `rosen/modules/render_service/core/feature/round_corner_display/rs_message_bus.h` | RCD 消息总线 |
| RSAny | `rosen/modules/render_service/core/feature/round_corner_display/rs_any.h` | RCD 类型擦除容器 |

## 核心模型

### Pointer Window Manager

`RSPointerWindowManager` 为单例，管理鼠标指针窗口：

1. **脏区域传播**：`UpdatePointerDirtyToGlobalDirty` 将指针窗口的脏区域传播到全局脏区域
2. **硬件光标**：
   - `CollectAllHardCursor`：收集所有硬件光标节点
   - `UpdateHardCursorStatus`：更新硬件光标状态
   - `HardCursorCreateLayer` / `HardCursorCreateLayerForDirect`：创建硬件光标 Layer
   - `CheckHardCursorSupport`：检查是否支持硬件光标
   - `CheckHardCursorValid`：检查硬件光标有效性
3. **指针信息**：
   - `SetHwcNodeBounds`：设置 HWC 节点边界
   - `SetHardCursorNodeInfo`：设置硬件光标节点信息
   - `isPointerEnableHwc_`：指针 HWC 使能（atomic）
   - `isPointerCanSkipFrame_`：指针是否可跳帧（atomic）
   - `isTuiEnabled_`：TUI 使能标志
4. **Bound 参数**：`BoundParam`（x, y, z, w）描述指针位置和大小
5. **多屏**：`IsPointerInvisibleInMultiScreen` 判断指针在多屏场景下是否不可见

### Round Corner Display

`RoundCornerDisplayManager` 管理圆角显示渲染：

1. **Layer 管理**：
   - `AddLayer`：添加 RCD Layer（TOP / BOTTOM）
   - `CheckLayerIsRCD`：检查是否为 RCD Layer
   - `GetLayerPair`：获取 Layer 信息
2. **渲染**：
   - `DrawRoundCorner`：绘制圆角（软绘）
   - `DrawTopRoundCorner` / `DrawBottomRoundCorner`：分别绘制上下圆角
3. **参数更新**：
   - `UpdateDisplayParameter`：更新显示宽高
   - `UpdateNotchStatus`：更新刘海状态
   - `UpdateOrientationStatus`：更新旋转方向
   - `UpdateHardwareResourcePrepared`：更新硬件资源准备状态
4. **硬件合成**：
   - `RunHardwareTask`：运行硬件合成 buffer 预备任务
   - `GetHardwareInfo`：获取硬件信息（`RoundCornerHardware`）
5. **脏区域**：`HandleRoundCornerDirtyRect` 处理圆角脏区域

Layer 类型枚举 `RCDLayerType`：INVALID / TOP / BOTTOM。

`CheckRcdRenderEnable`：检查 `ScreenState::HDI_OUTPUT_ENABLE` 才启用 RCD。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 硬件光标独立于指针窗口 | `CollectAllHardCursor` / `hardCursorNodeMap_` | 硬件光标由 HWC 合成，处理逻辑与软件指针不同 |
| 指针可跳帧 | `isPointerCanSkipFrame_` | 指针窗口在低帧率场景可跳帧以节省功耗 |
| Bound 使用 float x/y/z/w | `BoundParam` | HWC 需要 float 精度的位置信息，避免整数截断 |
| RCD Layer 分 TOP/BOTTOM | `RCDLayerType` | 圆角分上下两部分，可能分别由不同硬件 Layer 合成 |
| RCD 独立消息总线 | `RSMessageBus` | RCD 硬件合成任务与渲染线程通信需要异步消息 |
| RCD 使能检查屏幕状态 | `CheckRcdRenderEnable` | 只有 HDI 输出使能时才需要 RCD，避免无效渲染 |

## 待补充背景

- 硬件光标在多屏场景下的完整处理流程
- `RoundCornerHardware` 的完整结构
- RCD 硬件合成 buffer 的预备时序
- 刘海状态对圆角渲染的影响
- TUI 与指针窗口的交互逻辑
