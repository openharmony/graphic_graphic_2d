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
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
#include "render_context/render_context.h"
#endif
#include "rs_trace.h"

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
}

void RSBackgroundThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
#ifndef USE_ROSEN_DRAWING
void RSBackgroundThread::InitRenderContext(RenderContext* context)
{
    renderContext_ = context;
    PostTask([this]() {
	    grContext_ = CreateShareGrContext();
    });
}

void RSBackgroundThread::CreateShareEglContext()
{
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

sk_sp<GrDirectContext> RSBackgroundThread::GetShareGrContext() const
{
    return grContext_;
}

sk_sp<GrDirectContext> RSBackgroundThread::CreateShareGrContext()
{
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
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;

    auto handler = std::make_shared<MemoryHandler>();
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    /* /data/service/el0/render_service is shader cache dir*/
    handler->ConfigureContext(&options, glesVersion, size, "/data/service/el0/render_service", true);

    sk_sp<GrDirectContext> grContext = GrDirectContext::MakeGL(std::move(glInterface), options);
    if (grContext == nullptr) {
        RS_LOGE("nullptr grContext is null");
        return nullptr;
    }
    return grContext;
}

void RSBackgroundThread::CleanGrResource()
{
    PostTask([this]() {
        RS_TRACE_NAME("ResetGrContext release resource");
        if (grContext_ == nullptr) {
            RS_LOGE("RSBackgroundThread::grContext_ is nullptr");
            return;
        }
        grContext_->freeGpuResources();
        RS_LOGI("RSBackgroundThread::CleanGrResource() finished");
    });
}
#endif
#endif
}
