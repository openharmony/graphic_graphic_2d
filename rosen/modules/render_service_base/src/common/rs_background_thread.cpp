/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
RSBackgroundThread& RSBackgroundThread::Instance()
{
    static RSBackgroundThread instance;
    return instance;
}

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
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifdef RS_ENABLE_GL
void RSBackgroundThread::CreateShareEglContext()
{
    if (RSSystemProperties::IsUseVulkan()) {
        return;
    }
    if (renderContext_ == nullptr) {
        RS_LOGE("renderContext_ is nullptr.");
        return;
    }
    eglShareContext_ = renderContext_->CreateShareContext();
    if (eglShareContext_ == EGL_NO_CONTEXT) {
        RS_LOGE("eglShareContext_ is EGL_NO_CONTEXT");
        return;
    }
    if (!eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_)) {
        RS_LOGE("eglMakeCurrent failed.");
        return;
    }
}
#endif
void RSBackgroundThread::InitRenderContext(RenderContext* context)
{
    renderContext_ = context;
    PostTask([this]() {
        gpuContext_ = CreateShareGPUContext();
    });
}

std::shared_ptr<Drawing::GPUContext> RSBackgroundThread::GetShareGPUContext() const
{
    return gpuContext_;
}

std::shared_ptr<Drawing::GPUContext> RSBackgroundThread::CreateShareGPUContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
#ifdef RS_ENABLE_GL
    if (!RSSystemProperties::IsUseVulkan()) {
        auto gpuContext = std::make_shared<Drawing::GPUContext>();
        if (gpuContext == nullptr) {
            RS_LOGE("BuildFromVK fail");
            return nullptr;
        }
        CreateShareEglContext();

        Drawing::GPUContextOptions options = {};
        auto handler = std::make_shared<MemoryHandler>();
        auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        auto size = glesVersion ? strlen(glesVersion) : 0;
        handler->ConfigureContext(&options, glesVersion, size);
        if (!gpuContext->BuildFromGL(options)) {
            RS_LOGE("BuildFromGL fail");
            return nullptr;
        }
        return gpuContext;
    }
#endif

#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        auto gpuContext = RsVulkanContext::GetSingleton().CreateDrawingContext();
        if (gpuContext == nullptr) {
            RS_LOGE("BuildFromVK fail");
            return nullptr;
        }
        return gpuContext;
    }
#endif
    return nullptr;
}

void RSBackgroundThread::SetGrResourceFinishFlag(bool resourceFinish)
{
    resourceFinish_ = resourceFinish;
}

bool RSBackgroundThread::GetGrResourceFinishFlag()
{
    return resourceFinish_;
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
}
