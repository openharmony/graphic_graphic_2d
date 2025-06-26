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
#include "rs_uni_render_thread.h"

#include <fstream>
#include <malloc.h>
#include <memory>
#include <parameters.h>

#include "common/rs_background_thread.h"
#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_property_drawable_utils.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "graphic_common_c.h"
#include "hgm_core.h"
#include "include/core/SkGraphics.h"
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrDirectContext.h"
#else
#include "include/gpu/GrDirectContext.h"
#endif
#include "memory/rs_memory_manager.h"
#include "mem_param.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "graphic_feature_param_manager.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/rs_jank_stats.h"
#include "platform/ohos/rs_node_stats.h"
#include "rs_trace.h"
#include "rs_uni_render_engine.h"
#include "rs_uni_render_util.h"
#include "static_factory.h"
#include "surface.h"
#include "sync_fence.h"
#include "system/rs_system_parameters.h"

#ifdef RES_SCHED_ENABLE
#include <iservice_registry.h>
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#endif

#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif

#include <sched.h>
#include "res_sched_client.h"
#include "res_type.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
constexpr const char* PURGE_SHADER_CACHE_AFTER_ANIMATE = "PurgeShaderCacheAfterAnimate";
constexpr const char* DEFAULT_CLEAR_GPU_CACHE = "DefaultClearGpuCache";
constexpr const char* RECLAIM_MEMORY = "ReclaimMemory";
constexpr const char* PURGE_CACHE_BETWEEN_FRAMES = "PurgeCacheBetweenFrames";
constexpr const char* SUPPRESS_GPUCACHE_BELOW_CERTAIN_RATIO = "SuppressGpuCacheBelowCertainRatio";
const std::string PERF_FOR_BLUR_IF_NEEDED_TASK_NAME = "PerfForBlurIfNeeded";
constexpr uint32_t TIME_OF_SIX_FRAMES = 6000;
constexpr uint32_t TIME_OF_EIGHT_FRAMES = 8000;
constexpr uint32_t TIME_OF_THE_FRAMES = 1000;
constexpr uint32_t TIME_OF_DEFAULT_CLEAR_GPU_CACHE = 5000;
constexpr uint32_t TIME_OF_RECLAIM_MEMORY = 25000;
constexpr uint32_t WAIT_FOR_RELEASED_BUFFER_TIMEOUT = 3000;
constexpr uint32_t RELEASE_IN_HARDWARE_THREAD_TASK_NUM = 4;
constexpr uint64_t PERF_PERIOD_BLUR = 480000000;
constexpr uint64_t PERF_PERIOD_BLUR_TIMEOUT = 80000000;
constexpr size_t ONE_MEGABYTE = 1000 * 1000;
constexpr uint32_t TIME_OF_PERFORM_DEFERRED_CLEAR_GPU_CACHE = 30;

#ifdef OHOS_PLATFORM
const std::string RECLAIM_FILE_STRING = "1"; // for HM
#endif

const std::map<int, int32_t> BLUR_CNT_TO_BLUR_CODE {
    { 1, 10021 },
    { 2, 10022 },
    { 3, 10023 },
};

void PerfRequest(int32_t perfRequestCode, bool onOffTag)
{
#ifdef SOC_PERF_ENABLE
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(perfRequestCode, onOffTag, "");
    RS_LOGD("RSUniRenderThread::soc perf info [%{public}d %{public}d]", perfRequestCode, onOffTag);
#endif
}
};

thread_local CaptureParam RSUniRenderThread::captureParam_ = {};

void RSUniRenderThread::SetCaptureParam(const CaptureParam& param)
{
    captureParam_ = param;
}

CaptureParam& RSUniRenderThread::GetCaptureParam()
{
    return captureParam_;
}

void RSUniRenderThread::ResetCaptureParam()
{
    captureParam_ = {};
}

bool RSUniRenderThread::IsInCaptureProcess()
{
    return captureParam_.isSnapshot_;
}

bool RSUniRenderThread::IsExpandScreenMode()
{
    return !captureParam_.isSnapshot_ && !captureParam_.isMirror_;
}

RSUniRenderThread& RSUniRenderThread::Instance()
{
    static RSUniRenderThread instance;
    return instance;
}

RSUniRenderThread::RSUniRenderThread()
    :postImageReleaseTaskFlag_(Rosen::RSSystemProperties::GetImageReleaseUsingPostTask())
{}

RSUniRenderThread::~RSUniRenderThread() noexcept {}

void RSUniRenderThread::InitGrContext()
{
    // uniRenderEngine must be inited on the same thread with requestFrame
    uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    if (!uniRenderEngine_) {
        RS_LOGE("uniRenderEngine_ is nullptr");
        return;
    }
    uniRenderEngine_->Init();
#ifdef RS_ENABLE_VK
    if (Drawing::SystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        Drawing::SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        uniRenderEngine_->GetSkContext()->RegisterPostFunc([](const std::function<void()>& task) {
            RSUniRenderThread::Instance().PostImageReleaseTask(task);
        });
    }
    if (Drawing::SystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        Drawing::SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        if (RSSystemProperties::IsFoldScreenFlag()) {
            vmaOptimizeFlag_ = true;
        }
    }
#endif
    auto renderContext = uniRenderEngine_->GetRenderContext();
    if (!renderContext) {
        return;
    }
    auto grContext = renderContext->GetDrGPUContext();
    if (!grContext) {
        return;
    }
    RSMainThread::Instance()->InitVulkanErrorCallback(grContext);
    MemoryManager::SetGpuCacheSuppressWindowSwitch(
        grContext, RSSystemProperties::GetGpuCacheSuppressWindowEnabled());
    MemoryManager::SetGpuMemoryAsyncReclaimerSwitch(
        grContext, RSSystemProperties::GetGpuMemoryAsyncReclaimerEnabled(), []() {
            const uint32_t RS_IPC_QOS_LEVEL = 7;
            std::unordered_map<std::string, std::string> mapPayload = { { "bundleName", "render_service" },
                { "pid", std::to_string(getpid()) }, { std::to_string(gettid()), std::to_string(RS_IPC_QOS_LEVEL) } };
            using namespace OHOS::ResourceSchedule;
            auto& schedClient = ResSchedClient::GetInstance();
            schedClient.ReportData(ResType::RES_TYPE_THREAD_QOS_CHANGE, 0, mapPayload);
        });
}

void RSUniRenderThread::Inittcache()
{
    if (RSSystemParameters::GetTcacheEnabled()) {
        // enable cache
        mallopt(M_SET_THREAD_CACHE, M_THREAD_CACHE_ENABLE);
    }
}

void RSUniRenderThread::Start()
{
    runner_ = AppExecFwk::EventRunner::Create("RSUniRenderThread");
    if (!runner_) {
        RS_LOGE("RSUniRenderThread Start runner null");
        return;
    }
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    runner_->Run();
    auto postTaskProxy = [](RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
        AppExecFwk::EventQueue::Priority priority) {
        RSUniRenderThread::Instance().PostTask(task, name, delayTime, priority);
    };
    RSRenderNodeGC::Instance().SetRenderTask(postTaskProxy);
    PostSyncTask([this] {
        RS_LOGE("RSUniRenderThread Started ...");
        Inittcache();
        InitGrContext();
        tid_ = gettid();
#ifdef RES_SCHED_ENABLE
        SubScribeSystemAbility();
#endif
    });

    auto taskDispatchFunc = [this](const RSTaskDispatcher::RSTask& task, bool isSyncTask = false) {
        if (isSyncTask) {
            PostSyncTask(task);
        } else {
            PostTask(task);
        }
    };
    RSTaskDispatcher::GetInstance().RegisterTaskDispatchFunc(tid_, taskDispatchFunc);

    if (!rootNodeDrawable_) {
        const std::shared_ptr<RSBaseRenderNode> rootNode =
            RSMainThread::Instance()->GetContext().GetGlobalRootRenderNode();
        if (!rootNode) {
            RS_LOGE("rootNode is nullptr");
            return;
        }
        auto ptr = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rootNode);
        rootNodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(ptr);
    }
}

std::shared_ptr<RSBaseRenderEngine> RSUniRenderThread::GetRenderEngine() const
{
    return uniRenderEngine_;
}

void RSUniRenderThread::PostTask(const std::function<void()>& task)
{
    if (!handler_) {
        return;
    }
    handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void RSUniRenderThread::PostRTTask(const std::function<void()>& task)
{
    auto tid = gettid();
    if (tid == tid_) {
        task();
    } else {
        PostTask(task);
    }
}

void RSUniRenderThread::PostImageReleaseTask(const std::function<void()>& task)
{
    imageReleaseCount_++;
    if (postImageReleaseTaskFlag_) {
        PostRTTask(task);
        return;
    }
    if (tid_ == gettid()) {
        task();
        return;
    }
    std::unique_lock<std::mutex> releaseLock(imageReleaseMutex_);
    imageReleaseTasks_.push_back(task);
}

void RSUniRenderThread::RunImageReleaseTask()
{
    if (postImageReleaseTaskFlag_) { // release using post task
        RS_TRACE_NAME_FMT("RunImageReleaseTask using PostTask: count %d", imageReleaseCount_);
        imageReleaseCount_ = 0;
        return;
    }
    std::vector<Callback> tasks;
    {
        std::unique_lock<std::mutex> releaseLock(imageReleaseMutex_);
        std::swap(imageReleaseTasks_, tasks);
    }
    if (tasks.empty()) {
        return;
    }
    RS_TRACE_NAME_FMT("RunImageReleaseTask: count %d", imageReleaseCount_);
    imageReleaseCount_ = 0;
    for (auto task : tasks) {
        task();
    }
}

void RSUniRenderThread::PostTask(RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
    AppExecFwk::EventQueue::Priority priority)
{
    if (handler_) {
        handler_->PostTask(task, name, delayTime, priority);
    }
}

void RSUniRenderThread::RemoveTask(const std::string& name)
{
    if (handler_) {
        handler_->RemoveTask(name);
    }
}

void RSUniRenderThread::PostSyncTask(const std::function<void()>& task)
{
    if (!handler_) {
        return;
    }
    handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

bool RSUniRenderThread::IsIdle() const
{
    return handler_ ? handler_->IsIdle() : false;
}

void RSUniRenderThread::Sync(std::unique_ptr<RSRenderThreadParams>&& stagingRenderThreadParams)
{
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(std::move(stagingRenderThreadParams));
}

void RSUniRenderThread::Render()
{
    if (!rootNodeDrawable_) {
        RS_LOGE("rootNodeDrawable is nullptr");
        return;
    }
    if (vmaOptimizeFlag_) { // render this frame with vma cache on/off
        std::lock_guard<std::mutex> lock(vmaCacheCountMutex_);
        if (vmaCacheCount_ > 0) {
            vmaCacheCount_--;
            Drawing::StaticFactory::SetVmaCacheStatus(true);
        } else {
            Drawing::StaticFactory::SetVmaCacheStatus(false);
        }
    }
    Drawing::Canvas canvas;
    RSNodeStats::GetInstance().ClearNodeStats();
    rootNodeDrawable_->OnDraw(canvas);
    RSNodeStats::GetInstance().ReportRSNodeLimitExceeded();
    PerfForBlurIfNeeded();
}

void RSUniRenderThread::CollectReleaseTasks(std::vector<std::function<void()>>& releaseTasks)
{
    auto& renderThreadParams = GetRSRenderThreadParams();
    if (!renderThreadParams) {
        return;
    }
    for (const auto& drawable : renderThreadParams->GetSelfDrawables()) {
        if (UNLIKELY(!drawable)) {
            continue;
        }
        auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
        auto& params = surfaceDrawable->GetRenderParams();
        if (UNLIKELY(!params)) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(params.get());
        if (UNLIKELY(!surfaceParams)) {
            continue;
        }
        auto curHardWareEnabled = surfaceParams->GetHardwareEnabled();
        auto lastHardWareEnabled = surfaceParams->GetLastFrameHardwareEnabled();
        bool needRelease = !curHardWareEnabled || !surfaceParams->GetLayerCreated();
        if (needRelease && lastHardWareEnabled) {
            surfaceParams->releaseInHardwareThreadTaskNum_ = RELEASE_IN_HARDWARE_THREAD_TASK_NUM;
        }
        if (curHardWareEnabled != lastHardWareEnabled && params->GetIsOnTheTree()) {
            RS_LOGD("name:%{public}s id:%{public}" PRIu64 " hwcEnabled changed to:%{public}d needRelease:%{public}d",
                surfaceDrawable->GetName().c_str(), surfaceDrawable->GetId(), curHardWareEnabled, needRelease);
        }
        if (needRelease) {
            auto preBuffer = params->GetPreBuffer();
            if (preBuffer == nullptr) {
                if (surfaceParams->releaseInHardwareThreadTaskNum_ > 0) {
                    surfaceParams->releaseInHardwareThreadTaskNum_--;
                }
                continue;
            }
            auto releaseTask = [buffer = preBuffer, consumer = surfaceDrawable->GetConsumerOnDraw(),
                                   useReleaseFence = lastHardWareEnabled,
                                   acquireFence = acquireFence_]() mutable {
                if (consumer == nullptr) {
                    RS_LOGE("ReleaseSelfDrawingNodeBuffer failed consumer nullptr");
                    return;
                }
                auto ret = consumer->ReleaseBuffer(buffer, useReleaseFence ?
                    RSHardwareThread::Instance().releaseFence_ : acquireFence);
                if (ret != OHOS::SURFACE_ERROR_OK) {
                    RS_LOGD("ReleaseSelfDrawingNodeBuffer failed ret:%{public}d", ret);
                }
            };
            params->SetPreBuffer(nullptr);
            if (surfaceParams->releaseInHardwareThreadTaskNum_ > 0) {
                releaseTasks.emplace_back(releaseTask);
                surfaceParams->releaseInHardwareThreadTaskNum_--;
            } else {
                releaseTask();
            }
        }
    }
}

void RSUniRenderThread::ReleaseSelfDrawingNodeBuffer()
{
    std::vector<std::function<void()>> releaseTasks;
    CollectReleaseTasks(releaseTasks);
    if (releaseTasks.empty()) {
        return;
    }
    auto releaseBufferTask = [releaseTasks]() {
        for (const auto& task : releaseTasks) {
            task();
        }
    };
    auto delayTime = RSHardwareThread::Instance().delayTime_;
    if (delayTime > 0) {
        RSHardwareThread::Instance().PostDelayTask(releaseBufferTask, delayTime);
    } else {
        RSHardwareThread::Instance().PostTask(releaseBufferTask);
    }
}

void RSUniRenderThread::ReleaseSurface()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (tmpSurfaces_.size() > 0) {
        auto tmp = tmpSurfaces_.front();
        tmpSurfaces_.pop();
        tmp = nullptr;
    }
}

void RSUniRenderThread::AddToReleaseQueue(std::shared_ptr<Drawing::Surface>&& surface)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tmpSurfaces_.push(std::move(surface));
}

uint64_t RSUniRenderThread::GetCurrentTimestamp() const
{
    auto& renderThreadParams = GetRSRenderThreadParams();
    return renderThreadParams ? renderThreadParams->GetCurrentTimestamp() : 0;
}

int64_t RSUniRenderThread::GetActualTimestamp() const
{
    auto& renderThreadParams = GetRSRenderThreadParams();
    return renderThreadParams ? renderThreadParams->GetActualTimestamp() : 0;
}

uint64_t RSUniRenderThread::GetVsyncId() const
{
    auto& renderThreadParams = GetRSRenderThreadParams();
    return renderThreadParams ? renderThreadParams->GetVsyncId() : 0;
}

bool RSUniRenderThread::GetForceRefreshFlag() const
{
    auto& renderThreadParams = GetRSRenderThreadParams();
    return renderThreadParams ? renderThreadParams->GetForceRefreshFlag() : false;
}

uint32_t RSUniRenderThread::GetPendingScreenRefreshRate() const
{
    auto& renderThreadParams = GetRSRenderThreadParams();
    return renderThreadParams ? renderThreadParams->GetPendingScreenRefreshRate() : 0;
}

uint64_t RSUniRenderThread::GetPendingConstraintRelativeTime() const
{
    auto& renderThreadParams = GetRSRenderThreadParams();
    return renderThreadParams ? renderThreadParams->GetPendingConstraintRelativeTime() : 0;
}

uint64_t RSUniRenderThread::GetFastComposeTimeStampDiff() const
{
    auto& renderThreadParams = GetRSRenderThreadParams();
    return renderThreadParams ? renderThreadParams->GetFastComposeTimeStampDiff() : 0;
}

#ifdef RES_SCHED_ENABLE
void RSUniRenderThread::SubScribeSystemAbility()
{
    RS_LOGD("%{public}s", __func__);
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        RS_LOGE("%{public}s failed to get system ability manager client", __func__);
        return;
    }
    std::string threadName = "RSHardwareThread";
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
#endif
bool RSUniRenderThread::WaitUntilDisplayNodeBufferReleased(
    DrawableV2::RSDisplayRenderNodeDrawable& displayNodeDrawable)
{
    std::unique_lock<std::mutex> lock(displayNodeBufferReleasedMutex_);
    displayNodeBufferReleased_ = false; // prevent spurious wakeup of condition variable
    if (!displayNodeDrawable.IsSurfaceCreated()) {
        return true;
    }
    auto consumer = displayNodeDrawable.GetRSSurfaceHandlerOnDraw()->GetConsumer();
    if (consumer && consumer->QueryIfBufferAvailable()) {
        return true;
    }
    return displayNodeBufferReleasedCond_.wait_until(lock, std::chrono::system_clock::now() +
        std::chrono::milliseconds(WAIT_FOR_RELEASED_BUFFER_TIMEOUT), [this]() { return displayNodeBufferReleased_; });
}

void RSUniRenderThread::NotifyDisplayNodeBufferReleased()
{
    RS_TRACE_NAME("RSUniRenderThread::NotifyDisplayNodeBufferReleased");
    std::lock_guard<std::mutex> lock(displayNodeBufferReleasedMutex_);
    displayNodeBufferReleased_ = true;
    displayNodeBufferReleasedCond_.notify_one();
}

void RSUniRenderThread::PerfForBlurIfNeeded()
{
    if (!SOCPerfParam::IsBlurSOCPerfEnable()) {
        return;
    }

    if (!handler_) {
        return;
    }
    handler_->RemoveTask(PERF_FOR_BLUR_IF_NEEDED_TASK_NAME);
    static uint64_t prePerfTimestamp = 0;
    static int preBlurCnt = 0;
    static int cnt = 0;
    auto params = GetRSRenderThreadParams().get();
    if (!params) {
        return;
    }
    auto threadTimestamp = params->GetCurrentTimestamp();

    auto task = [this]() {
        if (preBlurCnt == 0) {
            return;
        }
        auto now = std::chrono::steady_clock::now().time_since_epoch();
        auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
        RS_OPTIONAL_TRACE_NAME_FMT("PerfForBlurIfNeeded now[%ld] timestamp[%ld] preBlurCnt[%d]",
            now, timestamp, preBlurCnt);
        if (static_cast<uint64_t>(timestamp) - prePerfTimestamp > PERF_PERIOD_BLUR_TIMEOUT) {
            PerfRequest(BLUR_CNT_TO_BLUR_CODE.at(preBlurCnt), false);
            prePerfTimestamp = 0;
            preBlurCnt = 0;
        }
    };

    // delay 100ms
    handler_->PostTask(task, PERF_FOR_BLUR_IF_NEEDED_TASK_NAME, 100);
    int blurCnt = RSPropertyDrawableUtils::GetAndResetBlurCnt();
    // clamp blurCnt to 0~3.
    blurCnt = std::clamp<int>(blurCnt, 0, 3);
    cnt = (blurCnt < preBlurCnt) ? (cnt + 1) : 0;

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
    if (threadTimestamp - prePerfTimestamp > PERF_PERIOD_BLUR || cntIsMatch) {
        RS_OPTIONAL_TRACE_NAME_FMT("PerfForBlurIfNeeded PerfRequest, preBlurCnt[%d] blurCnt[%ld]", preBlurCnt, blurCnt);
        PerfRequest(BLUR_CNT_TO_BLUR_CODE.at(blurCnt), true);
        prePerfTimestamp = threadTimestamp;
        preBlurCnt = blurCnt;
    }
}

bool RSUniRenderThread::GetClearMemoryFinished() const
{
    std::lock_guard<std::mutex> lock(clearMemoryMutex_);
    return clearMemoryFinished_;
}

bool RSUniRenderThread::GetClearMemDeeply() const
{
    std::lock_guard<std::mutex> lock(clearMemoryMutex_);
    return clearMemDeeply_;
}

void RSUniRenderThread::SetClearMoment(ClearMemoryMoment moment)
{
    clearMoment_ = moment;
}

ClearMemoryMoment RSUniRenderThread::GetClearMoment() const
{
    std::lock_guard<std::mutex> lock(clearMemoryMutex_);
    return clearMoment_;
}

uint32_t RSUniRenderThread::GetRefreshRate() const
{
    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderThread::GetRefreshRate screenManager is nullptr");
        return 60; // The default refreshrate is 60
    }
    return HgmCore::Instance().GetScreenCurrentRefreshRate(screenManager->GetDefaultScreenId());
}

std::shared_ptr<Drawing::Image> RSUniRenderThread::GetWatermarkImg()
{
    auto& renderThreadParams = GetRSRenderThreadParams();
    return renderThreadParams ? renderThreadParams->GetWatermarkImg() : nullptr;
}

bool RSUniRenderThread::GetWatermarkFlag() const
{
    auto& renderThreadParams = GetRSRenderThreadParams();
    return renderThreadParams ? renderThreadParams->GetWatermarkFlag() : false;
}

bool RSUniRenderThread::IsCurtainScreenOn() const
{
    auto& renderThreadParams = GetRSRenderThreadParams();
    return renderThreadParams ? renderThreadParams->IsCurtainScreenOn() : false;
}

bool RSUniRenderThread::IsColorFilterModeOn() const
{
    if (!uniRenderEngine_) {
        return false;
    }
    ColorFilterMode colorFilterMode = uniRenderEngine_->GetColorFilterMode();
    if (colorFilterMode == ColorFilterMode::INVERT_COLOR_DISABLE_MODE ||
        colorFilterMode >= ColorFilterMode::DALTONIZATION_NORMAL_MODE) {
        return false;
    }
    return true;
}

bool RSUniRenderThread::IsHighContrastTextModeOn() const
{
    if (!uniRenderEngine_) {
        return false;
    }
    return uniRenderEngine_->IsHighContrastEnabled();
}

static std::string FormatNumber(size_t number)
{
    constexpr int FORMATE_NUM_STEP = 3;
    std::string strNumber = std::to_string(number);
    int n = static_cast<int>(strNumber.length());
    for (int i = n - FORMATE_NUM_STEP; i > 0; i -= FORMATE_NUM_STEP) {
        strNumber.insert(i, ",");
    }
    return strNumber;
}

static void TrimMemEmptyType(Drawing::GPUContext* gpuContext)
{
    gpuContext->Flush();
    SkGraphics::PurgeAllCaches();
    gpuContext->FreeGpuResources();
    gpuContext->PurgeUnlockedResources(true);
    std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
    rendercontext->CleanAllShaderCache();
    gpuContext->FlushAndSubmit(true);
}

static void TrimMemShaderType()
{
    std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
    rendercontext->CleanAllShaderCache();
}

static void TrimMemGpuLimitType(Drawing::GPUContext* gpuContext, std::string& dumpString,
    std::string& type, const std::string& typeGpuLimit)
{
    size_t cacheLimit = 0;
    int maxResources;
    gpuContext->GetResourceCacheLimits(&maxResources, &cacheLimit);

    constexpr int MAX_GPU_LIMIT_SIZE = 4000;
    std::string strM = type.substr(typeGpuLimit.length());
    size_t maxResourcesBytes = cacheLimit; // max 4G
    char* end = nullptr;
    errno = 0;
    long long sizeM = std::strtoll(strM.c_str(), &end, 10);
    if (end != nullptr && end != strM.c_str() && errno == 0 && *end == '\0' &&
        sizeM > 0 && sizeM <= MAX_GPU_LIMIT_SIZE) {
        maxResourcesBytes = static_cast<size_t>(sizeM) * ONE_MEGABYTE;
    }

    gpuContext->SetResourceCacheLimits(maxResources, maxResourcesBytes);
    dumpString.append("setgpulimit: " + FormatNumber(cacheLimit)
        + "==>" + FormatNumber(maxResourcesBytes) + "\n");
}

void RSUniRenderThread::TrimMem(std::string& dumpString, std::string& type)
{
    auto task = [this, &dumpString, &type] {
        std::string typeGpuLimit = "setgpulimit";
        std::string avcodecVideo = "avcodecVideo";
        if (!uniRenderEngine_) {
            return;
        }
        auto renderContext = uniRenderEngine_->GetRenderContext();
        if (!renderContext) {
            return;
        }
        auto gpuContext = renderContext->GetDrGPUContext();
        if (gpuContext == nullptr) {
            return;
        }
        if (type.empty()) {
            TrimMemEmptyType(gpuContext);
        } else if (type == "cpu") {
            gpuContext->Flush();
            SkGraphics::PurgeAllCaches();
            gpuContext->FlushAndSubmit(true);
        } else if (type == "gpu") {
            gpuContext->Flush();
            gpuContext->FreeGpuResources();
            gpuContext->FlushAndSubmit(true);
        } else if (type == "uihidden") {
            gpuContext->Flush();
            gpuContext->PurgeUnlockAndSafeCacheGpuResources();
            gpuContext->FlushAndSubmit(true);
        } else if (type == "unlock") {
            gpuContext->Flush();
            gpuContext->PurgeUnlockedResources(false);
            gpuContext->FlushAndSubmit(true);
        } else if (type == "shader") {
            TrimMemShaderType();
        } else if (type == "flushcache") {
            int ret = mallopt(M_FLUSH_THREAD_CACHE, 0);
            dumpString.append("flushcache " + std::to_string(ret) + "\n");
        } else if (type.substr(0, typeGpuLimit.length()) == typeGpuLimit) {
            TrimMemGpuLimitType(gpuContext, dumpString, type, typeGpuLimit);
        } else if (type.substr(0, avcodecVideo.length()) == avcodecVideo) {
            RSJankStats::GetInstance().AvcodecVideoDump(dumpString, type, avcodecVideo);
        } else {
            uint32_t pid = static_cast<uint32_t>(std::atoi(type.c_str()));
            Drawing::GPUResourceTag tag(pid, 0, 0, 0, "TrimMem");
            MemoryManager::ReleaseAllGpuResource(gpuContext, tag);
        }
        dumpString.append("trimMem: " + type + "\n");
    };
    PostSyncTask(task);
}

void RSUniRenderThread::DumpMem(DfxString& log)
{
    std::vector<std::pair<NodeId, std::string>> nodeTags;
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([&nodeTags](const std::shared_ptr<RSSurfaceRenderNode> node) {
        NodeId nodeId = node->GetId();
        std::string name = node->GetName() + " " + std::to_string(ExtractPid(nodeId)) + " " + std::to_string(nodeId);
        nodeTags.push_back({nodeId, name});
    });
    PostSyncTask([&log, &nodeTags, this]() {
        if (!uniRenderEngine_) {
            return;
        }
        auto renderContext = uniRenderEngine_->GetRenderContext();
        if (!renderContext) {
            return;
        }
        auto gpuContext = renderContext->GetDrGPUContext();
        MemoryManager::DumpDrawingGpuMemory(log, gpuContext, nodeTags);
    });
}

void RSUniRenderThread::ClearMemoryCache(ClearMemoryMoment moment, bool deeply, pid_t pid)
{
    if (!RSSystemProperties::GetReleaseResourceEnabled()) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(clearMemoryMutex_);
        clearMemDeeply_ = clearMemDeeply_ || deeply;
        SetClearMoment(moment);
        clearMemoryFinished_ = false;
        exitedPidSet_.emplace(pid);
    }
    PostClearMemoryTask(moment, deeply, false);
}

void RSUniRenderThread::DefaultClearMemoryCache()
{
    // To clean memory when no render in 5s
    if (!RSSystemProperties::GetReleaseResourceEnabled()) {
        return;
    }
    PostClearMemoryTask(ClearMemoryMoment::DEFAULT_CLEAN, false, true);
}

void RSUniRenderThread::PostClearMemoryTask(ClearMemoryMoment moment, bool deeply, bool isDefaultClean)
{
    auto task = [this, moment, deeply, isDefaultClean]() {
        if (!uniRenderEngine_) {
            return;
        }
        auto renderContext = uniRenderEngine_->GetRenderContext();
        if (!renderContext) {
            return;
        }
        auto grContext = renderContext->GetDrGPUContext();
        if (UNLIKELY(!grContext)) {
            return;
        }
        RS_LOGD("Clear memory cache %{public}d", moment);
        RS_TRACE_NAME_FMT("Clear memory cache, cause the moment [%d] happen", moment);
        std::lock_guard<std::mutex> lock(clearMemoryMutex_);
        SKResourceManager::Instance().ReleaseResource();
        grContext->Flush();
        SkGraphics::PurgeAllCaches(); // clear cpu cache
        auto pid = *(this->exitedPidSet_.begin());
        if (this->exitedPidSet_.size() == 1 && pid == -1) { // no exited app, just clear scratch resource
            if (deeply || MEMParam::IsDeeplyRelGpuResEnable()) {
                MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(grContext);
            } else {
                MemoryManager::ReleaseUnlockGpuResource(grContext);
            }
        } else {
            MemoryManager::ReleaseUnlockGpuResource(grContext, this->exitedPidSet_);
        }
        auto screenManager_ = CreateOrGetScreenManager();
        screenManager_->ClearFrameBufferIfNeed();
        grContext->FlushAndSubmit(true);
        if (this->vmaOptimizeFlag_) {
            MemoryManager::VmaDefragment(grContext);
        }
        if (!isDefaultClean) {
            this->clearMemoryFinished_ = true;
        } else {
            this->isDefaultCleanTaskFinished_ = true;
            {
                RS_TRACE_NAME_FMT("Purge unlocked resources when clear memory");
                grContext->PerformDeferredCleanup(std::chrono::seconds(TIME_OF_PERFORM_DEFERRED_CLEAR_GPU_CACHE));
            }
        }
        RSUifirstManager::Instance().TryReleaseTextureForIdleThread();
        this->exitedPidSet_.clear();
        this->clearMemDeeply_ = false;
        this->SetClearMoment(ClearMemoryMoment::NO_CLEAR);
    };
    if (!isDefaultClean) {
        auto rate = GetRefreshRate();
        if (rate == 0) {
            RS_LOGE("PostClearMemoryTask::GetRefreshRate == 0, change refreshRate to 60.");
            const int defaultRefreshRate = 60;
            rate = defaultRefreshRate;
        }
        PostTask(task, CLEAR_GPU_CACHE,
            (MEMParam::IsDeeplyRelGpuResEnable() ? TIME_OF_THE_FRAMES : TIME_OF_EIGHT_FRAMES) / rate);
    } else {
        PostTask(task, DEFAULT_CLEAR_GPU_CACHE, TIME_OF_DEFAULT_CLEAR_GPU_CACHE);
    }
}

void RSUniRenderThread::ReclaimMemory()
{
    if (!RSSystemProperties::GetReclaimMemoryEnabled() || !MEMParam::IsReclaimEnabled()) {
        return;
    }

    // Reclaim memory when no render in 25s.
    PostReclaimMemoryTask(ClearMemoryMoment::RECLAIM_CLEAN, true);
}

void RSUniRenderThread::PostReclaimMemoryTask(ClearMemoryMoment moment, bool isReclaim)
{
    auto task = [this, moment, isReclaim]() {
        if (!uniRenderEngine_) {
            return;
        }
        auto renderContext = uniRenderEngine_->GetRenderContext();
        if (!renderContext) {
            return;
        }
        auto grContext = renderContext->GetDrGPUContext();
        if (UNLIKELY(!grContext)) {
            return;
        }
        grContext->ReclaimResources();

        RS_LOGD("Clear memory cache %{public}d", moment);
        RS_TRACE_NAME_FMT("Reclaim Memory, cause the moment [%d] happen", moment);
        std::lock_guard<std::mutex> lock(clearMemoryMutex_);
        // Ensure user don't enable the parameter. When we have an interrupt mechanism, remove it.
        if (isReclaim && system::GetParameter("persist.ace.testmode.enabled", "0") == "1") {
#ifdef OHOS_PLATFORM
            RSBackgroundThread::Instance().PostTask([]() {
                RS_LOGI("RSUniRenderThread::PostReclaimMemoryTask Start Reclaim File");
                std::string reclaimPath = "/proc/" + std::to_string(getpid()) + "/reclaim";
                std::string reclaimContent = RECLAIM_FILE_STRING;
                std::ofstream outfile(reclaimPath);
                if (outfile.is_open()) {
                    outfile << reclaimContent;
                    outfile.close();
                } else {
                    RS_LOGE("RSUniRenderThread::PostReclaimMemoryTask reclaim cannot open file");
                }
            });
#endif
        }
        this->isTimeToReclaim_.store(false);
        this->SetClearMoment(ClearMemoryMoment::NO_CLEAR);
    };

    PostTask(task, RECLAIM_MEMORY, TIME_OF_RECLAIM_MEMORY);
}

void RSUniRenderThread::ResetClearMemoryTask(bool isDoDirectComposition)
{
    if (!GetClearMemoryFinished()) {
        RemoveTask(CLEAR_GPU_CACHE);
        if (!isDoDirectComposition) {
            ClearMemoryCache(clearMoment_, clearMemDeeply_);
        }
    }
    if (!isDefaultCleanTaskFinished_) {
        RemoveTask(DEFAULT_CLEAR_GPU_CACHE);
        if (!isDoDirectComposition) {
            DefaultClearMemoryCache();
        }
    }
    if (isTimeToReclaim_.load()) {
        RemoveTask(RECLAIM_MEMORY);
        if (RSReclaimMemoryManager::Instance().IsReclaimInterrupt()) {
            isTimeToReclaim_.store(false);
            RSReclaimMemoryManager::Instance().SetReclaimInterrupt(false);
        } else if (!isDoDirectComposition) {
            ReclaimMemory();
        }
    }
}

void RSUniRenderThread::SetTimeToReclaim(bool isTimeToReclaim)
{
    isTimeToReclaim_.store(isTimeToReclaim);
}

bool RSUniRenderThread::IsTimeToReclaim()
{
    return isTimeToReclaim_.load();
}

void RSUniRenderThread::SetDefaultClearMemoryFinished(bool isFinished)
{
    isDefaultCleanTaskFinished_ = isFinished;
}

bool RSUniRenderThread::IsDefaultClearMemroyFinished()
{
    return isDefaultCleanTaskFinished_;
}

void RSUniRenderThread::PurgeCacheBetweenFrames()
{
    if (!RSSystemProperties::GetReleaseResourceEnabled()) {
        return;
    }
    RS_TRACE_NAME_FMT("MEM PurgeCacheBetweenFrames add task");
    PostTask(
        [this]() {
            if (!uniRenderEngine_) {
                return;
            }
            auto renderContext = uniRenderEngine_->GetRenderContext();
            if (!renderContext) {
                return;
            }
            auto grContext = renderContext->GetDrGPUContext();
            if (!grContext) {
                return;
            }
            RS_TRACE_NAME_FMT("PurgeCacheBetweenFrames");
            std::set<int> protectedPidSet = { RSMainThread::Instance()->GetDesktopPidForRotationScene() };
            MemoryManager::PurgeCacheBetweenFrames(grContext, true, this->exitedPidSet_, protectedPidSet);
            RemoveTask(PURGE_CACHE_BETWEEN_FRAMES);
        },
        PURGE_CACHE_BETWEEN_FRAMES, 0, AppExecFwk::EventQueue::Priority::LOW);
}

void RSUniRenderThread::FlushGpuMemoryInWaitQueueBetweenFrames()
{
    if (!uniRenderEngine_) {
        return;
    }
    auto renderContext = uniRenderEngine_->GetRenderContext();
    if (!renderContext) {
        return;
    }
    auto grContext = renderContext->GetDrGPUContext();
    if (!grContext) {
        return;
    }
    MemoryManager::FlushGpuMemoryInWaitQueue(grContext);
}

void RSUniRenderThread::SuppressGpuCacheBelowCertainRatioBetweenFrames()
{
    RemoveTask(SUPPRESS_GPUCACHE_BELOW_CERTAIN_RATIO);
    PostTask(
        [this]() {
            if (!uniRenderEngine_) {
                return;
            }
            auto renderContext = uniRenderEngine_->GetRenderContext();
            if (!renderContext) {
                return;
            }
            auto grContext = renderContext->GetDrGPUContext();
            if (!grContext) {
                return;
            }
            RS_TRACE_NAME_FMT("SuppressGpuCacheBelowCertainRatio");
            MemoryManager::SuppressGpuCacheBelowCertainRatio(grContext, [this]() -> bool {
               return this->handler_->HasPreferEvent(static_cast<int>(AppExecFwk::EventQueue::Priority::HIGH));
            });
        },
        SUPPRESS_GPUCACHE_BELOW_CERTAIN_RATIO, 0, AppExecFwk::EventQueue::Priority::LOW);
}

void RSUniRenderThread::MemoryManagementBetweenFrames()
{
    if (RSSystemProperties::GetGpuMemoryAsyncReclaimerEnabled()) {
        FlushGpuMemoryInWaitQueueBetweenFrames();
    }
    if (RSSystemProperties::GetGpuCacheSuppressWindowEnabled()) {
        SuppressGpuCacheBelowCertainRatioBetweenFrames();
    }
}

void RSUniRenderThread::PurgeShaderCacheAfterAnimate()
{
#ifdef RS_ENABLE_VK
    if (hasPurgeShaderCacheTask_) {
        RemoveTask(PURGE_SHADER_CACHE_AFTER_ANIMATE); // ensure only one task
        hasPurgeShaderCacheTask_ = false;
    }
    if (!RSJankStats::GetInstance().IsAnimationEmpty()) {
        return;
    }
    if (!uniRenderEngine_) {
        return;
    }
    auto renderContext = uniRenderEngine_->GetRenderContext();
    if (!renderContext) {
        return;
    }
    if (renderContext->CheckShaderCacheOverSoftLimit()) {
        RS_TRACE_NAME("ShaderCache OverSize, Posting Purge Task");
        hasPurgeShaderCacheTask_ = true;
        PostTask(
            [this]() {
                auto& shaderCache = ShaderCache::Instance();
                if (!shaderCache.IfInitialized()) {
                    RS_LOGD("PurgeShaderCacheAfterAnimate shaderCache not Initialized");
                    return;
                }
                RS_TRACE_NAME("PurgeShaderCacheAfterAnimate");
                shaderCache.PurgeShaderCacheAfterAnimate([this]() -> bool {
                return this->handler_->HasPreferEvent(static_cast<int>(AppExecFwk::EventQueue::Priority::HIGH));
                });
                hasPurgeShaderCacheTask_ = false;
            },
            PURGE_SHADER_CACHE_AFTER_ANIMATE,
            (MEMParam::IsDeeplyRelGpuResEnable() ? TIME_OF_THE_FRAMES : TIME_OF_SIX_FRAMES) / GetRefreshRate(),
            AppExecFwk::EventQueue::Priority::LOW);
    }
#else
    return;
#endif
}

void RSUniRenderThread::RenderServiceTreeDump(std::string& dumpString)
{
    PostSyncTask([this, &dumpString]() {
        if (!rootNodeDrawable_) {
            dumpString.append("rootNode is null\n");
            return;
        }
        rootNodeDrawable_->DumpDrawableTree(0, dumpString, RSMainThread::Instance()->GetContext());
    });
}

void RSUniRenderThread::UpdateDisplayNodeScreenId()
{
    const std::shared_ptr<RSBaseRenderNode> rootNode =
        RSMainThread::Instance()->GetContext().GetGlobalRootRenderNode();
    if (!rootNode) {
        RS_LOGE("RSUniRenderThread::UpdateDisplayNodeScreenId rootNode is nullptr");
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

uint32_t RSUniRenderThread::GetDynamicRefreshRate() const
{
    uint32_t refreshRate = OHOS::Rosen::HgmCore::Instance().GetScreenCurrentRefreshRate(displayNodeScreenId_);
    if (refreshRate == 0) {
        RS_LOGE("RSUniRenderThread::GetDynamicRefreshRate refreshRate is invalid");
        return STANDARD_REFRESH_RATE;
    }
    return refreshRate;
}

void RSUniRenderThread::SetAcquireFence(sptr<SyncFence> acquireFence)
{
    acquireFence_ = acquireFence;
}

void RSUniRenderThread::SetVmaCacheStatus(bool flag)
{
    static constexpr int MAX_VMA_CACHE_COUNT = 600;
    RS_LOGD("RSUniRenderThread::SetVmaCacheStatus(): %d, %d", vmaOptimizeFlag_, flag);
    if (!vmaOptimizeFlag_) {
        return;
    }
    std::lock_guard<std::mutex> lock(vmaCacheCountMutex_);
    vmaCacheCount_ = flag ? MAX_VMA_CACHE_COUNT : 0;
}

void RSUniRenderThread::DumpVkImageInfo(std::string &dumpString)
{
    std::weak_ptr<RSBaseRenderEngine> uniRenderEngine = uniRenderEngine_;
    PostSyncTask([&dumpString, uniRenderEngine]() {
        if (auto engine = uniRenderEngine.lock()) {
            engine->DumpVkImageInfo(dumpString);
        }
    });
}
} // namespace Rosen
} // namespace OHOS
