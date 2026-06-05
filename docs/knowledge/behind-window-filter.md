# Behind Window Filter / Dynamic Layer Skip

## 适用范围

- 后窗模糊滤镜使能控制
- 动态 Layer 跳过控制
- 全屏遮挡判断
- 渲染节点绘制内容检测

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSBehindWindowFilterManager | `rosen/modules/render_service_base/include/feature/behind_window_filter/rs_behind_window_filter_manager.h` | 后窗滤镜管理器 |
| RSDynamicLayerSkipController | `rosen/modules/render_service_base/include/feature/dynamic_layer_skip/rs_dynamic_layer_skip_controller.h` | 动态 Layer 跳过控制器 |
| LayerSkipContext | 同上文件（配套结构体） | Layer 跳过上下文 |

## 核心模型

### Behind Window Filter

`RSBehindWindowFilterManager` 为单例，提供后窗模糊滤镜的 CCM（Client Composition Manager）开关：

- `IsBehindWindowFilterEnabledByCCM`：查询 CCM 是否启用后窗滤镜
- `SetBehindWindowFilterEnabledByCCM`：设置 CCM 后窗滤镜使能
- `isBehindWindowFilterEnabledCCM_`：atomic bool，默认为 true

这是一个简单的全局开关，由 CCM 配置控制。后窗模糊是指在窗口后方应用模糊效果（如磨砂玻璃），CCM 可以动态关闭此效果以节省性能。

### Dynamic Layer Skip

`RSDynamicLayerSkipController` 提供动态 Layer 跳过判断：

1. **初始化**：`Init(screenRect, globalDisabled)` 设置屏幕矩形和全局禁用标志
2. **节点检查**：`CheckNodeDrawProperty` 检查节点绘制属性
3. **子树绘制内容标记**：`MarkParentSubTreeHasDrawContent` 标记父节点子树有绘制内容
4. **Drawable 检查**：`HasDrawContentDrawableInRange` 检查指定 Drawable 范围内是否有绘制内容
5. **节点遍历**：`VisitRenderNode` 遍历渲染节点
6. **全屏遮挡检测**：
   - `DetectScreenLayerValidity`：检测屏幕 Layer 有效性
   - `DetectScreenLayerValidityInner`：内部检测逻辑，维护 `targetArea` 和 `totalTargetCount`
   - `HasFullScreenSelfDrawingSurface`：判断是否有全屏自绘 Surface
7. **验证**：`VerifyScreenLayerValidity` 验证屏幕 Layer 有效性

`LayerSkipContext` 配套结构体：

- `screenLayerInvalid_`：屏幕 Layer 是否无效
- `relevantSurfaceNodeIds_`：相关 Surface 节点 ID 列表
- `SyncFrom`：从控制器同步状态
- `Reset`：重置状态

内部状态：

- `screenRect_`：屏幕矩形
- `globalOccluderDetected_`：全局遮挡器已检测
- `occluderInstanceRootNodeId_`：遮挡器实例根节点 ID
- `targetSelfDrawingSurface_`：目标自绘 Surface 列表
- `globalDisabled_`：全局禁用标志
- `visitedRenderNodeCount_`：已访问渲染节点计数

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 后窗滤镜 CCM 开关独立 | `RSBehindWindowFilterManager` 单例 | CCM 可动态控制后窗模糊，独立开关避免修改 UI 框架代码 |
| 后窗开关默认启用 | `isBehindWindowFilterEnabledCCM_ = true` | 后窗模糊是默认行为，CCM 可在性能不足时关闭 |
| Layer 跳过基于遮挡检测 | `DetectScreenLayerValidity` | 被全屏遮挡的 Layer 无需渲染，跳过可节省 GPU 资源 |
| 全局禁用标志 | `globalDisabled_` | 某些场景（如调试/性能测试）需要全局禁用 Layer 跳过 |
| 自绘 Surface 特殊处理 | `targetSelfDrawingSurface_` | 自绘 Surface（如视频/相机）可能不需要模糊后窗，特殊识别 |

## 待补充背景

- 后窗模糊的具体渲染实现位置
- Layer 跳过在统一渲染管线中的调用时序
- 遮挡检测的算法细节和性能开销
- `globalOccluderDetected_` 的触发条件和重置机制
- 后窗滤镜 CCM 开关的配置来源
