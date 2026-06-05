# 客户端 UI 节点

## 适用范围

- RSNode 及其派生类（RSCanvasNode、RSSurfaceNode、RSDisplayNode 等）的创建、属性设置和树操作
- 客户端节点树构建与事务提交
- RSUIDirector 帧驱动与节点树同步
- RSUIContext 多实例支持
- DrawOnNode 自定义绘制
- 隐式/显式动画在节点上的使用

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 基础节点 | `rosen/modules/render_service_client/core/ui/rs_node.h/.cpp` | RSNode：属性设置、树操作、动画接口 |
| Surface 节点 | `rosen/modules/render_service_client/core/ui/rs_surface_node.h/.cpp` | RSSurfaceNode：窗口 Surface 绑定、Buffer 管理 |
| Canvas 节点 | `rosen/modules/render_service_client/core/ui/rs_canvas_node.h/.cpp` | RSCanvasNode：自定义绘制录制 |
| Canvas 绘制节点 | `rs_canvas_drawing_node.h/.cpp` | RSCanvasDrawingNode：直接绘制模式 |
| Display 节点 | `rs_display_node.h/.cpp` | RSDisplayNode：显示节点 |
| Root 节点 | `rs_root_node.h/.cpp` | RSRootNode：根节点 |
| Effect 节点 | `rs_effect_node.h/.cpp` | RSEffectNode：效果节点 |
| Proxy 节点 | `rs_proxy_node.h/.cpp` | RSProxyNode：代理节点 |
| Depth 节点 | `rs_depth_node.h/.cpp` | RSDepthNode：深度节点 |
| Union 节点 | `rs_union_node.h/.cpp` | RSUnionNode：联合节点 |
| UI 控制器 | `rs_ui_director.h/.cpp` | RSUIDirector：帧驱动、节点树管理 |
| UI 上下文 | `rs_ui_context.h/.cpp` | RSUIContext：多实例上下文 |
| 上下文管理器 | `rs_ui_context_manager.h/.cpp` | RSUIContextManager：多实例注册与查找 |
| 属性提取器 | `rosen/modules/render_service_client/core/modifier/rs_modifier_extractor.h` | RSModifierExtractor：staging 属性读取 |
| 属性冻结器 | `rs_showing_properties_freezer.h` | RSShowingPropertiesFreezer：showing 属性只读访问 |

## 核心模型

### 节点继承体系

```
RSNode (enable_shared_from_this)
  ├── RSCanvasNode       -- 自定义绘制（录制模式）
  ├── RSCanvasDrawingNode -- 自定义绘制（直接模式）
  ├── RSSurfaceNode      -- 窗口 Surface，绑定 Consumer/Producer
  ├── RSDisplayNode      -- 显示输出
  ├── RSRootNode         -- 根节点
  ├── RSEffectNode       -- 效果容器
  ├── RSDepthNode        -- 深度排序
  ├── RSProxyNode        -- 代理/跨进程节点
  └── RSUnionNode        -- 联合节点
```

每个节点有 `RSUINodeType` 枚举标识类型，支持 `IsInstanceOf<T>()` + `ReinterpretCastTo<T>()` 安全转型。

### 属性设置与 Modifier

RSNode 的属性通过 ModifierNG 体系设置：

1. `SetBounds()` / `SetFrame()` 等属性设置方法最终调用 ModifierNG 的 `Setter()`。
2. Modifier 持有 `RSProperty<T>` 对象，属性变更时 MarkDirty。
3. Dirty Modifier 通过 Transaction 传递到服务端。

### 树操作

- `AddChild()` / `RemoveChild()` / `MoveChild()` — 标准树操作，生成对应 RSCommand。
- `AddCrossParentChild()` — 跨父节点添加（窗口跨屏场景）。
- `AddCrossScreenChild()` — PC 扩展屏场景，支持 `autoClearCloneNode`。
- `RemoveFromTree()` — 从树中移除，可能触发 Transition 动画。

### 事务提交

RSNode 的属性变更不会立即同步到服务端，而是在 RSUIDirector 帧末尾统一提交：

```
RSUIDirector::ProcessDirent()
  → 收集所有 dirty Modifier
  → 通过 RSTransactionProxy 发送 RSTransactionData 到 RenderService
```

### RSSurfaceNode 特有

- `RSSurfaceNodeConfig`：节点名、SurfaceId、同步模式、窗口类型等配置。
- `CreateSurfaceNode()` 工厂方法。
- 绑定 `IConsumerSurface` 用于接收应用侧 buffer。
- `SetTakeSurfaceForUIFlag()` 标记 UI 侧取 Surface。

### RSCanvasNode 特有

- `BeginRecording()` / `FinishRecording()` — 录制绘制命令到 CmdList。
- `DrawOnNode()` — 在指定 Modifier 类型上绘制自定义内容。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| enable_shared_from_this | `RSNode : public std::enable_shared_from_this<RSNode>` | 节点树使用 shared_ptr 管理，需要安全获取自身指针 |
| RSUINodeType + IsInstanceOf | 枚举类型 + 模板转型 | 替代 dynamic_cast，编译期确定类型，无 RTTI 开销 |
| 属性设置走 Modifier | `SetBounds()` → ModifierNG::Setter | 统一属性变更通道，支持动画、去重、脏标记 |
| 跨父节点子节点 | `AddCrossParentChild()` | 窗口跨屏场景，一个子节点挂载到多个父节点 |
| Freeze 机制 | `SetFreeze()` | 冻结节点内容，避免无效重绘，节省 GPU 开销 |
| RSUIContext 多实例 | `RSUIContext` + `RSUIContextManager` | 多窗口/多实例场景下隔离节点树和动画状态 |

## 待补充背景

- RSProxyNode 的跨进程代理机制和生命周期。
- RSCanvasDrawingNode 与 RSCanvasNode 的具体差异和使用场景。
- RSUnionNode 的设计意图和使用场景。
- RSUIDirector 的帧驱动与 VSync 的关系。
- Freeze 机制在 UniRender 下的具体生效条件。
