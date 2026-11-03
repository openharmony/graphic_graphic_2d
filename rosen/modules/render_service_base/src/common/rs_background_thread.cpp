/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "common/rs_background_thread.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#if defined(RS_ENABLE_UNI_RENDER)
#ifdef RS_ENABLE_GL
#include "render_context/new_render_context/render_context_gl.h"
#include "render_context/render_context.h"
#endif
#ifdef RS_ENABLE_VK
#include "vulkan_context/rs_vulkan_context.h"
#endif
#endif
#include "rs_trace.h"

#ifdef RES_BASE_SCHED_ENABLE
#include "qos.h"
#endif

namespace OHOS::Rosen {
RSBackgroundThread& RSBackgroundThread::Instance()
{
    static RSBackgroundThread instance;
    return instance;
}
// LCOV_EXCL_START
RSBackgroundThread::RSBackgroundThread()
{
    runner_ = AppExecFwk::EventRunner::Create("RSBackgroundThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
#ifdef RES_BASE_SCHED_ENABLE
    PostTask([this]() {
        auto ret = OHOS::QOS::SetThreadQos(OHOS::QOS::QosLevel::QOS_USER_INTERACTIVE);
        RS_LOGI("RSBackgroundThread: SetThreadQos retcode = %{public}d", ret);
    });
#endif
}

void RSBackgroundThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSBackgroundThread::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
void RSBackgroundThread::InitRenderContext(std::shared_ptr<RenderContext> context)
{
    renderContext_ = context;
    PostTask([this]() {
        gpuContext_ = CreateShareGPUContext();
        if (gpuContext_ == nullptr) {
            return;
        }
        gpuContext_->RegisterPostFunc([](const std::function<void()>& task) {
            RSBackgroundThread::Instance().PostTask(task);
        });
    });
}

std::shared_ptr<Drawing::GPUContext> RSBackgroundThread::GetShareGPUContext() const
{
    return gpuContext_;
}

std::shared_ptr<RenderContext> RSBackgroundThread::GetRenderContext() const
{
    return renderContext_;
}

void RSBackgroundThread::HoldSurface(std::shared_ptr<Drawing::Surface> surface)
{
    surfaceHolder_ = surface;
}

std::shared_ptr<Drawing::GPUContext> RSBackgroundThread::CreateShareGPUContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    gpuContext_ = renderContext_->CreateDrawingGPUContext();
    if (gpuContext_ == nullptr) {
        RS_LOGE("CreateShareGPUContext fail");
        return nullptr;
    }
    return gpuContext_;
#endif
    return nullptr;
}

void RSBackgroundThread::CleanGrResource()
{
    PostTask([this]() {
        RS_TRACE_NAME("ResetGPUContext release resource");
        if (gpuContext_ == nullptr) {
            RS_LOGE("RSBackgroundThread::gpuContext_ is nullptr");
            return;
        }
        gpuContext_->FreeGpuResources();
        RS_LOGD("RSBackgroundThread::CleanGrResource() finished");
    });
}
#endif
// LCOV_EXCL_STOP
}
