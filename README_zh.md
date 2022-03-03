# graphic_standard

-   [简介](#简介)
-   [目录](#目录)
-   [约束](#约束)
-   [编译构建](#编译构建)
-   [接口说明](#接口说明)
-   [使用说明](#使用说明)
-   [相关仓](#相关仓)

## 简介

**Graphic子系统** 提供了图形接口能力。

其主要的结构如下图所示：

![Graphic子系统架构图](./figures/graphic_rosen_architecture.jpg)

OpenHarmony 图形栈的分层说明如下：

• 接口层：提供图形的 NDK（native development kit，原生开发包）能力，包括：WebGL、Native Drawing的绘制能力、OpenGL 指令级的绘制能力支撑等。

• 框架层：分为 Render Service、Drawing、Animation、Effect、显示与内存管理五个模块。
| 模块                     | 能力描述                                                                                       |
|------------------------|--------------------------------------------------------------------------------------------|
| Render Servicel （渲染服务） | 提供UI框架的绘制能力，其核心职责是将ArkUI的控件描述转换成绘制树信息，根据对应的渲染策略，进行最佳路径渲染。同时，负责多窗口流畅和空间态下UI共享的核心底层机制。       |
| Drawing （绘制）           | 提供图形子系统内部的标准化接口，主要完成2D渲染、3D渲染和渲染引擎的管理等基本功能。                                                |
| Animation (动画）         | 提供动画引擎的相关能力。                                                                               |
| Effect （效果）            | 主要完成图片效果、渲染特效等效果处理的能力，包括：多效果的串联、并联处理，在布局时加入渲染特效、控件交互特效等相关能力。                               |
| 显示与内存管理                | 此模块是图形栈与硬件解耦的主要模块，主要定义了OpenHarmony 显示与内存管理的能力，其定义的南向HDI 接口需要让不同的OEM厂商完成对OpenHarmony图形栈的适配． |

• 引擎层：包括 2D 图形库和 3D 图形引擎两个模块。2D 图形库提供 2D 图形绘制底层 API，支持图形绘制与文本绘制底层能力。3D 图形引擎能力尚在构建中。


## 目录
```
foundation/graphic/standard/
├── figures                 # Markdown引用的图片目录
├── frameworks              # 框架代码目录
│   ├── animation_server    # AnimationServer代码
│   ├── bootanimation       # 开机动画目录
│   ├── dumper              # graphic dumper代码
│   ├── fence               # fence代码
│   ├── surface             # Surface代码
│   ├── vsync               # Vsync代码
├── rosen                   # 框架代码目录
│   ├── build               # 构建说明
│   ├── doc                 # doc
│   ├── include             # 对外头文件代码
│   ├── lib                 # lib
│   ├── modules             # graphic 子系统各模块代码
│   ├── samples             # 实例代码
│   ├── test                # 开发测试代码
│   ├── tools               # 工具代码
├── interfaces              # 图形接口存放目录
│   ├── innerkits           # 内部native接口存放目录
│   └── kits                # js/napi外部接口存放目录
└── utils                   # 小部件存放目录
```

## 约束


## 编译构建


## 接口说明


## 使用说明


## 相关仓
- **graphic_standard**
- ace_ace_engine
- aafwk_standard
- multimedia_media_standard
- multimedia_camera_standard
- Window Manager
