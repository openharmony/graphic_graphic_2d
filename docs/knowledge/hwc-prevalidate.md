# HWC Prevalidate

## 适用范围

- Layer 信息创建与收集
- HWC 预协商策略请求
- 硬件特性处理（ARSR/Copybit/VCLD/硬件光标）
- HWC 事件处理

## 快速代码地图

| 关键文件 | 相对路径 | 说明 |
| --- | --- | --- |
| RSUniHwcPrevalidateUtil | `rs_uni_hwc_prevalidate_util.h` | 预协商工具单例，负责 Layer 信息创建、策略请求、硬件特性处理；定义 RequestLayerInfo、RequestCompositionType 等核心结构 |
| RequestRect | `rs_uni_hwc_prevalidate_common.h` | 基础矩形结构体，包含 x、y、w、h 字段；定义 PreValidateInitFunc 函数指针类型 |

## 核心模型

### RSUniHwcPrevalidateUtil 单例与动态加载

RSUniHwcPrevalidateUtil 采用单例模式，在构造函数中动态加载 libprevalidate_client.z.so，获取四个函数指针：
- InitPrevalidate：初始化预协商库
- RequestLayerStrategy：请求 Layer 策略
- HandleHWCEvent：处理 HWC 事件
- GetVcldEnabled：查询 VCLD 使能状态

### Layer 信息创建

**CreateSurfaceNodeLayerInfo**：创建 Surface 节点的 Layer 信息
- srcRect：处理 crop metadata 缩放
- dstRect：处理 ROG 分辨率适配
- layerUsage：设置硬件光标、纯色层等标记
- perFrameParameters：设置 SourceCropTuning、ArsrDoEnhance、VcldParam、LayerLinearMatrix 等

**CreateScreenNodeLayerInfo**：创建 Screen 节点的 Layer 信息
**CreateRCDLayerInfo**：创建圆角显示节点 Layer 信息
**CollectSurfaceNodeLayerInfo**：收集所有 Surface 节点的 Layer 信息

### PreValidate 策略请求

调用动态库的 RequestLayerStrategy，传入 Layer 信息列表，返回合成策略映射：
- CLIENT：GPU 合成
- DEVICE：硬件合成
- OFFLINE_DEVICE：离线设备合成（HPAE_OFFLINE_DEVICE、GPU_OFFLINE_DEVICE）
- DEVICE_VSCF、DEVICE_VCLD_OFF、OFFLINE_VCLD_OFF

### 硬件特性处理

- **ARSR 增强**：YUV buffer 或特定 video layer 触发，设置 perFrameParameters["ArsrDoEnhance"]
- **Copybit 加速**：YUV buffer + DSS 旋转时触发，设置 perFrameParameters["TryToDoCopybit"]
- **VCLD 处理**：查询使能状态，传递 VcldParam 参数
- **硬件光标**：判断 IsHardwareEnabledTopSurface + GetHardCursorStatus，设置 USAGE_HARDWARE_CURSOR 标记
- **统一渲染图层**：USAGE_UNI_LAYER
- **其他标记**：USAGE_SOLID_LAYER_ENABLE、USAGE_NONE_PREMULTIPLIED

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| 动态加载策略库 | dlopen("libprevalidate_client.z.so") | 解耦平台差异 |
| HWC事件传递 | HandleHWCEvent | 传递HWC事件，预协商结果更准确 |
| perFrameParameters 扩展 | std::map<std::string, std::vector<int8_t>> | 灵活传递参数 |
| ScreenNode ROG适配 | IsRogResolution()=true时使用render resolution，反之使用phy resolution | RequestLayerStrategy与完整hwc流程存在差异 |
| SurfaceNode ROG适配 | !screenProperty.IsRogResolution() || screenProperty.GetHdiRogEnable() || IsPointerWindow(node)使用render resolution，反之使用phy resolution | RequestLayerStrategy与完整hwc流程存在差异 |

## 待补充背景

- libprevalidate_client.z.so 的接口规范和返回值含义
- ARSR/Copybit/VCLD 的详细触发条件和参数配置
- RequestCompositionType与OfflineDeviceType的实际含义、使用场景
- perFrameParameters 各字段的完整定义
