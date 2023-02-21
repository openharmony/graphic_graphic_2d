/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "pipeline/rs_main_thread.h"

#include <SkGraphics.h>
#include <securec.h>
#include "rs_trace.h"

#include "animation/rs_animation_fraction.h"
#include "command/rs_message_processor.h"
#include "delegate/rs_functional_delegate.h"
#include "memory/MemoryManager.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_cold_start_thread.h"
#include "pipeline/rs_divided_render_util.h"
#include "pipeline/rs_render_engine.h"
#include "pipeline/rs_render_service_visitor.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_hardware_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_unmarshal_thread.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "pipeline/rs_occlusion_config.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_innovation.h"
#include "platform/drawing/rs_vsync_client.h"
#include "property/rs_property_trace.h"
#include "property/rs_properties_painter.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_transaction_proxy.h"
#include "accessibility_config.h"
#include "rs_qos_thread.h"
#include "xcollie/watchdog.h"

#include "frame_trace.h"

#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
#include "pipeline/driven_render/rs_driven_render_manager.h"
#endif

using namespace FRAME_TRACE;
static const std::string RS_INTERVAL_NAME = "renderservice";

using namespace OHOS::AccessibilityConfig;
namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t RUQUEST_VSYNC_NUMBER_LIMIT = 10;
constexpr uint64_t REFRESH_PERIOD = 16666667;
constexpr int32_t PERF_ANIMATION_REQUESTED_CODE = 10017;
constexpr int32_t PERF_MULTI_WINDOW_REQUESTED_CODE = 10026;
constexpr uint64_t PERF_PERIOD = 250000000;
constexpr uint64_t CLEAN_CACHE_FREQ = 60;
constexpr uint64_t SKIP_COMMAND_FREQ_LIMIT = 30;
constexpr uint64_t PERF_PERIOD_BLUR = 80000000;
constexpr uint64_t PERF_PERIOD_MULTI_WINDOW = 80000000;
constexpr uint32_t MULTI_WINDOW_PERF_START_NUM = 2;
constexpr uint32_t MULTI_WINDOW_PERF_END_NUM = 4;
const std::map<int, int32_t> BLUR_CNT_TO_BLUR_CODE {
    { 1, 10021 },
    { 2, 10022 },
    { 3, 10023 },
};

bool Compare(const std::unique_ptr<RSTransactionData>& data1, const std::unique_ptr<RSTransactionData>& data2)
{
    if (!data1 || !data2) {
        RS_LOGW("Compare RSTransactionData: nullptr!");
        return true;
    }
    return data1->GetIndex() < data2->GetIndex();
}

void InsertToEnd(std::vector<std::unique_ptr<RSTransactionData>>& source,
    std::vector<std::unique_ptr<RSTransactionData>>& target)
{
    target.insert(target.end(), std::make_move_iterator(source.begin()), std::make_move_iterator(source.end()));
    source.clear();
}

void PerfRequest(int32_t perfRequestCode, bool onOffTag)
{
#ifdef SOC_PERF_ENABLE
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(perfRequestCode, onOffTag, "");
    RS_LOGD("RSMainThread::soc perf info [%d %d]", perfRequestCode, onOffTag);
#endif
}
}

class AccessibilityObserver : public AccessibilityConfigObserver {
public:
    AccessibilityObserver() = default;
    void OnConfigChanged(const CONFIG_ID id, const ConfigValue &value) override
    {
        RS_LOGD("AccessibilityObserver OnConfigChanged");
        ColorFilterMode mode = ColorFilterMode::COLOR_FILTER_END;
        if (id == CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER) {
            switch (value.daltonizationColorFilter) {
                case Protanomaly:
                    mode = ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE;
                    break;
                case Deuteranomaly:
                    mode = ColorFilterMode::DALTONIZATION_DEUTERANOMALY_MODE;
                    break;
                case Tritanomaly:
                    mode = ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE;
                    break;
                case Normal:
                    mode = ColorFilterMode::DALTONIZATION_NORMAL_MODE;
                    break;
                default:
                    break;
            }
            RSBaseRenderEngine::SetColorFilterMode(mode);
        } else if (id == CONFIG_ID::CONFIG_INVERT_COLOR) {
            mode = value.invertColor ? ColorFilterMode::INVERT_COLOR_ENABLE_MODE :
                                        ColorFilterMode::INVERT_COLOR_DISABLE_MODE;
            RSBaseRenderEngine::SetColorFilterMode(mode);
        } else {
            RSBaseRenderEngine::SetHighContrast(value.highContrastText);
        }
    }
};

RSMainThread* RSMainThread::Instance()
{
    static RSMainThread instance;
    RSAnimationFraction::Init();
    return &instance;
}

RSMainThread::RSMainThread() : mainThreadId_(std::this_thread::get_id())
{
    context_ = std::make_shared<RSContext>();
}

RSMainThread::~RSMainThread() noexcept
{
    RemoveRSEventDetector();
    RSInnovation::CloseInnovationSo();
}

void RSMainThread::Init()
{
    mainLoop_ = [&]() {
        RS_LOGD("RsDebug mainLoop start");
        PerfMultiWindow();
        if (RSSystemProperties::FrameTraceEnabled()) {
            QuickStartFrameTrace(RS_INTERVAL_NAME);
        }
        SetRSEventDetectorLoopStartTag();
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSMainThread::DoComposition");
        activeProcessPids_.clear();
        ConsumeAndUpdateAllNodes();
        WaitUntilUnmarshallingTaskFinished();
        ProcessCommand();
        Animate(timestamp_);
        CollectInfoForHardwareComposer();
        CheckColdStartMap();
        Render();
        ReleaseAllNodesBuffer();
        SendCommands();
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        SetRSEventDetectorLoopFinishTag();
        rsEventManager_.UpdateParam();
        RS_LOGD("RsDebug mainLoop end");
    };

    if (isUniRender_) {
        unmarshalBarrierTask_ = [this]() {
            auto cachedTransactionData = RSUnmarshalThread::Instance().GetCachedTransactionData();
            MergeToEffectiveTransactionDataMap(cachedTransactionData);
            {
                std::lock_guard<std::mutex> lock(unmarshalMutex_);
                ++unmarshalFinishedCount_;
            }
            unmarshalTaskCond_.notify_all();
        };
        RSUnmarshalThread::Instance().Start();
    }

    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread("RenderService", handler_);
    if (ret != 0) {
        RS_LOGW("Add watchdog thread failed");
    }
    InitRSEventDetector();
    sptr<VSyncConnection> conn = new VSyncConnection(rsVSyncDistributor_, "rs");
    rsVSyncDistributor_->AddConnection(conn);
    receiver_ = std::make_shared<VSyncReceiver>(conn, handler_);
    receiver_->Init();
    if (isUniRender_) {
        uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
        uniRenderEngine_->Init();
    } else {
        renderEngine_ = std::make_shared<RSRenderEngine>();
        renderEngine_->Init();
    }
#ifdef RS_ENABLE_GL
    int cacheLimitsTimes = 2; // double skia Resource Cache Limits
    auto grContext = isUniRender_? uniRenderEngine_->GetRenderContext()->GetGrContext() :
        renderEngine_->GetRenderContext()->GetGrContext();
    int maxResources = 0;
    size_t maxResourcesSize = 0;
    grContext->getResourceCacheLimits(&maxResources, &maxResourcesSize);
    grContext->setResourceCacheLimits(cacheLimitsTimes * maxResources, cacheLimitsTimes * maxResourcesSize);
#endif
    RSInnovation::OpenInnovationSo();
    Occlusion::Region::InitDynamicLibraryFunction();
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    RSDrivenRenderManager::InitInstance();
#endif

    accessibilityObserver_ = std::make_shared<AccessibilityObserver>();
    auto &config = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    config.InitializeContext();
    config.SubscribeConfigObserver(CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER, accessibilityObserver_);
    config.SubscribeConfigObserver(CONFIG_ID::CONFIG_INVERT_COLOR, accessibilityObserver_);
    if (isUniRender_) {
        config.SubscribeConfigObserver(CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT, accessibilityObserver_);
    }

    auto delegate = RSFunctionalDelegate::Create();
    delegate->SetRepaintCallback([]() { RSMainThread::Instance()->RequestNextVSync(); });
    RSOverdrawController::GetInstance().SetDelegate(delegate);
}

void RSMainThread::RsEventParamDump(std::string& dumpString)
{
    rsEventManager_.DumpAllEventParam(dumpString);
}

void RSMainThread::RemoveRSEventDetector()
{
    if (rsCompositionTimeoutDetector_ != nullptr) {
        rsEventManager_.RemoveEvent(rsCompositionTimeoutDetector_->GetStringId());
    }
}

void RSMainThread::InitRSEventDetector()
{
    // default Threshold value of Timeout Event: 100ms
    rsCompositionTimeoutDetector_ = RSBaseEventDetector::CreateRSTimeOutDetector(100, "RS_COMPOSITION_TIMEOUT");
    if (rsCompositionTimeoutDetector_ != nullptr) {
        rsEventManager_.AddEvent(rsCompositionTimeoutDetector_, 60000); // report Internal 1min:60s：60000ms
        RS_LOGD("InitRSEventDetector finish");
    }
}

void RSMainThread::SetRSEventDetectorLoopStartTag()
{
    if (rsCompositionTimeoutDetector_ != nullptr) {
        rsCompositionTimeoutDetector_->SetLoopStartTag();
    }
}

void RSMainThread::SetRSEventDetectorLoopFinishTag()
{
    if (rsCompositionTimeoutDetector_ != nullptr) {
        if (isUniRender_) {
            rsCompositionTimeoutDetector_->SetLoopFinishTag(
                focusAppPid_, focusAppUid_, focusAppBundleName_, focusAppAbilityName_);
        } else {
            std::string defaultFocusAppInfo = "";
            rsCompositionTimeoutDetector_->SetLoopFinishTag(
                -1, -1, defaultFocusAppInfo, defaultFocusAppInfo);
        }
    }
}

void RSMainThread::SetFocusAppInfo(
    int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName)
{
    focusAppPid_ = pid;
    focusAppUid_ = uid;
    focusAppBundleName_ = bundleName;
    focusAppAbilityName_ = abilityName;
}

void RSMainThread::Start()
{
    if (runner_) {
        runner_->Run();
    }
}

void RSMainThread::ProcessCommand()
{
    // To improve overall responsiveness, we make animations start on LAST frame instead of THIS frame.
    // If last frame is too far away (earlier than 1 vsync from now), we use currentTimestamp_ - REFRESH_PERIOD as
    // 'virtual' last frame timestamp.
    if (timestamp_ - lastAnimateTimestamp_ > REFRESH_PERIOD) { // if last frame is earlier than 1 vsync from now
        context_->currentTimestamp_ = timestamp_ - REFRESH_PERIOD;
    } else {
        context_->currentTimestamp_ = lastAnimateTimestamp_;
    }
    if (isUniRender_) {
        ProcessCommandForUniRender();
    } else {
        ProcessCommandForDividedRender();
    }
}

void RSMainThread::ProcessCommandForUniRender()
{
    ResetHardwareEnabledState();
    TransactionDataMap transactionDataEffective;
    std::string transactionFlags;
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);

        for (auto& elem: effectiveTransactionDataIndexMap_) {
            auto& transactionVec = elem.second.second;
            std::sort(transactionVec.begin(), transactionVec.end(), Compare);
        }

        for (auto& rsTransactionElem: effectiveTransactionDataIndexMap_) {
            auto pid = rsTransactionElem.first;
            auto& lastIndex = rsTransactionElem.second.first;
            auto& transactionVec = rsTransactionElem.second.second;
            auto iter = transactionVec.begin();
            for (; iter != transactionVec.end(); ++iter) {
                if ((*iter) == nullptr) {
                    continue;
                }
                auto curIndex = (*iter)->GetIndex();
                if (curIndex == lastIndex + 1) {
                    ++lastIndex;
                    transactionFlags += ", [" + std::to_string(pid) + ", " + std::to_string(curIndex) + "]";
                } else {
                    RS_LOGE("RSMainThread::ProcessCommandForUniRender wait curIndex:%llu, lastIndex:%llu, pid:%d",
                        curIndex, lastIndex, pid);
                    if (transactionDataLastWaitTime_[pid] == 0) {
                        transactionDataLastWaitTime_[pid] = timestamp_;
                    }
                    if ((timestamp_ - transactionDataLastWaitTime_[pid]) / REFRESH_PERIOD > SKIP_COMMAND_FREQ_LIMIT) {
                        transactionDataLastWaitTime_[pid] = 0;
                        lastIndex = curIndex;
                        transactionFlags += ", skip to[" + std::to_string(pid) + ", " + std::to_string(curIndex) + "]";
                        RS_LOGE("RSMainThread::ProcessCommandForUniRender skip to index:%llu, pid:%d", curIndex, pid);
                        continue;
                    }
                    break;
                }
            }
            if (iter != transactionVec.begin()) {
                transactionDataEffective[pid].insert(transactionDataEffective[pid].end(),
                    std::make_move_iterator(transactionVec.begin()), std::make_move_iterator(iter));
                transactionVec.erase(transactionVec.begin(), iter);
            }
        }
    }
    if (!transactionDataEffective.empty()) {
        doDirectComposition_ = false;
    }
    RS_TRACE_NAME("RSMainThread::ProcessCommandUni" + transactionFlags);
    for (auto& rsTransactionElem: transactionDataEffective) {
        for (auto& rsTransaction: rsTransactionElem.second) {
            if (rsTransaction) {
                context_->transactionTimestamp_ = rsTransaction->GetTimestamp();
                rsTransaction->Process(*context_);
                activeProcessPids_.emplace(rsTransactionElem.first);
            }
        }
    }
}

void RSMainThread::ProcessCommandForDividedRender()
{
    const auto& nodeMap = context_->GetNodeMap();
    RS_TRACE_BEGIN("RSMainThread::ProcessCommand");
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        if (!pendingEffectiveCommands_.empty()) {
            effectiveCommands_.swap(pendingEffectiveCommands_);
        }
        for (auto& [surfaceNodeId, commandMap] : cachedCommands_) {
            auto node = nodeMap.GetRenderNode<RSSurfaceRenderNode>(surfaceNodeId);
            auto bufferTimestamp = bufferTimestamps_.find(surfaceNodeId);
            std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>>::iterator effectIter;

            if (!node || !node->IsOnTheTree() || bufferTimestamp == bufferTimestamps_.end()) {
                // If node has been destructed or is not on the tree or has no valid buffer,
                // for all command cached in commandMap should be executed immediately
                effectIter = commandMap.end();
            } else {
                uint64_t timestamp = bufferTimestamp->second;
                effectIter = commandMap.upper_bound(timestamp);
            }

            for (auto it = commandMap.begin(); it != effectIter; it++) {
                effectiveCommands_[it->first].insert(effectiveCommands_[it->first].end(),
                    std::make_move_iterator(it->second.begin()), std::make_move_iterator(it->second.end()));
            }
            commandMap.erase(commandMap.begin(), effectIter);
        }
    }
    for (auto& [timestamp, commands] : effectiveCommands_) {
        context_->transactionTimestamp_ = timestamp;
        for (auto& command : commands) {
            if (command) {
                command->Process(*context_);
            }
        }
    }
    effectiveCommands_.clear();
    RS_TRACE_END();
}

void RSMainThread::ConsumeAndUpdateAllNodes()
{
    RS_TRACE_NAME("RSMainThread::ConsumeAndUpdateAllNodes");
    bool needRequestNextVsync = false;
    bufferTimestamps_.clear();
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes(
        [this, &needRequestNextVsync](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
        if (surfaceNode == nullptr) {
            return;
        }
        auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*surfaceNode);
        surfaceHandler.ResetCurrentFrameBufferConsumed();
        if (RSBaseRenderUtil::ConsumeAndUpdateBuffer(surfaceHandler)) {
            this->bufferTimestamps_[surfaceNode->GetId()] = static_cast<uint64_t>(surfaceNode->GetTimestamp());
            if (surfaceNode->UpdateDirtyIfFrameBufferConsumed()) {
                // collect surface view's pid to prevent wrong skip
                activeProcessPids_.emplace(ExtractPid(surfaceNode->GetId()));
            }
        }

        // still have buffer(s) to consume.
        if (surfaceHandler.GetAvailableBufferCount() > 0) {
            needRequestNextVsync = true;
        }
    });

    if (needRequestNextVsync) {
        RequestNextVSync();
    }
}

void RSMainThread::CollectInfoForHardwareComposer()
{
    if (!isUniRender_ || !RSSystemProperties::GetHardwareComposerEnabled()) {
        return;
    }
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes(
        [this](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
            if (surfaceNode == nullptr || !surfaceNode->IsOnTheTree()) {
                return;
            }
            if (surfaceNode->IsAppWindow()) {
                const auto& property = surfaceNode->GetRenderProperties();
                if (property.NeedFilter() || property.IsShadowValid()) {
                    isHardwareForcedDisabled_ = true;
                    return;
                }
            }
            auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*surfaceNode);
            if (surfaceHandler.IsCurrentFrameBufferConsumed()) {
                if (!surfaceNode->IsHardwareEnabledType() ||
                    surfaceNode->GetSrcRect().IsEmpty() || surfaceNode->GetDstRect().IsEmpty()) {
                    doDirectComposition_ = false;
                } else {
                    isHardwareEnabledBufferUpdated_ = true;
                }
            }

            if (surfaceNode->IsHardwareEnabledType() && surfaceNode->GetBuffer() != nullptr) {
                hardwareEnabledNodes_.emplace_back(surfaceNode);
                if (!surfaceNode->IsLastFrameHardwareEnabled()) {
                    doDirectComposition_ = false;
                }
            }
        });

    if (doWindowAnimate_ || isHardwareForcedDisabled_) {
        // setDirty for surfaceView if last frame is hardware enabled
        for (auto& surfaceNode : hardwareEnabledNodes_) {
            if (surfaceNode->IsLastFrameHardwareEnabled()) {
                surfaceNode->SetDirty();
                activeProcessPids_.emplace(ExtractPid(surfaceNode->GetId()));
            }
        }
    }
}

void RSMainThread::ReleaseAllNodesBuffer()
{
    RS_TRACE_NAME("RSMainThread::ReleaseAllNodesBuffer");
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
        if (surfaceNode == nullptr) {
            return;
        }
        // surfaceNode's buffer will be released in hardware thread if last frame enables hardware composer
        if (surfaceNode->IsHardwareEnabledType()) {
            surfaceNode->ResetCurrentFrameHardwareEnabledState();
            if (surfaceNode->IsLastFrameHardwareEnabled()) {
                return;
            }
        }
        // To avoid traverse surfaceNodeMap again, destroy cold start thread here
        if ((!surfaceNode->IsOnTheTree() || !surfaceNode->ShouldPaint()) &&
            RSColdStartManager::Instance().IsColdStartThreadRunning(surfaceNode->GetId())) {
            surfaceNode->ClearCachedImage();
            RSColdStartManager::Instance().StopColdStartThread(surfaceNode->GetId());
        }
        RSBaseRenderUtil::ReleaseBuffer(static_cast<RSSurfaceHandler&>(*surfaceNode));
    });
}

void RSMainThread::WaitUtilUniRenderFinished()
{
    std::unique_lock<std::mutex> lock(uniRenderMutex_);
    if (uniRenderFinished_) {
        return;
    }
    uniRenderCond_.wait(lock, [this]() { return uniRenderFinished_; });
    uniRenderFinished_ = false;
}

void RSMainThread::WaitUntilDisplayNodeBufferReleased(RSDisplayRenderNode& node)
{
    std::unique_lock<std::mutex> lock(displayNodeBufferReleasedMutex_);
    displayNodeBufferReleased_ = false; // prevent spurious wakeup of condition variable
    if (node.GetConsumer()->QueryIfBufferAvailable()) {
        return;
    }
    displayNodeBufferReleasedCond_.wait(lock, [this]() { return displayNodeBufferReleased_; });
}

void RSMainThread::WaitUtilDrivenRenderFinished()
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    std::unique_lock<std::mutex> lock(drivenRenderMutex_);
    if (drivenRenderFinished_) {
        return;
    }
    drivenRenderCond_.wait(lock, [this]() { return drivenRenderFinished_; });
    drivenRenderFinished_ = false;
#endif
}

void RSMainThread::WaitUntilUnmarshallingTaskFinished()
{
    if (!isUniRender_) {
        return;
    }
    RS_TRACE_NAME("RSMainThread::WaitUntilUnmarshallingTaskFinished");
    std::unique_lock<std::mutex> lock(unmarshalMutex_);
    unmarshalTaskCond_.wait(lock, [this]() { return unmarshalFinishedCount_ > 0; });
    --unmarshalFinishedCount_;
}

void RSMainThread::MergeToEffectiveTransactionDataMap(TransactionDataMap& cachedTransactionDataMap)
{
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    for (auto& elem : cachedTransactionDataMap) {
        auto pid = elem.first;
        if (effectiveTransactionDataIndexMap_.count(pid) == 0) {
            RS_LOGE("RSMainThread::MergeToEffectiveTransactionDataMap pid:%d not valid, skip it", pid);
            continue;
        }
        InsertToEnd(elem.second, effectiveTransactionDataIndexMap_[pid].second);
    }
    cachedTransactionDataMap.clear();
}

void RSMainThread::NotifyUniRenderFinish()
{
    if (std::this_thread::get_id() != Id()) {
        std::lock_guard<std::mutex> lock(uniRenderMutex_);
        uniRenderFinished_ = true;
        uniRenderCond_.notify_one();
    } else {
        uniRenderFinished_ = true;
    }
}

void RSMainThread::NotifyDisplayNodeBufferReleased()
{
    RS_TRACE_NAME("RSMainThread::NotifyDisplayNodeBufferReleased");
    std::lock_guard<std::mutex> lock(displayNodeBufferReleasedMutex_);
    displayNodeBufferReleased_ = true;
    displayNodeBufferReleasedCond_.notify_one();
}

void RSMainThread::NotifyDrivenRenderFinish()
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    if (std::this_thread::get_id() != Id()) {
        std::lock_guard<std::mutex> lock(drivenRenderMutex_);
        drivenRenderFinished_ = true;
        drivenRenderCond_.notify_one();
    } else {
        drivenRenderFinished_ = true;
    }
#endif
}

void RSMainThread::Render()
{
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        RS_LOGE("RSMainThread::Render GetGlobalRootRenderNode fail");
        return;
    }
    if (RSSystemProperties::GetRenderNodeTraceEnabled()) {
        RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    }
    RS_LOGD("RSMainThread::Render isUni:%d", isUniRender_);

    if (isUniRender_) {
        auto uniVisitor = std::make_shared<RSUniRenderVisitor>();
        uniVisitor->SetHardwareEnabledNodes(hardwareEnabledNodes_);
        if (isHardwareForcedDisabled_ || rootNode->GetChildrenCount() > 1) {
            // [PLANNING] GetChildrenCount > 1 indicates multi display, only Mirror Mode need be marked here
            // Mirror Mode reuses display node's buffer, so mark it and disable hardware composer in this case
            uniVisitor->MarkHardwareForcedDisabled();
            doDirectComposition_ = false;
        }
        bool needTraverseNodeTree = true;
        doDirectComposition_ = false;
        if (doDirectComposition_ && !isDirty_) {
            if (isHardwareEnabledBufferUpdated_) {
                needTraverseNodeTree = !uniVisitor->DoDirectComposition(rootNode);
            } else {
                RS_LOGI("RSMainThread::Render nothing to update");
                return;
            }
        }
        if (needTraverseNodeTree) {
            uniVisitor->SetAnimateState(doWindowAnimate_);
            uniVisitor->SetDirtyFlag(isDirty_);
            uniVisitor->SetFocusedWindowPid(focusAppPid_);
            rootNode->Prepare(uniVisitor);
            CalcOcclusion();
            rootNode->Process(uniVisitor);
        }
        isDirty_ = false;
        uniRenderEngine_->ShrinkCachesIfNeeded();
    } else {
        auto rsVisitor = std::make_shared<RSRenderServiceVisitor>();
        rsVisitor->SetAnimateState(doWindowAnimate_);
        rootNode->Prepare(rsVisitor);
        CalcOcclusion();

        bool doParallelComposition = false;
        if (!rsVisitor->ShouldForceSerial() && RSInnovation::GetParallelCompositionEnabled()) {
            doParallelComposition = DoParallelComposition(rootNode);
        }
        if (doParallelComposition) {
            renderEngine_->ShrinkCachesIfNeeded();
            return;
        }
        rootNode->Process(rsVisitor);
        renderEngine_->ShrinkCachesIfNeeded();
    }

    PerfForBlurIfNeeded();
}

bool RSMainThread::CheckSurfaceNeedProcess(OcclusionRectISet& occlusionSurfaces, std::shared_ptr<RSSurfaceRenderNode> curSurface)
{
    bool needProcess = false;
    if (curSurface->IsFocusedWindow(focusAppPid_)) {
        needProcess = true;
        if (!curSurface->HasContainerWindow() && !curSurface->IsTransparent()) {
            occlusionSurfaces.insert(curSurface->GetDstRect());
        }
    } else {
        size_t beforeSize = occlusionSurfaces.size();
        occlusionSurfaces.insert(curSurface->GetDstRect());
        bool insertSuccess = occlusionSurfaces.size() > beforeSize ? true : false;
        if (insertSuccess) {
            needProcess = true;
            if (curSurface->IsTransparent()) {
                auto iter = std::find_if(occlusionSurfaces.begin(), occlusionSurfaces.end(),
                    [&curSurface](RectI r) -> bool {return r == curSurface->GetDstRect();});
                if (iter != occlusionSurfaces.end()) {
                    occlusionSurfaces.erase(iter);
                }
            }
        }
    }
    return needProcess;
}

void RSMainThread::CalcOcclusionImplementation(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    Occlusion::Region accumulatedRegion;
    VisibleData curVisVec;
    OcclusionRectISet occlusionSurfaces;
    std::map<uint32_t, bool> pidVisMap;
    for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
        auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (curSurface == nullptr || curSurface->GetDstRect().IsEmpty()) {
            continue;
        }
        Occlusion::Rect occlusionRect;
        if (isUniRender_) {
            // In UniReder, CalcOcclusion should consider the shadow area of window
            occlusionRect = Occlusion::Rect {curSurface->GetOldDirtyInSurface()};
        } else {
            occlusionRect = Occlusion::Rect {curSurface->GetDstRect()};
        }
        curSurface->setQosCal(qosPidCal_);
        if (CheckSurfaceNeedProcess(occlusionSurfaces, curSurface)) {
            Occlusion::Region curRegion { occlusionRect };
            Occlusion::Region subResult = curRegion.Sub(accumulatedRegion);
            curSurface->SetVisibleRegionRecursive(subResult, curVisVec, pidVisMap);
            // when surface is in starting window stage, do not occlude other window surfaces
            // fix grey block when directly open app (i.e. setting) from notification center
            auto parentPtr = curSurface->GetParent().lock();
            if (parentPtr != nullptr && parentPtr->IsInstanceOf<RSSurfaceRenderNode>()) {
                auto surfaceParentPtr = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parentPtr);
                if (surfaceParentPtr->GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE &&
                    !curSurface->IsNotifyUIBufferAvailable()) {
                    continue;
                }
            }
            if (isUniRender_) {
                accumulatedRegion.OrSelf(curSurface->GetOpaqueRegion());
            } else {
                bool diff = (curSurface->GetDstRect().width_ > curSurface->GetBuffer()->GetWidth() ||
                            curSurface->GetDstRect().height_ > curSurface->GetBuffer()->GetHeight()) &&
                            curSurface->GetRenderProperties().GetFrameGravity() != Gravity::RESIZE &&
                            ROSEN_EQ(curSurface->GetGlobalAlpha(), 1.0f);
                if (!curSurface->IsTransparent() && !diff) {
                    accumulatedRegion.OrSelf(curRegion);
                }
            }
        } else {
            curSurface->SetVisibleRegionRecursive({}, curVisVec, pidVisMap);
        }
    }
    // Callback to WMS and QOS
    CallbackToWMS(curVisVec);
    CallbackToQOS(pidVisMap);
}

void RSMainThread::CalcOcclusion()
{
    RS_TRACE_NAME("RSMainThread::CalcOcclusion");
    if (doWindowAnimate_ && !isUniRender_) {
        return;
    }
    const std::shared_ptr<RSBaseRenderNode> node = context_->GetGlobalRootRenderNode();
    if (node == nullptr) {
        RS_LOGE("RSMainThread::CalcOcclusion GetGlobalRootRenderNode fail");
        return;
    }
    RSInnovation::UpdateOcclusionCullingSoEnabled();
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    if (node->GetSortedChildren().size() == 1) {
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(
            node->GetSortedChildren().front());
        if (displayNode) {
            curAllSurfaces = displayNode->GetCurAllSurfaces();
        }
    } else {
        node->CollectSurface(node, curAllSurfaces, isUniRender_);
    }
    // Judge whether it is dirty
    // Surface cnt changed or surface DstRectChanged or surface ZorderChanged
    bool winDirty = (lastSurfaceCnt_ != curAllSurfaces.size() || isDirty_ ||
        lastFocusAppPid_ != focusAppPid_);
    lastSurfaceCnt_ = curAllSurfaces.size();
    lastFocusAppPid_ = focusAppPid_;
    if (!winDirty) {
        for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
            auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
            if (surface == nullptr) {
                continue;
            }
            if (surface->GetZorderChanged() || surface->GetDstRectChanged() ||
                surface->IsOpaqueRegionChanged() ||
                surface->GetAlphaChanged() || (isUniRender_ && surface->IsDirtyRegionUpdated())) {
                winDirty = true;
            }
            surface->CleanDstRectChanged();
            surface->CleanAlphaChanged();
        }
    }
    if (!winDirty) {
        return;
    }
    CalcOcclusionImplementation(curAllSurfaces);
}

bool RSMainThread::CheckQosVisChanged(std::map<uint32_t, bool>& pidVisMap)
{
    bool isVisibleChanged = pidVisMap.size() != lastPidVisMap_.size();
    if (!isVisibleChanged) {
        auto iterCur = pidVisMap.begin();
        auto iterLast = lastPidVisMap_.begin();
        for (; iterCur != pidVisMap.end(); iterCur++, iterLast++) {
            if (iterCur->first != iterLast->first ||
                iterCur->second != iterLast->second) {
                isVisibleChanged = true;
                break;
            }
        }
    }

    lastPidVisMap_.clear();
    lastPidVisMap_.insert(pidVisMap.begin(), pidVisMap.end());
    return isVisibleChanged;
}

void RSMainThread::CallbackToQOS(std::map<uint32_t, bool>& pidVisMap)
{
    if (!RSInnovation::UpdateQosVsyncEnabled()) {
        if (qosPidCal_) {
            qosPidCal_ = false;
            RSQosThread::ResetQosPid();
            RSQosThread::GetInstance()->SetQosCal(qosPidCal_);
        }
        return;
    }
    qosPidCal_ = true;
    RSQosThread::GetInstance()->SetQosCal(qosPidCal_);
    if (!CheckQosVisChanged(pidVisMap)) {
        return;
    }
    RS_TRACE_NAME("RSQosThread::OnRSVisibilityChangeCB");
    RSQosThread::GetInstance()->OnRSVisibilityChangeCB(pidVisMap);
}

void RSMainThread::CallbackToWMS(VisibleData& curVisVec)
{
    // if visible surfaces changed callback to WMS：
    // 1. curVisVec size changed
    // 2. curVisVec content changed
    bool visibleChanged = curVisVec.size() != lastVisVec_.size();
    std::sort(curVisVec.begin(), curVisVec.end());
    if (!visibleChanged) {
        for (uint32_t i = 0; i < curVisVec.size(); i++) {
            if (curVisVec[i] != lastVisVec_[i]) {
                visibleChanged = true;
                break;
            }
        }
    }
    if (visibleChanged) {
        for (auto& listener : occlusionListeners_) {
            listener->OnOcclusionVisibleChanged(std::make_shared<RSOcclusionData>(curVisVec));
        }
    }
    lastVisVec_.clear();
    std::copy(curVisVec.begin(), curVisVec.end(), std::back_inserter(lastVisVec_));
}

void RSMainThread::RequestNextVSync()
{
    RS_TRACE_FUNC();
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = [this](uint64_t timestamp, void* data) { OnVsync(timestamp, data); },
    };
    if (receiver_ != nullptr) {
        requestNextVsyncNum_++;
        if (requestNextVsyncNum_ > RUQUEST_VSYNC_NUMBER_LIMIT) {
            RS_LOGW("RSMainThread::RequestNextVSync too many times:%d", requestNextVsyncNum_);
        }
        receiver_->RequestNextVSync(fcb);
    }
}

void RSMainThread::OnVsync(uint64_t timestamp, void* data)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSMainThread::OnVsync");
    timestamp_ = timestamp;
    requestNextVsyncNum_ = 0;
    if (isUniRender_) {
        MergeToEffectiveTransactionDataMap(cachedTransactionDataMap_);
        RSUnmarshalThread::Instance().PostTask(unmarshalBarrierTask_);
    }
    mainLoop_();
    auto screenManager_ = CreateOrGetScreenManager();
    if (screenManager_ != nullptr) {
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            RSHardwareThread::Instance().PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
        } else {
            PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
        }
    }
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSMainThread::Animate(uint64_t timestamp)
{
    RS_TRACE_FUNC();

    lastAnimateTimestamp_ = timestamp;

    if (context_->animatingNodeList_.empty()) {
        if (doWindowAnimate_ && RSInnovation::UpdateQosVsyncEnabled()) {
            // Preventing Occlusion Calculation from Being Completed in Advance
            RSQosThread::GetInstance()->OnRSVisibilityChangeCB(lastPidVisMap_);
        }
        doWindowAnimate_ = false;
        return;
    }
    doDirectComposition_ = false;
    bool curWinAnim = false;
    bool needRequestNextVsync = false;
    // iterate and animate all animating nodes, remove if animation finished
    EraseIf(context_->animatingNodeList_,
        [this, timestamp, &curWinAnim, &needRequestNextVsync](const auto& iter) -> bool {
        auto node = iter.second.lock();
        if (node == nullptr) {
            RS_LOGD("RSMainThread::Animate removing expired animating node");
            return true;
        }
        activeProcessPids_.emplace(ExtractPid(node->GetId()));
        auto result = node->Animate(timestamp);
        if (!result.first) {
            RS_LOGD("RSMainThread::Animate removing finished animating node %" PRIu64, node->GetId());
        }
        needRequestNextVsync = needRequestNextVsync || result.second;
        if (node->template IsInstanceOf<RSSurfaceRenderNode>()) {
            curWinAnim = true;
        }
        return !result.first;
    });

    if (!doWindowAnimate_ && curWinAnim && RSInnovation::UpdateQosVsyncEnabled()) {
        RSQosThread::ResetQosPid();
    }
    doWindowAnimate_ = curWinAnim;
    RS_LOGD("RSMainThread::Animate end, %d animating nodes remains, has window animation: %d",
        context_->animatingNodeList_.size(), curWinAnim);

    if (needRequestNextVsync) {
        RequestNextVSync();
    }
    PerfAfterAnim();
}

void RSMainThread::CheckColdStartMap()
{
    const auto& nodeMap = GetContext().GetNodeMap();
    RSColdStartManager::Instance().CheckColdStartMap(nodeMap);
}

void RSMainThread::RecvRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    if (!rsTransactionData) {
        return;
    }
    if (rsTransactionData->GetUniRender()) {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        cachedTransactionDataMap_[rsTransactionData->GetSendingPid()].emplace_back(std::move(rsTransactionData));
    } else {
        ClassifyRSTransactionData(rsTransactionData);
    }
    RequestNextVSync();
}

void RSMainThread::ClassifyRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    const auto& nodeMap = context_->GetNodeMap();
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    std::unique_ptr<RSTransactionData> transactionData(std::move(rsTransactionData));
    auto timestamp = transactionData->GetTimestamp();
    RS_LOGD("RSMainThread::RecvRSTransactionData timestamp = %" PRIu64, timestamp);
    for (auto& [nodeId, followType, command] : transactionData->GetPayload()) {
        if (nodeId == 0 || followType == FollowType::NONE) {
            pendingEffectiveCommands_[timestamp].emplace_back(std::move(command));
            continue;
        }
        auto node = nodeMap.GetRenderNode(nodeId);
        if (node && followType == FollowType::FOLLOW_TO_PARENT) {
            auto parentNode = node->GetParent().lock();
            if (parentNode) {
                nodeId = parentNode->GetId();
            } else {
                pendingEffectiveCommands_[timestamp].emplace_back(std::move(command));
                continue;
            }
        }
        cachedCommands_[nodeId][timestamp].emplace_back(std::move(command));
    }
}

void RSMainThread::PostTask(RSTaskMessage::RSTask task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSMainThread::PostSyncTask(RSTaskMessage::RSTask task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSMainThread::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    applicationAgentMap_.emplace(pid, app);
}

void RSMainThread::UnRegisterApplicationAgent(sptr<IApplicationAgent> app)
{
    EraseIf(applicationAgentMap_, [&app](const auto& iter) { return iter.second == app; });
}

void RSMainThread::RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback)
{
    occlusionListeners_.emplace_back(callback);
}

void RSMainThread::UnRegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback)
{
    auto iter = std::find(occlusionListeners_.begin(), occlusionListeners_.end(), callback);
    if (iter != occlusionListeners_.end()) {
        occlusionListeners_.erase(iter);
    }
}

void RSMainThread::CleanOcclusionListener()
{
    occlusionListeners_.clear();
}

void RSMainThread::SendCommands()
{
    RS_TRACE_FUNC();
    if (!RSMessageProcessor::Instance().HasTransaction()) {
        return;
    }

    // dispatch messages to corresponding application
    auto transactionMapPtr = std::make_shared<std::unordered_map<uint32_t, RSTransactionData>>(
        RSMessageProcessor::Instance().GetAllTransactions());
    PostTask([this, transactionMapPtr]() {
        for (auto& transactionIter : *transactionMapPtr) {
            auto pid = transactionIter.first;
            auto appIter = applicationAgentMap_.find(pid);
            if (appIter == applicationAgentMap_.end()) {
                RS_LOGW(
                    "RSMainThread::SendCommand no application agent registered as pid %d, this will cause memory leak!",
                    pid);
                continue;
            }
            auto& app = appIter->second;
            auto transactionPtr = std::make_shared<RSTransactionData>(std::move(transactionIter.second));
            app->OnTransaction(transactionPtr);
        }
    });
}

void RSMainThread::QosStateDump(std::string& dumpString)
{
    if (RSQosThread::GetInstance()->GetQosCal()) {
        dumpString.append("QOS is enabled\n");
    } else {
        dumpString.append("QOS is disabled\n");
    }
}

void RSMainThread::RenderServiceTreeDump(std::string& dumpString)
{
    dumpString.append("Animating Node: [");
    for (auto& [nodeId, _]: context_->animatingNodeList_) {
        dumpString.append(std::to_string(nodeId) + ", ");
    }
    dumpString.append("];\n");
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        dumpString.append("rootNode is null\n");
        return;
    }
    rootNode->DumpTree(0, dumpString);
}

bool RSMainThread::DoParallelComposition(std::shared_ptr<RSBaseRenderNode> rootNode)
{
    using CreateParallelSyncSignalFunc = void* (*)(uint32_t);
    using SignalCountDownFunc = void (*)(void*);
    using SignalAwaitFunc = void (*)(void*);
    using AssignTaskFunc = void (*)(std::function<void()>);
    using RemoveStoppedThreadsFunc = void (*)();

    auto CreateParallelSyncSignal = (CreateParallelSyncSignalFunc)RSInnovation::_s_createParallelSyncSignal;
    auto SignalCountDown = (SignalCountDownFunc)RSInnovation::_s_signalCountDown;
    auto SignalAwait = (SignalAwaitFunc)RSInnovation::_s_signalAwait;
    auto AssignTask = (AssignTaskFunc)RSInnovation::_s_assignTask;
    auto RemoveStoppedThreads = (RemoveStoppedThreadsFunc)RSInnovation::_s_removeStoppedThreads;

    void* syncSignal = (*CreateParallelSyncSignal)(rootNode->GetChildrenCount());
    if (!syncSignal) {
        return false;
    }

    (*RemoveStoppedThreads)();

    auto children = rootNode->GetSortedChildren();
    bool animate_ = doWindowAnimate_;
    for (auto it = children.rbegin(); it != children.rend(); it++) {
        auto child = *it;
        auto task = [&syncSignal, SignalCountDown, child, animate_]() {
            std::shared_ptr<RSNodeVisitor> visitor;
            auto rsVisitor = std::make_shared<RSRenderServiceVisitor>(true);
            rsVisitor->SetAnimateState(animate_);
            visitor = rsVisitor;
            child->Process(visitor);
            (*SignalCountDown)(syncSignal);
        };
        if (*it == *children.begin()) {
            task();
        } else {
            (*AssignTask)(task);
        }
    }
    (*SignalAwait)(syncSignal);
    ResetSortedChildren(rootNode);
    return true;
}

void RSMainThread::ResetSortedChildren(std::shared_ptr<RSBaseRenderNode> node)
{
    for (auto& child : node->GetSortedChildren()) {
        ResetSortedChildren(child);
    }
    node->ResetSortedChildren();
}

void RSMainThread::ClearTransactionDataPidInfo(pid_t remotePid)
{
    if (!isUniRender_) {
        return;
    }
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    if (effectiveTransactionDataIndexMap_.count(remotePid) > 0 &&
        !effectiveTransactionDataIndexMap_[remotePid].second.empty()) {
        RS_LOGD("RSMainThread::ClearTransactionDataPidInfo process:%d destroyed, skip commands", remotePid);
    }
    effectiveTransactionDataIndexMap_.erase(remotePid);
    transactionDataLastWaitTime_.erase(remotePid);

    // clear cpu cache when process exit
    // CLEAN_CACHE_FREQ to prevent multiple cleanups in a short period of time
    if ((timestamp_ - lastCleanCacheTimestamp_) / REFRESH_PERIOD > CLEAN_CACHE_FREQ) {
#ifdef RS_ENABLE_GL
        RS_LOGD("RSMainThread: clear cpu cache");
        auto grContext = GetRenderEngine()->GetRenderContext()->GetGrContext();
        grContext->flush();
        SkGraphics::PurgeAllCaches();
        grContext->flush(kSyncCpu_GrFlushFlag, 0, nullptr);
        lastCleanCacheTimestamp_ = timestamp_;
#endif
    }
}

void RSMainThread::TrimMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString)
{
#ifdef RS_ENABLE_GL
    if (!RSUniRenderJudgement::IsUniRender()) {
        dumpString.append("\n---------------\nNot in UniRender and no resource can be released");
        return;
    }
    std::string type;
    argSets.erase(u"trimMem");
    if (!argSets.empty()) {
        type = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(*argSets.begin());
    }
    auto grContext = GetRenderEngine()->GetRenderContext()->GetGrContext();
    if (type.empty()) {
        grContext->flush();
        SkGraphics::PurgeAllCaches();
        grContext->freeGpuResources();
        grContext->purgeUnlockedResources(true);
        std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
        rendercontext->CleanAllShaderCache();
        grContext->flush(kSyncCpu_GrFlushFlag, 0, nullptr);
    } else if (type == "cpu") {
        grContext->flush();
        SkGraphics::PurgeAllCaches();
        grContext->flush(kSyncCpu_GrFlushFlag, 0, nullptr);
    } else if (type == "gpu") {
        grContext->flush();
        grContext->freeGpuResources();
        grContext->flush(kSyncCpu_GrFlushFlag, 0, nullptr);
    } else if (type == "uihidden") {
        grContext->flush();
        grContext->purgeUnlockedResources(true);
        grContext->flush(kSyncCpu_GrFlushFlag, 0, nullptr);
    } else if (type == "shader") {
        std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
        rendercontext->CleanAllShaderCache();
    } else {
        type = "error";
    }
    dumpString.append("trimMem: " + type + "\n");
#else
    dumpString.append("No GPU in this device");
#endif
}

void RSMainThread::DumpMem(std::string& dumpString)
{
#ifdef RS_ENABLE_GL
    DfxString log;
    if (!RSUniRenderJudgement::IsUniRender()) {
        log.AppendFormat("\n---------------\nNot in UniRender and no information");
    } else {
        MemoryManager::DumpMemoryUsage(log, GetRenderEngine()->GetRenderContext()->GetGrContext());
    }
    dumpString.append(log.GetString());
#else
    dumpString.append("No GPU in this device");
#endif
}

void RSMainThread::AddTransactionDataPidInfo(pid_t remotePid)
{
    if (!isUniRender_) {
        return;
    }
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    if (effectiveTransactionDataIndexMap_.count(remotePid) > 0) {
        RS_LOGW("RSMainThread::AddTransactionDataPidInfo remotePid:%d already exists", remotePid);
    }
    effectiveTransactionDataIndexMap_[remotePid].first = 0;
}

void RSMainThread::SetDirtyFlag()
{
    isDirty_ = true;
}

void RSMainThread::PerfAfterAnim()
{
    if (!isUniRender_) {
        return;
    }
    if (!context_->animatingNodeList_.empty() && timestamp_ - prePerfTimestamp_ > PERF_PERIOD) {
        RS_LOGD("RSMainThread:: soc perf to render_service_animation");
        PerfRequest(PERF_ANIMATION_REQUESTED_CODE, true);
        prePerfTimestamp_ = timestamp_;
    } else if (context_->animatingNodeList_.empty()) {
        RS_LOGD("RSMainThread:: soc perf off render_service_animation");
        PerfRequest(PERF_ANIMATION_REQUESTED_CODE, false);
        prePerfTimestamp_ = 0;
    }
}
void RSMainThread::ForceRefreshForUni()
{
    if (isUniRender_) {
        PostTask([=]() {
            MergeToEffectiveTransactionDataMap(cachedTransactionDataMap_);
            RSUnmarshalThread::Instance().PostTask(unmarshalBarrierTask_);
            mainLoop_();
        });
        auto screenManager_ = CreateOrGetScreenManager();
        if (screenManager_ != nullptr) {
            auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
            if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
                RSHardwareThread::Instance().PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
            } else {
                PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
            }
        }
    } else {
        RequestNextVSync();
    }
}

void RSMainThread::PerfForBlurIfNeeded()
{
    static int preBlurCnt = 0;
    int blurCnt = RSPropertiesPainter::GetAndResetBlurCnt();
    // clamp blurCnt to 0~3.
    blurCnt = std::clamp<int>(blurCnt, 0, 3);
    if (blurCnt != preBlurCnt && preBlurCnt != 0) {
        PerfRequest(BLUR_CNT_TO_BLUR_CODE.at(preBlurCnt), false);
        preBlurCnt = 0;
    }
    if (blurCnt == 0) {
        return;
    }
    static uint64_t prePerfTimestamp = 0;
    if (timestamp_ - prePerfTimestamp > PERF_PERIOD_BLUR || blurCnt != preBlurCnt) {
        PerfRequest(BLUR_CNT_TO_BLUR_CODE.at(blurCnt), true);
        prePerfTimestamp = timestamp_;
        preBlurCnt = blurCnt;
    }
}

void RSMainThread::PerfMultiWindow()
{
    if (!isUniRender_) {
        return;
    }
    static uint64_t lastPerfTimestamp = 0;
    if (appWindowNum_ >= MULTI_WINDOW_PERF_START_NUM && appWindowNum_ <= MULTI_WINDOW_PERF_END_NUM
        && timestamp_ - lastPerfTimestamp > PERF_PERIOD_MULTI_WINDOW) {
        RS_LOGD("RSMainThread::PerfMultiWindow soc perf");
        PerfRequest(PERF_MULTI_WINDOW_REQUESTED_CODE, true);
        lastPerfTimestamp = timestamp_;
    } else if ((appWindowNum_ < MULTI_WINDOW_PERF_START_NUM || appWindowNum_ > MULTI_WINDOW_PERF_END_NUM)
        && timestamp_ - lastPerfTimestamp < PERF_PERIOD_MULTI_WINDOW) {
        RS_LOGD("RSMainThread::PerfMultiWindow soc perf off");
        PerfRequest(PERF_MULTI_WINDOW_REQUESTED_CODE, false);
    }
}

void RSMainThread::SetAppWindowNum(uint32_t num)
{
    appWindowNum_ = num;
}

void RSMainThread::ResetHardwareEnabledState()
{
    doDirectComposition_ = RSSystemProperties::GetHardwareComposerEnabled();
    isHardwareEnabledBufferUpdated_ = false;
    hardwareEnabledNodes_.clear();
    isHardwareForcedDisabled_ = false;
}
} // namespace Rosen
} // namespace OHOS
