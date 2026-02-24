/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "feature/color_picker/rs_color_picker_thread.h"

#include <chrono>

#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

#if defined(RS_ENABLE_UNI_RENDER)
#ifdef RS_ENABLE_GL
#include "render_context/render_context.h"
#endif
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#endif

#include "rs_trace.h"

namespace OHOS::Rosen {
RSColorPickerThread& RSColorPickerThread::Instance()
{
    static RSColorPickerThread instance;
    return instance;
}

RSColorPickerThread::RSColorPickerThread()
{
    RS_LOGI("RSColorPickerThread Start!");
    runner_ = AppExecFwk::EventRunner::Create("RSColorPickerThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
}

namespace {
constexpr uint32_t ONE_SECOND = 1000;
constexpr uint32_t MAX_TASKS_PER_SECOND = 20;
} // namespace

bool RSColorPickerThread::PostTask(const std::function<void()>& task, bool limited, int64_t delayTime)
{
    if (!handler_) {
        return false;
    }
    if (!limited) {
        handler_->PostTask(task, delayTime, AppExecFwk::EventQueue::Priority::HIGH);
        return true;
    }

    const auto now =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();
    int64_t lastReset = lastResetTime_.load(std::memory_order_relaxed);
    // Reset counter if more than 1 second has passed
    if (now - lastReset >= ONE_SECOND) {
        // Try to update the reset time atomically
        if (lastResetTime_.compare_exchange_strong(lastReset, now, std::memory_order_relaxed)) {
            taskCount_.store(0, std::memory_order_relaxed);
        }
    }

    // Check if we've exceeded the rate limit
    uint32_t currentCount = taskCount_.fetch_add(1, std::memory_order_relaxed);
    if (currentCount >= MAX_TASKS_PER_SECOND) {
        RS_LOGD("RSColorPickerThread: Task dropped due to rate limit (count: %{public}u)", currentCount + 1);
        return false;
    }

    handler_->PostTask(task, delayTime, AppExecFwk::EventQueue::Priority::HIGH);
    return true;
}

void RSColorPickerThread::RegisterNodeDirtyCallback(const NodeDirtyCallback& callback)
{
    if (callback == nullptr) {
        RS_LOGE("RSColorPickerThread RegisterNodeDirtyCallback, callback invalid!");
    }
    callback_ = callback;
}

void RSColorPickerThread::RegisterNotifyClientCallback(const NotifyClientCallback& callback)
{
    if (callback == nullptr) {
        RS_LOGE("RSColorPickerThread RegisterColorPickerCallback, callback invalid!");
    }
    notifyClient_ = callback;
}

void RSColorPickerThread::NotifyNodeDirty(uint64_t nodeId)
{
    if (callback_) {
        callback_(nodeId);
    }
}

void RSColorPickerThread::NotifyClient(uint64_t nodeId, uint32_t color)
{
    if (notifyClient_) {
        notifyClient_(nodeId, color);
    }
}

#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
void RSColorPickerThread::InitRenderContext(std::shared_ptr<RenderContext> context)
{
    renderContext_ = context;
    PostTask([this]() {
        gpuContext_ = CreateShareGPUContext();
        if (gpuContext_ == nullptr) {
            return;
        }
        gpuContext_->RegisterPostFunc(
            [](const std::function<void()>& task) { RSColorPickerThread::Instance().PostTask(task); });
        }, false);
}

std::shared_ptr<Drawing::GPUContext> RSColorPickerThread::GetShareGPUContext() const
{
    return gpuContext_;
}

std::shared_ptr<Drawing::GPUContext> RSColorPickerThread::CreateShareGPUContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
#ifdef RS_ENABLE_GL
    if (!RSSystemProperties::IsUseVulkan()) {
        auto gpuContext = std::make_shared<Drawing::GPUContext>();
        if (gpuContext == nullptr) {
            return nullptr;
        }
        renderContext_->CreateShareContext();

        Drawing::GPUContextOptions options = {};
        auto handler = std::make_shared<MemoryHandler>();
        auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        auto size = glesVersion ? strlen(glesVersion) : 0;
        handler->ConfigureContext(&options, glesVersion, size);
        if (!gpuContext->BuildFromGL(options)) {
            RS_LOGE("RSColorPickerThread BuildFromGL fail");
            return nullptr;
        }
        return gpuContext;
    }
#endif

#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        auto gpuContext = RsVulkanContext::GetSingleton().CreateDrawingContext();
        if (gpuContext == nullptr) {
            RS_LOGE("RSColorPickerThread BuildFromVK fail");
            return nullptr;
        }
        return gpuContext;
    }
#endif
    return nullptr;
}
#endif
} // namespace OHOS::Rosen
