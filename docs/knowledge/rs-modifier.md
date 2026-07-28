# Modifier 属性同步

## 适用范围

- ModifierNG 体系的客户端-服务端属性同步机制
- RSModifier（客户端）与 RSRenderModifier（服务端）的映射关系
- 属性类型（RSModifierType / RSPropertyType）和 Modifier 分类
- 属性脏标记与去重
- Modifier 在动画驱动下的值更新
- 自定义 Modifier（CustomModifier / StyleModifier）

## 快速代码地图

| 领域 | 文件路径 | 说明 |
| --- | --- | --- |
| 客户端 Modifier 基类 | `rosen/modules/render_service_client/core/modifier_ng/rs_modifier_ng.h` | ModifierNG::RSModifier：AttachProperty/SetDirty/CreateRenderModifier |
| Modifier 类型枚举 | `rosen/modules/render_service_base/include/modifier_ng/rs_modifier_ng_type.h` | RSModifierType / RSPropertyType 枚举定义 |
| 服务端 Modifier 基类 | `rosen/modules/render_service_base/include/modifier_ng/rs_render_modifier_ng.h` | ModifierNG::RSRenderModifier：Apply/AttachProperty/OnAttachModifier |
| 属性基类 | `rosen/modules/render_service_base/include/modifier/rs_render_property.h` | RSRenderPropertyBase / RSRenderProperty<T> |
| 客户端属性 | `rosen/modules/render_service_client/core/modifier/rs_property.h` | RSProperty<T> / RSAnimatableProperty<T> |
| 几何-Bounds | `modifier_ng/geometry/rs_bounds_modifier.h` → `modifier_ng/geometry/rs_bounds_render_modifier.h` | 客户端 Bounds Modifier → 服务端 Bounds RenderModifier |
| 几何-Frame | `modifier_ng/geometry/rs_frame_modifier.h` → `modifier_ng/geometry/rs_frame_render_modifier.h` | 客户端 Frame Modifier → 服务端 Frame RenderModifier |
| 几何-Transform | `modifier_ng/geometry/rs_transform_modifier.h` → `modifier_ng/geometry/rs_transform_render_modifier.h` | 客户端 Transform Modifier → 服务端 Transform RenderModifier |
| 几何-Clip | `modifier_ng/geometry/rs_bounds_clip_modifier.h` → `modifier_ng/geometry/rs_bounds_clip_render_modifier.h` | 裁剪 Modifier |
| 外观-Alpha | `modifier_ng/appearance/rs_alpha_modifier.h` → `modifier_ng/appearance/rs_alpha_render_modifier.h` | 透明度 Modifier |
| 外观-Shadow | `modifier_ng/appearance/rs_shadow_modifier.h` → `modifier_ng/appearance/rs_shadow_render_modifier.h` | 阴影 Modifier |
| 外观-Filter | `modifier_ng/appearance/rs_background_filter_modifier.h` → `.../rs_background_filter_render_modifier.h` | 滤镜 Modifier |
| 自定义-Content | `modifier_ng/custom/rs_content_style_modifier.h` | 自定义内容绘制 |
| 自定义-Foreground | `modifier_ng/custom/rs_foreground_style_modifier.h` | 自定义前景绘制 |
| 自定义-Transition | `modifier_ng/custom/rs_transition_style_modifier.h` | 过渡效果 |
| Modifier 管理器 | `rosen/modules/render_service_client/core/modifier/rs_modifier_manager.h` | RSModifierManager：dirty Modifier 收集与分发 |
| 属性提取器 | `modifier/rs_modifier_extractor.h` | RSModifierExtractor：staging 属性读取 |

## 核心模型

### 客户端-服务端 Modifier 对应关系

每个 RSModifier（客户端）在首次 Attach 时通过 `CreateRenderModifier()` 创建对应的 `RSRenderModifier`（服务端）。两者通过 `ModifierId` 关联。

```
客户端 RSModifier                 服务端 RSRenderModifier
├── properties_ (RSProperty)  ↔──  properties_ (RSRenderProperty)
├── node_ (weak_ptr<RSNode>)      ├── attachedNode_ (RSRenderNode*)
└── SetDirty()                     └── Apply(canvas, properties)
```

### 属性类型体系

RSModifierType 定义 Modifier 语义分类（BOUNDS/FRAME/TRANSFORM/ALPHA/BACKGROUND_FILTER 等），RSPropertyType 定义具体属性标识。

关键映射：
- 几何类：BOUNDS → bounds, FRAME → frame, TRANSFORM → transform
- 外观类：ALPHA → alpha, SHADOW → shadow, BACKGROUND_FILTER → bgFilter
- 自定义类：TRANSITION_STYLE, BACKGROUND_STYLE, CONTENT_STYLE, FOREGROUND_STYLE, OVERLAY_STYLE, NODE_MODIFIER

### 属性同步流程

```
1. 客户端设置属性
   RSNode::SetBounds(value)
     → ModifierNG::RSModifier::Setter(RSPropertyType::BOUNDS, value)
       → RSProperty<T>::Set(value)  // 更新 staging 值
       → SetDirty()                 // 标记脏

2. 帧末提交
   RSModifierManager::AddModifier(dirtyModifier)
     → 通过 RSTransactionData 传输到服务端

3. 服务端应用
   RSRenderModifier::Apply(canvas, properties)
     → 从 RSRenderProperty 读取值
     → 写入 RSProperties staging 区域
```

### Modifier 生命周期

1. **创建**：RSNode 构造时或首次设置属性时创建 RSModifier。
2. **Attach**：`OnAttach(node)` 将 Modifier 挂载到 RSNode，`AttachProperty()` 绑定属性。
3. **Dirty**：属性变更时 `SetDirty()`，加入 RSModifierManager 的待提交列表。
4. **提交**：帧末收集所有 dirty Modifier，序列化到 RSTransactionData。
5. **应用**：服务端 RSRenderModifier::Apply() 将属性写入 RSProperties。
6. **Detach**：节点销毁时 `OnDetachModifier()` 清理。

### 去重机制

- `IsDeduplicationEnabled()` 返回 true 的 Modifier 支持去重：同一帧内同一属性的多次设置只保留最后一次。
- 几何类和外观类 Modifier 默认启用去重，自定义类不启用。

### 自定义 Modifier

- **ContentStyleModifier** / **ForegroundStyleModifier** / **BackgroundStyleModifier**：通过 `DrawOnNode()` 注入自定义绘制命令。
- **TransitionStyleModifier**：过渡动画效果。
- **NodeModifier**：节点级自定义修改。

## 客户端到服务端同步链路

ModifierNG 的同步不是直接复制客户端对象，
而是把服务端需要的 `RSRenderModifier` 和属性通过 command 加入 transaction：

```text
RSNode SetXxx()
  -> ModifierNG::RSModifier::AttachProperty() / Setter()
  -> RSProperty<T> staging value 更新
  -> RSModifier::CreateRenderModifier()
  -> RSAddModifierNG / RSModifierNGAttachProperty command
  -> RSTransactionData
  -> 服务端 RSRenderModifier::Unmarshalling()
  -> RSRenderNode 挂载 RSRenderModifier
  -> RSRenderModifier::ApplyLegacyProperty()
  -> RSProperties / drawable dirty
```

关键代码锚点：

- 客户端 `AttachProperty()`：
  `rosen/modules/render_service_client/core/modifier_ng/rs_modifier_ng.cpp`。
- 首次创建服务端对象：
  `RSModifier::CreateRenderModifier()`。
- RSNode 上添加 modifier：
  `rosen/modules/render_service_client/core/ui/rs_node.cpp` 中的 `RSAddModifierNG`。
- 服务端属性绑定：
  `rosen/modules/render_service_base/src/modifier_ng/rs_render_modifier_ng.cpp`。
- 服务端去重序列化：
  各类 `*_render_modifier.cpp` 中的 `DeduplicationMarshalling()`。

### 新增属性同步步骤

1. 在 `RSPropertyType` 中增加属性标识，确认是否影响 ABI 或旧数据。
2. 在客户端 modifier 中新增 `RSProperty<T>`，并在 setter 中调用 `SetDirty()`。
3. 在 `CreateRenderModifier()` 路径确保服务端能创建同类型 `RSRenderProperty<T>`。
4. 在服务端 render modifier 的 apply/reset map 中写入 `RSProperties` 对应字段。
5. 判断是否启用 `DeduplicationMarshalling()`，同帧多次设置只保留最终值。
6. 补客户端、transaction、服务端和 drawable dirty 的最近单测或 fuzz。

## 生命周期和跨线程边界

### 创建与挂载

- Modifier 可以在节点构造、首次属性设置、动画创建或自定义绘制时生成。
- `AttachProperty()` 可能先于服务端节点可见，command 到达后要能按 nodeId 找到目标节点。
- shadow node、surface node 和普通 canvas node 可能使用不同 attach 路径，新增属性要逐个确认。

### Dirty 与提交

- 客户端 `SetDirty()` 只表示需要进 transaction，不等于服务端已应用。
- 同一帧重复设置同一属性时，要确认 dedup 后旧值不会触发无效 dirty。
- 自定义 modifier 默认不一定适合去重，因为 draw command list 可能有顺序语义。

### 动画驱动

动画会直接修改 `RSAnimatableProperty<T>`，再通过 `RSModifierManager::Animate()` 推动 dirty：

- UI 线程动画看 `RSModifierManager::AddAnimation()`、`Animate()`、`OnAnimationFinished()`。
- RenderThread 动画看 `RSRenderThread::Animate()` 和节点上的 render modifier。
- 弹簧动画有 propertyId 到 animationId 的注册表，新增属性要确认是否需要查询旧弹簧。

### 跨实例与销毁

- `RSModifierManager::MoveModifier()` 用于迁移 manager，不能假设 modifier 永远属于同一个 context。
- `weak_ptr<RSNode>` 失效时，setter 和动画结束回调不能解引用旧节点。
- 服务端 `OnAttachModifier()` / `OnDetachModifier()` 要清理 attached node 和 property 引用。
- 节点销毁后到达的迟到 command 应安全忽略，不能重新创建已销毁节点状态。

## 修改检查清单

- 客户端 modifier、服务端 render modifier、`RSPropertyType`、reset/apply map 是否同步。
- 新属性默认值是否同时覆盖客户端 staging、服务端 staging 和旧 transaction。
- 是否影响动画、隐式动画、交互动画或 spring animation 查询。
- 是否影响 drawable dirty、bounds/alpha/filter 等绘制顺序。
- 去重是否保持最终值语义；不能去重的自定义绘制要明确说明。
- 跨线程回调只捕获有生命周期保障的对象，避免裸指针和悬空 lambda。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 客户端/服务端 Modifier 配对 | RSModifier → CreateRenderModifier → RSRenderModifier | 客户端管理属性 staging，服务端管理渲染应用，职责分离 |
| RSModifierType 排序 | ModifiersNGMap 使用 std::map | 保证几何→外观→自定义的应用顺序，避免变换影响外观 |
| 属性去重 | `IsDeduplicationEnabled()` | 同帧多次 SetXxx 只保留最终值，减少无效 IPC |
| RSProperty<T> 模板 | `RSProperty<T>` / `RSAnimatableProperty<T>` | 类型安全的属性容器，支持动画插值 |
| SetDirty 链 | Modifier::SetDirty → ModifierManager → TransactionProxy | 统一脏标记传递链，确保属性变更最终到达服务端 |
| ApplyLegacyProperty | `RSRenderModifier::ApplyLegacyProperty()` | 兼容旧 RSProperties 接口，平滑迁移到 ModifierNG |

## 补充背景

### RSModifierManagerMap 的多实例管理逻辑

#### 单例 + 线程本地存储模式

`RSModifierManagerMap` 采用**单例 + thread_local**模式管理多个 `RSModifierManager` 实例：

```cpp
class RSModifierManagerMap final {
public:
    static std::shared_ptr<RSModifierManagerMap>& Instance();
    const std::shared_ptr<RSModifierManager>& GetModifierManager();
private:
    static std::shared_ptr<RSModifierManagerMap> instance_;
};

const std::shared_ptr<RSModifierManager>& RSModifierManagerMap::GetModifierManager()
{
    thread_local std::shared_ptr<RSModifierManager> manager = nullptr;
    if (manager == nullptr) {
        manager = std::make_shared<RSModifierManager>();
    }
    return manager;
}
```

#### 设计要点

1. **全局单例入口**：`Instance()` 返回唯一的 `RSModifierManagerMap` 实例，所有线程通过同一入口访问。

2. **线程独立实例**：`GetModifierManager()` 使用 `thread_local` 存储，每个线程首次调用时创建独立的 `RSModifierManager`。不同线程的 Modifier、动画和属性更新完全隔离，避免跨线程同步开销。

3. **典型使用场景**：
   - UI 线程：处理用户交互触发的属性变更和动画。
   - Render 线程（如果启用）：处理渲染线程上的自定义绘制。
   - 多窗口实例：每个 `RSUIContext` 可以绑定独立的 Modifier 管理逻辑。

4. **跨线程迁移**：`RSModifierManager::MoveModifier()` 支持将指定 nodeId 的所有 Modifier 迁移到另一个 Manager：

```cpp
void RSModifierManager::MoveModifier(std::shared_ptr<RSModifierManager> dstModifierManager, NodeId nodeId)
{
    for (auto iter = modifiers_.begin(); iter != modifiers_.end();) {
        if (*iter) {
            auto node = (*iter)->node_.lock();
            if (node && node->GetId() == nodeId) {
                dstModifierManager->modifiers_.insert(*iter);
                iter = modifiers_.erase(iter);
                continue;
            }
        }
        ++iter;
    }
}
```

此方法用于节点在不同上下文或线程间迁移时，保持 Modifier 关联关系。

#### RSModifierManager 核心职责

每个 `RSModifierManager` 实例管理三类对象：

| 成员 | 类型 | 作用 |
| --- | --- | --- |
| `modifiers_` | `std::set<std::shared_ptr<Modifier>>` | 当前帧需要更新到服务端的 dirty Modifier 集合 |
| `animations_` | `std::unordered_map<AnimationId, std::weak_ptr<RSRenderAnimation>>` | 运行中的动画，由 `Animate()` 驱动 |
| `springAnimations_` | `std::unordered_map<PropertyId, AnimationId>` | 弹簧动画注册表，用于查询正在运行的弹簧 |

关键操作：

- `AddModifier()`：将 dirty Modifier 加入集合，帧末由 `Draw()` 统一调用 `UpdateToRender()`。
- `Animate()`：驱动所有运行中的动画，计算帧率范围，处理动画完成回调。
- `Draw()`：遍历 `modifiers_`，调用每个 Modifier 的 `UpdateToRender()` 并清空集合。

### 自定义 Modifier 的 SimpleDrawCmdList 传递和回放机制

#### 客户端录制流程

自定义 Modifier（`RSCustomModifier` 及其子类）在客户端通过以下流程录制绘制命令：

```
RSCustomModifier::UpdateToRender()
  → RSCustomModifierHelper::CreateDrawingContext(node)
    → 创建 ExtendRecordingCanvas(width, height)
    → 返回 RSDrawingContext { canvas, width, height }
  → Draw(ctx)  // 用户实现的绘制逻辑
    → 在 ExtendRecordingCanvas 上执行绘制命令
  → RSCustomModifierHelper::FinishDrawing(ctx)
    → 从 recordingCanvas 提取 DrawCmdList
    → 可选: GenerateCache() (GetDrawTextAsBitmap 时)
    → 返回 std::shared_ptr<Drawing::DrawCmdList>
  → UpdateProperty(node, drawCmdList, propertyId)
    → 创建 RSUpdatePropertyDrawCmdListNG command
    → node->AddCommand(command)
```

关键点：

1. **ExtendRecordingCanvas**：继承自 `RecordingCanvas`，支持录制所有绘制操作到 `DrawCmdList`。
2. **DrawCmdList**：存储绘制命令序列（`DrawOpItem`），可序列化用于 IPC。
3. **ContentTransition 支持**：如果配置了 `ContentTransitionType::OPACITY`，可通过动画过渡新旧内容。

#### IPC 传输

`RSUpdatePropertyDrawCmdListNG` command 将 `DrawCmdList` 发送到服务端：

```cpp
void RSNodeCommandHelper::UpdatePropertyDrawCmdList(
    RSContext& context, NodeId nodeId, Drawing::DrawCmdListPtr drawCmdList, PropertyId id, PropertyUpdateType type)
{
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (auto property = node->GetProperty(id)) {
        std::static_pointer_cast<RSRenderProperty<SimpleDrawCmdListPtr>>(property)->Set(
            RSSimpleDrawCmdList::CreateFromDrawCmdList(drawCmdList), type);
    }
}
```

服务端接收时调用 `RSSimpleDrawCmdList::CreateFromDrawCmdList()`：

```cpp
std::shared_ptr<RSSimpleDrawCmdList> RSSimpleDrawCmdList::CreateFromDrawCmdList(Drawing::DrawCmdListPtr drawCmdList)
{
    if (!drawCmdList->UnmarshallingDrawOpsSimple()) {  // 反序列化所有 OpItem
        return nullptr;
    }
    return std::make_shared<RSSimpleDrawCmdList>(
        drawCmdList->GetWidth(), drawCmdList->GetHeight(), drawCmdList->GetDrawOpItems());
}
```

#### RSSimpleDrawCmdList 设计

`RSSimpleDrawCmdList` 是完全反序列化后的绘制命令容器：

```cpp
class RSSimpleDrawCmdList {
private:
    int32_t width_ = 0;
    int32_t height_ = 0;
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> drawOpItems_;
    mutable std::recursive_mutex mutex_;
};
```

特点：

1. **完全反序列化**：构造时调用 `UnmarshallingDrawOpsSimple()`，所有 `DrawOpItem` 从序列化数据恢复为可用对象。
2. **线程安全**：使用 `recursive_mutex` 保护 `drawOpItems_` 的访问。
3. **直接 Playback**：无需再次反序列化，可直接在 Canvas 上回放：

```cpp
void RSSimpleDrawCmdList::Playback(Drawing::Canvas& canvas, const Drawing::Rect* rect)
{
    if (width_ <= 0 || height_ <= 0) {
        return;
    }
    PlaybackByVector(canvas, rect);  // 遍历 drawOpItems_ 逐个 Playback
}
```

#### 服务端应用流程

`RSCustomRenderModifier` 在服务端应用 SimpleDrawCmdList：

```cpp
template<RSModifierType T>
void RSCustomRenderModifier<T>::Apply(RSPaintFilterCanvas* canvas, RSProperties& properties)
{
    auto propertyType = ModifierTypeConvertor::GetPropertyType(GetType());
    if (HasProperty(propertyType) && canvas) {
        auto cmds = Getter<SimpleDrawCmdListPtr>(propertyType, nullptr);
        RSPropertiesPainter::DrawFrame(properties, *canvas, cmds);
    }
}
```

`RSPropertiesPainter::DrawFrame()` 处理 Gravity 和 Playback：

```cpp
void RSPropertiesPainter::DrawFrame(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, SimpleDrawCmdListPtr& cmds)
{
    Drawing::Matrix mat;
    if (GetGravityMatrix(properties.GetFrameGravity(), properties.GetFrameRect(),
                         cmds->GetWidth(), cmds->GetHeight(), mat)) {
        canvas.ConcatMatrix(mat);  // 应用 Gravity 变换
    }
    auto frameRect = Rect2DrawingRect(properties.GetFrameRect());
    cmds->Playback(canvas, &frameRect);  // 回放所有绘制命令
}
```

#### Drawable 层回放

`RSCustomModifierDrawable` 在渲染线程再次回放：

```cpp
void RSCustomModifierDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    for (size_t i = 0; i < drawCmdListVec_.size(); i++) {
        const auto& drawCmdList = drawCmdListVec_[i];
        Drawing::Matrix mat;
        if (isCanvasNode_ && RSPropertyDrawableUtils::GetGravityMatrix(
            gravity_, *rect, drawCmdList->GetWidth(), drawCmdList->GetHeight(), mat)) {
            canvas->ConcatMatrix(mat);
        }
        drawCmdList->Playback(*canvas, rect);
    }
}
```

#### 传递链路总结

```
客户端:
  RSCustomModifier::Draw() → ExtendRecordingCanvas → DrawCmdList
    → RSUpdatePropertyDrawCmdListNG command → IPC

服务端:
  RSNodeCommandHelper::UpdatePropertyDrawCmdList()
    → RSSimpleDrawCmdList::CreateFromDrawCmdList() (反序列化)
    → RSRenderProperty<SimpleDrawCmdListPtr>::Set()

服务端 RenderModifier 层:
  RSCustomRenderModifier::Apply()
    → RSPropertiesPainter::DrawFrame()
    → SimpleDrawCmdList::Playback()

服务端 Drawable 层:
  RSCustomModifierDrawable::OnDraw()
    → SimpleDrawCmdList::Playback()
```

#### 设计背景

| 设计 | 代码体现 | 设计意图 |
| --- | --- | --- |
| DrawCmdList → SimpleDrawCmdList 转换 | `CreateFromDrawCmdList()` | IPC 时序列化，服务端一次性反序列化后可多次回放，避免每帧重复解析 |
| thread_local RSModifierManager | `GetModifierManager()` | 不同线程的绘制和动画完全隔离，无锁开销 |
| ExtendRecordingCanvas | `CreateDrawingContext()` | 复用 Recording 体系，支持所有 Drawing API |
| Gravity 变换 | `DrawFrame()` + `GetGravityMatrix()` | 支持 CanvasNode 内容按 Gravity 对齐到 Frame |
| noNeedUICaptured | `SetNoNeedUICaptured()` | 标记某些自定义绘制不应被 UI 截图捕获 |

---

## 待补充背景

- ModifierNG 与旧 Modifier 体系的完整迁移状态和兼容策略。
- SingleFrameModifier 的特殊处理逻辑。
- Modifier 渲染线程（rs_modifiers_draw_thread）的工作模式。
