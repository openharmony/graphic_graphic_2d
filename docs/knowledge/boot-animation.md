# BootAnimation

## 适用范围

- 开机动画显示（图片序列、视频）
- 开机声音播放
- 多屏幕开机动画
- OTA 编译进度显示
- 策略模式选择（独立显示/关联显示/兼容显示）
- 开机动画配置文件解析

## 快速代码地图

| 文件 | 相对路径 | 职责 |
|------|----------|------|
| main.cpp | `frameworks/bootanimation/src/main.cpp` | 入口 |
| boot_animation_controller.h | `frameworks/bootanimation/include/boot_animation_controller.h` | BootAnimationController 控制器 |
| boot_animation_strategy.h | `frameworks/bootanimation/include/boot_animation_strategy.h` | BootAnimationStrategy 策略基类 |
| boot_independent_display_strategy.h | `frameworks/bootanimation/include/boot_independent_display_strategy.h` | 独立显示策略 |
| boot_associative_display_strategy.h | `frameworks/bootanimation/include/boot_associative_display_strategy.h` | 关联显示策略 |
| boot_compatible_display_strategy.h | `frameworks/bootanimation/include/boot_compatible_display_strategy.h` | 兼容显示策略 |
| boot_animation_config.h | `frameworks/bootanimation/include/boot_animation_config.h` | BootAnimationConfig 配置结构 |
| boot_animation_factory.h | `frameworks/bootanimation/include/boot_animation_factory.h` | 策略工厂 |
| boot_animation_operation.h | `frameworks/bootanimation/include/boot_animation_operation.h` | 动画操作 |
| boot_player.h | `frameworks/bootanimation/include/boot_player.h` | 播放器基类 |
| boot_picture_player.h | `frameworks/bootanimation/include/boot_picture_player.h` | 图片播放器 |
| boot_video_player.h | `frameworks/bootanimation/include/boot_video_player.h` | 视频播放器 |
| boot_sound_player.h | `frameworks/bootanimation/include/boot_sound_player.h` | 声音播放器 |
| boot_compile_progress.h | `frameworks/bootanimation/include/boot_compile_progress.h` | OTA 编译进度 |
| bootanimation.cfg | `frameworks/bootanimation/bootanimation.cfg` | SA 配置 |
| util.h | `frameworks/bootanimation/include/util.h` | 工具函数 |

## 核心模型

开机动画采用 **策略模式** 架构：

```
BootAnimationController
  ├─ Start() → 入口
  ├─ WaitRenderServiceInit() → 等待 RS 服务就绪
  ├─ GetBootType() → 判断启动类型
  └─ 通过 BootAnimationFactory 创建策略

BootAnimationStrategy (策略基类)
  ├─ Display(duration, configs) → 纯虚函数
  ├─ CheckExitAnimation() → 检查是否退出
  ├─ CheckNeedOtaCompile() → 检查 OTA 编译
  ├─ SubscribeActiveScreenIdChanged() → 监听屏幕变化
  └─ connectToRenderMap_ → 屏幕 ID 到 RS 连接的映射

BootIndependentDisplayStrategy  → 各屏幕独立显示
BootAssociativeDisplayStrategy  → 多屏关联显示
BootCompatibleDisplayStrategy   → 兼容模式显示
```

**BootAnimationConfig** 配置：
- `picZipPath`：图片序列 ZIP 路径
- `soundPath`：声音文件路径
- `videoDefaultPath` / `videoExtraPath`：视频路径
- `screenId`：目标屏幕 ID
- `rotateScreenId` / `rotateDegree`：旋转配置
- `isFrameRateEnable`：帧率控制
- `videoExtPath`：扩展视频路径映射

**播放器**：
- `BootPicturePlayer`：逐帧播放图片序列（从 ZIP 解压）
- `BootVideoPlayer`：播放视频（依赖 Media Player 服务）
- `BootSoundPlayer`：播放声音（依赖 Media Player 服务）
- 均继承自 `BootPlayer` 基类

**退出条件**：
- `CheckExitAnimation()` 检查是否应该退出动画
- `FEATURE_CHECK_EXIT_ANIMATION_EXT` 扩展检查

数据流：系统启动 → BootAnimationController.Start() → 等待 RS 就绪 → 读取配置 → 创建策略 → 按屏幕创建 RS 连接 → 播放动画/声音 → 检查退出 → 结束。

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
|------|----------|----------|
| 策略模式 | BootAnimationStrategy + 三种派生策略 | 不同产品形态（单屏/多屏/兼容）使用不同显示策略 |
| 多屏支持 | connectToRenderMap_ + screenId | 多设备（折叠屏/外接屏）各自独立或关联显示开机动画 |
| 图片 ZIP 序列 | picZipPath 配置 | 图片序列预压缩，减少 IO 和存储开销 |
| Media Player 等待 | MAX_WAIT_MEDIA_CREATE_TIME = 5s | 开机早期 Media 服务可能未就绪，需等待 |
| OTA 编译进度 | BootCompileProgress | OTA 升级后首次启动需编译，显示编译进度 |
| 帧率控制 | isFrameRateEnable | 节省开机动画期间的功耗 |

## 待补充背景

- BootAnimationFactory 的策略选择逻辑
- 图片序列 ZIP 的格式要求与解压流程
- 多屏关联显示与独立显示的具体行为差异
- OTA 编译进度的获取与更新机制
- FEATURE_CHECK_EXIT_ANIMATION_EXT 扩展点的作用
