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
#include "rs_sub_thread.h"

#include <string>
#include "GLES3/gl3.h"
#include "include/core/SkCanvas.h"
#include "rs_trace.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "pipeline/rs_surface_render_node.h"
#ifdef RES_SCHED_ENABLE
#include "res_type.h"
#include "res_sched_client.h"
#endif

namespace OHOS::Rosen {
namespace {
#ifdef RES_SCHED_ENABLE
    const uint32_t RS_SUB_QOS_LEVEL = 7;
    constexpr const char* RS_BUNDLE_NAME = "render_service";
#endif
}
RSSubThread::~RSSubThread()
{
    RS_LOGI("~RSSubThread():%d", threadIndex_);
    PostTask([this]() {
        DestroyShareEglContext();
    });
}

void RSSubThread::Start()
{
    RS_LOGI("RSSubThread::Start():%d", threadIndex_);
    std::string name = "RSSubThread" + std::to_string(threadIndex_);
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
        CreateShareEglContext();
    });
}

void RSSubThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSSubThread::CreateShareEglContext()
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

void RSSubThread::DestroyShareEglContext()
{
#ifdef RS_ENABLE_GL
    if (renderContext_ != nullptr) {
        eglDestroyContext(renderContext_->GetEGLDisplay(), eglShareContext_);
        eglShareContext_ = EGL_NO_CONTEXT;
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
#endif
}

void RSSubThread::RenderCache(const std::shared_ptr<RSSuperRenderTask>& threadTask)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    visitor->SetSubThreadConfig(threadIndex_);
    if (threadTask == nullptr) {
        return;
    }
#ifdef RS_ENABLE_GL
    while (threadTask->GetTaskSize() > 0) {
        auto task = threadTask->GetNextRenderTask();
        if (!task || (task->GetIdx() == 0)) {
            RS_LOGE("renderTask is nullptr");
            continue;
        }
        auto node = task->GetNode();
        if (!node) {
            RS_LOGE("surfaceNode is nullptr");
            continue;
        }
        auto surfaceNodePtr = node->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (!surfaceNodePtr) {
            RS_LOGE("RenderCache ReinterpretCastTo fail");
            continue;
        }
        // flag CacheSurfaceProcessed is used for cacheCmdskippedNodes collection in rs_mainThread
        surfaceNodePtr->SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);

        if (RSMainThread::Instance()->GetFrameCount() != threadTask->GetFrameCount()) {
            surfaceNodePtr->SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
            continue;
        }

        RS_TRACE_NAME_FMT("draw cache render node: [%s, %llu]", surfaceNodePtr->GetName().c_str(),
            surfaceNodePtr->GetId());
        if (surfaceNodePtr->GetCacheSurface() == nullptr) {
            if (grContext_ == nullptr) {
                grContext_ = CreateShareGrContext();
            }
            if (grContext_ == nullptr) {
                RS_LOGE("Share GrContext is not ready!!!");
                return;
            }
            RSRenderNode::ClearCacheSurfaceFunc func = std::bind(&RSUniRenderUtil::ClearNodeCacheSurface,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            surfaceNodePtr->InitCacheSurface(grContext_.get(), func, threadIndex_);
        }

        bool needNotify = !surfaceNodePtr->HasCachedTexture();
        node->Process(visitor);
#ifndef NEW_SKIA
        auto skCanvas = surfaceNodePtr->GetCacheSurface() ? surfaceNodePtr->GetCacheSurface()->getCanvas() : nullptr;
        if (skCanvas) {
            RS_TRACE_NAME_FMT("render cache flush, %s", surfaceNodePtr->GetName().c_str());
            skCanvas->flush();
        } else {
            RS_LOGE("skCanvas is nullptr, flush failed");
        }
#else
        if (surfaceNodePtr && surfaceNodePtr->GetCacheSurface()) {
            RS_TRACE_NAME_FMT("Render cache skSurface flush and submit");
            surfaceNodePtr->GetCacheSurface()->flushAndSubmit(false);
        }
#endif
        surfaceNodePtr->UpdateBackendTexture();
        surfaceNodePtr->SetCacheSurfaceProcessedStatus(CacheProcessStatus::DONE);

        if (needNotify) {
            RSSubThreadManager::Instance()->NodeTaskNotify(node->GetId());
        }
    }
    eglCreateSyncKHR(renderContext_->GetEGLDisplay(), EGL_SYNC_FENCE_KHR, nullptr);
#endif
}

#ifdef NEW_SKIA
sk_sp<GrDirectContext> RSSubThread::CreateShareGrContext()
#else
sk_sp<GrContext> RSSubThread::CreateShareGrContext()
#endif
{
    const GrGLInterface *grGlInterface = GrGLCreateNativeInterface();
    sk_sp<const GrGLInterface> glInterface(grGlInterface);
    if (glInterface.get() == nullptr) {
        RS_LOGE("CreateShareGrContext failed");
        return nullptr;
    }

    GrContextOptions options = {};
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;
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
}