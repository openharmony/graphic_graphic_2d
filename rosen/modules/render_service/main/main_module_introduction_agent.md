# Render Service 进程管理模块介绍

## 1. 概述

进程管理模块是Rosen渲染服务的核心模块，负责render_service和render_process进程的创建、管理和生命周期控制。

## 2. 模块职责

### 2.1 核心职责
1. **进程创建管理**
    - 根据配置创建render_service主进程
    - 在多进程模式下fork子render_process进程
    - 管理子进程的初始化和启动

2. **进程生命周期管理**
    - fork的子进程不会动态销毁
    - 管理进程间的通信连接

3. **进程间通信协调**
    - 建立render_service和render_process之间的IPC连接
    - 管理连接的生命周期
    - 处理跨进程的消息传递

### 2.2 进程职责划分

#### render_service进程职责
- **系统服务（SA）**：作为系统服务注册到SA Manager，支持客户端通过Samgr获取
- **合成服务**：负责与composerManager交互，实际负责与硬件交互
- **进程管理**：作为主进程管理子进程的生命周期
- **资源协调**：协调screenManager、vsyncManager、composerManager、HGM等周边模块
- **IPC服务端**：作为Binder服务端响应客户端请求
- **硬件交互**：通过composerManager与硬件合成器交互，控制显示输出
- **事件循环**：拥有runner和handler，支持任务调度

#### render_process进程职责
- **渲染服务**：仅负责渲染工作
- **渲染管线**：包含mainThread和uniRenderThread，负责生成buffer
- **渲染线程管理**：
  - mainThread：主渲染线程，处理渲染任务
  - uniRenderThread：统一渲染线程，处理实际渲染操作
- **buffer生成**：通过渲染管线生成图形buffer
- **IPC客户端**：作为Binder客户端请求合成服务
- **服务获取**：子进程第一次通过Samgr获取render_service
- **IPC通信**：后续通过render_to_service_connection和service_to_render_connection通信
- **事件循环**：拥有runner和handler，支持任务调度
- **看门狗监控**：通过watchdog检测进程是否卡死

## 3. 模块文件结构

### 3.1 核心进程管理文件

#### 3.1.0 main.cpp（render_service主进程入口）
**功能**: render_service主进程的可执行入口程序。

**位置**: `main/render_server/main.cpp`

**主要职责**:
- 作为 `/system/bin/render_service` 可执行文件的入口
- 创建并初始化 RSRenderService 实例
- 设置进程调度优先级和策略
- 启动事件循环

**入口代码**:
```cpp
int main(int argc, const char *argv[])
{
    signal(SIGPIPE, SIG_IGN);  // 忽略SIGPIPE信号
    
    setpriority(PRIO_PROCESS, 0, -8);  // 设置进程优先级
    
    struct sched_param param = {0};
    param.sched_priority = 1;
    sched_setscheduler(0, SCHED_FIFO, &param);  // 设置实时调度策略
    
    auto renderService = sptr<RSRenderService>::MakeSptr();
    if (!renderService->Init()) {
        return -1;
    }
    
    renderService->Run();  // 启动事件循环
    return 0;
}
```

**进程初始化流程**:
1. **信号处理**: 忽略 SIGPIPE，防止管道破裂导致进程退出
2. **优先级设置**: 设置进程优先级为 -8（较高优先级）
3. **调度策略**: 使用 SCHED_FIFO 实时调度策略，保证渲染及时性
4. **服务创建**: 创建 RSRenderService 实例
5. **服务初始化**: 调用 Init() 初始化各子模块
6. **事件循环**: 调用 Run() 启动主事件循环

**与子进程入口对比**:
| 入口文件 | 进程类型 | 可执行路径 | 核心类 |
|----------|----------|------------|--------|
| `main/render_server/main.cpp` | 主进程 | `/system/bin/render_service` | RSRenderService |
| `main/render_process/rs_render_process_main.cpp` | 子进程 | `/system/bin/render_process` | RSRenderProcess |

---

#### 3.1.1 rs_render_service.h/cpp
**功能**: render_service主进程的核心类，作为系统服务（SA）运行。

**主要职责**:
- 作为系统服务注册到SA Manager，支持客户端通过Samgr获取
- 初始化和管理screenManager、vsyncManager、composerManager、HGM等周边模块
- 创建和管理工作进程管理器（单进程或多进程模式）
- 作为Binder服务端响应客户端请求
- 提供事件循环和任务调度机制

**核心成员**:
```cpp
class RSRenderService : public RSRenderServiceStub {
private:
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
    sptr<RSScreenManager> screenManager_;
    sptr<RSVsyncManager> vsyncManager_;
    sptr<RSRenderProcessManager> renderProcessManager_;
    std::shared_ptr<RSRenderComposerManager> rsRenderComposerManager_;
    std::shared_ptr<const RenderModeConfig> renderModeConfig_;
    std::shared_ptr<HgmContext> hgmContext_;
    std::shared_ptr<RSServiceDumper> rsDumper_;
    std::shared_ptr<RSServiceDumpManager> rsDumpManager_;
    std::shared_ptr<RSRenderPipeline> renderPipeline_;  // 单进程模式使用
};
```

#### 3.1.2 rs_render_process_manager.h/cpp
**功能**: 进程管理器的基类，定义进程管理的统一接口。

**主要职责**:
- 定义进程创建和管理的抽象接口
- 实现屏幕事件监听器接口（RSIScreenManagerListener）
- 提供进程间连接获取的统一方法
- 管理IPC持久化数据

**核心接口**:
```cpp
class RSRenderProcessManager : public RSIScreenManagerListener {
public:
    static sptr<RSRenderProcessManager> Create(RSRenderService& renderService);
    
    // 屏幕事件处理（继承自RSIScreenManagerListener）
    virtual sptr<IRemoteObject> OnScreenConnected(ScreenId id, 
        const std::shared_ptr<HdiOutput>& output, 
        const sptr<RSScreenProperty>& property) = 0;
    virtual void OnScreenDisconnected(ScreenId id) = 0;
    virtual void OnScreenPropertyChanged(ScreenId id, ScreenPropertyType type, 
                                   const sptr<ScreenPropertyBase>& property) = 0;
    
    // 进程间连接获取
    virtual sptr<RSIServiceToRenderConnection> GetServiceToRenderConn(ScreenId screenId) const = 0;
    virtual std::vector<sptr<RSIServiceToRenderConnection>> GetServiceToRenderConns() const = 0;
    virtual sptr<RSIConnectToRenderProcess> GetConnectToRenderConnection(ScreenId screenId) const = 0;
    
    // IPC持久化管理
    virtual std::shared_ptr<RSIpcPersistenceManager> GetIpcPersistenceManager() const { return nullptr; };
};
```

#### 3.1.3 rs_render_single_process_manager.h/cpp
**功能**: 单进程模式下的进程管理器实现。

**主要职责**:
- 在单进程模式下管理render_service和render_process的合并运行
- 创建本地连接对象（不涉及真实IPC）
- 处理屏幕事件并直接转发到渲染管线

**工作原理**:
```cpp
class RSSingleRenderProcessManager final : public RSRenderProcessManager {
public:
    explicit RSSingleRenderProcessManager(RSRenderService& renderService);
    
    sptr<IRemoteObject> OnScreenConnected(ScreenId id, 
        const std::shared_ptr<HdiOutput>& output, 
        const sptr<RSScreenProperty>& property) override {
        auto composerConn = renderService_.rsRenderComposerManager_->GetRSComposerConnection(id);
        renderService_.renderPipeline_->OnScreenConnected(property, composerConn, 
            composerToRenderConnection_, output);
        return connectToRenderConnection_->AsObject();
    }
    
private:
    sptr<RSIServiceToRenderConnection> serviceToRenderConnection_;
    sptr<IRSComposerToRenderConnection> composerToRenderConnection_;
    sptr<RSIRenderToServiceConnection> renderToServiceConnection_;
    sptr<RSIConnectToRenderProcess> connectToRenderConnection_;
};
```

#### 3.1.4 rs_render_multi_process_manager.h/cpp
**功能**: 多进程模式下的进程管理器实现。

**主要职责**:
- 管理多个render_process子进程的生命周期
- 处理进程fork、初始化和监控
- 维护进程到屏幕的映射关系
- 管理虚拟屏幕到物理屏幕的映射

**核心成员**:
```cpp
class RSMultiRenderProcessManager : public RSRenderProcessManager {
private:
    mutable std::mutex mutex_;
    std::unordered_map<GroupId, pid_t> groupIdToRenderProcessPid_;
    std::unordered_map<pid_t, sptr<IRSComposerToRenderConnection>> composerToRenderConnections_;
    std::unordered_map<pid_t, sptr<RSIServiceToRenderConnection>> serviceToRenderConnections_;
    std::unordered_map<pid_t, sptr<RSIConnectToRenderProcess>> connectToRenderConnections_;
    
    mutable std::mutex virtualScreenMutex_;
    std::map<ScreenId, ScreenId> virtualToPhysicalScreenMap_;
    
    mutable std::mutex renderProcessReadyPromiseMutex_;
    std::unordered_map<pid_t, std::promise<bool>> renderProcessReadyPromises_;
    std::unordered_map<pid_t, PendingScreenConnectInfo> pendingScreenConnectInfos_;
    std::condition_variable renderProcessReadyPromiseCv_;
    
    const std::shared_ptr<RSIpcPersistenceManager> ipcPersistenceManager_;
};
```

#### 3.1.5 rs_render_process.h/cpp
**功能**: render_process子进程的核心类。

**主要职责**:
- 作为子进程运行，负责渲染工作
- 通过SA Manager获取render_service服务
- 创建渲染管线和IPC连接
- 重放持久化数据

**核心成员**:
```cpp
class RSRenderProcess : public RefBase {
private:
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
    sptr<RSIRenderToServiceConnection> renderToServiceConnection_;
    std::shared_ptr<RSRenderPipeline> renderPipeline_;
};
```

#### 3.1.6 rs_render_process_main.cpp
**功能**: render_process子进程的入口程序。

**位置**: `main/render_process/rs_render_process_main.cpp`

**主要职责**:
- 作为子进程的可执行入口（`/system/bin/render_process`）
- 创建RSRenderProcess实例并初始化
- 启动事件循环

**入口代码**:
```cpp
int main(int argc, const char* argv[])
{
    signal(SIGPIPE, SIG_IGN);
    auto renderProcess = sptr<RSRenderProcess>::MakeSptr();
    if (!renderProcess->Init()) {
        return -1;
    }
    renderProcess->Run();
    return 0;
}
```

### 3.2 连接相关文件及代理类

#### 3.2.1 rs_render_service_agent.h/cpp
**功能**: render_service的代理类，提供线程安全的访问接口。

**主要职责**:
- 封装render_service的功能，提供线程安全访问
- 提供任务调度接口
- 提供进程信息获取接口

#### 3.2.2 rs_render_process_manager_agent.h/cpp
**功能**: 进程管理器的代理类，提供线程安全的进程管理接口。

**主要职责**:
- 封装进程管理器的功能，提供线程安全访问
- 处理多进程模式下的特殊逻辑
- 提供进程信息获取接口

#### 3.2.3 rs_render_process_agent.h/cpp
**功能**: render_process的代理类，提供对RSRenderProcess的安全访问。

**位置**: `main/render_process/rs_render_process_agent.h/cpp`

**主要职责**:
- 封装RSRenderProcess的功能，提供安全访问接口
- 处理屏幕连接/断连/属性变化事件的转发
- 作为RSServiceToRenderConnection的内部成员使用

**核心方法**:
```cpp
class RSRenderProcessAgent : public RefBase {
public:
    explicit RSRenderProcessAgent(RSRenderProcess& renderProcess);
    
    bool NotifyScreenConnectInfoToRender(const sptr<RSScreenProperty>& screenProperty,
        const sptr<IRSRenderToComposerConnection>& renderToComposerConn,
        const sptr<IRSComposerToRenderConnection>& composerToRenderConn);
    bool NotifyScreenDisconnectInfoToRender(ScreenId screenId);
    bool NotifyScreenPropertyChangedInfoToRender(
        ScreenId id, ScreenPropertyType type, const sptr<ScreenPropertyBase>& screenProperty);
private:
    RSRenderProcess& renderProcess_;
};
```

#### 3.2.4 rs_render_to_service_connection.h/cpp
**功能**: render_process到render_service的连接，继承自 `RSRenderToServiceConnectionStub`。

**主要职责**:
- 作为render_process端的Binder服务端
- 处理render_service发起的IPC调用
- 通知render_service端进程初始化完成
- 处理HGM帧率信息同步

#### 3.2.5 rs_service_to_render_connection.h/cpp
**功能**: render_service到render_process的连接，继承自 `RSServiceToRenderConnectionStub`。

**主要职责**:
- 作为render_service端的Binder服务端
- 处理render_process发起的IPC调用
- 通知render_process端屏幕事件
- 处理性能日志和调试信息

#### 3.2.6 rs_connect_to_render_process.h/cpp
**功能**: 用于创建客户端到render_process连接的服务端，继承自 `RSConnectToRenderProcessStub`。

**位置**: `core/transaction/rs_connect_to_render_process.h/cpp`

**主要职责**:
- 作为render_process端的Binder服务端
- 提供CreateRenderConnection接口，用于创建RSIClientToRenderConnection
- 在屏幕连接流程中返回给客户端，供客户端创建渲染连接

**核心方法**:
```cpp
class RSConnectToRenderProcess : public RSConnectToRenderProcessStub {
public:
    explicit RSConnectToRenderProcess(sptr<RSRenderPipelineAgent> renderPipelineAgent);
private:
    sptr<RSIClientToRenderConnection> CreateRenderConnection(
        const sptr<RSIConnectionToken>& token) override;
    const sptr<RSRenderPipelineAgent> renderPipelineAgent_;
};
```

#### 3.2.7 rs_client_to_service_connection.h/cpp
**功能**: 客户端到render_service的连接。

**主要职责**:
- 作为客户端的Binder代理
- 向render_service发起渲染请求
- 接收render_service的响应

#### 3.2.8 rs_client_to_render_connection.h/cpp
**功能**: 客户端到render_process的连接。

**位置**: `core/transaction/rs_client_to_render_connection.h/cpp`

**主要职责**:
- 作为客户端的Binder代理
- 向render_process发起渲染相关请求
- 接收render_process的响应

#### 3.2.9 rs_render_connect_parcel_info.h/cpp
**功能**: 定义进程间通信的数据结构。

**主要数据结构**:
```cpp
class ConnectToServiceInfo : public Parcelable {
public:
    sptr<IRemoteObject> composerToRenderProcessConnection_;
    sptr<IRemoteObject> vsyncToken_;
};

class ReplyToRenderInfo : public Parcelable {
public:
    sptr<IRemoteObject> composeConnection_;
    sptr<RSScreenProperty> rsScreenProperty_;
    sptr<IRemoteObject> vsyncConn_;
    std::shared_ptr<IpcPersistenceTypeToDataMap> replayData_;
};
```
