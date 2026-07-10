# NGEffect 框架

## 适用范围

- NGEffect（Next Generation Effect）框架的整体架构和数据驱动设计
- 服务端 `RSNGRender*` 模板体系（Filter / Shader / Mask / Shape 四类）
- 客户端 `RSNG*` 属性体系与 `RSNode` 集成方式
- 新增 Filter / Shader / Mask / Shape 的完整开发步骤
- 客户端工厂类 `RSNGFilterHelper` 等的使用方法
- 与 `graphic_graphics_effect` 组件（GEVisualEffect / GERender）的联通机制

## 快速代码地图

### 服务端（render_service_base）

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| 属性标签基类 | `rosen/modules/render_service_base/include/effect/rs_render_property_tag.h` | `RenderPropertyTagBase`、`RSNGEffectType` 枚举、标签宏 |
| 属性标签声明 | `rosen/modules/render_service_base/include/effect/rs_render_property_tag_def.in` | 全部属性的 X-macro 声明 |
| 效果模板基类 | `rosen/modules/render_service_base/include/effect/rs_render_effect_template.h` | `RSNGRenderEffectBase<>`、`RSNGRenderEffectTemplate<>` |
| Filter 基类 | `rosen/modules/render_service_base/include/effect/rs_render_filter_base.h` | `RSNGRenderFilterBase`、`DECLARE_FILTER` 宏 |
| Filter 类型声明 | `rosen/modules/render_service_base/include/effect/rs_render_filter_def.in` | 全部 Filter 的 X-macro 声明 |
| Shader 基类 | `rosen/modules/render_service_base/include/effect/rs_render_shader_base.h` | `RSNGRenderShaderBase`、`DECLARE_SHADER` 宏 |
| Shader 类型声明 | `rosen/modules/render_service_base/include/effect/rs_render_shader_def.in` | 全部 Shader 的 X-macro 声明 |
| Mask 基类 | `rosen/modules/render_service_base/include/effect/rs_render_mask_base.h` | `RSNGRenderMaskBase`、`DECLARE_MASK` 宏 |
| Mask 类型声明 | `rosen/modules/render_service_base/include/effect/rs_render_mask_def.in` | 全部 Mask 的 X-macro 声明 |
| Shape 基类 | `rosen/modules/render_service_base/include/effect/rs_render_shape_base.h` | `RSNGRenderShapeBase`、`DECLARE_SHAPE` 宏 |
| Shape 类型声明 | `rosen/modules/render_service_base/include/effect/rs_render_shape_def.in` | 全部 SDF Shape 的 X-macro 声明 |
| 效果辅助实现 | `rosen/modules/render_service_base/src/effect/rs_render_effect_template.cpp` | `CreateGEVisualEffect`、参数转换、哈希 |
| Filter 工厂实现 | `rosen/modules/render_service_base/src/effect/rs_render_filter_base.cpp` | creatorLUT、Unmarshalling、CalcRect |
| Shader 工厂实现 | `rosen/modules/render_service_base/src/effect/rs_render_shader_base.cpp` | creatorLUT、Unmarshalling |
| Mask 工厂实现 | `rosen/modules/render_service_base/src/effect/rs_render_mask_base.cpp` | creatorLUT、Unmarshalling |
| Shape 工厂实现 | `rosen/modules/render_service_base/src/effect/rs_render_shape_base.cpp` | creatorLUT、Unmarshalling、CalcRect |
| 属性挂载入口 | `rosen/modules/render_service_base/include/property/rs_properties.h` | `SetBackgroundNGFilter` 等 setter |
| 渲染桥接 | `rosen/modules/render_service_base/src/render/rs_drawing_filter.cpp` | `GenerateAndUpdateGEVisualEffect` → `GERender` |
| IPC 序列化 | `rosen/modules/render_service_base/src/platform/ohos/rs_marshalling_helper.cpp` | NG Filter / Mask / Shape / Shader 的 Marshalling |
| 单测 | `rosen/test/render_service/render_service_base/unittest/effect/` | 6 个测试文件 |

### 客户端（render_service_client/ui_effect）

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| 客户端属性标签 | `rosen/modules/render_service_client/core/ui_effect/property/include/rs_ui_property_tag.h` | `PropertyTagBase`、标签宏 |
| 客户端效果模板 | `rosen/modules/render_service_client/core/ui_effect/property/include/rs_ui_template.h` | `RSNGEffectBase<>`、`RSNGEffectTemplate<>` |
| Filter 客户端基类 | `rosen/modules/render_service_client/core/ui_effect/property/include/rs_ui_filter_base.h` | `RSNGFilterBase`、`RSNGFilterHelper` |
| Mask 客户端基类 | `rosen/modules/render_service_client/core/ui_effect/property/include/rs_ui_mask_base.h` | `RSNGMaskBase` |
| Shader 客户端基类 | `rosen/modules/render_service_client/core/ui_effect/property/include/rs_ui_shader_base.h` | `RSNGShaderBase` |
| Shape 客户端基类 | `rosen/modules/render_service_client/core/ui_effect/property/include/rs_ui_shape_base.h` | `RSNGShapeBase` |
| RSNode 集成 | `rosen/modules/render_service_client/core/ui/rs_node.h` | `SetBackgroundNGFilter` 等 |
| RSNode 实现 | `rosen/modules/render_service_client/core/ui/rs_node.cpp` | `SetVisualEffect`、各 NG setter |
| Legacy Filter 容器 | `rosen/modules/render_service_client/core/ui_effect/filter/include/filter.h` | 旧版 `Filter` + `FilterPara` |
| Legacy Mask 容器 | `rosen/modules/render_service_client/core/ui_effect/mask/include/mask.h` | 旧版 `Mask` + `MaskPara` |
| Legacy Effect 容器 | `rosen/modules/render_service_client/core/ui_effect/effect/include/visual_effect.h` | 旧版 `VisualEffect` + `VisualEffectPara` |

## 核心模型

### 四类效果

| 类别 | 服务端基类 | 客户端基类 | 用途 | GE 对应 |
| --- | --- | --- | --- | --- |
| Filter | `RSNGRenderFilterBase` | `RSNGFilterBase` | 图像处理（模糊/变形/光照等），改变内容像素 | `GEShaderFilter` |
| Shader | `RSNGRenderShaderBase` | `RSNGShaderBase` | 叠加在内容之上的流光/光晕等视觉效果 | `GEShader` |
| Mask | `RSNGRenderMaskBase` | `RSNGMaskBase` | Alpha 遮罩，可嵌套为 Filter 属性 | `GEShaderMask` |
| Shape | `RSNGRenderShapeBase` | `RSNGShapeBase` | SDF 几何形状，可嵌套为 Filter/Shape 属性 | `GEShaderShape` |

### 数据驱动的模板体系

框架基于 CRTP（Curiously Recurring Template Pattern）+ 可变参数模板，
通过 `.in` 文件中的宏声明生成具体效果类，无需手写子类：

```
RSNGRenderEffectBase<Derived>              ← CRTP 基类，链表 + 序列化 + 挂载
  ├── RSNGRenderFilterBase                 ← Filter 工厂 + 行为 LUT
  ├── RSNGRenderShaderBase                 ← Shader 工厂
  ├── RSNGRenderMaskBase                   ← Mask 工厂
  └── RSNGRenderShapeBase                  ← Shape 工厂 + CalcRect

RSNGRenderEffectTemplate<Base, Type, Tags...>   ← 属性绑定层
  持有 std::tuple<Tags...>，编译期类型安全
  Getter<Tag>() / Setter<Tag>() / Contains<Tag>()
```

### 属性标签系统

每个效果参数声明为 `RenderPropertyTagBase<Name, PropertyType>`：

- `DECLARE_ANIMATABLE_PROPERTY_TAG(EffectName, PropName, Type)` — 可动画，底层为 `RSAnimatableProperty`，支持插值和动画驱动
- `DECLARE_NONANIMATABLE_PROPERTY_TAG(EffectName, PropName, Type)` — 不可动画

支持的值类型：`int`、`float`、`bool`、`Vector2f`、`Vector3f`、`Vector4f`、
`Color`、`RRect`、`Matrix3f`、`std::vector<...>`、
`std::shared_ptr<Media::PixelMap>`、`std::shared_ptr<Drawing::Image>`、
`std::shared_ptr<RSPath>`、`std::shared_ptr<RSNGRenderMaskBase>`（`MASK_PTR`）、
`std::shared_ptr<RSNGRenderShapeBase>`（`SHAPE_PTR`）。

**Mask 和 Shape 可作为 Filter 的属性嵌套**（如 `EdgeLight.Mask`、`FrostedGlass.Shape`）。

### 工厂注册（creatorLUT）

每类效果在对应的 `*_base.cpp` 中维护一个静态 `unordered_map<RSNGEffectType, Creator>`：

| 工厂位置 | 约有类型数 |
| --- | --- |
| `rs_render_filter_base.cpp` creatorLUT | 25 Filter |
| `rs_render_shader_base.cpp` creatorLUT | 21 Shader |
| `rs_render_mask_base.cpp` creatorLUT | ~11 Mask |
| `rs_render_shape_base.cpp` creatorLUT | ~12 Shape |

`Create(RSNGEffectType)` 查表构造，未注册类型返回 `nullptr`。

Filter 额外维护行为 LUT：`getSnapshotRectLUT`、`getDrawRectLUT`、`checkFilterSkipLUT`。

### 效果链（Effect Chain）

每个效果实例通过 `nextEffect_` 构成单链表（上限 1000），IPC 序列化时以
`END_OF_CHAIN`（= `RSNGEffectType::INVALID`）作为终止哨兵。

### 端到端数据流

```
客户端（应用进程）                           服务端（Render Service 进程）
─────────────────                           ────────────────────────────
RSNGFilterHelper::CreateNGBlurFilter()
  → RSNGFilterTemplate 实例
node.SetBackgroundNGFilter(effect)
  → SetPropertyNG → ModifierNG
  → effect.Attach(node, modifier)             RSNGRenderFilterTemplate（服务端镜像）
  → IPC Marshalling                         ← IPC Unmarshalling（creatorLUT 重建）
                                              → RSProperties::SetBackgroundNGFilter()
                                               → ComposeNGRenderFilter()
                                                 → RSDrawingFilter::SetNGRenderFilter()
                                                → GenerateAndUpdateGEVisualEffect()
                                                  → RSNGRenderFilterHelper::GenerateGEVisualEffect()
                                                    → CreateGEVisualEffect(type) + SetParam(name, value)
                                                  → GEVisualEffectContainer
                                                → GERender::ApplyImageEffect(canvas, container)
                                                  → GEEffectFactory 创建 GEShaderFilter
                                                  → 处理图像 → 返回 Drawing::Image
```

## 开发指南：新增一个 Filter

以新增 `MyEffect` Filter 为例，完整步骤如下。

### 步骤 1：添加枚举值

在 `rs_render_property_tag.h` 的 `RSNGEffectType` 枚举中添加：

```cpp
enum class RSNGEffectType : int32_t {
    // ...
    MY_EFFECT,
    // ...
};
```

在 `rs_render_effect_template.h` 的 `GetEffectTypeString()` 中添加对应字符串：

```cpp
case RSNGEffectType::MY_EFFECT: return "MY_EFFECT";
```

> 该字符串必须与 `graphic_graphics_effect` 中 `GEFilterType` 的 name 一致，
> 因为 RS 侧用此字符串构造 `GEVisualEffect`。

### 步骤 2：声明属性标签

在 `rs_render_property_tag_def.in` 中添加：

```cpp
// 可动画属性（底层为 RSAnimatableProperty，支持插值）
DECLARE_ANIMATABLE_PROPERTY_TAG(MyEffect, Intensity, float);
// 不可动画属性
DECLARE_NONANIMATABLE_PROPERTY_TAG(MyEffect, Flag, bool);
// 嵌套 Mask
DECLARE_NONANIMATABLE_PROPERTY_TAG(MyEffect, Mask, MASK_PTR);
```

每个标签展开为 `using MyEffectIntensityRenderTag = RSRenderAnimatablePropertyTag<float, "MyEffect_Intensity">;`。

### 步骤 3：声明 Filter 类型

在 `rs_render_filter_def.in` 中添加：

```cpp
DECLARE_FILTER(MyEffect, MY_EFFECT,
    ADD_PROPERTY_TAG(MyEffect, Intensity),
    ADD_PROPERTY_TAG(MyEffect, Flag),
    ADD_PROPERTY_TAG(MyEffect, Mask));
```

展开为 `using RSNGRenderMyEffectFilter = RSNGRenderFilterTemplate<RSNGEffectType::MY_EFFECT, MyEffectIntensityRenderTag, MyEffectFlagRenderTag, MyEffectMaskRenderTag>;`。

### 步骤 4：注册工厂

在 `rs_render_filter_base.cpp` 的 `creatorLUT` 中添加：

```cpp
{
    RSNGEffectType::MY_EFFECT,
    [] { return std::make_shared<RSNGRenderMyEffectFilter>(); }
},
```

### 步骤 5：实现 GE 侧效果（跨仓）

在 `graphic_graphics_effect` 中新增 `GEShaderFilter` 子类和对应的 `.params.in` 参数定义，
并注册到 `GEEffectFactory`。参数名（`[[ge::prop("MY_EFFECT_INTENSITY")]]`）
必须与 RS 侧 `SetParam` 调用使用的 name 一致。

### 开发 Shader / Mask / Shape 的差异

- **Shader**：步骤 2-4 对应 `rs_render_shader_def.in` 和 `rs_render_shader_base.cpp`。
- **Mask**：对应 `rs_render_mask_def.in` 和 `rs_render_mask_base.cpp`。
- **Shape**：对应 `rs_render_shape_def.in` 和 `rs_render_shape_base.cpp`。
  Shape 不含可动画属性的场景也需声明 `DECLARE_SHAPE(Name, TYPE)`（无属性时省略 `ADD_PROPERTY_TAG`）。

## 客户端类使用说明

### 工厂快捷方法

```cpp
// 模糊滤镜
auto filter = RSNGFilterHelper::CreateNGBlurFilter(radiusX, radiusY, disableSystemAdaptation);

// 材质模糊
auto filter = RSNGFilterHelper::CreateNGMaterialBlurFilter(materialParam, colorMode);
```

### 通用工厂创建

```cpp
// 按类型创建
auto filter = RSNGFilterBase::Create(RSNGEffectType::BLUR);

// 从 legacy FilterPara 转换
auto filter = RSNGFilterBase::Create(filterPara);
```

### 通过 RSNode 应用

```cpp
// 背景 NG Filter
node->SetBackgroundNGFilter(filter);

// 前景 NG Filter
node->SetForegroundNGFilter(filter);

// 背景 NG Shader
node->SetBackgroundNGShader(shader);

// 材质 NG Filter
node->SetMaterialNGFilter(filter);

// SDF Shape
node->SetSDFShape(shape);

// 传入 nullptr 移除效果
node->SetBackgroundNGFilter(nullptr);
```

### RSNode setter 与 Modifier 映射

| RSNode 方法 | NG 类型 | ModifierNG 目标 |
| --- | --- | --- |
| `SetBackgroundNGFilter` | `RSNGFilterBase` | `RSBackgroundFilterModifier::SetNGFilterBase` |
| `SetForegroundNGFilter` | `RSNGFilterBase` | `RSForegroundFilterModifier::SetNGFilterBase` |
| `SetBackgroundNGShader` | `RSNGShaderBase` | `RSBackgroundNGShaderModifier` |
| `SetMaterialNGFilter` | `RSNGFilterBase` | `RSMaterialFilterModifier` |
| `SetCompositingNGFilter` | `RSNGFilterBase` | compositing filter modifier |
| `SetSDFShape` | `RSNGShapeBase` | `RSBoundsClipModifier` |
| `SetMaterialShader` | `RSNGShaderBase` | `RSMaterialShaderModifier` |
| `SetForegroundShader` | `RSNGShaderBase` | `RSForegroundShaderModifier` |
| `SetOverlayNGShader` | `RSNGShaderBase` | overlay shader modifier |
| `SetBorderSDFShader` / `SetOutlineSDFShader` | `RSNGShaderBase` | border/outline SDF modifier |

### Legacy Para 与 NG 的桥接

两代 API 共存，通过 convertorLUT 桥接：

- `RSNGFilterBase::Create(shared_ptr<FilterPara>)` — 按 ParaType 转换
- `RSNGMaskBase::Create(shared_ptr<MaskPara>)` — 按 Type 转换
- `RSNGShaderBase::Create(shared_ptr<VisualEffectPara>)` — 按 ParaType 转换

`RSNode::SetVisualEffect(VisualEffect*)` 是旧版 dispatcher：遍历 `VisualEffectPara`，
按类型路由到 NG setter。

## 与 graphic_graphics_effect 组件的联通

### 依赖关系

```
graphic_2d (Render Service)
    │  RS NG Render Filter/Shader/Mask/Shape 模板
    │  GenerateGEVisualEffect() → GEVisualEffect
    │
    ▼  链接 graphics_effect:graphics_effect_core
graphic_graphics_effect (GE 组件)
    │  GEEffectFactory → GEShaderFilter / GEShader / GEShaderMask / GEShaderShape
    │  GERender::ApplyImageEffect / DrawShaderEffect
    │
    ▼  构建期单向依赖
graphic_2d:2d_graphics (Drawing API: Canvas, Image, RuntimeEffect)
```

GE 依赖 `graphic_2d:2d_graphics`（Drawing API），不反向依赖 Render Service。
Render Service 依赖 `graphics_effect:graphics_effect_core`。

### 翻译层：RS NGEffect → GEVisualEffect

核心文件：`rs_render_effect_template.cpp`

- `RSNGRenderEffectHelper::CreateGEVisualEffect(type)` — 按效果类型字符串构造 `GEVisualEffect`
- `UpdateVisualEffectParamImpl` — 重载函数将 RS 类型转换为 `GEVisualEffect::SetParam(name, value)` 调用

| RS 类型 | GE SetParam 调用 |
| --- | --- |
| `float` / `int` / `bool` | `SetParam(name, value)` |
| `Vector2f` / `Vector3f` / `Vector4f` | 拆分为分量或直接传递 |
| `Color` | 转 `float` 或 `uint32_t` |
| `RRect` | 拆分为位置 + 圆角 |
| `Matrix3f` | 拆分为 9 个 float |
| `shared_ptr<PixelMap>` | `SetParam(name, pixelMap)` |
| `shared_ptr<RSPath>` | 转 Path 数据 |
| `shared_ptr<RSNGRenderMaskBase>` | 调用 mask 的 `GenerateGEVisualEffect()` 生成 `GEVisualEffect`，再 `SetParam` |
| `shared_ptr<RSNGRenderShapeBase>` | 调用 shape 的 `GenerateGEVisualEffect()` |

### 运行时调用：RSDrawingFilter → GERender

`rs_drawing_filter.cpp` 是核心桥接：

1. `GenerateAndUpdateGEVisualEffect()`：收集所有效果到 `GEVisualEffectContainer`
   - Legacy shaderFilters → `filter->GenerateGEVisualEffect(container)`
   - NG renderFilter → `RSNGRenderFilterHelper::GenerateGEVisualEffect()` + `UpdateToGEContainer()`
2. `ApplyImageEffect()`：`gerender->ApplyImageEffect(canvas, container, context, sampling)`
   → GE 内部经 `GEEffectFactory` 创建具体 `GEShaderFilter`，链式处理，返回 `Drawing::Image`
3. `ApplyHpsImageEffect()`：`gerender->ApplyHpsGEImageEffect(...)` — GE+HPS 混合 GPU 管线

Drawable 也可直接实例化 `GERender`：
`rs_property_drawable.cpp`、`rs_material_shader_drawable.cpp`、
`rs_overlay_ng_shader_drawable.cpp`、`rs_coverage_ng_shader_drawable.cpp` 等。

### GE 组件 API 概览

| GE 类 | 作用 |
| --- | --- |
| `GEVisualEffect` | 效果句柄，`SetParam(name, value)` 设置参数 |
| `GEVisualEffectContainer` | 有序效果链，携带几何信息和缓存标志 |
| `GERender` | 渲染入口：`ApplyImageEffect`、`DrawImageEffect`、`DrawShaderEffect`、`ApplyHpsGEImageEffect` |
| `GEEffectFactory` | 按 `GEFilterType` 创建具体效果实例 |
| `GEShaderFilter` | 图像处理基类（输入 Image，输出 Image） |
| `GEShader` | 直接绘制到 Canvas 的着色器基类 |
| `GEShaderMask` | Alpha 遮罩着色器基类 |
| `GEShaderShape` | SDF 形状着色器基类 |

### 新增效果的两仓协作流程

1. **GE 仓**：新增 `GEShaderFilter` 子类 + `.params.in` 参数定义 + 工厂注册
2. **graphic_2d 仓**：在 `.in` 文件声明属性标签和效果类型，在 `creatorLUT` 注册工厂，
   确保效果类型字符串与 GE 仓的 `GEFilterType` name 一致
3. 两仓的参数名必须完全对应（RS 侧 `SetParam` 的 name ← GE 侧 `[[ge::prop(...)]]`）

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 数据驱动模板代替虚函数继承 | `.in` 宏 + `RSNGRenderEffectTemplate` | 新增效果无需写子类，行为差异用 LUT 配置 |
| 效果链单链表 | `nextEffect_`（上限 1000） | 支持多效果组合，IPC 用 `END_OF_CHAIN` 哨兵终止 |
| 属性复用 RSAnimatableProperty | `RSRenderAnimatablePropertyTag` | 效果参数自动获得动画、dirty 追踪能力 |
| Mask/Shape 可嵌套为属性 | `MASK_PTR` / `SHAPE_PTR` 类型标签 | 支持 `EdgeLight.Mask`、`FrostedGlass.Shape` 等组合 |
| RS → GE 单向翻译 | `CreateGEVisualEffect` + `SetParam` | RS 不关心 shader 实现，GE 不依赖 RS，边界清晰 |
| 保留 Legacy Para 容器 | `convertorLUT` 桥接 | 渐进迁移，兼容已有 NAPI/Taihe/ANI 绑定 |
| IPC 白名单 | `IsWhitelistPara` | 安全边界，只有显式允许的类型可跨进程 |

## 待补充背景

- `GEFilterComposer` 多 pass 合成（HPS/Mesa/DirectDraw）的内部调度
- GE 动态扩展加载（`GEExternalDynamicLoader` / `libgraphics_effect_ext.z.so`）
- IPC Marshalling 中大对象（PixelMap/Path/vector）的大小限制和性能
- `ComposeNGRenderFilter` 中 CONTENT_LIGHT 旋转角同步和 FROSTED_GLASS_BLUR 跳帧的特殊处理
