# 多语言 API 绑定（NAPI/CJ/ANI/Taihe）

## 适用范围

改动涉及 interfaces/kits 中 NAPI、CJ、ANI、Taihe 多语言公开 API 绑定和 XTS 验证时，先读本文，再回到代码确认当前实现。

本文是背景知识和排查路线，不替代代码。修改前仍需读取对应头文件、实现文件和测试。

## 快速代码地图

| 方向 | 关键文件（完整路径） |
| --- | --- |
| NAPI 构建入口 | `interfaces/kits/napi/BUILD.gn` |
| NAPI Drawing | `interfaces/kits/napi/graphic/drawing/` |
| NAPI ColorManager | `interfaces/kits/napi/graphic/color_manager/` |
| NAPI EffectKit | `interfaces/kits/napi/graphic/effect_kit/` |
| NAPI HDR Capability | `interfaces/kits/napi/graphic/hdr_capability/` |
| NAPI HGM | `interfaces/kits/napi/graphic/hyper_graphic_manager/` |
| NAPI UIEffect | `interfaces/kits/napi/graphic/ui_effect/` |
| NAPI WebGL | `interfaces/kits/napi/graphic/webgl/` |
| NAPI WindowAnimation | `interfaces/kits/napi/graphic/animation/window_animation_manager/` |
| NAPI Text | `frameworks/text/interface/mlb/napi/` |
| CJ 构建入口 | `interfaces/kits/cj/BUILD.gn` |
| CJ ColorManager | `interfaces/kits/cj/color_manager/` |
| CJ EffectKit | `interfaces/kits/cj/effect_kit/` |
| ANI 构建入口 | `interfaces/kits/ani/BUILD.gn` |
| ANI ColorManager | `interfaces/kits/ani/color_manager/` |
| ANI Drawing | `interfaces/kits/ani/drawing/` |
| ANI EffectKit | `interfaces/kits/ani/effect_kit/` |
| ANI HDR Capability | `interfaces/kits/ani/hdr_capability/` |
| ANI UIEffect | `interfaces/kits/ani/ui_effect/` |
| ANI WindowAnimation | `interfaces/kits/ani/window_animation_manager/` |
| ANI Text | `frameworks/text/interface/mlb/ani/` |
| Taihe 构建入口 | `interfaces/kits/taihe/BUILD.gn` |
| Taihe UIEffect | `interfaces/kits/taihe/ui_effect/` |

## 核心模型

### 四层语言绑定

本仓通过 `interfaces/kits/` 下四个子目录为同一图形能力提供四种语言绑定：

1. **NAPI**（`interfaces/kits/napi/`）：JavaScript/TypeScript API，面向 ArkTS 应用和系统应用。是覆盖面最广的绑定层。
2. **CJ**（`interfaces/kits/cj/`）：仓颉语言 FFI 绑定，面向仓颉应用。当前仅覆盖 color_manager 和 effect_kit。
3. **ANI**（`interfaces/kits/ani/`）：ArkUI Native Interface 绑定，面向 C/C++ 原生模块。覆盖 color_manager、drawing、effect_kit、hdr_capability、ui_effect、window_animation_manager 和 text。
4. **Taihe**（`interfaces/kits/taihe/`）：太合语言绑定。当前仅覆盖 ui_effect。

### NAPI 模块列表

NAPI 是最完整的绑定层，`napi/BUILD.gn` 中 `napi_packages` group 包含以下 target：

| target 名 | 模块 |
| --- | --- |
| `drawingnapi` | 2D 绘制（Canvas、Pen、Path 等） |
| `colorspacemanager_napi` | 色彩空间管理 |
| `sendablecolorspacemanager_napi` | Sendable 色彩空间管理 |
| `effectkit` | 效果滤镜 |
| `hdrcapability_napi` | HDR 能力查询 |
| `libhgmnapi` | 超图形管理器（刷新率控制） |
| `uieffect_napi` | UI 特效 |
| `libwebglnapi` | WebGL |
| `windowanimationmanager_napi` | 窗口动画管理 |
| `textnapi` | 文本排版（路径在 `frameworks/text/interface/mlb/napi/`） |

### CJ 模块列表

| target 名 | 模块 |
| --- | --- |
| `cj_color_manager_ffi` | 色彩空间管理 |
| `cj_effect_kit_ffi` | 效果滤镜 |

### ANI 模块列表

| target 名 | 模块 |
| --- | --- |
| `ani_color_space_manager` | 色彩空间管理 |
| `ani_drawing` | 2D 绘制 |
| `effectKit_ani` / `effectKit_etc` | 效果滤镜 |
| `ani_hdr_capability` | HDR 能力查询 |
| `window_animation_manager_taihe_group` | 窗口动画管理 |
| `text_engine_ani_group` | 文本排版（路径在 `frameworks/text/interface/mlb/ani/`） |

### Taihe 模块列表

| target 名 | 模块 |
| --- | --- |
| `uieffect_taihe_native` / `uiEffect_etc` | UI 特效 |

### inner_kits 中的多语言头文件

`bundle.json` 的 `inner_kits` 声明了多语言绑定的公开头文件：

- NAPI Drawing：`canvas_napi/js_canvas.h`（header_base: `interfaces/kits/napi/graphic/drawing`）
- NAPI Text：`paragraph_napi/js_paragraph.h`（header_base: `frameworks/text/interface/mlb/napi`）
- NAPI ColorManager：`color_space_object_convertor.h`、`js_color_space.h` 等
- NAPI HDR：`js_hdr_format_utils.h`
- CJ ColorManager：`cj_color_manager.h`、`cj_color_mgr_utils.h`
- ANI ColorManager：`ani_color_space_object_convertor.h`
- ANI Drawing：`canvas_ani/ani_canvas.h`
- ANI Text：`ani_paragraph.h`

### 同步约束

同一图形能力在多种语言中暴露时，行为必须一致。例如：
- color_manager 在 NAPI、CJ、ANI 三种绑定中均有暴露，底层都调用 `utils/color_manager/` 和 `interfaces/inner_api/color_manager/`。
- effect_kit 在 NAPI、CJ、ANI、Taihe 四种绑定中均有暴露，底层调用 `rosen/modules/effect/`。
- window_animation_manager 在 NAPI 和 ANI 中暴露。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 四语言并列目录 | `interfaces/kits/{napi,cj,ani,taihe}/` | 各语言绑定独立构建、独立部署、独立版本管理，互不影响 |
| NAPI 覆盖最广 | NAPI 有 10 个子模块，CJ 仅 2 个，Taihe 仅 1 个 | 优先保障 ArkTS 生态完整性，新语言绑定按需求逐步扩展 |
| Text 独立于 kits 目录 | `frameworks/text/interface/mlb/napi/` 和 `ani/` 不在 `interfaces/kits/` 下 | Text 模块有独立的构建和版本路径，通过 `$rosen_text_root` 变量引用 |
| ANI WindowAnimation 使用 taihe_group 命名 | `ani/BUILD.gn` 中 `window_animation_manager:window_animation_manager_taihe_group` | ANI 版窗口动画管理器内部复用了 Taihe 层实现，命名反映了代码复用关系 |
| base_group 包含全部语言绑定 | `bundle.json` 中 `base_group` 包含 `napi_packages`、`ffi_packages`、`ani_packages`、`taihe_packages` | 语言绑定属于基础能力，所有产品形态都必须部署 |
| inner_kits 声明头文件 | `bundle.json` 中为 CJ、ANI 等绑定声明公开头文件 | 允许其他部件依赖这些绑定的头文件进行二次开发 |

## 待补充背景

这些内容需要模块责任人后续补充，不能仅靠静态扫描完全确定：

- 每种语言绑定的 XTS 测试覆盖范围和验证策略。
- CJ/ANI/Taihe 新增绑定的开发规范和模板（当前只能从已有代码推断）。
- Sendable ColorManager（`sendablecolorspacemanager_napi`）与普通 ColorManager 的运行时差异。
- ANI 版 Drawing 和 NAPI 版 Drawing 的功能差异（ANI 可能是子集）。
- Taihe UIEffect 的 API 设计与 NAPI UIEffect 的对应关系。
- 各语言绑定中 syscap 声明与 `bundle.json` 中 syscap 列表的一致性。
- 改动公开行为时，四层语言绑定需要同步修改的具体流程和检查清单。
