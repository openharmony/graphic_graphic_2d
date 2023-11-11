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

#define EGL_EGLEXT_PROTOTYPES
#include "rs_filter_sub_thread.h"

#include <string>

#include "GLES3/gl3.h"
#include "include/core/SkCanvas.h"
#include "memory/rs_memory_manager.h"
#include "memory/rs_tag_tracker.h"
#include "rs_trace.h"

#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_visitor.h"
#ifdef RES_SCHED_ENABLE
#include "res_sched_client.h"
#include "res_type.h"
#endif

namespace OHOS::Rosen {
namespace {
#ifdef RES_SCHED_ENABLE
const uint32_t RS_SUB_QOS_LEVEL = 7;
constexpr const char* RS_BUNDLE_NAME = "render_service";
#endif
// "/data/service/el0/render_service" is shader cache dir
const std::string SHADER_CACHE_DIR = "/data/service/el0/render_service";
} // namespace
RSFilterSubThread::~RSFilterSubThread()
{
    RS_LOGI("~RSSubThread():%{public}d", threadIndex_);
    RSFilter::postTask = nullptr;
    PostTask([this]() { DestroyShareEglContext(); });
}

void RSFilterSubThread::Start()
{
    RS_LOGI("RSFilterSubThread::Start():%{public}d", threadIndex_);
    std::string name = "RSFilterSubThread" + std::to_string(threadIndex_);
    runner_ = AppExecFwk::EventRunner::Create(name);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    PostTask([this]() {
#ifdef RES_SCHED_ENABLE
        std::string strBundleName = RS_BUNDLE_NAME;
        std::string strPid = std::to_string(getpid());
        std::string strTid = std::to_string(gettid());
        std::string strQos = std::to_string(RS_SUB_QOS_LEVEL);
        std::unordered_map<std::string, std::string> mapPayload;
        mapPayload["pid"] = strPid;
        mapPayload[strTid] = strQos;
        mapPayload["bundleName"] = strBundleName;
        uint32_t type = OHOS::ResourceSchedule::ResType::RES_TYPE_THREAD_QOS_CHANGE;
        int64_t value = 0;
        OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, value, mapPayload);
#endif
        grContext_ = CreateShareGrContext();
    });
    RSFilter::postTask = [this](std::weak_ptr<RSFilter::RSFilterTask> task) {
        PostTask([this, task]() { RenderCache(task); });
    };
}

void RSFilterSubThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSFilterSubThread::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSFilterSubThread::DumpMem(DfxString& log)
{
    PostSyncTask([&log, this]() { MemoryManager::DumpDrawingGpuMemory(log, grContext_.get()); });
}

float RSFilterSubThread::GetAppGpuMemoryInMB()
{
    float total = 0.f;
    PostSyncTask([&total, this]() {
        total = MemoryManager::GetAppGpuMemoryInMB(grContext_.get());
    });
    return total;
}

void RSFilterSubThread::CreateShareEglContext()
{
#ifdef RS_ENABLE_GL
    if (renderContext_ == nullptr) {
        RS_LOGE("renderContext_ is nullptr");
        return;
    }
    eglShareContext_ = renderContext_->CreateShareContext();
    if (eglShareContext_ == EGL_NO_CONTEXT) {
        RS_LOGE("eglShareContext_ is EGL_NO_CONTEXT");
        return;
    }
    if (!eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_)) {
        RS_LOGE("eglMakeCurrent failed");
        return;
    }
#endif
}

void RSFilterSubThread::DestroyShareEglContext()
{
#ifdef RS_ENABLE_GL
    if (renderContext_ != nullptr) {
        eglDestroyContext(renderContext_->GetEGLDisplay(), eglShareContext_);
        eglShareContext_ = EGL_NO_CONTEXT;
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
#endif
}

void RSFilterSubThread::RenderCache(std::weak_ptr<RSFilter::RSFilterTask> filterTask)
{
    RS_TRACE_NAME("RenderCache");
    auto task = filterTask.lock();
    if (!task) {
        RS_LOGE("task is null");
        return;
    }
    if (grContext_ == nullptr) {
        grContext_ = CreateShareGrContext();
    }
    if (grContext_ == nullptr) {
        RS_LOGE("grContext is null");
        return;
    }
    if (!task->InitSurface(grContext_.get())) {
        RS_LOGE("InitSurface failed");
        return;
    }
    if (!task->Render()) {
        RS_LOGE("Render failed");
    }
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
sk_sp<GrDirectContext> RSFilterSubThread::CreateShareGrContext()
#else
sk_sp<GrContext> RSFilterSubThread::CreateShareGrContext()
#endif
{
    RS_TRACE_NAME("CreateShareGrContext");
    CreateShareEglContext();
    const GrGLInterface* grGlInterface = GrGLCreateNativeInterface();
    sk_sp<const GrGLInterface> glInterface(grGlInterface);
    if (glInterface.get() == nullptr) {
        RS_LOGE("CreateShareGrContext failed");
        return nullptr;
    }

    GrContextOptions options = {};
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;

    auto handler = std::make_shared<MemoryHandler>();
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    handler->ConfigureContext(&options, glesVersion, size, SHADER_CACHE_DIR, true);

#ifdef NEW_SKIA
    sk_sp<GrDirectContext> grContext = GrDirectContext::MakeGL(std::move(glInterface), options);
#else
    sk_sp<GrContext> grContext = GrContext::MakeGL(std::move(glInterface), options);
#endif
    if (grContext == nullptr) {
        RS_LOGE("nullptr grContext is null");
        return nullptr;
    }
    return grContext;
}
#else
std::shared_ptr<Drawing::GPUContext> RSFilterSubThread::CreateShareGrContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
    CreateShareEglContext();
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    Drawing::GPUContextOptions options;
    auto handler = std::make_shared<MemoryHandler>();
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    handler->ConfigureContext(&options, glesVersion, size, SHADER_CACHE_DIR, true);

    if (!gpuContext->BuildFromGL(options)) {
        RS_LOGE("nullptr gpuContext is null");
        return nullptr;
    }
    return gpuContext;
}
#endif

void RSFilterSubThread::ResetGrContext()
{
    RS_TRACE_NAME("ResetGrContext release resource");
    if (grContext_ == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    grContext_->freeGpuResources();
#else
    grContext_->FreeGpuResources();
#endif
}
} // namespace OHOS::Rosen
