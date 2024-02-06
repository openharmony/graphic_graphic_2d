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

#include "common/rs_upload_texture_thread.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "pipeline/rs_task_dispatcher.h"
#include "rs_trace.h"
#ifdef NEW_SKIA
#include "src/gpu/GrSurfaceProxy.h"
#endif
#if defined(RS_ENABLE_UNI_RENDER) && defined(RS_ENABLE_GL)
#include "render_context/render_context.h"
#endif

namespace OHOS::Rosen {
RSUploadTextureThread& RSUploadTextureThread::Instance()
{
    static RSUploadTextureThread instance;
    return instance;
}

RSUploadTextureThread::RSUploadTextureThread()
{
    runner_ = AppExecFwk::EventRunner::Create("RSUploadTextureThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
}

void RSUploadTextureThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSUploadTextureThread::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSUploadTextureThread::PostTask(const std::function<void()>& task, const std::string& name)
{
    if (handler_) {
        handler_->PostImmediateTask(task, name);
    }
}

void RSUploadTextureThread::RemoveTask(const std::string& name)
{
    if (handler_) {
        handler_->RemoveTask(name);
    }
}
 
bool RSUploadTextureThread::IsEnable() const
{
    return uploadProperity_ && isTargetPlatform_;
}

void RSUploadTextureThread::OnRenderEnd()
{
    if (IsEnable()) {
        std::unique_lock<std::mutex> lock(uploadTaskMutex_);
        enableTime_ = true;
        uploadTaskCond_.notify_all();
    }
#if defined(RS_ENABLE_UNI_RENDER) && defined(RS_ENABLE_GL) && !defined(USE_ROSEN_DRAWING)
    ReleaseNotUsedPinnedViews();
#endif
}

void RSUploadTextureThread::OnProcessBegin()
{
    if (IsEnable()) {
        std::unique_lock<std::mutex> lock(uploadTaskMutex_);
        enableTime_ = false;
    }
    frameCount_.fetch_add(1);
}

int64_t RSUploadTextureThread::GetFrameCount() const
{
    return frameCount_.load();
}

void RSUploadTextureThread::WaitUntilRenderEnd()
{
    RS_TRACE_NAME("Waitfor render_service finish");
    std::unique_lock<std::mutex> lock(uploadTaskMutex_);
    uploadTaskCond_.wait(lock, [this]() { return enableTime_; });
}

bool RSUploadTextureThread::TaskIsValid(int64_t count)
{
    if (count < frameCount_.load()) {
        return false;
    }
    WaitUntilRenderEnd();
    return true;
}

bool RSUploadTextureThread::ImageSupportParallelUpload(int w, int h)
{
    return (w < IMG_WIDTH_MAX) && (h < IMG_HEIGHT_MAX);
}

#if defined(RS_ENABLE_UNI_RENDER) && defined(RS_ENABLE_GL)
#ifndef USE_ROSEN_DRAWING
void RSUploadTextureThread::InitRenderContext(RenderContext* context)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return;
    }
    renderContext_ = context;
    isTargetPlatform_ = RSSystemProperties::IsPhoneType();
    uploadProperity_ = RSSystemProperties::GetParallelUploadTexture();
    PostTask([this]() {
        grContext_ = CreateShareGrContext();
        if (grContext_) {
            grContext_->getCollection().enableCollect();
        }
    });
}

void RSUploadTextureThread::CreateShareEglContext()
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
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

sk_sp<GrDirectContext> RSUploadTextureThread::GetShareGrContext() const
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return nullptr;
    }
    return grContext_;
}

sk_sp<GrDirectContext> RSUploadTextureThread::CreateShareGrContext()
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return nullptr;
    }
    RS_TRACE_NAME("CreateShareGrContext");
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
    handler->ConfigureContext(&options, glesVersion, size);

    sk_sp<GrDirectContext> grContext = GrDirectContext::MakeGL(std::move(glInterface), options);
    if (grContext == nullptr) {
        RS_LOGE("nullptr grContext is null");
        return nullptr;
    }
    return grContext;
}

void RSUploadTextureThread::CleanGrResource()
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return;
    }
    PostTask([this]() {
        WaitUntilRenderEnd();
        RS_TRACE_NAME("ResetGrContext release resource");
        if (grContext_ == nullptr) {
            RS_LOGE("RSUploadTextureThread::grContext_ is nullptr");
            return;
        }
        grContext_->freeGpuResources();
        RS_LOGI("RSUploadTextureThread::CleanGrResource() finished");
    });
}

void RSUploadTextureThread::ReleaseNotUsedPinnedViews()
{
    if (!grContext) {
        return;
    }
    auto& collection = grContext->getCollection();
    if (collection.getCollectionSize() > CLEAN_VIEW_COUNT) {
        auto arrPtr = std::make_shared<std::vector<sk_sp<GrSurfaceProxy>>>();
        if (!arrPtr) {
            return;
        }
        collection.detachCollection(*arrPtr);
        PostTask([proxyArr = std::move(arrPtr)]() {
            RSUploadTextureThread::Instance().WaitUntilRenderEnd();
            RS_TRACE_NAME("ReleasePinnedViews");
            proxyArr->clear();
        });
    }
}

#endif
#endif
}
