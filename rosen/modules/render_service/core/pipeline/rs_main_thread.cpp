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

#include <list>
#include "include/core/SkGraphics.h"
#include "memory/rs_memory_graphic.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include <securec.h>
#include <stdint.h>
#include <string>
#include <unistd.h>
#include <malloc.h>
#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#include "include/gpu/GrGpuResource.h"
#endif
#include "rs_trace.h"

#include "animation/rs_animation_fraction.h"
#include "command/rs_message_processor.h"
#include "common/rs_background_thread.h"
#include "delegate/rs_functional_delegate.h"
#include "memory/rs_memory_manager.h"
#include "memory/rs_memory_track.h"
#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "hgm_core.h"
#include "platform/ohos/rs_jank_stats.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_cold_start_thread.h"
#include "pipeline/rs_divided_render_util.h"
#include "pipeline/rs_frame_report.h"
#include "pipeline/rs_render_engine.h"
#include "pipeline/rs_render_service_visitor.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_hardware_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_unmarshal_thread.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_occlusion_config.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_innovation.h"
#include "platform/common/rs_system_properties.h"
#include "platform/drawing/rs_vsync_client.h"
#include "property/rs_property_trace.h"
#include "property/rs_properties_painter.h"
#ifdef NEW_RENDER_CONTEXT
#include "render_context/memory_handler.h"
#endif
#include "render/rs_pixel_map_util.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_transaction_proxy.h"

#include "rs_qos_thread.h"
#include "xcollie/watchdog.h"

#include "render_frame_trace.h"

#if defined(ACCESSIBILITY_ENABLE)
#include "accessibility_config.h"
#endif

#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
#include "pipeline/driven_render/rs_driven_render_manager.h"
#endif

#if defined(RS_ENABLE_RECORDING)
#include "benchmarks/rs_recording_thread.h"
#endif

#include "scene_board_judgement.h"
#include "vsync_iconnection_token.h"

using namespace FRAME_TRACE;
static const std::string RS_INTERVAL_NAME = "renderservice";

#if defined(ACCESSIBILITY_ENABLE)
using namespace OHOS::AccessibilityConfig;
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t REQUEST_VSYNC_NUMBER_LIMIT = 10;
constexpr uint64_t REFRESH_PERIOD = 16666667;
constexpr int32_t PERF_MULTI_WINDOW_REQUESTED_CODE = 10026;
constexpr int32_t FLUSH_SYNC_TRANSACTION_TIMEOUT = 100;
constexpr uint64_t PERF_PERIOD = 250000000;
constexpr uint64_t CLEAN_CACHE_FREQ = 60;
constexpr uint64_t SKIP_COMMAND_FREQ_LIMIT = 30;
constexpr uint64_t PERF_PERIOD_BLUR = 80000000;
constexpr const char* MEM_GPU_TYPE = "gpu";
constexpr uint64_t PERF_PERIOD_MULTI_WINDOW = 80000000;
constexpr uint32_t MULTI_WINDOW_PERF_START_NUM = 2;
constexpr uint32_t MULTI_WINDOW_PERF_END_NUM = 4;
constexpr uint32_t WAIT_FOR_RELEASED_BUFFER_TIMEOUT = 3000;
constexpr const char* WALLPAPER_VIEW = "WallpaperView";
#ifdef RS_ENABLE_GL
constexpr size_t DEFAULT_SKIA_CACHE_SIZE        = 96 * (1 << 20);
constexpr int DEFAULT_SKIA_CACHE_COUNT          = 2 * (1 << 12);
#endif
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

#if defined(ACCESSIBILITY_ENABLE)
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
        RSMainThread::Instance()->PostTask([]() {
            RSMainThread::Instance()->SetAccessibilityConfigChanged();
            RSMainThread::Instance()->RequestNextVSync();
        });
    }
};
#endif
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
        RenderFrameStart();
        PerfMultiWindow();
        SetRSEventDetectorLoopStartTag();
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSMainThread::DoComposition");
        ConsumeAndUpdateAllNodes();
        WaitUntilUnmarshallingTaskFinished();
        ProcessCommand();
        Animate(timestamp_);
        CollectInfoForHardwareComposer();
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
        CollectInfoForDrivenRender();
#endif
        CheckColdStartMap();
        Render();
        InformHgmNodeInfo();
        ReleaseAllNodesBuffer();
        SendCommands();
        activeAppsInProcess_.clear();
        activeProcessNodeIds_.clear();
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        SetRSEventDetectorLoopFinishTag();
        rsEventManager_.UpdateParam();
    };
#ifdef RS_ENABLE_RECORDING
    RSRecordingThread::Instance().Start();
#endif
    isUniRender_ = RSUniRenderJudgement::IsUniRender();
    SetDeviceType();
    RsFrameReport::GetInstance().Init();
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
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    sptr<VSyncConnection> conn = new VSyncConnection(rsVSyncDistributor_, "rs", token->AsObject());
    rsVSyncDistributor_->AddConnection(conn);
    receiver_ = std::make_shared<VSyncReceiver>(conn, token->AsObject(), handler_);
    receiver_->Init();
    if (isUniRender_) {
        uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
        uniRenderEngine_->Init();
    } else {
        renderEngine_ = std::make_shared<RSRenderEngine>();
        renderEngine_->Init();
    }
#ifdef RS_ENABLE_GL
    int cacheLimitsTimes = 3;
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
    auto grContext = isUniRender_? uniRenderEngine_->GetDrawingContext()->GetDrawingContext() :
        renderEngine_->GetDrawingContext()->GetDrawingContext();
#else
    auto grContext = isUniRender_? uniRenderEngine_->GetRenderContext()->GetGrContext() :
        renderEngine_->GetRenderContext()->GetGrContext();
#endif
    int maxResources = 0;
    size_t maxResourcesSize = 0;
    grContext->getResourceCacheLimits(&maxResources, &maxResourcesSize);
    if (maxResourcesSize > 0) {
        grContext->setResourceCacheLimits(cacheLimitsTimes * maxResources, cacheLimitsTimes *
            std::fmin(maxResourcesSize, DEFAULT_SKIA_CACHE_SIZE));
    } else {
        grContext->setResourceCacheLimits(DEFAULT_SKIA_CACHE_COUNT, DEFAULT_SKIA_CACHE_SIZE);
    }
#else
    auto gpuContext = isUniRender_? uniRenderEngine_->GetRenderContext()->GetDrGPUContext() :
        renderEngine_->GetRenderContext()->GetDrGPUContext();
    if (gpuContext == nullptr) {
        RS_LOGE("RSMainThread::Init gpuContext is nullptr!");
        return;
    }
    int32_t maxResources = 0;
    size_t maxResourcesSize = 0;
    gpuContext->GetResourceCacheLimits(maxResources, maxResourcesSize);
    if (maxResourcesSize > 0) {
        gpuContext->SetResourceCacheLimits(cacheLimitsTimes * maxResources, cacheLimitsTimes * maxResourcesSize);
    } else {
        gpuContext->SetResourceCacheLimits(DEFAULT_SKIA_CACHE_COUNT, DEFAULT_SKIA_CACHE_SIZE);
    }
#endif
#endif
    RSInnovation::OpenInnovationSo();
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    RSDrivenRenderManager::InitInstance();
#endif

#if defined(ACCESSIBILITY_ENABLE)
    accessibilityObserver_ = std::make_shared<AccessibilityObserver>();
    auto &config = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    config.InitializeContext();
    config.SubscribeConfigObserver(CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER, accessibilityObserver_);
    config.SubscribeConfigObserver(CONFIG_ID::CONFIG_INVERT_COLOR, accessibilityObserver_);
    if (isUniRender_) {
        config.SubscribeConfigObserver(CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT, accessibilityObserver_);
    }
#endif

    auto delegate = RSFunctionalDelegate::Create();
    delegate->SetRepaintCallback([]() { RSMainThread::Instance()->RequestNextVSync(); });
    RSOverdrawController::GetInstance().SetDelegate(delegate);

    frameRateMgr_ = std::make_unique<HgmFrameRateManager>();
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

void RSMainThread::SetDeviceType()
{
    auto deviceTypeStr = system::GetParameter("const.product.devicetype", "pc");
    if (deviceTypeStr == "pc") {
        deviceType_ = DeviceType::PC;
    } else if (deviceTypeStr == "tablet") {
        deviceType_ = DeviceType::TABLET;
    } else if (deviceTypeStr == "phone") {
        deviceType_ = DeviceType::PHONE;
    } else {
        deviceType_ = DeviceType::OTHERS;
    }
}

void RSMainThread::SetIsCachedSurfaceUpdated(bool isCachedSurfaceUpdated)
{
    isCachedSurfaceUpdated_ = isCachedSurfaceUpdated;
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
    int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName, uint64_t focusNodeId)
{
    focusAppPid_ = pid;
    focusAppUid_ = uid;
    focusAppBundleName_ = bundleName;
    focusAppAbilityName_ = abilityName;
    focusNodeId_ = focusNodeId;
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
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().AnimateStart();
    }
}

void RSMainThread::CacheCommands()
{
    RS_OPTIONAL_TRACE_FUNC_BEGIN();
    for (auto& skipTransactionData : cachedSkipTransactionDataMap_) {
        pid_t pid = skipTransactionData.first;
        RS_TRACE_NAME("cacheCmd pid: " + std::to_string(pid));
        auto& skipTransactionDataVec = skipTransactionData.second;
        cachedTransactionDataMap_[pid].insert(cachedTransactionDataMap_[pid].begin(),
            std::make_move_iterator(skipTransactionDataVec.begin()),
            std::make_move_iterator(skipTransactionDataVec.end()));
    }
    RS_OPTIONAL_TRACE_FUNC_END();
}

void RSMainThread::CheckIfNodeIsBundle(std::shared_ptr<RSSurfaceRenderNode> node)
{
    currentBundleName_ = node->GetBundleName();
    if (node->GetName() == WALLPAPER_VIEW) {
        noBundle_ = true;
    }
}

void RSMainThread::InformHgmNodeInfo()
{
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    int32_t informResult = EXEC_SUCCESS;
    if (currentBundleName_ != "") {
        informResult = hgmCore.RefreshBundleName(currentBundleName_);
    } else if (noBundle_) {
        currentBundleName_ = "";
        informResult = hgmCore.RefreshBundleName(currentBundleName_);
        noBundle_ = false;
    }
    if (informResult != EXEC_SUCCESS) {
        RS_LOGE("RSMainThread::InformHgmNodeInfo failed to refresh bundle name in hgm");
    }
}

std::unordered_map<NodeId, bool> RSMainThread::GetCacheCmdSkippedNodes() const
{
    return cacheCmdSkippedNodes_;
}

void RSMainThread::CheckParallelSubThreadNodesStatus()
{
    RS_OPTIONAL_TRACE_FUNC_BEGIN();
    cacheCmdSkippedInfo_.clear();
    cacheCmdSkippedNodes_.clear();
    for (auto& node : subThreadNodes_) {
        if (node == nullptr) {
            RS_LOGE("RSMainThread::CheckParallelSubThreadNodesStatus sunThreadNode is nullptr!");
            continue;
        }
        if (node->GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DOING) {
            RS_TRACE_NAME("node:[ " + node->GetName() + "]");
            pid_t pid = 0;
            if (node->IsAppWindow()) {
                pid = ExtractPid(node->GetId());
            } else if (node->IsLeashWindow()) {
                for (auto& child : node->GetSortedChildren()) {
                    auto surfaceNodePtr = child->ReinterpretCastTo<RSSurfaceRenderNode>();
                    if (surfaceNodePtr && surfaceNodePtr->IsAppWindow()) {
                        pid = ExtractPid(child->GetId());
                        break;
                    }
                }
            }
            cacheCmdSkippedNodes_[node->GetId()] = false;
            if (pid == 0) {
                continue;
            }
            RS_LOGD("RSMainThread::CheckParallelSubThreadNodesStatus pid = %s, node name: %s, id: %llu",
                std::to_string(pid).c_str(), node->GetName().c_str(), node->GetId());
            if (cacheCmdSkippedInfo_.count(pid) == 0) {
                cacheCmdSkippedInfo_[pid] = std::make_pair(std::vector<NodeId>{node->GetId()}, false);
            } else {
                cacheCmdSkippedInfo_[pid].first.push_back(node->GetId());
            }
        }
    }
    RS_OPTIONAL_TRACE_FUNC_END();
}

bool RSMainThread::IsNeedSkip(NodeId instanceRootNodeId, pid_t pid)
{
    for (auto& cacheCmdSkipNodeId: cacheCmdSkippedInfo_[pid].first) {
        if (cacheCmdSkipNodeId == instanceRootNodeId) {
            return true;
        }
    }
    return false;
}

void RSMainThread::SkipCommandByNodeId(std::vector<std::unique_ptr<RSTransactionData>>& transactionVec, pid_t pid)
{
    if (cacheCmdSkippedInfo_.find(pid) == cacheCmdSkippedInfo_.end()) {
        return;
    }
    std::vector<std::unique_ptr<RSTransactionData>> skipTransactionVec;
    const auto& nodeMap = context_->GetNodeMap();
    for (auto& transactionData: transactionVec) {
        std::vector<std::tuple<NodeId, FollowType, std::unique_ptr<RSCommand>>> skipPayload;
        std::vector<size_t> skipPayloadIndexVec;
        auto& processPayload = transactionData->GetPayload();
        for (size_t index = 0; index < processPayload.size(); ++index) {
            auto& elem = processPayload[index];
            if (std::get<2>(elem) == nullptr) { // check elem is valid
                continue;
            }
            NodeId nodeId = std::get<2>(elem)->GetNodeId();
            auto node = nodeMap.GetRenderNode(nodeId);
            if (node == nullptr) {
                continue;
            }
            NodeId instanceRootNodeId = node->GetInstanceRootNodeId();
            if (IsNeedSkip(instanceRootNodeId, pid)) {
                skipPayload.emplace_back(std::move(elem));
                skipPayloadIndexVec.push_back(index);
            }
        }
        if (!skipPayload.empty()) {
            std::unique_ptr<RSTransactionData> skipTransactionData = std::make_unique<RSTransactionData>();
            skipTransactionData->SetTimestamp(transactionData->GetTimestamp());
            std::string ablityName = transactionData->GetAbilityName();
            skipTransactionData->SetAbilityName(ablityName);
            skipTransactionData->SetSendingPid(transactionData->GetSendingPid());
            skipTransactionData->SetIndex(transactionData->GetIndex());
            skipTransactionData->GetPayload() = std::move(skipPayload);
            skipTransactionData->SetIsCached(true);
            skipTransactionVec.emplace_back(std::move(skipTransactionData));
        }
        for (auto iter = skipPayloadIndexVec.rbegin(); iter != skipPayloadIndexVec.rend(); ++iter) {
            processPayload.erase(processPayload.begin() + *iter);
        }
    }
    if (!skipTransactionVec.empty()) {
        cachedSkipTransactionDataMap_[pid] = std::move(skipTransactionVec);
    }
}

void RSMainThread::CheckAndUpdateTransactionIndex(std::shared_ptr<TransactionDataMap>& transactionDataEffective,
    std::string& transactionFlags)
{
    for (auto& rsTransactionElem: effectiveTransactionDataIndexMap_) {
        auto pid = rsTransactionElem.first;
        auto& lastIndex = rsTransactionElem.second.first;
        auto& transactionVec = rsTransactionElem.second.second;
        auto iter = transactionVec.begin();
        for (; iter != transactionVec.end(); ++iter) {
            if ((*iter) == nullptr) {
                continue;
            }
            if ((*iter)->GetIsCached()) {
                continue;
            }
            auto curIndex = (*iter)->GetIndex();
            if (curIndex == lastIndex + 1) {
                if ((*iter)->GetTimestamp() >= timestamp_) {
                    break;
                }
                ++lastIndex;
                transactionFlags += " [" + std::to_string(pid) + "," + std::to_string(curIndex) + "]";
            } else {
                RS_LOGE("%s wait curIndex:%llu, lastIndex:%llu, pid:%d", __FUNCTION__, curIndex, lastIndex, pid);
                if (transactionDataLastWaitTime_[pid] == 0) {
                    transactionDataLastWaitTime_[pid] = timestamp_;
                }
                if ((timestamp_ - transactionDataLastWaitTime_[pid]) / REFRESH_PERIOD > SKIP_COMMAND_FREQ_LIMIT) {
                    transactionDataLastWaitTime_[pid] = 0;
                    lastIndex = curIndex;
                    transactionFlags += " skip to[" + std::to_string(pid) + "," + std::to_string(curIndex) + "]";
                    RS_LOGE("%s skip to index:%llu, pid:%d", __FUNCTION__, curIndex, pid);
                    continue;
                }
                break;
            }
        }
        if (iter != transactionVec.begin()) {
            (*transactionDataEffective)[pid].insert((*transactionDataEffective)[pid].end(),
                std::make_move_iterator(transactionVec.begin()), std::make_move_iterator(iter));
            transactionVec.erase(transactionVec.begin(), iter);
        }
    }
}

void RSMainThread::ProcessCommandForUniRender()
{
    ResetHardwareEnabledState();
    std::shared_ptr<TransactionDataMap> transactionDataEffective = std::make_shared<TransactionDataMap>();
    std::string transactionFlags;
    if (RSSystemProperties::GetUIFirstEnabled() && RSSystemProperties::GetCacheCmdEnabled()) {
        CheckParallelSubThreadNodesStatus();
    }
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        cachedSkipTransactionDataMap_.clear();
        for (auto& rsTransactionElem: effectiveTransactionDataIndexMap_) {
            auto pid = rsTransactionElem.first;
            auto& transactionVec = rsTransactionElem.second.second;
            if (RSSystemProperties::GetUIFirstEnabled() && RSSystemProperties::GetCacheCmdEnabled()) {
                SkipCommandByNodeId(transactionVec, pid);
            }
            std::sort(transactionVec.begin(), transactionVec.end(), Compare);
        }
        if (RSSystemProperties::GetUIFirstEnabled() && RSSystemProperties::GetCacheCmdEnabled()) {
            CacheCommands();
        }
        CheckAndUpdateTransactionIndex(transactionDataEffective, transactionFlags);
    }
    if (!transactionDataEffective->empty()) {
        doDirectComposition_ = false;
    }
    RS_OPTIONAL_TRACE_BEGIN("RSMainThread::ProcessCommandUni" + transactionFlags);
    for (auto& rsTransactionElem: *transactionDataEffective) {
        for (auto& rsTransaction: rsTransactionElem.second) {
            if (rsTransaction) {
                if (rsTransaction->IsNeedSync() || syncTransactionData_.count(rsTransactionElem.first) > 0) {
                    ProcessSyncRSTransactionData(rsTransaction, rsTransactionElem.first);
                    continue;
                }
                ProcessRSTransactionData(rsTransaction, rsTransactionElem.first);
            }
        }
    }
    RSBackgroundThread::Instance().PostTask([ transactionDataEffective ] () {
        RS_TRACE_NAME("RSMainThread::ProcessCommandForUniRender transactionDataEffective clear");
        transactionDataEffective->clear();
    });
    RS_OPTIONAL_TRACE_END();
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

void RSMainThread::ProcessRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData, pid_t pid)
{
    context_->transactionTimestamp_ = rsTransactionData->GetTimestamp();
    rsTransactionData->Process(*context_);
    for (auto& [nodeId, followType, command] : rsTransactionData->GetPayload()) {
        if (command != nullptr) {
            AddActiveNodeId(pid, command->GetNodeId());
        }
    }
}

void RSMainThread::ProcessSyncRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData, pid_t pid)
{
    if (!rsTransactionData->IsNeedSync()) {
        syncTransactionData_[pid].emplace_back(std::move(rsTransactionData));
        return;
    }

    if (!syncTransactionData_.empty() && syncTransactionData_.begin()->second.front() &&
        (syncTransactionData_.begin()->second.front()->GetSyncId() > rsTransactionData->GetSyncId())) {
        ROSEN_LOGD("RSMainThread ProcessSyncRSTransactionData while syncId less GetCommandCount: %lu pid: %llu",
            rsTransactionData->GetCommandCount(), rsTransactionData->GetSendingPid());
        ProcessRSTransactionData(rsTransactionData, pid);
        return;
    }

    bool isNeedCloseSync = rsTransactionData->IsNeedCloseSync();
    if (syncTransactionData_.empty()) {
        if (handler_) {
            auto task = [this]() {
                ROSEN_LOGD("RSMainThread ProcessAllSyncTransactionData timeout task");
                ProcessAllSyncTransactionData();
            };
            handler_->PostTask(task, "ProcessAllSyncTransactionsTimeoutTask", FLUSH_SYNC_TRANSACTION_TIMEOUT);
        }
    }
    if (!syncTransactionData_.empty() && syncTransactionData_.begin()->second.front() &&
        (syncTransactionData_.begin()->second.front()->GetSyncId() != rsTransactionData->GetSyncId())) {
        ProcessAllSyncTransactionData();
    }
    if (syncTransactionData_.count(pid) == 0) {
        syncTransactionData_.insert({ pid, std::vector<std::unique_ptr<RSTransactionData>>() });
    }
    if (isNeedCloseSync) {
        syncTransactionCount_ += rsTransactionData->GetSyncTransactionNum();
    } else {
        syncTransactionCount_ -= 1;
    }
    syncTransactionData_[pid].emplace_back(std::move(rsTransactionData));
    if (syncTransactionCount_ == 0) {
        ProcessAllSyncTransactionData();
    }
}

void RSMainThread::ProcessAllSyncTransactionData()
{
    for (auto& [pid, transactions] : syncTransactionData_) {
        for (auto& transaction: transactions) {
            ROSEN_LOGD("RSMainThread ProcessAllSyncTransactionData GetCommandCount: %lu pid: %llu",
                transaction->GetCommandCount(), pid);
            ProcessRSTransactionData(transaction, pid);
        }
    }
    syncTransactionData_.clear();
    syncTransactionCount_ = 0;
    RequestNextVSync();
}

void RSMainThread::ConsumeAndUpdateAllNodes()
{
    RS_OPTIONAL_TRACE_BEGIN("RSMainThread::ConsumeAndUpdateAllNodes");
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
            if (surfaceNode->IsCurrentFrameBufferConsumed() && !surfaceNode->IsHardwareEnabledType()) {
                surfaceNode->SetContentDirty();
                AddActiveNodeId(ExtractPid(surfaceNode->GetId()), surfaceNode->GetId());
                doDirectComposition_ = false;
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
    RS_OPTIONAL_TRACE_END();
}

void RSMainThread::CollectInfoForHardwareComposer()
{
    if (!isUniRender_) {
        return;
    }
    CheckIfHardwareForcedDisabled();
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes(
        [this](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
            if (surfaceNode == nullptr || !surfaceNode->IsOnTheTree()) {
                return;
            }
            if (!surfaceNode->IsHardwareEnabledType()) {
                return;
            }

            // if hwc node is set on the tree this frame, mark its parent app node to be prepared
            if (surfaceNode->IsNewOnTree()) {
                auto appNodeId = surfaceNode->GetInstanceRootNodeId();
                AddActiveNodeId(ExtractPid(appNodeId), appNodeId);
                surfaceNode->ResetIsNewOnTree();
            }

            if (surfaceNode->GetBuffer() != nullptr) {
                // collect hwc nodes vector, used for display node skip and direct composition cases
                hardwareEnabledNodes_.emplace_back(surfaceNode);
            }

            // set content dirty for hwc node if needed
            if (isHardwareForcedDisabled_) {
                // buffer updated or hwc -> gpu
                if (surfaceNode->IsCurrentFrameBufferConsumed() || surfaceNode->IsLastFrameHardwareEnabled()) {
                    surfaceNode->SetContentDirty();
                    AddActiveNodeId(ExtractPid(surfaceNode->GetId()), surfaceNode->GetId());
                }
            } else { // gpu -> hwc
                if (!surfaceNode->IsLastFrameHardwareEnabled()) {
                    surfaceNode->SetContentDirty();
                    AddActiveNodeId(ExtractPid(surfaceNode->GetId()), surfaceNode->GetId());
                    doDirectComposition_ = false;
                }
            }

            if (surfaceNode->IsCurrentFrameBufferConsumed()) {
                isHardwareEnabledBufferUpdated_ = true;
            }
        });
}

void RSMainThread::CheckIfHardwareForcedDisabled()
{
    ColorFilterMode colorFilterMode = renderEngine_->GetColorFilterMode();
    bool hasColorFilter = colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
        colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE;
    std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    bool isMultiDisplay = rootNode && rootNode->GetChildrenCount() > 1;

    // [PLANNING] GetChildrenCount > 1 indicates multi display, only Mirror Mode need be marked here
    // Mirror Mode reuses display node's buffer, so mark it and disable hardware composer in this case
    isHardwareForcedDisabled_ = isHardwareForcedDisabled_ || doWindowAnimate_ || isMultiDisplay || hasColorFilter;
}

void RSMainThread::CollectInfoForDrivenRender()
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    hasDrivenNodeOnUniTree_ = false;
    hasDrivenNodeMarkRender_ = false;
    if (!isUniRender_ || !RSSystemProperties::GetHardwareComposerEnabled() ||
        !RSDrivenRenderManager::GetInstance().GetDrivenRenderEnabled()) {
        return;
    }

    std::vector<std::shared_ptr<RSRenderNode>> drivenNodes;
    std::vector<std::shared_ptr<RSRenderNode>> markRenderDrivenNodes;

    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseDrivenRenderNodes(
        [&](const std::shared_ptr<RSRenderNode>& node) mutable {
            if (node == nullptr || !node->IsOnTheTree()) {
                return;
            }
            drivenNodes.emplace_back(node);
            if (node->GetPaintState()) {
                markRenderDrivenNodes.emplace_back(node);
            }
        });

    for (auto& node : drivenNodes) {
        node->SetPaintState(false);
        node->SetIsMarkDrivenRender(false);
    }
    if (!drivenNodes.empty()) {
        hasDrivenNodeOnUniTree_ = true;
    } else {
        hasDrivenNodeOnUniTree_ = false;
    }
    if (markRenderDrivenNodes.size() == 1) { // only support 1 driven node
        auto node = markRenderDrivenNodes.front();
        node->SetIsMarkDrivenRender(true);
        hasDrivenNodeMarkRender_ = true;
    } else {
        hasDrivenNodeMarkRender_ = false;
    }
#endif
}

bool RSMainThread::NeedReleaseGpuResource(const RSRenderNodeMap& nodeMap)
{
    bool needReleaseGpuResource = lastFrameHasFilter_;
    bool currentFrameHasFilter = false;
    auto residentSurfaceNodeMap = nodeMap.GetResidentSurfaceNodeMap();
    for (const auto& [_, surfaceNode] : residentSurfaceNodeMap) {
        if (!surfaceNode || !surfaceNode->IsOnTheTree()) {
            continue;
        }
        // needReleaseGpuResource will be set true when all nodes don't need filter, otherwise false.
        needReleaseGpuResource = needReleaseGpuResource &&
            !(surfaceNode->GetRenderProperties().NeedFilter() || !(surfaceNode->GetChildrenNeedFilterRects().empty()));
        // currentFrameHasFilter will be set true when one node needs filter, otherwise false.
        currentFrameHasFilter = currentFrameHasFilter ||
            surfaceNode->GetRenderProperties().NeedFilter() || !(surfaceNode->GetChildrenNeedFilterRects().empty());
    }
    lastFrameHasFilter_ = currentFrameHasFilter;
    return needReleaseGpuResource;
}

void RSMainThread::ReleaseAllNodesBuffer()
{
    RS_OPTIONAL_TRACE_BEGIN("RSMainThread::ReleaseAllNodesBuffer");
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([this](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
        if (surfaceNode == nullptr) {
            return;
        }
        // surfaceNode's buffer will be released in hardware thread if last frame enables hardware composer
        if (surfaceNode->IsHardwareEnabledType()) {
            if (surfaceNode->IsLastFrameHardwareEnabled()) {
                if (!surfaceNode->IsCurrentFrameHardwareEnabled()) {
                    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*surfaceNode);
                    auto preBuffer = surfaceHandler.GetPreBuffer();
                    if (preBuffer.buffer != nullptr) {
                        auto releaseTask = [buffer = preBuffer.buffer, consumer = surfaceHandler.GetConsumer(),
                            fence = preBuffer.releaseFence]() mutable {
                            auto ret = consumer->ReleaseBuffer(buffer, fence);
                            if (ret != OHOS::SURFACE_ERROR_OK) {
                                RS_LOGW("surfaceHandler ReleaseBuffer failed(ret: %d)!", ret);
                            }
                        };
                        preBuffer.Reset();
                        RSHardwareThread::Instance().PostTask(releaseTask);
                    }
                }
                surfaceNode->ResetCurrentFrameHardwareEnabledState();
                return;
            }
            surfaceNode->ResetCurrentFrameHardwareEnabledState();
        }
        // To avoid traverse surfaceNodeMap again, destroy cold start thread here
        if ((!surfaceNode->IsOnTheTree() || !surfaceNode->ShouldPaint()) &&
            RSColdStartManager::Instance().IsColdStartThreadRunning(surfaceNode->GetId())) {
            if (RSColdStartManager::Instance().IsColdStartThreadIdle(surfaceNode->GetId())) {
                surfaceNode->ClearCachedImage();
                RSColdStartManager::Instance().StopColdStartThread(surfaceNode->GetId());
            }
        }
        RSBaseRenderUtil::ReleaseBuffer(static_cast<RSSurfaceHandler&>(*surfaceNode));
    });
#ifdef RS_ENABLE_GL
    if (NeedReleaseGpuResource(nodeMap)) {
        PostTask([this]() {
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
            MemoryManager::ReleaseUnlockGpuResource(GetRenderEngine()->GetDrawingContext()->GetDrawingContext());
#else
            MemoryManager::ReleaseUnlockGpuResource(GetRenderEngine()->GetRenderContext()->GetGrContext());
#endif
#else
            MemoryManager::ReleaseUnlockGpuResource(GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
#endif
        });
    }
#endif
    RS_OPTIONAL_TRACE_END();
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

bool RSMainThread::WaitUntilDisplayNodeBufferReleased(RSDisplayRenderNode& node)
{
    std::unique_lock<std::mutex> lock(displayNodeBufferReleasedMutex_);
    displayNodeBufferReleased_ = false; // prevent spurious wakeup of condition variable
    if (node.GetConsumer()->QueryIfBufferAvailable()) {
        return true;
    }
    return displayNodeBufferReleasedCond_.wait_until(lock, std::chrono::system_clock::now() +
        std::chrono::milliseconds(WAIT_FOR_RELEASED_BUFFER_TIMEOUT), [this]() { return displayNodeBufferReleased_; });
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
    RS_OPTIONAL_TRACE_BEGIN("RSMainThread::WaitUntilUnmarshallingTaskFinished");
    std::unique_lock<std::mutex> lock(unmarshalMutex_);
    unmarshalTaskCond_.wait(lock, [this]() { return unmarshalFinishedCount_ > 0; });
    --unmarshalFinishedCount_;
    RS_OPTIONAL_TRACE_END();
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

void RSMainThread::ProcessHgmFrameRate(FrameRateRangeData data, uint64_t timestamp)
{
    // 0.[Planning]: The HGM logic here will be processed using sub-threads in the future.

    frameRateMgr_->Reset();
    // 1.[Planning]: Check and processing software vsync frame rate switching task for RS.

    // 2.Decision-making process for current frame.
    frameRateMgr_->UniProcessData(data);

    // 3.[Planning]: Post app and rs switch software vsync rate task.
}

void RSMainThread::UniRender(std::shared_ptr<RSBaseRenderNode> rootNode)
{
    UpdateUIFirstSwitch();
    auto uniVisitor = std::make_shared<RSUniRenderVisitor>();
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    uniVisitor->SetDrivenRenderFlag(hasDrivenNodeOnUniTree_, hasDrivenNodeMarkRender_);
#endif
    uniVisitor->SetHardwareEnabledNodes(hardwareEnabledNodes_);
    uniVisitor->SetAppWindowNum(appWindowNum_);
    uniVisitor->SetProcessorRenderEngine(GetRenderEngine());

    if (isHardwareForcedDisabled_) {
        uniVisitor->MarkHardwareForcedDisabled();
        doDirectComposition_ = false;
    }
    bool needTraverseNodeTree = true;
    if (doDirectComposition_ && !isDirty_ && !isAccessibilityConfigChanged_ && !isCachedSurfaceUpdated_) {
        if (isHardwareEnabledBufferUpdated_) {
            needTraverseNodeTree = !uniVisitor->DoDirectComposition(rootNode);
        } else {
            RS_LOGI("RSMainThread::Render nothing to update");
            for (auto& node: hardwareEnabledNodes_) {
                if (!node->IsHardwareForcedDisabled()) {
                    node->MarkCurrentFrameHardwareEnabled();
                }
            }
            return;
        }
    }
    isCachedSurfaceUpdated_ = false;
    if (needTraverseNodeTree) {
        uniVisitor->SetAnimateState(doWindowAnimate_);
        uniVisitor->SetDirtyFlag(isDirty_ || isAccessibilityConfigChanged_);
        isAccessibilityConfigChanged_ = false;
        uniVisitor->SetFocusedNodeId(focusNodeId_);
        rootNode->Prepare(uniVisitor);
        ProcessHgmFrameRate(uniVisitor->GetFrameRateRangeData(), timestamp_);
        CalcOcclusion();
        bool doParallelComposition = RSInnovation::GetParallelCompositionEnabled(isUniRender_);
        if (doParallelComposition && rootNode->GetChildrenCount() > 1) {
            RS_LOGD("RSMainThread::Render multi-threads parallel composition begin.");
            doParallelComposition = uniVisitor->ParallelComposition(rootNode);
            if (doParallelComposition) {
                RS_LOGD("RSMainThread::Render multi-threads parallel composition end.");
                isDirty_ = false;
                PerfForBlurIfNeeded();
                return;
            }
        }
        if (IsUIFirstOn()) {
            auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(
                rootNode->GetSortedChildren().front());
            std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
            std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
            RSUniRenderUtil::AssignWindowNodes(displayNode, mainThreadNodes, subThreadNodes, focusNodeId_, deviceType_);
            const auto& nodeMap = context_->GetNodeMap();
            RSUniRenderUtil::ClearSurfaceIfNeed(nodeMap, displayNode, oldDisplayChildren_);
            uniVisitor->DrawSurfaceLayer(displayNode, subThreadNodes);
            RSUniRenderUtil::CacheSubThreadNodes(subThreadNodes_, subThreadNodes);
        }
        rootNode->Process(uniVisitor);
    }
    isDirty_ = false;
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

    if (isUniRender_) {
        UniRender(rootNode);
    } else {
        auto rsVisitor = std::make_shared<RSRenderServiceVisitor>();
        rsVisitor->SetAnimateState(doWindowAnimate_);
        rootNode->Prepare(rsVisitor);
        CalcOcclusion();

        bool doParallelComposition = false;
        if (!rsVisitor->ShouldForceSerial() && RSInnovation::GetParallelCompositionEnabled(isUniRender_)) {
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

bool RSMainThread::CheckSurfaceNeedProcess(OcclusionRectISet& occlusionSurfaces,
    std::shared_ptr<RSSurfaceRenderNode> curSurface)
{
    bool needProcess = false;
    if (curSurface->IsFocusedNode(focusNodeId_)) {
        needProcess = true;
        if (!curSurface->HasContainerWindow() && !curSurface->IsTransparent() &&
            curSurface->GetName().find("hisearch") == std::string::npos) {
            occlusionSurfaces.insert(curSurface->GetDstRect());
        }
    } else {
        size_t beforeSize = occlusionSurfaces.size();
        occlusionSurfaces.insert(curSurface->GetDstRect());
        bool insertSuccess = occlusionSurfaces.size() > beforeSize ? true : false;
        if (insertSuccess) {
            needProcess = true;
            if (curSurface->IsTransparent() || curSurface->GetName().find("hisearch") != std::string::npos) {
                auto iter = std::find_if(occlusionSurfaces.begin(), occlusionSurfaces.end(),
                    [&curSurface](RectI r) -> bool {return r == curSurface->GetDstRect();});
                if (iter != occlusionSurfaces.end()) {
                    occlusionSurfaces.erase(iter);
                }
            }
        }
    }

    if (needProcess) {
        CheckIfNodeIsBundle(curSurface);
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
        if (curSurface == nullptr || curSurface->GetDstRect().IsEmpty() || curSurface->IsLeashWindow()) {
            continue;
        }
        Occlusion::Rect occlusionRect;
        if (isUniRender_) {
            // In UniRender, CalcOcclusion should consider the shadow area of window
            occlusionRect = Occlusion::Rect {curSurface->GetOldDirtyInSurface()};
        } else {
            occlusionRect = Occlusion::Rect {curSurface->GetDstRect()};
        }
        RS_LOGD("RSMainThread::CalcOcclusionImplementation name: %s id: %llu rect: %s",
            curSurface->GetName().c_str(), curSurface->GetId(), occlusionRect.GetRectInfo().c_str());
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
                if (curSurface->GetName().find("hisearch") == std::string::npos) {
                    // When a surfacenode is in animation (i.e. 3d animation), its dstrect cannot be trusted, we treated
                    // it as a full transparent layer.
                    if (parentPtr != nullptr && parentPtr->IsInstanceOf<RSSurfaceRenderNode>()) {
                        auto surfaceParentPtr = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parentPtr);
                        // leashwindow scaling is also means animation
                        if (surfaceParentPtr->IsLeashWindow() && surfaceParentPtr->IsScale()) {
                            curSurface->SetAnimateState();
                        }
                    }
                    if (!(curSurface->GetAnimateState())) {
                        accumulatedRegion.OrSelf(curSurface->GetOpaqueRegion());
                    } else {
                        curSurface->ResetAnimateState();
                    }
                }
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
    RS_LOGD("RSMainThread::CalcOcclusion animate:%d isUniRender:%d", doWindowAnimate_.load(), isUniRender_);
    if (doWindowAnimate_ && !isUniRender_) {
        return;
    }
    const std::shared_ptr<RSBaseRenderNode> node = context_->GetGlobalRootRenderNode();
    if (node == nullptr) {
        RS_LOGE("RSMainThread::CalcOcclusion GetGlobalRootRenderNode fail");
        return;
    }
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    if (node->GetChildrenCount()== 1) {
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(
            node->GetSortedChildren().front());
        if (displayNode) {
            curAllSurfaces = displayNode->GetCurAllSurfaces();
        }
    } else {
        node->CollectSurface(node, curAllSurfaces, isUniRender_, false);
    }
    // Judge whether it is dirty
    // Surface cnt changed or surface DstRectChanged or surface ZorderChanged
    bool winDirty = (lastSurfaceCnt_ != curAllSurfaces.size() || isDirty_ ||
        lastFocusNodeId_ != focusNodeId_);
    lastSurfaceCnt_ = curAllSurfaces.size();
    lastFocusNodeId_ = focusNodeId_;
    if (!winDirty) {
        for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
            auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
            if (surface == nullptr || surface->IsLeashWindow()) {
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
        std::lock_guard<std::mutex> lock(occlusionMutex_);
        for (auto it = occlusionListeners_.begin(); it != occlusionListeners_.end(); it++) {
            if (it->second) {
                RS_LOGD("RSMainThread::CallbackToWMS curVisVec size:%u", curVisVec.size());
                it->second->OnOcclusionVisibleChanged(std::make_shared<RSOcclusionData>(curVisVec));
            }
        }
    }
    lastVisVec_.clear();
    std::copy(curVisVec.begin(), curVisVec.end(), std::back_inserter(lastVisVec_));
}

void RSMainThread::RequestNextVSync()
{
    RS_OPTIONAL_TRACE_FUNC_BEGIN();
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = [this](uint64_t timestamp, void* data) { OnVsync(timestamp, data); },
    };
    if (receiver_ != nullptr) {
        requestNextVsyncNum_++;
        if (requestNextVsyncNum_ > REQUEST_VSYNC_NUMBER_LIMIT) {
            RS_LOGW("RSMainThread::RequestNextVSync too many times:%d", requestNextVsyncNum_);
        }
        receiver_->RequestNextVSync(fcb);
    }
    RS_OPTIONAL_TRACE_FUNC_END();
}

void RSMainThread::OnVsync(uint64_t timestamp, void* data)
{
    RSJankStats::GetInstance().SetStartTime();
    timestamp_ = timestamp;
    curTime_ = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    requestNextVsyncNum_ = 0;
    frameCount_++;
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
    RSJankStats::GetInstance().SetEndTime();
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
        if (cacheCmdSkippedInfo_.count(ExtractPid(node->GetId())) > 0) {
            RS_LOGD("RSMainThread::Animate skip the cached node");
            return false;
        }
        if (node->IsOnTheTree()) {
            AddActiveNodeId(ExtractPid(node->GetId()), node->GetId());
        }
        auto [hasRunningAnimation, nodeNeedRequestNextVsync] = node->Animate(timestamp);
        if (!hasRunningAnimation) {
            RS_LOGD("RSMainThread::Animate removing finished animating node %" PRIu64, node->GetId());
        }
        // request vsync if: 1. node has running animation, or 2. transition animation just ended
        needRequestNextVsync = needRequestNextVsync || nodeNeedRequestNextVsync || (node.use_count() == 1);
        if (node->template IsInstanceOf<RSSurfaceRenderNode>() && hasRunningAnimation) {
            if (isUniRender_) {
                auto surfacenode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
                surfacenode->SetAnimateState();
            }
            curWinAnim = true;
        }
        return !hasRunningAnimation;
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
    PerfAfterAnim(needRequestNextVsync);
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
    if (isUniRender_) {
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

void RSMainThread::RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(occlusionMutex_);
    occlusionListeners_[pid] = callback;
}

void RSMainThread::UnRegisterOcclusionChangeCallback(pid_t pid)
{
    std::lock_guard<std::mutex> lock(occlusionMutex_);
    occlusionListeners_.erase(pid);
}

void RSMainThread::SendCommands()
{
    RS_OPTIONAL_TRACE_FUNC_BEGIN();
    RsFrameReport& fr = RsFrameReport::GetInstance();
    if (fr.GetEnable()) {
        fr.SendCommandsStart();
        fr.RenderEnd();
    }
    if (!RSMessageProcessor::Instance().HasTransaction()) {
        RS_OPTIONAL_TRACE_FUNC_END();
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
    RS_OPTIONAL_TRACE_FUNC_END();
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
    return true;
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
        RS_LOGD("RSMainThread: clear cpu cache pid:%d", remotePid);
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
        auto grContext = GetRenderEngine()->GetDrawingContext()->GetDrawingContext();
#else
        auto grContext = GetRenderEngine()->GetRenderContext()->GetGrContext();
#endif
        grContext->flush();
        SkGraphics::PurgeAllCaches(); // clear cpu cache
        if (!IsResidentProcess(remotePid)) {
            ReleaseExitSurfaceNodeAllGpuResource(grContext);
        } else {
            RS_LOGW("this pid:%d is resident process, no need release gpu resource", remotePid);
        }
#ifdef NEW_SKIA
        grContext->flushAndSubmit(true);
#else
        grContext->flush(kSyncCpu_GrFlushFlag, 0, nullptr);
#endif
#else
        auto gpuContext = GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
        if (gpuContext == nullptr) {
            return;
        }
        gpuContext->Flush();
        if (!IsResidentProcess(remotePid)) {
            ReleaseExitSurfaceNodeAllGpuResource(gpuContext);
        } else {
            RS_LOGW("this pid:%d is resident process, no need release gpu resource", remotePid);
        }
#endif // USE_ROSEN_DRAWING
        lastCleanCacheTimestamp_ = timestamp_;
#endif
    }
}

bool RSMainThread::IsResidentProcess(pid_t pid)
{
    const auto& nodeMap = context_->GetNodeMap();
    return pid == ExtractPid(nodeMap.GetEntryViewNodeId()) || pid == ExtractPid(nodeMap.GetScreenLockWindowNodeId()) ||
        pid == ExtractPid(nodeMap.GetWallPaperViewNodeId());
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
void RSMainThread::ReleaseExitSurfaceNodeAllGpuResource(GrDirectContext* grContext)
#else
void RSMainThread::ReleaseExitSurfaceNodeAllGpuResource(GrContext* grContext)
#endif
#else
void RSMainThread::ReleaseExitSurfaceNodeAllGpuResource(Drawing::GPUContext* gpuContext)
#endif
{
    switch (RSSystemProperties::GetReleaseGpuResourceEnabled()) {
        case ReleaseGpuResourceType::WINDOW_HIDDEN:
        case ReleaseGpuResourceType::WINDOW_HIDDEN_AND_LAUCHER:
#ifndef USE_ROSEN_DRAWING
            MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(grContext);
#else
            MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(gpuContext);
#endif
            break;
        default:
            break;
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
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
    auto grContext = GetRenderEngine()->GetDrawingContext()->GetDrawingContext();
#else
    auto grContext = GetRenderEngine()->GetRenderContext()->GetGrContext();
#endif
    if (type.empty()) {
        grContext->flush();
        SkGraphics::PurgeAllCaches();
        grContext->freeGpuResources();
        grContext->purgeUnlockedResources(true);
#ifdef NEW_RENDER_CONTEXT
        MemoryHandler::ClearShader();
#else
        std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
        rendercontext->CleanAllShaderCache();
#endif
#ifdef NEW_SKIA
        grContext->flushAndSubmit(true);
#else
        grContext->flush(kSyncCpu_GrFlushFlag, 0, nullptr);
#endif
    } else if (type == "cpu") {
        grContext->flush();
        SkGraphics::PurgeAllCaches();
#ifdef NEW_SKIA
        grContext->flushAndSubmit(true);
#else
        grContext->flush(kSyncCpu_GrFlushFlag, 0, nullptr);
#endif
    } else if (type == "gpu") {
        grContext->flush();
        grContext->freeGpuResources();
#ifdef NEW_SKIA
        grContext->flushAndSubmit(true);
#else
        grContext->flush(kSyncCpu_GrFlushFlag, 0, nullptr);
#endif
    } else if (type == "uihidden") {
        grContext->flush();
        grContext->purgeUnlockedResources(true);
#ifdef NEW_SKIA
        grContext->flushAndSubmit(true);
#else
        grContext->flush(kSyncCpu_GrFlushFlag, 0, nullptr);
#endif
    } else if (type == "shader") {
#ifdef NEW_RENDER_CONTEXT
        MemoryHandler::ClearShader();
#else
        std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
        rendercontext->CleanAllShaderCache();
#endif
    } else if (type == "flushcache") {
        int ret = mallopt(M_FLUSH_THREAD_CACHE, 0);
        dumpString.append("flushcache " + std::to_string(ret) + "\n");
    } else {
        uint32_t pid = static_cast<uint32_t>(std::stoll(type));
        GrGpuResourceTag tag(pid, 0, 0, 0);
        MemoryManager::ReleaseAllGpuResource(grContext, tag);
    }
    dumpString.append("trimMem: " + type + "\n");
#endif
#else
    dumpString.append("No GPU in this device");
#endif
}

void RSMainThread::DumpMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString,
    std::string& type, int pid)
{
#ifdef RS_ENABLE_GL
    DfxString log;
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
    auto grContext = GetRenderEngine()->GetDrawingContext()->GetDrawingContext();
#else
    auto grContext = GetRenderEngine()->GetRenderContext()->GetGrContext();
#endif
    if (pid != 0) {
        MemoryManager::DumpPidMemory(log, pid, grContext);
    } else {
        MemoryManager::DumpMemoryUsage(log, grContext, type);
    }
#else
    auto gpuContext = GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
    if (gpuContext != nullptr) {
        if (pid != 0) {
            MemoryManager::DumpPidMemory(log, pid, gpuContext);
        } else {
            MemoryManager::DumpMemoryUsage(log, gpuContext, type);
        }
    }
#endif
    if (type.empty() || type == MEM_GPU_TYPE) {
        auto subThreadManager = RSSubThreadManager::Instance();
        if (subThreadManager) {
            subThreadManager->DumpMem(log);
        }
    }
    dumpString.append("dumpMem: " + type + "\n");
    dumpString.append(log.GetString());
#else
    dumpString.append("No GPU in this device");
#endif
}

void RSMainThread::CountMem(int pid, MemoryGraphic& mem)
{
#ifdef RS_ENABLE_GL
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
    mem = MemoryManager::CountPidMemory(pid, GetRenderEngine()->GetDrawingContext()->GetDrawingContext());
#else
    mem = MemoryManager::CountPidMemory(pid, GetRenderEngine()->GetRenderContext()->GetGrContext());
#endif
#else
    mem = MemoryManager::CountPidMemory(pid, GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
#endif
#endif
}

void RSMainThread::CountMem(std::vector<MemoryGraphic>& mems)
{
#ifdef RS_ENABLE_GL
    if (!context_) {
        RS_LOGE("RSMainThread::CountMem Context is nullptr");
        return;
    }
    const auto& nodeMap = context_->GetNodeMap();
    std::vector<pid_t> pids;
    nodeMap.TraverseSurfaceNodes([&pids] (const std::shared_ptr<RSSurfaceRenderNode>& node) {
        auto pid = ExtractPid(node->GetId());
        if (std::find(pids.begin(), pids.end(), pid) == pids.end()) {
            pids.emplace_back(pid);
        }
    });
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
    MemoryManager::CountMemory(pids, GetRenderEngine()->GetDrawingContext()->GetDrawingContext(), mems);
#else
    MemoryManager::CountMemory(pids, GetRenderEngine()->GetRenderContext()->GetGrContext(), mems);
#endif
#else
    MemoryManager::CountMemory(pids, GetRenderEngine()->GetRenderContext()->GetDrGPUContext(), mems);
#endif
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

void RSMainThread::SetAccessibilityConfigChanged()
{
    isAccessibilityConfigChanged_ = true;
}

void RSMainThread::PerfAfterAnim(bool needRequestNextVsync)
{
    if (!isUniRender_) {
        return;
    }
    if (needRequestNextVsync && timestamp_ - prePerfTimestamp_ > PERF_PERIOD) {
        RS_LOGD("RSMainThread:: soc perf to render_service_animation");
        prePerfTimestamp_ = timestamp_;
    } else if (!needRequestNextVsync && prePerfTimestamp_) {
        RS_LOGD("RSMainThread:: soc perf off render_service_animation");
        prePerfTimestamp_ = 0;
    }
}

void RSMainThread::ForceRefreshForUni()
{
    if (isUniRender_) {
        PostTask([=]() {
            MergeToEffectiveTransactionDataMap(cachedTransactionDataMap_);
            RSUnmarshalThread::Instance().PostTask(unmarshalBarrierTask_);
            auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            timestamp_ = timestamp_ + (now - curTime_);
            curTime_ = now;
            RS_TRACE_NAME("RSMainThread::ForceRefreshForUni timestamp:" + std::to_string(timestamp_));
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

void RSMainThread::RenderFrameStart()
{
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().RenderStart();
    }
    RenderFrameTrace::GetInstance().RenderStartFrameTrace(RS_INTERVAL_NAME);
}

void RSMainThread::SetAppWindowNum(uint32_t num)
{
    appWindowNum_ = num;
}

void RSMainThread::AddActiveNodeId(pid_t pid, NodeId id)
{
    if (id == 0) {
        activeAppsInProcess_[pid].emplace(INVALID_NODEID);
        return;
    }
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(id);
    if (node == nullptr) {
        return;
    }
    // root nodeid should be updated before check
    if (!node->IsOnTheTree()) {
        return;
    }
    if (auto parentPtr = node->GetParent().lock()) {
        auto rootNodeId = node->GetInstanceRootNodeId();
        activeAppsInProcess_[pid].emplace(rootNodeId);
        activeProcessNodeIds_[rootNodeId].emplace(id);
    }
}

void RSMainThread::ResetHardwareEnabledState()
{
    isHardwareForcedDisabled_ = !RSSystemProperties::GetHardwareComposerEnabled();
    doDirectComposition_ = !isHardwareForcedDisabled_;
    isHardwareEnabledBufferUpdated_ = false;
    hardwareEnabledNodes_.clear();
}

void RSMainThread::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    std::lock_guard<std::mutex> lock(watermarkMutex_);
    isShow_ = isShow;
    if (isShow_) {
#ifndef USE_ROSEN_DRAWING
        watermarkImg_ = RSPixelMapUtil::ExtractSkImage(std::move(watermarkImg));
#else
        watermarkImg_ = RSPixelMapUtil::ExtractDrawingImage(std::move(watermarkImg));
#endif
    } else {
        watermarkImg_ = nullptr;
    }
    SetDirtyFlag();
    RequestNextVSync();
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImage> RSMainThread::GetWatermarkImg()
#else
std::shared_ptr<Drawing::Image> RSMainThread::GetWatermarkImg()
#endif
{
    return watermarkImg_;
}

bool RSMainThread::GetWatermarkFlag()
{
    return isShow_;
}

bool RSMainThread::IsSingleDisplay()
{
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        RS_LOGE("RSMainThread::IsSingleDisplay GetGlobalRootRenderNode fail");
        return false;
    }
    return rootNode->GetChildrenCount() == 1;
}

const uint32_t UIFIRST_MINIMUM_NODE_NUMBER = 20;
void RSMainThread::UpdateUIFirstSwitch()
{
    if (deviceType_ == DeviceType::PHONE) {
        isUiFirstOn_ = (RSSystemProperties::GetUIFirstEnabled() && IsSingleDisplay());
        return;
    }
    isUiFirstOn_ = false;
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode && IsSingleDisplay()) {
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(
            rootNode->GetSortedChildren().front());
        if (displayNode) {
            uint32_t childrenCount = 0;
            if (Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
                std::vector<RSBaseRenderNode::SharedPtr> curAllSurfacesVec;
                displayNode->CollectSurface(displayNode, curAllSurfacesVec, true, true);
                childrenCount = curAllSurfacesVec.size();
            } else {
                childrenCount = displayNode->GetChildrenCount();
            }
            isUiFirstOn_ = RSSystemProperties::GetUIFirstEnabled() &&
                (childrenCount >= UIFIRST_MINIMUM_NODE_NUMBER);
        }
    }
}

bool RSMainThread::IsUIFirstOn() const
{
    return isUiFirstOn_;
}
} // namespace Rosen
} // namespace OHOS
