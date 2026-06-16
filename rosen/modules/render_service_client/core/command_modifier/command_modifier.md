# command_modifier 目录说明

## 概述

本目录实现了 Render Service 客户端的 **命令型 Modifier（CmdModifier）** 框架。当应用在前台时，通过 `SetRSCmdProperty` / `SetRSCmdPropertyWithResult` 发送 command 来更新节点属性。推前台时，系统会遍历所有 RSCmdModifier 来恢复节点属性。

## 整体架构

```
RSCmdModifier（基类）
├── RSNode 系列
│   ├── NodeNameCmdModifier
│   ├── OcclusionCullingStatusCmdModifier
│   ├── IsP3ColorCmdModifier
│   ├── DrawRegionCmdModifier
│   ├── UseCmdlistDrawRegionCmdModifier
│   ├── NodeGroupCmdModifier
│   ├── ExcludeNodeGroupCmdModifier
│   ├── MarkNodeSingleFrameComposerCmdModifier
│   ├── IsRepaintBoundaryCmdModifier
│   ├── MarkOpincNodeCmdModifier
│   ├── IsUifirstNodeCmdModifier
│   ├── IsForceUifirstNodeCmdModifier
│   ├── SyncDrawNodeTypeCmdModifier
│   ├── UIFirstSwitchCmdModifier
│   ├── OutOfParentCmdModifier
│   └── IsCrossNodeCmdModifier
├── RSCanvasNode 系列
│   ├── HdrPresentCmdModifier
│   ├── ColorGamutCmdModifier
│   ├── IsFreezeCmdModifier
│   ├── ClearRecordingCmdModifier
│   ├── FinishRecordCmdModifier
│   └── DrawOnNodeCmdModifier
├── RSSurfaceNode 系列（数量最多）
│   ├── LeashPersistentIdCmdModifier
│   ├── SecurityLayerCmdModifier
│   ├── SkipLayerCmdModifier
│   ├── SnapshotSkipLayerCmdModifier
│   ├── HasFingerprintCmdModifier
│   ├── ColorSpaceCmdModifier
│   ├── AbilityBGAlphaCmdModifier
│   ├── NotifyUIBufferAvailableCmdModifier
│   ├── ContainerWindowCmdModifier
│   ├── FreezeCmdModifier
│   ├── HardwareEnabledCmdModifier
│   ├── HardwareEnableHintCmdModifier
│   ├── BootAnimationCmdModifier
│   ├── GlobalPositionEnabledCmdModifier
│   ├── ClonedNodeInfoCmdModifier
│   ├── ForceUIFirstCmdModifier
│   ├── AncoFlagsCmdModifier
│   ├── SkipDrawCmdModifier
│   ├── WatermarkEnabledCmdModifier
│   ├── AbilityStateCmdModifier
│   ├── HidePrivacyContentCmdModifier
│   ├── ApiCompatibleVersionCmdModifier
│   ├── VirtualDisplayIdCmdModifier
│   ├── AttachToWindowContainerCmdModifier
│   ├── RegionToBeMagnifiedCmdModifier
│   ├── DetachFromWindowContainerCmdModifier
│   ├── CompositeLayerCmdModifier
│   ├── StaticCachedCmdModifier
│   ├── FrameGravityNewVersionEnabledCmdModifier
│   ├── SurfaceBufferOpaqueCmdModifier
│   ├── ContainerWindowTransparentCmdModifier
│   ├── BufferAvailableCallbackCmdModifier
│   └── SurfaceDefaultSizeCmdModifier
├── RSDepthNode 系列
│   └── DepthSpaceTypeCmdModifier
├── RSEffectNode 系列
│   └── PreFreezeCmdModifier
├── RSRootNode 系列
│   └── AttachRootNodeCmdModifier
└── RSWindowKeyFrameNode 系列
    ├── WkfIsFreezeCmdModifier
    └── WkfLinkedNodeIdCmdModifier
```

## 核心类与接口

### RSCmdModifier（基类）

| 文件 | `rs_command_modifier.h` / `.cpp` |
|---|---|
| 命名空间 | `OHOS::Rosen` |
| 继承 | `std::enable_shared_from_this<RSCmdModifier>` |

**关键接口：**

| 方法 | 说明 |
|---|---|
| `UpdateToRender()` | 纯虚函数，子类实现。**必须**使用 `AddCommand` 将 `RSCommand` 添加到事务队列；前台恢复时会被遍历调用。调用处使用 `SetRSCmdProperty` |
| `UpdateToRenderWithResult()` | 带返回值的版本，返回类型为 `std::variant<bool, RSInterfaceErrorCode>`。用于特殊 IPC 适配（如 `RSRenderInterface`）。基类默认调用 `UpdateToRender()` 并返回 `false`。调用处使用 `SetRSCmdPropertyWithResult` |
| `GetType()` | 纯虚函数，返回 `RSCmdModifierType` 枚举值 |
| `DumpParam(std::string&)` | 虚函数，将参数信息序列化到字符串，用于调试 dump |
| `AddCommand(...)` | 受保护方法，将 `RSCommand` 添加到节点的事务命令队列 |

### RSCmdModifierType 枚举

定义了所有命令 Modifier 的类型标识，用于注册、查找和序列化。按节点类型分组：

- **RSNode 通用**：`NODE_NAME` ~ `IS_CROSS_NODE`
- **RSCanvasNode**：`HDR_PRESENT` ~ `DRAW_ON_NODE`
- **RSSurfaceNode**：`LEASH_PERSISTENT_ID` ~ `UPDATE_SURFACE_DEFAULT_SIZE`
- **RSDepthNode**：`SET_DEPTH_SPACE_TYPE`
- **RSEffectNode**：`PRE_FREEZE`
- **RSRootNode**：`ATTACH_ROOT_NODE`
- **RSWindowKeyFrameNode**：`WKF_IS_FREEZE`、`WKF_LINKED_NODE_ID`

## 设计模式

每个具体的 CmdModifier 都遵循统一的设计模式：

1. **参数结构体（Param）**：以 `XxxCmdParam` 命名，存储该 Modifier 所需的参数。
2. **Modifier 类**：以 `XxxCmdModifier` 命名，继承 `RSCmdModifier`，包含：
   - 构造函数接收 `weak_ptr<RSNode>` 和参数结构体
   - `SetParam()` 方法：比较新旧参数，相同则不更新（减少冗余命令）
   - `UpdateToRender()`：**必须**通过 `AddCommand` 提交 `RSCommand`，前台恢复时被遍历调用
   - `UpdateToRenderWithResult()`：可选重写，用于特殊适配（如 IPC 接口），参见下文"两种 Update 路径与开发范式"
   - `DumpParam()`：将参数格式化为字符串用于调试
   - `GetParam()`：返回参数的只读引用

## 各文件说明

### 基类文件

| 文件 | 说明 |
|---|---|
| `rs_command_modifier.h/.cpp` | 基类 `RSCmdModifier`，定义核心接口和 `AddCommand` 辅助方法 |

### RSNode 通用 Modifier

| 文件 | 说明 |
|---|---|
| `rs_node_command_modifier.h/.cpp` | 定义了 RSNode 层面通用的 15 个 CmdModifier，包括节点名称、遮挡剔除、颜色空间标记、绘制区域、节点分组、重绘边界、OPINC 标记、UIFirst 标记、绘制节点类型、越界父节点、跨节点标记等 |

### RSCanvasNode Modifier

| 文件 | 说明 |
|---|---|
| `rs_canvas_node_command_modifier.h/.cpp` | 定义了画布节点专用的 6 个 CmdModifier：HDR 显示、色域设置、冻结状态、清除录制、完成录制、在节点上绘制 |

### RSSurfaceNode Modifier

| 文件 | 说明 |
|---|---|
| `rs_surface_node_command_modifier.h/.cpp` | 定义了 Surface 节点专用的 30+ 个 CmdModifier，涵盖安全层、跳过层、指纹、颜色空间、Ability 背景透明度、缓冲区可用通知、容器窗口、冻结、硬件加速、开机动画、全局位置、克隆节点、ANCO 标志、水印、隐私内容隐藏、API 兼容版本、虚拟屏幕、窗口容器挂载/摘离、放大区域、复合图层、静态缓存、Surface 默认尺寸等。此外还定义了 `RSBufferAvailableCallbackWrapper`，用于将 `std::function<void()>` 包装为 IPC 回调 |

### RSDepthNode Modifier

| 文件 | 说明 |
|---|---|
| `rs_depth_node_command_modifier.h/.cpp` | 定义了深度空间类型 `DepthSpaceTypeCmdModifier`，`UpdateToRender()` 尚未实现（标记 To be implemented） |

### RSEffectNode Modifier

| 文件 | 说明 |
|---|---|
| `rs_effect_node_command_modifier.h/.cpp` | 定义了预冻结 `PreFreezeCmdModifier`，用于提前标记节点冻结状态 |

### RSRootNode Modifier

| 文件 | 说明 |
|---|---|
| `rs_root_node_command_modifier.h/.cpp` | 定义了根节点挂载 `AttachRootNodeCmdModifier`，将 RootNode 挂载到统一渲染的 SurfaceNode 上 |

### RSWindowKeyFrameNode Modifier

| 文件 | 说明 |
|---|---|
| `rs_window_keyframe_node_command_modifier.h/.cpp` | 定义了窗口关键帧节点专用的 2 个 CmdModifier：冻结状态和关联节点 ID |

## 数据流

```
应用层修改节点属性
       ↓
CmdModifier.SetParam() 更新参数
       ↓
CmdModifier.UpdateToRender() 创建 RSCommand
       ↓
RSCmdModifier.AddCommand() 添加到节点命令队列
       ↓
Transaction IPC 发送至 Render Service
       ↓
Render Service 服务端处理命令并更新渲染状态
```

## 两种 Update 路径与调用关系

`RSCmdModifier` 提供两个更新接口，由 `RSNode` 上的两个模板函数分别调用：

```
RSNode::SetRSCmdProperty<Modifier>          →  modifier->UpdateToRender()
RSNode::SetRSCmdPropertyWithResult<Modifier> →  modifier->UpdateToRenderWithResult()
```

- **`UpdateToRender()`**：纯虚函数，**必须**使用 `AddCommand` 将 `RSCommand` 添加到事务命令队列。
  前台恢复时系统会遍历所有 CmdModifier 调用此方法重建节点状态。

- **`UpdateToRenderWithResult()`**：基类默认实现为调用 `UpdateToRender()` 并返回 `false`。
  对于一些特殊接口（如需要使用 IPC 直接调用 `RSRenderInterface` 等），子类可重写此方法进行适配，
  调用处使用 `SetRSCmdPropertyWithResult`。

---

### 开发范式一：标准 AddCommand 路径

绝大多数 CmdModifier 只需实现 `UpdateToRender()`，通过 `AddCommand` 提交命令。调用处使用
`SetRSCmdProperty`。

以 `NodeGroupCmdModifier` 为例：

**1. 在头文件中定义参数结构体和 Modifier 类**

```cpp
// rs_node_command_modifier.h

struct NodeGroupCmdParam {
    bool isNodeGroup_;
    bool nodeGroupIsForced_;
    bool nodeGroupIncludeProperty_;
};

class NodeGroupCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::MARK_NODE_GROUP;

    NodeGroupCmdModifier(std::weak_ptr<RSNode> node, const NodeGroupCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const NodeGroupCmdParam& param)
    {
        if (param_.isNodeGroup_ == param.isNodeGroup_ &&
            param_.nodeGroupIsForced_ == param.nodeGroupIsForced_ &&
            param_.nodeGroupIncludeProperty_ == param.nodeGroupIncludeProperty_) {
            return true;
        }
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    void DumpParam(std::string& out) const override;

    const NodeGroupCmdParam& GetParam() const
    {
        return param_;
    }

private:
    NodeGroupCmdParam param_;
};
```

**2. 在 cpp 文件中实现 UpdateToRender()**

```cpp
// rs_node_command_modifier.cpp

void NodeGroupCmdModifier::UpdateToRender()
{
    auto node = GetNode();
    if (!node) return;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkNodeGroup>(
        node->GetId(), param_.isNodeGroup_, param_.nodeGroupIsForced_, param_.nodeGroupIncludeProperty_);
    AddCommand(command, node->IsRenderServiceNode());  // 必须通过 AddCommand 提交
}

void NodeGroupCmdModifier::DumpParam(std::string& out) const
{
    out += "{isNodeGroup:" + std::string(param_.isNodeGroup_ ? "true" : "false") +
           ", nodeGroupIsForced:" + std::string(param_.nodeGroupIsForced_ ? "true" : "false") +
           ", nodeGroupIncludeProperty:" + std::string(param_.nodeGroupIncludeProperty_ ? "true" : "false") + "}";
}
```

**3. 在节点类中通过 SetRSCmdProperty 调用**

```cpp
// rs_node.cpp

SetRSCmdProperty<NodeGroupCmdModifier>(NodeGroupCmdParam{
    isNodeGroup, nodeGroupIsForced, nodeGroupIncludeProperty});
```

**要点：**
- `UpdateToRender()` 中**必须**通过 `AddCommand()` 提交命令
- 命令会随 Transaction 批量发送至 Render Service，保证时序正确
- 前台恢复时系统会遍历所有 CmdModifier 调用此方法，因此它承担了状态重建的职责
- 调用处使用 `SetRSCmdProperty`

---

### 开发范式二：特殊 IPC 适配路径

对于一些特殊接口（如直接使用 `RSRenderInterface` 的 IPC 方法、需要同步返回值、注册回调等），
可以重写 `UpdateToRenderWithResult()` 进行适配。调用处使用 `SetRSCmdPropertyWithResult`。

以 `HardwareEnabledCmdModifier` 为例：

**1. 在头文件中额外声明 UpdateToRenderWithResult**

```cpp
// rs_surface_node_command_modifier.h

struct HardwareEnabledCmdParam {
    bool isHardwareEnabled_;
    SelfDrawingNodeType isSelfDrawingNodeType_;
    bool isDynamicHardwareEnable_;
};

class HardwareEnabledCmdModifier : public RSCmdModifier {
public:
    static inline constexpr auto Type = RSCmdModifierType::HARDWARE_ENABLED;

    HardwareEnabledCmdModifier(std::weak_ptr<RSNode> node, const HardwareEnabledCmdParam& param)
        : RSCmdModifier(std::move(node)), param_(param)
    {
    }

    RSCmdModifierType GetType() const override
    {
        return Type;
    }

    bool SetParam(const HardwareEnabledCmdParam& param)
    {
        if (param_.isHardwareEnabled_ == param.isHardwareEnabled_ &&
            param_.isSelfDrawingNodeType_ == param.isSelfDrawingNodeType_ &&
            param_.isDynamicHardwareEnable_ == param.isDynamicHardwareEnable_) {
            return true;
        }
        param_ = param;
        return true;
    }

    void UpdateToRender() override;

    RSCmdModifier::UpdateResult UpdateToRenderWithResult() override;  // 额外声明

    void DumpParam(std::string& out) const override;

    const HardwareEnabledCmdParam& GetParam() const
    {
        return param_;
    }

private:
    HardwareEnabledCmdParam param_;
};
```

**2. 在 cpp 文件中实现**

```cpp
// rs_surface_node_command_modifier.cpp

// UpdateToRender() 必须使用 AddCommand 提交 command，用于推前台时恢复节点属性。
// 如果没有对应的 command，需要新增 command。
void HardwareEnabledCmdModifier::UpdateToRender()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) return;
    // TODO: 需要通过 AddCommand 提交对应的 command
    // 示例：
    // std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetHardwareEnabled>(
    //     node->GetId(), param_.isHardwareEnabled_, param_.isSelfDrawingNodeType_, param_.isDynamicHardwareEnable_);
    // AddCommand(command, true);
}

// UpdateToRenderWithResult() 用于特殊 IPC 适配，如直接调用 RSRenderInterface 接口。
// 调用处通过 SetRSCmdPropertyWithResult 触发。
RSCmdModifier::UpdateResult HardwareEnabledCmdModifier::UpdateToRenderWithResult()
{
    auto node = std::static_pointer_cast<RSSurfaceNode>(GetNode());
    if (!node) {
        return false;
    }
    auto rsUIContext = node->GetRSUIContext();
    if (rsUIContext == nullptr || rsUIContext->GetRSRenderInterface() == nullptr) {
        ROSEN_LOGE("RSSurfaceNode::SetHardwareEnabled uiContext is nullptr");
        return false;
    }
    rsUIContext->GetRSRenderInterface()->SetHardwareEnabled(
        node->GetId(), param_.isHardwareEnabled_, param_.isSelfDrawingNodeType_, param_.isDynamicHardwareEnable_);
    return true;
}

void HardwareEnabledCmdModifier::DumpParam(std::string& out) const
{
    out += "{isHardwareEnabled:" + std::string(param_.isHardwareEnabled_ ? "true" : "false") +
           ", isSelfDrawingNodeType:" + std::to_string(static_cast<int>(param_.isSelfDrawingNodeType_)) +
           ", isDynamicHardwareEnable:" + std::string(param_.isDynamicHardwareEnable_ ? "true" : "false") + "}";
}
```

**3. 在节点类中通过 SetRSCmdPropertyWithResult 调用**

```cpp
// rs_surface_node.cpp

SetRSCmdPropertyWithResult<HardwareEnabledCmdModifier>(HardwareEnabledCmdParam{
    isHardwareEnabled, isSelfDrawingNodeType, isDynamicHardwareEnable});
```

**要点：**
- `UpdateToRender()` **必须**使用 `AddCommand` 提交 command，用于推前台恢复；如果没有对应的 command，需要新增
- `UpdateToRenderWithResult()` 用于特殊 IPC 适配（如 `RSRenderInterface`），调用处通过 `SetRSCmdPropertyWithResult` 触发
- 基类默认的 `UpdateToRenderWithResult()` 实现为调用 `UpdateToRender()` 并返回 `false`，
  因此不涉及特殊 IPC 适配的 Modifier 无需重写

---

### 两种路径对比

| | `UpdateToRender()` | `UpdateToRenderWithResult()` |
|---|---|---|
| 调用入口 | `SetRSCmdProperty` | `SetRSCmdPropertyWithResult` |
| 命令提交方式 | **必须**使用 `AddCommand` | 可使用 IPC 接口等特殊适配 |
| 前台恢复 | 会被遍历调用，承担状态重建 | 基类默认实现为调用 `UpdateToRender()` |
| 返回值 | `void` | `std::variant<bool, RSInterfaceErrorCode>` |
| 典型场景 | 节点标记、开关、属性同步 | 硬件加速、回调注册、隐私内容设置等特殊 IPC 适配 |
| 示例 | `NodeGroupCmdModifier`、`SecurityLayerCmdModifier` | `HardwareEnabledCmdModifier`、`BufferAvailableCallbackCmdModifier` |

---

### 前台恢复

当应用从后台切到前台时，系统会遍历节点上注册的所有 CmdModifier，依次调用 `UpdateToRender()` 来重新向 Render Service 同步节点状态。因此 `UpdateToRender()` 必须通过 `AddCommand` 提交命令，确保前台恢复时命令能正确进入 Transaction 队列。这是 CmdModifier 存在的核心价值之一。