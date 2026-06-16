# Text Framework (Texgine)

## 适用范围

- 文本渲染服务框架（service 层）
- Texgine 文本引擎集成
- SkiaTxt 文本渲染后端
- 全局字体配置（global_config）
- 文本效果（text_effect）
- MLB 接口层
- NDK 文本接口
- 文本适配层（adapter）

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| service/ | `frameworks/text/service/` | 文本渲染服务 |
| service/texgine/ | `frameworks/text/service/texgine/` | Texgine 文本引擎 |
| service/skia_txt/ | `frameworks/text/service/skia_txt/` | SkiaTxt 文本渲染后端 |
| service/global_config/ | `frameworks/text/service/global_config/` | 全局字体配置 |
| service/text_effect/ | `frameworks/text/service/text_effect/` | 文本效果 |
| service/common/ | `frameworks/text/service/common/` | 公共工具 |
| interface/export/ | `frameworks/text/interface/export/` | 对外导出接口 |
| interface/mlb/ | `frameworks/text/interface/mlb/` | MLB 接口 |
| interface/ndk_src/ | `frameworks/text/interface/ndk_src/` | NDK 文本接口源码 |
| adapter/ | `frameworks/text/adapter/` | 平台适配层 |
| utils/ | `frameworks/text/utils/` | 文本工具 |
| test/ | `frameworks/text/test/` | 测试 |
| config.gni | `frameworks/text/config.gni` | 构建配置 |
| BUILD.gn | `frameworks/text/BUILD.gn` | 构建入口 |

## 核心模型

文本框架采用 **服务层 + 接口层 + 适配层** 三层架构：

```
接口层 (interface/)
  ├─ export/  → 对外 C++ 导出接口
  ├─ mlb/     → MLB 语言绑定接口
  └─ ndk_src/ → NDK C 接口实现

服务层 (service/)
  ├─ texgine/     → Texgine 文本引擎（主引擎）
  ├─ skia_txt/    → SkiaTxt 文本后端（备选/兼容后端）
  ├─ global_config/ → 全局字体配置管理
  ├─ text_effect/   → 文本装饰效果
  └─ common/        → 服务公共逻辑

适配层 (adapter/)
  → 屏蔽不同平台的字体管理、文本布局差异
```

**Texgine** 是 OpenHarmony 自研文本引擎，负责：
- 文本布局（换行、 bidi、脚本项化）
- 字体选择与 fallback
- 文本测量与字形定位

**SkiaTxt** 是基于 Skia 的文本渲染后端，用于：
- 与 Skia 渲染管线紧密集成的场景
- 作为 Texgine 的备选/兼容后端

**全局配置**（global_config）管理：
- 系统字体路径与注册
- 默认字体族配置
- 字体 fallback 链

数据流：上层文本请求 → 接口层 → 服务层（Texgine/SkiaTxt 布局）→ 字形数据 → 2D Graphics Font/TextBlob 渲染。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 双引擎架构 | texgine/ + skia_txt/ | Texgine 自研引擎逐步替换 SkiaTxt，双引擎共存保证兼容 |
| 三层分离 | interface / service / adapter | 接口稳定，服务可替换，适配层隔离平台差异 |
| 独立 NDK 接口 | ndk_src/ 目录 | 文本 NDK 与 2D Graphics NDK 分开管理 |
| MLB 接口 | interface/mlb/ | 多语言绑定统一接口 |
| global_config 独立模块 | service/global_config/ | 字体配置集中管理，支持动态更新 |

## 待补充背景

- Texgine 的完整文本布局流程（从输入字符串到 TextBlob）
- SkiaTxt 与 Texgine 的切换条件和共存策略
- global_config 的字体注册与 fallback 链配置细节
- text_effect 支持的效果类型
- MLB 接口的具体功能与调用方式
- 文本框架与 2D Graphics Text 模块的分工边界
