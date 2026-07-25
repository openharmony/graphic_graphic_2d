# Pointer Window Manager

## 适用范围

- 鼠标指针窗口管理
- 硬件光标（Hard Cursor）HWC 合成
- TUI（Trusted UI）与指针

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSPointerWindowManager | `rosen/modules/render_service/core/feature/pointer_window_manager/rs_pointer_window_manager.h` | 指针窗口管理器 |

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

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 硬件光标独立于指针窗口 | `CollectAllHardCursor` / `hardCursorNodeMap_` | 硬件光标由 HWC 合成，处理逻辑与软件指针不同 |
| 指针可跳帧 | `isPointerCanSkipFrame_` | 指针窗口在低帧率场景可跳帧以节省功耗 |
| Bound 使用 float x/y/z/w | `BoundParam` | HWC 需要 float 精度的位置信息，避免整数截断 |

## 待补充背景

- 硬件光标在多屏场景下的完整处理流程
- TUI 与指针窗口的交互逻辑