# Vulkan Layers / Trace3D

## 适用范围

- Vulkan Swapchain Layer（VkLayer_swapchain）拦截与增强
- Trace3D 捕获层（VkLayer_Trace3D）GPU 帧捕获
- Trace3D API Platform 平台接口
- Vulkan Layer JSON 配置
- Swapchain 行为定制（如 HDR、保护内容）

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| swapchain_layer.cpp | `frameworks/vulkan_layers/swapchain_layer/swapchain_layer.cpp` | VkLayer_swapchain 实现 |
| VkLayer_swapchain.json | `frameworks/vulkan_layers/swapchain_layer/VkLayer_swapchain.json` | Swapchain Layer Vulkan 层配置 |
| trace3d_api_platform.h | `frameworks/vulkan_layers/trace3d/include/trace3d/api_platform/trace3d_api_platform.h` | Trace3D 平台 API 接口 |
| VkLayer_Trace3D.json | `frameworks/vulkan_layers/trace3d/VkLayer_Trace3D.json` | Trace3D Layer Vulkan 层配置 |
| swapchain_layer_log.h | `frameworks/vulkan_layers/include/swapchain_layer_log.h` | Swapchain Layer 日志 |
| BUILD.gn | `frameworks/vulkan_layers/BUILD.gn` | 构建配置 |

## 核心模型

**Swapchain Layer** 是一个 Vulkan 显式层（Explicit Layer），拦截 `vkCreateSwapchainKHR` 等 Swapchain 相关调用：

- 通过 Vulkan Layer 机制在应用和驱动之间插入
- `VkLayer_swapchain.json` 声明层名称、描述、API 版本和拦截的函数
- 可在创建 Swapchain 时修改参数（如格式、颜色空间、保护内容标志）

**Trace3D Layer** 是 GPU 帧捕获层：

- `VkLayer_Trace3D.json` 声明层配置
- `trace3d_api_platform.h` 定义平台接口：
  - 版本控制：`TRACE3D_API_VERSION(maj, min, patch)` 宏编码为 uint32_t
  - 当前平台版本：0.1.0
  - Magic Number：`PLATFORM_MAGIC = "#TRACE3D"` 的 uint64 编码
  - 错误码：`RetCodePlatform` 枚举（SUCCESS / ERR_UNKNOWN / ERR_INVALID_VERSION / ERR_UNSUPPORTED / ERR_INVALID_PARAM / ERR_ACCESS_DENIED / ERR_INIT / HRP_SERVICE 错误）
  - HRP Service 目录类型：`HrpServiceDir`（UNKNOWN / COMMON / RS / TRACE3D）
- 捕获库：`libVkLayer_Trace3DCapture.so`
- Layer 参数：`trace3d.layer`

数据流：应用 Vulkan 调用 → VkLayer 拦截 → 修改/记录/转发 → GPU 驱动执行。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| Vulkan Explicit Layer | JSON 配置 + 层拦截 | 标准化 Vulkan 层机制，可按需启用禁用 |
| Trace3D API 版本化 | `TRACE3D_API_VERSION` 宏 | 平台接口版本兼容性检查 |
| Magic Number 校验 | `PLATFORM_MAGIC` | 防止非法或版本不匹配的库加载 |
| HRP Service 集成 | `HrpServiceDir` 枚举 + HRP 错误码 | Trace3D 捕获结果需要通过 HRP 服务存储和传输 |
| Swapchain Layer 独立编译 | 单独 BUILD.gn target | 可按产品裁剪是否包含 Swapchain 增强 |

## 待补充背景

- Swapchain Layer 具体拦截和修改了哪些参数
- Trace3D 帧捕获的完整工作流（捕获 → 存储 → 回放）
- HRP Service 的接口定义和调用方式
- Trace3D 与 Render Service 的交互方式
- Trace3D 在不同产品上的部署和启用策略
