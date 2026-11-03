# Pointer Window Manager

## 适用范围

改动涉及以下任一内容时，必须先读本文：

- 鼠标指针窗口管理
- 硬件光标（Hard Cursor）HWC 合成与 Layer 创建
- 指针窗口脏区域传播与强制提交
- 多屏场景下指针可见性判断
- 指针 Bound 更新与 HWC 节点属性同步
- TUI（Trusted UI）与硬件光标互斥（仅 ROG 场景关闭硬光标，非 ROG 场景不关闭）
- 指针跳帧优化与帧提交决策
- `rosen/modules/render_service/core/feature/pointer_window_manager/` 内部实现

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| RSPointerWindowManager | `rosen/modules/render_service/core/feature/pointer_window_manager/rs_pointer_window_manager.h` | 光标，硬光标 |
| RSPointerWindowManager | `rosen/modules/render_service/core/feature/pointer_window_manager/rs_pointer_window_manager.cpp` | 全部业务逻辑 |
| RSUniHwcComputeUtil | `rosen/modules/render_service/core/feature/hwc/rs_uni_hwc_compute_util.h` | UpdateHwcNodeProperty / GetLayerTransform |
| RSRenderThreadParams | `rosen/modules/render_service/core/pipeline/render_thread/rs_render_thread_params.h` | HardCursorDrawables 传递到渲染线程 |

## 框架通路

### 每帧处理主流程

`RSPointerWindowManager` 为单例，由主线程和渲染线程协作驱动，每帧处理顺序如下：

```
主线程 OnVsync → mainLoop_():
  1. UpdateHardCursorStatus(hardCursorNode, screenNode)     // ← 硬件光标支持判断
  2. CollectAllHardCursor(hardCursorNode, screenNode, displayNode)  // ← 收集 Drawable
  3. UpdatePointerDirtyToGlobalDirty(pointerWindow, screenNode)     // ← 脏区域传播
  4. IsPointerInvisibleInMultiScreen()                       // ← 多屏可见性

渲染线程：
  5. HardCursorCreateLayer(processor, screenNodeId)          // ← Layer 创建
  6. GetHardCursorNeedCommit(screenNodeId)                   // ← 帧提交决策
  7. ResetHardCursorDrawables()                              // ← 帧末清理
```

### 硬件光标状态更新

```
TUI 使能标志由 HWC 事件回调更新（非 Render Service 内部决策）：
  RSUniHwcEventManager::OnHwcEvent
    → HWCEVENT_TUI_ENTER：仅 ROG 场景调用 SetTuiEnabled(true)
    → HWCEVENT_TUI_EXIT：SetTuiEnabled(false)

UpdateHardCursorStatus(hardCursorNode, screenNode)
  → 判断 hardCursorNode.IsHardwareEnabledTopSurface()
  → CheckHardCursorSupport(screenNode)
      = screenNode->GetScreenProperty().IsHardCursorSupport() && !IsTuiEnabled()
  → hardCursorNode.SetHardCursorStatus(result)
```

### 硬件光标 Drawable 收集

```
CollectAllHardCursor(hardCursorNode, screenNode, displayNode)
  → 前置条件：IsHardwareEnabledTopSurface() && (ShouldPaint() || GetHardCursorStatus())
  → 获取 hardCursorNode.GetRenderDrawable()
  → 存入 hardCursorDrawableVec_: (screenNodeId, displayNodeId, drawable)
```

### 脏区域传播

```
UpdatePointerDirtyToGlobalDirty(pointerWindow, screenNode)
  → 获取 pointerWindow->GetDirtyManager()
  → 若硬件光标状态变化（当前 true，上一帧 false）：
      → 合并上一帧 Surface 位置到 screenNode->GetDirtyManager()
  → 获取 pointerWindow 当前帧脏区域
  → 若脏区域非空：
      → 合并到 screenNode HWC 脏区域
      → 清空指针脏区域
      → isNeedForceCommitByPointer_ = true
  → 否则：
      → isNeedForceCommitByPointer_ = false
```

### 多屏可见性判断

```
IsPointerInvisibleInMultiScreen()
  → isPointerInvisible = true
  → TraverseScreenNodes:
      → screenProperty.GetTypeBlackList().empty()
      → (node->IsMirrorScreen() || screenProperty.IsVirtual())
      → 若同时满足：isPointerInvisible = false，提前退出
  → 返回 isPointerInvisible
```

### 渲染线程 Layer 创建

```
HardCursorCreateLayer(processor, screenNodeId)
  → GetHardCursorDrawable(screenNodeId) 查找 drawable
  → processor->CreateLayerForRenderThread(drawable)
  → 记录 hardCursorCommitResultMap_[screenNodeId] = surfaceParams->GetLayerCreated()
```

### 帧提交决策

```
GetHardCursorNeedCommit(screenNodeId)
  → 获取 drawable，判断当前帧是否有 buffer
  → 查找 hardCursorCommitResultMap_ 中上一帧提交结果
  → 返回 hasBuffer != lastCommitResult（buffer 状态变化时需提交）
```

### Bound 更新与 HWC 属性同步

```
SetHwcNodeBounds(rsNodeId, x, y, z, w)  ← IPC 线程调用
  → lock_guard(mtx_)：记录 bound_ 和 rsNodeId_
  → SetBoundHasUpdate(true)

UpdatePointerInfo()  ← 主线程调用
  → CAS boundHasUpdate_ true→false，失败则提前返回
  → lock_guard(mtx_)：拷贝 rsNodeId_ 和 bound_ 到局部变量
  → 获取 RenderNode，设置 Bounds，标记 dirty，ApplyModifiers
  → RSUniHwcComputeUtil::UpdateHwcNodeProperty(surfaceNode)
  → GetLayerTransform → UpdateHwcNodeLayerInfo
```

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 硬件光标独立于指针窗口 | `CollectAllHardCursor` / `hardCursorNodeMap_` | 硬件光标由 HWC 合成，处理逻辑与软件指针不同 |
| 指针可跳帧 | `isPointerCanSkipFrame_`（atomic） | 低帧率场景可跳帧以节省功耗 |
| Bound 使用 float x/y/z/w | `BoundParam` | HWC 需要 float 精度位置信息，避免整数截断 |
| 脏区域变化时强制提交 | `isNeedForceCommitByPointer_` | 硬件光标状态切换或指针移动时必须送显 |
| 帧提交按 buffer 状态变化判断 | `GetHardCursorNeedCommit` | buffer 状态未变时跳过提交，减少无效合成 |
| Bound 更新用 CAS 保护 | `BoundHasUpdateCompareChange` | 避免多线程重复处理同一 Bound 更新 |
| TUI 与硬件光标互斥仅限 ROG 场景 | HWC 事件回调中仅 ROG 场景才 `SetTuiEnabled(true)`；`CheckHardCursorSupport` 仍为 `!IsTuiEnabled()` | 非 ROG 场景 TUI 使能时不关闭硬光标；ROG 场景下 TUI 使能时仍关闭硬光标 |

## 逐屏状态管理

| 成员 | 类型 | 生命周期 | 说明 |
| --- | --- | --- | --- |
| `hardCursorDrawableVec_` | `vector<tuple<screenNodeId, displayNodeId, drawable>>` | 帧末 `ResetHardCursorDrawables()` 清空 | 当前帧各屏的硬件光标 Drawable |
| `hardCursorCommitResultMap_` | `unordered_map<screenNodeId, bool>` | `RemoveCommitResult(screenNodeId)` 移除 | 各屏上一帧 Layer 创建结果，用于帧提交决策 |
| `hardCursorNodeMap_` | `map<nodeId, RSSurfaceRenderNode>` | 帧末 `ResetHardCursorDrawables()` 清空 | 全局硬件光标节点，用于 Direct 模式 Layer 创建 |

## 线程安全

| 成员 | 保护方式 | 说明 |
| --- | --- | --- |
| `mtx_` | `std::mutex` | 保护 Bound 更新和 rsNodeId |
| `isPointerEnableHwc_` | `std::atomic<bool>` | HWC 使能标志 |
| `isPointerCanSkipFrame_` | `std::atomic<bool>` | 跳帧标志，CAS 更新 |
| `boundHasUpdate_` | `std::atomic<bool>` | Bound 更新标志，CAS 更新 |
| `hardCursorDrawableVec_` | 无显式保护 | 帧末清空，仅渲染线程访问 |
| `hardCursorCommitResultMap_` | 无显式保护 | 仅渲染线程访问 |
| `hardCursorNodeMap_` | 无显式保护 | 主线程更新 |

## 优化规则

### 硬件光标帧提交优化

硬件光标 buffer 状态未变时跳过提交：

1. `GetHardCursorNeedCommit` 比较当前帧 buffer 有无与上一帧提交结果
2. `hasBuffer != lastCommitResult` 时才返回 true，触发提交
3. `isPointerCanSkipFrame_` 使用 `compare_exchange_weak` 原子更新，避免多线程竞争

### 脏区域传播优化

仅在硬件光标实际移动或状态变化时传播脏区域：

1. 硬件光标状态切换（visible ↔ invisible）时，合并上一帧位置到屏幕脏区域
2. 指针脏区域非空时才设置 `isNeedForceCommitByPointer_`
3. 脏区域为空时清空强制提交标志

### 多屏可见性优化

遍历所有屏幕时发现任一可见屏幕即提前退出：

1. 初始化 `isPointerInvisible = true`
2. 遍历屏幕节点，满足 `typeBlackList.empty() && (IsMirrorScreen() || IsVirtual())` 时标记可见并退出
3. 避免遍历全部屏幕节点

### Bound 更新优化

仅在有实际更新时处理 Bound：

1. `SetHwcNodeBounds`（IPC 线程）记录数据并设置 `boundHasUpdate_`
2. `UpdatePointerInfo`（主线程）CAS 消费更新标志，无更新则提前返回
3. 拷贝数据到局部变量后释放锁，再执行节点属性更新

### Direct 模式 Layer 创建优化

`HardCursorCreateLayerForDirect` 中跳过已消费 buffer 的节点：

1. 检查 `IsCurrentFrameBufferConsumed()`
2. 未消费且存在 preBuffer 时，将 preBuffer 置空并加入 pending sync
3. 避免重复使用已消费的 buffer

## 硬件光标有效性校验

`CheckHardCursorValid` 检查硬件光标 Layer 尺寸下限：

- DSS 硬件不支持长宽 ≤ 2 的 Layer 合成
- `srcRect` 宽或高 ≤ `MIN_LAYER_WIDTH`（2）时记录错误日志
- 日志包含 buffer 尺寸和 bounds 尺寸，便于定位截断问题

## 集成点

| 线程 | 方法 | 说明 |
| --- | --- | --- |
| IPC 线程 | `SetHwcNodeBounds` | 记录 Bound 数据，mutex 保护 |
| 主线程 | `UpdateHardCursorStatus` | 按屏幕属性设置硬件光标状态 |
| 主线程 | `CollectAllHardCursor` | 收集各屏 Drawable 到 vec |
| 主线程 | `UpdatePointerDirtyToGlobalDirty` | 脏区域传播到屏幕 |
| 主线程 | `IsPointerInvisibleInMultiScreen` | 多屏可见性判断 |
| 主线程 | `UpdatePointerInfo` | Bound 更新与 HWC 属性同步 |
| 渲染线程 | `HardCursorCreateLayer` | 按 screenNodeId 创建 Layer |
| 渲染线程 | `HardCursorCreateLayerForDirect` | Direct 模式遍历 nodeMap 创建 Layer |
| 渲染线程 | `GetHardCursorNeedCommit` | 帧提交决策 |
| 渲染线程 | `ResetHardCursorDrawables` | 帧末清空 vec 和 nodeMap |
