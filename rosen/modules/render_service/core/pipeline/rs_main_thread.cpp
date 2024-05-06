/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#define EGL_EGLEXT_PROTOTYPES

#include "pipeline/rs_main_thread.h"

#include <algorithm>
#include <cstdint>
#include <list>
#include <malloc.h>
#include <securec.h>
#include <stdint.h>
#include <string>
#include <unistd.h>

#include "benchmarks/file_utils.h"
#include "delegate/rs_functional_delegate.h"
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "include/core/SkGraphics.h"
#include "include/gpu/GrDirectContext.h"
#include "mem_mgr_client.h"
#include "render_frame_trace.h"
#include "rs_frame_report.h"
#include "rs_profiler.h"
#include "rs_trace.h"
#include "scene_board_judgement.h"
#include "vsync_iconnection_token.h"
#include "xcollie/watchdog.h"

#include "animation/rs_animation_fraction.h"
#include "command/rs_message_processor.h"
#include "common/rs_background_thread.h"
#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "drawable/rs_property_drawable_utils.h"
#include "luminance/rs_luminance_control.h"
#include "memory/rs_memory_graphic.h"
#include "memory/rs_memory_manager.h"
#include "memory/rs_memory_track.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/round_corner_display/rs_rcd_render_manager.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_divided_render_util.h"
#include "pipeline/rs_hardware_thread.h"
#include "pipeline/rs_occlusion_config.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_render_engine.h"
#include "pipeline/rs_render_service_visitor.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "pipeline/rs_unmarshal_thread.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_uifirst_manager.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_innovation.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/drawing/rs_vsync_client.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "platform/ohos/rs_jank_stats.h"
#include "property/rs_point_light_manager.h"
#include "property/rs_properties_painter.h"
#include "property/rs_property_trace.h"
#include "render/rs_pixel_map_util.h"
#include "render/rs_typeface_cache.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_transaction_proxy.h"

#ifdef RS_ENABLE_GL
#include "GLES3/gl3.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#endif

#ifdef RS_ENABLE_PARALLEL_UPLOAD
#include "rs_upload_resource_thread.h"
#endif

#ifdef NEW_RENDER_CONTEXT
#include "render_context/memory_handler.h"
#endif

#if defined(ACCESSIBILITY_ENABLE)
#include "accessibility_config.h"
#endif

#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif

#if defined(RS_ENABLE_CHIPSET_VSYNC)
#include "chipset_vsync.h"
#endif
#ifdef RES_SCHED_ENABLE
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include <iservice_registry.h>
#endif

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
constexpr int32_t VISIBLEAREARATIO_FORQOS = 3;
constexpr uint64_t PERF_PERIOD = 250000000;
constexpr uint64_t CLEAN_CACHE_FREQ = 60;
constexpr uint64_t SKIP_COMMAND_FREQ_LIMIT = 30;
constexpr uint64_t PERF_PERIOD_BLUR = 1000000000;
constexpr uint64_t PERF_PERIOD_BLUR_TIMEOUT = 80000000;
constexpr uint64_t MAX_DYNAMIC_STATUS_TIME = 5000000000;
constexpr uint64_t MAX_SYSTEM_SCENE_STATUS_TIME = 800000000;
constexpr uint64_t PERF_PERIOD_MULTI_WINDOW = 80000000;
constexpr uint32_t MULTI_WINDOW_PERF_START_NUM = 2;
constexpr uint32_t MULTI_WINDOW_PERF_END_NUM = 4;
constexpr uint32_t TIME_OF_EIGHT_FRAMES = 8000;
constexpr uint32_t TIME_OF_THE_FRAMES = 1000;
constexpr uint32_t WAIT_FOR_RELEASED_BUFFER_TIMEOUT = 3000;
constexpr uint32_t WAIT_FOR_HARDWARE_THREAD_TASK_TIMEOUT = 3000;
constexpr uint32_t WAIT_FOR_SURFACE_CAPTURE_PROCESS_TIMEOUT = 1000;
constexpr uint32_t WATCHDOG_TIMEVAL = 5000;
constexpr uint32_t HARDWARE_THREAD_TASK_NUM = 3;
constexpr int32_t SIMI_VISIBLE_RATE = 2;
constexpr int32_t DEFAULT_RATE = 1;
constexpr int32_t INVISBLE_WINDOW_RATE = 10;
constexpr int32_t SYSTEM_ANIMATED_SECNES_RATE = 2;
constexpr uint32_t WAIT_FOR_MEM_MGR_SERVICE = 100;
constexpr uint32_t CAL_NODE_PREFERRED_FPS_LIMIT = 50;
constexpr uint32_t EVENT_SET_HARDWARE_UTIL = 100004;
constexpr const char* WALLPAPER_VIEW = "WallpaperView";
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
constexpr const char* MEM_MGR = "MemMgr";
constexpr const char* DESKTOP_NAME_FOR_ROTATION = "SCBDesktop";
#ifdef RS_ENABLE_GL
constexpr size_t DEFAULT_SKIA_CACHE_SIZE        = 96 * (1 << 20);
constexpr int DEFAULT_SKIA_CACHE_COUNT          = 2 * (1 << 12);
#endif
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
constexpr const char* MEM_GPU_TYPE = "gpu";
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
    RS_LOGD("RSMainThread::soc perf info [%{public}d %{public}d]", perfRequestCode, onOffTag);
#endif
}
std::string g_dumpStr = "";
std::mutex g_dumpMutex;
std::condition_variable g_dumpCond_;
}

#if defined(ACCESSIBILITY_ENABLE)
class AccessibilityObserver : public AccessibilityConfigObserver {
public:
    AccessibilityObserver() = default;
    void OnConfigChanged(const CONFIG_ID id, const ConfigValue &value) override
    {
        RS_LOGD("AccessibilityObserver OnConfigChanged configId: %{public}d", id);
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
        } else if (id == CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT) {
            RSBaseRenderEngine::SetHighContrast(value.highContrastText);
        } else {
            RS_LOGW("AccessibilityObserver configId: %{public}d is not supported yet.", id);
        }
        RSMainThread::Instance()->PostTask([]() {
            RSMainThread::Instance()->SetAccessibilityConfigChanged();
            RSMainThread::Instance()->RequestNextVSync();
        });
    }
};
#endif
static inline void WaitUntilUploadTextureTaskFinished(bool isUniRender)
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_PARALLEL_UPLOAD)
#if defined(NEW_SKIA) && defined(RS_ENABLE_UNI_RENDER)
    if (isUniRender) {
        RSUploadResourceThread::Instance().OnProcessBegin();
    }
    return;
#endif
#endif
}

RSMainThread* RSMainThread::Instance()
{
    static RSMainThread instance;
    RSAnimationFraction::Init();
    return &instance;
}

RSMainThread::RSMainThread() : mainThreadId_(std::this_thread::get_id()),
    rsParallelType_(RSSystemParameters::GetRsParallelType())
{
    context_ = std::make_shared<RSContext>();
    context_->Initialize();
}

RSMainThread::~RSMainThread() noexcept
{
    RemoveRSEventDetector();
    RSInnovation::CloseInnovationSo();
    if (rsAppStateListener_) {
        Memory::MemMgrClient::GetInstance().UnsubscribeAppState(*rsAppStateListener_);
    }
}

void RSMainThread::Init()
{
    mainLoop_ = [&]() {
        RS_PROFILER_ON_FRAME_BEGIN();
        if (isUniRender_ && !renderThreadParams_) {
            // fill the params, and sync to render thread later
            renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
        }
        RenderFrameStart(timestamp_);
#if defined(RS_ENABLE_UNI_RENDER)
        WaitUntilSurfaceCapProcFinished();
#endif
        PerfMultiWindow();
        SetRSEventDetectorLoopStartTag();
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSMainThread::DoComposition: " + std::to_string(curTime_));
        ConsumeAndUpdateAllNodes();
        WaitUntilUnmarshallingTaskFinished();
        ProcessCommand();
        Animate(timestamp_);
        CollectInfoForHardwareComposer();
        RSUifirstManager::Instance().PrepareCurrentFrameEvent();
        ProcessHgmFrameRate(timestamp_);
        RS_PROFILER_ON_RENDER_BEGIN();
        // may mark rsnotrendering
        Render(); // now render is traverse tree to prepare
        RS_PROFILER_ON_RENDER_END();
        if (isUniRender_ && !doDirectComposition_) {
            renderThreadParams_->SetContext(context_);
            drawFrame_.SetRenderThreadParams(renderThreadParams_);
            drawFrame_.PostAndWait();
        }
        if (isUniRender_ && doDirectComposition_) {
            UpdateDisplayNodeScreenId();
            if (!markRenderFlag_) {
                SetFrameIsRender(true);
            }
            markRenderFlag_ = false;
        }

        InformHgmNodeInfo();
        if (!isUniRender_) {
            ReleaseAllNodesBuffer();
        }
        SendCommands();
        {
            std::lock_guard<std::mutex> lock(context_->activeNodesInRootMutex_);
            context_->activeNodesInRoot_.clear();
        }
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        SetRSEventDetectorLoopFinishTag();
        rsEventManager_.UpdateParam();
        SKResourceManager::Instance().ReleaseResource();
        // release node memory
        if (RSRenderNodeGC::Instance().GetNodeSize() > 0) {
            RSBackgroundThread::Instance().PostTask([] () {
                RS_TRACE_NAME("ReleaseRSRenderNodeMemory");
                RSRenderNodeGC::Instance().ReleaseNodeMemory();
            });
        }
#ifdef RS_ENABLE_PARALLEL_UPLOAD
        RSUploadResourceThread::Instance().OnRenderEnd();
#endif
        RSTypefaceCache::Instance().HandleDelayDestroyQueue();
#if defined(RS_ENABLE_CHIPSET_VSYNC)
        ConnectChipsetVsyncSer();
#endif
        RS_PROFILER_ON_FRAME_END();
    };
    static std::function<void (std::shared_ptr<Drawing::Image> image)> holdDrawingImagefunc =
        [] (std::shared_ptr<Drawing::Image> image) -> void {
            if (image) {
                SKResourceManager::Instance().HoldResource(image);
            }
        };
    Drawing::DrawOpItem::SetBaseCallback(holdDrawingImagefunc);
    static std::function<std::shared_ptr<Drawing::Typeface> (uint64_t)> customTypefaceQueryfunc =
        [] (uint64_t globalUniqueId) -> std::shared_ptr<Drawing::Typeface> {
            return RSTypefaceCache::Instance().GetDrawingTypefaceCache(globalUniqueId);
        };
    Drawing::DrawOpItem::SetTypefaceQueryCallBack(customTypefaceQueryfunc);

    isUniRender_ = RSUniRenderJudgement::IsUniRender();
    SetDeviceType();
    qosPidCal_ = deviceType_ == DeviceType::PC;
    isFoldScreenDevice_ = system::GetParameter("const.window.foldscreen.type", "") == "true";
    auto taskDispatchFunc = [](const RSTaskDispatcher::RSTask& task, bool isSyncTask = false) {
        RSMainThread::Instance()->PostTask(task);
    };
    context_->SetTaskRunner(taskDispatchFunc);
    if (isUniRender_) {
        auto rtTaskDispatchFunc = [](const RSTaskDispatcher::RSTask& task) {
            RSUniRenderThread::Instance().PostRTTask(task);
        };
        context_->SetRTTaskRunner(rtTaskDispatchFunc);
    }
    context_->SetVsyncRequestFunc([]() {
        RSMainThread::Instance()->RequestNextVSync();
        RSMainThread::Instance()->SetDirtyFlag();
    });
    RSTaskDispatcher::GetInstance().RegisterTaskDispatchFunc(gettid(), taskDispatchFunc);
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
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread("RenderService", handler_, WATCHDOG_TIMEVAL);
    if (ret != 0) {
        RS_LOGW("Add watchdog thread failed");
    }
#ifdef RES_SCHED_ENABLE
    SubScribeSystemAbility();
#endif
    InitRSEventDetector();
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    sptr<VSyncConnection> conn = new VSyncConnection(rsVSyncDistributor_, "rs", token->AsObject());
    rsFrameRateLinker_ = std::make_shared<RSRenderFrameRateLinker>();
    conn->id_ = rsFrameRateLinker_->GetId();
    rsVSyncDistributor_->AddConnection(conn);
    receiver_ = std::make_shared<VSyncReceiver>(conn, token->AsObject(), handler_, "rs");
    receiver_->Init();
    if (!isUniRender_) {
        renderEngine_ = std::make_shared<RSRenderEngine>();
        renderEngine_->Init();
    }
#ifdef RS_ENABLE_GL
    /* move to render thread ? */
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        int cacheLimitsTimes = 3;
#ifdef NEW_RENDER_CONTEXT
        auto gpuContext = isUniRender_? uniRenderEngine_->GetDrawingContext()->GetDrawingContext() :
            renderEngine_->GetDrawingContext()->GetDrawingContext();
#else
        auto gpuContext = isUniRender_? GetRenderEngine()->GetRenderContext()->GetDrGPUContext() :
            renderEngine_->GetRenderContext()->GetDrGPUContext();
#endif
        if (gpuContext == nullptr) {
            RS_LOGE("RSMainThread::Init gpuContext is nullptr!");
            return;
        }
        int32_t maxResources = 0;
        size_t maxResourcesSize = 0;
        gpuContext->GetResourceCacheLimits(&maxResources, &maxResourcesSize);
        if (maxResourcesSize > 0) {
            gpuContext->SetResourceCacheLimits(cacheLimitsTimes * maxResources, cacheLimitsTimes *
                std::fmin(maxResourcesSize, DEFAULT_SKIA_CACHE_SIZE));
        } else {
            gpuContext->SetResourceCacheLimits(DEFAULT_SKIA_CACHE_COUNT, DEFAULT_SKIA_CACHE_SIZE);
        }
    }
#endif // RS_ENABLE_GL
    RSInnovation::OpenInnovationSo();
#if defined(RS_ENABLE_UNI_RENDER)
    /* move to render thread ? */
    RSBackgroundThread::Instance().InitRenderContext(GetRenderEngine()->GetRenderContext().get());
#endif

    RSRcdRenderManager::InitInstance();

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_PARALLEL_UPLOAD)
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
#if defined(NEW_SKIA) && defined(RS_ENABLE_UNI_RENDER)
        RSUploadResourceThread::Instance().InitRenderContext(GetRenderEngine()->GetRenderContext().get());
#endif
    }
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

    frameRateMgr_ = OHOS::Rosen::HgmCore::Instance().GetFrameRateMgr();
    frameRateMgr_->SetForceUpdateCallback([this](bool idleTimerExpired, bool forceUpdate) {
        RSMainThread::Instance()->PostTask([this, idleTimerExpired, forceUpdate]() {
            RS_TRACE_NAME_FMT("RSMainThread::TimerExpiredCallback Run idleTimerExpiredFlag: %s  forceUpdateFlag: %s",
                idleTimerExpired? "True":"False", forceUpdate? "True": "False");
            RSMainThread::Instance()->SetForceUpdateUniRenderFlag(forceUpdate);
            RSMainThread::Instance()->SetIdleTimerExpiredFlag(idleTimerExpired);
            RS_TRACE_NAME_FMT("DVSyncIsOn: %d", this->rsVSyncDistributor_->IsDVsyncOn());
            if (forceUpdate) {
                RSMainThread::Instance()->RequestNextVSync("ltpoForceUpdate");
            } else {
                RSMainThread::Instance()->RequestNextVSync();
            }
        });
    });
    frameRateMgr_->Init(rsVSyncController_, appVSyncController_, vsyncGenerator_);
    SubscribeAppState();
    PrintCurrentStatus();
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
    if (deviceTypeStr == "pc" || deviceTypeStr == "2in1") {
        deviceType_ = DeviceType::PC;
    } else if (deviceTypeStr == "tablet") {
        deviceType_ = DeviceType::TABLET;
    } else if (deviceTypeStr == "phone") {
        deviceType_ = DeviceType::PHONE;
    } else {
        deviceType_ = DeviceType::OTHERS;
    }
}

DeviceType RSMainThread::GetDeviceType() const
{
    return deviceType_;
}

uint64_t RSMainThread::GetFocusNodeId() const
{
    return focusNodeId_;
}

uint64_t RSMainThread::GetFocusLeashWindowId() const
{
    return focusLeashWindowId_;
}

void RSMainThread::SetFocusLeashWindowId()
{
    const auto& nodeMap = context_->GetNodeMap();
    auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodeMap.GetRenderNode(focusNodeId_));
    if (node != nullptr) {
        auto parent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node->GetParent().lock());
        if (node->IsAppWindow() && parent && parent->IsLeashWindow()) {
            focusLeashWindowId_ = parent->GetId();
        }
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
    UpdateFocusNodeId(focusNodeId_, focusNodeId);
}

void RSMainThread::UpdateFocusNodeId(NodeId oldFocusNodeId, NodeId newFocusNodeId)
{
    if (newFocusNodeId == oldFocusNodeId || newFocusNodeId == INVALID_NODEID) {
        return;
    }
    UpdateNeedDrawFocusChange(oldFocusNodeId);
    UpdateNeedDrawFocusChange(newFocusNodeId);
    focusNodeId_ = newFocusNodeId;
}

void RSMainThread::UpdateNeedDrawFocusChange(NodeId id)
{
    const auto& nodeMap = context_->GetNodeMap();
    auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodeMap.GetRenderNode(id));
    // while nodeMap can't find node, return instantly
    if (!node) {
        return;
    }
    auto parentNode =
        node ? RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node->GetParent().lock()) : nullptr;
    // while node's parent isn't LEASH_WINDOW_NODE, itselt need SetNeedDrawFocusChange
    if (!parentNode || parentNode->GetSurfaceNodeType() != RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        node->SetNeedDrawFocusChange(true);
        return;
    }
    // while node's parent is LEASH_WINDOW_NODE, parent need SetNeedDrawFocusChange
    parentNode->SetNeedDrawFocusChange(true);
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
    RS_PROFILER_ON_PROCESS_COMMAND();
    if (isUniRender_) {
        ProcessCommandForUniRender();
    } else {
        ProcessCommandForDividedRender();
    }
    switch(context_->purgeType_) {
        case RSContext::PurgeType::GENTLY:
            isUniRender_ ? RSUniRenderThread::Instance().ClearMemoryCache(context_->clearMoment_, false) :
                ClearMemoryCache(context_->clearMoment_, false);
            break;
        case RSContext::PurgeType::STRONGLY:
            isUniRender_ ? RSUniRenderThread::Instance().ClearMemoryCache(context_->clearMoment_, true) :
                ClearMemoryCache(context_->clearMoment_, true);
            break;
        default:
            break;
    }
    context_->purgeType_ = RSContext::PurgeType::NONE;
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().AnimateStart();
    }
}

void RSMainThread::PrintCurrentStatus()
{
    std::string gpuType = "";
    switch (OHOS::Rosen::RSSystemProperties::GetGpuApiType()) {
        case OHOS::Rosen::GpuApiType::OPENGL:
            gpuType = "opengl";
            break;
        case OHOS::Rosen::GpuApiType::VULKAN:
            gpuType = "vulkan";
            break;
        case OHOS::Rosen::GpuApiType::DDGR:
            gpuType = "ddgr";
            break;
        default:
            break;
    }
    RS_LOGI("[Drawing] Version: Non-released");
    RS_LOGE("RSMainThread::PrintCurrentStatus:  drawing is opened, gpu type is %{public}s", gpuType.c_str());
}

void RSMainThread::SubScribeSystemAbility()
{
    RS_LOGD("%{public}s", __func__);
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        RS_LOGE("%{public}s failed to get system ability manager client", __func__);
        return;
    }
    std::string threadName = "RSMainThread";
    std::string strUid = std::to_string(getuid());
    std::string strPid = std::to_string(getpid());
    std::string strTid = std::to_string(gettid());

    saStatusChangeListener_ = new (std::nothrow)VSyncSystemAbilityListener(threadName, strUid, strPid, strTid);
    int32_t ret = systemAbilityManager->SubscribeSystemAbility(RES_SCHED_SYS_ABILITY_ID, saStatusChangeListener_);
    if (ret != ERR_OK) {
        RS_LOGE("%{public}s subscribe system ability %{public}d failed.", __func__, RES_SCHED_SYS_ABILITY_ID);
        saStatusChangeListener_ = nullptr;
    }
}

void RSMainThread::CacheCommands()
{
    RS_OPTIONAL_TRACE_FUNC();
    for (auto& skipTransactionData : cachedSkipTransactionDataMap_) {
        pid_t pid = skipTransactionData.first;
        RS_TRACE_NAME("cacheCmd pid: " + std::to_string(pid));
        auto& skipTransactionDataVec = skipTransactionData.second;
        cachedTransactionDataMap_[pid].insert(cachedTransactionDataMap_[pid].begin(),
            std::make_move_iterator(skipTransactionDataVec.begin()),
            std::make_move_iterator(skipTransactionDataVec.end()));
    }
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

bool RSMainThread::CheckParallelSubThreadNodesStatus()
{
    RS_OPTIONAL_TRACE_FUNC();
    cacheCmdSkippedInfo_.clear();
    cacheCmdSkippedNodes_.clear();
    if (subThreadNodes_.empty() &&
        (deviceType_ != DeviceType::PC || (leashWindowCount_ > 0 && isUiFirstOn_ == false))) {
        if (!isUniRender_) {
            RSSubThreadManager::Instance()->ResetSubThreadGrContext(); // planning: move to prepare
        }
        return false;
    }
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
                for (auto& child : *node->GetSortedChildren()) {
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
            RS_LOGD("RSMainThread::CheckParallelSubThreadNodesStatus pid = %{public}s, node name: %{public}s,"
                "id: %{public}" PRIu64 "", std::to_string(pid).c_str(), node->GetName().c_str(), node->GetId());
            if (cacheCmdSkippedInfo_.count(pid) == 0) {
                cacheCmdSkippedInfo_[pid] = std::make_pair(std::vector<NodeId>{node->GetId()}, false);
            } else {
                cacheCmdSkippedInfo_[pid].first.push_back(node->GetId());
            }
            if (!node->HasAbilityComponent()) {
                continue;
            }
            for (auto& nodeId : node->GetAbilityNodeIds()) {
                pid_t abilityNodePid = ExtractPid(nodeId);
                if (cacheCmdSkippedInfo_.count(abilityNodePid) == 0) {
                    cacheCmdSkippedInfo_[abilityNodePid] = std::make_pair(std::vector<NodeId>{node->GetId()}, true);
                } else {
                    cacheCmdSkippedInfo_[abilityNodePid].first.push_back(node->GetId());
                }
            }
        }
    }
    if (!cacheCmdSkippedNodes_.empty()) {
        return true;
    }
    if (!isUiFirstOn_) {
        // clear subThreadNodes_ when UIFirst off and none of subThreadNodes_ is in the state of doing
        subThreadNodes_.clear();
    }
    return false;
}

bool RSMainThread::IsNeedSkip(NodeId instanceRootNodeId, pid_t pid)
{
    return std::any_of(cacheCmdSkippedInfo_[pid].first.begin(), cacheCmdSkippedInfo_[pid].first.end(),
        [instanceRootNodeId](const auto& cacheCmdSkipNodeId) {
            return cacheCmdSkipNodeId == instanceRootNodeId;
        });
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
            NodeId firstLevelNodeId = node->GetFirstLevelNodeId();
            if (IsNeedSkip(firstLevelNodeId, pid)) {
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
#ifdef ROSEN_EMULATOR
                    transactionFlags += "cache [" + std::to_string(pid) + "," + std::to_string(curIndex) + "]";
                    RequestNextVSync();
#endif
                    break;
                }
                ++lastIndex;
                transactionFlags += " [" + std::to_string(pid) + "," + std::to_string(curIndex) + "]";
            } else {
                RS_LOGE("%{public}s wait curIndex:%{public}" PRIu64 ", lastIndex:%{public}" PRIu64 ", pid:%{public}d",
                    __FUNCTION__, curIndex, lastIndex, pid);
                if (transactionDataLastWaitTime_[pid] == 0) {
                    transactionDataLastWaitTime_[pid] = timestamp_;
                }
                if ((timestamp_ - transactionDataLastWaitTime_[pid]) / REFRESH_PERIOD > SKIP_COMMAND_FREQ_LIMIT) {
                    transactionDataLastWaitTime_[pid] = 0;
                    lastIndex = curIndex;
                    transactionFlags += " skip to[" + std::to_string(pid) + "," + std::to_string(curIndex) + "]";
                    RS_LOGE("%{public}s skip to index:%{public}" PRIu64 ", pid:%{public}d",
                        __FUNCTION__, curIndex, pid);
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
    std::shared_ptr<TransactionDataMap> transactionDataEffective = std::make_shared<TransactionDataMap>();
    std::string transactionFlags;
    bool isNeedCacheCmd = CheckParallelSubThreadNodesStatus();
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        cachedSkipTransactionDataMap_.clear();
        for (auto& rsTransactionElem: effectiveTransactionDataIndexMap_) {
            auto& transactionVec = rsTransactionElem.second.second;
            if (isNeedCacheCmd) {
                auto pid = rsTransactionElem.first;
                SkipCommandByNodeId(transactionVec, pid);
            }
            std::sort(transactionVec.begin(), transactionVec.end(), Compare);
        }
        if (isNeedCacheCmd) {
            CacheCommands();
        }
        CheckAndUpdateTransactionIndex(transactionDataEffective, transactionFlags);
    }
    if (!transactionDataEffective->empty()) {
        doDirectComposition_ = false;
    }
    const auto& nodeMap = context_->GetNodeMap();
    nodeMap.TraverseCanvasDrawingNodes([](const std::shared_ptr<RSCanvasDrawingRenderNode>& canvasDrawingNode) {
        if (canvasDrawingNode == nullptr) {
            return;
        }
        if (canvasDrawingNode->IsNeedProcess()) {
            auto drawableNode = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasDrawingNode);
            if (!drawableNode) {
                return;
            }
            static_cast<DrawableV2::RSCanvasDrawingRenderNodeDrawable*>(drawableNode.get())->
                PlaybackInCorrespondThread();
        }
    });
    RS_TRACE_NAME("RSMainThread::ProcessCommandUni" + transactionFlags);
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
    if (!transactionDataEffective->empty()) {
        RSBackgroundThread::Instance().PostTask([ transactionDataEffective ] () {
            RS_TRACE_NAME("RSMainThread::ProcessCommandForUniRender transactionDataEffective clear");
            transactionDataEffective->clear();
        });
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

void RSMainThread::ProcessRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData, pid_t pid)
{
    context_->transactionTimestamp_ = rsTransactionData->GetTimestamp();
    rsTransactionData->Process(*context_);
}

void RSMainThread::ProcessSyncRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData, pid_t pid)
{
    if (!rsTransactionData->IsNeedSync()) {
        syncTransactionData_[pid].emplace_back(std::move(rsTransactionData));
        return;
    }

    if (!syncTransactionData_.empty() && syncTransactionData_.begin()->second.front() &&
        (syncTransactionData_.begin()->second.front()->GetSyncId() > rsTransactionData->GetSyncId())) {
        ROSEN_LOGD("RSMainThread ProcessSyncRSTransactionData while syncId less GetCommandCount: %{public}lu"
            "pid: %{public}d", rsTransactionData->GetCommandCount(), rsTransactionData->GetSendingPid());
        ProcessRSTransactionData(rsTransactionData, pid);
        return;
    }

    bool isNeedCloseSync = rsTransactionData->IsNeedCloseSync();
    if (syncTransactionData_.empty()) {
        if (handler_) {
            auto task = [this, syncId = rsTransactionData->GetSyncId()]() {
                if (!syncTransactionData_.empty() && syncTransactionData_.begin()->second.front() &&
                        syncTransactionData_.begin()->second.front()->GetSyncId() != syncId) {
                    return;
                }
                ROSEN_LOGD("RSMainThread ProcessAllSyncTransactionData timeout task");
                ProcessAllSyncTransactionData();
            };
            handler_->PostTask(task, "ProcessAllSyncTransactionsTimeoutTask",
                RSSystemProperties::GetSyncTransactionWaitDelay());
        }
    }
    if (!syncTransactionData_.empty() && syncTransactionData_.begin()->second.front() &&
        (syncTransactionData_.begin()->second.front()->GetSyncId() != rsTransactionData->GetSyncId())) {
        ProcessAllSyncTransactionData();
    }
    if (syncTransactionData_.count(pid) == 0) {
        syncTransactionData_.insert({ pid, std::vector<std::unique_ptr<RSTransactionData>>() });
    }
    syncTransactionCount_ += rsTransactionData->GetSyncTransactionNum();
    if (isNeedCloseSync) {
        isNeedCloseSync_ = true;
    } else {
        syncTransactionCount_ -= 1;
    }
    syncTransactionData_[pid].emplace_back(std::move(rsTransactionData));
    if (syncTransactionCount_ == 0 && isNeedCloseSync_) {
        ProcessAllSyncTransactionData();
    }
}

void RSMainThread::ProcessAllSyncTransactionData()
{
    for (auto& [pid, transactions] : syncTransactionData_) {
        for (auto& transaction: transactions) {
            ROSEN_LOGD("RSMainThread ProcessAllSyncTransactionData GetCommandCount: %{public}lu pid: %{public}d",
                transaction->GetCommandCount(), pid);
            ProcessRSTransactionData(transaction, pid);
        }
    }
    syncTransactionData_.clear();
    syncTransactionCount_ = 0;
    isNeedCloseSync_ = false;
    RequestNextVSync();
}

void RSMainThread::ConsumeAndUpdateAllNodes()
{
    if (isUniRender_) {
        ResetHardwareEnabledState();
        hasProtectedLayer_ = false;
    }
    RS_OPTIONAL_TRACE_BEGIN("RSMainThread::ConsumeAndUpdateAllNodes");
    bool needRequestNextVsync = false;
    bufferTimestamps_.clear();
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes(
        [this, &needRequestNextVsync](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
        if (surfaceNode == nullptr) {
            return;
        }
        surfaceNode->ResetAnimateState();
        surfaceNode->ResetRotateState();
        // Reset BasicGeoTrans info at the beginning of cmd process
        if (surfaceNode->IsLeashOrMainWindow()) {
            surfaceNode->ResetIsOnlyBasicGeoTransform();
        }
        if (surfaceNode->IsHardwareEnabledType()
            && CheckSubThreadNodeStatusIsDoing(surfaceNode->GetInstanceRootNodeId())) {
            RS_LOGD("SubThread is processing %{public}s, skip acquire buffer", surfaceNode->GetName().c_str());
            return;
        }
        auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*surfaceNode);
        surfaceHandler.ResetCurrentFrameBufferConsumed();
        if (RSBaseRenderUtil::ConsumeAndUpdateBuffer(surfaceHandler, false, timestamp_)) {
            this->bufferTimestamps_[surfaceNode->GetId()] = static_cast<uint64_t>(surfaceNode->GetTimestamp());
            if (surfaceNode->IsCurrentFrameBufferConsumed() && !surfaceNode->IsHardwareEnabledType()) {
                surfaceNode->SetContentDirty();
                doDirectComposition_ = false;
            }
            if (isUniRender_ && surfaceNode->IsCurrentFrameBufferConsumed()) {
                surfaceNode->UpdateBufferInfo(surfaceNode->GetBuffer(), surfaceNode->GetAcquireFence(),
                    surfaceNode->GetPreBuffer().buffer);
                if (surfaceNode->GetBufferSizeChanged()) {
                    doDirectComposition_ = false;
                }
            }
            if (deviceType_ == DeviceType::PC && isUiFirstOn_ && surfaceNode->IsCurrentFrameBufferConsumed()
                && surfaceNode->IsHardwareEnabledType() && surfaceNode->IsHardwareForcedDisabledByFilter()) {
                    RS_OPTIONAL_TRACE_NAME(surfaceNode->GetName() +
                        " SetContentDirty for UIFirst assigning to subthread");
                    surfaceNode->SetContentDirty();
                    doDirectComposition_ = false;
                }
        }

        const auto& surfaceBuffer = surfaceNode->GetBuffer();
        if (RSSystemProperties::GetDrmEnabled() && deviceType_ == DeviceType::PHONE && surfaceBuffer &&
            (surfaceBuffer->GetUsage() & BUFFER_USAGE_PROTECTED)) {
            if (!surfaceNode->GetProtectedLayer()) {
                surfaceNode->SetProtectedLayer(true);
            }
            const auto& instanceNode = surfaceNode->GetInstanceRootNode();
            if (instanceNode && instanceNode->IsOnTheTree()) {
                hasProtectedLayer_ = true;
            }
        }

        // still have buffer(s) to consume.
        if (surfaceHandler.GetAvailableBufferCount() > 0 || surfaceHandler.HasBufferCache()) {
            needRequestNextVsync = true;
        }
    });

    if (needRequestNextVsync) {
        RequestNextVSync();
    }
    RS_OPTIONAL_TRACE_END();
}

bool RSMainThread::CheckSubThreadNodeStatusIsDoing(NodeId appNodeId) const
{
    for (auto& node : subThreadNodes_) {
        if (node->GetCacheSurfaceProcessedStatus() != CacheProcessStatus::DOING) {
            continue;
        }
        if (node->GetId() == appNodeId) {
            return true;
        }
        for (auto& child : *node->GetSortedChildren()) {
            auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
            if (surfaceNode && surfaceNode->GetId() == appNodeId) {
                return true;
            }
        }
    }
    return false;
}

void RSMainThread::CollectInfoForHardwareComposer()
{
    if (!isUniRender_) {
        return;
    }
    CheckIfHardwareForcedDisabled();
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes(
        [this, &nodeMap](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
            if (surfaceNode == nullptr) {
                return;
            }
            if (surfaceNode->GetBuffer() != nullptr) {
                selfDrawingNodes_.emplace_back(surfaceNode);
            }
            if (!surfaceNode->IsOnTheTree()) {
                if (surfaceNode->IsCurrentFrameBufferConsumed()) {
                    surfaceNode->UpdateHardwareDisabledState(true);
                    doDirectComposition_ = false;
                }
                return;
            }

            if (surfaceNode->IsLeashWindow() && surfaceNode->GetForceUIFirstChanged()) {
                forceUIFirstChanged_ = true;
                surfaceNode->SetForceUIFirstChanged(false);
            }

            if (!surfaceNode->IsHardwareEnabledType()) {
                return;
            }

            // if hwc node is set on the tree this frame, mark its parent app node to be prepared
            auto appNodeId = surfaceNode->GetInstanceRootNodeId();
            if (surfaceNode->IsNewOnTree()) {
                context_->AddActiveNode(nodeMap.GetRenderNode(appNodeId));
            }

            if (surfaceNode->GetBuffer() != nullptr) {
                // collect hwc nodes vector, used for display node skip and direct composition cases
                surfaceNode->SetIsLastFrameHwcEnabled(!surfaceNode->IsHardwareForcedDisabled());
                hardwareEnabledNodes_.emplace_back(surfaceNode);
            }

            // set content dirty for hwc node if needed
            if (isHardwareForcedDisabled_) {
                // buffer updated or hwc -> gpu
                if (surfaceNode->IsCurrentFrameBufferConsumed() || surfaceNode->GetIsLastFrameHwcEnabled()) {
                    surfaceNode->SetContentDirty();
                }
            } else if (!surfaceNode->GetIsLastFrameHwcEnabled()) { // gpu -> hwc
                doDirectComposition_ = false;
                if (surfaceNode->IsCurrentFrameBufferConsumed()) {
                    surfaceNode->SetContentDirty();
                } else {
                    surfaceNode->SetHwcDelayDirtyFlag(true);
                }
            } else { // hwc -> hwc
                // self-drawing node don't set content dirty when gpu -> hwc
                // so first frame in hwc -> hwc, should set content dirty
                if (surfaceNode->GetHwcDelayDirtyFlag()) {
                    surfaceNode->SetContentDirty();
                    surfaceNode->SetHwcDelayDirtyFlag(false);
                    doDirectComposition_ = false;
                }
            }

            if (surfaceNode->IsCurrentFrameBufferConsumed()) {
                isHardwareEnabledBufferUpdated_ = true;
            }
        });
}

bool RSMainThread::IsLastFrameUIFirstEnabled(NodeId appNodeId) const
{
    for (auto& node : subThreadNodes_) {
        if (node->IsAppWindow()) {
            if (node->GetId() == appNodeId) {
                return true;
            }
        } else {
            for (auto& child : *node->GetSortedChildren()) {
                auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
                if (surfaceNode && surfaceNode->IsAppWindow() && surfaceNode->GetId() == appNodeId) {
                    return true;
                }
            }
        }
    }
    return false;
}

void RSMainThread::CheckIfHardwareForcedDisabled()
{
    ColorFilterMode colorFilterMode = renderEngine_->GetColorFilterMode();
    bool hasColorFilter = colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
        colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE;
    std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    bool isMultiDisplay = rootNode && rootNode->GetChildrenCount() > 1;

    // check all children of global root node, and only disable hardware composer
    // in case node's composite type is UNI_RENDER_EXPAND_COMPOSITE
    const auto& children = rootNode->GetChildren();
    auto itr = std::find_if(children->begin(), children->end(), [](const std::shared_ptr<RSRenderNode>& child) -> bool {
            if (child == nullptr) {
                return false;
            }
            if (child->GetType() != RSRenderNodeType::DISPLAY_NODE) {
                return false;
            }
            auto displayNodeSp = std::static_pointer_cast<RSDisplayRenderNode>(child);
            return displayNodeSp->GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE;
    });

    bool isExpandScreenCase = itr != children->end();

    // [PLANNING] GetChildrenCount > 1 indicates multi display, only Mirror Mode need be marked here
    // Mirror Mode reuses display node's buffer, so mark it and disable hardware composer in this case
    isHardwareForcedDisabled_ = isHardwareForcedDisabled_ || doWindowAnimate_ ||
        (isMultiDisplay && isExpandScreenCase && !hasProtectedLayer_) || hasColorFilter;
    RS_OPTIONAL_TRACE_NAME_FMT("hwc debug global: CheckIfHardwareForcedDisabled isHardwareForcedDisabled_:%d "
        "doWindowAnimate_:%d isMultiDisplay:%d hasColorFilter:%d",
        isHardwareForcedDisabled_, doWindowAnimate_.load(), isMultiDisplay, hasColorFilter);

    if (isMultiDisplay && !isHardwareForcedDisabled_) {
        // Disable direct composition when hardware composer is enabled for virtual screen
        doDirectComposition_ = false;
    }
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
                    auto& preBuffer = surfaceHandler.GetPreBuffer();
                    if (preBuffer.buffer != nullptr) {
                        auto releaseTask = [buffer = preBuffer.buffer, consumer = surfaceHandler.GetConsumer(),
                            fence = preBuffer.releaseFence]() mutable {
                            auto ret = consumer->ReleaseBuffer(buffer, fence);
                            if (ret != OHOS::SURFACE_ERROR_OK) {
                                RS_LOGD("surfaceHandler ReleaseBuffer failed(ret: %{public}d)!", ret);
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
        RSBaseRenderUtil::ReleaseBuffer(static_cast<RSSurfaceHandler&>(*surfaceNode));
    });
    RS_OPTIONAL_TRACE_END();
}

uint32_t RSMainThread::GetRefreshRate() const
{
    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSMainThread::GetRefreshRate screenManager is nullptr");
        return STANDARD_REFRESH_RATE;
    }
    uint32_t refreshRate = OHOS::Rosen::HgmCore::Instance().GetScreenCurrentRefreshRate(
        screenManager->GetDefaultScreenId());
    if (refreshRate == 0) {
        RS_LOGE("RSMainThread::GetRefreshRate refreshRate is invalid");
        return STANDARD_REFRESH_RATE;
    }
    return refreshRate;
}

uint32_t RSMainThread::GetDynamicRefreshRate() const
{
    uint32_t refreshRate = OHOS::Rosen::HgmCore::Instance().GetScreenCurrentRefreshRate(displayNodeScreenId_);
    if (refreshRate == 0) {
        RS_LOGE("RSMainThread::GetDynamicRefreshRate refreshRate is invalid");
        return STANDARD_REFRESH_RATE;
    }
    return refreshRate;
}

void RSMainThread::ClearMemoryCache(ClearMemoryMoment moment, bool deeply, pid_t pid)
{
    if (!RSSystemProperties::GetReleaseResourceEnabled()) {
        return;
    }
    this->clearMemoryFinished_ = false;
    this->clearMemDeeply_ = this->clearMemDeeply_ || deeply;
    this->SetClearMoment(moment);
    this->exitedPidSet_.emplace(pid);
    auto task =
        [this, moment, deeply]() {
            auto grContext = GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
            if (!grContext) {
                return;
            }
            RS_LOGD("Clear memory cache %{public}d", this->GetClearMoment());
            RS_TRACE_NAME_FMT("Clear memory cache, cause the moment [%d] happen", this->GetClearMoment());
            SKResourceManager::Instance().ReleaseResource();
            grContext->Flush();
            SkGraphics::PurgeAllCaches(); // clear cpu cache
            auto pid = *(this->exitedPidSet_.begin());
            if (this->exitedPidSet_.size() == 1 && pid == -1) {  // no exited app, just clear scratch resource
                if (deeply || this->deviceType_ != DeviceType::PHONE) {
                    MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(grContext);
                } else {
                    MemoryManager::ReleaseUnlockGpuResource(grContext);
                }
            } else {
                MemoryManager::ReleaseUnlockGpuResource(grContext, this->exitedPidSet_);
            }
            grContext->FlushAndSubmit(true);
            this->clearMemoryFinished_ = true;
            this->exitedPidSet_.clear();
            this->clearMemDeeply_ = false;
            this->SetClearMoment(ClearMemoryMoment::NO_CLEAR);
        };
    if (!isUniRender_ || rsParallelType_ == RsParallelType::RS_PARALLEL_TYPE_SINGLE_THREAD) {
        PostTask(task, CLEAR_GPU_CACHE,
            (this->deviceType_ == DeviceType::PHONE ? TIME_OF_EIGHT_FRAMES : TIME_OF_THE_FRAMES) / GetRefreshRate(),
            AppExecFwk::EventQueue::Priority::HIGH);
    } else {
        RSUniRenderThread::Instance().PostTask(task, CLEAR_GPU_CACHE,
            (this->deviceType_ == DeviceType::PHONE ? TIME_OF_EIGHT_FRAMES : TIME_OF_THE_FRAMES) / GetRefreshRate(),
            AppExecFwk::EventQueue::Priority::HIGH);
    }
}

void RSMainThread::WaitUtilUniRenderFinished()
{
    std::unique_lock<std::mutex> lock(uniRenderMutex_);
    if (uniRenderFinished_) {
        return;
    }
    RS_TRACE_NAME("RSMainThread::WaitUtilUniRenderFinished");
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

void RSMainThread::WaitUntilUnmarshallingTaskFinished()
{
    if (!isUniRender_) {
        return;
    }
    if (!needWaitUnmarshalFinished_) {
        /* if needWaitUnmarshalFinished_ is false, it means UnmarshallingTask is finished, no need to wait.
         * reset needWaitUnmarshalFinished_ to true, maybe it need to wait next time.
         */
        needWaitUnmarshalFinished_ = true;
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
            RS_LOGE("RSMainThread::MergeToEffectiveTransactionDataMap pid:%{public}d not valid, skip it", pid);
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

void RSMainThread::NotifySurfaceCapProcFinish()
{
    RS_TRACE_NAME("RSMainThread::NotifySurfaceCapProcFinish");
    std::lock_guard<std::mutex> lock(surfaceCapProcMutex_);
    surfaceCapProcFinished_ = true;
    surfaceCapProcTaskCond_.notify_one();
}

void RSMainThread::WaitUntilSurfaceCapProcFinished()
{
    if (GetDeviceType() != DeviceType::PHONE) {
        return;
    }
    std::unique_lock<std::mutex> lock(surfaceCapProcMutex_);
    if (surfaceCapProcFinished_) {
        return;
    }
    RS_OPTIONAL_TRACE_BEGIN("RSMainThread::WaitUntilSurfaceCapProcFinished");
    surfaceCapProcTaskCond_.wait_until(lock, std::chrono::system_clock::now() +
        std::chrono::milliseconds(WAIT_FOR_SURFACE_CAPTURE_PROCESS_TIMEOUT),
        [this]() { return surfaceCapProcFinished_; });
    RS_OPTIONAL_TRACE_END();
}

void RSMainThread::SetSurfaceCapProcFinished(bool flag)
{
    std::lock_guard<std::mutex> lock(surfaceCapProcMutex_);
    surfaceCapProcFinished_ = flag;
}

bool RSMainThread::IsRequestedNextVSync()
{
    if (receiver_ != nullptr) {
        return receiver_->IsRequestedNextVSync();
    }
    return false;
}

void RSMainThread::ProcessHgmFrameRate(uint64_t timestamp)
{
    RS_TRACE_FUNC();
    if (rsFrameRateLinker_ != nullptr) {
        rsFrameRateLinker_->SetExpectedRange(rsCurrRange_);
        RS_TRACE_NAME_FMT("rsCurrRange = (%d, %d, %d)", rsCurrRange_.min_, rsCurrRange_.max_, rsCurrRange_.preferred_);
    }
    // Check and processing refresh rate task.
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    hgmCore.SetTimestamp(timestamp);
    auto pendingRefreshRate = frameRateMgr_->GetPendingRefreshRate();
    if (pendingRefreshRate != nullptr) {
        hgmCore.SetPendingScreenRefreshRate(*pendingRefreshRate);
        frameRateMgr_->ResetPendingRefreshRate();
        RS_TRACE_NAME_FMT("RSMainThread::ProcessHgmFrameRate pendingRefreshRate: %d", *pendingRefreshRate);
    }

    // hgm warning: use IsLtpo instead after GetDisplaySupportedModes ready
    if (frameRateMgr_->GetCurScreenStrategyId().find("LTPO") == std::string::npos) {
        frameRateMgr_->UniProcessDataForLtps(idleTimerExpiredFlag_);
    } else {
        auto appFrameLinkers = GetContext().GetFrameRateLinkerMap().Get();
        frameRateMgr_->UniProcessDataForLtpo(timestamp, rsFrameRateLinker_, appFrameLinkers,
            idleTimerExpiredFlag_, rsVSyncDistributor_->IsDVsyncOn());
    }

    if (rsVSyncDistributor_->IsDVsyncOn()) {
        auto pendingRefreshRate = frameRateMgr_->GetPendingRefreshRate();
        if (pendingRefreshRate != nullptr) {
            hgmCore.SetPendingScreenRefreshRate(*pendingRefreshRate);
            frameRateMgr_->ResetPendingRefreshRate();
        }
    }
}

bool RSMainThread::GetParallelCompositionEnabled()
{
    return doParallelComposition_;
}

void RSMainThread::SetFrameIsRender(bool isRender)
{
    markRenderFlag_ = true;
    rsVSyncDistributor_->SetFrameIsRender(isRender);
}

void RSMainThread::ColorPickerRequestVsyncIfNeed()
{
    if (colorPickerForceRequestVsync_) {
        colorPickerRequestFrameNum_--;
        if (colorPickerRequestFrameNum_ > 0) {
            RSMainThread::Instance()->SetNoNeedToPostTask(false);
            RSMainThread::Instance()->SetDirtyFlag();
            RequestNextVSync();
        } else {
            // The last frame reset to 15, then to request next 15 frames if need
            colorPickerRequestFrameNum_ = 15;
            RSMainThread::Instance()->SetNoNeedToPostTask(true);
            colorPickerForceRequestVsync_ = false;
        }
    } else {
        RSMainThread::Instance()->SetNoNeedToPostTask(false);
    }
}

void RSMainThread::WaitUntilUploadTextureTaskFinishedForGL()
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        WaitUntilUploadTextureTaskFinished(isUniRender_);
    }
}

void RSMainThread::UniRender(std::shared_ptr<RSBaseRenderNode> rootNode)
{
    if (isAccessibilityConfigChanged_) {
        RSUniRenderVisitor::ClearRenderGroupCache();
    }
    UpdateUIFirstSwitch();
    UpdateRogSizeIfNeeded();
    auto uniVisitor = std::make_shared<RSUniRenderVisitor>();
    uniVisitor->SetHardwareEnabledNodes(hardwareEnabledNodes_);
    uniVisitor->SetAppWindowNum(appWindowNum_);
    uniVisitor->SetProcessorRenderEngine(GetRenderEngine());
    uniVisitor->SetForceUpdateFlag(forceUpdateUniRenderFlag_);

    if (isHardwareForcedDisabled_) {
        uniVisitor->MarkHardwareForcedDisabled();
        doDirectComposition_ = false;
    }
    bool needTraverseNodeTree = true;
    if (doDirectComposition_ && !isDirty_ && !isAccessibilityConfigChanged_
        && !isCachedSurfaceUpdated_) {
        if (isHardwareEnabledBufferUpdated_) {
            needTraverseNodeTree = !DoDirectComposition(rootNode, !isLastFrameDirectComposition_);
        } else if (forceUpdateUniRenderFlag_) {
            RS_TRACE_NAME("RSMainThread::UniRender ForceUpdateUniRender");
        } else if (colorPickerForceRequestVsync_) {
            RS_TRACE_NAME("RSMainThread::UniRender ColorPickerForceRequestVsync");
            RSMainThread::Instance()->SetNoNeedToPostTask(false);
            RSMainThread::Instance()->SetDirtyFlag();
            RequestNextVSync();
            return;
        } else {
            RS_LOGD("RSMainThread::Render nothing to update");
            RSMainThread::Instance()->SetFrameIsRender(false);
            for (auto& node: hardwareEnabledNodes_) {
                if (!node->IsHardwareForcedDisabled()) {
                    node->MarkCurrentFrameHardwareEnabled();
                }
            }
            WaitUntilUploadTextureTaskFinishedForGL();
            return;
        }
    }

    ColorPickerRequestVsyncIfNeed();

    isCachedSurfaceUpdated_ = false;
    if (needTraverseNodeTree) {
        RSUifirstManager::Instance().ProcessForceUpdateNode();
        doDirectComposition_ = false;
        renderThreadParams_->selfDrawingNodes_ = std::move(selfDrawingNodes_);
        renderThreadParams_->hardwareEnabledTypeNodes_ = std::move(hardwareEnabledNodes_);
        uniVisitor->SetAnimateState(doWindowAnimate_);
        uniVisitor->SetDirtyFlag(isDirty_ || isAccessibilityConfigChanged_ || forceUIFirstChanged_);
        forceUIFirstChanged_ = false;
        SetFocusLeashWindowId();
        uniVisitor->SetFocusedNodeId(focusNodeId_, focusLeashWindowId_);
        if (RSSystemProperties::GetQuickPrepareEnabled()) {
            //planning:the QuickPrepare will be replaced by Prepare
            rootNode->QuickPrepare(uniVisitor);
        } else {
            rootNode->Prepare(uniVisitor);
        }
        isAccessibilityConfigChanged_ = false;
        RSPointLightManager::Instance()->PrepareLight();
        vsyncControlEnabled_ = (deviceType_ == DeviceType::PC) && RSSystemParameters::GetVSyncControlEnabled();
        systemAnimatedScenesEnabled_ = RSSystemParameters::GetSystemAnimatedScenesEnabled();
        if (!RSSystemProperties::GetQuickPrepareEnabled()) {
            CalcOcclusion();
        }
        if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
            WaitUntilUploadTextureTaskFinished(isUniRender_);
        }
        if (false) { // planning: move to prepare
            auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(
                rootNode->GetFirstChild());
            std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
            std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
            RSUniRenderUtil::AssignWindowNodes(displayNode, mainThreadNodes, subThreadNodes);
            const auto& nodeMap = context_->GetNodeMap();
            RSUniRenderUtil::ClearSurfaceIfNeed(nodeMap, displayNode, oldDisplayChildren_, deviceType_);
            RSUniRenderUtil::CacheSubThreadNodes(subThreadNodes_, subThreadNodes);
        }
        // set params used in render thread
        uniVisitor->SetUniRenderThreadParam(renderThreadParams_);
    } else if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        WaitUntilUploadTextureTaskFinished(isUniRender_);
    }
    isDirty_ = false;
    forceUpdateUniRenderFlag_ = false;
    idleTimerExpiredFlag_ = false;
}

bool RSMainThread::DoDirectComposition(std::shared_ptr<RSBaseRenderNode> rootNode, bool waitForRT)
{
    auto children = rootNode->GetChildren();
    if (children->empty()) {
        return false;
    }
    RS_TRACE_NAME("DoDirectComposition");
    auto displayNode = RSRenderNode::ReinterpretCast<RSDisplayRenderNode>(children->front());
    if (!displayNode ||
        displayNode->GetCompositeType() != RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE) {
        RS_LOGE("RSMainThread::DoDirectComposition displayNode state error");
        return false;
    }
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    auto screenInfo = screenManager->QueryScreenInfo(displayNode->GetScreenId());
    if (screenInfo.state != ScreenState::HDI_OUTPUT_ENABLE) {
        RS_LOGE("RSMainThread::DoDirectComposition: ScreenState error!");
        return false;
    }
    auto processor = RSProcessorFactory::CreateProcessor(displayNode->GetCompositeType());
    auto renderEngine = GetRenderEngine();
    if (processor == nullptr || renderEngine == nullptr) {
        RS_LOGE("RSMainThread::DoDirectComposition: RSProcessor or renderEngine is null!");
        return false;
    }

    if (!processor->Init(*displayNode, displayNode->GetDisplayOffsetX(), displayNode->GetDisplayOffsetY(),
        INVALID_SCREEN_ID, renderEngine)) {
        RS_LOGE("RSMainThread::DoDirectComposition: processor init failed!");
        return false;
    }

    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExecute()) {
        RS_LOGW("RSMainThread::DoDirectComposition: hardwareThread task has too many to Execute");
    }
    processor->ProcessDisplaySurface(*displayNode);
    for (auto& surfaceNode : hardwareEnabledNodes_) {
        if (!surfaceNode->IsHardwareForcedDisabled()) {
            auto params = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
            processor->CreateLayer(*surfaceNode, *params);
        }
    }
    if (waitForRT) {
        RSUniRenderThread::Instance().PostSyncTask([processor]() {
            RS_TRACE_NAME("DoDirectComposition PostProcess");
            processor->PostProcess();
        });
    } else {
        processor->PostProcess();
    }
    RS_LOGD("RSMainThread::DoDirectComposition end");
    return true;
}

pid_t RSMainThread::GetDesktopPidForRotationScene() const
{
    return desktopPidForRotationScene_;
}

void RSMainThread::Render()
{
    if (RSSystemParameters::GetRenderStop()) {
        return;
    }
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
            WaitUntilUploadTextureTaskFinished(isUniRender_);
        }
        RS_LOGE("RSMainThread::Render GetGlobalRootRenderNode fail");
        return;
    }
    if (isUniRender_) {
        auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
        renderThreadParams_->SetTimestamp(hgmCore.GetCurrentTimestamp());
        renderThreadParams_->SetRequestNextVsyncFlag(needRequestNextVsyncAnimate_);
        renderThreadParams_->SetPendingScreenRefreshRate(hgmCore.GetPendingScreenRefreshRate());
        renderThreadParams_->SetForceCommitLayer(isHardwareEnabledBufferUpdated_ || forceUpdateUniRenderFlag_);
        renderThreadParams_->SetOcclusionEnabled(RSSystemProperties::GetOcclusionEnabled());
    }
    if (RSSystemProperties::GetRenderNodeTraceEnabled()) {
        RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    }
    if (focusAppBundleName_.find(DESKTOP_NAME_FOR_ROTATION) != std::string::npos) {
        desktopPidForRotationScene_ = focusAppPid_;
    }
    int dumpTreeCount = RSSystemParameters::GetDumpRSTreeCount();
    if (UNLIKELY(dumpTreeCount)) {
        RS_TRACE_NAME("dump rstree");
        RenderServiceTreeDump(g_dumpStr);
        RSSystemParameters::SetDumpRSTreeCount(dumpTreeCount - 1);
    }
    if (isUniRender_) {
        renderThreadParams_->SetWatermark(watermarkFlag_, watermarkImg_);
        renderThreadParams_->SetCurtainScreenUsingStatus(isCurtainScreenOn_);
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
    if (!isUniRender_) {
        CallbackDrawContextStatusToWMS();
        CheckSystemSceneStatus();
        PerfForBlurIfNeeded();
    }

    if (auto screenManager = CreateOrGetScreenManager()) {
        auto& rsLuminance = RSLuminanceControl::Get();
        for (const auto& child : *rootNode->GetSortedChildren()) {
            auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(child);
            if (displayNode == nullptr) {
                continue;
            }

            auto screenId = displayNode->GetScreenId();
            if (rsLuminance.IsDimmingOn(screenId)) {
                rsLuminance.DimmingIncrease(screenId);
            }
            if (rsLuminance.IsNeedUpdateLuminance(screenId)) {
                uint32_t newLevel = rsLuminance.GetNewHdrLuminance(screenId);
                screenManager->SetScreenBacklight(screenId, newLevel);
                rsLuminance.SetNowHdrLuminance(screenId, newLevel);
            }
        }
    }
}

void RSMainThread::CheckSystemSceneStatus()
{
    std::lock_guard<std::mutex> lock(systemAnimatedScenesMutex_);
    uint64_t curTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    while (!systemAnimatedScenesList_.empty()) {
        if (curTime - static_cast<uint64_t>(systemAnimatedScenesList_.front().second) > MAX_SYSTEM_SCENE_STATUS_TIME) {
            systemAnimatedScenesList_.pop_front();
        } else {
            break;
        }
    }
    while (!threeFingerScenesList_.empty()) {
        if (curTime - static_cast<uint64_t>(threeFingerScenesList_.front().second) > MAX_SYSTEM_SCENE_STATUS_TIME) {
            threeFingerScenesList_.pop_front();
        } else {
            break;
        }
    }
}

void RSMainThread::CallbackDrawContextStatusToWMS()
{
    VisibleData drawStatusVec;
    for (auto dynamicNodeId : curDrawStatusVec_) {
        if (lastDrawStatusMap_.find(dynamicNodeId) == lastDrawStatusMap_.end()) {
            drawStatusVec.emplace_back(std::make_pair(dynamicNodeId,
                WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_DYNAMIC_STATUS));
            RS_OPTIONAL_TRACE_NAME_FMT("%s nodeId[%" PRIu64 "] status[%d]",
                __func__, dynamicNodeId, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_DYNAMIC_STATUS);
        }
        lastDrawStatusMap_[dynamicNodeId] = timestamp_;
    }
    auto drawStatusIter = lastDrawStatusMap_.begin();
    while (drawStatusIter != lastDrawStatusMap_.end()) {
        if (timestamp_ - drawStatusIter->second > MAX_DYNAMIC_STATUS_TIME) {
            drawStatusVec.emplace_back(std::make_pair(drawStatusIter->first,
                WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_STATIC_STATUS));
            RS_OPTIONAL_TRACE_NAME_FMT("%s nodeId[%" PRIu64 "] status[%d]",
                __func__, drawStatusIter->first, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_STATIC_STATUS);
            auto tmpIter = drawStatusIter++;
            lastDrawStatusMap_.erase(tmpIter);
        } else {
            drawStatusIter++;
        }
    }
    curDrawStatusVec_.clear();
    if (!drawStatusVec.empty()) {
        std::lock_guard<std::mutex> lock(occlusionMutex_);
        for (auto it = occlusionListeners_.begin(); it != occlusionListeners_.end(); it++) {
            if (it->second) {
                it->second->OnOcclusionVisibleChanged(std::make_shared<RSOcclusionData>(drawStatusVec));
            }
        }
    }
}

bool RSMainThread::CheckSurfaceNeedProcess(OcclusionRectISet& occlusionSurfaces,
    std::shared_ptr<RSSurfaceRenderNode> curSurface)
{
    bool needProcess = false;
    if (curSurface->IsFocusedNode(focusNodeId_)) {
        needProcess = true;
        if (!curSurface->HasContainerWindow() && !curSurface->IsTransparent() &&
            !curSurface->HasWindowCorner() &&
            !curSurface->GetAnimateState() && // when node animating (i.e. 3d animation), the region cannot be trusted
            curSurface->GetName().find("hisearch") == std::string::npos) {
            occlusionSurfaces.insert({curSurface->GetId(), curSurface->GetDstRect()});
        }
    } else {
        size_t beforeSize = occlusionSurfaces.size();
        occlusionSurfaces.insert({curSurface->GetId(), curSurface->GetDstRect()});
        bool insertSuccess = occlusionSurfaces.size() > beforeSize ? true : false;
        if (insertSuccess) {
            needProcess = true;
            if (curSurface->IsTransparent() ||
                curSurface->HasWindowCorner() ||
                curSurface->GetAnimateState() || // when node animating(i.e. 3d animation), the region cannot be trusted
                curSurface->GetName().find("hisearch") != std::string::npos) {
                auto iter = std::find_if(occlusionSurfaces.begin(), occlusionSurfaces.end(),
                    [&curSurface](const auto& r) -> bool {return r.second == curSurface->GetDstRect();});
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

RSVisibleLevel RSMainThread::GetRegionVisibleLevel(const Occlusion::Region& curRegion,
    const Occlusion::Region& visibleRegion)
{
    if (visibleRegion.IsEmpty()) {
        return RSVisibleLevel::RS_INVISIBLE;
    } else if (visibleRegion.Area() == curRegion.Area()) {
        return RSVisibleLevel::RS_ALL_VISIBLE;
    } else if (static_cast<uint>(visibleRegion.Area()) <
        (static_cast<uint>(curRegion.Area()) >> VISIBLEAREARATIO_FORQOS)) {
        return RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE;
    }
    return RSVisibleLevel::RS_SEMI_NONDEFAULT_VISIBLE;
}

RSVisibleLevel RSMainThread::CalcSurfaceNodeVisibleRegion(const std::shared_ptr<RSDisplayRenderNode>& displayNode,
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
    Occlusion::Region& accumulatedRegion, Occlusion::Region& curRegion, Occlusion::Region& totalRegion)
{
    if (!surfaceNode) {
        return RSVisibleLevel::RS_INVISIBLE;
    }
    
    if (!isUniRender_) {
        if (displayNode) {
            surfaceNode->SetDstRect(displayNode->GetSurfaceDstRect(surfaceNode->GetId()));
        }
    }

    Occlusion::Rect occlusionRect = surfaceNode->GetSurfaceOcclusionRect(isUniRender_);
    curRegion = Occlusion::Region { occlusionRect };
    Occlusion::Region subRegion = curRegion.Sub(accumulatedRegion);

    RSVisibleLevel visibleLevel = GetRegionVisibleLevel(curRegion, subRegion);

    if (!isUniRender_) {
        Occlusion::Region visSurface = surfaceNode->GetVisibleRegion();
        totalRegion = subRegion.Or(visSurface);
    } else {
        totalRegion = subRegion;
    }

    return visibleLevel;
}

void RSMainThread::CalcOcclusionImplementation(const std::shared_ptr<RSDisplayRenderNode>& displayNode,
    std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces, VisibleData& dstCurVisVec,
    std::map<NodeId, RSVisibleLevel>& dstVisMapForVsyncRate)
{
    Occlusion::Region accumulatedRegion;
    VisibleData curVisVec;
    OcclusionRectISet occlusionSurfaces;
    std::map<NodeId, RSVisibleLevel> visMapForVsyncRate;
    bool hasFilterCacheOcclusion = false;
    bool filterCacheOcclusionEnabled = RSSystemParameters::GetFilterCacheOcculusionEnabled();

    auto calculator = [this, &displayNode, &occlusionSurfaces, &accumulatedRegion, &curVisVec, &visMapForVsyncRate,
        &hasFilterCacheOcclusion, filterCacheOcclusionEnabled] (std::shared_ptr<RSSurfaceRenderNode>& curSurface,
        bool needSetVisibleRegion) {
        curSurface->setQosCal(vsyncControlEnabled_);
        if (!CheckSurfaceNeedProcess(occlusionSurfaces, curSurface)) {
            curSurface->SetVisibleRegionRecursive({}, curVisVec, visMapForVsyncRate);
            return;
        }

        Occlusion::Region curRegion {};
        Occlusion::Region totalRegion {};
        auto visibleLevel =
            CalcSurfaceNodeVisibleRegion(displayNode, curSurface, accumulatedRegion, curRegion, totalRegion);

        curSurface->SetVisibleRegionRecursive(totalRegion, curVisVec, visMapForVsyncRate, needSetVisibleRegion,
            visibleLevel, !systemAnimatedScenesList_.empty());
        curSurface->AccumulateOcclusionRegion(
            accumulatedRegion, curRegion, hasFilterCacheOcclusion, isUniRender_, filterCacheOcclusionEnabled);
    };

    for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
        auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (curSurface && !curSurface->IsLeashWindow()) {
            curSurface->SetOcclusionInSpecificScenes(deviceType_ == DeviceType::PC && !threeFingerScenesList_.empty());
            calculator(curSurface, true);
        }
    }

    // if there are valid filter cache occlusion, recalculate surfacenode visibleregionforcallback for WMS/QOS callback
    if (hasFilterCacheOcclusion && isUniRender_) {
        curVisVec.clear();
        visMapForVsyncRate.clear();
        occlusionSurfaces.clear();
        accumulatedRegion = {};
        for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
            auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
            if (curSurface && !curSurface->IsLeashWindow()) {
                calculator(curSurface, false);
            }
        }
    }

    dstCurVisVec.insert(dstCurVisVec.end(), curVisVec.begin(), curVisVec.end());
    dstVisMapForVsyncRate.insert(visMapForVsyncRate.begin(), visMapForVsyncRate.end());
}

void RSMainThread::CalcOcclusion()
{
    RS_OPTIONAL_TRACE_NAME("RSMainThread::CalcOcclusion");
    RS_LOGD("RSMainThread::CalcOcclusion animate:%{public}d isUniRender:%{public}d",
        doWindowAnimate_.load(), isUniRender_);
    if (doWindowAnimate_ && !isUniRender_) {
        return;
    }
    const std::shared_ptr<RSBaseRenderNode> node = context_->GetGlobalRootRenderNode();
    if (node == nullptr) {
        RS_LOGE("RSMainThread::CalcOcclusion GetGlobalRootRenderNode fail");
        return;
    }
    std::map<RSDisplayRenderNode::SharedPtr, std::vector<RSBaseRenderNode::SharedPtr>> curAllSurfacesInDisplay;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    for (const auto& child : *node->GetSortedChildren()) {
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(child);
        if (displayNode) {
            const auto& surfaces = displayNode->GetCurAllSurfaces();
            curAllSurfacesInDisplay[displayNode] = surfaces;
            curAllSurfaces.insert(curAllSurfaces.end(), surfaces.begin(), surfaces.end());
        }
    }

    if (node->GetChildrenCount()== 1) {
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(node->GetFirstChild());
        if (displayNode) {
            curAllSurfaces = displayNode->GetCurAllSurfaces();
        }
    } else {
        node->CollectSurface(node, curAllSurfaces, isUniRender_, false);
    }
    // Judge whether it is dirty
    // Surface cnt changed or surface DstRectChanged or surface ZorderChanged
    std::vector<NodeId> curSurfaceIds;
    curSurfaceIds.reserve(curAllSurfaces.size());
    for (auto it = curAllSurfaces.begin(); it != curAllSurfaces.end(); ++it) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surface == nullptr) {
            continue;
        }
        curSurfaceIds.emplace_back(surface->GetId());
    }
    bool winDirty = (isDirty_ || lastFocusNodeId_ != focusNodeId_ || lastSurfaceIds_ != curSurfaceIds);
    lastSurfaceIds_ = std::move(curSurfaceIds);
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
            } else if (RSSystemParameters::GetFilterCacheOcculusionEnabled() &&
                surface->IsTransparent() && surface->IsFilterCacheStatusChanged()) {
                // When current frame's filter cache is valid or last frame's occlusion use filter cache as opaque
                // The occlusion needs to be recalculated
                winDirty = true;
            }
            surface->CleanDstRectChanged();
            surface->CleanAlphaChanged();
            surface->CleanOpaqueRegionChanged();
            surface->CleanDirtyRegionUpdated();
        }
    }
    if (!winDirty && !(systemAnimatedScenesList_.empty() && isReduceVSyncBySystemAnimatedScenes_)) {
        if (SurfaceOcclusionCallBackIfOnTreeStateChanged()) {
            SurfaceOcclusionCallback();
        }
        return;
    }
    isReduceVSyncBySystemAnimatedScenes_ = false;
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstVisMapForVsyncRate;
    for (auto& surfaces : curAllSurfacesInDisplay) {
        CalcOcclusionImplementation(surfaces.first, surfaces.second, dstCurVisVec, dstVisMapForVsyncRate);
    }

    // Callback to WMS and QOS
    CallbackToWMS(dstCurVisVec);
    SetVSyncRateByVisibleLevel(dstVisMapForVsyncRate, curAllSurfaces);
    // Callback for registered self drawing surfacenode
    SurfaceOcclusionCallback();
}

bool RSMainThread::CheckSurfaceVisChanged(std::map<NodeId, RSVisibleLevel>& visMapForVsyncRate,
    std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    if (!systemAnimatedScenesList_.empty()) {
        visMapForVsyncRate.clear();
        for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
            auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
            if (curSurface == nullptr || curSurface->GetDstRect().IsEmpty() || curSurface->IsLeashWindow()) {
                continue;
            }
            visMapForVsyncRate[curSurface->GetId()] = RSVisibleLevel::RS_SYSTEM_ANIMATE_SCENE;
        }
        isReduceVSyncBySystemAnimatedScenes_ = true;
    }
    bool isVisibleChanged = visMapForVsyncRate.size() != visMapForVsyncRate.size();
    if (!isVisibleChanged) {
        auto iterCur = visMapForVsyncRate.begin();
        auto iterLast = lastVisMapForVsyncRate_.begin();
        for (; iterCur != visMapForVsyncRate.end(); iterCur++, iterLast++) {
            if (iterCur->first != iterLast->first ||
                iterCur->second != iterLast->second) {
                isVisibleChanged = true;
                break;
            }
        }
    }

    if (isVisibleChanged) {
        lastVisMapForVsyncRate_ = visMapForVsyncRate;
    }
    return isVisibleChanged;
}

void RSMainThread::SetVSyncRateByVisibleLevel(std::map<NodeId, RSVisibleLevel>& visMapForVsyncRate,
    std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    if (!vsyncControlEnabled_ || !CheckSurfaceVisChanged(visMapForVsyncRate, curAllSurfaces) ||
        appVSyncDistributor_ == nullptr) {
        return;
    }
    RS_TRACE_NAME_FMT("%s visMapForVsyncRateSize[%lu]", __func__, visMapForVsyncRate.size());
    for (auto iter:visMapForVsyncRate) {
        if (iter.second == RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE) {
            appVSyncDistributor_->SetQosVSyncRate(iter.first, SIMI_VISIBLE_RATE);
        } else if (iter.second == RSVisibleLevel::RS_SYSTEM_ANIMATE_SCENE) {
            appVSyncDistributor_->SetQosVSyncRate(iter.first, SYSTEM_ANIMATED_SECNES_RATE);
        } else if (iter.second == RSVisibleLevel::RS_INVISIBLE) {
            appVSyncDistributor_->SetQosVSyncRate(iter.first, INVISBLE_WINDOW_RATE);
        } else {
            appVSyncDistributor_->SetQosVSyncRate(iter.first, DEFAULT_RATE);
        }
    }
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
            if ((curVisVec[i].first != lastVisVec_[i].first) || (curVisVec[i].second != lastVisVec_[i].second)) {
                visibleChanged = true;
                break;
            }
        }
    }
    if (visibleChanged) {
        std::lock_guard<std::mutex> lock(occlusionMutex_);
        for (auto it = occlusionListeners_.begin(); it != occlusionListeners_.end(); it++) {
            if (it->second) {
                it->second->OnOcclusionVisibleChanged(std::make_shared<RSOcclusionData>(curVisVec));
            }
        }
    }
    lastVisVec_.clear();
    std::swap(lastVisVec_, curVisVec);
}

void RSMainThread::SurfaceOcclusionCallback()
{
    const auto& nodeMap = context_->GetNodeMap();
    for (auto &listener : surfaceOcclusionListeners_) {
        if (savedAppWindowNode_.find(listener.first) == savedAppWindowNode_.end()) {
            auto node = nodeMap.GetRenderNode(listener.first);
            if (!node || !node->IsOnTheTree()) {
                RS_LOGW("RSMainThread::SurfaceOcclusionCallback cannot find surfacenode %{public}"
                    PRIu64 ".", listener.first);
                continue;
            }
            auto appWindowNodeId = node->GetInstanceRootNodeId();
            if (appWindowNodeId == INVALID_NODEID) {
                RS_LOGW("RSMainThread::SurfaceOcclusionCallback surfacenode %{public}"
                    PRIu64 " cannot find app window node.", listener.first);
                continue;
            }
            auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>();
            auto appWindowNode = nodeMap.GetRenderNode(appWindowNodeId)->ReinterpretCastTo<RSSurfaceRenderNode>();
            if (!surfaceNode || !appWindowNode) {
                RS_LOGW("RSMainThread::SurfaceOcclusionCallback ReinterpretCastTo fail.");
                continue;
            }
            savedAppWindowNode_[listener.first] = std::make_pair(surfaceNode, appWindowNode);
        }
        uint8_t level = 0;
        float visibleAreaRatio = 0.0f;
        bool isOnTheTree = savedAppWindowNode_[listener.first].first->IsOnTheTree();
        if (isOnTheTree) {
            const auto& property = savedAppWindowNode_[listener.first].second->GetRenderProperties();
            auto dstRect = property.GetBoundsGeometry()->GetAbsRect();
            if (dstRect.IsEmpty()) {
                continue;
            }
            visibleAreaRatio = static_cast<float>(savedAppWindowNode_[listener.first].second->
                GetVisibleRegionForCallBack().Area()) / static_cast<float>(dstRect.GetWidth() * dstRect.GetHeight());
            auto& partitionVector = std::get<2>(listener.second); // get tuple 2 partition points vector
            bool vectorEmpty = partitionVector.empty();
            if (vectorEmpty && (visibleAreaRatio > 0.0f)) {
                level = 1;
            } else if (!vectorEmpty && ROSEN_EQ(visibleAreaRatio, 1.0f)) {
                level = partitionVector.size();
            } else if (!vectorEmpty && (visibleAreaRatio > 0.0f)) {
                for (const auto &point : partitionVector) {
                    if (visibleAreaRatio > point) {
                        level += 1;
                        continue;
                    }
                    break;
                }
            }
        }
        auto& savedLevel = std::get<3>(listener.second); // tuple 3, check visible is changed
        if (savedLevel != level) {
            RS_LOGD("RSMainThread::SurfaceOcclusionCallback surfacenode: %{public}" PRIu64 ".", listener.first);
            savedLevel = level;
            if (isOnTheTree) {
                std::get<1>(listener.second)->OnSurfaceOcclusionVisibleChanged(visibleAreaRatio);
            }
        }
    }
}

bool RSMainThread::WaitHardwareThreadTaskExecute()
{
    std::unique_lock<std::mutex> lock(hardwareThreadTaskMutex_);
    return hardwareThreadTaskCond_.wait_until(lock, std::chrono::system_clock::now() +
        std::chrono::milliseconds(WAIT_FOR_HARDWARE_THREAD_TASK_TIMEOUT),
        []() { return RSHardwareThread::Instance().GetunExecuteTaskNum() <= HARDWARE_THREAD_TASK_NUM; });
}

void RSMainThread::NotifyHardwareThreadCanExecuteTask()
{
    RS_TRACE_NAME("RSMainThread::NotifyHardwareThreadCanExecuteTask");
    std::lock_guard<std::mutex> lock(hardwareThreadTaskMutex_);
    hardwareThreadTaskCond_.notify_one();
}

void RSMainThread::RequestNextVSync(const std::string& fromWhom, int64_t lastVSyncTS)
{
    RS_OPTIONAL_TRACE_FUNC();
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = [this](uint64_t timestamp, void* data) { OnVsync(timestamp, data); },
    };
    if (receiver_ != nullptr) {
        requestNextVsyncNum_++;
        if (requestNextVsyncNum_ > REQUEST_VSYNC_NUMBER_LIMIT) {
            RS_LOGD("RSMainThread::RequestNextVSync too many times:%{public}d", requestNextVsyncNum_.load());
        }
        receiver_->RequestNextVSync(fcb, fromWhom, lastVSyncTS);
    }
}

void RSMainThread::ProcessScreenHotPlugEvents()
{
    auto screenManager_ = CreateOrGetScreenManager();
    if (!screenManager_) {
        return;
    }

    if (!screenManager_->TrySimpleProcessHotPlugEvents()) {
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            RSHardwareThread::Instance().PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
        } else {
            PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
        }
    }
}

void RSMainThread::OnVsync(uint64_t timestamp, void* data)
{
    isOnVsync_.store(true);
    const int64_t onVsyncStartTime = GetCurrentSystimeMs();
    const int64_t onVsyncStartTimeSteady = GetCurrentSteadyTimeMs();
    RSJankStatsOnVsyncStart(onVsyncStartTime, onVsyncStartTimeSteady);
    timestamp_ = timestamp;
    curTime_ = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    RS_PROFILER_PATCH_TIME(timestamp_);
    RS_PROFILER_PATCH_TIME(curTime_);
    requestNextVsyncNum_ = 0;
    frameCount_++;
    if (isUniRender_) {
        MergeToEffectiveTransactionDataMap(cachedTransactionDataMap_);
        if (RSUnmarshalThread::Instance().CachedTransactionDataEmpty()) {
            // set needWaitUnmarshalFinished_ to false, it means mainLoop do not wait unmarshalBarrierTask_
            needWaitUnmarshalFinished_ = false;
        } else {
            RSUnmarshalThread::Instance().PostTask(unmarshalBarrierTask_);
        }
    }
    mainLoop_();
#if defined(RS_ENABLE_CHIPSET_VSYNC)
    SetVsyncInfo(timestamp);
#endif
    ProcessScreenHotPlugEvents();
    RSJankStatsOnVsyncEnd(onVsyncStartTime, onVsyncStartTimeSteady);
    isOnVsync_.store(false);
}

void RSMainThread::RSJankStatsOnVsyncStart(int64_t onVsyncStartTime, int64_t onVsyncStartTimeSteady)
{
    if (isUniRender_) {
        if (!renderThreadParams_) {
            // fill the params, and sync to render thread later
            renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
        }
        renderThreadParams_->SetIsUniRenderAndOnVsync(true);
        renderThreadParams_->SetOnVsyncStartTime(onVsyncStartTime);
        renderThreadParams_->SetOnVsyncStartTimeSteady(onVsyncStartTimeSteady);
        SetDiscardJankFrames(false);
        SetSkipJankAnimatorFrame(false);
    }
}

void RSMainThread::RSJankStatsOnVsyncEnd(int64_t onVsyncStartTime, int64_t onVsyncStartTimeSteady)
{
    if (isUniRender_ && doDirectComposition_) {
        const JankDurationParams rsParams = { .timeStart_ = onVsyncStartTime,
                                              .timeStartSteady_ = onVsyncStartTimeSteady,
                                              .timeEnd_ = GetCurrentSystimeMs(),
                                              .timeEndSteady_ = GetCurrentSteadyTimeMs(),
                                              .refreshRate_ = GetDynamicRefreshRate(),
                                              .discardJankFrames_ = GetDiscardJankFrames() };
        drawFrame_.PostDirectCompositionJankStats(rsParams);
    }
}

#if defined(RS_ENABLE_CHIPSET_VSYNC)
void RSMainThread::ConnectChipsetVsyncSer()
{
    if (initVsyncServiceFlag_ && (OHOS::Camera::ChipsetVsyncClient::Instance().InitChipsetVsync() == -1)) {
        initVsyncServiceFlag_ = true;
    } else {
        initVsyncServiceFlag_ = false;
    }
}
#endif

#if defined(RS_ENABLE_CHIPSET_VSYNC)
void RSMainThread::SetVsyncInfo(uint64_t timestamp)
{
    int64_t vsyncPeriod = 0;
    if (receiver_) {
        receiver_->GetVSyncPeriod(vsyncPeriod);
    }
    OHOS::Camera::ChipsetVsyncHostCallback::GetInstance()->SetVsync(timestamp, vsyncPeriod);
    RS_LOGD("UpdateVsyncTime = %{public}lld, period = %{public}lld",
        static_cast<long long>(timestamp), static_cast<long long>(vsyncPeriod));
}
#endif

void RSMainThread::Animate(uint64_t timestamp)
{
    RS_TRACE_FUNC();
    lastAnimateTimestamp_ = timestamp;
    rsCurrRange_.Reset();

    if (context_->animatingNodeList_.empty()) {
        doWindowAnimate_ = false;
        return;
    }
    doDirectComposition_ = false;
    bool curWinAnim = false;
    bool needRequestNextVsync = false;
    needRequestNextVsyncAnimate_ = false;
    // isCalculateAnimationValue is embedded modify for stat animate frame drop
    bool isCalculateAnimationValue = false;
    bool isRateDeciderEnabled = (context_->animatingNodeList_.size() <= CAL_NODE_PREFERRED_FPS_LIMIT);
    bool isDisplaySyncEnabled =
        HgmCore::Instance().GetCurrentRefreshRateMode() == HGM_REFRESHRATE_MODE_AUTO ? true : false;
    int64_t period = 0;
    if (receiver_) {
        receiver_->GetVSyncPeriod(period);
    }
    RSRenderAnimation::isCalcAnimateVelocity_ = isRateDeciderEnabled;
    uint32_t totalAnimationSize = 0;
    uint32_t animatingNodeSize = context_->animatingNodeList_.size();
    bool needPrintAnimationDFX = false;
    std::set<pid_t> animationPids;
    if (IsTagEnabled(HITRACE_TAG_GRAPHIC_AGP)) {
        auto screenManager = CreateOrGetScreenManager();
        needPrintAnimationDFX = screenManager != nullptr && screenManager->IsAllScreensPowerOff();
    }
    // iterate and animate all animating nodes, remove if animation finished
    EraseIf(context_->animatingNodeList_,
        [this, timestamp, period, isDisplaySyncEnabled, isRateDeciderEnabled, &totalAnimationSize,
        &curWinAnim, &needRequestNextVsync, &isCalculateAnimationValue, &needPrintAnimationDFX,
        &animationPids](const auto& iter) -> bool {
        auto node = iter.second.lock();
        if (node == nullptr) {
            RS_LOGD("RSMainThread::Animate removing expired animating node");
            return true;
        }
        if (cacheCmdSkippedInfo_.count(ExtractPid(node->GetId())) > 0) {
            rsCurrRange_.Merge(node->animationManager_.GetDecideFrameRateRange());
            RS_LOGD("RSMainThread::Animate skip the cached node");
            return false;
        }
        totalAnimationSize += node->animationManager_.GetAnimationsSize();
        auto frameRateGetFunc = [this](const RSPropertyUnit unit, float velocity) -> int32_t {
            return frameRateMgr_->GetExpectedFrameRate(unit, velocity);
        };
        node->animationManager_.SetRateDeciderEnable(isRateDeciderEnabled, frameRateGetFunc);
        auto [hasRunningAnimation, nodeNeedRequestNextVsync, nodeCalculateAnimationValue] =
            node->Animate(timestamp, period, isDisplaySyncEnabled);
        if (!hasRunningAnimation) {
            node->InActivateDisplaySync();
            RS_LOGD("RSMainThread::Animate removing finished animating node %{public}" PRIu64, node->GetId());
        } else {
            node->UpdateDisplaySyncRange();
            rsCurrRange_.Merge(node->animationManager_.GetDecideFrameRateRange());
        }
        // request vsync if: 1. node has running animation, or 2. transition animation just ended
        needRequestNextVsync = needRequestNextVsync || nodeNeedRequestNextVsync || (node.use_count() == 1);
        isCalculateAnimationValue = isCalculateAnimationValue || nodeCalculateAnimationValue;
        if (node->template IsInstanceOf<RSSurfaceRenderNode>() && hasRunningAnimation) {
            if (isUniRender_) {
                auto surfacenode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
                surfacenode->SetAnimateState();
            }
            curWinAnim = true;
        }
        if (needPrintAnimationDFX && needRequestNextVsync && node->animationManager_.GetAnimationsSize() > 0) {
            animationPids.insert(node->animationManager_.GetAnimationPid());
        }
        return !hasRunningAnimation;
    });
    if (needPrintAnimationDFX && needRequestNextVsync && animationPids.size() > 0) {
        std::string pidList;
        for (const auto& pid : animationPids) {
            pidList += "[" + std::to_string(pid) + "]";
        }
        RS_TRACE_NAME_FMT("Animate from pid %s", pidList.c_str());
    }

    RS_TRACE_NAME_FMT("Animate [nodeSize, totalAnimationSize] is [%lu, %lu]", animatingNodeSize, totalAnimationSize);
    if (!isCalculateAnimationValue && needRequestNextVsync) {
        RS_TRACE_NAME("Animation running empty");
    }

    doWindowAnimate_ = curWinAnim;
    RS_LOGD("RSMainThread::Animate end, animating nodes remains, has window animation: %{public}d", curWinAnim);

    if (needRequestNextVsync) {
        if (!rsVSyncDistributor_->IsDVsyncOn()) {
            RequestNextVSync("animate", timestamp_);
        } else {
            needRequestNextVsyncAnimate_ = true;  // set the member variable instead of directly calling rnv
            RS_TRACE_NAME("rs_RequestNextVSync");
        }
    } else if (isUniRender_) {
        renderThreadParams_->SetImplicitAnimationEnd(true);
    }

    PerfAfterAnim(needRequestNextVsync);
}

bool RSMainThread::IsNeedProcessBySingleFrameComposer(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    if (!isUniRender_ || !rsTransactionData) {
        return false;
    }

    if (!RSSingleFrameComposer::IsShouldProcessByIpcThread(rsTransactionData->GetSendingPid()) &&
        !RSSystemProperties::GetSingleFrameComposerEnabled()) {
        return false;
    }

    // animation node will call RequestNextVsync() in mainLoop_, here we simply ignore animation scenario
    if (!context_->animatingNodeList_.empty()) {
        return false;
    }

    // ignore mult-window scenario
    auto currentVisibleLeashWindowCount = context_->GetNodeMap().GetVisibleLeashWindowCount();
    if (currentVisibleLeashWindowCount >= MULTI_WINDOW_PERF_START_NUM) {
        return false;
    }

    return true;
}

void RSMainThread::ProcessDataBySingleFrameComposer(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    if (!rsTransactionData || !isUniRender_) {
        return;
    }

    if (RSSystemProperties::GetSingleFrameComposerEnabled()) {
        RSSingleFrameComposer::SetSingleFrameFlag(std::this_thread::get_id());
        context_->transactionTimestamp_ = rsTransactionData->GetTimestamp();
        rsTransactionData->ProcessBySingleFrameComposer(*context_);
    }

    RecvAndProcessRSTransactionDataImmediately(rsTransactionData);
}

void RSMainThread::RecvAndProcessRSTransactionDataImmediately(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    if (!rsTransactionData || !isUniRender_) {
        return;
    }
    RS_TRACE_NAME("ProcessBySingleFrameComposer");
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        cachedTransactionDataMap_[rsTransactionData->GetSendingPid()].emplace_back(std::move(rsTransactionData));
    }
    ForceRefreshForUni();
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
    RS_LOGD("RSMainThread::RecvRSTransactionData timestamp = %{public}" PRIu64, timestamp);
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

void RSMainThread::PostTask(RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
    AppExecFwk::EventQueue::Priority priority)
{
    if (handler_) {
        handler_->PostTask(task, name, delayTime, priority);
    }
}

void RSMainThread::RemoveTask(const std::string& name)
{
    if (handler_) {
        handler_->RemoveTask(name);
    }
}

void RSMainThread::PostSyncTask(RSTaskMessage::RSTask task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

bool RSMainThread::IsIdle() const
{
    return handler_ ? handler_->IsIdle() : false;
}

void RSMainThread::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    applicationAgentMap_.emplace(pid, app);
}

void RSMainThread::UnRegisterApplicationAgent(sptr<IApplicationAgent> app)
{
    EraseIf(applicationAgentMap_,
        [&app](const auto& iter) { return iter.second && app && iter.second->AsObject() == app->AsObject(); });
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

void RSMainThread::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints)
{
    std::lock_guard<std::mutex> lock(surfaceOcclusionMutex_);
    uint8_t level = 1;
    if (!partitionPoints.empty()) {
        level = partitionPoints.size();
    }
    surfaceOcclusionListeners_[id] = std::make_tuple(pid, callback, partitionPoints, level);
}

void RSMainThread::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    std::lock_guard<std::mutex> lock(surfaceOcclusionMutex_);
    surfaceOcclusionListeners_.erase(id);
    savedAppWindowNode_.erase(id);
}

void RSMainThread::ClearSurfaceOcclusionChangeCallback(pid_t pid)
{
    std::lock_guard<std::mutex> lock(surfaceOcclusionMutex_);
    for (auto it = surfaceOcclusionListeners_.begin(); it != surfaceOcclusionListeners_.end();) {
        if (std::get<0>(it->second) == pid) {
            if (savedAppWindowNode_.find(it->first) != savedAppWindowNode_.end()) {
                savedAppWindowNode_.erase(it->first);
            }
            surfaceOcclusionListeners_.erase(it++);
        } else {
            it++;
        }
    }
}

void RSMainThread::SurfaceOcclusionChangeCallback(VisibleData& dstCurVisVec)
{
    std::lock_guard<std::mutex> lock(occlusionMutex_);
    for (auto it = occlusionListeners_.begin(); it != occlusionListeners_.end(); it++) {
        if (it->second) {
            it->second->OnOcclusionVisibleChanged(std::make_shared<RSOcclusionData>(dstCurVisVec));
        }
    }
}

bool RSMainThread::SurfaceOcclusionCallBackIfOnTreeStateChanged()
{
    std::vector<NodeId> registeredSurfaceOnTree;
    for (auto it = savedAppWindowNode_.begin(); it != savedAppWindowNode_.end(); ++it) {
        if (it->second.first->IsOnTheTree()) {
            registeredSurfaceOnTree.push_back(it->first);
        }
    }
    if (lastRegisteredSurfaceOnTree_ != registeredSurfaceOnTree) {
        lastRegisteredSurfaceOnTree_ = registeredSurfaceOnTree;
        return true;
    }
    return false;
}

void RSMainThread::SendCommands()
{
    RS_OPTIONAL_TRACE_FUNC();
    RsFrameReport& fr = RsFrameReport::GetInstance();
    if (fr.GetEnable()) {
        fr.SendCommandsStart();
        fr.RenderEnd();
    }
    if (!RSMessageProcessor::Instance().HasTransaction()) {
        return;
    }

    // dispatch messages to corresponding application
    auto transactionMapPtr = std::make_shared<std::unordered_map<uint32_t, std::shared_ptr<RSTransactionData>>>(
        RSMessageProcessor::Instance().GetAllTransactions());
    RSMessageProcessor::Instance().ReInitializeMovedMap();
    PostTask([this, transactionMapPtr]() {
        for (const auto& transactionIter : *transactionMapPtr) {
            auto pid = transactionIter.first;
            auto appIter = applicationAgentMap_.find(pid);
            if (appIter == applicationAgentMap_.end()) {
                RS_LOGW("RSMainThread::SendCommand no application agent registered as pid %{public}d,"
                    "this will cause memory leak!", pid);
                continue;
            }
            auto& app = appIter->second;
            auto transactionPtr = transactionIter.second;
            app->OnTransaction(transactionPtr);
        }
    });
}

void RSMainThread::RenderServiceTreeDump(std::string& dumpString, bool forceDumpSingleFrame)
{
    if (LIKELY(forceDumpSingleFrame)) {
        RS_TRACE_NAME("GetDumpTree");
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

        dumpString += "\n====================================\n";
        RSUniRenderThread::Instance().RenderServiceTreeDump(dumpString);
    } else {
        dumpString += g_dumpStr;
        g_dumpStr = "";
    }
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

    auto children = *rootNode->GetSortedChildren();
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
        RS_LOGD("RSMainThread::ClearTransactionDataPidInfo process:%{public}d destroyed, skip commands", remotePid);
    }
    effectiveTransactionDataIndexMap_.erase(remotePid);
    transactionDataLastWaitTime_.erase(remotePid);

    // clear cpu cache when process exit
    // CLEAN_CACHE_FREQ to prevent multiple cleanups in a short period of time
    if (remotePid != lastCleanCachePid_ ||
        ((timestamp_ - lastCleanCacheTimestamp_) / REFRESH_PERIOD) > CLEAN_CACHE_FREQ) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        RS_LOGD("RSMainThread: clear cpu cache pid:%{public}d", remotePid);
        if (!IsResidentProcess(remotePid)) {
            if (isUniRender_) {
                RSUniRenderThread::Instance().ClearMemoryCache(ClearMemoryMoment::PROCESS_EXIT, true, remotePid);
            } else {
                ClearMemoryCache(ClearMemoryMoment::PROCESS_EXIT, true);
            }
            lastCleanCacheTimestamp_ = timestamp_;
            lastCleanCachePid_ = remotePid;
        }
#endif
    }
}

bool RSMainThread::IsResidentProcess(pid_t pid) const
{
    return pid == ExtractPid(context_->GetNodeMap().GetEntryViewNodeId());
}

void RSMainThread::TrimMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!RSUniRenderJudgement::IsUniRender()) {
        dumpString.append("\n---------------\nNot in UniRender and no resource can be released");
        return;
    }
    std::string type;
    argSets.erase(u"trimMem");
    if (!argSets.empty()) {
        type = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(*argSets.begin());
    }
    RSUniRenderThread::Instance().TrimMem(dumpString, type);
#endif
}

void RSMainThread::DumpNode(std::string& result, uint64_t nodeId) const
{
    const auto& nodeMap = context_->GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (!node) {
        result.append("have no this node");
        return;
    }
    DfxString log;
    node->DumpNodeInfo(log);
    result.append(log.GetString());
}

void RSMainThread::DumpMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString,
    std::string& type, int pid)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    DfxString log;
    if (pid != 0) {
        RSUniRenderThread::Instance().PostSyncTask([&log, pid] {
            RS_TRACE_NAME_FMT("Dumping memory of pid[%d]", pid);
            MemoryManager::DumpPidMemory(log, pid,
                RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
        });
    } else {
        MemoryManager::DumpMemoryUsage(log, type);
    }
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
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    RSUniRenderThread::Instance().PostSyncTask([&mem, pid] {
        RS_TRACE_NAME_FMT("Counting memory of pid[%d]", pid);
        mem = MemoryManager::CountPidMemory(pid,
            RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
    });
#endif
}

void RSMainThread::CountMem(std::vector<MemoryGraphic>& mems)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
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
    MemoryManager::CountMemory(pids, GetRenderEngine()->GetRenderContext()->GetDrGPUContext(), mems);
#endif
}

void RSMainThread::AddTransactionDataPidInfo(pid_t remotePid)
{
    if (!isUniRender_) {
        return;
    }
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    if (effectiveTransactionDataIndexMap_.count(remotePid) > 0) {
        RS_LOGW("RSMainThread::AddTransactionDataPidInfo remotePid:%{public}d already exists", remotePid);
    }
    effectiveTransactionDataIndexMap_[remotePid].first = 0;
}

void RSMainThread::SetDirtyFlag()
{
    isDirty_ = true;
}

void RSMainThread::SetColorPickerForceRequestVsync(bool colorPickerForceRequestVsync)
{
    colorPickerForceRequestVsync_ = colorPickerForceRequestVsync;
}

void RSMainThread::SetNoNeedToPostTask(bool noNeedToPostTask)
{
    noNeedToPostTask_ = noNeedToPostTask;
}

bool RSMainThread::GetNoNeedToPostTask()
{
    return noNeedToPostTask_.load();
}

void RSMainThread::SetAccessibilityConfigChanged()
{
    isAccessibilityConfigChanged_ = true;
}

bool RSMainThread::IsAccessibilityConfigChanged() const
{
    return isAccessibilityConfigChanged_;
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
            RS_PROFILER_PATCH_TIME(now);
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
    handler_->RemoveTask("PerfForBlurIfNeeded");
    static uint64_t prePerfTimestamp = 0;
    static int preBlurCnt = 0;
    static int cnt = 0;
    auto timestamp = timestamp_;
    if (isUniRender_) {
        auto params = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
        if (!params) {
            return;
        }
        timestamp = params->GetCurrentTimestamp();
    }
    auto task = [this, timestamp]() {
        if (preBlurCnt == 0) {
            return;
        }
        RS_OPTIONAL_TRACE_NAME_FMT("PerfForBlurIfNeeded now[%ld] timestamp[%ld] preBlurCnt[%d]",
            std::chrono::steady_clock::now().time_since_epoch(), timestamp, preBlurCnt);
        if (static_cast<uint64_t>(timestamp) - prePerfTimestamp > PERF_PERIOD_BLUR_TIMEOUT && preBlurCnt != 0) {
            PerfRequest(BLUR_CNT_TO_BLUR_CODE.at(preBlurCnt), false);
            prePerfTimestamp = 0;
            preBlurCnt = 0;
        }
    };
    // delay 100ms
    handler_->PostTask(task, "PerfForBlurIfNeeded", 100);
    int blurCnt = isUniRender_ ? RSPropertyDrawableUtils::GetAndResetBlurCnt() :
        RSPropertiesPainter::GetAndResetBlurCnt();
    // clamp blurCnt to 0~3.
    blurCnt = std::clamp<int>(blurCnt, 0, 3);
    if (blurCnt < preBlurCnt) {
        cnt++;
    } else {
        cnt = 0;
    }
    // if blurCnt > preBlurCnt, than change perf code;
    // if blurCnt < preBlurCnt 10 times continuously, than change perf code.
    bool cntIsMatch = blurCnt > preBlurCnt || cnt > 10;
    if (cntIsMatch && preBlurCnt != 0) {
        RS_OPTIONAL_TRACE_NAME_FMT("PerfForBlurIfNeeded Perf close, preBlurCnt[%d] blurCnt[%ld]", preBlurCnt, blurCnt);
        PerfRequest(BLUR_CNT_TO_BLUR_CODE.at(preBlurCnt), false);
        preBlurCnt = blurCnt == 0 ? 0 : preBlurCnt;
    }
    if (blurCnt == 0) {
        return;
    }
    if (timestamp - prePerfTimestamp > PERF_PERIOD_BLUR || cntIsMatch) {
        RS_OPTIONAL_TRACE_NAME_FMT("PerfForBlurIfNeeded PerfRequest, preBlurCnt[%d] blurCnt[%ld]", preBlurCnt, blurCnt);
        PerfRequest(BLUR_CNT_TO_BLUR_CODE.at(blurCnt), true);
        prePerfTimestamp = timestamp;
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

void RSMainThread::RenderFrameStart(uint64_t timestamp)
{
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().RenderStart(timestamp);
    }
    RenderFrameTrace::GetInstance().RenderStartFrameTrace(RS_INTERVAL_NAME);
    int hardwareTid = RSHardwareThread::Instance().GetHardwareTid();
    if (hardwareTid_ != hardwareTid) {
        hardwareTid_ = hardwareTid;
        RsFrameReport::GetInstance().SetFrameParam(EVENT_SET_HARDWARE_UTIL, 0, 0, hardwareTid_);
    }
}

void RSMainThread::SetAppWindowNum(uint32_t num)
{
    appWindowNum_ = num;
}

bool RSMainThread::SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes)
{
    RS_OPTIONAL_TRACE_NAME_FMT("%s systemAnimatedScenes[%u] systemAnimatedScenes_[%u] threeFingerScenesListSize[%d] "
        "systemAnimatedScenesListSize_[%d]", __func__, systemAnimatedScenes,
        systemAnimatedScenes_, threeFingerScenesList_.size(), systemAnimatedScenesList_.size());
    if (systemAnimatedScenes < SystemAnimatedScenes::ENTER_MISSION_CENTER ||
            systemAnimatedScenes > SystemAnimatedScenes::OTHERS) {
        RS_LOGD("RSMainThread::SetSystemAnimatedScenes Out of range.");
        return false;
    }
    systemAnimatedScenes_ = systemAnimatedScenes;
    if (!systemAnimatedScenesEnabled_) {
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(systemAnimatedScenesMutex_);
        if (systemAnimatedScenes == SystemAnimatedScenes::OTHERS) {
            if (!threeFingerScenesList_.empty()) {
                threeFingerScenesList_.pop_front();
            }
            if (!systemAnimatedScenesList_.empty()) {
                systemAnimatedScenesList_.pop_front();
            }
        } else {
            uint64_t curTime = static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()).count());
            if (systemAnimatedScenes == SystemAnimatedScenes::ENTER_TFS_WINDOW ||
                systemAnimatedScenes == SystemAnimatedScenes::EXIT_TFU_WINDOW ||
                systemAnimatedScenes == SystemAnimatedScenes::ENTER_WIND_CLEAR ||
                systemAnimatedScenes == SystemAnimatedScenes::ENTER_WIND_RECOVER) {
                threeFingerScenesList_.push_back(std::make_pair(systemAnimatedScenes, curTime));
            }
            if (systemAnimatedScenes != SystemAnimatedScenes::APPEAR_MISSION_CENTER) {
                systemAnimatedScenesList_.push_back(std::make_pair(systemAnimatedScenes, curTime));
            }
        }
    }
    return true;
}

SystemAnimatedScenes RSMainThread::GetSystemAnimatedScenes()
{
    return systemAnimatedScenes_;
}

bool RSMainThread::CheckNodeHasToBePreparedByPid(NodeId nodeId, bool isClassifyByRoot)
{
    std::lock_guard<std::mutex> lock(context_->activeNodesInRootMutex_);
    if (context_->activeNodesInRoot_.empty() || nodeId == INVALID_NODEID) {
        return false;
    }
    if (!isClassifyByRoot) {
        // Match by PID
        auto pid = ExtractPid(nodeId);
        return std::any_of(context_->activeNodesInRoot_.begin(), context_->activeNodesInRoot_.end(),
            [pid](const auto& iter) { return ExtractPid(iter.first) == pid; });
    } else {
        return context_->activeNodesInRoot_.count(nodeId);
    }
}

bool RSMainThread::IsDrawingGroupChanged(RSRenderNode& cacheRootNode) const
{
    std::lock_guard<std::mutex> lock(context_->activeNodesInRootMutex_);
    auto iter = context_->activeNodesInRoot_.find(cacheRootNode.GetInstanceRootNodeId());
    if (iter != context_->activeNodesInRoot_.end()) {
        const auto& activeNodeIds = iter->second;
        // do not need to check cacheroot node itself
        // in case of tree change, parent node would set content dirty and reject before
        auto cacheRootId = cacheRootNode.GetId();
        auto groupNodeIds = cacheRootNode.GetVisitedCacheRootIds();
        for (auto [id, subNode] : activeNodeIds) {
            auto node = subNode.lock();
            if (node == nullptr || id == cacheRootId) {
                continue;
            }
            if (groupNodeIds.find(node->GetDrawingCacheRootId()) != groupNodeIds.end()) {
                return true;
            }
        }
    }
    return false;
}

void RSMainThread::CheckAndUpdateInstanceContentStaticStatus(std::shared_ptr<RSSurfaceRenderNode> instanceNode) const
{
    if (instanceNode == nullptr) {
        RS_LOGE("CheckAndUpdateInstanceContentStaticStatus instanceNode invalid.");
        return ;
    }
    std::lock_guard<std::mutex> lock(context_->activeNodesInRootMutex_);
    auto iter = context_->activeNodesInRoot_.find(instanceNode->GetId());
    if (iter != context_->activeNodesInRoot_.end()) {
        instanceNode->UpdateSurfaceCacheContentStatic(iter->second);
    } else {
        instanceNode->UpdateSurfaceCacheContentStatic();
    }
}

void RSMainThread::ResetHardwareEnabledState()
{
    isHardwareForcedDisabled_ = !RSSystemProperties::GetHardwareComposerEnabled();
    isLastFrameDirectComposition_ = doDirectComposition_;
    doDirectComposition_ = !isHardwareForcedDisabled_;
    isHardwareEnabledBufferUpdated_ = false;
    hardwareEnabledNodes_.clear();
    selfDrawingNodes_.clear();
}

void RSMainThread::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool flag)
{
    std::lock_guard<std::mutex> lock(watermarkMutex_);
    watermarkFlag_ = flag;
    if (flag) {
        watermarkImg_ = RSPixelMapUtil::ExtractDrawingImage(std::move(watermarkImg));
    } else {
        watermarkImg_ = nullptr;
    }
    SetDirtyFlag();
    RequestNextVSync();
}

std::shared_ptr<Drawing::Image> RSMainThread::GetWatermarkImg()
{
    return watermarkImg_;
}

bool RSMainThread::GetWatermarkFlag()
{
    return watermarkFlag_;
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

void RSMainThread::UpdateRogSizeIfNeeded()
{
    if (!RSSystemProperties::IsPhoneType() || RSSystemProperties::IsFoldScreenFlag()) {
        return;
    }
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (!rootNode) {
        return;
    }
    auto child = rootNode->GetFirstChild();
    if (child != nullptr && child->IsInstanceOf<RSDisplayRenderNode>()) {
        auto displayNode = child->ReinterpretCastTo<RSDisplayRenderNode>();
        if (displayNode) {
            auto screenManager_ = CreateOrGetScreenManager();
            screenManager_->SetRogScreenResolution(
                displayNode->GetScreenId(), displayNode->GetRogWidth(), displayNode->GetRogHeight());
        }
    }
}

void RSMainThread::UpdateDisplayNodeScreenId()
{
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (!rootNode) {
        RS_LOGE("RSMainThread::UpdateDisplayNodeScreenId rootNode is nullptr");
        return;
    }
    auto child = rootNode->GetFirstChild();
    if (child != nullptr && child->IsInstanceOf<RSDisplayRenderNode>()) {
        auto displayNode = child->ReinterpretCastTo<RSDisplayRenderNode>();
        if (displayNode) {
            displayNodeScreenId_ = displayNode->GetScreenId();
        }
    }
}

const uint32_t UIFIRST_MINIMUM_NODE_NUMBER = 4; // minimum window number(4) for enabling UIFirst
const uint32_t FOLD_DEVICE_SCREEN_NUMBER = 2; // alt device has two screens

void RSMainThread::UpdateUIFirstSwitch()
{
    if (RSSystemProperties::GetUIFirstForceEnabled()) {
        isUiFirstOn_ = true;
        return;
    }

    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (!rootNode) {
        RSUifirstManager::Instance().SetUiFirstSwitch(isUiFirstOn_);
        return;
    }
    auto firstChildren = rootNode->GetFirstChild();
    if (!firstChildren) {
        RSUifirstManager::Instance().SetUiFirstSwitch(isUiFirstOn_);
        return;
    }
    auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(firstChildren);
    if (!displayNode) {
        RSUifirstManager::Instance().SetUiFirstSwitch(isUiFirstOn_);
        return;
    }
    auto screenManager_ = CreateOrGetScreenManager();
    uint32_t actualScreensNum = screenManager_->GetActualScreensNum();
    if (deviceType_ != DeviceType::PC) {
        if (deviceType_ == DeviceType::PHONE && hasProtectedLayer_) {
            isUiFirstOn_ = false;
        } else if (isFoldScreenDevice_) {
            isUiFirstOn_ = (RSSystemProperties::GetUIFirstEnabled() && actualScreensNum == FOLD_DEVICE_SCREEN_NUMBER);
        } else {
            isUiFirstOn_ = (RSSystemProperties::GetUIFirstEnabled() && actualScreensNum == 1);
        }
        RSUifirstManager::Instance().SetUiFirstSwitch(isUiFirstOn_);
        return;
    }
    isUiFirstOn_ = false;
    if (IsSingleDisplay()) {
        uint32_t LeashWindowCount = 0;
        displayNode->CollectSurfaceForUIFirstSwitch(LeashWindowCount, UIFIRST_MINIMUM_NODE_NUMBER);
        isUiFirstOn_ = RSSystemProperties::GetUIFirstEnabled() && LeashWindowCount >=  UIFIRST_MINIMUM_NODE_NUMBER;
    }
    RSUifirstManager::Instance().SetUiFirstSwitch(isUiFirstOn_);
}

bool RSMainThread::IsUIFirstOn() const
{
    return isUiFirstOn_;
}

void RSMainThread::ReleaseSurface()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (tmpSurfaces_.size() > 0) {
        auto tmp = tmpSurfaces_.front();
        tmpSurfaces_.pop();
        tmp = nullptr;
    }
}

void RSMainThread::AddToReleaseQueue(std::shared_ptr<Drawing::Surface>&& surface)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tmpSurfaces_.push(std::move(surface));
}

void RSMainThread::GetAppMemoryInMB(float& cpuMemSize, float& gpuMemSize)
{
    PostSyncTask([&cpuMemSize, &gpuMemSize, this]() {
        gpuMemSize = MemoryManager::GetAppGpuMemoryInMB(GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
        cpuMemSize = MemoryTrack::Instance().GetAppMemorySizeInMB();
    });
}

void RSMainThread::SubscribeAppState()
{
    PostTask(
        [this]() {
            rsAppStateListener_ = std::make_shared<RSAppStateListener>();
            if (Memory::MemMgrClient::GetInstance().SubscribeAppState(*rsAppStateListener_) != -1) {
                RS_LOGD("Subscribe MemMgr Success");
                subscribeFailCount_ = 0;
                return;
            } else {
                RS_LOGE("Subscribe Failed, try again");
                subscribeFailCount_++;
                if (subscribeFailCount_ < 10) { // The maximum number of failures is 10
                    SubscribeAppState();
                } else {
                    RS_LOGE("Subscribe Failed 10 times, exiting");
                }
            }
        },
        MEM_MGR, WAIT_FOR_MEM_MGR_SERVICE);
}

void RSMainThread::HandleOnTrim(Memory::SystemMemoryLevel level)
{
    if (handler_) {
        handler_->PostTask(
            [level, this]() {
                RS_LOGD("Enter level:%{public}d, OnTrim Success", level);
                RS_TRACE_NAME_FMT("System is low memory, HandleOnTrim Enter level:%d", level);
                switch (level) {
                    case Memory::SystemMemoryLevel::MEMORY_LEVEL_CRITICAL:
                        if (isUniRender_) {
                            RSUniRenderThread::Instance().ClearMemoryCache(ClearMemoryMoment::LOW_MEMORY, true);
                        } else {
                            ClearMemoryCache(ClearMemoryMoment::LOW_MEMORY, true);
                        }
                        break;
                    case Memory::SystemMemoryLevel::MEMORY_LEVEL_LOW:
                    case Memory::SystemMemoryLevel::MEMORY_LEVEL_MODERATE:
                    case Memory::SystemMemoryLevel::MEMORY_LEVEL_PURGEABLE:
                        break;
                    default:
                        break;
                }
            },
            AppExecFwk::EventQueue::Priority::IDLE);
    }
}

void RSMainThread::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    isCurtainScreenOn_ = isCurtainScreenOn;
    SetDirtyFlag();
    RequestNextVSync();
    RS_LOGD("RSMainThread::SetCurtainScreenUsingStatus %{public}d", isCurtainScreenOn);
}

bool RSMainThread::IsCurtainScreenOn() const
{
    return isCurtainScreenOn_;
}

int64_t RSMainThread::GetCurrentSystimeMs() const
{
    auto curTime = std::chrono::system_clock::now().time_since_epoch();
    int64_t curSysTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime).count();
    return curSysTime;
}

int64_t RSMainThread::GetCurrentSteadyTimeMs() const
{
    auto curTime = std::chrono::steady_clock::now().time_since_epoch();
    int64_t curSteadyTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime).count();
    return curSteadyTime;
}
} // namespace Rosen
} // namespace OHOS
