# 渲染服务安全检视问题记录（截图与连接路径 第二批）

日期：2026-08-18
检视范围：rosen/modules/render_service、render_service_base 截图、IPC 连接与 Vulkan 后端路径。
检视方式：静态代码走读，结合调用图与实际业务使用方式判断；未做设备验证，结论以当前 master 调用图为准。
关联文档：docs/reports/security-review-capture-and-connection-20260817.md（第一批，问题编号连续）。

## 问题一：RSUiCaptureTaskParallel::Run 中 endNodeDrawable_（rs_ui_capture_task_parallel.cpp:367）

```cpp
if (HasEndNodeRect()) {
    if (endNodeDrawable_->GetRenderParams()->GetMatrix().Invert(invertMatrix)) {
        relativeMatrix.PreConcat(invertMatrix);
    }
}
```

- 疑问：`endNodeDrawable_` 与 `GetRenderParams()` 是否需要判空。
- 结论：**不需要，非缺陷**。
  依据：
  1. `endNodeDrawable_` 在 `CreateResources()`（:217-241）创建并判空，任何失败路径都 `return false`，
     `Capture()` 在 :164 中止，`Run()` 根本不会被投递执行（全仓唯一入口
     rs_render_pipeline_agent.cpp:456，无绕过路径）。
  2. `IsStartEndSameNode()`（:847）在 `CreateResources()` 之后才调用（`Capture` :164 与 :177 的顺序），
     `CreateResources` 执行时 `isStartEndNodeSame_` 恒为 false（成员默认值，构造函数不设置），
     条件 `HasEndNodeRect() && !isStartEndNodeSame_` 退化为 `HasEndNodeRect()`，
     走 endNode 路径必创建 drawable（含同节点场景，此时用起点节点生成）。
  3. `renderParams_` 在 `OnGenerate` 的 `InitRenderParams` switch 全分支 `make_unique` 创建、
     无置空路径（rs_render_node_drawable_adapter.cpp:158-184）。
- 残留风险：:366 条件（只判 `HasEndNodeRect()`）与 :384（`HasEndNodeRect() && !isStartEndNodeSame_`）
  不对称。安全性依赖"IsStartEndSameNode 必须晚于 CreateResources"的隐式跨函数时序，
  若调用顺序被调整（把 :177 前移），同节点场景将变成空指针崩溃。
- 建议（加固，非必改）：:366 条件对齐 :384，或补 `endNodeDrawable_ != nullptr` 防御。
- 追问记录：用户后续追问"endNodeDrawable_ 为什么会是空"，重新核对执行顺序后确认
  首轮分析中"同节点场景可达空指针"的路径不成立，最终定性以本节为准。

## 问题二：RSSurfaceCaptureTaskParallel::CreateResources 中 surfaceNodeParams（rs_surface_capture_task_parallel.cpp:237-241）

```cpp
auto surfaceNodeParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable_->GetRenderParams().get());
bool isF16Capture = (captureConfig_.needF16WindowCaptureForScRGB && RSHdrUtil::NeedUseF16Capture(curNode)) ||
    (captureConfig_.isHdrCapture && surfaceNodeParams->SelfOrChildHasHDR());
```

- 疑问：直接对 `surfaceNodeParams` 解引用是否有空指针风险。
- 结论：**没有可达风险，非缺陷**。与问题一同源（同一构造不变量）。
  依据：
  1. `surfaceNodeDrawable_` 已判空（:234-236），null 直接 `return false`。
  2. `GetRenderParams().get()` 保证非空：`renderParams_` 在 `OnGenerate → InitRenderParams`
     switch 全分支 `make_unique` 创建（含 default 分支），此后无置空/重赋值路径；
     drawable 存活 ⇒ params 非空。缓存命中路径条目也是初始化完成后才发布，
     且本函数在主线程执行，无跨线程竞争。
  3. `static_cast<RSSurfaceRenderParams*>` 类型正确：`curNode` 为 `RSSurfaceRenderNode`
     （含 leash window 父节点，SURFACE_NODE；protective solid 变体同样映射
     `RSSurfaceRenderParams`，adapter.cpp:177-178），向下转型合法。
- 备注：同文件 `Run()` 中同模式代码有防御性判空（:287-294 `curNodeParams == nullptr`），
  `CreateResources` 没有，属防御层次不统一，非缺陷。不修改。

## 问题三：RSClientToServiceConnection::ShowWatermark 中 watermarkImg（rs_client_to_service_connection.cpp:1725-1743）

- 疑问：`watermarkImg` 是否缺少判空。
- 结论：**不缺，非缺陷**。链路为"入口校验 + 出口自防"双层结构，本函数只是转发节点，
  函数体内无任何解引用 `watermarkImg` 的语句（仅透传给 persistence data 与各 conn）。
  依据：
  1. 入口一（render_server 侧 stub，rs_client_to_service_connection_stub.cpp:2337-2349）：
     `ReadParcelable` 后立即 `if (!watermarkImg) { ret = ERR_INVALID_DATA; break; }`，空值到不了本函数。
  2. 入口二（render 进程侧 stub，rs_service_to_render_connection_stub.cpp:448-460）：同样判空拦截。
  3. 持久化路径：`RegisterWithoutCallingPid` 只在入口判空之后执行，存入的 `watermarkImg` 必非空；
     恢复路径 `ShowWatermarkPersistenceData::Unmarshalling`（rs_ipc_persistence_data.cpp:99-108）
     读失败直接返回 nullptr，不会被 Apply。
  4. 最终消费者自防：`RSMainThread::ShowWatermark`（rs_main_thread.cpp:5505）使用处为
     `if (flag && watermarkImg)`——即使未来新增入口漏判，最后关口也兜住。

## 问题四：RSTransactionData::UnmarshallingCommand 的 payloadLock 与 max_size()（rs_transaction_data.cpp:395-441）

```cpp
size_t readableSize = parcel.GetReadableBytes() / minCommandSize;     // :387 未持锁
size_t len = static_cast<size_t>(commandSize);                        // :388 commandSize 可为负
if (len > readableSize || len > payload_.max_size()) { return false; } // :389 未持锁
...
std::unique_lock<std::mutex> payloadLock(commandMutex_, std::defer_lock);  // :395
for (...) {
    ...
    payloadLock.lock();            // :427
    payload_.emplace_back(...);
    payloadLock.unlock();          // :437
}
```

- 疑问 1：payloadLock 手动 lock/unlock 是否存在死锁。
- 疑问 2：`emplace_back` 抛 `bad_alloc` 时 unlock 不执行、defer_lock 构造导致析构调
  `release()`，`commandMutex_` 是否永久锁死。
- 疑问 3：:389 `payload_.max_size()` 在未持 `commandMutex_` 的情况下调用，是否有多线程问题。
- 结论：**三项均无风险，非缺陷**。
  依据：
  1. 死锁四成因逐一排除：锁窗口（:427-437）内无重入 commandMutex_ 的调用（入口 :363 的
     `Clear()` 在 :395 defer_lock 构造之前已释放）；窗口内只有 hilog/hitrace 叶子锁，无 ABBA
     环；lock/unlock 之间无 return/continue，OHOS 禁异常、emplace_back 无抛出路径，配对完整；
     对象在 `Unmarshalling`（:49-52）中 `new` 出来、unmarshal 完成前不发布给任何线程，
     跨线程争用不可达（分段并行 unmarshal 同样各自 new 独立对象）。
  2. defer_lock 疑问已用可运行程序验证（/tmp/opencode/unique_lock_defer_test.cpp，g++ -std=c++17）：
     抛异常与提前 return 两种场景锁均正确 RELEASED。机制：defer_lock 只影响构造函数，
     `~unique_lock()` 析构只看 `owns` 标志（`lock()` 已置位）→ 调用 `unlock()`；
     `release()` 是需显式调用的成员，析构从不调用。unique_lock 的核心价值正是
     "只要持有，析构必解锁，异常安全"。
  3. max_size() 多线程：:389 确实未持锁（payloadLock :395 才构造），但同上"对象未发布"
     不变量覆盖此行——unmarshal 期间无第二线程可触达该对象，不存在并发读写；
     且 `vector::max_size()` 本身是 const、无分配、与容器内容无关的纯查询，空容器调用亦合法。
     另外 max_size 检查实际冗余：`readableSize` 受 PARCEL_MAX_CAPACITY（4MB）/ 最小命令尺寸
     （5 或 14 字节）约束，上限约 80 万，而 64 位下 max_size() ≈ 7.7e17，
     `len > readableSize` 永远先触发；commandSize 为负时 `static_cast<size_t>` 产生天文数字，
     同样被第一条件拦截。
- 残留（理论性质）：若工程将来放开异常，`emplace_back` 抛出会跳过 `Unmarshalling` 的
  `delete transactionData`（:64）造成对象泄漏——但锁仍正确释放，与死锁无关；
  OHOS 默认 -fno-exceptions 下分配失败直接 abort，泄漏也不存在。
- 处置：不修改。可选改进（纯可读性）：手动 lock/unlock 改为 lock_guard 作用域块；
  该锁在当前调用图下实际空转（无争用方），每轮 lock/unlock 仅固定开销。


## 两问题的共同不变量（供后续检视复用）

"drawable 经 `OnGenerate` 创建且已判空 ⇒ `GetRenderParams()` 非空"是 render_service_base
drawable 体系的构造不变量：`InitRenderParams` 无条件 `make_unique`，全仓无 `renderParams_ =
nullptr` 赋值。凡满足"drawable 已判空"前提的调用点，`GetRenderParams()` 无需再判；
该结论适用于问题 1（第一批）、问题 3（第一批）及本批问题二。