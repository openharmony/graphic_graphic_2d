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

#include "rs_upload_resource_thread.h"

#include "rs_trace.h"
#include "platform/common/rs_system_properties.h"
#include "pipeline/rs_task_dispatcher.h"
#include "render/rs_resource_manager.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "engine_adapter/skia_adapter/skia_gpu_context.h"

namespace OHOS::Rosen {
// Task Related
RSUploadResourceThread& RSUploadResourceThread::Instance()
{
    static RSUploadResourceThread sInstance;
    return sInstance;
}

RSUploadResourceThread::RSUploadResourceThread()
{
    runner_ = AppExecFwk::EventRunner::Create("RSUploadResourceThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
}

void RSUploadResourceThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSUploadResourceThread::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSUploadResourceThread::PostTask(const std::function<void()>& task, const std::string& name)
{
    if (handler_) {
        handler_->PostImmediateTask(task, name);
    }
}

void RSUploadResourceThread::RemoveTask(const std::string& name)
{
    if (handler_) {
        handler_->RemoveTask(name);
    }
}

void RSUploadResourceThread::OnRenderEnd()
{
    if (IsEnable()) {
        std::unique_lock<std::mutex> lock(uploadTaskMutex_);
        enableTime_ = true;
        uploadTaskCond_.notify_all();
    }
    ReleaseNotUsedPinnedViews();
}

void RSUploadResourceThread::OnProcessBegin()
{
    if (IsEnable()) {
        std::unique_lock<std::mutex> lock(uploadTaskMutex_);
        enableTime_ = false;
    }
    frameCount_.fetch_add(1);
}

void RSUploadResourceThread::WaitUntilRenderEnd()
{
    RS_TRACE_NAME("Waitfor render_service finish");
    std::unique_lock<std::mutex> lock(uploadTaskMutex_);
    uploadTaskCond_.wait(lock, [this]() { return enableTime_; });
}

bool RSUploadResourceThread::TaskIsValid(int64_t count)
{
    if (count < frameCount_.load()) {
        return false;
    }
    WaitUntilRenderEnd();
    return true;
}

void RSUploadResourceThread::InitRenderContext(RenderContext* context)
{
    renderContext_ = context;
    isTargetPlatform_ = RSSystemProperties::IsPhoneType();
    uploadProperity_ = RSSystemProperties::GetParallelUploadTexture();
    PostTask([this]() {
        grContext_ = CreateShareGrContext();
        if (grContext_) {
#ifndef USE_ROSEN_DRAWING
            grContext_->setResourceCollector(&resCollector_);
#else
            auto skiaGPUContext  = grContext_->GetImpl<Drawing::SkiaGPUContext>();
            if (!skiaGPUContext) {
                return;
            } else {
                skContext_ = skiaGPUContext->GetGrContext();
            }
            if (skContext_) {
                skContext_->setResourceCollector(&resCollector_);
            }
#endif
        }
#ifndef USE_ROSEN_DRAWING
        RSResourceManager::Instance().SetUploadTextureFunction(UploadTextureWithSkia);
#else
        RSResourceManager::Instance().SetUploadTextureFunction(UploadTextureWithDrawing);
#endif
    });
}

#ifdef RS_ENABLE_GL
void RSUploadResourceThread::CreateShareEglContext()
{
    if (Rosen::RSSystemProperties::GetGpuApiType() == Rosen::GpuApiType::OPENGL) {
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
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<GrDirectContext> RSUploadResourceThread::GetShareGrContext() const
{
    return grContext_;
}
sk_sp<GrDirectContext> RSUploadResourceThread::CreateShareGrContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
    sk_sp<GrDirectContext> grContext = nullptr;
#ifdef RS_ENABLE_GL
    if (Rosen::RSSystemProperties::GetGpuApiType() == Rosen::GpuApiType::OPENGL) {
        CreateShareEglContext();
        const GrGLInterface* glGlInterface = GrGLCreateNativeInterface();
        sk_sp<const GrGLInterface> glInterface(glGlInterface);
        if (glInterface.get() == nullptr) {
            RS_LOGE("GrGLCreateNativeInterface failed.");
            return nullptr;
        }

        GrContextOptions options = {};
        options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
        // fix svg antialiasing bug
        options.fGpuPathRenderers &= ~GpuPathRenderers::kAtlas;
        options.fPreferExternalImagesOverES3 = true;
        options.fDisableDistanceFieldPaths = true;

        auto handler = std::make_shared<MemoryHandler>();
        auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        auto size = glesVersion ? strlen(glesVersion) : 0;
        /* /data/service/el0/render_service is shader cache dir */
        handler->ConfigureContext(&options, glesVersion, size, "/data/service/el0/render_service", true);

        grContext = GrDirectContext::MakeGL(std::move(glInterface), options);
    }
#endif

#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
    // Get Vulkan Context
        auto backendContext = RsVulkanContext::GetSingleton().GetGrVkBackendContext();
        grContext = GrDirectContext::MakeVulkan(backendContext);
    }
#endif

    if (grContext == nullptr) {
        RS_LOGE("nullptr grContext is null");
        return nullptr;
    }
    return grContext;
}
#else
std::shared_ptr<Drawing::GPUContext> RSUploadResourceThread::GetShareGrContext() const
{
    return grContext_;
}
std::shared_ptr<Drawing::GPUContext> RSUploadResourceThread::CreateShareGrContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
#ifdef RS_ENABLE_GL
    if (Rosen::RSSystemProperties::GetGpuApiType() == Rosen::GpuApiType::OPENGL) {
        CreateShareEglContext();
        gpuContext = std::make_shared<Drawing::GPUContext>();
        Drawing::GPUContextOptions options;
        auto handler = std::make_shared<MemoryHandler>();
        auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        auto size = glesVersion ? strlen(glesVersion) : 0;
        /* /data/service/el0/render_service is shader cache dir */
        handler->ConfigureContext(&options, glesVersion, size, "/data/service/el0/render_service", true);

        if (!gpuContext->BuildFromGL(options)) {
            RS_LOGE("nullptr gpuContext is null");
            return nullptr;
        }
    }
#endif

#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        gpuContext = std::make_shared<Drawing::GPUContext>();
        if (!gpuContext->BuildFromVK(RsVulkanContext::GetSingleton().GetGrVkBackendContext())) {
            RS_LOGE("nullptr gpuContext is null");
            return nullptr;
        }
    }
#endif
    return gpuContext;
}
#endif

void RSUploadResourceThread::ReleaseNotUsedPinnedViews()
{
    if (!grContext_ || OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        return;
    }
 
#ifndef USE_ROSEN_DRAWING
    sk_sp<GrDirectContext> grContext = grContext_;
#else
    sk_sp<GrDirectContext> grContext = skContext_;
    if (!grContext) {
        return;
    }
#endif
    auto arrPtr = std::make_shared<std::vector<sk_sp<GrSurfaceProxy>>>();
    resCollector_.SwapCollection(*arrPtr, CLEAN_VIEW_COUNT);
    if (arrPtr->empty()) {
        return;
    }
    PostTask([proxyArr = std::move(arrPtr)]() {
        RSUploadResourceThread::Instance().WaitUntilRenderEnd();
        RS_TRACE_NAME("ReleasePinnedViews");
        proxyArr->clear();
    });
}
}
