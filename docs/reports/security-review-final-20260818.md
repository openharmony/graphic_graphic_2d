# 渲染服务安全检视问题总报告（2026-08-18 汇总）

检视范围：rosen/modules/render_service、render_service_base、render_service_client 截图、
IPC 连接、事务处理、主线程管线与 Vulkan 后端路径。
检视方式：静态代码走读，结合调用图与实际业务使用方式判断；并发问题明确线程归属，
空指针问题按"仅空指针、正常业务流程"口径定性。
未做设备验证，结论以当前 master 调用图为准。
详细过程记录见 docs/reports/security-review-capture-and-connection-20260817.md（问题 1-4 详档）
与 security-review-capture-and-connection-20260818.md（问题 1/2/3/4 详档），本报告为全量汇总。

## 汇总表

| # | 位置 | 问题类型 | 定性 | 处置 |
| --- | --- | --- | --- | --- |
| 1 | rs_ui_capture_task_parallel.cpp:367 endNodeDrawable_ | 空指针 | 非缺陷 | 可选加固（条件对齐 :384） |
| 2 | rs_surface_capture_task_parallel.cpp:706 AddBlur outImage | 空指针 | **现存缺陷（渲染进程崩溃级）** | 建议必修 |
| 3 | rs_surface_capture_task_parallel.cpp:187 surfaceNodeDrawable | 空指针 | 非缺陷 | 不修改 |
| 4 | rs_client_to_render_connection.cpp:157-180 cleanDone_ | TOCTOU | **竞态（低危，重复执行）** | 暂不修改，已记录 |
| 5 | rs_render_pipeline_client.cpp:582-586 erase 盲擦除 | TOCTOU | **竞态（中低危，回调丢失）** | 建议修复 |
| 6 | rs_render_pipeline_client.cpp:757 surfaceBufferCbDirector_ | TOCTOU | 非缺陷（正确参照写法） | 不修改 |
| 7 | rs_surface_ohos_vulkan.cpp:812-835 CopyContentBuffer | 空指针/校验缺失 | **潜在缺陷（当前死代码）** | 激活前必修 |
| 8 | rs_transaction_data.cpp:395-441 payloadLock/max_size | 死锁/并发 | 非缺陷 | 可选改 lock_guard（可读性） |
| 9 | rs_render_pipeline_agent.cpp:225-244 ExecuteSynchronousTask | TOCTOU+数据竞争 | **竞态（中高危，序列化撕裂）** | 建议修复 |
| 10 | rs_ui_director.cpp currentUIDirectorState_ | 数据竞争 | 非缺陷（主线程约定） | 不修改 |
| 11 | rs_ui_capture_solo_task_parallel.cpp:84 空 pixelMap 透传 | 空指针 | 非缺陷（设计自洽） | 可选补注释 |
| 12 | rs_uni_render_visitor.cpp:2655-2657 curScreenNode_ | 空指针 | 非缺陷（遍历不变量） | 不修改 |
| 13 | rs_render_service_connect_hub.cpp:421 renderPrecess | 空指针 | 非缺陷（调用点保证） | 可选防御 |
| 14 | rs_surface_ohos_vulkan.cpp:621-622 SubmitGpu | 空指针 | 非缺陷（帧生命周期） | 不修改 |
| 15 | rs_surface_capture_task_parallel.cpp:237-241 surfaceNodeParams | 空指针 | 非缺陷（构造不变量） | 不修改 |
| 16 | rs_client_to_service_connection.cpp:1725-1743 watermarkImg | 空指针 | 非缺陷（双层防护） | 不修改 |
| 17 | rs_client_to_service_connection.cpp:2445 UnregisterByType | 越权注销 | **持久化跨进程误删（低中危）** | 建议修复（一行） |
| 18 | rs_render_pipeline_client.cpp:864 transactionDataCbDirector_ | TOCTOU | 非缺陷（锁内创建） | 不修改 |
| 19 | rs_main_thread.cpp:1930-1970 effectiveCommands_ | 并发 | 非缺陷（swap 所有权转移） | 不修改 |
| 20 | rs_main_thread.cpp:2493-2498 GetGlobalRootRenderNode | 空指针 | 非缺陷（NSDMI 单例） | 不修改 |
| 21 | rs_main_thread.cpp:2864 ReinterpretCastTo 后裸调用 | 空指针 | 非缺陷（正常口径） | 附注恶意 id 场景 |
| 22 | rs_main_thread.cpp:3310-3315 params/surfaceHandler | 空指针 | 非缺陷（注册不变量） | 不修改 |
| 23 | rs_main_thread.cpp:4068-4074 requestNextVsyncNum_ | lost update | **统计偏差（低危，无功能危害）** | 可选加固 exchange |
| 24 | rs_main_thread.cpp:6361 CHECK_INTERVAL 取模 | 除零 | 非缺陷（短路求值） | 不修改 |
| 25 | rs_main_thread.cpp pendingSplitTransactions_ | OOM | 非缺陷（三重约束有界） | 不修改 |
| 26 | rs_render_service_listener.cpp:262-276 OnTransformChange handler | 空指针 | 非缺陷（同源生命周期） | 可选风格对齐 |

统计：26 项中**缺陷 6 项**（#2、#4、#5、#7、#9、#17，另 #23 为低危统计偏差可选加固），
非缺陷 20 项。

## 第一部分：缺陷项

### #2 AddBlur 中 outImage 空解引用（建议必修，崩溃级）

rs_surface_capture_task_parallel.cpp:702-706：

```cpp
auto outImage = geRender->ApplyImageEffect(canvas, *mesaContainer, {...}, Drawing::SamplingOptions());
canvas.DrawImage(*outImage, 0, 0, Drawing::SamplingOptions());   // 无判空
```

- `GERender::ApplyImageEffect`（graphics_effect 仓 ge_render.cpp:64-79）循环内直接承接
  `geShaderFilter->ProcessImage(...)` 返回值（:147）。
- `GEMESABlurShaderFilter::ProcessImage` 存在大量真实失败路径
  （ge_mesa_blur_shader_filter.cpp:289、:318、:335、:377、:401、:419 多处 return nullptr），
  触发条件为 `MakeImage` 离屏分配失败（GPU/内存压力）。
- 失败相关性：进入 MESA 分支前提是 `ApplyHpsBlur` 已失败（:696），图形环境处于降级状态，
  后续失败概率放大。
- API 自身契约：`GERender::DrawImageEffect` 调用后立即判空（ge_render.cpp:54-57）。
- 影响：渲染线程空解引用，render service 系统进程死亡。
- 修复：判空后提前 return（与本函数 :689 对 image 判空风格一致），补 RS_LOGE。

### #9 ExecuteSynchronousTask 超时后序列化撕裂（中高危）

rs_render_pipeline_agent.cpp:225-244：

- 第一层（TOCTOU，低危）：`expired()` 检查（:237）与 `reset()`（:242）非原子，
  主线程检查通过后 IPC 线程超时置空 → 任务超时后仍执行，违背"超时不再执行"注释承诺。
- 第二层（数据竞争，中高危）：主线程 `Process` 执行中跨越 deadline → `wait_for` 超时返回 →
  stub 在 IPC 线程调 `task->Marshalling(reply)`（rs_client_to_render_connection_stub.cpp:450），
  与主线程 Process 并发读写 `isTimeout_/success_/propertiesMap_/fraction_`
  （rs_node_showing_command.cpp:74-76、109-116）。`propertiesMap_`（std::map）边遍历序列化边写入，
  迭代器失效/UB，崩溃风险。
- weak_ptr 机制只挡"尚未开始"的任务，对"已开始未结束"零保护。
- 可达性：默认 timeout 100ms，主线程拥塞时窗口真实可达。
- 修复方向：超时分支直接回静态超时应答（不 marshal 任务成员），仅 future_status::ready 才序列化。
  同族问题：客户端侧 RSRenderThreadClient::ExecuteSynchronousTask
  （rs_render_thread_client.cpp:48）超时后读 GetProperty() 有同样竞争。

### #17 UnregisterByType 持久化跨进程误删（低中危，一行修复）

rs_client_to_service_connection.cpp:2433-2451：

- 注册按 pid 存（`RegisterWithCallingPid`，:2416-2419）；live 注销按 pid 删
  （`conn->UnRegisterSelfDrawingNodeRectChangeCallback(remotePid_)`，:2449）；
  但持久化注销用 `UnregisterByType`（:2445）→ `replayData_.erase(type)`
  （rs_ipc_persistence_manager.cpp:57-61），**全系统**该类型 entry 一并清除。
- 攻击路径澄清：原始疑问的"窃取他进程代理"不成立（binder capability 模型，持代理即授权）。
  真实路径更简单：任何进程用**自己的**连接注销，就会把其他注册者（系统侧用户）的 replay
  数据一起擦掉，render 进程重启回放时这些回调静默失效。任意能建连接的应用进程可触发。
- 佐证：按 pid 删的 API `UnregisterByTypeAndCallingPid(type, pid)`（manager:112-116）已存在，
  全仓 `UnregisterByType` 仅此一个调用点——选错了工具。
- 修复：改为 `UnregisterByTypeAndCallingPid(type, remotePid_)`。

### #5 TakeSurfaceCaptureWithAllWindows 失败擦除竞态（中低危）

rs_render_pipeline_client.cpp:550-589：

- 竞态 A（搭车者回调丢失）：T1 emplace 后 IPC 在途，T2 同 key 搭车（:569，已承诺成功）；
  T1 IPC 返回错误（如 agent pipeline 失效 ERR_INVALID_VALUE，服务端不回调）→
  T1 `erase(key)`（:585）连 T2 的回调一起销毁，T2 的截图调用方悬挂。
- 竞态 B（陈旧 key 误杀新请求）：服务端权限拒绝路径先回调再返回错误
  （agent :666-670）；回调触发 TriggerSurfaceCaptureCallback 擦除 key 后，T3 新请求同 key
  emplace，T1 的错误返回再 erase 掉 T3 的新条目，T3 结果到达时 "callbackVector is empty"。
- 根因：key `{id, captureConfig}` 无请求唯一性，失败航班的盲擦除波及共享该 key 的搭车者与新请求。
- 定性：活性缺陷（回调静默丢失），所有 map 访问持锁，无内存破坏。
- 修复方向：失败时锁内取出回调向量并逐个以错误码触发（复用 TriggerSurfaceCaptureCallback
  的 captureErrorCode 通道）；彻底方案为 map 值加单调 flightId。
- 同族问题：本文件四个兄弟函数 director 锁外创建（TakeSurfaceCapture :461、
  TakeSelfSurfaceCapture :506、SetWindowFreezeImmediately :542、TakeUICaptureInRange :627），
  对 sptr surfaceCaptureCbDirector_ 无锁 check-then-new，并发不同 key 时双重 new +
  一个对象泄漏。修复：director 创建移入临界区（对齐本函数 :575 与 #6 的正确写法）。

### #4 CleanAll cleanDone_ TOCTOU（低危）

rs_client_to_render_connection.cpp:157-180：

- check（:161）与置位（:173）分处两个临界区，窗口覆盖整个 `Clean()` 执行期
  （内部 ScheduleMainThreadTask(...).wait() 同步等待，agent :2213-2219，负载高时达数十 ms）。
- 并发方：死亡通知线程 OnRemoteDied（:215）、REMOVE_CONNECTION IPC（stub:97），
  可同时通过检查 → Clean/RemoveConnection 重复执行。CleanResources 幂等，
  定性为冗余清理/性能抖动，非崩溃级。
- "结束后置位"争议已分析：晚置位无失败语义（Clean 返回 void，wait 返回值被丢弃），
  也无调用方依赖 false 重试（可重复清理走 CleanForRefresh，完全绕开 cleanDone_）。
- 修复方案 A（推荐）：检查与置位同临界区、动作移出锁外；方案 B：std::call_once。

### #7 CopyContentBuffer 判空/校验缺失（潜在缺陷，当前死代码）

rs_surface_ohos_vulkan.cpp:812-835：

- `surfaceBufferSrc` 参数完全未检查（调用方传空直接崩）；两个 `GetVirAddr()` 返回值未判空
  （映射失败可为 null）；`memcpy_s` 无 srcSize/dstSize 一致性校验——
  src 大于 dst 时 memcpy_s 自身拒绝（返回 EINVAL）但函数只打日志不 return，
  继续执行 InvalidateCache/FlushCache；src 小于 dst 时静默部分拷贝，目标剩余为陈旧像素。
- 全仓无调用者（预留接口），当前不可达，定级"激活即暴露"。
- 修复：参数判空 + GetVirAddr 判空 + 尺寸一致性校验，一并补齐。

### #23 requestNextVsyncNum_ lost update（低危，可选加固）

rs_main_thread.cpp:4068-4074：

- `drawingRequestNextVsyncNum_.store(requestNextVsyncNum_)` 与 `requestNextVsyncNum_ = 0`
  之间到达的 `++`（38 处调用点）会被清零覆盖。窗口真实存在。
- 但被覆盖的只是统计计数（超限 dump、:5282 跳首帧判定的边沿精度），vsync 请求本身由
  RequestNextVSyncInner 独立发出，不受影响；丢计数后果是保守方向（不跳帧）。
- 可选加固：`drawingRequestNextVsyncNum_.store(requestNextVsyncNum_.exchange(0));` 一条指令归零窗口。

## 第二部分：非缺陷项（含依据）

### #1 endNodeDrawable_（rs_ui_capture_task_parallel.cpp:367）

- `CreateResources()`（:217-241）创建并判空，失败即 return，Run() 不会被投递；
- `IsStartEndSameNode()` 晚于 `CreateResources()` 调用（Capture :164/:177 顺序），
  执行时 isStartEndNodeSame_ 恒 false，走 endNode 路径必创建；
- `renderParams_` 由 InitRenderParams 全分支 make_unique 创建，无置空路径。
- 残留：:366 与 :384 条件不对称，安全性依赖隐式跨函数时序，若调用顺序调整将变崩溃。
  首轮分析曾误判"同节点场景可达空指针"，核对执行顺序后修正。

### #3 surfaceNodeDrawable（rs_surface_capture_task_parallel.cpp:176-191）

- OnGenerate 在此上下文的全部返回路径非空：node 已判空（:179）；SURFACE_NODE 由静态注册器
  库加载时注册（instance_）；生成器返回 new（OHOS 禁异常，OOM 直接 abort）；
  缓存命中返回 lock() 成功结果。
- lambda 按值捕获 shared_ptr，PostTask 异步期间保活；static_pointer_cast 继承关系正确。

### #6 surfaceBufferCbDirector_（rs_render_pipeline_client.cpp:757）

- 唯一写点在 unique_lock 临界区内，check-then-new 原子；:761 锁外读由
  happens-before + 单次初始化（无置空路径）保证。是 #5 兄弟函数应对齐的正确参照。

### #8 payloadLock 与 max_size()（rs_transaction_data.cpp:395-441）

- 死锁四成因排除：锁窗口内无重入、无 ABBA、lock/unlock 间无 return/continue、
  OHOS 禁异常无抛出路径；对象在 unmarshal 完成前不发布，跨线程争用不可达。
- defer_lock + bad_alloc 疑问已用可运行程序验证：析构只看 owns 标志（lock() 已置位），
  栈回退必然 unlock()；release() 需显式调用，析构从不调。异常与提前 return 均正确 RELEASED。
- :389 max_size() 未持锁：被"对象未发布"不变量覆盖；且 max_size 为纯查询，
  该检查实际冗余（readableSize 受 4MB/最小命令尺寸约束，永远先触发）。
- 可选改进：手动 lock/unlock 改 lock_guard 作用域块（纯可读性）。

### #10 currentUIDirectorState_（rs_ui_director）

- 写点全部在 Go* 生命周期函数，客户端 API 主线程约定调用；GetCurrentState() 仓内无调用方
  （rs_pipeline_dumper 读的是服务端另一个类 RSUIRenderDirector）。约定而非机制保证，
  若外部跨线程调用为形式竞争（对齐 enum 撕裂读，不崩溃）。

### #11 CaptureSoloNode 空 pixelMap 透传（rs_ui_capture_solo_task_parallel.cpp:79-85）

- null 有合法线格式（Marshalling 写 -1，Unmarshalling 读 -1 还原 nullptr），仓内消费方
  只透传不解引用，空值是"该节点截图失败"的一等信号。仓外调用方按 shared_ptr 契约自理。
- 可选：给 rs_interfaces.h 声明补"pixelmap 可能为空"注释。

### #12 curScreenNode_（rs_uni_render_visitor.cpp:2655-2657）

- 遍历结构不变量：surface 节点必为 screen 节点后代，screen 先经 InitScreenInfo 置位
  （失败整棵子树不访问）；访问器每帧新建无跨帧残留。
- 同函数 :1327/:1367 更早处已裸解引用，单补 :2655 判空无意义。文件内其它判空属于
  capture/delegate 等独立入口路径，防御层次与可达上下文一致。

### #13 renderPrecess->AsObject()（rs_render_service_connect_hub.cpp:421）

- 唯一调用点 :226-229 上游 :221-224 已对 iface_cast 结果判空；
  proxy 存活期间 AsObject() 非空且确定。同文件 RemoveRenderProcessDeathRecipient（:444-449）
  的三连判空是防御层次不统一，非本处疏漏的证据。

### #14 SubmitGpu renderContext_/GPUContext（rs_surface_ohos_vulkan.cpp:621-622）

- flushState_.valid 门槛（:614）⇒ 必经 FlushGpu 成功 ⇒ 必经 RequestFrame 成功，
  后者在 :203 判空 renderContext_、:288/:250 已成功解引用 GetSharedDrGPUContext()。
  同帧固定顺序（FlushGpu→SubmitGpu→FlushBuffer），无置空窗口。
  WaitSurfaceClear 的双判空是任意时刻可调的独立入口，防御层次不同。

### #15 surfaceNodeParams（rs_surface_capture_task_parallel.cpp:237-241）

- surfaceNodeDrawable_ 已判空（:234）；GetRenderParams() 构造不变量见 #1；
  static_cast 类型正确（SURFACE_NODE → RSSurfaceRenderParams）。

### #16 watermarkImg（rs_client_to_service_connection.cpp:1725-1743）

- 两条 IPC 入口 stub 均判空拦截（render_server :2345-2349、render 进程 :456-460）；
  持久化恢复读失败不 Apply；最终消费者 RSMainThread::ShowWatermark（:5505）`flag && watermarkImg`
  出口自防。本函数为转发节点，无解引用语句。

### #18 transactionDataCbDirector_（rs_render_pipeline_client.cpp:864-866）

- 与 #6 同构：唯一写点在 transactionDataCallbackMutex_ 临界区内，锁外 :870 读安全，
  无置空路径。

### #19 effectiveCommands_（rs_main_thread.cpp:1930-1970）

- 全仓仅本函数访问；swap（:1937，锁内）是所有权转移点，转移后锁外遍历/clear 操作的
  是本线程独占副本，与 pendingEffectiveCommands_ 无别名。锁只覆盖共享容器交接、
  重活（command->Process）移出锁外是有意设计。生产者 ClassifyRSTransactionData 经
  PostTask 串行化在同一线程。若求形式对称可改为局部变量（成员声明才是疑问根源）。

### #20 GetGlobalRootRenderNode（rs_main_thread.cpp:2493-2498）

- globalRootRenderNode_ 为 NSDMI 单例成员（rs_context.h:285，构造时 make_shared），
  全仓无重赋值/置空。全仓 11 处使用仅 1 处防御式判空，共识假设非空。

### #21 ReinterpretCastTo 后裸调用（rs_main_thread.cpp:2864）

- 正常口径：任务唯一生产路径有 SURFACE_NODE 类型闸门（agent :581），类型不可变，
  销毁由 :2858 拦截，ReinterpretCastTo 不返回空。
- 附注（超出空指针口径，仅记录）：恶意 id 复用（surface 销毁后同 id 建 canvas 节点，
  两类命令的 nodeId 均客户端可控且只校验 pid 前缀）可在跨帧等待窗口造成类型混淆，
  :2864/:2876/:2894-2896 三处接续 UB，构成非系统应用对 render service 的 DoS。
  若需抵抗该场景，消费点需补类型闸门，另立项处理。

### #22 DoDirectComposition 的 params/surfaceHandler（rs_main_thread.cpp:3310-3315）

- params：OnRegister → InitRenderParams（surface 版）无条件 make_unique
  （rs_surface_render_node.cpp:3309-3312）；唯一置空路径 ResetRenderParams（:330-334）
  全仓零调用（死代码）。且遍历源 hardwareEnabledNodes_ 是本帧刚经历完整 prepare 的节点。
- surfaceHandler：构造函数初始化列表三元兜底创建
  （rs_surface_render_node.cpp:166，传入非空用传入值、传入空当场 make_shared），
  成员无重赋值路径（SetSurfaceHandler 属 canvas 节点另一个类）。

### #24 CHECK_INTERVAL 取模（rs_main_thread.cpp:6361）

- `CHECK_INTERVAL != 0 && i % CHECK_INTERVAL == 0`：&& 左到右短路求值，到达 % 时必非零；
  size_t 无符号无符号问题。

### #25 pendingSplitTransactions_（rs_main_thread.cpp）

- 三重约束：entry 数受系统进程数上限且进程死亡由 CleanResources → ClearRebuildTransactionData
  （:878-882）清理；每 pid 队列寿命被 MAX_TOTAL_TIME_MS（默认 100ms，:6339/:6354）硬上限
  截断，超时跳过时间片检查单帧强制排干并 erase；生产侧受 parcel 4MB 上限与 unmarshal
  节流。峰值内存有界瞬时，非持续占用。残余：100ms 尖峰与强制排干帧卡顿（性能非内存）。

### #26 OnTransformChange handler（rs_render_service_listener.cpp:262-276）

- 两个 weak_ptr 同源（listener 唯一构造点 agent :1389-1390，surfaceHandler 即
  node->GetRSSurfaceHandler()）；node 活 ⇒ 其成员 handler 活 ⇒ lock() 非空。
  handler 为空 ⇒ node 必为空 ⇒ 已被 :266 拦截。矛盾保证 :275 不可达空指针。
- OnGoBackground（:235）判 handler 是防御风格不同，非发现真实场景。
- 依赖前提：surfaceHandler_ 无替换接口、两 weak_ptr 永远同源。任一被打破则成真缺陷，
  可选一行风格对齐消除依赖。

## 第三部分：检视复用规则（本次沉淀）

1. **drawable renderParams_ 不变量**：drawable 经 OnGenerate 创建且已判空 ⇒
   GetRenderParams() 非空。InitRenderParams 无条件 make_unique，全仓无置空赋值
   （适用于 #1、#3、#15）。
2. **NSDMI/构造期单例成员**：构造时无条件创建 + 全仓无重赋值路径的成员（如
   globalRootRenderNode_、surfaceHandler_、stagingRenderParams_），调用点无需判空
   （适用于 #20、#22）。
3. **锁内 swap/拷贝、锁外处理**：共享容器在临界区内完成所有权转移，重活移出锁外
   操作独占副本，是标准安全模式（适用于 #19、#5 的 TriggerSurfaceCaptureCallback）。
4. **director 创建必须在锁内**：check-then-new 对 sptr 成员必须整体在临界区内完成
   （#6/#18 正确；#5 兄弟函数错误）。
5. **唯一调用点前置校验可作调用契约**：非 static 的私有/公开方法若全仓只有一个调用方
   且其上游已挡空，则函数内可不判空，但需在评审记录中标注契约（#13、#14）。
6. **"对象未发布"覆盖未持锁访问**：new 出来、完成初始化前不交给其它线程的对象，
   其任何成员访问（含未持锁）都无数据竞争（#8）。
7. **binder capability 语义**：per-connection proxy 只回传给持 token 调用方，
   "窃取他进程代理"类攻击路径默认不成立，权限分析应聚焦"自有连接能做什么"（#17）。
