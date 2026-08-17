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

#include "common/rs_optional_trace.h"
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

#ifdef RES_BASE_SCHED_ENABLE
#include "qos.h"
#endif

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
#ifdef RES_BASE_SCHED_ENABLE
    PostTask(
        [this]() {
            auto ret = OHOS::QOS::SetThreadQos(OHOS::QOS::QosLevel::QOS_USER_INTERACTIVE);
            RS_LOGI("RSColorPickerThread: SetThreadQos retcode = %{public}d", ret);
        },
        0);
#endif
}

bool RSColorPickerThread::PostTask(const std::function<void()>& task, int64_t delayTime)
{
    if (!handler_) {
        return false;
    }
    handler_->PostTask(task, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
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
    PostTask(
        [this]() {
            gpuContext_ = CreateShareGPUContext();
            if (gpuContext_ == nullptr) {
                return;
            }
            gpuContext_->RegisterPostFunc(
                [](const std::function<void()>& task) { RSColorPickerThread::Instance().PostTask(task, 0); });
        },
        0);
}

std::shared_ptr<Drawing::GPUContext> RSColorPickerThread::GetShareGPUContext() const
{
    return gpuContext_;
}

std::shared_ptr<RenderContext> RSColorPickerThread::GetRenderContext() const
{
    return renderContext_;
}

std::shared_ptr<Drawing::GPUContext> RSColorPickerThread::CreateShareGPUContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
    auto gpuContext = renderContext_->CreateDrawingGPUContext();
    if (gpuContext == nullptr) {
        RS_LOGE("RSColorPickerThread CreateShareGPUContext fail");
        return nullptr;
    }
    return gpuContext;
}
#endif
} // namespace OHOS::Rosen
