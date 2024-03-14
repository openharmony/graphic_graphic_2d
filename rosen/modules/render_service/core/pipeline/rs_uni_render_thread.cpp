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

#include "rs_trace.h"
#include "hgm_core.h"

#include "common/rs_common_def.h"
#include "include/core/SkGraphics.h"
#include "memory/rs_memory_manager.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
constexpr uint32_t TIME_OF_EIGHT_FRAMES = 8000;
constexpr uint32_t TIME_OF_THE_FRAMES = 1000;
};

RSUniRenderThread& RSUniRenderThread::Instance()
{
    static RSUniRenderThread instance;
    return instance;
}

RSUniRenderThread::RSUniRenderThread()
{

}

RSUniRenderThread::~RSUniRenderThread() noexcept {}

void RSUniRenderThread::InitGrContext()
{
    // uniRenderEngine must be inited on the same thread with requestFrame
    uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    if (!uniRenderEngine_) {
        RS_LOGE("uniRenderEngine_ is nullptr");
    }
    uniRenderEngine_->Init();
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
        InitGrContext();
    });

    if (!rootNodeDrawable_) {
        const std::shared_ptr<RSBaseRenderNode> rootNode =
            RSMainThread::Instance()->GetContext().GetGlobalRootRenderNode();
        if (!rootNode) {
            RS_LOGE("rootNode is nullptr");
            return;
        }
        rootNodeDrawable_ = std::make_unique<RSRenderNodeDrawable>(rootNode);
    }
}

const std::shared_ptr<RSBaseRenderEngine> RSUniRenderThread::GetRenderEngine() const
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

void RSUniRenderThread::PostTask(RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
    AppExecFwk::EventQueue::Priority priority)
{
    if (handler_) {
        handler_->PostTask(task, name, delayTime, priority);
    }
}

void RSUniRenderThread::PostSyncTask(const std::function<void()>& task)
{
    if (!handler_) {
        return;
    }
    handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
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
    rootNodeDrawable_->OnDraw(canvas);
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
    if (UNLIKELY(!context_)) {
        return;
    }
    context_->SetClearMoment(moment);
}

ClearMemoryMoment RSUniRenderThread::GetClearMoment() const
{
    if (UNLIKELY(!context_)) {
        return ClearMemoryMoment::NO_CLEAR;
    }
    return context_->GetClearMoment();
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

void RSUniRenderThread::ClearMemoryCache(ClearMemoryMoment moment, bool deeply)
{
    if (!RSSystemProperties::GetReleaseResourceEnabled()) {
        return;
    }     
    this->clearMemoryFinished_ = false;
    this->clearMemDeeply_ = this->clearMemDeeply_ || deeply;
    this->SetClearMoment(moment);
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
            if (deeply || this->deviceType_ != DeviceType::PHONE) {
                MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(grContext);
            } else {
                MemoryManager::ReleaseUnlockGpuResource(grContext);
            }
            grContext->FlushAndSubmit(true);
            this->clearMemoryFinished_ = true;
            this->clearMemDeeply_ = false;
            this->SetClearMoment(ClearMemoryMoment::NO_CLEAR);
        };
    PostTask(task, CLEAR_GPU_CACHE,
        (this->deviceType_ == DeviceType::PHONE ? TIME_OF_EIGHT_FRAMES : TIME_OF_THE_FRAMES)
                / GetRefreshRate());
}

} // namespace Rosen
} // namespace OHOS
