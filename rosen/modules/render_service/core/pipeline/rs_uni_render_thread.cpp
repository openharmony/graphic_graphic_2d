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
#include "pipeline/rs_uni_render_thread.h"
#include <memory>

#include <malloc.h>
#include "graphic_common_c.h"
#include "rs_trace.h"
#include "hgm_core.h"

#include "common/rs_common_def.h"
#include "include/core/SkGraphics.h"
#include "surface.h"
#include "sync_fence.h"
#include "memory/rs_memory_manager.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_hardware_thread.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/rs_jank_stats.h"
#include "platform/ohos/rs_node_stats.h"
#ifdef RES_SCHED_ENABLE
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include <iservice_registry.h>
#endif
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "common/rs_singleton.h"
#include "pipeline/round_corner_display/rs_round_corner_display.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
constexpr uint32_t TIME_OF_EIGHT_FRAMES = 8000;
constexpr uint32_t TIME_OF_THE_FRAMES = 1000;
constexpr uint32_t WAIT_FOR_RELEASED_BUFFER_TIMEOUT = 3000;
constexpr uint32_t RELEASE_IN_HARDWARE_THREAD_TASK_NUM = 4;
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
    }
    uniRenderEngine_->Init();
#ifdef RS_ENABLE_VK
    if (Drawing::SystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        Drawing::SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        uniRenderEngine_->GetSkContext()->RegisterPostFunc([](const std::function<void()>& task) {
            RSUniRenderThread::Instance().PostImageReleaseTask(task);
        });
    }
#endif
}

void RSUniRenderThread::Inittcache()
{
    // enable cache
    mallopt(M_SET_THREAD_CACHE, M_THREAD_CACHE_ENABLE);
}

void RSUniRenderThread::Start()
{
    runner_ = AppExecFwk::EventRunner::Create("RSUniRenderThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    if (!runner_) {
        RS_LOGE("RSUniRenderThread Start runner null");
    }
    runner_->Run();
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

void RSUniRenderThread::Sync(std::unique_ptr<RSRenderThreadParams>& stagingRenderThreadParams)
{
    renderThreadParams_ = std::move(stagingRenderThreadParams);
}

void RSUniRenderThread::Render()
{
    if (!rootNodeDrawable_) {
        RS_LOGE("rootNodeDrawable is nullptr");
    }
    // TO-DO replace Canvas* with Canvas&
    Drawing::Canvas canvas;
    RSNodeStats::GetInstance().ClearNodeStats();
    rootNodeDrawable_->OnDraw(canvas);
    RSNodeStats::GetInstance().ReportRSNodeLimitExceeded();
    RSMainThread::Instance()->PerfForBlurIfNeeded();

    if (RSMainThread::Instance()->GetMarkRenderFlag() == false) {
        RSMainThread::Instance()->SetFrameIsRender(true);
        DvsyncRequestNextVsync();
    }
    RSMainThread::Instance()->ResetMarkRenderFlag();
}

void RSUniRenderThread::ReleaseSelfDrawingNodeBuffer()
{
    std::vector<std::function<void()>> releaseTasks;
    for (const auto& surfaceNode : renderThreadParams_->GetSelfDrawingNodes()) {
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
        if (!params->GetHardwareEnabled() && params->GetLastFrameHardwareEnabled()) {
            params->releaseInHardwareThreadTaskNum_ = RELEASE_IN_HARDWARE_THREAD_TASK_NUM;
        }
        if (!params->GetHardwareEnabled()) {
            auto& preBuffer = params->GetPreBuffer();
            if (preBuffer == nullptr) {
                if (params->releaseInHardwareThreadTaskNum_ > 0) {
                    params->releaseInHardwareThreadTaskNum_--;
                }
                continue;
            }
            auto releaseTask = [buffer = preBuffer, consumer = surfaceNode->GetConsumer(),
                useReleaseFence = params->GetLastFrameHardwareEnabled(), acquireFence = acquireFence_]() mutable {
                auto ret = consumer->ReleaseBuffer(buffer, useReleaseFence ?
                    RSHardwareThread::Instance().releaseFence_ : acquireFence);
                if (ret != OHOS::SURFACE_ERROR_OK) {
                    RS_LOGD("ReleaseSelfDrawingNodeBuffer failed ret:%{public}d", ret);
                }
            };
            preBuffer = nullptr;
            if (params->releaseInHardwareThreadTaskNum_ > 0) {
                releaseTasks.emplace_back(releaseTask);
                params->releaseInHardwareThreadTaskNum_--;
            } else {
                releaseTask();
            }
        }
    }
    if (releaseTasks.empty()) {
        return;
    }
    auto releaseBufferTask = [releaseTasks]() {
        for (const auto& task : releaseTasks) {
            task();
        }
    };
    RSHardwareThread::Instance().PostTask(releaseBufferTask);
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
    return renderThreadParams_->GetCurrentTimestamp();
}

uint32_t RSUniRenderThread::GetPendingScreenRefreshRate() const
{
    return renderThreadParams_->GetPendingScreenRefreshRate();
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
bool RSUniRenderThread::WaitUntilDisplayNodeBufferReleased(std::shared_ptr<RSSurfaceHandler> surfaceHandler)
{
    std::unique_lock<std::mutex> lock(displayNodeBufferReleasedMutex_);
    displayNodeBufferReleased_ = false; // prevent spurious wakeup of condition variable
    if (surfaceHandler->GetConsumer()->QueryIfBufferAvailable()) {
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


bool RSUniRenderThread::GetClearMemoryFinished() const
{
    return clearMemoryFinished_;
}

bool RSUniRenderThread::GetClearMemDeeply() const
{
    return clearMemDeeply_;
}

void RSUniRenderThread::SetClearMoment(ClearMemoryMoment moment)
{
    clearMoment_ = moment;
}

ClearMemoryMoment RSUniRenderThread::GetClearMoment() const
{
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
    return renderThreadParams->GetWatermarkImg();
}

bool RSUniRenderThread::GetWatermarkFlag()
{
    auto& renderThreadParams = GetRSRenderThreadParams();
    return renderThreadParams->GetWatermarkFlag();
}

void RSUniRenderThread::TrimMem(std::string& dumpString, std::string& type)
{
    auto task = [this, &dumpString, &type] {
        auto gpuContext = GetRenderEngine()->GetRenderContext()->GetDrGPUContext();
        if (type.empty()) {
            gpuContext->Flush();
            SkGraphics::PurgeAllCaches();
            gpuContext->FreeGpuResources();
            gpuContext->PurgeUnlockedResources(true);
#ifdef NEW_RENDER_CONTEXT
            MemoryHandler::ClearShader();
#else
            std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
            rendercontext->CleanAllShaderCache();
#endif
            gpuContext->FlushAndSubmit(true);
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
            Drawing::GPUResourceTag tag(pid, 0, 0, 0);
            MemoryManager::ReleaseAllGpuResource(gpuContext, tag);
        }
        dumpString.append("trimMem: " + type + "\n");
    };
    PostSyncTask(task);
}

void RSUniRenderThread::DumpMem(DfxString& log)
{
    PostSyncTask([&log, this]() {
        MemoryManager::DumpDrawingGpuMemory(log, GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
    });
}

void RSUniRenderThread::ClearMemoryCache(ClearMemoryMoment moment, bool deeply, pid_t pid)
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
            if (UNLIKELY(!grContext)) {
                return;
            }
            RS_LOGD("Clear memory cache %{public}d", this->GetClearMoment());
            RS_TRACE_NAME_FMT("Clear memory cache, cause the moment [%d] happen", this->GetClearMoment());
            SKResourceManager::Instance().ReleaseResource();
            grContext->Flush();
            SkGraphics::PurgeAllCaches(); // clear cpu cache
            auto pid = *(this->exitedPidSet_.begin());
            if (this->exitedPidSet_.size() == 1 && pid == -1) {         // no exited app, just clear scratch resource
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
    PostTask(task, CLEAR_GPU_CACHE,
        (this->deviceType_ == DeviceType::PHONE ? TIME_OF_EIGHT_FRAMES : TIME_OF_THE_FRAMES)
                / GetRefreshRate());
}

void RSUniRenderThread::RenderServiceTreeDump(std::string& dumpString) const
{
    if (!rootNodeDrawable_) {
        dumpString.append("rootNode is null\n");
        return;
    }
    rootNodeDrawable_->DumpDrawableTree(0, dumpString);
}

void RSUniRenderThread::DvsyncRequestNextVsync()
{
    if ((renderThreadParams_ && renderThreadParams_->GetRequestNextVsyncFlag()) ||
        RSMainThread::Instance()->rsVSyncDistributor_->HasPendingUIRNV()) {
        RSMainThread::Instance()->rsVSyncDistributor_->MarkRSAnimate();
        RSMainThread::Instance()->RequestNextVSync("animate", renderThreadParams_->GetCurrentTimestamp());
    } else {
        RSMainThread::Instance()->rsVSyncDistributor_->UnmarkRSAnimate();
    }
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
            auto params = static_cast<RSDisplayRenderParams*>(displayNode->GetRenderParams().get());
            if (!params) {
                RS_LOGE("RSUniRenderThread::UpdateDisplayNodeScreenId params is nullptr");
                return;
            }
            displayNodeScreenId_ = params->GetScreenId();
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
} // namespace Rosen
} // namespace OHOS
