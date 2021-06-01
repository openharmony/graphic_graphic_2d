# graphic_standard

-   [简介](#简介)
-   [目录](#目录)
-   [约束](#约束)
-   [编译构建](#编译构建)
-   [接口说明](#接口说明)
-   [使用说明](#使用说明)
-   [相关仓](#相关仓)

## 简介

**Graphic子系统** 提供了图形接口能力和窗口管理接口能力，

其主要的结构如下图所示：

![Graphic子系统架构图](./figures/graphic.png)

- **Surface**

    图形缓冲区管理接口，负责管理图形缓冲区和高效便捷的轮转缓冲区。

- **Vsync**

    垂直同步信号管理接口，负责管理所有垂直同步信号注册和响应。

- **WindowManager**

    窗口管理器接口，负责创建和管理窗口。

- **WaylandProtocols**

    窗口管理器和合成器的通信协议。

- **Compositor**

    合成器，负责合成各个图层。

- **Renderer**

    合成器的后端渲染模块。

- **Wayland protocols**

    Wayland 进程间通信协议

- **Shell**

    提供多窗口能力

- **Input Manger**

    多模输入模块，负责接收事件输入


## 目录
```
foundation/graphic/standard/
├── frameworks              # 框架代码目录
│   ├── bootanimation       # 开机动画目录
│   ├── surface             # Surface代码
│   ├── vsync               # Vsync代码
│   └── wm                  # WindowManager代码
├── interfaces              # 对外接口存放目录
│   ├── innerkits           # native接口存放目录
│   └── kits                # js/napi接口存放目录
└── utils                   # 小部件存放目录
```

## 约束
- 语言版本
    - C++11或以上

## 编译构建
可以依赖的接口有:
- graphic_standard:libwms_client
- graphic_standard:libsurface
- graphic_standard:libvsync_client

## 接口说明

### WindowManager

| 接口名          | 职责                        |
|-----------------|-----------------------------|
| GetInstance     | 获取WindowManager的单例指针 |
| GetMaxWidth     | 获取当前屏幕宽度            |
| GetMaxHeight    | 获取当前屏幕高度            |
| CreateWindow    | 创建一个标准窗口            |
| CreateSubWindow | 创建一个子窗口              |
| StartShotScreen | 截屏操作                    |
| StartShotWindow | 截取窗口操作                |
| SwitchTop       | 将指定窗口调整至最上层显示  |

### Window
| 接口名                      | 职责                         |
|-----------------------------|------------------------------|
| Show                        | 显示当前窗口                 |
| Hide                        | 隐藏当前窗口                 |
| Move                        | 移动当前窗口至指定位置       |
| SwitchTop                   | 将当前窗口调整到最上层显示   |
| ChangeWindowType            | 更改当前窗口类型             |
| ReSize                      | 调整当前窗口至指定大小       |
| Rotate                      | 旋转当前窗口                 |
| RegistPointerButtonCb       | 注册鼠标Button事件回调       |
| RegistPointerEnterCb        | 注册鼠标Enter事件回调        |
| RegistPointerLeaveCb        | 注册鼠标Leave事件回调        |
| RegistPointerMotionCb       | 注册鼠标Motion事件回调       |
| RegistPointerAxisDiscreteCb | 注册鼠标AxisDiscrete事件回调 |
| RegistPointerAxisSourceCb   | 注册鼠标AxisSource事件回调   |
| RegistPointerAxisStopCb     | 注册鼠标AxisStop事件回调     |
| RegistPointerAxisCb         | 注册鼠标Axis事件回调         |
| RegistTouchUpCb             | 注册TouchUp事件回调          |
| RegistTouchDownCb           | 注册TouchDown事件回调        |
| RegistTouchEmotionCb        | 注册TouchEmotion事件回调     |
| RegistTouchFrameCb          | 注册TouchFrame事件回调       |
| RegistTouchCancelCb         | 注册TouchCancel事件回调      |
| RegistTouchShapeCb          | 注册TouchShape事件回调       |
| RegistTouchOrientationCb    | 注册TouchOrientation事件回调 |
| RegistKeyboardKeyCb         | 注册键盘Key事件回调          |
| RegistKeyboardKeyMapCb      | 注册键盘KeyMap事件回调       |
| RegistKeyboardLeaveCb       | 注册键盘Leave事件回调        |
| RegistKeyboardEnterCb       | 注册键盘Enter事件回调        |
| RegistKeyboardRepeatInfoCb  | 注册键盘RepeatInfo事件回调   |

### SubWindow
| 接口名           | 职责               |
|------------------|--------------------|
| Move             | 移动当前子窗口     |
| SetSubWindowSize | 调整当前子窗口位置 |

### Surface
| 接口名                     | 职责                                                              |
|----------------------------|-------------------------------------------------------------------|
| CreateSurfaceAsConsumer    | Buffer的消费者来使用该函数创建一个Surface                         |
| CreateSurfaceAsProducer    | Buffer的生产者使用该函数创建一个Surface，只能使用与生产相关的接口 |
| GetProducer                | 获得一个Surface内部的IBufferProducer对象                          |
| RequestBuffer              | 请求一个待生产的SurfaceBuffer对象                                 |
| CancelBuffer               | 取消、归还一个待生产的SurfaceBuffer对象                           |
| FlushBuffer                | 归还一个生产好的SurfaceBuffer对象并携带一些信息                   |
| AcquireBuffer              | 请求一个待消费的SurfaceBuffer对象                                 |
| ReleaseBuffer              | 归还一个已消费的SurfaceBuffer对象                                 |
| GetQueueSize               | 获得当前同时能并存buffer的数量                                    |
| SetQueueSize               | 设置当前同时能并存buffer的数量                                    |
| SetDefaultWidthAndHeight   | 设置默认宽和高                                                    |
| GetDefaultWidth            | 获得默认宽度                                                      |
| GetDefaultHeight           | 获得默认高度                                                      |
| SetUserData                | 存贮字符串数据，不随着IPC传递                                     |
| GetUserData                | 取出字符串数据                                                    |
| RegisterConsumerListener   | 注册一个消费监听器，监听Buffer的Flush事件                         |
| UnregisterConsumerListener | 取消监听                                                          |

### SurfaceBuffer
| 接口名          | 职责                                |
|-----------------|-------------------------------------|
| GetBufferHandle | 获得SurfaceBuffer的BufferHandle指针 |
| GetWidth        | 获得SurfaceBuffer的宽度             |
| GetHeight       | 获得SurfaceBuffer的高度             |
| GetFormat       | 获得SurfaceBuffer的颜色格式         |
| GetUsage        | 获得SurfaceBuffer的用途             |
| GetPhyAddr      | 获得SurfaceBuffer的物理地址         |
| GetKey          | 获得SurfaceBuffer的key              |
| GetVirAddr      | 获得SurfaceBuffer的虚拟地址         |
| GetSize         | 获得SurfaceBuffer的文件句柄         |
| SetInt32        | 获得SurfaceBuffer的缓冲区大小       |
| GetInt32        | 设置SurfaceBuffer的32位整数         |
| SetInt64        | 获得SurfaceBuffer的32位整数         |
| GetInt64        | 设置SurfaceBuffer的64位整数         |

### VsyncHelper
| 接口名               | 职责                              |
|----------------------|-----------------------------------|
| Current              | 获取当前runner对应的VsyncHelper   |
| VsyncHelper          | 用EventHandler对象构造VsyncHelper |
| RequestFrameCallback | 注册一个帧回调                    |

## 使用说明

### 具名服务-传递一个生产型Surface

#### 注册
```cpp
// 拿到一个消费型Surface
sptr<Surface> surface = Surface::CreateSurfaceAsConsumer();

// 拿出里面的生产者对象
sptr<IBufferProducer> producer = surface->GetProducer();

// 注册服务
auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
sm->AddSystemAbility(IPC_SA_ID, producer->AsObject());
```

#### 客户端获得生产型Surface
```cpp
// 获得远程对象
auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
sptr<IRemoteObject> robj = sm->GetSystemAbility(IPC_SA_ID);

// 构造Surface
sptr<IBufferProducer> bp = iface_cast<IBufferProducer>(robj);
sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bp);
```

### 匿名服务-传递一个生产型Surface
场景: 在一次IPC过程中

#### 发送
```cpp
// 拿到一个消费型Surface
sptr<Surface> surface = CreateSurfaceAsConsumer();

// 拿出里面的生产者对象
sptr<IRemoteObject> producer = surface->GetProducer();

// 返回给客户端
parcel.WriteRemoteObject(producer);
```

#### 接受并获得生产型Surface
```cpp
// 获得远程对象
sptr<IRemoteObject> remoteObject = parcel.ReadRemoteObject();

// 构造Surface
sptr<IBufferProducer> bp = iface_cast<IBufferProducer>(robj);
sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bp);
```

### 生产一个SurfaceBuffer
条件： 一个生产型Surface

```cpp
BufferRequestConfig requestConfig = {
    .width = 1920, // 屏幕宽度
    .height = 1080, // 屏幕高度
    .strideAlignment = 8, // stride对齐字节
    .format = PIXEL_FMT_RGBA_8888, // 颜色格式
    .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA, // 用法
    .timeout = 0, // 时延
};

sptr<SurfaceBuffer> buffer;
int32_t releaseFence;

SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, requestConfig);
if (ret != SURFACE_ERROR_OK) {
    // failed
}

BufferFlushConfig flushConfig = {
    .damage = {                   // 重绘区域
        .x = 0,                   // 起点横坐标
        .y = 0,                   // 起点纵坐标
        .w = buffer->GetWidth(),  // 区域宽度
        .h = buffer->GetHeight(), // 区域高度
    },
    .timestamp = 0 // 给消费者看的时间，0为使用当前时间
};

ret = surface->FlushBuffer(buffer, -1, flushConfig);
if (ret != SURFACE_ERROR_OK) {
    // failed
}
```

### 消费一个SurfaceBuffer
条件： 一个消费型Surface

```cpp
class TestConsumerListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override {
        sptr<SurfaceBuffer> buffer;
        int32_t flushFence;

        SurfaceError ret = surface->AcquireBuffer(buffer, flushFence, timestamp, damage);
        if (ret != SURFACE_ERROR_OK) {
            // failed
        }

        // ...

        ret = surface->ReleaseBuffer(buffer, -1);
        if (ret != SURFACE_ERROR_OK) {
            // failed
        }
    }
};

sptr<IBufferConsumerListener> listener = new TestConsumerListener();
SurfaceError ret = surface->RegisterConsumerListener(listener);
if (ret != SURFACE_ERROR_OK) {
    // failed
}
```

### 给SurfaceBuffer带上自定义数据
```cpp
sptr<SurfaceBuffer> buffer;
SurfaceError ret = buffer->SetInt32(1, 3);
if (ret != SURFACE_ERROR_OK) {
    // failed
}

int32_t val;
ret = buffer->GetInt32(1, val);
if (ret != SURFACE_ERROR_OK) {
    // failed
}
```

### 注册一个Vsync回调事件
#### 用handler构造VsyncHelper
```cpp
auto runner = AppExecFwk::EventRunner::Create(true);
auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
auto helper = new VsyncHelper(handler);
runner->Run();

struct FrameCallback cb = {
    .timestamp_ = 0,
    .userdata_ = nullptr,
    .callback_ = [](int64_t timestamp, void* userdata) {
    },
};

VsyncError ret = helper->RequestFrameCallback(cb);
if (ret != VSYNC_ERROR_OK) {
    // failed
}
```

#### 在handler里用Current
```cpp
auto runner = AppExecFwk::EventRunner::Create(true);
auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
handler->PostTask([]() {
    auto helper = VsyncHelper::Current();
    struct FrameCallback cb = {
        .timestamp_ = 0,
        .userdata_ = nullptr,
        .callback_ = [](int64_t timestamp, void* userdata) {
        },
    };

    VsyncError ret = helper->RequestFrameCallback(cb);
    if (ret != VSYNC_ERROR_OK) {
        // failed
    }
});

runner->Run();
```

## 相关仓
- **graphic_standard**
- ace_ace_engine
- aafwk_standard
- multimedia_media_standard
- multimedia_camera_standard
