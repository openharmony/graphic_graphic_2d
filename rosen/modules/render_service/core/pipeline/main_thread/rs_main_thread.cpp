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

#include "pipeline/main_thread/rs_main_thread.h"

#include <algorithm>
#include <cstdint>
#include <list>
#include <malloc.h>
#include <parameters.h>
#include <securec.h>
#include <stdint.h>
#include <string>
#include <unistd.h>

#include "app_mgr_client.h"
#include "delegate/rs_functional_delegate.h"
#include "hgm_energy_consumption_policy.h"
#include "hgm_frame_rate_manager.h"
#include "include/core/SkGraphics.h"
#include "include/gpu/GrDirectContext.h"
#include "mem_mgr_client.h"
#include "render_frame_trace.h"
#include "rs_frame_report.h"
#include "rs_profiler.h"
#include "rs_trace.h"
#include "hisysevent.h"
#include "scene_board_judgement.h"
#include "vsync_iconnection_token.h"
#include "xcollie/watchdog.h"

#include "animation/rs_animation_fraction.h"
#include "command/rs_animation_command.h"
#include "command/rs_message_processor.h"
#include "command/rs_node_command.h"
#include "common/rs_background_thread.h"
#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "display_engine/rs_color_temperature.h"
#include "display_engine/rs_luminance_control.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "feature/hdr/rs_hdr_util.h"
#include "feature/anco_manager/rs_anco_manager.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "gfx/performance/rs_perfmonitor_reporter.h"
#include "graphic_feature_param_manager.h"
#include "info_collection/rs_gpu_dirty_region_collection.h"
#include "memory/rs_memory_graphic.h"
#include "memory/rs_memory_manager.h"
#include "memory/rs_memory_track.h"
#include "metadata_helper.h"
#include "monitor/self_drawing_node_monitor.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/render_thread/rs_divided_render_util.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/rs_occlusion_config.h"
#include "pipeline/rs_pointer_window_manager.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/main_thread/rs_render_service_visitor.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/rs_unmarshal_task_manager.h"
#include "rs_frame_rate_vote.h"
#include "singleton.h"
#ifdef RS_ENABLE_VK
#include "pipeline/rs_vk_pipeline_config.h"
#endif
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/sk_resource_manager.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "pipeline/magic_pointer_render/rs_magic_pointer_render_manager.h"
#endif
#include "platform/common/rs_innovation.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/drawing/rs_vsync_client.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "platform/ohos/rs_jank_stats.h"
#include "property/rs_point_light_manager.h"
#include "property/rs_properties_painter.h"
#include "property/rs_property_trace.h"
#include "render/rs_image_cache.h"
#include "render/rs_pixel_map_util.h"
#include "render/rs_typeface_cache.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_transaction_metric_collector.h"
#include "transaction/rs_transaction_proxy.h"
#include "transaction/rs_unmarshal_thread.h"

#ifdef RS_ENABLE_GPU
#include "feature/capture/rs_ui_capture_task_parallel.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/main_thread/rs_uni_render_visitor.h"
#endif

#ifdef RS_ENABLE_GL
#include "GLES3/gl3.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#endif

#ifdef RS_ENABLE_PARALLEL_UPLOAD
#include "rs_upload_resource_thread.h"
#endif

#if defined(ACCESSIBILITY_ENABLE)
#include "accessibility_config.h"
#endif

#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif

#if defined(RS_ENABLE_CHIPSET_VSYNC)
#include "chipset_vsync_impl.h"
#endif
#ifdef RES_SCHED_ENABLE
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include <iservice_registry.h>
#include "ressched_event_listener.h"
#endif

// cpu boost
#include "c/ffrt_cpu_boost.h"
// blur predict
#include "rs_frame_blur_predict.h"
#include "rs_frame_deadline_predict.h"
#include "feature_cfg/feature_param/extend_feature/mem_param.h"

#include "feature_cfg/graphic_feature_param_manager.h"
#include "feature_cfg/feature_param/feature_param.h"

using namespace FRAME_TRACE;
static const std::string RS_INTERVAL_NAME = "renderservice";

#if defined(ACCESSIBILITY_ENABLE)
using namespace OHOS::AccessibilityConfig;
#endif

#undef LOG_TAG
#define LOG_TAG "RSMainThread"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t VSYNC_LOG_ENABLED_TIMES_THRESHOLD = 500;
constexpr uint32_t VSYNC_LOG_ENABLED_STEP_TIMES = 100;
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
constexpr uint32_t WAIT_FOR_UNMARSHAL_THREAD_TASK_TIMEOUT = 4000;
constexpr uint32_t WATCHDOG_TIMEVAL = 5000;
constexpr uint32_t HARDWARE_THREAD_TASK_NUM = 2;
constexpr uint32_t NODE_DUMP_STRING_LEN = 8;
constexpr int32_t SIMI_VISIBLE_RATE = 2;
constexpr int32_t DEFAULT_RATE = 1;
constexpr int32_t INVISBLE_WINDOW_RATE = 10;
constexpr int32_t MAX_CAPTURE_COUNT = 5;
constexpr int32_t SYSTEM_ANIMATED_SCENES_RATE = 2;
constexpr uint32_t CAL_NODE_PREFERRED_FPS_LIMIT = 50;
constexpr uint32_t EVENT_SET_HARDWARE_UTIL = 100004;
constexpr uint32_t EVENT_NAME_MAX_LENGTH = 50;
constexpr uint32_t HIGH_32BIT = 32;
constexpr uint64_t PERIOD_MAX_OFFSET = 1000000; // 1ms
constexpr const char* WALLPAPER_VIEW = "WallpaperView";
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
constexpr const char* DESKTOP_NAME_FOR_ROTATION = "SCBDesktop";
const std::string PERF_FOR_BLUR_IF_NEEDED_TASK_NAME = "PerfForBlurIfNeeded";
constexpr const char* CAPTURE_WINDOW_NAME = "CapsuleWindow";
constexpr const char* HIDE_NOTCH_STATUS = "persist.sys.graphic.hideNotch.status";
constexpr const char* DRAWING_CACHE_DFX = "rosen.drawingCache.enabledDfx";
constexpr const char* DEFAULT_SURFACE_NODE_NAME = "DefaultSurfaceNodeName";
#ifdef RS_ENABLE_GL
constexpr size_t DEFAULT_SKIA_CACHE_SIZE        = 96 * (1 << 20);
constexpr int DEFAULT_SKIA_CACHE_COUNT          = 2 * (1 << 12);
#endif
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
constexpr const char* MEM_GPU_TYPE = "gpu";
#endif
constexpr size_t MEMUNIT_RATE = 1024;
constexpr size_t MAX_GPU_CONTEXT_CACHE_SIZE = 1024 * MEMUNIT_RATE * MEMUNIT_RATE;   // 1G
constexpr uint32_t REQUEST_VSYNC_DUMP_NUMBER = 100;
const std::string DVSYNC_NOTIFY_UNMARSHAL_TASK_NAME = "DVSyncNotifyUnmarshalTask";
const std::map<int, int32_t> BLUR_CNT_TO_BLUR_CODE {
    { 1, 10021 },
    { 2, 10022 },
    { 3, 10023 },
};

static int64_t SystemTime()
{
    timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; // 1000000000ns == 1s
}

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

#ifdef RS_ENABLE_GPU
// rcd node will be handled by RS tree in OH 6.0 rcd refactoring, should remove this later
void DoScreenRcdTask(RSDisplayRenderNode& displayNode, std::shared_ptr<RSProcessor> &processor)
{
    if (processor == nullptr) {
        RS_LOGD("DoScreenRcdTask has no processor");
        return;
    }
    auto screenInfo =
        static_cast<RSDisplayRenderParams *>(displayNode.GetStagingRenderParams().get())->GetScreenInfo();
    if (screenInfo.state != ScreenState::HDI_OUTPUT_ENABLE) {
        RS_LOGD("DoScreenRcdTask is not at HDI_OUPUT mode");
        return;
    }
    auto hardInfo = RSSingleton<RoundCornerDisplayManager>::GetInstance().GetHardwareInfo(displayNode.GetId());
        auto rcdNodeTop = std::static_pointer_cast<RSRcdSurfaceRenderNode>(displayNode.GetRcdSurfaceNodeTop());
    if (rcdNodeTop) {
        rcdNodeTop->DoProcessRenderMainThreadTask(hardInfo.resourceChanged, processor);
    } else {
        RS_LOGD("Top rcdnode is null");
    }
    auto rcdNodeBottom = std::static_pointer_cast<RSRcdSurfaceRenderNode>(displayNode.GetRcdSurfaceNodeBottom());
    if (rcdNodeBottom) {
        rcdNodeBottom->DoProcessRenderMainThreadTask(hardInfo.resourceChanged, processor);
    } else {
        RS_LOGD("Bottom rcdnode is null");
    }
}
#endif

class RSEventDumper : public AppExecFwk::Dumper {
public:
    void Dump(const std::string &message) override
    {
        dumpString.append(message);
    }

    std::string GetTag() override
    {
        return std::string("RSEventDumper");
    }

    std::string GetOutput() const
    {
        return dumpString;
    }
private:
    std::string dumpString = "";
};

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
        ColorFilterMode mode = ColorFilterMode::COLOR_FILTER_END;
        if (id == CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER) {
            if (!AccessibilityParam::IsColorCorrectionEnabled()) {
                RS_LOGE("RSAccessibility ColorCorrectionEnabled is not supported");
                return;
            }

            RS_LOGI("RSAccessibility DALTONIZATION_COLOR_FILTER: %{public}d",
                static_cast<int>(value.daltonizationColorFilter));
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
            if (!AccessibilityParam::IsColorReverseEnabled()) {
                RS_LOGE("RSAccessibility ColorReverseEnabled is not supported");
                return;
            }

            RS_LOGI("RSAccessibility INVERT_COLOR: %{public}d", static_cast<int>(value.invertColor));
            mode = value.invertColor ? ColorFilterMode::INVERT_COLOR_ENABLE_MODE :
                                        ColorFilterMode::INVERT_COLOR_DISABLE_MODE;
            RSBaseRenderEngine::SetColorFilterMode(mode);
        } else if (id == CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT) {
            if (!AccessibilityParam::IsHighContrastEnabled()) {
                RS_LOGE("RSAccessibility HighContrastEnabled is not supported");
                return;
            }

            RS_LOGI("RSAccessibility HIGH_CONTRAST: %{public}d", static_cast<int>(value.highContrastText));
            RSBaseRenderEngine::SetHighContrast(value.highContrastText);
        } else {
            RS_LOGW("RSAccessibility configId: %{public}d is not supported yet.", id);
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

RSMainThread::RSMainThread() : rsParallelType_(RSSystemParameters::GetRsParallelType()),
    mainThreadId_(std::this_thread::get_id())
{
    context_ = std::make_shared<RSContext>();
    context_->Initialize();
}

RSMainThread::~RSMainThread() noexcept
{
    RSNodeCommandHelper::SetCommitDumpNodeTreeProcessor(nullptr);
    RemoveRSEventDetector();
    RSInnovation::CloseInnovationSo();
    if (rsAppStateListener_) {
        Memory::MemMgrClient::GetInstance().UnsubscribeAppState(*rsAppStateListener_);
    }
}

void RSMainThread::TraverseCanvasDrawingNodesNotOnTree()
{
    const auto& nodeMap = context_->GetNodeMap();
    nodeMap.TraverseCanvasDrawingNodes([](const std::shared_ptr<RSCanvasDrawingRenderNode>& canvasDrawingNode) {
        if (canvasDrawingNode == nullptr) {
            return;
        }
        canvasDrawingNode->ContentStyleSlotUpdate();
    });
}

void RSMainThread::Init()
{
    mainLoop_ = [&]() {
        RS_PROFILER_ON_FRAME_BEGIN(timestamp_);
        if (isUniRender_ && !renderThreadParams_) {
#ifdef RS_ENABLE_GPU
            // fill the params, and sync to render thread later
            renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
#endif
        }
        RenderFrameStart(timestamp_);
        RSRenderNodeGC::Instance().SetGCTaskEnable(true);
        PerfMultiWindow();
        SetRSEventDetectorLoopStartTag();
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSMainThread::DoComposition: " + std::to_string(curTime_));
        ConsumeAndUpdateAllNodes();
        ClearNeedDropframePidList();
        WaitUntilUnmarshallingTaskFinished();
        ProcessCommand();
        RsFrameBlurPredict::GetInstance().AdjustCurrentFrameDrawLargeAreaBlurFrequencyPredictively();
        UpdateSubSurfaceCnt();
        Animate(timestamp_);
        CollectInfoForHardwareComposer();
#ifdef RS_ENABLE_GPU
        RSUifirstManager::Instance().PrepareCurrentFrameEvent();
#endif
        ProcessHgmFrameRate(timestamp_);
        RS_PROFILER_ON_RENDER_BEGIN();
        // cpu boost feature start
        ffrt_cpu_boost_start(CPUBOOST_START_POINT);
        // may mark rsnotrendering
        Render(); // now render is traverse tree to prepare
        RsFrameBlurPredict::GetInstance().UpdateCurrentFrameDrawLargeAreaBlurFrequencyPrecisely();
        // cpu boost feature end
        ffrt_cpu_boost_end(CPUBOOST_START_POINT);
        TraverseCanvasDrawingNodesNotOnTree();
        RS_PROFILER_ON_RENDER_END();
        OnUniRenderDraw();
        UIExtensionNodesTraverseAndCallback();
        if (!isUniRender_) {
            RSMainThread::GPUCompositonCacheGuard guard;
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
        ResetAnimateNodeFlag();
        SKResourceManager::Instance().ReleaseResource();
        // release batched node from render tree (enabled by default, can be disabled in RSSystemProperties)
        RSRenderNodeGC::Instance().ReleaseFromTree();
        // release node memory
        RSRenderNodeGC::Instance().ReleaseNodeMemory();
        if (!isUniRender_) {
            RSRenderNodeGC::Instance().ReleaseDrawableMemory();
        }
        if (!RSImageCache::Instance().CheckUniqueIdIsEmpty()) {
            static std::function<void()> task = []() -> void {
                RSImageCache::Instance().ReleaseUniqueIdList();
            };
            RSBackgroundThread::Instance().PostTask(task);
        }
#ifdef RS_ENABLE_PARALLEL_UPLOAD
        RSUploadResourceThread::Instance().OnRenderEnd();
#endif
        RSTypefaceCache::Instance().HandleDelayDestroyQueue();
#if defined(RS_ENABLE_CHIPSET_VSYNC)
        ConnectChipsetVsyncSer();
#endif
#ifdef RS_ENABLE_VK
        if (needCreateVkPipeline_) {
            needCreateVkPipeline_ = false;
            RSBackgroundThread::Instance().PostTask([]() {
                Rosen::RDC::RDCConfig rdcConfig;
                rdcConfig.LoadAndAnalyze(std::string(Rosen::RDC::CONFIG_XML_FILE));
            });
        }
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
    {
        using namespace std::placeholders;
        RSNodeCommandHelper::SetCommitDumpNodeTreeProcessor(
            std::bind(&RSMainThread::OnCommitDumpClientNodeTree, this, _1, _2, _3, _4));
    }
    Drawing::DrawSurfaceBufferOpItem::RegisterSurfaceBufferCallback({
        .OnFinish = RSSurfaceBufferCallbackManager::Instance().GetOnFinishCb(),
        .OnAfterAcquireBuffer = RSSurfaceBufferCallbackManager::Instance().GetOnAfterAcquireBufferCb(),
    });
    RSSurfaceBufferCallbackManager::Instance().SetIsUniRender(true);
#ifdef RS_ENABLE_GPU
    RSSurfaceBufferCallbackManager::Instance().SetRunPolicy([](auto task) {
        RSHardwareThread::Instance().PostTask(task);
    });
#endif
    RSSurfaceBufferCallbackManager::Instance().SetVSyncFuncs({
        .requestNextVsync = []() {
            RSMainThread::Instance()->RequestNextVSync();
        },
        .isRequestedNextVSync = []() {
            return RSMainThread::Instance()->IsRequestedNextVSync();
        },
    });

    isUniRender_ = RSUniRenderJudgement::IsUniRender();
    SetDeviceType();
    isFoldScreenDevice_ = RSSystemProperties::IsFoldScreenFlag();
    auto taskDispatchFunc = [](const RSTaskDispatcher::RSTask& task, bool isSyncTask = false) {
        RSMainThread::Instance()->PostTask(task);
    };
    context_->SetTaskRunner(taskDispatchFunc);
    rsVsyncRateReduceManager_.Init(appVSyncDistributor_);
    if (isUniRender_) {
#ifdef RS_ENABLE_GPU
        auto rtTaskDispatchFunc = [](const RSTaskDispatcher::RSTask& task) {
            RSUniRenderThread::Instance().PostRTTask(task);
        };
        context_->SetRTTaskRunner(rtTaskDispatchFunc);
#endif
    }
    context_->SetVsyncRequestFunc([]() {
        RSMainThread::Instance()->RequestNextVSync();
        RSMainThread::Instance()->SetDirtyFlag();
    });
    RSTaskDispatcher::GetInstance().RegisterTaskDispatchFunc(gettid(), taskDispatchFunc);
    RsFrameReport::GetInstance().Init();
    RSSystemProperties::WatchSystemProperty(HIDE_NOTCH_STATUS, OnHideNotchStatusCallback, nullptr);
    RSSystemProperties::WatchSystemProperty(DRAWING_CACHE_DFX, OnDrawingCacheDfxSwitchCallback, nullptr);
    if (isUniRender_) {
#ifdef RS_ENABLE_GPU
        unmarshalBarrierTask_ = [this]() {
            auto cachedTransactionData = RSUnmarshalThread::Instance().GetCachedTransactionData();
            MergeToEffectiveTransactionDataMap(cachedTransactionData);
            {
                std::lock_guard<std::mutex> lock(unmarshalMutex_);
                ++unmarshalFinishedCount_;
                waitForDVSyncFrame_.store(false);
            }
            unmarshalTaskCond_.notify_all();
        };
        RSUnmarshalThread::Instance().Start();
#endif
    }

    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    uint32_t timeForWatchDog = WATCHDOG_TIMEVAL;
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread("RenderService", handler_, timeForWatchDog);
    if (ret != 0) {
        RS_LOGW("Add watchdog thread failed");
    }
#ifdef RES_SCHED_ENABLE
    SubScribeSystemAbility();
#endif
    InitRSEventDetector();
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    sptr<VSyncConnection> conn = new VSyncConnection(rsVSyncDistributor_, "rs", token->AsObject());
    rsFrameRateLinker_ = std::make_shared<RSRenderFrameRateLinker>([this] (const RSRenderFrameRateLinker& linker) {
        HgmCore::Instance().SetHgmTaskFlag(true);
    });
    conn->id_ = rsFrameRateLinker_->GetId();
    rsVSyncDistributor_->AddConnection(conn);
    receiver_ = std::make_shared<VSyncReceiver>(conn, token->AsObject(), handler_, "rs");
    receiver_->Init();
    if (!isUniRender_) {
        renderEngine_ = std::make_shared<RSRenderEngine>();
        renderEngine_->Init();
    }
    RSOpincManager::Instance().ReadOPIncCcmParam();
    RSUifirstManager::Instance().ReadUIFirstCcmParam();
    auto PostTaskProxy = [](RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
        AppExecFwk::EventQueue::Priority priority) {
        RSMainThread::Instance()->PostTask(task, name, delayTime, priority);
    };
    RSRenderNodeGC::Instance().SetMainTask(PostTaskProxy);
    auto GCNotifyTaskProxy = [](bool isEnable) {
        RSRenderNodeGC::Instance().SetGCTaskEnable(isEnable);
    };
    conn->SetGCNotifyTask(GCNotifyTaskProxy);
#ifdef RS_ENABLE_GL
    /* move to render thread ? */
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        int cacheLimitsTimes = 3;
        auto gpuContext = isUniRender_? GetRenderEngine()->GetRenderContext()->GetDrGPUContext() :
            renderEngine_->GetRenderContext()->GetDrGPUContext();
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
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#if defined (RS_ENABLE_VK)
    RSMagicPointerRenderManager::InitInstance(GetRenderEngine()->GetVkImageManager());
#endif

#if defined (RS_ENABLE_GL) && defined (RS_ENABLE_EGLIMAGE)
    RSMagicPointerRenderManager::InitInstance(GetRenderEngine()->GetEglImageManager());
#endif
#endif

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

    RSDisplayRenderNode::SetReleaseTask(&impl::RSScreenManager::ReleaseScreenDmaBuffer);
    RSDisplayRenderNode::SetScreenStatusNotifyTask([](bool status) {
        sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
        if (screenManager == nullptr) {
            RS_LOGE("RSMainThread::Init screenManager is nullptr");
            return;
        }
        screenManager->SetScreenSwitchStatus(status);
    });
    auto delegate = RSFunctionalDelegate::Create();
    delegate->SetRepaintCallback([this]() {
        bool isOverDrawEnabled = RSOverdrawController::GetInstance().IsEnabled();
        PostTask([this, isOverDrawEnabled]() {
            SetDirtyFlag();
            isOverDrawEnabledOfCurFrame_ = isOverDrawEnabled;
            RequestNextVSync("OverDrawUpdate");
        });
    });
    RSOverdrawController::GetInstance().SetDelegate(delegate);

    HgmTaskHandleThread::Instance().PostSyncTask([this] () {
        auto frameRateMgr = OHOS::Rosen::HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr == nullptr) {
            return;
        }
        frameRateMgr->SetForceUpdateCallback([this](bool idleTimerExpired, bool forceUpdate) {
            RSMainThread::Instance()->PostTask([this, idleTimerExpired, forceUpdate]() {
                RS_TRACE_NAME_FMT("RSMainThread::TimerExpiredCallback Run idleTimerExpiredFlag: %s forceUpdateFlag: %s",
                    idleTimerExpired? "True":"False", forceUpdate? "True": "False");
                RSMainThread::Instance()->SetForceUpdateUniRenderFlag(forceUpdate);
                RSMainThread::Instance()->SetIdleTimerExpiredFlag(idleTimerExpired);
                RSMainThread::Instance()->RequestNextVSync("ltpoForceUpdate");
            });
        });
        frameRateMgr->Init(rsVSyncController_, appVSyncController_, vsyncGenerator_);
    });
    SubscribeAppState();
    PrintCurrentStatus();
    UpdateGpuContextCacheSize();
    RSLuminanceControl::Get().Init();
    RSColorTemperature::Get().Init();
    // used to force refresh screen when cct is updated
    std::function<void()> refreshFunc = []() {
        RSMainThread::Instance()->SetDirtyFlag();
        RSMainThread::Instance()->RequestNextVSync();
    };
    RSColorTemperature::Get().RegisterRefresh(std::move(refreshFunc));
#ifdef RS_ENABLE_GPU
    MemoryManager::InitMemoryLimit();
    MemoryManager::SetGpuMemoryLimit(GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
#endif
}

void RSMainThread::NotifyUnmarshalTask(int64_t uiTimestamp)
{
    if (isUniRender_ && rsVSyncDistributor_->IsUiDvsyncOn() && static_cast<uint64_t>(uiTimestamp) +
        static_cast<uint64_t>(rsVSyncDistributor_->GetUiCommandDelayTime()) >= dvsyncRsTimestamp_.load()
        && waitForDVSyncFrame_.load()) {
        auto cachedTransactionData = RSUnmarshalThread::Instance().GetCachedTransactionData();
        MergeToEffectiveTransactionDataMap(cachedTransactionData);
        {
            std::lock_guard<std::mutex> lock(unmarshalMutex_);
            ++unmarshalFinishedCount_;
            waitForDVSyncFrame_.store(false);
            RSUnmarshalThread::Instance().RemoveTask(DVSYNC_NOTIFY_UNMARSHAL_TASK_NAME);
        }
        unmarshalTaskCond_.notify_all();
    }
}

void RSMainThread::UpdateGpuContextCacheSize()
{
#ifdef RS_ENABLE_GPU
    auto gpuContext = isUniRender_? GetRenderEngine()->GetRenderContext()->GetDrGPUContext() :
        renderEngine_->GetRenderContext()->GetDrGPUContext();
    if (gpuContext == nullptr) {
        RS_LOGE("RSMainThread::UpdateGpuContextCacheSize gpuContext is nullptr!");
        return;
    }
    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSMainThread::UpdateGpuContextCacheSize screenManager is nullptr");
        return;
    }
    size_t cacheLimitsResourceSize = 0;
    size_t maxResourcesSize = 0;
    int32_t maxResources = 0;
    gpuContext->GetResourceCacheLimits(&maxResources, &maxResourcesSize);
    auto maxScreenInfo = screenManager->GetActualScreenMaxResolution();
    constexpr size_t baseResourceSize = 500;    // 500 M memory is baseline
    constexpr int32_t baseResolution = 3427200; // 3427200 is base resolution
    float actualScale = maxScreenInfo.phyWidth * maxScreenInfo.phyHeight * 1.0f / baseResolution;
    cacheLimitsResourceSize = baseResourceSize * actualScale
        * MEMUNIT_RATE * MEMUNIT_RATE; // adjust by actual Resolution
    cacheLimitsResourceSize = cacheLimitsResourceSize > MAX_GPU_CONTEXT_CACHE_SIZE ?
        MAX_GPU_CONTEXT_CACHE_SIZE : cacheLimitsResourceSize;
    if (cacheLimitsResourceSize > maxResourcesSize) {
        gpuContext->SetResourceCacheLimits(maxResources, cacheLimitsResourceSize);
    }
    static int systemCacheLimitResourceSize = MEMParam::GetRSCacheLimitsResourceSize();
    RS_LOGD("systemCacheLimitResourceSize: %{public}d", systemCacheLimitResourceSize);
    if (systemCacheLimitResourceSize > 0) {
        gpuContext->SetResourceCacheLimits(maxResources, systemCacheLimitResourceSize);
    }

    auto purgeableMaxCount = RSSystemParameters::GetPurgeableResourceLimit();
    gpuContext->SetPurgeableResourceLimit(purgeableMaxCount);
#endif
}

void RSMainThread::InitVulkanErrorCallback(Drawing::GPUContext* gpuContext)
{
    if (gpuContext == nullptr) {
        RS_LOGE("RSMainThread::InitVulkanErrorCallback gpuContext is nullptr");
        return;
    }

    gpuContext->RegisterVulkanErrorCallback([this]() {
        RS_LOGE("FocusLeashWindowName:[%{public}s]", this->focusLeashWindowName_.c_str());

        char appWindowName[EVENT_NAME_MAX_LENGTH];
        char focusLeashWindowName[EVENT_NAME_MAX_LENGTH];
        char extinfodefault[EVENT_NAME_MAX_LENGTH] = "ext_info_default";

        auto cpyresult = strcpy_s(appWindowName, EVENT_NAME_MAX_LENGTH, appWindowName_.c_str());
        if (cpyresult != 0) {
            RS_LOGE("Copy appWindowName_ error, AppWindowName:%{public}s", appWindowName_.c_str());
        }
        cpyresult = strcpy_s(focusLeashWindowName, EVENT_NAME_MAX_LENGTH, focusLeashWindowName_.c_str());
        if (cpyresult != 0) {
            RS_LOGE("Copy focusLeashWindowName error, focusLeashWindowName:%{public}s", focusLeashWindowName_.c_str());
        }

        HiSysEventParam pPID = { .name = "PID", .t = HISYSEVENT_UINT32, .v = { .ui32 = appPid_ }, .arraySize = 0 };

        HiSysEventParam pAppNodeId = {
            .name = "AppNodeId", .t = HISYSEVENT_UINT64, .v = { .ui64 = appWindowId_ }, .arraySize = 0
        };

        HiSysEventParam pAppNodeName = {
            .name = "AppNodeName", .t = HISYSEVENT_STRING, .v = { .s = appWindowName }, .arraySize = 0
        };

        HiSysEventParam pLeashWindowId = {
            .name = "LeashWindowId", .t = HISYSEVENT_UINT64, .v = { .ui64 = focusLeashWindowId_ }, .arraySize = 0
        };

        HiSysEventParam pLeashWindowName = {
            .name = "LeashWindowName", .t = HISYSEVENT_STRING, .v = { .s = focusLeashWindowName }, .arraySize = 0
        };

        HiSysEventParam pExtInfo = {
            .name = "ExtInfo", .t = HISYSEVENT_STRING, .v = { .s = extinfodefault }, .arraySize = 0
        };

        HiSysEventParam paramsHebcFault[] = { pPID, pAppNodeId, pAppNodeName, pLeashWindowId, pLeashWindowName,
            pExtInfo };

        int ret = OH_HiSysEvent_Write("GRAPHIC", "RS_VULKAN_ERROR", HISYSEVENT_FAULT, paramsHebcFault,
            sizeof(paramsHebcFault) / sizeof(paramsHebcFault[0]));
        if (ret == 0) {
            RS_LOGE("Successed to rs_vulkan_error fault event.");
        } else {
            RS_LOGE("Faild to upload rs_vulkan_error event, ret = %{public}d", ret);
        }
    });
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
            focusLeashWindowName_ = parent->GetName();
            appWindowId_ = node->GetId();
            appWindowName_ = node->GetName();
            appPid_ = appWindowId_ >> HIGH_32BIT;
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
#ifdef RS_ENABLE_GPU
            rsCompositionTimeoutDetector_->SetLoopFinishTag(
                focusAppPid_, focusAppUid_, focusAppBundleName_, focusAppAbilityName_);
#endif
        } else {
            std::string defaultFocusAppInfo = "";
            rsCompositionTimeoutDetector_->SetLoopFinishTag(
                -1, -1, defaultFocusAppInfo, defaultFocusAppInfo);
        }
    }
}

void RSMainThread::SetFocusAppInfo(
    int32_t pid, int32_t uid, const std::string& bundleName, const std::string& abilityName, uint64_t focusNodeId)
{
    focusAppPid_ = pid;
    focusAppUid_ = uid;
    focusAppBundleName_ = bundleName;
    focusAppAbilityName_ = abilityName;
    UpdateFocusNodeId(focusNodeId);
    RSPerfMonitorReporter::GetInstance().SetFocusAppInfo(bundleName.c_str());
}

void RSMainThread::UpdateFocusNodeId(NodeId focusNodeId)
{
    if (focusNodeId_ == focusNodeId || focusNodeId == INVALID_NODEID) {
        return;
    }
    UpdateNeedDrawFocusChange(focusNodeId_);
    UpdateNeedDrawFocusChange(focusNodeId);
    focusNodeId_ = focusNodeId;
}

void RSMainThread::UpdateNeedDrawFocusChange(NodeId id)
{
    const auto& nodeMap = context_->GetNodeMap();
    auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodeMap.GetRenderNode(id));
    // while nodeMap can't find node, return instantly
    if (!node) {
        return;
    }
    auto parentNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node->GetParent().lock());
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
    RSUnmarshalThread::Instance().ClearTransactionDataStatistics();

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
#ifdef RS_ENABLE_GPU
        ProcessCommandForUniRender();
#endif
    } else {
        ProcessCommandForDividedRender();
    }
#ifdef RS_ENABLE_GPU
    switch(context_->purgeType_) {
        case RSContext::PurgeType::GENTLY:
            isUniRender_ ? RSUniRenderThread::Instance().ClearMemoryCache(context_->clearMoment_, false) :
                ClearMemoryCache(context_->clearMoment_, false);
                isNeedResetClearMemoryTask_ = true;
            break;
        case RSContext::PurgeType::STRONGLY:
            isUniRender_ ? RSUniRenderThread::Instance().ClearMemoryCache(context_->clearMoment_, true) :
                ClearMemoryCache(context_->clearMoment_, true);
                isNeedResetClearMemoryTask_ = true;
            break;
        default:
            break;
    }
#endif
    context_->purgeType_ = RSContext::PurgeType::NONE;
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().AnimateStart();
    }
}

void RSMainThread::UpdateSubSurfaceCnt()
{
    auto& updateInfo = context_->subSurfaceCntUpdateInfo_;
    if (updateInfo.empty()) {
        return;
    }
    RS_TRACE_NAME_FMT("UpdateSubSurfaceCnt size: %zu", updateInfo.size());
    const auto& nodeMap = context_->GetNodeMap();
    for (auto& iter : updateInfo) {
        if (iter.curParentId_ != INVALID_NODEID) {
            if (auto curParent = nodeMap.GetRenderNode(iter.curParentId_)) {
                curParent->UpdateSubSurfaceCnt(iter.updateCnt_);
            }
        }
        if (iter.preParentId_ != INVALID_NODEID) {
            if (auto preParent = nodeMap.GetRenderNode(iter.preParentId_)) {
                preParent->UpdateSubSurfaceCnt(-iter.updateCnt_);
            }
        }
    }
    context_->subSurfaceCntUpdateInfo_.clear();
}

void RSMainThread::PrintCurrentStatus()
{
#ifdef RS_ENABLE_GPU
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
#endif
}

void RSMainThread::SubScribeSystemAbility()
{
    RS_LOGI("%{public}s", __func__);
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
    if (saStatusChangeListener_ == nullptr) {
        RS_LOGE("RSMainThread::SubScribeSystemAbility new VSyncSystemAbilityListener failed");
        return;
    }
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
        skipTransactionDataVec.clear();
    }
}

const std::unordered_map<NodeId, bool>& RSMainThread::GetCacheCmdSkippedNodes() const
{
    return cacheCmdSkippedNodes_;
}

bool RSMainThread::CheckParallelSubThreadNodesStatus()
{
    RS_OPTIONAL_TRACE_FUNC();
    cacheCmdSkippedInfo_.clear();
    cacheCmdSkippedNodes_.clear();
    if (subThreadNodes_.empty() &&
        (RSUifirstManager::Instance().GetUiFirstType() != UiFirstCcmType::MULTI
            || (leashWindowCount_ > 0 && !RSUifirstManager::Instance().GetUiFirstSwitch()))) {
#ifdef RS_ENABLE_GPU
        if (!isUniRender_) {
            RSSubThreadManager::Instance()->ResetSubThreadGrContext(); // planning: move to prepare
        }
#endif
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
            auto it = cacheCmdSkippedInfo_.find(pid);
            if (it == cacheCmdSkippedInfo_.end()) {
                cacheCmdSkippedInfo_.emplace(pid, std::make_pair(std::vector<NodeId>{node->GetId()}, false));
            } else {
                it->second.first.push_back(node->GetId());
            }
            if (!node->HasAbilityComponent()) {
                continue;
            }
            for (auto& nodeId : node->GetAbilityNodeIds()) {
                pid_t abilityNodePid = ExtractPid(nodeId);
                it = cacheCmdSkippedInfo_.find(abilityNodePid);
                if (it == cacheCmdSkippedInfo_.end()) {
                    cacheCmdSkippedInfo_.emplace(abilityNodePid,
                        std::make_pair(std::vector<NodeId>{node->GetId()}, true));
                } else {
                    it->second.first.push_back(node->GetId());
                }
            }
        }
    }
    if (!cacheCmdSkippedNodes_.empty()) {
        return true;
    }
    if (!RSUifirstManager::Instance().GetUiFirstSwitch()) {
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

void RSMainThread::RequestNextVsyncForCachedCommand(std::string& transactionFlags, pid_t pid, uint64_t curIndex)
{
#ifdef ROSEN_EMULATOR
    transactionFlags += " cache [" + std::to_string(pid) + "," + std::to_string(curIndex) + "]";
    RequestNextVSync();
#else
    transactionFlags += " cache (" + std::to_string(pid) + "," + std::to_string(curIndex) + ")";
    RS_TRACE_NAME("RSMainThread::CheckAndUpdateTransactionIndex Trigger NextVsync");
    if (rsVSyncDistributor_->IsUiDvsyncOn()) {
        RequestNextVSync("fromRsMainCommand", 0);
    } else {
        RequestNextVSync("UI", 0);
    }
#endif
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
            RS_PROFILER_REPLAY_FIX_TRINDEX(curIndex, lastIndex);
            if (curIndex == lastIndex + 1) {
                if ((*iter)->GetTimestamp() + static_cast<uint64_t>(rsVSyncDistributor_->GetUiCommandDelayTime())
                    >= timestamp_) {
                    RequestNextVsyncForCachedCommand(transactionFlags, pid, curIndex);
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
            if (transactionDataEffective == nullptr) {
                transactionDataEffective = std::make_shared<TransactionDataMap>();
            }
            (*transactionDataEffective)[pid].insert((*transactionDataEffective)[pid].end(),
                std::make_move_iterator(transactionVec.begin()), std::make_move_iterator(iter));
            transactionVec.erase(transactionVec.begin(), iter);
        }
    }
}

void RSMainThread::ProcessCommandForUniRender()
{
#ifdef RS_ENABLE_GPU
    std::shared_ptr<TransactionDataMap> transactionDataEffective = nullptr;
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
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetTransactionFlags(transactionFlags);
    if ((transactionDataEffective != nullptr && !transactionDataEffective->empty()) ||
        RSPointerWindowManager::Instance().GetBoundHasUpdate()) {
        doDirectComposition_ = false;
        RS_OPTIONAL_TRACE_NAME_FMT("rs debug: %s transactionDataEffective not empty", __func__);
    }
    const auto& nodeMap = context_->GetNodeMap();
    nodeMap.TraverseCanvasDrawingNodes([](const std::shared_ptr<RSCanvasDrawingRenderNode>& canvasDrawingNode) {
        if (canvasDrawingNode == nullptr) {
            return;
        }
        if (canvasDrawingNode->IsNeedProcess()) {
            auto drawableNode = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasDrawingNode);
            if (!drawableNode) {
                RS_LOGE("RSMainThread::ProcessCommandForUniRender GetCanvasDrawable Failed NodeId[%{public}" PRIu64 "]",
                    canvasDrawingNode->GetId());
                return;
            }
            std::static_pointer_cast<DrawableV2::RSCanvasDrawingRenderNodeDrawable>(drawableNode)
                ->PostPlaybackInCorrespondThread();
        }
    });
    RS_TRACE_NAME("RSMainThread::ProcessCommandUni" + transactionFlags);
    if (transactionFlags != "") {
        transactionFlags_ = transactionFlags;
    }
    if (transactionDataEffective != nullptr && !transactionDataEffective->empty()) {
        for (auto& rsTransactionElem : *transactionDataEffective) {
            for (auto& rsTransaction : rsTransactionElem.second) {
                // If this transaction is marked as requiring synchronization and the SyncId for synchronization is not
                // 0, or if there have been previous transactions of this process considered as synchronous, then all
                // subsequent transactions of this process will be synchronized.
                if (rsTransaction && ((rsTransaction->IsNeedSync() && rsTransaction->GetSyncId() > 0) ||
                    syncTransactionData_.count(rsTransactionElem.first) > 0)) {
                    ProcessSyncRSTransactionData(rsTransaction, rsTransactionElem.first);
                } else if (rsTransaction) {
                    ProcessRSTransactionData(rsTransaction, rsTransactionElem.first);
                }
            }
        }
        RSBackgroundThread::Instance().PostTask([transactionDataEffective]() {
            RS_TRACE_NAME("RSMainThread::ProcessCommandForUniRender transactionDataEffective clear");
            transactionDataEffective->clear();
        });
    }
#endif
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
            auto bufferTimestamp = dividedRenderbufferTimestamps_.find(surfaceNodeId);
            std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>>::iterator effectIter;

            if (!node || !node->IsOnTheTree() || bufferTimestamp == dividedRenderbufferTimestamps_.end()) {
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

void RSMainThread::StartSyncTransactionFallbackTask(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    if (handler_) {
        auto task = [this, syncId = rsTransactionData->GetSyncId()]() {
            if (!syncTransactionData_.empty() && syncTransactionData_.begin()->second.front() &&
                    syncTransactionData_.begin()->second.front()->GetSyncId() != syncId) {
                return;
            }
            ROSEN_LOGD("RSMainThread ProcessAllSyncTransactionData timeout task");
            ProcessAllSyncTransactionData();
        };
        handler_->PostTask(
            task, "ProcessAllSyncTransactionsTimeoutTask", RSSystemProperties::GetSyncTransactionWaitDelay());
    }
}

void RSMainThread::ProcessSyncTransactionCount(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    auto sendingPid = rsTransactionData->GetSendingPid();
    auto parentPid = rsTransactionData->GetParentPid();
    subSyncTransactionCounts_[sendingPid] += rsTransactionData->GetSyncTransactionNum();
    if (subSyncTransactionCounts_[sendingPid] == 0) {
        subSyncTransactionCounts_.erase(sendingPid);
    }
    if (!rsTransactionData->IsNeedCloseSync()) {
        subSyncTransactionCounts_[parentPid]--;
        if (subSyncTransactionCounts_[parentPid] == 0) {
            subSyncTransactionCounts_.erase(parentPid);
        }
    }
    ROSEN_LOGI("RSMainThread::ProcessSyncTransactionCount isNeedCloseSync:%{public}d syncId:%{public}" PRIu64 ""
               " parentPid:%{public}d syncNum:%{public}d subSyncTransactionCounts_.size:%{public}zd",
        rsTransactionData->IsNeedCloseSync(), rsTransactionData->GetSyncId(), parentPid,
        rsTransactionData->GetSyncTransactionNum(), subSyncTransactionCounts_.size());
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

    if (!syncTransactionData_.empty() && syncTransactionData_.begin()->second.front() &&
        (syncTransactionData_.begin()->second.front()->GetSyncId() != rsTransactionData->GetSyncId())) {
        ProcessAllSyncTransactionData();
    }
    if (syncTransactionData_.empty()) {
        StartSyncTransactionFallbackTask(rsTransactionData);
    }
    if (syncTransactionData_.count(pid) == 0) {
        syncTransactionData_.insert({ pid, std::vector<std::unique_ptr<RSTransactionData>>() });
    }
    ProcessSyncTransactionCount(rsTransactionData);
    syncTransactionData_[pid].emplace_back(std::move(rsTransactionData));
    if (subSyncTransactionCounts_.empty()) {
        ROSEN_LOGI("SyncTransaction success");
        ProcessAllSyncTransactionData();
    }
}

void RSMainThread::ProcessAllSyncTransactionData()
{
    RS_TRACE_NAME("RSMainThread::ProcessAllSyncTransactionData");
    for (auto& [pid, transactions] : syncTransactionData_) {
        for (auto& transaction: transactions) {
            ROSEN_LOGD("RSMainThread ProcessAllSyncTransactionData GetCommandCount: %{public}lu pid: %{public}d",
                transaction->GetCommandCount(), pid);
            ProcessRSTransactionData(transaction, pid);
        }
    }
    syncTransactionData_.clear();
    subSyncTransactionCounts_.clear();
    RequestNextVSync();
}

void RSMainThread::ConsumeAndUpdateAllNodes()
{
    ResetHardwareEnabledState(isUniRender_);
    RS_OPTIONAL_TRACE_BEGIN("RSMainThread::ConsumeAndUpdateAllNodes");
    bool needRequestNextVsync = false;
    if (!isUniRender_) {
        dividedRenderbufferTimestamps_.clear();
    }
    drmNodes_.clear();
    const auto& nodeMap = GetContext().GetNodeMap();
    bool isHdrSwitchChanged = RSLuminanceControl::Get().IsHdrPictureOn() != prevHdrSwitchStatus_;
    nodeMap.TraverseSurfaceNodes(
        [this, &needRequestNextVsync, isHdrSwitchChanged](
            const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
        if (surfaceNode == nullptr) {
            return;
        }
        surfaceNode->ResetAnimateState();
        surfaceNode->ResetRotateState();
        surfaceNode->ResetSpecialLayerChangedFlag();
        surfaceNode->ResetIsBufferFlushed();
        // Reset BasicGeoTrans info at the beginning of cmd process
        if (surfaceNode->IsLeashOrMainWindow()) {
            surfaceNode->ResetIsOnlyBasicGeoTransform();
        }
        if (surfaceNode->GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos ||
            (isHdrSwitchChanged && surfaceNode->GetHDRPresent())) {
            RS_LOGD("RSMainThread::ConsumeAndUpdateAllNodes set %{public}s content dirty",
                surfaceNode->GetName().c_str());
            surfaceNode->SetContentDirty(); // screen recording capsule and hdr switch change force mark dirty
        }
        if (surfaceNode->IsHardwareEnabledType()
            && CheckSubThreadNodeStatusIsDoing(surfaceNode->GetInstanceRootNodeId())) {
            RS_LOGD("SubThread is processing %{public}s, skip acquire buffer", surfaceNode->GetName().c_str());
            return;
        }
        auto surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler();
        if (surfaceHandler->GetAvailableBufferCount() > 0) {
            if (rsVSyncDistributor_ != nullptr) {
                rsVSyncDistributor_->SetHasNativeBuffer();
            }
            auto name = surfaceNode->GetName().empty() ? DEFAULT_SURFACE_NODE_NAME : surfaceNode->GetName();
            auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
            if (frameRateMgr != nullptr) {
                frameRateMgr->UpdateSurfaceTime(
                    name, ExtractPid(surfaceNode->GetId()), UIFWKType::FROM_SURFACE);
            }
        }
        surfaceHandler->ResetCurrentFrameBufferConsumed();
        bool needConsume = true;
        bool enableAdaptive = rsVSyncDistributor_->AdaptiveDVSyncEnable(surfaceNode->GetName(),
            timestamp_, surfaceHandler->GetAvailableBufferCount(), needConsume);
        if (RSBaseRenderUtil::ConsumeAndUpdateBuffer(*surfaceHandler, timestamp_,
            IsNeedDropFrameByPid(surfaceHandler->GetNodeId()), enableAdaptive, needConsume)) {
            if (!isUniRender_) {
                this->dividedRenderbufferTimestamps_[surfaceNode->GetId()] =
                    static_cast<uint64_t>(surfaceHandler->GetTimestamp());
            }
#ifdef RS_ENABLE_GPU
            if (surfaceHandler->IsCurrentFrameBufferConsumed() && surfaceNode->IsHardwareEnabledType()) {
                GpuDirtyRegionCollection::GetInstance().UpdateActiveDirtyInfoForDFX(surfaceNode->GetId(),
                    surfaceNode->GetName(), surfaceHandler->GetDamageRegion());
            }
#endif
            if (surfaceHandler->IsCurrentFrameBufferConsumed() && !surfaceNode->IsHardwareEnabledType()) {
                surfaceNode->SetContentDirty();
                doDirectComposition_ = false;
                RS_OPTIONAL_TRACE_NAME_FMT(
                    "rs debug: name %s, id %" PRIu64", buffer consumed and not HardwareEnabledType",
                    surfaceNode->GetName().c_str(), surfaceNode->GetId());
            }
            if (isUniRender_ && surfaceHandler->IsCurrentFrameBufferConsumed()) {
#ifdef RS_ENABLE_GPU
                auto buffer = surfaceHandler->GetBuffer();
                auto preBuffer = surfaceHandler->GetPreBuffer();
                surfaceNode->UpdateBufferInfo(buffer,
                    surfaceHandler->GetDamageRegion(), surfaceHandler->GetAcquireFence(), preBuffer);
                if (surfaceHandler->GetBufferSizeChanged() || surfaceHandler->GetBufferTransformTypeChanged()) {
                    surfaceNode->SetContentDirty();
                    doDirectComposition_ = false;
                    surfaceHandler->SetBufferTransformTypeChanged(false);
                    RS_OPTIONAL_TRACE_NAME_FMT("rs debug: name %s, id %" PRIu64", surfaceNode buffer size changed",
                        surfaceNode->GetName().c_str(), surfaceNode->GetId());
                    RS_LOGD("ConsumeAndUpdateAllNodes name:%{public}s id:%{public}" PRIu64" buffer size changed, "
                        "buffer:[%{public}d, %{public}d], preBuffer:[%{public}d, %{public}d]",
                        surfaceNode->GetName().c_str(), surfaceNode->GetId(),
                        buffer ? buffer->GetSurfaceBufferWidth() : 0, buffer ? buffer->GetSurfaceBufferHeight() : 0,
                        preBuffer ? preBuffer->GetSurfaceBufferWidth() : 0,
                        preBuffer ? preBuffer->GetSurfaceBufferHeight() : 0);
                }
#endif
            }
            if (RSUifirstManager::Instance().GetUiFirstSwitch() && surfaceHandler->IsCurrentFrameBufferConsumed()
                && surfaceNode->IsHardwareEnabledType() && surfaceNode->IsHardwareForcedDisabledByFilter()) {
                    RS_OPTIONAL_TRACE_NAME(surfaceNode->GetName() +
                        " SetContentDirty for UIFirst assigning to subthread");
                    surfaceNode->SetContentDirty();
                    doDirectComposition_ = false;
                    RS_OPTIONAL_TRACE_NAME_FMT("rs debug: name %s, id %" PRIu64", pc uifirst on",
                        surfaceNode->GetName().c_str(), surfaceNode->GetId());
            }
        }
#ifdef RS_ENABLE_VK
        if ((RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) &&
            (surfaceHandler->GetBufferUsage() & BUFFER_USAGE_PROTECTED)) {
            if (!surfaceNode->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
                surfaceNode->SetProtectedLayer(true);
            }
            const auto& instanceNode = surfaceNode->GetInstanceRootNode();
            if (instanceNode && instanceNode->IsOnTheTree()) {
                hasProtectedLayer_ = true;
                auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(
                    surfaceNode->GetFirstLevelNode());
                if (firstLevelNode && firstLevelNode->IsOnTheTree()) {
                    drmNodes_[firstLevelNode->GetId()].push_back(surfaceNode);
                }
                auto displayLock = surfaceNode->GetAncestorDisplayNode().lock();
                std::shared_ptr<RSDisplayRenderNode> ancestor = nullptr;
                if (displayLock != nullptr) {
                    ancestor = displayLock->ReinterpretCastTo<RSDisplayRenderNode>();
                }
                if (ancestor == nullptr) {
                    return;
                }
                auto protectedLayerScreenId = ancestor->GetScreenId();
                auto screenManager = CreateOrGetScreenManager();
                if (screenManager == nullptr) {
                    RS_LOGE("screenManager is NULL");
                    return;
                }

                auto output = screenManager->GetOutput(ToScreenPhysicalId(protectedLayerScreenId));
                if (output == nullptr) {
                    RS_LOGE("output is NULL");
                    return;
                }
                RS_TRACE_NAME_FMT("output->GetProtectedFrameBuffer: ret= %d, addr = %p.",
                    output->GetProtectedFrameBufferState(), output.get());
                if (output->GetProtectedFrameBufferState()) {
                    return;
                }
                auto protectedBuffer = surfaceHandler->GetBuffer();
                auto preAllocateProtectedBufferTask = [buffer = protectedBuffer, screenId = protectedLayerScreenId]() {
                    RSHardwareThread::Instance().PreAllocateProtectedBuffer(buffer, screenId);
                };
                RSBackgroundThread::Instance().PostTask(preAllocateProtectedBufferTask);
            }
        }
#endif
        // still have buffer(s) to consume.
        if (surfaceHandler->GetAvailableBufferCount() > 0) {
            needRequestNextVsync = true;
        }
        surfaceNode->SetVideoHdrStatus(RSHdrUtil::CheckIsHdrSurface(*surfaceNode));
        if (surfaceNode->GetVideoHdrStatus() == HdrStatus::NO_HDR) {
            surfaceNode->SetSdrHasMetadata(RSHdrUtil::CheckIsSurfaceWithMetadata(*surfaceNode));
        }
    });
    prevHdrSwitchStatus_ = RSLuminanceControl::Get().IsHdrPictureOn();
    if (needRequestNextVsync) {
        RequestNextVSync();
    }
    RS_OPTIONAL_TRACE_END();
}

bool RSMainThread::CheckSubThreadNodeStatusIsDoing(NodeId appNodeId) const
{
    for (auto& node : subThreadNodes_) {
        if (node == nullptr) {
            continue;
        }
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
#ifdef RS_ENABLE_GPU
    if (!isUniRender_) {
        return;
    }
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    // pre proc for tv overlay display
    RSOverlayDisplayManager::Instance().PreProcForRender();
#endif
    CheckIfHardwareForcedDisabled();
    if (!pendingUiCaptureTasks_.empty()) {
        RS_OPTIONAL_TRACE_NAME("rs debug: uiCapture SetDoDirectComposition false");
        doDirectComposition_ = false;
    }
    const auto& nodeMap = GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes(
        [this, &nodeMap](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
            if (surfaceNode == nullptr) {
                return;
            }
            if (surfaceNode->IsCloneCrossNode()) {
                auto sourceNode = surfaceNode->GetSourceCrossNode().lock();
                auto sourceSurface = sourceNode == nullptr ? nullptr:
                    sourceNode->ReinterpretCastTo<RSSurfaceRenderNode>();
                auto leashWindowNode = sourceSurface == nullptr ? nullptr :
                    RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(sourceSurface->GetFirstLevelNode());
                if (leashWindowNode && leashWindowNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED) &&
                    surfaceNode->GetDisplayNodeId() != INVALID_NODEID) {
                        if (drmNodes_.find(leashWindowNode->GetId()) != drmNodes_.end()) {
                            for (auto& node : drmNodes_[leashWindowNode->GetId()]) {
                                if (node->IsOnTheTree()) {
                                    hardwareEnabledDrwawables_.emplace_back(
                                        std::make_pair(surfaceNode->GetDisplayNodeId(), node->GetRenderDrawable()));
                                }
                            }
                        }
                    }
            }
            auto surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler();
            if (surfaceHandler->GetBuffer() != nullptr) {
                AddSelfDrawingNodes(surfaceNode);
                selfDrawables_.emplace_back(surfaceNode->GetRenderDrawable());
                RSPointerWindowManager::Instance().SetHardCursorNodeInfo(surfaceNode);
            }

            if (!surfaceNode->GetDoDirectComposition()) {
                doDirectComposition_ = false;
                RS_OPTIONAL_TRACE_NAME_FMT("rs debug: name %s, id %" PRIu64", node GetDoDirectComposition is false",
                    surfaceNode->GetName().c_str(), surfaceNode->GetId());
                surfaceNode->SetDoDirectComposition(true);
            }

            if (!surfaceNode->IsOnTheTree()) {
                if (surfaceHandler->IsCurrentFrameBufferConsumed()) {
                    surfaceNode->UpdateHardwareDisabledState(true);
                    doDirectComposition_ = false;
                    RS_OPTIONAL_TRACE_NAME_FMT("rs debug: name %s, id %" PRIu64", node not on the tree "
                        "and buffer consumed", surfaceNode->GetName().c_str(), surfaceNode->GetId());
                }
                return;
            }

            // If hardware don't support hdr render, should disable direct composition
            if (RSLuminanceControl::Get().IsCloseHardwareHdr() &&
                surfaceNode->GetVideoHdrStatus() != HdrStatus::NO_HDR &&
                !surfaceNode->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
                doDirectComposition_ = false;
                RS_OPTIONAL_TRACE_NAME_FMT("rs debug: name %s, id %" PRIu64", HDR disable direct composition",
                    surfaceNode->GetName().c_str(), surfaceNode->GetId());
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

            if (surfaceHandler->GetBuffer() != nullptr) {
                // collect hwc nodes vector, used for display node skip and direct composition cases
                surfaceNode->SetIsLastFrameHwcEnabled(!surfaceNode->IsHardwareForcedDisabled());
                hardwareEnabledNodes_.emplace_back(surfaceNode);
                hardwareEnabledDrwawables_.emplace_back(std::make_pair(surfaceNode->GetDisplayNodeId(),
                surfaceNode->GetRenderDrawable()));
            }

            // set content dirty for hwc node if needed
            if (isHardwareForcedDisabled_) {
                // buffer updated or hwc -> gpu
                if (surfaceHandler->IsCurrentFrameBufferConsumed() || surfaceNode->GetIsLastFrameHwcEnabled()) {
                    surfaceNode->SetContentDirty();
                }
            } else if (!surfaceNode->GetIsLastFrameHwcEnabled()) { // gpu -> hwc
                if (surfaceHandler->IsCurrentFrameBufferConsumed()) {
                    surfaceNode->SetContentDirty();
                    doDirectComposition_ = false;
                    RS_OPTIONAL_TRACE_NAME_FMT(
                        "rs debug: name %s, id %" PRIu64", isLastFrameHwcEnabled not enabled and buffer consumed",
                        surfaceNode->GetName().c_str(), surfaceNode->GetId());
                } else {
                    if (surfaceNode->GetAncoForceDoDirect()) {
                        surfaceNode->SetContentDirty();
                    }
                    surfaceNode->SetHwcDelayDirtyFlag(true);
                }
            } else { // hwc -> hwc
                // self-drawing node don't set content dirty when gpu -> hwc
                // so first frame in hwc -> hwc, should set content dirty
                if (surfaceNode->GetHwcDelayDirtyFlag() ||
                    RSUniRenderUtil::GetRotationDegreeFromMatrix(surfaceNode->GetTotalMatrix()) % RS_ROTATION_90 != 0) {
                    surfaceNode->SetContentDirty();
                    surfaceNode->SetHwcDelayDirtyFlag(false);
                    doDirectComposition_ = false;
                    RS_OPTIONAL_TRACE_NAME_FMT("rs debug: name %s, id %" PRIu64", HwcDelayDirtyFlag is true",
                        surfaceNode->GetName().c_str(), surfaceNode->GetId());
                }
            }

            if (surfaceHandler->IsCurrentFrameBufferConsumed()) {
                isHardwareEnabledBufferUpdated_ = true;
            }
        });
#endif
}

bool RSMainThread::IsLastFrameUIFirstEnabled(NodeId appNodeId) const
{
    for (auto& node : subThreadNodes_) {
        if (node == nullptr) {
            continue;
        }
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

static bool CheckOverlayDisplayEnable()
{
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    return RSOverlayDisplayManager::Instance().IsOverlayDisplayEnableForCurrentVsync();
#else
    return false;
#endif
}

void RSMainThread::CheckIfHardwareForcedDisabled()
{
    ColorFilterMode colorFilterMode = renderEngine_->GetColorFilterMode();
    bool hasColorFilter = colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
        colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE;
    std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        RS_LOGE("RSMainThread::CheckIfHardwareForcedDisabled rootNode is nullptr");
        return;
    }
    bool isMultiDisplay = rootNode->GetChildrenCount() > 1;
    MultiDisplayChange(isMultiDisplay);

    auto hwcFeatureParam = std::static_pointer_cast<HWCParam>(
        GraphicFeatureParamManager::GetInstance().GetFeatureParam(FEATURE_CONFIGS[HWC]));
    // check all children of global root node, and only disable hardware composer
    // in case node's composite type is UNI_RENDER_EXPAND_COMPOSITE or Wired projection
    const auto& children = rootNode->GetChildren();
    auto itr = std::find_if(children->begin(), children->end(),
        [hwcFeature = hwcFeatureParam](const std::shared_ptr<RSRenderNode>& child) -> bool {
            if (child == nullptr || child->GetType() != RSRenderNodeType::DISPLAY_NODE) {
                return false;
            }
            auto displayNodeSp = std::static_pointer_cast<RSDisplayRenderNode>(child);
            if (displayNodeSp->GetMirrorSource().lock()) {
                // wired projection case
                return displayNodeSp->GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE;
            }
            if (!hwcFeature) {
                return false;
            }
            if (hwcFeature->IsHwcExpandingScreenEnabled()) {
                return displayNodeSp->GetCompositeType() ==
                    RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE;
            }
            auto screenManager = CreateOrGetScreenManager();
            if (!screenManager) {
                return false;
            }
            RSScreenType screenType;
            screenManager->GetScreenType(displayNodeSp->GetScreenId(), screenType);
            // For PC expand physical screen.
            return displayNodeSp->GetScreenId() != 0 && screenType != RSScreenType::VIRTUAL_TYPE_SCREEN;
    });

    bool isExpandScreenOrWiredProjectionCase = itr != children->end();
    bool enableHwcForMirrorMode = RSSystemProperties::GetHardwareComposerEnabledForMirrorMode();
    // [PLANNING] GetChildrenCount > 1 indicates multi display, only Mirror Mode need be marked here
    // Mirror Mode reuses display node's buffer, so mark it and disable hardware composer in this case
    isHardwareForcedDisabled_ = isHardwareForcedDisabled_ || doWindowAnimate_ ||
        hasColorFilter || CheckOverlayDisplayEnable() ||
        (isMultiDisplay && !hasProtectedLayer_ && (isExpandScreenOrWiredProjectionCase || !enableHwcForMirrorMode));

    RS_OPTIONAL_TRACE_NAME_FMT("hwc debug global: CheckIfHardwareForcedDisabled isHardwareForcedDisabled_:%d "
        "doWindowAnimate_:%d isMultiDisplay:%d hasColorFilter:%d",
        isHardwareForcedDisabled_, doWindowAnimate_.load(), isMultiDisplay, hasColorFilter);

    if (isMultiDisplay && !isHardwareForcedDisabled_) {
        // Disable direct composition when hardware composer is enabled for virtual screen
        doDirectComposition_ = false;
        RS_OPTIONAL_TRACE_NAME_FMT("rs debug: %s isMultiDisplay disable doDirectComposition", __func__);
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
        auto surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler();
        // surfaceNode's buffer will be released in hardware thread if last frame enables hardware composer
        if (surfaceNode->IsHardwareEnabledType()) {
            if (surfaceNode->IsLastFrameHardwareEnabled()) {
                if (!surfaceNode->IsCurrentFrameHardwareEnabled()) {
                    auto preBuffer = surfaceHandler->GetPreBuffer();
                    if (preBuffer != nullptr) {
                        auto releaseTask = [buffer = preBuffer, consumer = surfaceHandler->GetConsumer(),
                            fence = surfaceHandler->GetPreBufferReleaseFence()]() mutable {
                            auto ret = consumer->ReleaseBuffer(buffer, fence);
                            if (ret != OHOS::SURFACE_ERROR_OK) {
                                RS_LOGD("surfaceHandler ReleaseBuffer failed(ret: %{public}d)!", ret);
                            }
                        };
                        surfaceHandler->ResetPreBuffer();
#ifdef RS_ENABLE_GPU
                        RSHardwareThread::Instance().PostTask(releaseTask);
#endif
                    }
                }
                surfaceNode->ResetCurrentFrameHardwareEnabledState();
                return;
            }
            surfaceNode->ResetCurrentFrameHardwareEnabledState();
        }
        RSBaseRenderUtil::ReleaseBuffer(*surfaceHandler);
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
#ifdef RS_ENABLE_GPU
    if (!RSSystemProperties::GetReleaseResourceEnabled()) {
        return;
    }
    bool isDeeplyRelGpuResEnable = false;
    auto relGpuResParam = std::static_pointer_cast<DeeplyRelGpuResParam>(
        GraphicFeatureParamManager::GetInstance().GetFeatureParam(FEATURE_CONFIGS[DEEPLY_REL_GPU_RES]));
    if (relGpuResParam != nullptr) {
        isDeeplyRelGpuResEnable = relGpuResParam->IsDeeplyRelGpuResEnable();
    }
    this->clearMemoryFinished_ = false;
    this->clearMemDeeply_ = this->clearMemDeeply_ || deeply;
    this->SetClearMoment(moment);
    this->exitedPidSet_.emplace(pid);
    auto task =
        [this, moment, deeply, isDeeplyRelGpuResEnable]() {
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
                if (deeply || isDeeplyRelGpuResEnable) {
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
    auto refreshRate = GetRefreshRate();
    if (refreshRate > 0) {
        if (!isUniRender_ || rsParallelType_ == RsParallelType::RS_PARALLEL_TYPE_SINGLE_THREAD) {
            PostTask(task, CLEAR_GPU_CACHE,
                (isDeeplyRelGpuResEnable ? TIME_OF_THE_FRAMES : TIME_OF_EIGHT_FRAMES) / refreshRate,
                AppExecFwk::EventQueue::Priority::HIGH);
        } else {
            RSUniRenderThread::Instance().PostTask(task, CLEAR_GPU_CACHE,
                (isDeeplyRelGpuResEnable ? TIME_OF_THE_FRAMES : TIME_OF_EIGHT_FRAMES) / refreshRate,
                AppExecFwk::EventQueue::Priority::HIGH);
        }
    }
#endif
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
    if (RSSystemProperties::GetUnmarshParallelFlag()) {
        RSUnmarshalThread::Instance().Wait();
        auto cachedTransactionData = RSUnmarshalThread::Instance().GetCachedTransactionData();
        MergeToEffectiveTransactionDataMap(cachedTransactionData);
    } else {
        std::unique_lock<std::mutex> lock(unmarshalMutex_);
        if (unmarshalFinishedCount_ > 0) {
            waitForDVSyncFrame_.store(false);
        } else {
            waitForDVSyncFrame_.store(true);
        }
        if (!unmarshalTaskCond_.wait_for(lock, std::chrono::milliseconds(WAIT_FOR_UNMARSHAL_THREAD_TASK_TIMEOUT),
            [this]() { return unmarshalFinishedCount_ > 0; })) {
            if (auto task = RSUnmarshalTaskManager::Instance().GetLongestTask()) {
                RSUnmarshalThread::Instance().RemoveTask(task.value().name);
                RS_LOGI("RSMainThread::WaitUntilUnmarshallingTaskFinished"
                    "the wait time exceeds %{public}d ms, remove task %{public}s",
                    WAIT_FOR_UNMARSHAL_THREAD_TASK_TIMEOUT, task.value().name.c_str());
                RS_TRACE_NAME_FMT("RSMainThread::WaitUntilUnmarshallingTaskFinished"
                    "the wait time exceeds %d ms, remove task %s",
                    WAIT_FOR_UNMARSHAL_THREAD_TASK_TIMEOUT, task.value().name.c_str());
            }
        }
        RSUnmarshalTaskManager::Instance().Clear();
        --unmarshalFinishedCount_;
    }
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

void RSMainThread::OnHideNotchStatusCallback(const char *key, const char *value, void *context)
{
    if (strcmp(key, HIDE_NOTCH_STATUS) != 0) {
        RS_LOGI("RSMainThread::OnHideNotchStatusCallback, key is not HIDE_NOTCH_STATUS");
        return;
    }
    RS_LOGI("RSMainThread::OnHideNotchStatusCallback HideNotchStatus is change, status is %{public}d",
        RSSystemParameters::GetHideNotchStatus());
    RSMainThread::Instance()->RequestNextVSync();
}

void RSMainThread::OnDrawingCacheDfxSwitchCallback(const char *key, const char *value, void *context)
{
    if (strcmp(key, DRAWING_CACHE_DFX) != 0) {
        return;
    }
    bool isDrawingCacheDfxEnabled;
    if (value) {
        isDrawingCacheDfxEnabled = (std::atoi(value) != 0);
    } else {
        isDrawingCacheDfxEnabled = RSSystemParameters::GetDrawingCacheEnabledDfx();
    }
    RSMainThread::Instance()->PostTask([isDrawingCacheDfxEnabled]() {
        RSMainThread::Instance()->SetDirtyFlag();
        RSMainThread::Instance()->SetDrawingCacheDfxEnabledOfCurFrame(isDrawingCacheDfxEnabled);
        RSMainThread::Instance()->RequestNextVSync("DrawingCacheDfx");
    });
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
    int changed = 0;
    if (bool enable = RSSystemParameters::GetShowRefreshRateEnabled(&changed); changed != 0) {
        RSRealtimeRefreshRateManager::Instance().SetShowRefreshRateEnabled(enable, 1);
    }
    // Start of DVSync
    bool isUiDvsyncOn = rsVSyncDistributor_ != nullptr ? rsVSyncDistributor_->IsUiDvsyncOn() : false;
    // End of DVSync
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr == nullptr || rsVSyncDistributor_ == nullptr) {
        return;
    }

    int32_t isAdaptive = frameRateMgr->AdaptiveStatus();
    if (isAdaptive == SupportASStatus::SUPPORT_AS) {
        frameRateMgr->HandleGameNode(GetContext().GetNodeMap());
    }

    static std::once_flag initUIFwkTableFlag;
    std::call_once(initUIFwkTableFlag, [this]() {
        if (auto config = HgmCore::Instance().GetPolicyConfigData(); config != nullptr) {
            GetContext().SetUiFrameworkTypeTable(config->appBufferList_);
        }
    });
    // Check and processing refresh rate task.
    frameRateMgr->ProcessPendingRefreshRate(timestamp, vsyncId_, rsVSyncDistributor_->GetRefreshRate(), isUiDvsyncOn);

    if (rsFrameRateLinker_ != nullptr) {
        auto rsCurrRange = rsCurrRange_;
        rsCurrRange.type_ = RS_ANIMATION_FRAME_RATE_TYPE;
        HgmEnergyConsumptionPolicy::Instance().GetAnimationIdleFps(rsCurrRange);
        rsFrameRateLinker_->SetExpectedRange(rsCurrRange);
        RS_TRACE_NAME_FMT("rsCurrRange = (%d, %d, %d)", rsCurrRange.min_, rsCurrRange.max_, rsCurrRange.preferred_);
    }

    if (rsCurrRange_.IsValid()) {
        frameRateMgr->GetRsFrameRateTimer().Start();
    } else {
        frameRateMgr->GetRsFrameRateTimer().Stop();
    }

    bool needRefresh = frameRateMgr->UpdateUIFrameworkDirtyNodes(GetContext().GetUiFrameworkDirtyNodes(), timestamp_);
    bool setHgmTaskFlag = HgmCore::Instance().SetHgmTaskFlag(false);
    bool vrateStatusChange = rsVsyncRateReduceManager_.SetVSyncRatesChangeStatus(false);
    bool isVideoCallVsyncChange = HgmEnergyConsumptionPolicy::Instance().GetVideoCallVsyncChange();

    if (!vrateStatusChange && !setHgmTaskFlag && !needRefresh && !isVideoCallVsyncChange &&
        HgmCore::Instance().GetPendingScreenRefreshRate() == frameRateMgr->GetCurrRefreshRate()) {
        return;
    }

    HgmTaskHandleThread::Instance().PostTask([timestamp, rsFrameRateLinker = rsFrameRateLinker_,
                                        appFrameRateLinkers = GetContext().GetFrameRateLinkerMap().Get(),
                                        linkers = rsVsyncRateReduceManager_.GetVrateMap() ]() mutable {
        RS_TRACE_NAME("ProcessHgmFrameRate");
        if (auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr(); frameRateMgr != nullptr) {
            frameRateMgr->UniProcessDataForLtpo(timestamp, rsFrameRateLinker, appFrameRateLinkers, linkers);
        }
    });
}

void RSMainThread::SetFrameIsRender(bool isRender)
{
    if (rsVSyncDistributor_ != nullptr) {
        rsVSyncDistributor_->SetFrameIsRender(isRender);
    }
}

void RSMainThread::WaitUntilUploadTextureTaskFinishedForGL()
{
#if (defined(RS_ENABLE_GPU) && defined(RS_ENABLE_GL))
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        WaitUntilUploadTextureTaskFinished(isUniRender_);
    }
#endif
}

void RSMainThread::AddUiCaptureTask(NodeId id, std::function<void()> task)
{
    pendingUiCaptureTasks_.emplace_back(id, task);
    if (!IsRequestedNextVSync()) {
        RequestNextVSync();
    }
}

void RSMainThread::PrepareUiCaptureTasks(std::shared_ptr<RSUniRenderVisitor> uniVisitor)
{
    std::vector<std::tuple<NodeId, std::function<void()>>> remainUiCaptureTasks;
    const auto& nodeMap = context_->GetNodeMap();
    for (auto [id, captureTask]: pendingUiCaptureTasks_) {
        auto node = nodeMap.GetRenderNode(id);
        bool flag = context_->GetUiCaptureCmdsExecutedFlag(id);
        if (!flag) {
            remainUiCaptureTasks.emplace_back(id, captureTask);
            continue;
        }
        context_->EraseUiCaptureCmdsExecutedFlag(id);
        if (!node) {
            RS_LOGW("RSMainThread::PrepareUiCaptureTasks node is nullptr");
        } else if (!node->IsOnTheTree() || node->IsDirty() || node->IsSubTreeDirty()) {
            node->PrepareSelfNodeForApplyModifiers();
        }
        uiCaptureTasks_.emplace(id, captureTask);
    }
    pendingUiCaptureTasks_.clear();
    pendingUiCaptureTasks_.insert(pendingUiCaptureTasks_.end(),
        remainUiCaptureTasks.begin(), remainUiCaptureTasks.end());
    remainUiCaptureTasks.clear();
}

void RSMainThread::ProcessUiCaptureTasks()
{
#ifdef RS_ENABLE_GPU
    while (!uiCaptureTasks_.empty()) {
        if (RSUiCaptureTaskParallel::GetCaptureCount() >= MAX_CAPTURE_COUNT) {
            return;
        }
        auto captureTask = std::get<1>(uiCaptureTasks_.front());
        uiCaptureTasks_.pop();
        captureTask();
    }
#endif
}

void RSMainThread::CheckBlurEffectCountStatistics(std::shared_ptr<RSRenderNode> rootNode)
{
    uint32_t terminateLimit = RSSystemProperties::GetBlurEffectTerminateLimit();
    if (terminateLimit == 0) {
        return;
    }
    static std::unique_ptr<AppExecFwk::AppMgrClient> appMgrClient =
        std::make_unique<AppExecFwk::AppMgrClient>();
    auto children = rootNode->GetChildren();
    if (children->empty()) {
        return;
    }
    auto displayNode = RSRenderNode::ReinterpretCast<RSDisplayRenderNode>(children->front());
    if (displayNode == nullptr) {
        return;
    }
    auto scbPid = displayNode->GetCurrentScbPid();
    int32_t uid = 0;
    std::string bundleName;
    for (auto& [pid, count] : rootNode->blurEffectCounter_) {
        if (pid == scbPid) {
            continue;
        }
        appMgrClient->GetBundleNameByPid(pid, bundleName, uid);
        if (count > terminateLimit) {
            auto res = appMgrClient->KillApplicationByUid(bundleName, uid);
            if (res) {
                RS_LOGI("RSMainThread: bundleName[%{public}s] was killed for too many blur effcts. "
                    "BlurEffectCountStatistics: pid[%{public}d] uid[%{public}d] blurCount[%{public}zu]",
                    bundleName.c_str(), pid, uid, count);
                rootNode->blurEffectCounter_.erase(pid);
            } else {
                RS_LOGE("RSMainThread: kill bundleName[%{public}s] for too many blur effcts failed. "
                    "BlurEffectCountStatistics: pid[%{public}d] uid[%{public}d] blurCount[%{public}zu]",
                    bundleName.c_str(), pid, uid, count);
            }
        }
    }
}

void RSMainThread::StartGPUDraw()
{
    gpuDrawCount_.fetch_add(1, std::memory_order_relaxed);
}

void RSMainThread::EndGPUDraw()
{
    if (gpuDrawCount_.fetch_sub(1, std::memory_order_relaxed) == 1) {
        // gpuDrawCount_ is now 0
        ClearUnmappedCache();
    }
}

void RSMainThread::ClearUnmappedCache()
{
    std::set<uint32_t> bufferIds;
    std::set<uint32_t> bufferMirrorIds;
    {
        std::lock_guard<std::mutex> lock(unmappedCacheSetMutex_);
        bufferIds.swap(unmappedCacheSet_);
        bufferMirrorIds.swap(unmappedVirScreenCacheSet_);
    }
    if (!bufferIds.empty()) {
        auto engine = GetRenderEngine();
        if (engine) {
            engine->ClearCacheSet(bufferIds);
        }
        RSHardwareThread::Instance().ClearRedrawGPUCompositionCache(bufferIds);
    }
    if (!bufferMirrorIds.empty()) {
        auto engine = GetRenderEngine();
        if (engine) {
            engine->ClearCacheSet(bufferIds, true);
        }
    }
}

void RSMainThread::UniRender(std::shared_ptr<RSBaseRenderNode> rootNode)
{
#ifdef RS_ENABLE_GPU
    if (isAccessibilityConfigChanged_) {
        RS_LOGD("RSMainThread::UniRender AccessibilityConfig has Changed");
    }
    RSUifirstManager::Instance().RefreshUIFirstParam();
    UpdateRogSizeIfNeeded();
    auto uniVisitor = std::make_shared<RSUniRenderVisitor>();
    uniVisitor->SetProcessorRenderEngine(GetRenderEngine());
    int64_t rsPeriod = 0;
    if (receiver_) {
        receiver_->GetVSyncPeriod(rsPeriod);
    }
    rsVsyncRateReduceManager_.ResetFrameValues(impl::CalculateRefreshRate(rsPeriod));

    if (isHardwareForcedDisabled_) {
        uniVisitor->MarkHardwareForcedDisabled();
        doDirectComposition_ = false;
        RS_OPTIONAL_TRACE_NAME_FMT("rs debug: %s HardwareForcedDisabled is true", __func__);
    }
    // need draw skipped node at cur frame
    doDirectComposition_ &= LIKELY(!RSUifirstManager::Instance().NeedNextDrawForSkippedNode());
    bool needTraverseNodeTree = true;
    needDrawFrame_ = true;
    bool pointerSkip = !RSPointerWindowManager::Instance().IsPointerCanSkipFrameCompareChange(false, true);
    if (doDirectComposition_ && !isDirty_ && !isAccessibilityConfigChanged_
        && !isCachedSurfaceUpdated_ && pointerSkip) {
        doDirectComposition_ = isHardwareEnabledBufferUpdated_;
        if (isHardwareEnabledBufferUpdated_) {
            needTraverseNodeTree = !DoDirectComposition(rootNode, !isLastFrameDirectComposition_);
        } else if (forceUpdateUniRenderFlag_) {
            RS_TRACE_NAME("RSMainThread::UniRender ForceUpdateUniRender");
        } else if (!pendingUiCaptureTasks_.empty()) {
            RS_LOGD("RSMainThread::Render pendingUiCaptureTasks_ not empty");
        } else {
            needDrawFrame_ = false;
            RS_LOGD("RSMainThread::Render nothing to update");
            RS_TRACE_NAME("RSMainThread::UniRender nothing to update");
            RSMainThread::Instance()->SetFrameIsRender(false);
            RSMainThread::Instance()->SetSkipJankAnimatorFrame(true);
            for (auto& node: hardwareEnabledNodes_) {
                if (!node->IsHardwareForcedDisabled()) {
                    node->MarkCurrentFrameHardwareEnabled();
                }
            }
            WaitUntilUploadTextureTaskFinishedForGL();
            renderThreadParams_->selfDrawables_ = std::move(selfDrawables_);
            renderThreadParams_->hardwareEnabledTypeDrawables_ = std::move(hardwareEnabledDrwawables_);
            RsFrameReport::GetInstance().ReportSchedEvent(FrameSchedEvent::RS_RENDER_END, {});
            renderThreadParams_->hardCursorDrawableMap_ = RSPointerWindowManager::Instance().GetHardCursorDrawableMap();
            return;
        }
    }

    isCachedSurfaceUpdated_ = false;
    if (needTraverseNodeTree) {
        RSUniRenderThread::Instance().PostTask([] {
            RSUniRenderThread::Instance().ResetClearMemoryTask();
        });
        RSUifirstManager::Instance().ProcessForceUpdateNode();
        RSPointerWindowManager::Instance().UpdatePointerInfo();
        doDirectComposition_ = false;
        RS_OPTIONAL_TRACE_NAME_FMT("rs debug: %s needTraverseNodeTree disable doDirectComposition", __func__);
        uniVisitor->SetAnimateState(doWindowAnimate_);
        uniVisitor->SetDirtyFlag(isDirty_ || isAccessibilityConfigChanged_ || forceUIFirstChanged_);
        forceUIFirstChanged_ = false;
        isFirstFrameOfPartialRender_ = (!isPartialRenderEnabledOfLastFrame_ || isRegionDebugEnabledOfLastFrame_) &&
            uniVisitor->GetIsPartialRenderEnabled() && !uniVisitor->GetIsRegionDebugEnabled();
        SetFocusLeashWindowId();
        uniVisitor->SetFocusedNodeId(focusNodeId_, focusLeashWindowId_);
        rsVsyncRateReduceManager_.SetFocusedNodeId(focusNodeId_);
        rootNode->QuickPrepare(uniVisitor);
        uniVisitor->ResetCrossNodesVisitedStatus();

#ifdef RES_SCHED_ENABLE
        const auto& nodeMapForFrameReport = GetContext().GetNodeMap();
        uint32_t frameRatePidFromRSS = ResschedEventListener::GetInstance()->GetCurrentPid();
        if (frameRatePidFromRSS != 0) {
            nodeMapForFrameReport.TraverseSurfaceNodesBreakOnCondition(
                [this, frameRatePidFromRSS](
                    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) {
                    if (surfaceNode == nullptr) {
                        return false;
                    }
                    uint32_t pidFromNode = ExtractPid(surfaceNode->GetId());
                    if (frameRatePidFromRSS != pidFromNode) {
                        return false;
                    }
                    auto dirtyManager = surfaceNode->GetDirtyManager();
                    if (dirtyManager == nullptr || dirtyManager->GetCurrentFrameDirtyRegion().IsEmpty()) {
                        return false;
                    }
                    ResschedEventListener::GetInstance()->ReportFrameCountAsync(pidFromNode);
                    return true;
            });
        }
#endif // RES_SCHED_ENABLE

        auto socPerfParam = std::static_pointer_cast<SOCPerfParam>(
            GraphicFeatureParamManager::GetInstance().GetFeatureParam(FEATURE_CONFIGS[SOC_PERF]));
        if (socPerfParam != nullptr && socPerfParam->IsMultilayersSOCPerfEnable()) {
            RSUniRenderUtil::MultiLayersPerf(uniVisitor->GetLayerNum());
        }
        CheckBlurEffectCountStatistics(rootNode);
        uniVisitor->SurfaceOcclusionCallbackToWMS();
        SelfDrawingNodeMonitor::GetInstance().TriggerRectChangeCallback();
        rsVsyncRateReduceManager_.SetUniVsync();
        renderThreadParams_->selfDrawables_ = std::move(selfDrawables_);
        renderThreadParams_->hardCursorDrawableMap_ = RSPointerWindowManager::Instance().GetHardCursorDrawableMap();
        renderThreadParams_->hardwareEnabledTypeDrawables_ = std::move(hardwareEnabledDrwawables_);
        renderThreadParams_->isOverDrawEnabled_ = isOverDrawEnabledOfCurFrame_;
        renderThreadParams_->isDrawingCacheDfxEnabled_ = isDrawingCacheDfxEnabledOfCurFrame_;
        isAccessibilityConfigChanged_ = false;
        isCurtainScreenUsingStatusChanged_ = false;
        RSPointLightManager::Instance()->PrepareLight();
        vsyncControlEnabled_ = rsVsyncRateReduceManager_.GetVRateDeviceSupport()
                               && RSSystemParameters::GetVSyncControlEnabled();
        systemAnimatedScenesEnabled_ = RSSystemParameters::GetSystemAnimatedScenesEnabled();
        if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
            WaitUntilUploadTextureTaskFinished(isUniRender_);
        }
        if (false) { // planning: move to prepare
            auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(
                rootNode->GetFirstChild());
            std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
            std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
            const auto& nodeMap = context_->GetNodeMap();
            RSUniRenderUtil::CacheSubThreadNodes(subThreadNodes_, subThreadNodes);
        }
        lastWatermarkFlag_ = watermarkFlag_;
        isPartialRenderEnabledOfLastFrame_ = uniVisitor->GetIsPartialRenderEnabled();
        isRegionDebugEnabledOfLastFrame_ = uniVisitor->GetIsRegionDebugEnabled();
        isOverDrawEnabledOfLastFrame_ = isOverDrawEnabledOfCurFrame_;
        isDrawingCacheDfxEnabledOfLastFrame_ = isDrawingCacheDfxEnabledOfCurFrame_;
        // set params used in render thread
        uniVisitor->SetUniRenderThreadParam(renderThreadParams_);
    } else if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        WaitUntilUploadTextureTaskFinished(isUniRender_);
    } else {
        RsFrameReport::GetInstance().ReportSchedEvent(FrameSchedEvent::RS_RENDER_END, {});
    }

    PrepareUiCaptureTasks(uniVisitor);
    screenPowerOnChanged_ = false;
    forceUpdateUniRenderFlag_ = false;
    idleTimerExpiredFlag_ = false;
#endif
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
        RS_LOGE("DoDirectComposition displayNode state error");
        return false;
    }
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("DoDirectComposition screenManager is nullptr");
        return false;
    }
    auto screenInfo = screenManager->QueryScreenInfo(displayNode->GetScreenId());
    if (screenInfo.state != ScreenState::HDI_OUTPUT_ENABLE) {
        RS_LOGE("DoDirectComposition: ScreenState error!");
        return false;
    }
#ifdef RS_ENABLE_GPU
    auto processor = RSProcessorFactory::CreateProcessor(displayNode->GetCompositeType());
    auto renderEngine = GetRenderEngine();
    if (processor == nullptr || renderEngine == nullptr) {
        RS_LOGE("DoDirectComposition: RSProcessor or renderEngine is null!");
        return false;
    }

    if (!processor->Init(*displayNode, displayNode->GetDisplayOffsetX(), displayNode->GetDisplayOffsetY(),
        INVALID_SCREEN_ID, renderEngine)) {
        RS_LOGE("DoDirectComposition: processor init failed!");
        return false;
    }
#endif
    auto drawable = displayNode->GetRenderDrawable();
    if (drawable != nullptr) {
#ifdef RS_ENABLE_GPU
        auto displayDrawable = std::static_pointer_cast<DrawableV2::RSDisplayRenderNodeDrawable>(drawable);
        auto surfaceHandler = displayDrawable->GetRSSurfaceHandlerOnDraw();
#else
        auto surfaceHandler = nullptr;
#endif
#ifdef RS_ENABLE_GPU
        if (RSAncoManager::Instance()->AncoOptimizeDisplayNode(surfaceHandler, hardwareEnabledNodes_,
            displayNode->GetRotation(), screenInfo.GetRotatedPhyWidth(), screenInfo.GetRotatedPhyHeight())) {
            return false;
        }
#endif
    }

    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExecute()) {
        RS_LOGW("DoDirectComposition: hardwareThread task has too many to Execute"
                " TaskNum:[%{public}d]", RSHardwareThread::Instance().GetunExecuteTaskNum());
        RSHardwareThread::Instance().DumpEventQueue();
    }
#ifdef RS_ENABLE_GPU
    uint32_t screenId = displayNode->GetScreenId();
    for (auto& surfaceNode : hardwareEnabledNodes_) {
        if (surfaceNode == nullptr) {
            RS_LOGE("DoDirectComposition: surfaceNode is null!");
            continue;
        }
        RSHdrUtil::UpdateSurfaceNodeNit(*surfaceNode, screenId);
        displayNode->CollectHdrStatus(surfaceNode->GetVideoHdrStatus());
        auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
        if (!surfaceNode->IsHardwareForcedDisabled()) {
            auto params = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
            if (!surfaceHandler->IsCurrentFrameBufferConsumed() && params->GetPreBuffer() != nullptr) {
                params->SetPreBuffer(nullptr);
                surfaceNode->AddToPendingSyncList();
            }
            std::shared_ptr<RSDisplayRenderNode> ancestor = nullptr;
            if (auto ancestorDisplayNode = surfaceNode->GetAncestorDisplayNode().lock()) {
                ancestor = ancestorDisplayNode->ReinterpretCastTo<RSDisplayRenderNode>();
            }
            if (ancestor != nullptr && params->GetDRMGlobalPositionEnabled()) {
                params->SetOffsetX(ancestor->GetDisplayOffsetX());
                params->SetOffsetY(ancestor->GetDisplayOffsetY());
                params->SetRogWidthRatio(params->IsDRMCrossNode() ?
                    screenManager->QueryScreenInfo(ancestor->GetScreenId()).GetRogWidthRatio() : 1.0f);
            } else {
                params->SetOffsetX(0);
                params->SetOffsetY(0);
                params->SetRogWidthRatio(1.0f);
            }
            processor->CreateLayer(*surfaceNode, *params);
            // buffer is synced to directComposition
            params->SetBufferSynced(true);
        }
    }
    RSLuminanceControl::Get().SetHdrStatus(screenId, displayNode->GetDisplayHdrStatus());
#endif
#ifdef RS_ENABLE_GPU
    RSPointerWindowManager::Instance().HardCursorCreateLayerForDirect(processor);
    if (RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        DoScreenRcdTask(*displayNode, processor);
    }
#endif
    if (waitForRT) {
#ifdef RS_ENABLE_GPU
        RSUniRenderThread::Instance().PostSyncTask([processor, displayNode]() {
            RS_TRACE_NAME("DoDirectComposition PostProcess");
            HgmCore::Instance().SetDirectCompositionFlag(true);
            processor->ProcessDisplaySurface(*displayNode);
            processor->PostProcess();
        });
#endif
    } else {
        HgmCore::Instance().SetDirectCompositionFlag(true);
#ifdef RS_ENABLE_GPU
        processor->ProcessDisplaySurface(*displayNode);
        processor->PostProcess();
#endif
    }

    RS_LOGD("DoDirectComposition end");
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
#ifdef RS_ENABLE_GPU
        if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
            WaitUntilUploadTextureTaskFinished(isUniRender_);
        }
#endif
        RS_LOGE("RSMainThread::Render GetGlobalRootRenderNode fail");
        return;
    }
    if (isUniRender_) {
#ifdef RS_ENABLE_GPU
        auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
        renderThreadParams_->SetTimestamp(hgmCore.GetCurrentTimestamp());
        renderThreadParams_->SetActualTimestamp(hgmCore.GetActualTimestamp());
        renderThreadParams_->SetVsyncId(hgmCore.GetVsyncId());
        renderThreadParams_->SetForceRefreshFlag(isForceRefresh_);
        renderThreadParams_->SetRequestNextVsyncFlag(needRequestNextVsyncAnimate_);
        renderThreadParams_->SetPendingScreenRefreshRate(hgmCore.GetPendingScreenRefreshRate());
        renderThreadParams_->SetPendingConstraintRelativeTime(hgmCore.GetPendingConstraintRelativeTime());
        renderThreadParams_->SetForceCommitLayer(isHardwareEnabledBufferUpdated_ || forceUpdateUniRenderFlag_);
        renderThreadParams_->SetOcclusionEnabled(RSSystemProperties::GetOcclusionEnabled());
        renderThreadParams_->SetCacheEnabledForRotation(RSSystemProperties::GetCacheEnabledForRotation());
        renderThreadParams_->SetUIFirstCurrentFrameCanSkipFirstWait(
            RSUifirstManager::Instance().GetCurrentFrameSkipFirstWait());
        // If use DoDirectComposition, we do not sync renderThreadParams,
        // so we use hgmCore to keep force refresh flag, then reset flag.
        hgmCore.SetForceRefreshFlag(isForceRefresh_);
        isForceRefresh_ = false;
        uint64_t fastComposeTimeStampDiff = 0;
        if (lastFastComposeTimeStamp_ == timestamp_) {
            fastComposeTimeStampDiff = lastFastComposeTimeStampDiff_;
        }
        renderThreadParams_->SetFastComposeTimeStampDiff(fastComposeTimeStampDiff);
        hgmCore.SetFastComposeTimeStampDiff(fastComposeTimeStampDiff);
#endif
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
#ifdef RS_ENABLE_GPU
        renderThreadParams_->SetWatermark(watermarkFlag_, watermarkImg_);
        {
            std::lock_guard<std::mutex> lock(watermarkMutex_);
            renderThreadParams_->SetWatermarks(surfaceNodeWatermarks_);
        }

        renderThreadParams_->SetCurtainScreenUsingStatus(isCurtainScreenOn_);
#ifdef RS_ENABLE_GPU
        UniRender(rootNode);
#endif
        frameCount_++;
#endif
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
        PerfForBlurIfNeeded();
    }
    RSSurfaceBufferCallbackManager::Instance().RunSurfaceBufferCallback();
    CheckSystemSceneStatus();
    UpdateLuminanceAndColorTemp();
}

void RSMainThread::OnUniRenderDraw()
{
    if (!isUniRender_) {
        return;
    }
#ifdef RS_ENABLE_GPU
    needPostAndWait_ = !doDirectComposition_ && needDrawFrame_;
    if (needPostAndWait_) {
        renderThreadParams_->SetContext(context_);
        renderThreadParams_->SetDiscardJankFrames(GetDiscardJankFrames());
        drawFrame_.SetRenderThreadParams(renderThreadParams_);
        RsFrameReport::GetInstance().PostAndWait();
        drawFrame_.PostAndWait();
        return;
    }
    // To remove ClearMemoryTask for first frame of doDirectComposition or if needed
    if ((doDirectComposition_ && !isLastFrameDirectComposition_) || isNeedResetClearMemoryTask_ || !needDrawFrame_) {
        RSUniRenderThread::Instance().PostTask([] {
            RSUniRenderThread::Instance().ResetClearMemoryTask(true);
        });
        isNeedResetClearMemoryTask_ = false;
    }

    UpdateDisplayNodeScreenId();
#endif
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

void RSMainThread::CallbackDrawContextStatusToWMS(bool isUniRender)
{
#ifdef RS_ENABLE_GPU
    auto& curDrawStatusVec = isUniRender ? RSUniRenderThread::Instance().GetDrawStatusVec() : curDrawStatusVec_;
    auto timestamp = isUniRender ? RSUniRenderThread::Instance().GetCurrentTimestamp() : timestamp_;
#else
    auto& curDrawStatusVec = curDrawStatusVec_;
    auto timestamp = timestamp_;
#endif
    VisibleData drawStatusVec;
    for (auto dynamicNodeId : curDrawStatusVec) {
        if (lastDrawStatusMap_.find(dynamicNodeId) == lastDrawStatusMap_.end()) {
            drawStatusVec.emplace_back(std::make_pair(dynamicNodeId,
                WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_DYNAMIC_STATUS));
            RS_OPTIONAL_TRACE_NAME_FMT("%s nodeId[%" PRIu64 "] status[%d]",
                __func__, dynamicNodeId, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_DYNAMIC_STATUS);
        }
        lastDrawStatusMap_[dynamicNodeId] = timestamp;
    }
    auto drawStatusIter = lastDrawStatusMap_.begin();
    while (drawStatusIter != lastDrawStatusMap_.end()) {
        if (timestamp - drawStatusIter->second > MAX_DYNAMIC_STATUS_TIME) {
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
    curDrawStatusVec.clear();
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
            curSurface->SetOcclusionInSpecificScenes(rsVsyncRateReduceManager_.GetVRateDeviceSupport()
                                                    && !threeFingerScenesList_.empty());
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
    bool needRefreshRates = systemAnimatedScenesList_.empty() &&
        rsVsyncRateReduceManager_.GetIsReduceBySystemAnimatedScenes();
    if (!winDirty && !needRefreshRates) {
        if (SurfaceOcclusionCallBackIfOnTreeStateChanged()) {
            SurfaceOcclusionCallback();
        }
        return;
    }
    rsVsyncRateReduceManager_.SetIsReduceBySystemAnimatedScenes(false);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstVisMapForVsyncRate;
    for (auto& surfaces : curAllSurfacesInDisplay) {
        CalcOcclusionImplementation(surfaces.first, surfaces.second, dstCurVisVec, dstVisMapForVsyncRate);
    }

    // Callback to WMS and QOS
    CallbackToWMS(dstCurVisVec);
    rsVsyncRateReduceManager_.SetVSyncRateByVisibleLevel(dstVisMapForVsyncRate, curAllSurfaces);
    // Callback for registered self drawing surfacenode
    SurfaceOcclusionCallback();
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
    std::list<std::pair<sptr<RSISurfaceOcclusionChangeCallback>, float>> callbackList;
    {
        std::lock_guard<std::mutex> lock(surfaceOcclusionMutex_);
        for (auto &listener : surfaceOcclusionListeners_) {
            if (!CheckSurfaceOcclusionNeedProcess(listener.first)) {
                continue;
            }
            uint8_t level = 0;
            float visibleAreaRatio = 0.0f;
            bool isOnTheTree = savedAppWindowNode_[listener.first].first->IsOnTheTree();
            if (isOnTheTree) {
                const auto& property = savedAppWindowNode_[listener.first].second->GetRenderProperties();
                auto& geoPtr = property.GetBoundsGeometry();
                if (!geoPtr) {
                    continue;
                }
                auto absRect = geoPtr->GetAbsRect();
                if (absRect.IsEmpty()) {
                    continue;
                }
                auto surfaceRegion = Occlusion::Region{ Occlusion::Rect{ absRect } };
                auto visibleRegion = savedAppWindowNode_[listener.first].second->GetVisibleRegion();
                // take the intersection of these two regions to get rid of shadow area, then calculate visible ratio
                visibleAreaRatio = static_cast<float>(visibleRegion.And(surfaceRegion).Area()) /
                    static_cast<float>(surfaceRegion.Area());
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
                    callbackList.push_back(std::make_pair(std::get<1>(listener.second), visibleAreaRatio));
                }
            }
        }
    }
    for (auto &callback : callbackList) {
        if (callback.first) {
            callback.first->OnSurfaceOcclusionVisibleChanged(callback.second);
        }
    }
}

bool RSMainThread::CheckSurfaceOcclusionNeedProcess(NodeId id)
{
    const auto& nodeMap = context_->GetNodeMap();
    if (savedAppWindowNode_.find(id) == savedAppWindowNode_.end()) {
        auto node = nodeMap.GetRenderNode(id);
        if (!node || !node->IsOnTheTree()) {
            RS_LOGD("RSMainThread::SurfaceOcclusionCallback cannot find surfacenode %{public}"
                PRIu64 ".", id);
            return false;
        }
        auto appWindowNodeId = node->GetInstanceRootNodeId();
        if (appWindowNodeId == INVALID_NODEID) {
            RS_LOGD("RSMainThread::SurfaceOcclusionCallback surfacenode %{public}"
                PRIu64 " cannot find app window node.", id);
            return false;
        }
        auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>();
        auto appWindowNode =
            RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodeMap.GetRenderNode(appWindowNodeId));
        if (!surfaceNode || !appWindowNode) {
            RS_LOGD("RSMainThread::SurfaceOcclusionCallback ReinterpretCastTo fail.");
            return false;
        }
        savedAppWindowNode_[id] = std::make_pair(surfaceNode, appWindowNode);
    } else {
        if (!savedAppWindowNode_[id].first || !savedAppWindowNode_[id].second) {
            return false;
        }
        auto appWindowNodeId = savedAppWindowNode_[id].first->GetInstanceRootNodeId();
        auto lastAppWindowNodeId = savedAppWindowNode_[id].second->GetId();
        if (appWindowNodeId != lastAppWindowNodeId && appWindowNodeId != INVALID_NODEID) {
            auto appWindowNode =
                RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodeMap.GetRenderNode(appWindowNodeId));
            if (!appWindowNode) {
                return false;
            }
            savedAppWindowNode_[id].second = appWindowNode;
        }
    }
    return true;
}

bool RSMainThread::WaitHardwareThreadTaskExecute()
{
#ifdef RS_ENABLE_GPU
    std::unique_lock<std::mutex> lock(hardwareThreadTaskMutex_);
    return hardwareThreadTaskCond_.wait_until(lock, std::chrono::system_clock::now() +
        std::chrono::milliseconds(WAIT_FOR_HARDWARE_THREAD_TASK_TIMEOUT),
        []() { return RSHardwareThread::Instance().GetunExecuteTaskNum() <= HARDWARE_THREAD_TASK_NUM; });
#else
    return false;
#endif
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
        .callbackWithId_ = [this](uint64_t timestamp, uint64_t frameCount, void* data) {
                OnVsync(timestamp, frameCount, data);
            },
    };
    if (receiver_ != nullptr) {
        requestNextVsyncNum_++;
        if (requestNextVsyncNum_ > REQUEST_VSYNC_NUMBER_LIMIT) {
            RS_LOGD("RSMainThread::RequestNextVSync too many times:%{public}d", requestNextVsyncNum_.load());
            if ((requestNextVsyncNum_ - currentNum_) >= REQUEST_VSYNC_DUMP_NUMBER) {
                RS_LOGW("RSMainThread::RequestNextVSync EventHandler is idle: %{public}d", handler_->IsIdle());
                DumpEventHandlerInfo();
            }
        }
        receiver_->RequestNextVSync(fcb, fromWhom, lastVSyncTS);
        if (requestNextVsyncNum_ >= VSYNC_LOG_ENABLED_TIMES_THRESHOLD &&
            requestNextVsyncNum_ % VSYNC_LOG_ENABLED_STEP_TIMES == 0) {
            vsyncGenerator_->PrintGeneratorStatus();
            rsVSyncDistributor_->PrintConnectionsStatus();
        }
    }
}

void RSMainThread::DumpEventHandlerInfo()
{
    RSEventDumper dumper;
    handler_->Dump(dumper);
    dumpInfo_ = dumper.GetOutput().c_str();
    RS_LOGW("RSMainThread::RequestNextVSync HistoryEventQueue is %{public}s", SubHistoryEventQueue(dumpInfo_).c_str());
    size_t immediateStart = dumpInfo_.find("Immediate priority event queue infomation:");
    if (immediateStart != std::string::npos) {
        size_t immediateEnd = dumpInfo_.find("RSEventDumper High priority event", immediateStart);
        if (immediateEnd != std::string::npos) {
            std::string priorityEventQueue = dumpInfo_.substr(immediateStart, immediateEnd - immediateStart).c_str();
            RS_LOGW("RSMainThread::RequestNextVSync PriorityEventQueue is %{public}s",
                SubPriorityEventQueue(priorityEventQueue).c_str());
        }
    }
    currentNum_ = requestNextVsyncNum_.load();
}

std::string RSMainThread::SubHistoryEventQueue(std::string input)
{
    const int CONTEXT_LINES = 3;
    const std::string TARGET_STRING = "completeTime time = ,";
    std::vector<std::string> lines;
    std::string line;
    std::istringstream stream(input);
    bool foundTargetStr = false;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    std::string result;
    for (int i = 0; i < static_cast<int>(lines.size()); ++i) {
        if (lines[i].find(TARGET_STRING) != std::string::npos) {
            foundTargetStr = true;
            int start = std::max(0, i - CONTEXT_LINES);
            int end = std::min(static_cast<int>(lines.size() - 1), i + CONTEXT_LINES);
            for (int j = start; j < end; ++j) {
                result += lines[j] + "\n";
            }
            break;
        }
    }
    if (!foundTargetStr) {
        RS_LOGW("RSMainThread::SubHistoryEventQueue No task is being executed");
        // If the TARGET_STRING is not found, dump the information of the first 10 lines.
        int end = std::min(static_cast<int>(lines.size() - 1) - 1, 9);
        for (int j = 0; j <= end; ++j) {
            result += lines[j] + "\n";
        }
    }
    return result;
}

std::string RSMainThread::SubPriorityEventQueue(std::string input)
{
    std::string result;
    std::string line;
    std::istringstream stream(input);

    while (std::getline(stream, line)) {
        if (line.find("RSEventDumper No.") != std::string::npos) {
            size_t dot_pos = line.find('.');
            if (dot_pos != std::string::npos) {
                size_t space_pos = line.find(' ', dot_pos);
                if (space_pos != std::string::npos) {
                    std::string num_str = line.substr(dot_pos + 1, space_pos - dot_pos - 1);
                    int num = std::stoi(num_str);
                    if (num >= 1 && num <= 3) { // dump the first three lines of information.
                        result += line + "\n";
                    }
                }
            }
        } else if (line.find("Total size of Immediate Events") != std::string::npos) {
            result += line + "\n";
        }
    }
    return result;
}

void RSMainThread::ProcessScreenHotPlugEvents()
{
    auto screenManager_ = CreateOrGetScreenManager();
    if (!screenManager_) {
        return;
    }
#ifdef RS_ENABLE_GPU
    if (!screenManager_->TrySimpleProcessHotPlugEvents()) {
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            RSHardwareThread::Instance().PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
        } else {
            PostTask([=]() { screenManager_->ProcessScreenHotPlugEvents(); });
        }
    }
#endif
}

void RSMainThread::OnVsync(uint64_t timestamp, uint64_t frameCount, void* data)
{
    SetFrameInfo(frameCount, false);
    const int64_t onVsyncStartTime = GetCurrentSystimeMs();
    const int64_t onVsyncStartTimeSteady = GetCurrentSteadyTimeMs();
    const float onVsyncStartTimeSteadyFloat = GetCurrentSteadyTimeMsFloat();
    RSJankStatsOnVsyncStart(onVsyncStartTime, onVsyncStartTimeSteady, onVsyncStartTimeSteadyFloat);
    timestamp_ = timestamp;
    dvsyncRsTimestamp_.store(timestamp_);
    curTime_ = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    RS_PROFILER_PATCH_TIME(timestamp_);
    RS_PROFILER_PATCH_TIME(curTime_);
    requestNextVsyncNum_ = 0;
    vsyncId_ = frameCount;
    frameCount_++;
    if (isUniRender_) {
#ifdef RS_ENABLE_GPU
        MergeToEffectiveTransactionDataMap(cachedTransactionDataMap_);
        if (RSUnmarshalThread::Instance().CachedTransactionDataEmpty()) {
            // set needWaitUnmarshalFinished_ to false, it means mainLoop do not wait unmarshalBarrierTask_
            needWaitUnmarshalFinished_ = false;
        } else {
            if (!RSSystemProperties::GetUnmarshParallelFlag()) {
                RSUnmarshalThread::Instance().PostTask(unmarshalBarrierTask_, DVSYNC_NOTIFY_UNMARSHAL_TASK_NAME);
            }
        }
#endif
    }
    mainLoop_();
#if defined(RS_ENABLE_CHIPSET_VSYNC)
    SetVsyncInfo(timestamp);
#endif
    ProcessScreenHotPlugEvents();
    RSJankStatsOnVsyncEnd(onVsyncStartTime, onVsyncStartTimeSteady, onVsyncStartTimeSteadyFloat);
}

void RSMainThread::RSJankStatsOnVsyncStart(int64_t onVsyncStartTime, int64_t onVsyncStartTimeSteady,
                                           float onVsyncStartTimeSteadyFloat)
{
    if (isUniRender_) {
#ifdef RS_ENABLE_GPU
        if (!renderThreadParams_) {
            // fill the params, and sync to render thread later
            renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
        }
        renderThreadParams_->SetIsUniRenderAndOnVsync(true);
        renderThreadParams_->SetOnVsyncStartTime(onVsyncStartTime);
        renderThreadParams_->SetOnVsyncStartTimeSteady(onVsyncStartTimeSteady);
        renderThreadParams_->SetOnVsyncStartTimeSteadyFloat(onVsyncStartTimeSteadyFloat);
        SetSkipJankAnimatorFrame(false);
#endif
    }
}

void RSMainThread::AddSelfDrawingNodes(std::shared_ptr<RSSurfaceRenderNode> selfDrawingNode)
{
    selfDrawingNodes_.emplace_back(selfDrawingNode);
}

const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& RSMainThread::GetSelfDrawingNodes() const
{
    return selfDrawingNodes_;
}

void RSMainThread::ClearSelfDrawingNodes()
{
    selfDrawingNodes_.clear();
}

void RSMainThread::RSJankStatsOnVsyncEnd(int64_t onVsyncStartTime, int64_t onVsyncStartTimeSteady,
                                         float onVsyncStartTimeSteadyFloat)
{
#ifdef RS_ENABLE_GPU
    if (isUniRender_ && !needPostAndWait_) {
        const JankDurationParams rsParams = { .timeStart_ = onVsyncStartTime,
                                              .timeStartSteady_ = onVsyncStartTimeSteady,
                                              .timeStartSteadyFloat_ = onVsyncStartTimeSteadyFloat,
                                              .timeEnd_ = GetCurrentSystimeMs(),
                                              .timeEndSteady_ = GetCurrentSteadyTimeMs(),
                                              .timeEndSteadyFloat_ = GetCurrentSteadyTimeMsFloat(),
                                              .refreshRate_ = GetDynamicRefreshRate(),
                                              .discardJankFrames_ = GetDiscardJankFrames(),
                                              .skipJankAnimatorFrame_ = GetSkipJankAnimatorFrame() };
        drawFrame_.PostDirectCompositionJankStats(rsParams);
    }
    if (isUniRender_) {
        SetDiscardJankFrames(false);
    }
#endif
}

#if defined(RS_ENABLE_CHIPSET_VSYNC)
void RSMainThread::ConnectChipsetVsyncSer()
{
    if (initVsyncServiceFlag_ && (OHOS::Camera::ChipsetVsyncImpl::Instance().InitChipsetVsyncImpl() == -1)) {
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
    bool allowFramerateChange = false;
    if (receiver_) {
        receiver_->GetVSyncPeriod(vsyncPeriod);
    }
    if (context_) {
        // framerate not vote at animation and mult-window scenario
        allowFramerateChange = context_->GetAnimatingNodeList().empty() &&
            context_->GetNodeMap().GetVisibleLeashWindowCount() < MULTI_WINDOW_PERF_START_NUM;
    }
    OHOS::Camera::ChipsetVsyncImpl::Instance().SetVsyncImpl(timestamp, vsyncPeriod, allowFramerateChange);
    RS_LOGD("UpdateVsyncTime = %{public}lld, period = %{public}lld, allowFramerateChange = %{public}d",
        static_cast<long long>(timestamp), static_cast<long long>(vsyncPeriod), allowFramerateChange);
}
#endif

void RSMainThread::Animate(uint64_t timestamp)
{
    RS_TRACE_FUNC();
    lastAnimateTimestamp_ = timestamp;
    rsCurrRange_.Reset();
    needRequestNextVsyncAnimate_ = false;

    if (context_->animatingNodeList_.empty()) {
        doWindowAnimate_ = false;
        context_->SetRequestedNextVsyncAnimate(false);
        return;
    }
    UpdateAnimateNodeFlag();
    bool curWinAnim = false;
    bool needRequestNextVsync = false;
    // isCalculateAnimationValue is embedded modify for stat animate frame drop
    bool isCalculateAnimationValue = false;
    bool isRateDeciderEnabled = (context_->animatingNodeList_.size() <= CAL_NODE_PREFERRED_FPS_LIMIT);
    bool isDisplaySyncEnabled = true;
    int64_t period = 0;
    if (receiver_) {
        receiver_->GetVSyncPeriod(period);
    }
    RSRenderAnimation::isCalcAnimateVelocity_ = isRateDeciderEnabled;
    uint32_t totalAnimationSize = 0;
    uint32_t animatingNodeSize = context_->animatingNodeList_.size();
    bool needPrintAnimationDFX = IsTagEnabled(HITRACE_TAG_GRAPHIC_AGP) ? true : false;
    std::set<pid_t> animationPids;
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
        auto frameRateGetFunc =
            [this](const RSPropertyUnit unit, float velocity, int32_t area, int32_t length) -> int32_t {
            auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
            if (frameRateMgr != nullptr) {
                return frameRateMgr->GetExpectedFrameRate(unit, velocity, area, length);
            }
            return 0;
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
#ifdef RS_ENABLE_GPU
                auto surfacenode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
                surfacenode->SetAnimateState();
#endif
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
        doDirectComposition_ = false;
        RS_OPTIONAL_TRACE_NAME_FMT("rs debug: %s doDirectComposition false", __func__);
        HgmEnergyConsumptionPolicy::Instance().StatisticAnimationTime(timestamp / NS_PER_MS);
        RequestNextVSync("animate", timestamp_);
    } else if (isUniRender_) {
#ifdef RS_ENABLE_GPU
        renderThreadParams_->SetImplicitAnimationEnd(true);
#endif
    }
    context_->SetRequestedNextVsyncAnimate(needRequestNextVsync);

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

    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        RSTransactionMetricCollector::GetInstance().Collect(rsTransactionData);
        cachedTransactionDataMap_[rsTransactionData->GetSendingPid()].emplace_back(std::move(rsTransactionData));
    }
    PostTask([this]() {
        if (!context_) {
            return;
        }
        // animation node will call RequestNextVsync() in mainLoop_, here we simply ignore animation scenario
        // and also ignore mult-window scenario
        bool isNeedSingleFrameCompose = context_->GetAnimatingNodeList().empty() &&
            context_->GetNodeMap().GetVisibleLeashWindowCount() < MULTI_WINDOW_PERF_START_NUM;
        if (isNeedSingleFrameCompose) {
            ForceRefreshForUni();
        } else {
            RequestNextVSync();
        }
    });
}

void RSMainThread::RecvRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    if (!rsTransactionData) {
        return;
    }
    int64_t timestamp = rsTransactionData->GetTimestamp();
    if (isUniRender_) {
#ifdef RS_ENABLE_GPU
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        RSTransactionMetricCollector::GetInstance().Collect(rsTransactionData);
        cachedTransactionDataMap_[rsTransactionData->GetSendingPid()].emplace_back(std::move(rsTransactionData));
#endif
    } else {
        ClassifyRSTransactionData(rsTransactionData);
    }
    RequestNextVSync("UI", timestamp);
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
    applicationAgentMap_.insert_or_assign(pid, app);
}

void RSMainThread::UnRegisterApplicationAgent(sptr<IApplicationAgent> app)
{
    RSReclaimMemoryManager::Instance().TriggerReclaimTask();

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
    RsFrameReport::GetInstance().SendCommandsStart();
    if (!context_->needSyncFinishAnimationList_.empty()) {
        for (const auto& [nodeId, animationId, token] : context_->needSyncFinishAnimationList_) {
            RS_LOGI("RSMainThread::SendCommands sync finish animation node is %{public}" PRIu64 ","
                " animation is %{public}" PRIu64, nodeId, animationId);
            std::unique_ptr<RSCommand> command =
                std::make_unique<RSAnimationCallback>(nodeId, animationId, token, FINISHED);
            RSMessageProcessor::Instance().AddUIMessage(ExtractPid(animationId), std::move(command));
        }
        context_->needSyncFinishAnimationList_.clear();
    }
    if (!RSMessageProcessor::Instance().HasTransaction()) {
        return;
    }

    // dispatch messages to corresponding application
    auto transactionMapPtr = std::make_shared<std::unordered_map<uint32_t, std::shared_ptr<RSTransactionData>>>(
        RSMessageProcessor::Instance().GetAllTransactions());
    PostTask([this, transactionMapPtr]() {
        std::string dfxString;
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
            if (transactionPtr != nullptr) {
                dfxString += "[pid:" + std::to_string(pid) + ",index:" + std::to_string(transactionPtr->GetIndex())
                    + ",cnt:" + std::to_string(transactionPtr->GetCommandCount()) + "]";
            }
            app->OnTransaction(transactionPtr);
        }
        RS_LOGI("RS send to %{public}s", dfxString.c_str());
        RS_TRACE_NAME_FMT("RSMainThread::SendCommand to %s", dfxString.c_str());
    });
}

void RSMainThread::TransactionDataMapDump(const TransactionDataMap& transactionDataMap, std::string& dumpString)
{
    for (const auto& [pid, transactionData] : transactionDataMap) {
        dumpString.append("[pid: " + std::to_string(pid));
        for (const auto& transcation : transactionData) {
            dumpString.append(", [index: " + std::to_string(transcation->GetIndex()));
            transcation->DumpCommand(dumpString);
            dumpString.append("]");
        }
        dumpString.append("]");
    }
}

void RSMainThread::RenderServiceTreeDump(std::string& dumpString, bool forceDumpSingleFrame, bool needUpdateJankStats)
{
    if (LIKELY(forceDumpSingleFrame)) {
        int64_t onVsyncStartTime = 0;
        int64_t onVsyncStartTimeSteady = 0;
        float onVsyncStartTimeSteadyFloat = 0.0f;
        if (needUpdateJankStats) {
            onVsyncStartTime = GetCurrentSystimeMs();
            onVsyncStartTimeSteady = GetCurrentSteadyTimeMs();
            onVsyncStartTimeSteadyFloat = GetCurrentSteadyTimeMsFloat();
            RSJankStatsOnVsyncStart(onVsyncStartTime, onVsyncStartTimeSteady, onVsyncStartTimeSteadyFloat);
        }
        RS_TRACE_NAME("GetDumpTree");
        dumpString.append("-- RS transactionFlags: " + transactionFlags_ + "\n");
        dumpString.append("-- current timeStamp: " + std::to_string(timestamp_) + "\n");
        dumpString.append("-- vsyncId: " + std::to_string(vsyncId_) + "\n");
        dumpString.append("Animating Node: [");
        for (auto& [nodeId, _]: context_->animatingNodeList_) {
            dumpString.append(std::to_string(nodeId) + ", ");
        }
        dumpString.append("];\n");
        dumpString.append("-- CacheTransactionData: ");
        {
            std::lock_guard<std::mutex> lock(transitionDataMutex_);
            TransactionDataMapDump(cachedTransactionDataMap_, dumpString);
        }
        dumpString.append("\n");
        const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
        if (rootNode == nullptr) {
            dumpString.append("rootNode is null\n");
            return;
        }
        rootNode->DumpTree(0, dumpString);

        dumpString += "\n-- RenderServiceUniThread\n";
        RSUniRenderThread::Instance().RenderServiceTreeDump(dumpString);

        if (needUpdateJankStats) {
            needPostAndWait_ = false;
            RSJankStatsOnVsyncEnd(onVsyncStartTime, onVsyncStartTimeSteady, onVsyncStartTimeSteadyFloat);
        }
    } else {
        dumpString += g_dumpStr;
        g_dumpStr = "";
    }
}

static std::string Data2String(std::string data, uint32_t tagetNumber)
{
    if (data.length() < tagetNumber) {
        return std::string(tagetNumber - data.length(), ' ') + data;
    } else {
        return data;
    }
}

void RSMainThread::RenderServiceAllNodeDump(DfxString& log)
{
    std::unordered_map<int, std::pair<int, int>> node_info; // [pid, [count, ontreecount]]
    std::unordered_map<int, int> nullnode_info; // [pid, count]
    for (auto& [nodeId, info] : MemoryTrack::Instance().GetMemNodeMap()) {
        auto node = context_->GetMutableNodeMap().GetRenderNode(nodeId);
        int pid = info.pid;
        if (node) {
            if (node_info.count(pid)) {
                node_info[pid].first++;
                node_info[pid].second += node->IsOnTheTree() ? 1 : 0;
            } else {
                node_info[pid].first = 1;
                node_info[pid].second = node->IsOnTheTree() ? 1 : 0;
            }
        } else {
            if (nullnode_info.count(pid)) {
                nullnode_info[pid]++;
            } else {
                nullnode_info[pid] = 1;
            }
        }
    }
    std::string log_str = Data2String("Pid", NODE_DUMP_STRING_LEN) + "\t" +
        Data2String("Count", NODE_DUMP_STRING_LEN) + "\t" +
        Data2String("OnTree", NODE_DUMP_STRING_LEN);
    log.AppendFormat("%s\n", log_str.c_str());
    for (auto& [pid, info]: node_info) {
        log_str = Data2String(std::to_string(pid), NODE_DUMP_STRING_LEN) + "\t" +
            Data2String(std::to_string(info.first), NODE_DUMP_STRING_LEN) + "\t" +
            Data2String(std::to_string(info.second), NODE_DUMP_STRING_LEN);
        log.AppendFormat("%s\n", log_str.c_str());
    }
    if (!nullnode_info.empty()) {
        log_str = "Purgeable node: \n" +
            Data2String("Pid", NODE_DUMP_STRING_LEN) + "\t" +
            Data2String("Count", NODE_DUMP_STRING_LEN);
        log.AppendFormat("%s\n", log_str.c_str());
        for (auto& [pid, info]: nullnode_info) {
            log_str = Data2String(std::to_string(pid), NODE_DUMP_STRING_LEN) + "\t" +
                Data2String(std::to_string(info), NODE_DUMP_STRING_LEN);
            log.AppendFormat("%s\n", log_str.c_str());
        }
    }
    node_info.clear();
    nullnode_info.clear();
}

void RSMainThread::RenderServiceAllSurafceDump(DfxString& log)
{
    log.AppendFormat("%s\n", "the memory of size of all surfaces buffer: ");
    const auto& nodeMap = context_->GetNodeMap();
    nodeMap.TraversalNodes([&log](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node == nullptr || !node->IsInstanceOf<RSSurfaceRenderNode>()) {
            return;
        }
        const auto& surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
        const auto& surfaceConsumer = surfaceNode->GetRSSurfaceHandler()->GetConsumer();
        if (surfaceConsumer == nullptr) {
            return;
        }
        std::string dumpSurfaceInfo;
        surfaceConsumer->Dump(dumpSurfaceInfo);
        if (dumpSurfaceInfo.find("sequence") == std::string::npos) {
            return;
        }
        log.AppendFormat("pid = %d nodeId:%llu", ExtractPid(node->GetId()), node->GetId());
        log.AppendFormat("%s\n", dumpSurfaceInfo.c_str());
    });
}

void RSMainThread::SendClientDumpNodeTreeCommands(uint32_t taskId)
{
    RS_TRACE_NAME_FMT("DumpClientNodeTree start task[%u]", taskId);
    std::unique_lock<std::mutex> lock(nodeTreeDumpMutex_);
    if (nodeTreeDumpTasks_.find(taskId) != nodeTreeDumpTasks_.end()) {
        RS_LOGW("SendClientDumpNodeTreeCommands task[%{public}u] duplicate", taskId);
        return;
    }

    std::unordered_map<pid_t, std::vector<NodeId>> topNodes;
    if (const auto& rootNode = context_->GetGlobalRootRenderNode()) {
        for (const auto& displayNode : *rootNode->GetSortedChildren()) {
            for (const auto& node : *displayNode->GetSortedChildren()) {
                NodeId id = node->GetId();
                topNodes[ExtractPid(id)].push_back(id);
            }
        }
    }
    context_->GetNodeMap().TraversalNodes([this, &topNodes] (const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node->IsOnTheTree() && node->GetType() == RSRenderNodeType::ROOT_NODE) {
            if (auto parent = node->GetParent().lock()) {
                NodeId id = parent->GetId();
                topNodes[ExtractPid(id)].push_back(id);
            }
            NodeId id = node->GetId();
            topNodes[ExtractPid(id)].push_back(id);
        }
    });

    auto& task = nodeTreeDumpTasks_[taskId];
    for (const auto& [pid, nodeIds] : topNodes) {
        auto iter = applicationAgentMap_.find(pid);
        if (iter == applicationAgentMap_.end() || !iter->second) {
            continue;
        }
        auto transactionData = std::make_shared<RSTransactionData>();
        for (auto id : nodeIds) {
            auto command = std::make_unique<RSDumpClientNodeTree>(id, pid, taskId);
            transactionData->AddCommand(std::move(command), id, FollowType::NONE);
            task.count++;
            RS_TRACE_NAME_FMT("DumpClientNodeTree add task[%u] pid[%u] node[%" PRIu64 "]",
                taskId, pid, id);
            RS_LOGI("SendClientDumpNodeTreeCommands add task[%{public}u] pid[%u] node[%" PRIu64 "]",
                taskId, pid, id);
        }
        iter->second->OnTransaction(transactionData);
    }
    RS_LOGI("SendClientDumpNodeTreeCommands send task[%{public}u] count[%{public}zu]",
        taskId, task.count);
}

void RSMainThread::CollectClientNodeTreeResult(uint32_t taskId, std::string& dumpString, size_t timeout)
{
    std::unique_lock<std::mutex> lock(nodeTreeDumpMutex_);
    {
        RS_TRACE_NAME_FMT("DumpClientNodeTree wait task[%u]", taskId);
        nodeTreeDumpCondVar_.wait_for(lock, std::chrono::milliseconds(timeout), [this, taskId] () {
            const auto& task = nodeTreeDumpTasks_[taskId];
            return task.completionCount == task.count;
        });
    }

    const auto& task = nodeTreeDumpTasks_[taskId];
    size_t completed = task.completionCount;
    RS_TRACE_NAME_FMT("DumpClientNodeTree end task[%u] completionCount[%zu]", taskId, completed);
    dumpString += "\n-- ClientNodeTreeDump: ";
    for (const auto& [pid, data] : task.data) {
        dumpString += "\n| pid[";
        dumpString += std::to_string(pid);
        dumpString += "]";
        if (data) {
            dumpString += "\n";
            dumpString += data.value();
        }
    }
    nodeTreeDumpTasks_.erase(taskId);

    RS_LOGI("CollectClientNodeTreeResult task[%{public}u] completionCount[%{public}zu]",
        taskId, completed);
}

void RSMainThread::OnCommitDumpClientNodeTree(NodeId nodeId, pid_t pid, uint32_t taskId, const std::string& result)
{
    RS_TRACE_NAME_FMT("DumpClientNodeTree collected task[%u] dataSize[%zu] pid[%d]",
        taskId, result.size(), pid);
    {
        std::unique_lock<std::mutex> lock(nodeTreeDumpMutex_);
        auto iter = nodeTreeDumpTasks_.find(taskId);
        if (iter == nodeTreeDumpTasks_.end()) {
            RS_LOGW("OnDumpClientNodeTree task[%{public}u] not found for pid[%d]", taskId, pid);
            return;
        }

        iter->second.completionCount++;
        auto& data = iter->second.data[pid];
        if (data) {
            data->append("\n");
            data->append(result);
        } else {
            data = result;
        }
        nodeTreeDumpCondVar_.notify_all();
    }

    RS_LOGI("OnDumpClientNodeTree task[%{public}u] dataSize[%{public}zu] pid[%d]",
        taskId, result.size(), pid);
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
    auto it = effectiveTransactionDataIndexMap_.find(remotePid);
    if (it != effectiveTransactionDataIndexMap_.end()) {
        if (!it->second.second.empty()) {
            RS_LOGD("RSMainThread::ClearTransactionDataPidInfo process:%{public}d destroyed, skip commands", remotePid);
        }
        effectiveTransactionDataIndexMap_.erase(it);
    }
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
                isNeedResetClearMemoryTask_ = true;
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
    std::string& type, pid_t pid)
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
    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSMainThread::DumpMem screenManager is nullptr");
        return;
    }
    auto maxScreenInfo = screenManager->GetActualScreenMaxResolution();
    dumpString.append("ScreenResolution = " + std::to_string(maxScreenInfo.phyWidth) +
        "x" + std::to_string(maxScreenInfo.phyHeight) + "\n");
    dumpString.append(log.GetString());

    RSUniRenderThread::Instance().DumpVkImageInfo(dumpString);
    RSHardwareThread::Instance().DumpVkImageInfo(dumpString);
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
    RSUniRenderThread::Instance().PostSyncTask([&mems, &pids] {
        MemoryManager::CountMemory(pids,
            RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetDrGPUContext(), mems);
    });
#endif
}

void RSMainThread::AddTransactionDataPidInfo(pid_t remotePid)
{
    if (!isUniRender_) {
        return;
    }
    std::lock_guard<std::mutex> lock(transitionDataMutex_);
    auto it = effectiveTransactionDataIndexMap_.find(remotePid);
    if (it != effectiveTransactionDataIndexMap_.end()) {
        RS_LOGW("RSMainThread::AddTransactionDataPidInfo remotePid:%{public}d already exists", remotePid);
        it->second.first = 0;
    } else {
        effectiveTransactionDataIndexMap_.emplace(remotePid,
            std::make_pair(0, std::vector<std::unique_ptr<RSTransactionData>>()));;
    }
}

void RSMainThread::SetDirtyFlag(bool isDirty)
{
    isDirty_ = isDirty;
}

bool RSMainThread::GetDirtyFlag()
{
    return isDirty_;
}

void RSMainThread::SetScreenPowerOnChanged(bool val)
{
    screenPowerOnChanged_ = val;
}

bool RSMainThread::GetScreenPowerOnChanged() const
{
    return screenPowerOnChanged_;
}

void RSMainThread::SetAccessibilityConfigChanged()
{
    isAccessibilityConfigChanged_ = true;
}

bool RSMainThread::IsAccessibilityConfigChanged() const
{
    return isAccessibilityConfigChanged_;
}

bool RSMainThread::IsCurtainScreenUsingStatusChanged() const
{
    return isCurtainScreenUsingStatusChanged_;
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

void RSMainThread::CheckFastCompose(int64_t lastFlushedDesiredPresentTimeStamp)
{
    auto nowTime = SystemTime();
    uint64_t unsignedNowTime = static_cast<uint64_t>(nowTime);
    uint64_t unsignedLastFlushedDesiredPresentTimeStamp = static_cast<uint64_t>(lastFlushedDesiredPresentTimeStamp);
    int64_t vsyncPeriod = 0;
    VsyncError ret = VSYNC_ERROR_UNKOWN;
    if (receiver_) {
        ret = receiver_->GetVSyncPeriod(vsyncPeriod);
    }
    uint64_t unsignedVsyncPeriod = static_cast<uint64_t>(vsyncPeriod);
    if (ret != VSYNC_ERROR_OK || unsignedVsyncPeriod > REFRESH_PERIOD + PERIOD_MAX_OFFSET ||
    	unsignedVsyncPeriod < REFRESH_PERIOD - PERIOD_MAX_OFFSET || !context_) {
        RequestNextVSync();
        return;
    }
    uint64_t lastVsyncTime = 0;
    if (lastFastComposeTimeStamp_ > 0 && timestamp_ == lastFastComposeTimeStamp_) {
        lastVsyncTime = timestamp_ - lastFastComposeTimeStampDiff_;
    } else {
        lastVsyncTime = timestamp_;
        lastFastComposeTimeStampDiff_ = 0;
    }
    lastVsyncTime = unsignedNowTime - (unsignedNowTime - lastVsyncTime) % unsignedVsyncPeriod;
    RS_TRACE_NAME_FMT("RSMainThread::CheckFastCompose now = %" PRIu64 "" \
        ", lastVsyncTime = %" PRIu64 ", timestamp_ = %" PRIu64, unsignedNowTime, lastVsyncTime, timestamp_);
    // ignore animation scenario and mult-window scenario
    bool isNeedSingleFrameCompose = context_->GetAnimatingNodeList().empty() &&
        context_->GetNodeMap().GetVisibleLeashWindowCount() < MULTI_WINDOW_PERF_START_NUM;
    // 1/2 vsync period to support continunous fastcompose
    if (isNeedSingleFrameCompose && unsignedNowTime - timestamp_ > unsignedVsyncPeriod / 2 &&
        unsignedLastFlushedDesiredPresentTimeStamp > lastVsyncTime - unsignedVsyncPeriod &&
        unsignedLastFlushedDesiredPresentTimeStamp < lastVsyncTime &&
        unsignedNowTime - lastVsyncTime < REFRESH_PERIOD / 2) { // invoke when late less than 1/2 refresh period
        RS_TRACE_NAME("RSMainThread::CheckFastCompose success, start fastcompose");
        RS_LOGD("RSMainThread::CheckFastCompose fastcompose start"
            ", buffer late for %{public}" PRIu64, unsignedNowTime - lastVsyncTime);
        ForceRefreshForUni(true);
    } else {
        RequestNextVSync();
    }
}

void RSMainThread::ForceRefreshForUni(bool needDelay)
{
    RS_LOGI("RSMainThread::ForceRefreshForUni call");
    if (isUniRender_) {
#ifdef RS_ENABLE_GPU
        PostTask([=]() {
            const int64_t onVsyncStartTime = GetCurrentSystimeMs();
            const int64_t onVsyncStartTimeSteady = GetCurrentSteadyTimeMs();
            const float onVsyncStartTimeSteadyFloat = GetCurrentSteadyTimeMsFloat();
            RSJankStatsOnVsyncStart(onVsyncStartTime, onVsyncStartTimeSteady, onVsyncStartTimeSteadyFloat);
            MergeToEffectiveTransactionDataMap(cachedTransactionDataMap_);
            if (!RSSystemProperties::GetUnmarshParallelFlag()) {
                RSUnmarshalThread::Instance().PostTask(unmarshalBarrierTask_);
            }
            auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            RS_PROFILER_PATCH_TIME(now);
            timestamp_ = timestamp_ + (now - curTime_);
            dvsyncRsTimestamp_.store(timestamp_);
            int64_t vsyncPeriod = 0;
            VsyncError ret = VSYNC_ERROR_UNKOWN;
            if (receiver_) {
                ret = receiver_->GetVSyncPeriod(vsyncPeriod);
            }
            if (ret == VSYNC_ERROR_OK && vsyncPeriod > 0) {
                lastFastComposeTimeStampDiff_ = (now - curTime_ + lastFastComposeTimeStampDiff_) % vsyncPeriod;
                lastFastComposeTimeStamp_ = timestamp_;
                RS_TRACE_NAME_FMT("RSMainThread::ForceRefreshForUni record"
                    "Time diff: %" PRIu64, lastFastComposeTimeStampDiff_);
                // When fast compose by buffer, pipeline still need to delay to maintain smooth rendering
                isForceRefresh_ = !needDelay;
            } else {
                isForceRefresh_ = true;
            }
            curTime_ = now;
            // Not triggered by vsync, so we set frameCount to 0.
            SetFrameInfo(0, isForceRefresh_);
            RS_TRACE_NAME("RSMainThread::ForceRefreshForUni timestamp:" + std::to_string(timestamp_));
            mainLoop_();
            RSJankStatsOnVsyncEnd(onVsyncStartTime, onVsyncStartTimeSteady, onVsyncStartTimeSteadyFloat);
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
#endif
    } else {
        RequestNextVSync();
    }
}

void RSMainThread::PerfForBlurIfNeeded()
{
    auto socPerfParam = std::static_pointer_cast<SOCPerfParam>(
        GraphicFeatureParamManager::GetInstance().GetFeatureParam(FEATURE_CONFIGS[SOC_PERF]));
    if (socPerfParam != nullptr && !socPerfParam->IsBlurSOCPerfEnable()) {
        return;
    }
    handler_->RemoveTask(PERF_FOR_BLUR_IF_NEEDED_TASK_NAME);
    static uint64_t prePerfTimestamp = 0;
    static int preBlurCnt = 0;
    static int cnt = 0;

    auto task = [this]() {
        if (preBlurCnt == 0) {
            return;
        }
        auto now = std::chrono::steady_clock::now().time_since_epoch();
        auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
        RS_OPTIONAL_TRACE_NAME_FMT("PerfForBlurIfNeeded now[%ld] timestamp[%ld] preBlurCnt[%d]",
            std::chrono::steady_clock::now().time_since_epoch(), timestamp, preBlurCnt);
        if (static_cast<uint64_t>(timestamp) - prePerfTimestamp > PERF_PERIOD_BLUR_TIMEOUT && preBlurCnt != 0) {
            PerfRequest(BLUR_CNT_TO_BLUR_CODE.at(preBlurCnt), false);
            prePerfTimestamp = 0;
            preBlurCnt = 0;
        }
    };
    // delay 100ms
    handler_->PostTask(task, PERF_FOR_BLUR_IF_NEEDED_TASK_NAME, 100);
    int blurCnt = RSPropertiesPainter::GetAndResetBlurCnt();
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
        RS_OPTIONAL_TRACE_NAME_FMT("PerfForBlurIfNeeded Perf close, preBlurCnt[%d] blurCnt[%d]", preBlurCnt, blurCnt);
        PerfRequest(BLUR_CNT_TO_BLUR_CODE.at(preBlurCnt), false);
        preBlurCnt = blurCnt == 0 ? 0 : preBlurCnt;
    }
    if (blurCnt == 0) {
        return;
    }
    if (timestamp_ - prePerfTimestamp > PERF_PERIOD_BLUR || cntIsMatch) {
        RS_OPTIONAL_TRACE_NAME_FMT("PerfForBlurIfNeeded PerfRequest, preBlurCnt[%d] blurCnt[%d]", preBlurCnt, blurCnt);
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

void RSMainThread::RenderFrameStart(uint64_t timestamp)
{
    uint32_t unExecuteTaskNum = RSHardwareThread::Instance().GetunExecuteTaskNum();
    if (preUnExecuteTaskNum_ != unExecuteTaskNum) {
        preUnExecuteTaskNum_ = unExecuteTaskNum;
        std::unordered_map<std::string, std::string> payload = {};
        payload["bufferCount"] = std::to_string(preUnExecuteTaskNum_);
        RsFrameReport::GetInstance().ReportSchedEvent(FrameSchedEvent::RS_BUFFER_COUNT, payload);
    }
#ifdef RS_ENABLE_GPU
    int hardwareTid = RSHardwareThread::Instance().GetHardwareTid();
    if (preHardwareTid_ != hardwareTid) {
        preHardwareTid_ = hardwareTid;
        std::unordered_map<std::string, std::string> param = {};
        param["hardwareTid"] = std::to_string(preHardwareTid_);
        RsFrameReport::GetInstance().ReportSchedEvent(FrameSchedEvent::RS_HARDWARE_INFO, param);
    }
#endif
    std::unordered_map<std::string, std::string> paramter = {};
    paramter["vsyncTime"] = std::to_string(timestamp);
    RsFrameReport::GetInstance().ReportSchedEvent(FrameSchedEvent::RS_RENDER_START, paramter);
    RenderFrameTrace::GetInstance().RenderStartFrameTrace(RS_INTERVAL_NAME);
}

void RSMainThread::SetAppWindowNum(uint32_t num)
{
    appWindowNum_ = num;
}

bool RSMainThread::SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation)
{
    RS_OPTIONAL_TRACE_NAME_FMT("%s systemAnimatedScenes[%u] systemAnimatedScenes_[%u] threeFingerScenesListSize[%d] "
        "systemAnimatedScenesListSize_[%d] isRegularAnimation_[%d]", __func__, systemAnimatedScenes,
        systemAnimatedScenes_, threeFingerScenesList_.size(), systemAnimatedScenesList_.size(), isRegularAnimation);
    if (systemAnimatedScenes < SystemAnimatedScenes::ENTER_MISSION_CENTER ||
            systemAnimatedScenes > SystemAnimatedScenes::OTHERS) {
        RS_LOGD("RSMainThread::SetSystemAnimatedScenes Out of range.");
        return false;
    }
    systemAnimatedScenes_ = systemAnimatedScenes;
    isRegularAnimation_ = isRegularAnimation;
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
            if (systemAnimatedScenes != SystemAnimatedScenes::APPEAR_MISSION_CENTER &&
                systemAnimatedScenes != SystemAnimatedScenes::ENTER_RECENTS &&
                systemAnimatedScenes != SystemAnimatedScenes::EXIT_RECENTS) {
                // systemAnimatedScenesList_ is only for pc now
                systemAnimatedScenesList_.push_back(std::make_pair(systemAnimatedScenes, curTime));
            }
        }
    }
    return true;
}
bool RSMainThread::GetIsRegularAnimation() const
{
    return (isRegularAnimation_ &&
        systemAnimatedScenes_ < SystemAnimatedScenes::OTHERS &&
        RSSystemParameters::GetAnimationOcclusionEnabled()) || IsPCThreeFingerScenesListScene();
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

bool RSMainThread::IsDrawingGroupChanged(const RSRenderNode& cacheRootNode) const
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

void RSMainThread::ResetHardwareEnabledState(bool isUniRender)
{
    if (isUniRender) {
#ifdef RS_ENABLE_GPU
        isHardwareForcedDisabled_ = !RSSystemProperties::GetHardwareComposerEnabled();
        isLastFrameDirectComposition_ = doDirectComposition_;
        doDirectComposition_ = isHardwareForcedDisabled_ ? false : RSSystemProperties::GetDoDirectCompositionEnabled();
        isHardwareEnabledBufferUpdated_ = false;
        hasProtectedLayer_ = false;
        hardwareEnabledNodes_.clear();
        hardwareEnabledDrwawables_.clear();
        ClearSelfDrawingNodes();
        selfDrawables_.clear();
        RSPointerWindowManager::Instance().ResetHardCursorDrawables();
#endif
    }
}

bool RSMainThread::IsHardwareEnabledNodesNeedSync()
{
    bool needSync = false;
#ifdef RS_ENABLE_GPU
    for (const auto& node : hardwareEnabledNodes_) {
        if (node != nullptr && ((!doDirectComposition_ && node->GetStagingRenderParams() != nullptr &&
            node->GetStagingRenderParams()->NeedSync()) ||
            (doDirectComposition_ && !node->IsHardwareForcedDisabled()))) {
            needSync = true;
            break;
        }
    }
#endif
    RS_TRACE_NAME_FMT("%s %u", __func__, needSync);
    RS_LOGD("%{public}s %{public}u", __func__, needSync);

    return needSync;
}

bool RSMainThread::IsOcclusionNodesNeedSync(NodeId id, bool useCurWindow)
{
    auto nodePtr = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(
        GetContext().GetNodeMap().GetRenderNode(id));
    if (nodePtr == nullptr) {
        return false;
    }

    if (useCurWindow == false) {
        auto parentNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr->GetParent().lock());
        if (parentNode && parentNode->IsLeashWindow() && parentNode->ShouldPaint()) {
            nodePtr = parentNode;
        }
    }

    if (nodePtr->GetIsFullChildrenListValid() == false || !nodePtr->IsOnTheTree()) {
        nodePtr->PrepareSelfNodeForApplyModifiers();
        return true;
    }

    bool needSync = false;
    if (nodePtr->IsLeashWindow()) {
        auto children = nodePtr->GetSortedChildren();
        for (auto child : *children) {
            auto childSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
            if (childSurfaceNode && childSurfaceNode->IsMainWindowType() &&
                childSurfaceNode->GetVisibleRegion().IsEmpty()) {
                childSurfaceNode->PrepareSelfNodeForApplyModifiers();
                needSync = true;
            }
        }
    } else if (nodePtr->IsMainWindowType() && nodePtr->GetVisibleRegion().IsEmpty()) {
        nodePtr->PrepareSelfNodeForApplyModifiers();
        needSync = true;
    }

    return needSync;
}

void RSMainThread::SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark)
{
    std::lock_guard<std::mutex> lock(watermarkMutex_);
    surfaceNodeWatermarks_[name] = watermark;
}

void RSMainThread::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool flag)
{
    std::lock_guard<std::mutex> lock(watermarkMutex_);
    auto screenManager_ = CreateOrGetScreenManager();
    if (flag && screenManager_) {
        auto screenInfo = screenManager_->QueryDefaultScreenInfo();
        constexpr int32_t maxScale = 2;
        if (screenInfo.id != INVALID_SCREEN_ID && watermarkImg &&
            (watermarkImg->GetWidth() > maxScale * static_cast<int32_t>(screenInfo.width) ||
            watermarkImg->GetHeight() > maxScale * static_cast<int32_t>(screenInfo.height))) {
            RS_LOGE("RSMainThread::ShowWatermark width %{public}" PRId32" or height %{public}" PRId32" has reached"
                " the maximum limit!", watermarkImg->GetWidth(), watermarkImg->GetHeight());
            return;
        }
    }

    watermarkFlag_ = flag;
    if (flag) {
        watermarkImg_ = RSPixelMapUtil::ExtractDrawingImage(watermarkImg);
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

bool RSMainThread::HasMirrorDisplay() const
{
    bool hasWiredMirrorDisplay = false;
    bool hasVirtualMirrorDisplay = false;
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode == nullptr || rootNode->GetChildrenCount() <= 1) {
        hasWiredMirrorDisplay_.store(false);
        hasVirtualMirrorDisplay_.store(false);
        return false;
    }

    for (auto& child : *rootNode->GetSortedChildren()) {
        if (!child || !child->IsInstanceOf<RSDisplayRenderNode>()) {
            continue;
        }
        auto displayNode = child->ReinterpretCastTo<RSDisplayRenderNode>();
        if (!displayNode) {
            continue;
        }
        if (auto mirroredNode = displayNode->GetMirrorSource().lock()) {
            if (displayNode->GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE) {
                hasWiredMirrorDisplay = true;
            } else {
                hasVirtualMirrorDisplay = true;
            }
        }
    }
    hasWiredMirrorDisplay_.store(hasWiredMirrorDisplay);
    hasVirtualMirrorDisplay_.store(hasVirtualMirrorDisplay);
    return hasWiredMirrorDisplay || hasVirtualMirrorDisplay;
}

void RSMainThread::UpdateRogSizeIfNeeded()
{
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (!rootNode) {
        return;
    }
    auto child = rootNode->GetFirstChild();
    if (child != nullptr && child->IsInstanceOf<RSDisplayRenderNode>()) {
        auto displayNode = child->ReinterpretCastTo<RSDisplayRenderNode>();
        if (displayNode == nullptr) {
            return;
        }
        RSHardwareThread::Instance().PostTask([displayNode]() {
            auto screenManager = CreateOrGetScreenManager();
            if (screenManager == nullptr) {
                return;
            }
            screenManager->SetRogScreenResolution(
                displayNode->GetScreenId(), displayNode->GetRogWidth(), displayNode->GetRogHeight());
        });
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

const uint32_t FOLD_DEVICE_SCREEN_NUMBER = 2; // alt device has two screens

void RSMainThread::UpdateAnimateNodeFlag()
{
    if (!context_) {
        return;
    }
    context_->curFrameAnimatingNodeList_.insert(context_->animatingNodeList_.begin(),
        context_->animatingNodeList_.end());
    for (auto& item : context_->curFrameAnimatingNodeList_) {
        auto node = item.second.lock();
        if (node) {
            node->SetCurFrameHasAnimation(true);
        }
    }
}

void RSMainThread::ResetAnimateNodeFlag()
{
    if (!context_) {
        return;
    }
    for (auto& item : context_->curFrameAnimatingNodeList_) {
        auto node = item.second.lock();
        if (node) {
            node->SetCurFrameHasAnimation(false);
        }
    }
    context_->curFrameAnimatingNodeList_.clear();
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
#ifdef RS_ENABLE_GPU
    RSUniRenderThread::Instance().PostSyncTask([&cpuMemSize, &gpuMemSize] {
        gpuMemSize = MemoryManager::GetAppGpuMemoryInMB(
            RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
        cpuMemSize = MemoryTrack::Instance().GetAppMemorySizeInMB();
    });
#endif
}

void RSMainThread::SubscribeAppState()
{
    RSBackgroundThread::Instance().PostTask(
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
        });
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
#ifdef RS_ENABLE_GPU
                            RSUniRenderThread::Instance().ClearMemoryCache(ClearMemoryMoment::LOW_MEMORY, true);
                            isNeedResetClearMemoryTask_ = true;
#endif
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
    if (!AccessibilityParam::IsCurtainScreenEnabled()) {
        RS_LOGE("RSMainThread::SetCurtainScreenUsingStatus CurtainScreenEnabled is not supported");
        return;
    }

#ifdef RS_ENABLE_GPU
    if (isCurtainScreenOn_ == isCurtainScreenOn) {
        RS_LOGD("RSMainThread::SetCurtainScreenUsingStatus: curtain screen status not change");
        return;
    }
    isCurtainScreenOn_ = isCurtainScreenOn;
    isCurtainScreenUsingStatusChanged_ = true;
    SetDirtyFlag();
    RequestNextVSync();
    RS_LOGD("RSMainThread::SetCurtainScreenUsingStatus %{public}d", isCurtainScreenOn);
#endif
}

void RSMainThread::AddPidNeedDropFrame(std::vector<int32_t> pidList)
{
    for (const auto& pid: pidList) {
        surfacePidNeedDropFrame_.insert(pid);
    }
}

void RSMainThread::ClearNeedDropframePidList()
{
    surfacePidNeedDropFrame_.clear();
}

bool RSMainThread::IsNeedDropFrameByPid(NodeId nodeId)
{
    int32_t pid = ExtractPid(nodeId);
    return surfacePidNeedDropFrame_.find(pid) != surfacePidNeedDropFrame_.end();
}

void RSMainThread::SetLuminanceChangingStatus(ScreenId id, bool isLuminanceChanged)
{
    std::lock_guard<std::mutex> lock(luminanceMutex_);
    displayLuminanceChanged_[id] = isLuminanceChanged;
}

bool RSMainThread::ExchangeLuminanceChangingStatus(ScreenId id)
{
    std::lock_guard<std::mutex> lock(luminanceMutex_);
    bool ret = false;
    auto it = displayLuminanceChanged_.find(id);
    if (it != displayLuminanceChanged_.end()) {
        ret = it->second;
        it->second = false;
    }
    return ret;
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

float RSMainThread::GetCurrentSteadyTimeMsFloat() const
{
    auto curTime = std::chrono::steady_clock::now().time_since_epoch();
    int64_t curSteadyTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(curTime).count();
    float curSteadyTime = curSteadyTimeUs / MS_TO_US;
    return curSteadyTime;
}

void RSMainThread::UpdateLuminanceAndColorTemp()
{
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        return;
    }
    bool isNeedRefreshAll{false};
    if (auto screenManager = CreateOrGetScreenManager()) {
        auto& rsLuminance = RSLuminanceControl::Get();
        auto& rsColorTemperature = RSColorTemperature::Get();
        for (const auto& child : *rootNode->GetSortedChildren()) {
            auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(child);
            if (displayNode == nullptr) {
                continue;
            }
            auto screenId = displayNode->GetScreenId();
            if (rsLuminance.IsNeedUpdateLuminance(screenId)) {
                uint32_t newLevel = rsLuminance.GetNewHdrLuminance(screenId);
                screenManager->SetScreenBacklight(screenId, newLevel);
                rsLuminance.SetNowHdrLuminance(screenId, newLevel);
            }
            if (rsLuminance.IsDimmingOn(screenId)) {
                rsLuminance.DimmingIncrease(screenId);
                isNeedRefreshAll = true;
                SetLuminanceChangingStatus(screenId, true);
            }
            if (rsColorTemperature.IsDimmingOn(screenId)) {
                std::vector<float> matrix = rsColorTemperature.GetNewLinearCct(screenId);
                screenManager->SetScreenLinearMatrix(screenId, matrix);
                rsColorTemperature.DimmingIncrease(screenId);
                isNeedRefreshAll = true;
            }
        }
    }
    if (isNeedRefreshAll) {
        SetForceUpdateUniRenderFlag(true);
        SetDirtyFlag();
        RequestNextVSync();
    }
}

void RSMainThread::RegisterUIExtensionCallback(pid_t pid, uint64_t userId, sptr<RSIUIExtensionCallback> callback,
    bool unobscured)
{
    std::lock_guard<std::mutex> lock(uiExtensionMutex_);
    RS_LOGI("RSMainThread::RegisterUIExtensionCallback for User: %{public}" PRIu64 " PID: %{public}d.", userId, pid);
    if (!unobscured) {
        uiExtensionListenners_[pid] = std::pair<uint64_t, sptr<RSIUIExtensionCallback>>(userId, callback);
    } else {
        uiUnobscuredExtensionListenners_[pid] = std::pair<uint64_t, sptr<RSIUIExtensionCallback>>(userId, callback);
    }
}

void RSMainThread::UnRegisterUIExtensionCallback(pid_t pid)
{
    std::lock_guard<std::mutex> lock(uiExtensionMutex_);
    if (uiExtensionListenners_.erase(pid) != 0) {
        RS_LOGI("RSMainThread::UnRegisterUIExtensionCallback for PID: %{public}d.", pid);
    }
    if (uiUnobscuredExtensionListenners_.erase(pid) != 0) {
        RS_LOGI("RSMainThread::UnRegisterUIUnobscuredExtensionCallback for PID: %{public}d.", pid);
    }
}

void RSMainThread::SetAncoForceDoDirect(bool direct)
{
    RS_LOGI("RSMainThread::SetAncoForceDoDirect %{public}d.", direct);
    RSSurfaceRenderNode::SetAncoForceDoDirect(direct);
}

void RSMainThread::UIExtensionNodesTraverseAndCallback()
{
    std::lock_guard<std::mutex> lock(uiExtensionMutex_);
#ifdef RS_ENABLE_GPU
    RSUniRenderUtil::UIExtensionFindAndTraverseAncestor(context_->GetNodeMap(), uiExtensionCallbackData_);
    RSUniRenderUtil::UIExtensionFindAndTraverseAncestor(context_->GetNodeMap(), unobscureduiExtensionCallbackData_,
        true);
#endif
    if (CheckUIExtensionCallbackDataChanged()) {
        RS_OPTIONAL_TRACE_NAME_FMT("RSMainThread::UIExtensionNodesTraverseAndCallback data size: [%lu]",
            uiExtensionCallbackData_.size());
        for (const auto& item : uiExtensionListenners_) {
            auto userId = item.second.first;
            auto callback = item.second.second;
            if (callback) {
                callback->OnUIExtension(std::make_shared<RSUIExtensionData>(uiExtensionCallbackData_), userId);
            }
        }
        for (const auto& item : uiUnobscuredExtensionListenners_) {
            auto userId = item.second.first;
            auto callback = item.second.second;
            if (callback) {
                callback->OnUIExtension(std::make_shared<RSUIExtensionData>(unobscureduiExtensionCallbackData_),
                    userId);
            }
        }
    }
    lastFrameUIExtensionDataEmpty_ = uiExtensionCallbackData_.empty();
    uiExtensionCallbackData_.clear();
}

bool RSMainThread::CheckUIExtensionCallbackDataChanged() const
{
    // empty for two consecutive frames, callback can be skipped.
    if (uiExtensionCallbackData_.empty()) {
        return !lastFrameUIExtensionDataEmpty_;
    }
    // layout of host node was not changed, callback can be skipped.
    const auto& nodeMap = context_->GetNodeMap();
    for (const auto& data : uiExtensionCallbackData_) {
        auto hostNode = nodeMap.GetRenderNode(data.first);
        if (hostNode != nullptr && !hostNode->LastFrameSubTreeSkipped()) {
            return true;
        }
    }
    RS_OPTIONAL_TRACE_NAME("RSMainThread::CheckUIExtensionCallbackDataChanged, all host nodes were not changed.");
    return false;
}

void RSMainThread::SetHardwareTaskNum(uint32_t num)
{
    rsVSyncDistributor_->SetHardwareTaskNum(num);
}

uint64_t RSMainThread::GetRealTimeOffsetOfDvsync(int64_t time)
{
    return rsVSyncDistributor_->GetRealTimeOffsetOfDvsync(time);
}

void RSMainThread::SetFrameInfo(uint64_t frameCount, bool forceRefreshFlag)
{
    // use the same function as vsync to get current time
    int64_t currentTimestamp = SystemTime();
    auto &hgmCore = OHOS::Rosen::HgmCore::Instance();
    hgmCore.SetActualTimestamp(currentTimestamp);
    hgmCore.SetVsyncId(frameCount);

    auto &frameDeadline = OHOS::Rosen::RsFrameDeadlinePredict::GetInstance();
    frameDeadline.ReportRsFrameDeadline(hgmCore, forceRefreshFlag);
}

void RSMainThread::MultiDisplayChange(bool isMultiDisplay)
{
    if (isMultiDisplay == isMultiDisplayPre_) {
        isMultiDisplayChange_ = false;
        return;
    }
    isMultiDisplayChange_ = true;
    isMultiDisplayPre_ = isMultiDisplay;
}

void RSMainThread::NotifyPackageEvent(const std::vector<std::string>& packageList)
{
    rsVSyncDistributor_->NotifyPackageEvent(packageList);
}

void RSMainThread::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt)
{
    rsVSyncDistributor_->NotifyTouchEvent(touchStatus, touchCnt);
}

void RSMainThread::SetBufferInfo(std::string &name, int32_t bufferCount, int64_t lastFlushedTimeStamp)
{
    rsVSyncDistributor_->SetBufferInfo(name, bufferCount, lastFlushedTimeStamp);
}
} // namespace Rosen
} // namespace OHOS
