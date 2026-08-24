# 渲染服务安全检视总报告（最终版 2026-08-19）

检视范围：rosen/modules/render_service、render_service_base、render_service_client、
platform 层（eventhandler/refbase）、Vulkan 后端。
检视方式：静态代码走读，结合调用图与实际业务使用方式判断；空指针问题按
"仅空指针、正常业务流程"口径，并发问题明确线程归属，权限问题按 binder
capability 模型分析。未做设备验证。

关联文档（详细论证过程）：
- security-review-capture-and-connection-20260817.md（问题 1-4 过程详档）
- security-review-capture-and-connection-20260818.md（4 项过程详档）
- security-review-final-20260818.md（前 26 项中期汇总）
- security-review-non-defects-20260818.md（22 项非缺陷详报）
- security-review-unmarshalling-command-20260819.md（UnmarshallingCommand 专项）

本报告为全会话最终汇总，编号统一重排，按定性分组。

## 一、缺陷项（建议修复，按优先级）

### A 级（崩溃/UB 级，建议必修）

| # | 位置 | 问题 | 修复建议 |
| --- | --- | --- | --- |
| D1 | rs_surface_capture_task_parallel.cpp:706 AddBlur outImage | ApplyImageEffect 可返回空（MESA 滤镜 MakeImage 失败，GPU/内存压力下真实可达），渲染线程空解引用，render service 崩溃 | 判空提前 return，补 RS_LOGE |
| D2 | rs_render_pipeline_agent.cpp:225-244 ExecuteSynchronousTask | 超时后 stub 在 IPC 线程 Marshal 任务成员，与主线程 Process 并发读写 propertiesMap_（std::map 迭代器失效，UB） | 超时分支回静态超时应答，仅 future ready 才序列化 |
| D3 | rs_logical_display_render_node.cpp:167-171 ClearModifiersByPid | range-for 内调 RemoveModifier（内部 slot.erase），vector 迭代器失效 UB，进程清理路径可达，漏删/越界 | erase-remove 惯用法重写 |

### B 级（安全/功能缺陷，建议修复）

| # | 位置 | 问题 | 修复建议 |
| --- | --- | --- | --- |
| D4 | rs_transaction_data_callback_manager.cpp + stub REGISTER_TRANSACTION_DATA_CALLBACK | 注册侧无 token 归属校验 + token 可预测（tid<<32\|counter）+ rebuild 用 timeStamp=0 + duplicate 静默失败 = key 抢占，受害者 rebuild 回调永久失效（UI 卡死），DFX 无感知 | 注册 key 加 callingPid（三元组），消除抢占 |
| D5 | rs_unmarshal_thread.cpp:188-189 noNeedWaitTaskNum_ | check-then-decrement 非原子 + increment/decrement 不对称（waitUnmarshalling 任务偷吃计数），并行下可致负值，主线程误等 unmarshal barrier，持续 jank | lambda 捕获 waitUnmarshalling 条件化 decrement，或 CAS 循环 |
| D6 | rs_client_to_render_connection.cpp:2445 UnregisterByType | 持久化注销按类型全删（不传 pid），任意自有连接注销即擦全系统 replay 条目，render 进程重启后其它进程回调静默失效 | 换用 UnregisterByTypeAndCallingPid(type, remotePid_)（API 已存在） |
| D7 | rs_render_pipeline_client.cpp:582-586 TakeSurfaceCaptureWithAllWindows erase 盲擦除 | 失败航班的 erase(key) 波及搭车者与新请求（key 无请求唯一性），回调静默丢失，调用方悬挂 | 失败时锁内取出回调向量逐个以错误码触发 |
| D8 | rs_surface_buffer_callback_manager.cpp:321 static lambda 捕获 rootNodeId | static 局部 lambda 首次捕获后永久冻结，多实例子树切换后新子树回调永不触发（Vulkan 多实例必现） | 删掉 static |
| D9 | rs_client_to_render_connection_stub.cpp REGISTER_TRANSACTION_DATA_CALLBACK 空命令洪水（专项报告问题1） | hasCommand=0 绕过统计（opCount 恒 0），杀人阈值/告警永久失明，CPU/binder/vsync 三路消耗无限持续 | 统计口径计入实际解析条数 |
| D10 | rs_render_service_connect_hub.cpp:83-108 析构 while 循环 | refcount ≤0 时 DecStrongRefCount 空操作死循环；DecStrongRef 与外部 sptr 析构竞态可穿 0（CPU 耗尽/UAF） | 循环加 >0 下界退出或改纯 RAII 设计 |
| D11 | rs_render_service_connect_hub.cpp:198-232 GetRenderProcessTokenMaskId | check（FindTokenMaskIdByRenderRemote）与 act（IPC+存储）分离，窗口毫秒级，同一 render 进程建两条连接，第一条泄漏 | duplicate check 加 connectToRenderRemote 维度或 reserve-then-fill |
| D12 | rs_main_thread.cpp:4068-4074 requestNextVsyncNum_ lost update | store 与清零之间到达的 ++ 被覆盖，统计偏差致跳首帧判定边缘失误（无功能危害） | exchange(0) 一条指令归零窗口 |
| D13 | rs_ui_capture_task_parallel.cpp:714-719 GetAppGpuMemoryInMB 失败分支 | 唯一漏掉 ClearNodeCacheSurface 的失败分支（同函数其余 4 处都有），节点缓存残留 | 补一行 ClearNodeCacheSurface |
| D14 | rs_render_service_stub.cpp:75-79 CREATE_CONNECTION WriteRemoteObject | 两次 WriteRemoteObject 均未检查，失败时客户端拿空 proxy 且返回 ERR_OK | 补检查返 ERR_INVALID_REPLY |
| D15 | rs_render_pipeline_agent.cpp:1771 GetPixelMapByProcessId repCode | repCode 恒 SUCCESS（lambda promote 失败/转换全失败均不感知），上游 repCode!=SUCCESS 校验被架空 | 有输入无输出时设错误码 |
| D16 | rs_render_pipeline_client.cpp:266 RegisterBufferClearListener | IPC 返回值丢弃，恒 return true，失败静默（buffer clear 通知丢失无感知） | 检查返回值回传 false |
| D17 | rs_transaction_data.cpp:125-133 Marshalling WriteUint8×3 | hasCommand 标志字节写失败未纳入 success 链，线格式错位污染（低概率） | 三处统一 success && 前缀 |
| D18 | rs_client_to_render_connection_proxy.cpp:87-95 空同步事务分支 | func() 返回值丢弃，FillParcel 失败时 parcelVector 为空仍 return ERR_OK，跨进程同步状态失步 | 补 if(!func()) return ERR_INVALID_VALUE |
| D19 | rs_ui_capture_task_parallel.cpp CreateResources scale 无上界 | UI 截图路径 scaleX/Y 只挡 0/负，极大值致 inf→int UB 或巨型分配（同族 RSSurfaceCaptureTaskParallel 有 >1.f 校验） | 对齐补 scaleX > 1.f 拦截 |
| D20 | rs_main_thread.cpp:2864 ReinterpretCastTo 类型混淆（附注级） | 恶意 id 复用（surface 销毁后同 id 建 canvas）可在跨帧窗口造成 :2864/:2876/:2894 UB，系统进程 DoS（超出空指针口径，另立项） | 消费点补类型闸门 |

### C 级（休眠/理论缺陷与加固项，可随版本修）

| # | 位置 | 问题 | 修复建议 |
| --- | --- | --- | --- |
| D21 | rs_surface_ohos_vulkan.cpp:812-835 CopyContentBuffer | 死代码激活即暴露：参数/GetVirAddr 判空缺失、memcpy_s 尺寸一致性缺失、mSurfaceList.back() 空容器 UB（唯一不判空的 back()） | 激活前补齐三组校验 |
| D22 | rs_transaction_handler.cpp:156-174 flushEmptyCallback 持锁回调 | 回调在 mutex_ 持有期执行，回调链触碰同 handler 持锁方法即自死锁（当前零调用，休眠） | 回调拷贝后锁外执行 |
| D23 | rs_render_thread.cpp:198-208 SetVSyncFuncs 裸 this 捕获 | 双 magic static 析构顺序不受控，静态析构期调用即 UAF（同文件 :187 RegisterGetRootNodeIdFuncForRT 同型） | 改经 Instance() 访问 |
| D24 | rs_ui_director.cpp:123 GetAnimationFallbackNode | 裸解引用无判空，RSNodeMap 析构置空后（静态析构期）UAF；服务端同语义代码判了空 | 补判空（一行） |
| D25 | rs_ui_director.cpp GoBackground 状态机 | RESUME→BACKGROUND 转移被拒（前置过严），后台命令丢失，客户端服务端状态失步 | 前置放宽含 RESUME |
| D26 | rs_render_service_connect_hub.cpp:421 AddDeathRecipient 失败 | 仅日志不回退，render 进程已死窗口下连接照建，条目永不清理（泄漏） | 失败 return 不存储（需接口改 ErrCode） |
| D27 | rs_render_service_listener.cpp 等五处 surfaceCaptureCbDirector_ 锁外创建 | 四个兄弟函数 check-then-new 在锁外，并发不同 key 双重 new+泄漏（TakeSurfaceCaptureWithAllWindows :575 为正确参照） | director 创建移入临界区 |
| D28 | rs_render_service_connect_hub.cpp:83-108 AddRenderProcessConnectionToken 等 | token_ 手动 DecStrongRef 系列（#44 同族）、CreateRenderConnection proxy token 未判空（#53）、renderPrecess AsObject（#13）——调用方契约式安全，防御不对称 | 各补一行判空 |
| D29 | rs_main_thread.cpp:3993-4017 SubHistoryEventQueue | size()==1 时 fallback 分支 size_t 下溢（end=-2 输出空）+ 主路径 off-by-one（j<end 丢命中行） | 两处循环边界修正 |
| D30 | rs_render_interface.cpp TakeSurfaceCaptureForUIWithoutUni this 捕获 | lambda 捕获未使用的 this，未来成员访问即悬空（当前无害） | 删捕获 |
| D31 | rs_surface_ohos_vulkan.cpp:674-681 FlushBuffer dup×3 | fd 表耗尽（EMFILE）时 fence 静默丢失无日志 | 失败补日志+短路 |
| D32 | rs_texture_export.cpp 半初始化对象 | 构造判空 return 制造半初始化对象，使用点裸解引用（当前唯一调用方已保证非空） | 删构造内 return 或使用点补判空 |
| D33 | rs_marshalling_helper.cpp RSSurfaceRenderNodeConfig nodeType | Unmarshalling 无范围校验，消费端全等值比较免疫（降级不越界），未来 switch/索引即暴露 | 补 NODE_MAX 上界一行 |
| D34 | rs_render_service_visitor.cpp:87 logicalScreenWidth | frame 属性可被命令通道写超大值（无上界），divided render 路径休眠中 | 补 QueryMaxGpuBufferSize 或相对屏幕倍数上界 |
| D35 | QuickPrepareUnionRenderNode:1538 / QuickPrepareSurfaceRenderNode:1377 等 | dirtyManager 检查在状态 mutate 之后/中间返回半恢复（curAlpha_ 泄漏给兄弟节点，正常流程 dirtyManager 不可空） | 检查前移或 RAII guard |
| D36 | rs_transaction_handler.cpp:112-127 AB-BA 锁序 | MoveCommandByNodeId* 固定 A→B 加锁，当前调用方向恒定不可达环，依赖单线程约定 | 改 std::scoped_lock 双锁 |

## 二、非缺陷项（分析确认无问题，共 42 项）

### 空指针类（调用图/构造不变量保证）

| # | 位置 | 保证来源 |
| --- | --- | --- |
| N1 | rs_ui_capture_task_parallel.cpp:367 endNodeDrawable_ | CreateResources 判空失败即不投递 Run |
| N2 | rs_surface_capture_task_parallel.cpp:187 surfaceNodeDrawable | OnGenerate 全路径非空（静态注册器+new） |
| N3 | rs_surface_capture_task_parallel.cpp:237 surfaceNodeParams | renderParams_ 构造不变量 |
| N4 | rs_surface_ohos_vulkan.cpp:621 SubmitGpu | 帧生命周期（FlushGpu 前置保证） |
| N5 | rs_render_pipeline_agent.cpp 全类 GetMainThread/GetUniRenderThread | pipeline 构造期单例赋值+初始化时序 |
| N6 | rs_render_pipeline_agent.cpp:1608 GetAppGpuMemoryInMB 三级链 | InitGrContext 同步阻塞+同线程 FIFO |
| N7 | rs_render_pipeline_agent.cpp:1670 CollectSurfaceBuffers handler/GetBuffer | 构造不变量+空值透传设计 |
| N8 | rs_render_pipeline_agent.cpp:1672 GetBoundsGeometry | NSDMI 值成员+构造期 make_shared |
| N9 | rs_render_pipeline_agent.cpp SetRogScreenResolution PostMainThreadSyncTask 返回值 | 初始化时序保证必执行（顺带：:1654 日志函数名笔误） |
| N10 | rs_unmarshal_thread.cpp RSMainThread::Instance() | 函数局部静态地址永真非空 |
| N11 | rs_client_to_service_connection.cpp ShowWatermark watermarkImg | 入口双 stub 判空+出口自防 |
| N12 | rs_render_pipeline.cpp:174 OnScreenConnected rsScreenProperty | 实屏调用方 :74 先裸解引用兜底（防御不对称记录） |
| N13 | rs_render_interface.cpp:544 renderPipelineClient_ | 构造函数无条件 make_shared |
| N14 | rs_main_thread.cpp:2493 GetGlobalRootRenderNode | NSDMI 单例成员 |
| N15 | rs_main_thread.cpp:2864 ReinterpretCastTo（正常口径） | 入队类型闸门+类型不可变（恶意 id 场景见 D20） |
| N16 | rs_main_thread.cpp:3310-3315 params/surfaceHandler | 注册/构造双重不变量 |
| N17 | rs_render_service_listener.cpp:262 OnTransformChange handler | 同源生命周期蕴含 |
| N18 | rs_render_service_connect_hub.cpp:421 renderPrecess AsObject | 唯一调用点上游判空 |
| N19 | rs_transaction_handler MoveCommandByNodeId handler 判空 | 唯一调用方已判空+GetRSTransaction 构造不变量 |
| N20 | rs_connect_to_render_process_proxy.cpp:44 token（见 D28 加固） | 唯一调用链 new 产物 |
| N21 | rs_ui_director.cpp Init :123 GetAnimationFallbackNode（正常期） | 构造无条件创建（析构窗口见 D24） |

### 并发类（线程归属/锁分析确认）

| # | 位置 | 结论依据 |
| --- | --- | --- |
| N22 | rs_render_pipeline_client.cpp:757 surfaceBufferCbDirector_ | 唯一写点在锁内（正确参照） |
| N23 | rs_render_pipeline_client.cpp:864 transactionDataCbDirector_ | 同构正确写法 |
| N24 | rs_transaction_data.cpp:395 payloadLock 手动 lock/unlock | RAII 析构兜底+对象未发布（defer_lock 误释已实测排除） |
| N25 | rs_transaction_data.cpp:389 max_size() 未持锁 | 对象未发布不变量+检查冗余 |
| N26 | rs_main_thread.cpp effectiveCommands_ 锁内 swap 锁外 clear | 所有权转移模式 |
| N27 | rs_render_pipeline_client.cpp TakeSurfaceCaptureWithAllWindows 锁段 | publish-once+锁 happens-before（erase 见 D7） |
| N28 | rs_ui_director currentUIDirectorState_/isActive_/rsUIContext_/skipDestroyUIContext_/rootNode_ 全生命周期成员 | RSUIDirector 主线程单线程对象约定 |
| N29 | RSRenderThread timestamp_ OnVsync/ProcessCommand | vsync 回调绑定本线程 EventRunner，单线程串行 |
| N30 | rs_ui_director ReleaseRenderNode/ExecuteGoDestroy check-act | 主线程程序顺序，窗口为零 |
| N31 | RSUIDirector::ExecuteGoDestroy rsUIContext_ 逻辑 UAF 疑虑 | shared_ptr 保活对象本体，仅一帧级逻辑竞态（记录不修） |
| N32 | SetCacheEnabledForRotation TF_ASYNC 重放疑虑 | binder 无请求缓存，操作幂等无安全语义 |
| N33 | RSRenderPipelineAgent::Clean 无超时 wait | 清理语义无超时更正确，主线程卡死是独立根因 |
| N34 | MoveCommandByNodeId AB-BA（可达性） | 调用方向恒定 pre→new（模式隐患见 D36） |

### 输入校验/权限类（capability 模型或既有防线覆盖）

| # | 位置 | 结论依据 |
| --- | --- | --- |
| N35 | CheckCreateNodeAndSurface isTokenTypeValid | INVALID token 走宽松路径无提权，nodeId-pid 绑定兜底 |
| N36 | CREATE_PIXEL_MAP_FROM_SURFACE 无归属校验 | remoteObject 是 capability，不持有他进程 producer 即不可传入 |
| N37 | REGISTER_APPLICATION_AGENT 无 token 校验 | key 为 GetCallingPid()（内核可信），无抢占面（对照 D4） |
| N38 | RSRenderPipeline::AddConnection / RSConnectToRenderProcessStub 无权限矩阵 | capability 域闭合+GetCallingPid 内核身份 |
| N39 | RemoveConnection 接口认证 | 同上，pid+tokenMaskId 双匹配只可能清自己 |
| N40 | UnmarshallingCommand 四问（负 commandSize/max_size/死锁/失败 return） | readableSize 前置防线+对象未发布+RAII（专项报告） |
| N41 | UnmarshallingCommand 真命令洪水 OOM | parcel 4MB 有界+2 发触发杀人阈值 |
| N42 | UnmarshallingCommand IsCallingPidValid 事后校验 | 校验对象在反序列化后才存在，防御覆盖 |

### 其他

| # | 位置 | 结论 |
| --- | --- | --- |
| N43 | rs_main_thread.cpp:6361 CHECK_INTERVAL 取模 | 短路求值无除零 |
| N44 | pendingSplitTransactions_ OOM | 三重有界（进程数+100ms 强排+parcel 节流） |
| N45 | rsUIContextMap_ 无上界 | 生命周期配对+实例数上限 |
| N46 | rs_surface_ohos_vulkan SetNativeWindowInfo 六次 HandleOpt 无检查 | API 无失败语义+GET 回读自愈（三后端一致风格） |
| N47 | CaptureSoloNode 空 pixelMap 透传 | 空值有合法线格式，透传设计自洽 |
| N48 | cleanDone_/OnRemoteDied token_ 等（TOCTOU 系列） | 见 0817 报告问题 4（低危记录在 D 级之外单独评估） |

## 三、专项分析

### UnmarshallingCommand 专项（security-review-unmarshalling-command-20260819.md）
- 空命令洪水绕过统计阈值（D9，缺陷）
- 真命令 OOM 不可达（N41）
- 事后校验开销不可回收属固有代价（N42）
- readableSize 校验本质：按最小命令体积反推的条数上界，防伪造 commandSize，
  防不了合法条数下的空内容（与 D9 修复需配合）

### 复用规则沉淀（详见 non-defects 报告，11 条）
核心不变量：drawable renderParams_ 构造不变量、NSDMI/构造期单例成员、
锁内交接锁外处理、publish-once 成员锁外使用三条件、初始化时序保证、
"对象未发布"、binder capability 语义、同源生命周期蕴含、主线程单线程对象、
EventRunner 绑定即单线程、判空边界（运行期可变 vs 构造后冻结）。

## 四、统计

| 定性 | 数量 | 说明 |
| --- | --- | --- |
| 缺陷（A 级必修） | 3 | 崩溃/UB 级 |
| 缺陷（B 级建议修） | 17 | 安全/功能缺陷 |
| 加固/休眠（C 级） | 16 | 随版本修 |
| 非缺陷 | 42+ | 含全部论证依据 |
| 合计 | 78+ | 跨 5 份过程文档 |

修复优先级建议：D1/D2/D3 立即排期 → D4-D9 安全类 → 其余按版本节奏。
