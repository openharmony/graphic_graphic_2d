# DDGR / 2D Engine

## 适用范围

- DDGR (Device-Driven Graphics Rendering) 2D 渲染引擎
- GPU 加速 2D 绘制替代 Skia 后端
- Shader 注册与编译
- DDGR 编译器模块（libddgr_compiler）
- 条件编译与产品裁剪

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| BUILD.gn | `rosen/modules/2d_engine/ddgr/BUILD.gn` | DDGR 构建配置 |
| libddgr.versionscript | `rosen/modules/2d_engine/ddgr/libddgr.versionscript` | 符号版本脚本 |

## 核心模型

DDGR 是图形栈的 2D 渲染引擎模块，通过 `graphic_2d_ext_feature_enable_ddgr` 构建宏控制是否启用。

**构建结构**（从 BUILD.gn 分析）：

```
libddgr (主库)
  ├─ ddgr_src_set → 核心渲染实现源码
  ├─ ddgr_config  → 编译配置
  └─ public_ddgr_config → 对外暴露的编译配置

libddgr_compiler (编译器库)
  ├─ ddgr_compiler_src_set → 着色器编译实现
  ├─ ddgr_compiler_config  → 编译配置
  └─ generate_shader_register_code → 着色器注册代码生成
```

**条件编译**：
- 仅当 `graphic_2d_ext_feature_enable_ddgr` 或 `ddgr_feature_cross_compile` 定义时才构建实际代码
- 否则生成空壳 `libddgr` 共享库（sources = []）

**着色器注册代码生成**：
- `ddgr_shader_register_set.script` 指定代码生成脚本
- `ddgr_shader_register_set.sources` 指定输入
- `ddgr_shader_register_set.outputs` 指定输出
- 生成的代码由 `ddgr_src_set` 和 `ddgr_compiler_src_set` 依赖

**安全加固**：
- CFI（Control Flow Integrity）全部启用：`cfi = true`、`cfi_cross_dso = true`、`cfi_no_nvcall = true`
- ARM64 特定链接选项：`-Wl,--emit-relocs`、`-Wl,--no-relax`、`-mno-fix-cortex-a53-843419`

**外部配置**：所有源码、编译标志、依赖均通过 `ddgr_*_set` 变量从外部（产品扩展配置）注入，本仓 BUILD.gn 仅定义构建框架。

数据流：2D Graphics → DDGR 后端适配 → GPU 着色器执行 → 帧输出。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 条件编译控制 | `graphic_2d_ext_feature_enable_ddgr` | DDGR 为可选后端，未启用时不影响构建 |
| 编译器分离 | libddgr + libddgr_compiler 分库 | 编译器可独立部署/更新，主库运行时不需要编译器 |
| 着色器代码生成 | `generate_shader_register_code` action | 自动化着色器注册，减少手工维护 |
| 外部配置注入 | `ddgr_*_set` 变量 | 源码不在本仓，通过构建配置注入，解耦代码和构建 |
| CFI 全面启用 | 所有 target 均开启 CFI | 安全加固，防止控制流劫持 |
| 版本脚本 | `libddgr.versionscript` | 控制符号导出，保证 ABI 稳定 |

## 待补充背景

- DDGR 的完整 API 与 2D Graphics 后端适配接口
- DDGR 与 Skia 后端的切换机制
- 着色器注册代码生成的具体脚本与输入输出格式
- DDGR 支持的 2D 绘制操作范围
- DDGR 的 GPU 后端支持（是否仅限特定 GPU 厂商）
- ddgr_feature_cross_compile 的使用场景
