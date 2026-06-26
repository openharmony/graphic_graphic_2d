/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "modifier_render_thread/rs_canvas_modifiers_draw_thread.h"

#include "concurrent_task_client.h"
#include "qos.h"

#include "platform/common/rs_log.h"
#include "render_context/shader_cache.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* CLEAN_FREE_BUFFERS_TASK_NAME = "CleanFreeBuffersTask";
constexpr int64_t CLEAN_FREE_BUFFERS_DURATION = 2000;
constexpr int64_t CLEAN_FREE_BUFFERS_IMMEDIATELY_DELAY = 50;
}

RSCanvasModifiersDrawThread::RSCanvasModifiersDrawThread()
{
    canvasModifiersDraw_ = std::make_shared<RSCanvasModifiersDraw>();
}

RSCanvasModifiersDrawThread::~RSCanvasModifiersDrawThread()
{
    if (!IsStarted()) {
        return;
    }
    std::lock_guard<std::mutex> lock(startMutex_);
    ClearResource();
    started_ = false;
}

void RSCanvasModifiersDrawThread::Destroy()
{
    if (!IsStarted()) {
        return;
    }
    RemoveTask(CLEAN_FREE_BUFFERS_TASK_NAME);
    auto task = [this]() {
        ClearResource();
        started_ = false;
    };
    PostSyncTask(task);
}

void RSCanvasModifiersDrawThread::ClearResource()
{
    if (handler_ != nullptr) {
        handler_->RemoveAllEvents();
        handler_ = nullptr;
    }
    if (runner_ != nullptr) {
        runner_->Stop();
        runner_ = nullptr;
    }
}

void RSCanvasModifiersDrawThread::SetCacheDir(const std::string& path)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = canvasModifiersDraw_;
    PostTask([weakCanvasModifiersDraw, cacheDir = path] {
        if (auto canvasModifiersDraw = weakCanvasModifiersDraw.lock()) {
            canvasModifiersDraw->SetCacheDir(cacheDir);
        }
    });
}

bool RSCanvasModifiersDrawThread::IsStarted()
{
    std::lock_guard<std::mutex> lock(startMutex_);
    return started_;
}

void RSCanvasModifiersDrawThread::Start()
{
    {
        std::lock_guard<std::mutex> lock(startMutex_);
        if (started_) {
            return;
        }
        runner_ = AppExecFwk::EventRunner::Create("CanvasModifiersDraw");
        handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
        runner_->Run();
        started_ = true;
    }
    PostTask([] {
        OHOS::ConcurrentTask::IntervalReply reply;
        reply.tid = gettid();
        OHOS::ConcurrentTask::ConcurrentTaskClient::GetInstance().QueryInterval(
            OHOS::ConcurrentTask::QUERY_MODIFIER_DRAW, reply);
        SetThreadQos(QOS::QosLevel::QOS_USER_INTERACTIVE);
        // Init shader cache, shader save delay time differs between uni-render and hybrid-render.
        std::string vkVersion = std::to_string(VK_API_VERSION_1_2);
        auto size = vkVersion.size();
        auto& cache = ShaderCache::Instance();
        cache.InitShaderCache(vkVersion.c_str(), size, false);
    });
    PostCleanFreeBuffersTask(CLEAN_FREE_BUFFERS_DURATION, false);
    RS_LOGI("%{public}s RSCanvasModifiersDrawThread started", __func__);
}

void RSCanvasModifiersDrawThread::PostCleanFreeBuffersTask(int64_t delayTime, bool immediately)
{
    if (immediately) {
        RemoveTask(CLEAN_FREE_BUFFERS_TASK_NAME);
    }
    std::weak_ptr<RSCanvasModifiersDrawThread> weakThread = shared_from_this();
    PostTask([weakThread, delayTime, immediately] {
            auto thread = weakThread.lock();
            if (thread == nullptr || thread->canvasModifiersDraw_ == nullptr) {
                return;
            }
            thread->canvasModifiersDraw_->CleanFreeBuffers(immediately ? 0 : delayTime);
            thread->PostCleanFreeBuffersTask(CLEAN_FREE_BUFFERS_DURATION, false);
        }, CLEAN_FREE_BUFFERS_TASK_NAME, delayTime);
}

void RSCanvasModifiersDrawThread::PostCleanFreeBuffersTaskImmediately()
{
    PostCleanFreeBuffersTask(CLEAN_FREE_BUFFERS_IMMEDIATELY_DELAY, true);
}

void RSCanvasModifiersDrawThread::PostTask(
    const std::function<void()>&& task, const std::string& name, int64_t delayTime)
{
    if (!IsStarted()) {
        Start();
    }
    if (handler_ != nullptr) {
        handler_->PostTask(task, name, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSCanvasModifiersDrawThread::PostSyncTask(const std::function<void()>&& task)
{
    if (!IsStarted()) {
        Start();
    }
    if (handler_ != nullptr) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSCanvasModifiersDrawThread::RemoveTask(const std::string& name)
{
    if (!IsStarted()) {
        return;
    }
    if (handler_ != nullptr) {
        handler_->RemoveTask(name);
    }
}

std::shared_ptr<RSCanvasModifiersDraw> RSCanvasModifiersDrawThread::GetCanvasModifiersDraw()
{
    return canvasModifiersDraw_;
}
} // namespace Rosen
} // namespace OHOS