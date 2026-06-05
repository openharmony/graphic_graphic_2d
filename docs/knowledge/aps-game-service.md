# APS 与 Game Service

## 适用范围

- 应用性能调度插件（IApsPlugin）动态加载与调用
- 游戏服务插件（IGameServicePlugin）动态加载与调用
- 游戏 FPS 控制与 SwapBuffer 功耗管理
- EGL Slice 上报与 Vulkan 渲染上报

## 快速代码地图

| 模块 | 关键文件 | 相对路径 |
| --- | --- | --- |
| APS 插件接口 | `iaps_plugin.h` | `frameworks/aps_monitor/include/` |
| GameService 插件接口 | `igameservice_plugin.h` | `frameworks/gameservice_plugin/include/` |

## 核心模型

### IApsPlugin：应用性能调度插件

IApsPlugin 继承自 RefBase，通过 dlopen 动态加载 `libaps_client.z.so`，调用 `LoadApsPlugin` 导出函数获取实例。双重检查锁保证线程安全。

**加载流程**：
1. `Instance()` 首次调用 → 加锁 → `dlopen("libaps_client.z.so", RTLD_NOW)`
2. `dlsym(handle, "LoadApsPlugin")` 获取加载函数
3. 调用 `loadApsFunc(instance_)` 获取插件实例
4. 加载失败时回退到默认空实现

**虚函数接口**：

| 方法 | 功能 |
| --- | --- |
| `InitGameFpsCtrl()` | 初始化游戏 FPS 控制 |
| `PowerControlOfSwapbuffer()` | SwapBuffer 功耗控制 |

默认实现为空操作。具体功能由 `libaps_client.z.so` 提供。

### IGameServicePlugin：游戏服务插件

IGameServicePlugin 继承自 RefBase，通过 dlopen 动态加载 `libgameservice_graphic_plugin.z.so`，调用 `LoadGameServicePlugin` 导出函数获取实例。`std::call_once` 保证只初始化一次。

**加载流程**：
1. `Instance()` 首次调用 → `dlopen("libgameservice_graphic_plugin.z.so", RTLD_NOW)`
2. `dlsym(handle, "LoadGameServicePlugin")` 获取加载函数
3. 调用 `loadGameServiceFunc(instance_)` 获取插件实例
4. 加载失败时回退到默认空实现
5. 析构时 `dlclose(handle)`

**虚函数接口**：

| 方法 | 功能 |
| --- | --- |
| `InitEglSliceReport()` | 初始化 EGL Slice 上报 |
| `AddEglGraphicCount()` | 增加 EGL 图形计数 |
| `InitVulkanReport()` | 初始化 Vulkan 渲染上报 |
| `ReportVulkanRender()` | 上报 Vulkan 渲染事件 |

### 设计模式

两个插件均采用"接口 + dlopen 动态加载"模式：

1. **编译期解耦**：RS 不直接依赖 `libaps_client.z.so` 或 `libgameservice_graphic_plugin.z.so`，通过虚函数接口调用
2. **运行期加载**：仅在首次使用时 dlopen，加载失败回退到空实现
3. **RefBase 引用计数**：使用 `sptr` 管理，保证生命周期安全

## 设计背景与决策理由

| 决策 | 代码体现 | 设计意图 |
| --- | --- | --- |
| dlopen 动态加载 | `dlopen("libaps_client.z.so", RTLD_NOW)` | 编译解耦，产品可裁剪插件 so，不影响 RS 编译和运行 |
| 回退到空实现 | 加载失败 `instance_ = new IApsPlugin()` | 插件不可用时 RS 正常运行，不崩溃 |
| RefBase + sptr | `sptr<IApsPlugin> instance_` | 引用计数管理，跨线程安全 |
| IGameServicePlugin 析构 dlclose | `~IGameServicePlugin()` 中 `dlclose(loadFileHandle_)` | 释放动态库句柄，IApsPlugin 不释放（生命周期与进程相同） |
| call_once vs 双重检查锁 | IGameServicePlugin 用 `std::call_once`，IApsPlugin 用双重检查锁 | IApsPlugin 使用双重检查锁因为需要支持重新加载场景 |
| 虚函数默认空实现 | `virtual void InitGameFpsCtrl() {}` | 子类按需覆盖，RS 侧无需判空 |

## 待补充背景

- `libaps_client.z.so` 的具体实现和 `InitGameFpsCtrl`/`PowerControlOfSwapbuffer` 的实际行为。
- `libgameservice_graphic_plugin.z.so` 的具体实现和 EGL Slice/Vulkan 上报的数据流向。
- 游戏场景下 FPS 控制的策略细节（如何调整帧率、阈值是什么）。
- SwapBuffer 功耗控制的具体机制（是否涉及 GPU 频率调整、帧率限制）。
- 两个插件 so 在不同产品形态下的可用性（哪些产品包含、哪些裁剪）。
- `LoadApsPlugin`/`LoadGameServicePlugin` 导出函数的签名和版本兼容性。
- 插件加载失败的日志和告警机制。
- 历史线上问题：插件 so 缺失导致功能不可用、dlopen 失败等案例。
