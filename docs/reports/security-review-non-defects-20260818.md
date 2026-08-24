# 渲染服务检视非缺陷项详报（2026-08-18）

检视对象：本轮检视中定性为"非缺陷、无需修改"的问题项。
检视方式：静态代码走读，结合调用图与实际业务使用方式判断；空指针问题按
"仅空指针、正常业务流程"口径定性，并发问题明确线程归属。
未做设备验证，结论以当前 master 调用图为准。
关联文档：docs/reports/security-review-final-20260818.md（全量汇总），
security-review-capture-and-connection-20260817.md、-20260818.md（过程详档）。

## 汇总表

| # | 位置 | 疑问焦点 | 非空/安全保证来源 |
| --- | --- | --- | --- |
| 1 | rs_ui_capture_task_parallel.cpp:367 endNodeDrawable_ | drawable/params 判空 | 调用顺序 + 构造不变量 |
| 2 | rs_surface_capture_task_parallel.cpp:176-191 surfaceNodeDrawable | lambda 内判空 | OnGenerate 全路径非空 |
| 3 | rs_render_pipeline_client.cpp:749-761 surfaceBufferCbDirector_ | check-then-new 竞态 | 写点全在锁内 |
| 4 | rs_transaction_data.cpp:387-441 payloadLock/max_size() | 死锁/异常/未持锁访问 | RAII 析构 + 对象未发布 |
| 5 | rs_surface_capture_task_parallel.cpp:237-241 surfaceNodeParams | static_cast 后解引用 | 构造不变量 |
| 6 | rs_main_thread.cpp:1930-1970 effectiveCommands_ | 锁内 swap 锁外 clear | 所有权转移模式 |
| 7 | rs_main_thread.cpp:2493-2498 GetGlobalRootRenderNode | 根节点判空 | NSDMI 单例成员 |
| 8 | rs_main_thread.cpp:2864 ReinterpretCastTo 裸调用 | 类型转换返回空 | 入队类型闸门 |
| 9 | rs_main_thread.cpp:3310-3315 params/surfaceHandler | 双重解引用判空 | 注册/构造双重不变量 |
| 10 | rs_main_thread.cpp:6361 CHECK_INTERVAL 取模 | 除零 | 短路求值 |
| 11 | rs_main_thread.cpp pendingSplitTransactions_ | OOM | 三重有界约束 |
| 12 | rs_render_service_listener.cpp:262-276 OnTransformChange handler | weak_ptr lock 后判空 | 同源生命周期 |
| 13 | rs_render_pipeline_client.cpp:550-589 TakeSurfaceCaptureWithAllWindows 锁段 | 锁内初始化锁外入参使用 | publish-once + 锁 happens-before |
| 14 | rs_render_pipeline_agent.cpp 全类 GetMainThread/GetUniRenderThread | 线程指针判空 | 构造期单例赋值 + 初始化时序 |
| 15 | rs_render_pipeline_agent.cpp:1608-1609 GetAppGpuMemoryInMB 链式调用 | 三级链式裸解引用 | 初始化时序 + 同线程 FIFO |
| 16 | rs_render_pipeline_agent.cpp:1670 CollectSurfaceBuffersByProcessId | handler/GetBuffer 判空 | 构造不变量 + 空值透传设计 |
| 17 | rs_render_pipeline_agent.cpp:1672 GetBoundsGeometry | 链式裸解引用 | NSDMI 值成员 + 构造期 make_shared |
| 18 | rs_render_pipeline_agent.cpp SetRogScreenResolution | PostMainThreadSyncTask 返回值 | 初始化时序保证必执行 |
| 19 | rs_unmarshal_thread.cpp RSMainThread::Instance() | 单例判空 | 函数局部静态，地址永真非空 |
| 20 | rs_client_to_service_connection.cpp:1725-1743 ShowWatermark watermarkImg | 参数判空 | 入口校验 + 出口自防双层结构 |
| 21 | rs_render_pipeline.cpp:158-177 OnScreenConnected rsScreenProperty | 判空不一致 | 调用方保证非空（实屏 :74 先裸解引用） |
| 22 | rs_render_interface.cpp:532-546 GetMaxGpuBufferSize renderPipelineClient_ | 成员判空 | 构造函数无条件 make_shared，构造后冻结 |

## 问题 1：RSUiCaptureTaskParallel::Run 中 endNodeDrawable_（rs_ui_capture_task_parallel.cpp:366-374）

```cpp
if (HasEndNodeRect()) {
    if (endNodeDrawable_->GetRenderParams()->GetMatrix().Invert(invertMatrix)) {
        relativeMatrix.PreConcat(invertMatrix);
    }
}
```

- 疑问：`endNodeDrawable_` 与 `GetRenderParams()` 两级是否需要判空。
- 结论：**均不需要，非缺陷**。
- 依据：
  1. **endNodeDrawable_ 的创建先于 Run 且带判空**。它只在 `CreateResources()`
     （:217-241）的 `HasEndNodeRect() && !isStartEndNodeSame_` 分支创建，
     创建后立即判空（:233-236），任何失败路径都 `return false`；
     上游 `Capture()`（:164）对 false 直接回调并 return，`Run()` 所在的
     captureTask lambda 根本不会被投递到渲染线程。全仓唯一入口
     rs_render_pipeline_agent.cpp:456，无绕过路径。
  2. **"同节点场景必创建"由调用顺序保证**。`isStartEndNodeSame_` 是成员默认值
     false（rs_ui_capture_task_parallel.h:110），构造函数不设置，唯一赋值点在
     `IsStartEndSameNode()`（:847），而它在 `Capture()` 中晚于 `CreateResources()`
     被调用（:164 与 :177 的顺序）。因此 `CreateResources` 执行时该标志恒为
     false，条件退化为 `HasEndNodeRect()`——只要走 endNode 路径必创建 drawable
     （含起点终点同 id 场景，此时用起点节点生成）。
  3. **renderParams_ 永真非空**。`OnGenerate` → `InitRenderParams`（adapter.cpp
     :158-184）switch 全分支 `make_unique` 创建（含 default 分支），全仓无
     `renderParams_ = nullptr` 或 reset 路径。drawable 存活 ⇒ params 非空。
- 残留风险（记录备查）：:366 条件（只判 `HasEndNodeRect()`）与 :384
  （`HasEndNodeRect() && !isStartEndNodeSame_`）不对称，安全性依赖
  "IsStartEndSameNode 必须晚于 CreateResources"的隐式跨函数时序。若未来有人
  把 :177 前移（看似无害的整理），同节点场景将变成真空指针。
- 处置：不修改。可选加固：:366 条件对齐 :384。
- 讨论修正记录：首轮分析曾认为"同节点场景可达空指针"，用户追问
  "endNodeDrawable_ 为什么会是空"后重新核对 `Capture()` 内 :164/:177 的执行
  顺序，确认该路径不成立，以本节结论为准。

## 问题 2：RSSurfaceCaptureTaskParallel::ClearCacheImageByFreeze（rs_surface_capture_task_parallel.cpp:176-191）

```cpp
if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
    auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    std::function<void()> clearCacheTask = [id, surfaceNodeDrawable]() -> void {
        surfaceNodeDrawable->SetCacheImageByCapture(nullptr);
    };
    RSUniRenderThread::Instance().PostTask(clearCacheTask);
}
```

- 疑问：lambda 异步执行时 `surfaceNodeDrawable` 是否存在空指针解引用。
- 结论：**不存在，非缺陷**。
- 依据（OnGenerate 在此调用上下文的全部返回路径逐一排除）：
  1. node 为 null 返回 null（adapter.cpp:104-106）——不可达，:179 已判空且
     走 surfaceNode 分支。
  2. GeneratorMap 无此类型返回 null（:137-141）——不可达，SURFACE_NODE 由
     静态注册器 `RSSurfaceRenderNodeDrawable::instance_`
     （rs_surface_render_node_drawable.cpp:92）在库加载时注册，先于任何
     业务代码执行。
  3. 生成器路径（:143）返回 `new RSSurfaceRenderNodeDrawable(...)`（drawable.cpp
     :148-152）——OHOS 禁异常，new 失败直接 abort 而非返回空。
  4. 缓存命中路径（:122-128）返回 `lock()` 成功结果——`lock()` 非空才返回。
- 生命周期：lambda 按值捕获 shared_ptr（拷贝），`PostTask` 异步投递到渲染线程
  期间对象保活，无悬空；`static_pointer_cast` 类型正确
  （`RSSurfaceRenderNodeDrawable : public RSRenderNodeDrawable`）。
- 处置：不修改。

## 问题 3：RegisterSurfaceBufferCallback 的 surfaceBufferCbDirector_（rs_render_pipeline_client.cpp:748-762）

```cpp
{
    std::unique_lock<std::shared_mutex> lock { surfaceBufferCallbackMutex_ };
    if (surfaceBufferCallbacks_.find(uid) == std::end(surfaceBufferCallbacks_)) {
        surfaceBufferCallbacks_.emplace(uid, callback);
    } else { ... return false; }
    if (surfaceBufferCbDirector_ == nullptr) {
        surfaceBufferCbDirector_ = new SurfaceBufferCallbackDirector(weak_from_this());
    }
}   // 锁到这里才释放
clientToRenderConnection->RegisterSurfaceBufferCallback(pid, uid, surfaceBufferCbDirector_);
```

- 疑问：`surfaceBufferCbDirector_` 的 check-then-new 是否有 TOCTOU。
- 结论：**没有，非缺陷（正确参照写法）**。
- 依据：
  1. **写点互斥完备**：全仓 `surfaceBufferCbDirector_` 仅 :757-758 一个写点，
     位于 `unique_lock`（写锁）临界区内。并发不同 uid 的注册在 :749 串行化，
     第二个线程进入时必然看到第一个已发布的 director（锁的释放-获取构成
     happens-before），不存在双线程同时 new 导致的 sptr 无锁写竞争与对象泄漏。
  2. **:761 锁外读安全**：本线程刚在临界区内确认/创建（同线程顺序可见）；
     一旦创建，该成员无任何置空/重赋值路径，是单次初始化（monotonic）成员，
     读到的值永久有效。
  3. Unregister（:773-780）与 Trigger 路径（:789、:806）只操作
     `surfaceBufferCallbacks_`（分别持写锁/读锁），不碰 director。
- 对比：本文件四个兄弟函数（TakeSurfaceCapture :461、TakeSelfSurfaceCapture :506、
  SetWindowFreezeImmediately :542、TakeUICaptureInRange :627）的
  `surfaceCaptureCbDirector_` 创建在锁外，存在双重 new + sptr 无锁写竞争
  （缺陷已另行立案）。本函数与 `transactionDataCbDirector_`（:864，见汇总报告
  #18）是应对齐的正确写法：**director 创建移入临界区即可，无需更多改动**。
- 附带既有行为（非竞态，记录备查）：:761 的 IPC 若失败，map 条目不回滚
  （无 erase），该 uid 后续注册永远返回 "callback exists" 直到进程重启。
  Register 类接口通常由系统侧单次调用，实际影响有限。
- 处置：不修改。

## 问题 4：RSTransactionData::UnmarshallingCommand 的 payloadLock 与 max_size()（rs_transaction_data.cpp:387-441）

```cpp
size_t readableSize = parcel.GetReadableBytes() / minCommandSize;      // :387 未持锁
size_t len = static_cast<size_t>(commandSize);                         // :388 commandSize 可为负
if (len > readableSize || len > payload_.max_size()) { return false; } // :389 未持锁
...
std::unique_lock<std::mutex> payloadLock(commandMutex_, std::defer_lock);  // :395
for (size_t i = 0; i < len; i++) {
    ...
    payloadLock.lock();            // :427
    payload_.emplace_back(...);
    payloadLock.unlock();          // :437
}
```

- 疑问 1：手动 lock/unlock 是否存在死锁。
- 疑问 2：`emplace_back` 抛 `bad_alloc` 时 unlock 不执行、defer_lock 构造导致
  析构调 `release()`，`commandMutex_` 是否永久锁死。
- 疑问 3：:389 `payload_.max_size()` 未持锁调用是否有多线程问题。
- 结论：**三项均无风险，非缺陷**。
- 依据：
  1. **死锁四成因逐一排除**：锁窗口（:427-437）内只有 trace 格式化、RS_LOGW、
     `emplace_back`，无任何重入 `commandMutex_` 的调用（入口 :363 的 `Clear()`
     虽然内部也加锁，但在 :395 defer_lock 构造之前已释放，无重叠）；窗口内只
     获取 hilog/hitrace 叶子锁，无 ABBA 环；lock/unlock 之间无 return/continue
     （:438 的 else 分支在锁外），OHOS 禁异常、emplace_back 无抛出路径，配对
     完整；对象在 `Unmarshalling`（:49-52）中 `new` 出来、unmarshal 完成前不
     发布给任何线程，跨线程争用不可达（分段并行 unmarshal 同样各自 new 独立对象）。
  2. **defer_lock 疑问已用可运行程序验证**（/tmp/opencode/unique_lock_defer_test.cpp，
     g++ -std=c++17 -pthread）：抛 bad_alloc 与提前 return 两种场景锁均正确
     RELEASED。机制澄清：`defer_lock` 只影响构造函数行为，`~unique_lock()` 析构
     只看 `owns` 标志（`lock()` 已置位为 true）→ 调用 `unlock()`；`release()`
     是需要显式调用的成员函数（解除关联且不解锁），析构从不调用它。
     "只要持有，析构必解锁"正是 unique_lock 相对手动 mutex::lock/unlock 的
     核心价值。首轮曾误判"插入提前 return 会保持持有 → 析构 Clear() 自死锁"，
     验证后收回。
  3. **max_size() 未持锁安全**：:389 确实在 payloadLock 构造（:395）之前执行，
     但同上"对象未发布"不变量覆盖此行——unmarshal 期间无第二线程能触达该
     对象，不存在并发读写。且 `vector::max_size()` 本身是 const、无分配、
     与容器内容无关的纯查询，空容器调用亦合法。
  4. **max_size 检查实际冗余（无害的防御）**：`readableSize` 受
     PARCEL_MAX_CAPACITY（4MB，:36）/ 最小命令尺寸（5 或 14 字节）约束，上限约
     80 万；而 64 位下 `payload_.max_size()` ≈ 7.7e17。`len > readableSize`
     永远先触发。`commandSize` 为负时 `static_cast<size_t>` 产生天文数字
     （-1 → SIZE_MAX），同样被第一条件拦截——真正的限流防线是 readableSize
     反推上界，max_size 只是锦上添花的兜底。
- 残留（理论性质）：若工程将来放开异常，`emplace_back` 抛出会跳过
  `Unmarshalling` 的 `delete transactionData`（:64）造成对象泄漏——但锁仍正确
  释放，与死锁无关；OHOS 默认 -fno-exceptions 下分配失败直接 abort，泄漏也不存在。
- 处置：不修改。可选改进（纯可读性）：手动 lock/unlock 改为 lock_guard 作用域块；
  该锁在当前调用图下实际空转（对象未发布期间无争用方），每轮 lock/unlock 仅
  固定开销，若确认无"边填边读"场景可评估移到循环外一次持锁。

## 问题 5：RSSurfaceCaptureTaskParallel::CreateResources 中 surfaceNodeParams（rs_surface_capture_task_parallel.cpp:232-242）

```cpp
surfaceNodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(curNode));
if (!surfaceNodeDrawable_) {
    return false;
}
auto surfaceNodeParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable_->GetRenderParams().get());
bool isF16Capture = (captureConfig_.needF16WindowCaptureForScRGB && RSHdrUtil::NeedUseF16Capture(curNode)) ||
    (captureConfig_.isHdrCapture && surfaceNodeParams->SelfOrChildHasHDR());
```

- 疑问：直接对 `surfaceNodeParams` 解引用是否有空指针风险。
- 结论：**没有可达风险，非缺陷**。与问题 1 同源（同一构造不变量）。
- 依据：
  1. `surfaceNodeDrawable_` 已判空（:234-236），null 直接 `return false`。
  2. `GetRenderParams().get()` 保证非空：`renderParams_` 在 `OnGenerate →
     InitRenderParams` switch 全分支 `make_unique` 创建（含 default 分支），
     此后无置空/重赋值路径；drawable 存活 ⇒ params 非空。缓存命中路径
     （`node->renderDrawable_` 或 `RenderNodeDrawableCache_`）的条目也是生成
     完成+初始化后才发布，且本函数在主线程执行，render node 树访问无跨线程竞争。
  3. `static_cast<RSSurfaceRenderParams*>` 类型正确：`curNode` 为
     `RSSurfaceRenderNode`（含 leash window 父节点，SURFACE_NODE；protective
     solid 变体同样映射 `RSSurfaceRenderParams`，adapter.cpp:177-178），
     向下转型合法，`SelfOrChildHasHDR()` 调用安全。
- 备注：同文件 `Run()` 中同模式代码有防御性判空（:287-294
  `curNodeParams == nullptr`），`CreateResources` 没有，属防御层次不统一，
  非缺陷。
- 处置：不修改。

## 问题 6：RSMainThread::ProcessCommandForDividedRender 的 effectiveCommands_（rs_main_thread.cpp:1930-1970）

```cpp
{
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    if (!pendingEffectiveCommands_.empty()) {
        effectiveCommands_.swap(pendingEffectiveCommands_);   // 锁内：所有权交接
    }
    for (auto& [surfaceNodeId, commandMap] : cachedCommands_) { ...   // 锁内：合并
        effectiveCommands_[it->first].insert(...);
    }
}
for (auto& [timestamp, commands] : effectiveCommands_) {      // 锁外：遍历执行
    context_->transactionTimestamp_ = timestamp;
    for (auto& command : commands) { command->Process(*context_); }
}
effectiveCommands_.clear();                                    // 锁外：清空
```

- 疑问：`effectiveCommands_` 在锁内被 swap、锁外被遍历和 clear，是否有并发问题。
- 结论：**没有，非缺陷**。这是"锁保护交接点、锁外处理独占数据"的标准模式。
- 依据：
  1. **effectiveCommands_ 是函数私有工作区**：全仓仅本函数 4 个使用点
     （:1937、:1954、:1960、:1968），没有任何其它代码路径——持锁的或锁外的
     ——能碰到这个对象。它对外的唯一"暴露时刻"就是 :1937 swap 一瞬间，
     而那在锁内完成。
  2. **锁外段落操作的是 swap 之后的独占副本**：swap 后 `pendingEffectiveCommands_`
     变空，若另一线程此刻拿到锁写 pending（如 ClassifyRSTransactionData :4418），
     写的是**不同的对象**，与正在遍历的 effectiveCommands_ 毫无别名关系，
     clear 不与任何人竞争。
  3. **调用线程单一**：唯一调用点 `ProcessCommand`（:1484，!isUniRender_ 分支）
     在 RSMainThread 事件循环每帧执行；生产者 `ClassifyRSTransactionData`
     （:4410-4432，写 pendingEffectiveCommands_）经 `RecvRSTransactionData` 的
     PostTask（:4400-4405）投递到同一线程，分类动作串行化在主线程。锁
     （:4413、:1935）是防备未来直接跨线程调用的加固。
  4. **clear 不放进锁内是有意设计**：`command->Process(*context_)` 是重活
     （节点树更新），若整段持锁会阻塞所有 IPC 事务写入方
     （cachedTransactionDataMap_ 的 emplace :4397 等同一把锁）。
- 处置：不修改。可选形式优化：把 effectiveCommands_ 改为函数局部变量
  （`std::map<...> effectiveCommands;`）语义更直白——成员声明反而暗示跨函数
  共享，是本次疑问的根源。

## 问题 7：RSMainThread::CheckIfHardwareForcedDisabled 的 rootNode（rs_main_thread.cpp:2488-2511）

```cpp
std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
...
const auto& children = rootNode->GetChildren();   // 未判空
```

- 疑问：`rootNode` 是否有空指针解引用风险。
- 结论：**没有，非缺陷**。
- 依据：
  1. **构造不变量**（rs_context.h:285）：
     ```cpp
     std::shared_ptr<RSBaseRenderNode> globalRootRenderNode_ = std::make_shared<RSRenderNode>(0, true);
     ```
     NSDMI（类内默认初始化）在 RSContext 构造时即创建，全仓**无任何重赋值/
     置空语句**。RSContext 一存在，该 shared_ptr 必非空。
  2. `context_` 有效性：本函数唯一调用点 `Render()`（:2301）在主线程渲染循环，
     context 早已初始化（且同帧更早路径已大量使用 `context_->`）。
  3. `GetChildren()` 同样不返回空：返回成员引用（RSBaseRenderNode 构造时创建），
     :2499 直接 begin() 安全。
  4. 旁证：`GetGlobalRootRenderNode()` 在 rs_main_thread.cpp 有 11 处使用，仅
     :4696 一处防御式判空，其余 10 处（含 rs_root_node_command.cpp:51 的
     `->AddChild` 链式调用）都裸解引用——代码库整体视为永真非空，是共识假设
     而非疏忽。
- 处置：不修改。

## 问题 8：RSMainThread::CheckWindowCapTasks 的 ReinterpretCastTo 裸调用（rs_main_thread.cpp:2858-2876）

```cpp
auto node = nodeMap.GetRenderNode(nodeId);
if (!node) { ... continue; }
auto nodeState = node->ReinterpretCastTo<RSSurfaceRenderNode>()->GetAbilityState();   // :2864 未判空
```

- 疑问：`ReinterpretCastTo` 类型不匹配时返回 nullptr，此处裸调用是否崩。
- 结论（按"仅空指针、正常业务流程"口径）：**非缺陷，无需判空**。
- 依据：
  1. **入队有类型闸门**：任务唯一生产路径 `AddWindowCapTask`（:2818）←
     rs_render_pipeline_agent.cpp:588，上游 :581 已确认
     `node->GetType() == RSRenderNodeType::SURFACE_NODE` 才入队。
  2. **类型不可变**：节点类型构造时确定、生命周期内不变，`IsInstanceOf`
     检查恒通过 → 返回 `static_pointer_cast` 结果非空
     （rs_render_node.h:380-383）。
  3. **销毁已处理**：等待期间节点被销毁 → `GetRenderNode` 返回 null →
     :2858 拦截，到不了 :2864。
  4. nodeMap 同一 id 不会在正常流程中"换类型"复用。:2867（parentNode 已判空）、
     :2876（leash window 必为 surface）均同构安全。
- 附注（超出空指针口径，仅记录不立案）：恶意 id 复用场景（surface 节点销毁后
  同 id 创建 canvas 节点，两类命令的 nodeId 均客户端可控且只校验 pid 前缀）
  可在跨帧等待窗口造成类型混淆，:2864/:2876/:2894-2896 接续 UB，构成非系统
  应用对 render service 的 DoS。抵抗该场景需在消费点补类型闸门，另立项处理。
- 处置：不修改。

## 问题 9：DoDirectComposition PostProcess 的 params/surfaceHandler（rs_main_thread.cpp:3298-3341）

```cpp
auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
if (!surfaceNode->IsHardwareForcedDisabled()) {
    auto params = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    bool isCurrentFrameBufferConsumed = surfaceHandler->IsCurrentFrameBufferConsumed();  // 两级均未判空
    if (isCurrentFrameBufferConsumed) {
        auto preBufferOwnerCount = params->GetPreBufferOwnerCount();
        ...
```

- 疑问：`params` 与 `surfaceHandler` 是否需要判空。
- 结论：**均不需要，非缺陷**（但两项保证来源不同，分别记录）。
- 依据：
  1. **params（stagingRenderParams_）——注册时机不变量**：节点经 `OnRegister`
     （rs_render_node.cpp:215-221）→ `InitRenderParams`（surface 版，
     rs_surface_render_node.cpp:3309-3312）**无条件**
     `stagingRenderParams_ = make_unique<RSSurfaceRenderParams>(GetId())`。
     节点能进入 nodeMap / hardwareEnabledNodes_ 必然已注册，params 必非空。
     唯一置空路径 `ResetRenderParams`（:330-334）全仓零调用（死代码）。
     与 drawable 侧 renderParams_（问题 1/5）是两套 params，保证来源不同
     （OnRegister vs OnGenerate），注意区分。
  2. **消费点先决条件更强**：遍历源 `hardwareEnabledNodes_`（:3298）是本帧
     prepare 阶段筛选的 hardware-enabled surface 节点，这些节点刚经历完整
     prepare 流程（`UpdateRenderParams` 等重度依赖 stagingRenderParams_），
     同一帧内刚被使用的对象不可能为空。
  3. **surfaceHandler——构造期三元兜底**（rs_surface_render_node.cpp:163-166
     构造函数初始化列表）：
     ```cpp
     surfaceHandler_(surfaceHandler ? surfaceHandler : std::make_shared<RSSurfaceHandler>(config.id))
     ```
     传入非空则用传入值，传入空则当场创建——**无论哪种情况成员必非空**，
     对象诞生起成立。全仓 `surfaceHandler_` 赋值仅此一处（:2324 成员声明后
     无重赋值；grep 到的 `SetSurfaceHandler` 是 RSCanvasDrawingRenderNode
     另一个类的成员，无关）。且主线程消费路径（rs_main_thread.cpp:2148-2152）
     同帧已大量使用该 handler。
  4. 旁证：`RSUniHwcComputeUtil` 里同模式代码判了空
     （rs_uni_hwc_compute_util.cpp:45-47）——但那是被多种节点类型复用的工具
     函数，防御层次不同；本处节点来自强先决条件筛选列表，裸解引用合理。
- 处置：不修改。

## 问题 10：ProcessSplitTransactionCommands 的 CHECK_INTERVAL 取模（rs_main_thread.cpp:6361）

```cpp
if (!isTotalTimeExceeded && CHECK_INTERVAL != 0 && i % CHECK_INTERVAL == 0) {
```

- 疑问：`i % CHECK_INTERVAL` 是否有除零风险。
- 结论：**没有，非缺陷**。
- 依据：C++ 的 `&&` 保证从左到右短路求值，`CHECK_INTERVAL != 0` 为 false 时
  右侧 `i % CHECK_INTERVAL` 不会求值；到达 `%` 时 CHECK_INTERVAL 必为非零。
  `CHECK_INTERVAL` 是 `size_t`（无符号，:6341），来自系统参数
  （GetSplitTransactionCheckInterval，负值被钳为默认 200，
  rs_system_properties.cpp:1247-1255），不存在负数取模的符号问题。
- 处置：不修改。唯一引入风险的方式是未来重构把 `!= 0` 检查挪到 `%` 之后或
  拆开条件——当前写法顺序正确。

## 问题 11：pendingSplitTransactions_ 无容量上界（rs_main_thread.h / rs_main_thread.cpp:6310-6326）

```cpp
std::unordered_map<pid_t, SplitRebuildState> pendingSplitTransactions_;   // 无显式容量上限
auto& state = pendingSplitTransactions_[pid];
state.transactions.push_back(std::move(transaction));                     // 无条数上限
```

- 疑问：无容量上界限制是否存在 OOM 风险。
- 结论：**无无界 OOM 风险，非缺陷**。增长被三重机制约束，最坏是有界瞬时尖峰。
- 依据：
  1. **key（pid 数量）有界且带死亡清理**：入队前提是持有效 RS 连接
     （binder capability），进程数受系统上限；每个 pid 的 entry 在进程死亡时
     由 `CleanResources` → `ClearRebuildTransactionData`（:878-882）删除，
     不因进程退出泄漏。
  2. **每 pid 队列寿命被 100ms 硬上限截断（关键防线）**：`startTimeMs` 只在
     队列空→非空时设置（:6320-6322），burst 期间不重置；默认
     `MAX_TOTAL_TIME_MS = 100ms`（rs_system_properties.cpp:1221）。
     `isTotalTimeExceeded`（:6354）一旦为真，跳过时间片检查、单帧强制清空
     整个剩余队列（anti-starvation cap），清空即 erase（:6391）。攻击者无法
     续命：持续灌入只保持 startTimeMs 不变，100ms 后照样强制排干；排干后再灌
     已是新 burst（内存已释放）。
  3. **峰值内存 = 生产速率 × 100ms，生产侧受节流**：单笔 parcel 受
     PARCEL_SPLIT_THRESHOLD（1800KB）/ PARCEL_MAX_CAPACITY（4MB）与 binder
     传输约束，unmarshal 线程逐笔反序列化构成天然节流。
- 残余风险（如实说明，均非 OOM）：100ms 窗口内的内存尖峰在低内存设备上可能
  加剧压力（低危）；强制排干那一帧要一口气处理整个队列 → 主线程卡顿
  （性能问题，非内存问题）。
- 处置：不修改。若要防御纵深，可加每 pid 队列条数上限（超限丢弃并打日志），
  按现有三重约束不是必须。

## 问题 12：RSRenderServiceListener::OnTransformChange 的 handler（rs_render_service_listener.cpp:254-277）

```cpp
RSMainThread::Instance()->PostTask([surfaceRenderNode = surfaceBufferInterface_, surfaceHandler = surfaceHandler_,
                                       nodeId = nodeId_, nodeName = name_]() {
    auto node = surfaceRenderNode.lock();
    auto handler = surfaceHandler.lock();
    if (node == nullptr) { ... return; }        // 只判了 node
    node->OnTransformChange();
    ...
    handler->SetBufferTransformTypeChanged(true);   // handler 未判空
});
```

- 疑问：`handler` 在 lambda 内 lock 后未判空，是否有空指针风险。
- 结论：**实际风险为零，非缺陷**。与同文件 `OnGoBackground` 的判空差异
  **仅是防御风格不同，不是它发现了本处漏掉的真实场景**。
- 依据：
  1. **两个 weak_ptr 同源**：listener 唯一构造点（rs_render_pipeline_agent.cpp
     :1389-1392）：
     ```cpp
     std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(node);
     std::weak_ptr<RSSurfaceHandler> surfaceHandler(node->GetRSSurfaceHandler());
     ```
     `surfaceHandler` 就是 node 的成员 `surfaceHandler_`。
  2. **蕴含关系保证安全**：node 拥有其 surfaceHandler_ 成员的强引用链
     （构造期无条件创建、无重赋值路径，见问题 9 第 3 点）。因此
     `handler.lock() 为空 ⇒ node 已销毁 ⇒ node.lock() 也为空`，
     而 `node == nullptr` 分支已拦截——能走到 `handler->SetBuffer...` 时
     node 非空 ⇒ handler 必非空。矛盾保证该行不可达空指针。
  3. `OnGoBackground`（:235-238 先判 handler 再判 node）只是防御顺序不同；
     两者覆盖的场景集合在该同源前提下等价。
- 依赖前提（记录备查）：该保证依赖 ① `RSSurfaceRenderNode::surfaceHandler_`
  永不替换；② 两个 weak_ptr 永远同源构造。任一被未来代码打破（如给 surface
  节点加 handler 替换接口），本处立即变成真空指针。
- 处置：不修改。可选风格对齐：lambda 内补 `if (handler == nullptr) return;`
  （一行成本消除对隐式前提的依赖）。

## 问题 13：TakeSurfaceCaptureWithAllWindows 的锁段（rs_render_pipeline_client.cpp:550-589）

```cpp
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = surfaceCaptureCbMap_.find(key);
    if (iter != surfaceCaptureCbMap_.end()) {
        iter->second.emplace_back(callback);     // 搭车
        return true;
    }
    std::vector<std::shared_ptr<SurfaceCaptureCallback>> callbackVector = { callback };
    surfaceCaptureCbMap_.emplace(key, callbackVector);
    if (surfaceCaptureCbDirector_ == nullptr) {
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(weak_from_this());  // 锁内创建
    }
}
auto ret = clientToRenderConnection->TakeSurfaceCaptureWithAllWindows(...);   // 锁外 IPC 入参使用
if (ret != ERR_OK) {
    std::lock_guard<std::mutex> lock(mutex_);
    ...
    return false;
}
```

- 疑问 1：map 占位与 director 创建是否有 TOCTOU。
- 疑问 2：**"锁内初始化、锁外函数入参使用"这一模式是否会引入竞态**。
- 结论：**锁段与该模式均无问题，非缺陷**。本函数是五个使用
  surfaceCaptureCbDirector_ 的函数里唯一写对的一个。
- 依据：
  1. **锁段原子**：`find` / `emplace` / director 创建在同一个 `mutex_` 临界区内
     （:564-578），check-then-act 原子：并发同 key 请求要么搭车（:569）要么
     占位，无窗口。IPC 放锁外也正确（不能持锁跨 binder 调用）。
  2. **"锁内初始化、锁外使用"模式的安全性证明（三条件合取）**：
     - 条件①（写点唯一且在锁内）：`surfaceCaptureCbDirector_` 的唯一写操作是
       null→非空的首次创建（check-then-new 整体在临界区内），此后全仓无置空/
       重赋值路径——publish-once 成员。
     - 条件②（读前必经持锁检查）：锁外读发生在本线程刚退出临界区之后，
       该线程必然在锁内确认过非空（否则自己就是写者）。锁的释放-获取与任何
       后续临界区构成 happens-before。
     - 条件③（无并发写）：唯一写最多发生一次，且必然先于任何锁外读完成
       （否则该读者在锁内见到的就是 null）。此后锁外只可能与其它**读**并发，
       读读无竞争。
     形式化：写最多一次且被锁串行化并先于所有锁外读完成 ⇒ 锁外读不与任何写
     并发 ⇒ 无数据竞争。入参按 sptr 值拷贝，IPC 往返期间强引用保活，服务端
     注册后再持有一份，生命周期闭合。
  3. **该模式会出竞态的变体（对照，帮助识别）**：

     | 变体 | 后果 | 仓内实例 |
     | --- | --- | --- |
     | check-then-new 移到锁外 | 双重 new、sptr 无锁写竞争、对象泄漏 | 四个兄弟函数（:461/:506/:542/:627，总报告 #5 立案） |
     | 存在置空路径（如 Unregister 时置 nullptr） | 锁外读与置空写并发，或空指针传入 IPC | 当前不存在 ✓ |
     | 读方线程未经持锁检查直接无锁读 | 与首次创建写并发 | 当前不存在（每处锁外读都在同函数紧邻锁段后）✓ |
     | 所搭的锁被移除/更换（该成员搭 map 锁的便车） | 全部保证无声失效 | 隐患，见建议 |
  4. **判定规则**：锁外使用本身不是风险点，风险点是"是否存在与该使用并发的写"。
     对 publish-once 成员，唯一写被锁串行化且必然先于任何锁外读完成——安全；
     一旦引入置空/重赋值，或读方绕过锁，平衡即破。
- 建议（加固性质，非必改）：这类"搭便车"保护宜在成员声明处显式化
  （如注释 `// protected by mutex_, created lazily, never reset`），把三个
  隐式条件写明——否则未来优化锁粒度或加置空逻辑时，没有编译期或评审期信号。
  其余四个兄弟函数整改时直接对齐本函数 :575-577 的写法。

## 问题 14：RSRenderPipelineAgent 全类的 GetMainThread/GetUniRenderThread（rs_render_pipeline.h:128-142）

- 疑问：agent 所有函数里 `GetMainThread()`/`GetUniRenderThread()` 是否需要判空。
- 结论：**不需要，非缺陷**。
- 依据：
  1. **成员初始化在 pipeline 构造内同步完成，且来源是进程单例**：
     - `InitMainThread`（rs_render_pipeline.cpp:284-291）：
       `mainThread_ = RSMainThread::Instance()`——单例 Instance() 永不返回空；
     - `InitUniRenderThread`（:314）：`uniRenderThread_ = &(RSUniRenderThread::Instance())`——单例引用；
     - 两者都在 `RSRenderPipeline::Create → Init`（:77-78 → :94-96）中执行，
       `Create` 返回前完成。此后全仓无任何重新赋值/置空——构造后即冻结。
  2. **agent 时序保证**：`RSRenderPipelineAgent` 持有的 `renderPipeline_`（weak_ptr）
     指向的 pipeline 只能来自 `RSRenderPipeline::Create` 的返回值——agent 拿到
     pipeline 时，两个线程指针必然已初始化。agent 中每个函数开头
     `rsRenderPipeline_.lock()` 成功 ⇒ pipeline 完整构造 ⇒ 线程指针必非空。
  3. **代码库佐证**：agent 里 110 处 `GetMainThread()`、11 处 `GetUniRenderThread()`
     调用，仅 2 处判空（:1582、:2096），其余 119 处全部裸用——共识假设永真。
  4. **pipeline 自身 Post\* 方法判空（:111、:128）不构成反证**：那些方法可能在
     Init 过程中被早期回调触发，与 agent 的 IPC 入口时机不同。
- 注意区分：线程指针本身不判空；但从它出发**再取的运行期对象**（如
  `GetRenderEngine()->GetRenderContext()` 等）是真实可空的运行期状态，agent 里对那些
  的判空（:1513-1519 等）是必要且正确的——不要因"线程不用判"而误删下游判空。
- 处置：不修改。

## 问题 15：GetAppGpuMemoryInMB 三级链式调用（rs_render_pipeline_agent.cpp:1608-1609）

```cpp
RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetDrGPUContext()
//                            ①                    ②                  ③
```

- 疑问：三级链式裸解引用是否需要判空。
- 结论：**不需要，非缺陷**（但链路保证来源分层，需区分"构造冻结"与"运行期状态"）。
- 依据：
  1. **① GetRenderEngine()（uniRenderEngine_）——时序保证，扎实**：
     `RSRenderPipeline::Create → Init → InitUniRenderThread`（:316）调
     `uniRenderThread_->Start()`，`Start()` 内 `InitGrContext()` 经 PostSyncTask
     投递（rs_uni_render_thread.cpp:267-270）——同步任务阻塞调用方直到执行完成，
     `Create` 返回时 `uniRenderEngine_` 必已 make_shared（失败即 abort）。
     线程未启动时 PostSyncTask 对空 handler_ 直接 no-op（:393-395），lambda 不执行。
  2. **② GetRenderContext()——正常流程非空，含状态分支**：默认返回
     `renderContext_`，GPU 构建下由 `uniRenderEngine_->Init()` 创建；初始化失败则
     `InitGrContext` 自己提前返回（:194-197），随后正常渲染本身无法进行——轮不到
     本查询先崩。唯一运行期变量：`isProtected_` 为 true 时改返回
     `protectedRenderContext_`（rs_base_render_engine.h:284-290），保护上下文按需创建
     平时可为 null。但本查询任务与渲染帧在同一线程队列串行，`isProtected_` 在帧内
     设置/恢复，需要"别处 bug 把保护态遗留在帧外"时此处才会取到空——需要另一个
     缺陷先行存在，正常流程不可达。
  3. **③ GetDrGPUContext()——与②共生**（renderContext 存在则伴随创建），
     且有第四级兜底：`MemoryManager::GetAppGpuMemoryInMB` 自身开头
     `if (!gpuContext) return 0.f;`（rs_memory_manager.cpp:514-516）。
     注意该判空只保护最后一环，救不了中间链的 `->` 解引用。
  4. **旁证**：兄弟函数 `GetMemoryGraphics`（:1633-1634）在使用前显式判了
     `GetRenderEngine() == nullptr || GetRenderContext() == nullptr`——但那是在
     IPC 线程跨线程读取，防御层次不同；本处在渲染线程自身执行，时序保证更强。
- 判空边界建议：判空的边界应划在"运行期可变状态"（engine/context）而不是
  "构造后冻结的指针"（线程）上。若做防御纵深，可对齐 GetMemoryGraphics 的两级判空
  （null 时 gpuMemSize 记 0），一行成本同时消化 isProtected_ 遗留的理论场景——
  属可选加固，非必改。
- 同模式在 rs_main_thread.cpp:5647-5648（GetAppMemoryInMB）亦有，同样分析适用。
- 处置：不修改。

## 问题 16：CollectSurfaceBuffersByProcessId 的 handler/GetBuffer（rs_render_pipeline_agent.cpp:1670）

```cpp
auto surfaceBuffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
```

- 疑问：`GetRSSurfaceHandler()` 和 `GetBuffer()` 是否需要判空。
- 结论：**不需要，非缺陷**。
- 依据：
  1. `surfaceNode`：`:1669` 的 `if (auto surfaceNode = ...)` 已判空。
  2. `GetRSSurfaceHandler()`：构造期三元兜底创建（rs_surface_render_node.cpp:166
     `surfaceHandler_(surfaceHandler ? surfaceHandler : make_shared<...>)`），
     全仓无重赋值/置空路径——node 活着 ⇒ handler 非空（问题 9 第 3 点同一不变量）。
  3. `GetBuffer()` 返回值：持锁返回 `buffer_.buffer` 的 sptr 拷贝
     （rs_surface_handler.h:213-217），**设计上允许返回 null**（节点尚未收到
     buffer 时）。这里用法正确：`surfaceBuffer` 被塞进 tuple（:1671-1673）作为
     数据透传，**本函数不解引用它**——与问题 11（solo pixelMap 空值透传）同模式。
     下游 `ConvertBuffersToPixelMaps` / `GetRotationInfoFromSurfaceBuffer`
     （:1777-1780）均已判空。
  4. 执行上下文：整个遍历运行在 `PostMainThreadSyncTask`（:1768）投递的主线程
     同步任务中，与节点树遍历同线程串行，无 TOCTOU 窗口。
- 处置：不修改。

## 问题 17：GetBoundsGeometry 链式调用（rs_render_pipeline_agent.cpp:1672）

```cpp
surfaceNode->GetRenderProperties().GetBoundsGeometry()->GetAbsRect()
```

- 疑问：三级链式是否有空指针风险。
- 结论：**没有，非缺陷**。
- 依据：
  1. `surfaceNode`：`:1669` 已判空。
  2. `GetRenderProperties()`：返回 `renderProperties_` 的 const 引用
     ——`renderProperties_` 是 `RSRenderNode` 的**值成员**（非指针，
     rs_render_node.h:532），node 存在即存在，不可能为空。
  3. `GetBoundsGeometry()`：返回 `boundsGeo_`（`std::shared_ptr<RSObjAbsGeometry>`，
     rs_properties.h:1144），在 `RSProperties` 构造函数中 `make_shared` 创建
     （rs_properties.cpp:160），全仓**唯一赋值点**，无 reset/置空——publish-once 成员。
     node 活着 ⇒ renderProperties 活着 ⇒ boundsGeo_ 非空。
- 处置：不修改。

## 问题 18：SetRogScreenResolution 的 PostMainThreadSyncTask 返回值（rs_render_pipeline_agent.cpp）

```cpp
pipeline->PostMainThreadSyncTask(task);
return ERR_OK;
```

- 疑问：是否需要检查 `PostMainThreadSyncTask` 的返回值。
- 结论：**不需要，非缺陷**。
- 依据：
  1. `mainThread_` 非空（初始化时序不变量，问题 14 已确立）：`pipeline` 非空 ⇒
     pipeline 完整构造 ⇒ `mainThread_` 已赋值。`PostMainThreadSyncTask` 内部
     `if (mainThread_ == nullptr) return false;`（rs_render_pipeline.cpp:136-138）
     在正常流程下不会触发。
  2. 主线程 handler 在 `RSMainThread::Init` 中创建，早于 IPC 服务发布；同步任务
     在 FIFO 队列中排队执行，无竞态。
  3. lambda 捕获安全：`renderPipeline = pipeline` 按 shared_ptr 强引用捕获，
     PostSyncTask 期间 pipeline 保活；`this` 指向 agent（上游 IPC 调用方持 agent 活）。
  4. 代码库旁证：同文件 9 处 `PostMainThreadSyncTask` 调用，仅 :269 一处检查
     返回值，其余 8 处全部不检查——共识认为"pipeline 非空 ⇒ task 必执行"。
- 附带小问题（非空指针，记录备查）：:1654 日志
  `"GetPidGpuMemoryInMB pipeline is nullptr, return"` 是从别处拷贝来的，
  函数名写错（应为 `SetRogScreenResolution`），不影响功能但建议修。
- 处置：不修改。

## 问题 19：rs_unmarshal_thread.cpp 全部 RSMainThread::Instance() 判空

- 疑问：文件中 7 处 `RSMainThread::Instance()` 调用（:159、:172、:179、:185、:191、:203、:224）是否需要判空。
- 结论：**不需要，非缺陷**。
- 依据：
  1. **`Instance()` 返回函数局部静态对象的地址，永真非空**（rs_main_thread.cpp:473-477）：
     ```cpp
     RSMainThread* RSMainThread::Instance()
     {
         static RSMainThread instance;   // 函数局部静态
         return &instance;              // 地址不可为空
     }
     ```
     C++11 保证静态初始化线程安全且只执行一次，`&instance` 不可能是空指针。
  2. **时序闭合**：`RSUnmarshalThread::Start()` 由 `RSMainThread::Init`
     （rs_main_thread.cpp:716）调用——unmarshal 线程启动在 main thread 初始化之后，
     其任务执行更晚，此时 `RSMainThread::Instance()` 早已完成静态初始化且 `Init`
     已执行完毕（handler_、context_ 等均已就绪）。
  3. **同源不变量**：#14 中 `mainThread_ = RSMainThread::Instance()`
     （rs_render_pipeline.cpp:289）也是该单例赋值——已确认线程指针永真非空，
     本项是同一不变量的直接调用侧。
  4. 仓内同类 `Instance()` 单例（`RSUniRenderThread::Instance()`、
     `RSUnmarshalThread::Instance()` 等）全仓裸调用无判空，同共识。
- 处置：不修改。

## 问题 20：RSClientToServiceConnection::ShowWatermark 中 watermarkImg（rs_client_to_service_connection.cpp:1725-1743）

- 疑问：`watermarkImg` 参数是否缺少判空。
- 结论：**不缺，非缺陷**。链路为"入口校验 + 出口自防"双层结构，本函数只是转发节点，
  函数体内无任何解引用 `watermarkImg` 的语句（仅透传给 persistence data 与各 conn）。
- 依据：
  1. **入口一**（render_server 侧 stub，rs_client_to_service_connection_stub.cpp:2337-2349）：
     `ReadParcelable` 后立即 `if (!watermarkImg) { ret = ERR_INVALID_DATA; break; }`，
     空值到不了本函数。
  2. **入口二**（render 进程侧 stub，rs_service_to_render_connection_stub.cpp:448-460）：
     同样判空拦截。
  3. **持久化路径**：`RegisterWithoutCallingPid` 只在入口判空之后执行，存入的
     `watermarkImg` 必非空；恢复路径 `ShowWatermarkPersistenceData::Unmarshalling`
     （rs_ipc_persistence_data.cpp:99-108）读失败直接返回 nullptr，不会被 Apply。
  4. **最终消费者自防**：`RSMainThread::ShowWatermark`（rs_main_thread.cpp:5505）
     使用处为 `if (flag && watermarkImg)`——即使未来新增入口漏判，最后关口也兜住。
- 处置：不修改。

- 处置：不修改。

## 问题 21：RSRenderPipeline::OnScreenConnected 中 rsScreenProperty 判空不一致（rs_render_pipeline.cpp:158-177）

```cpp
RS_LOGI(..., rsScreenProperty ? rsScreenProperty->GetScreenId() : INVALID_SCREEN_ID);  // :163 判空
if (!mainThread_) { ... return; }
if (rpDumpManager_) {
    rpDumpManager_->SetScreenId(rsScreenProperty ? rsScreenProperty->GetScreenId() : ...);  // :170 判空
}
mainThread_->OnScreenConnected(rsScreenProperty);   // :172 透传
if (rsScreenProperty->IsVirtual()) {                // :174 未判空（:187 还有第二次裸调用）
```

- 疑问：:174 对 rsScreenProperty 裸解引用，与 :163/:170 的三元判空不一致，是否有空指针风险。
- 结论：**按正常业务流程口径：非缺陷，业务上无风险**。判空差异只是实现风格不统一。
- 依据（调用方保证分析）：
  1. Caller 1（实屏，rs_render_single_process_manager.cpp:71-75）：:74
     property->GetScreenId() 在调用前就裸解引用——property 为空时崩在 :74，
     到不了 :174，隐式保证非空。
  2. Caller 2（虚屏，:95-98）：直接透传，无前置解引用；靠 ScreenManager
     创建屏幕时必然附带有效 RSScreenProperty 保证（正常流程成立）。
  3. Caller 3（multi_process :125-133）：透传，同 Caller 2 由 screen manager 侧保证。
- 备注（防御不对称，记录备查）：:163/:170 的三元判空说明作者曾认为该参数可空，
  :174 却裸解引用——若未来 screen manager 异常路径传入空 property 或虚屏路径新增
  不保证非空的入口，:174 会崩。加固一行：if (rsScreenProperty && rsScreenProperty->IsVirtual())。
- 处置：不修改。

- 处置：不修改。

## 问题 22：RSRenderInterface::GetMaxGpuBufferSize 的 renderPipelineClient_（rs_render_interface.cpp:532-546）

- 疑问：UniRender 分支 `renderPipelineClient_->GetMaxGpuBufferSize(...)` 是否需要判空。
- 结论：**不需要，非缺陷**。
- 依据：
  1. **构造不变量**（rs_render_interface.cpp:49-58）：两个构造函数都无条件
     `renderPipelineClient_ = std::make_shared<RSRenderPipelineClient>(...)`——
     对象存在即成员非空，无默认构造路径。make_shared 失败即 abort（OHOS 禁异常），
     不返回空。
  2. **无置空路径**：全仓对 renderPipelineClient_ 无 reset/置空/重赋值
     （除构造时的初始化）——构造后冻结。
  3. **旁证**：文件内 85 处 renderPipelineClient_ 使用，仅 3 处判空
     （:304/:314/:371），其余 82 处（含本函数 :544）裸用——判空属防御层次
     不统一，不是可达空指针的证据。
- 处置：不修改。

## 检视复用规则（本报告涉及）

1. **drawable renderParams_ 不变量**：drawable 经 OnGenerate 创建且已判空 ⇒
   GetRenderParams() 非空（问题 1、2、5）。
2. **NSDMI/构造期单例成员**：构造时无条件创建 + 全仓无重赋值路径的成员，
   调用点无需判空（问题 7 的 globalRootRenderNode_、问题 9 的 surfaceHandler_、
   问题 17 的 boundsGeo_；问题 9 的 stagingRenderParams_ 是 OnRegister 时机变体；
   问题 14 的 mainThread_/uniRenderThread_ 是构造期单例赋值）。
3. **锁内 swap/拷贝、锁外处理**：共享容器在临界区内完成所有权转移，重活移出
   锁外操作独占副本（问题 6；TriggerSurfaceCaptureCallback 同模式）。
4. **director 创建必须在锁内**：check-then-new 对 sptr 成员整体在临界区内完成
   （问题 3 正确；问题 13 的 TakeSurfaceCaptureWithAllWindows :575 亦正确）。
5. **publish-once 成员的锁外使用**：唯一写在锁内且先于所有锁外读完成、读前
   必经持锁检查、无置空路径——三者合取则锁外使用无竞态；判定要点是
   "是否存在与使用并发的写"（问题 13）。
6. **"对象未发布"覆盖未持锁访问**：new 出来、初始化完成前不交给其它线程的
   对象，其任何成员访问（含未持锁、含手动 lock/unlock 段）都无数据竞争
   （问题 4）。
7. **unique_lock 析构语义**：defer_lock 只影响构造；析构只看 owns 标志，
   异常/提前 return 均正确解锁；release() 需显式调用，析构从不调用（问题 4）。
8. **同源生命周期蕴含**：weak_ptr 指向另一对象的独占成员时，"成员死 ⇒ 属主死"，
   判属主即足够（问题 12）。
9. **初始化时序保证**：pipeline 构造（Create→Init）同步完成线程/引擎初始化，
   agent 拿到 pipeline 必然在构造完成之后（IPC 服务发布晚于构造）；
   PostSyncTask 的 InitGrContext 同步阻塞调用方，返回即完成
   （问题 14、15、18）。
10. **空值透传设计**：查询接口（如 GetBuffer）设计上允许返回 null，作为
    "该资源尚未就绪"的一等信号透传给下游，下游判空——调用点无需判
    （问题 16；问题 11 同模式）。
11. **判空边界规则**：判空应划在"运行期可变状态"（engine/context/protectedContext
    等 isProtected_ 分支可能影响的返回值）而不是"构造后冻结的指针"
    （线程指针、构造期 make_shared 的成员）上（问题 14 vs 15）。
