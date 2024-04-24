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

#include "memory/rs_memory_graphic.h"
#include "memory/rs_memory_manager.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_visitor.h"

#ifdef RS_PARALLEL
#include "pipeline/rs_uifirst_manager.h"
#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#endif

#ifdef RES_SCHED_ENABLE
#include "res_type.h"
#include "res_sched_client.h"
#endif

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
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
    RS_LOGI("~RSSubThread():%{public}d", threadIndex_);
    PostTask([this]() {
        DestroyShareEglContext();
    });
}

pid_t RSSubThread::Start()
{
    RS_LOGI("RSSubThread::Start():%{public}d", threadIndex_);
    std::string name = "RSSubThread" + std::to_string(threadIndex_);
    runner_ = AppExecFwk::EventRunner::Create(name);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    pid_t tid;
    PostSyncTask([&tid]() {
        tid = gettid();
    });
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
        if (grContext_ == nullptr) {
            return;
        }
        grContext_->RegisterPostFunc([this](const std::function<void()>& task) {
            PostTask(task);
        });
    });
    return tid;
}

void RSSubThread::PostTask(const std::function<void()>& task, const std::string& name)
{
    if (handler_) {
        handler_->PostImmediateTask(task, name);
    }
}

void RSSubThread::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSSubThread::RemoveTask(const std::string& name)
{
    if (handler_) {
        handler_->RemoveTask(name);
    }
}

void RSSubThread::DumpMem(DfxString& log)
{
    PostSyncTask([&log, this]() {
        MemoryManager::DumpDrawingGpuMemory(log, grContext_.get());
    });
}

float RSSubThread::GetAppGpuMemoryInMB()
{
    float total = 0.f;
    PostSyncTask([&total, this]() {
        total = MemoryManager::GetAppGpuMemoryInMB(grContext_.get());
    });
    return total;
}

void RSSubThread::CreateShareEglContext()
{
    if (renderContext_ == nullptr) {
        RS_LOGE("renderContext_ is nullptr");
        return;
    }
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
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
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
        return;
    }
    if (renderContext_ != nullptr) {
        eglDestroyContext(renderContext_->GetEGLDisplay(), eglShareContext_);
        eglShareContext_ = EGL_NO_CONTEXT;
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
#endif
}

void RSSubThread::RenderCache(const std::shared_ptr<RSSuperRenderTask>& threadTask)
{
    RS_TRACE_NAME("RSSubThread::RenderCache");
    if (threadTask == nullptr || threadTask->GetTaskSize() == 0) {
        RS_LOGE("RSSubThread::RenderCache threadTask == nullptr %p || threadTask->GetTaskSize() == 0 %d",
            threadTask.get(), int(threadTask->GetTaskSize()));
        return;
    }
    if (grContext_ == nullptr) {
        grContext_ = CreateShareGrContext();
        if (grContext_ == nullptr) {
            return;
        }
    }
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    visitor->SetSubThreadConfig(threadIndex_);
    visitor->SetFocusedNodeId(RSMainThread::Instance()->GetFocusNodeId(),
        RSMainThread::Instance()->GetFocusLeashWindowId());
    auto screenManager = CreateOrGetScreenManager();
    visitor->SetScreenInfo(screenManager->QueryScreenInfo(screenManager->GetDefaultScreenId()));
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    bool needRequestVsync = false;
    while (threadTask->GetTaskSize() > 0) {
        auto task = threadTask->GetNextRenderTask();
        if (!task || (task->GetIdx() == 0)) {
            continue;
        }
        auto nodeDrawable = task->GetNode();
        if (!nodeDrawable) {
            continue;
        }
        auto surfaceNodePtr = std::static_pointer_cast<RSSurfaceRenderNode>(nodeDrawable);
        if (!surfaceNodePtr) {
            continue;
        }
        // flag CacheSurfaceProcessed is used for cacheCmdskippedNodes collection in rs_mainThread
        surfaceNodePtr->SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);
        if (RSMainThread::Instance()->GetFrameCount() != threadTask->GetFrameCount()) {
            surfaceNodePtr->SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
            continue;
        }

        RS_TRACE_NAME_FMT("draw cache render nodeDrawable: [%s, %llu]", surfaceNodePtr->GetName().c_str(),
            surfaceNodePtr->GetId());
        if (surfaceNodePtr->GetCacheSurface(threadIndex_, true) == nullptr || surfaceNodePtr->NeedInitCacheSurface()) {
            RSRenderNode::ClearCacheSurfaceFunc func = std::bind(&RSUniRenderUtil::ClearNodeCacheSurface,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
            surfaceNodePtr->InitCacheSurface(grContext_.get(), func, threadIndex_);
        }

        RSTagTracker nodeProcessTracker(grContext_.get(), surfaceNodePtr->GetId(),
            RSTagTracker::TAGTYPE::TAG_SUB_THREAD);
        bool needNotify = !surfaceNodePtr->HasCachedTexture();
        nodeDrawable->Process(visitor);
        nodeProcessTracker.SetTagEnd();
        auto cacheSurface = surfaceNodePtr->GetCacheSurface(threadIndex_, true);
        if (cacheSurface) {
            RS_TRACE_NAME_FMT("Rendercache skSurface flush and submit");
            RSTagTracker nodeFlushTracker(grContext_.get(), surfaceNodePtr->GetId(),
                RSTagTracker::TAGTYPE::TAG_SUB_THREAD);
            cacheSurface->FlushAndSubmit(true);
            nodeFlushTracker.SetTagEnd();
        }
        surfaceNodePtr->UpdateBackendTexture();
        RSMainThread::Instance()->PostTask([]() {
            RSMainThread::Instance()->SetIsCachedSurfaceUpdated(true);
        });
        surfaceNodePtr->SetCacheSurfaceProcessedStatus(CacheProcessStatus::DONE);
        surfaceNodePtr->SetCacheSurfaceNeedUpdated(true);
        needRequestVsync = true;

        if (needNotify) {
            RSSubThreadManager::Instance()->NodeTaskNotify(nodeDrawable->GetId());
        }
    }
    if (needRequestVsync) {
        RSMainThread::Instance()->RequestNextVSync();
    }
#endif
}

#ifdef RS_PARALLEL
void RSSubThread::DrawableCache(DrawableV2::RSSurfaceRenderNodeDrawable* nodeDrawable)
{
    RS_TRACE_NAME_FMT("RSSubThread::DrawableCache");
    if (grContext_ == nullptr) {
        grContext_ = CreateShareGrContext();
        if (grContext_ == nullptr) {
            RS_LOGE("RSSubThread::RenderCache DrawableCache grContext is null");
            return;
        }
    }

    const auto& param = nodeDrawable->GetRenderParams();
    if (!param) {
        return;
    }
    nodeDrawable->SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);

    auto cacheSurface = nodeDrawable->GetCacheSurface(threadIndex_, true);
    if (!cacheSurface || nodeDrawable->NeedInitCacheSurface()) {
        DrawableV2::RSSurfaceRenderNodeDrawable::ClearCacheSurfaceFunc func = std::bind(
            &RSUniRenderUtil::ClearNodeCacheSurface, std::placeholders::_1, std::placeholders::_2,
            std::placeholders::_3, std::placeholders::_4);
        nodeDrawable->InitCacheSurface(grContext_.get(), func, threadIndex_);
        cacheSurface = nodeDrawable->GetCacheSurface(threadIndex_, true);
    }

    if (!cacheSurface) {
        RS_LOGE("RSSubThread::DrawableCache cacheSurface is nullptr");
        return;
    }

    auto rscanvas = std::make_shared<RSPaintFilterCanvas>(cacheSurface.get());
    if (rscanvas == nullptr) {
        RS_LOGE("RSSubThread::DrawableCache canvas is nullptr");
        return;
    }

    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    if (!uniParam) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw uniParam is nullptr");
        return;
    }
    bool uifirstDebug = uniParam->GetUIFirstDebugEnabled();

    rscanvas->SetIsParallelCanvas(true);
    rscanvas->SetDisableFilterCache(true);
    rscanvas->SetParallelThreadIdx(threadIndex_);
    rscanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    if (uifirstDebug) {
        Drawing::Brush rectBrush;
        // Alpha 128, blue 255
        rectBrush.SetColor(Drawing::Color(128, 0, 0, 255));
        rscanvas->AttachBrush(rectBrush);
        // Left 800, top 500, width 1000, height 700
        rscanvas->DrawRect(Drawing::Rect(800, 500, 1000, 700));
        rscanvas->DetachBrush();
    }
    nodeDrawable->SubDraw(*rscanvas);
    if (uifirstDebug) {
        Drawing::Brush rectBrush;
        // Alpha 128, blue 255
        rectBrush.SetColor(Drawing::Color(128, 0, 0, 255));
        rscanvas->AttachBrush(rectBrush);
        // Left 300, top 500, width 500, height 700
        rscanvas->DrawRect(Drawing::Rect(300, 500, 500, 700));
        rscanvas->DetachBrush();
    }
    if (cacheSurface) {
        RS_TRACE_NAME_FMT("Render cache skSurface flush and submit");
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        auto& vkContext = RsVulkanContext::GetSingleton().GetRsVulkanInterface();

        VkExportSemaphoreCreateInfo exportSemaphoreCreateInfo;
        exportSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO;
        exportSemaphoreCreateInfo.pNext = nullptr;
        exportSemaphoreCreateInfo.handleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT;

        VkSemaphoreCreateInfo semaphoreInfo;
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = &exportSemaphoreCreateInfo;
        semaphoreInfo.flags = 0;
        VkSemaphore semaphore;
        vkContext.vkCreateSemaphore(vkContext.GetDevice(), &semaphoreInfo, nullptr, &semaphore);
        RS_TRACE_NAME_FMT("VkSemaphore %p", semaphore);
        GrBackendSemaphore backendSemaphore;
        backendSemaphore.initVulkan(semaphore);

        DestroySemaphoreInfo* destroyInfo =
            new DestroySemaphoreInfo(vkContext.vkDestroySemaphore, vkContext.GetDevice(), semaphore);

        Drawing::FlushInfo drawingFlushInfo;
        drawingFlushInfo.backendSurfaceAccess = true;
        drawingFlushInfo.numSemaphores = 1;
        drawingFlushInfo.backendSemaphore = static_cast<void*>(&backendSemaphore);
        drawingFlushInfo.finishedProc = [](void *context) {
            DestroySemaphoreInfo::DestroySemaphore(context);
        };
        drawingFlushInfo.finishedContext = destroyInfo;
        cacheSurface->Flush(&drawingFlushInfo);
        grContext_->Submit();
        DestroySemaphoreInfo::DestroySemaphore(destroyInfo);
    } else {
        cacheSurface->FlushAndSubmit(true);
    }
#else
    cacheSurface->FlushAndSubmit(true);
#endif
    }
    nodeDrawable->UpdateBackendTexture();
    RSMainThread::Instance()->PostTask([]() {
        RSMainThread::Instance()->SetIsCachedSurfaceUpdated(true);
    });
    nodeDrawable->SetCacheSurfaceProcessedStatus(CacheProcessStatus::DONE);
    nodeDrawable->SetCacheSurfaceNeedUpdated(true);

    RSSubThreadManager::Instance()->NodeTaskNotify(param->GetId());

    RSMainThread::Instance()->RequestNextVSync();

    // mark nodedrawable can release
    RSUifirstManager::Instance().AddProcessDoneNode(param->GetId());
    doingCacheProcessNum--;

    // uifirst_debug dump img
    std::string pidstring = nodeDrawable->GetDebugInfo();
    RSBaseRenderUtil::WriteCacheImageRenderNodeToPng(cacheSurface, pidstring);
}
#endif

std::shared_ptr<Drawing::GPUContext> RSSubThread::CreateShareGrContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        CreateShareEglContext();
        auto gpuContext = std::make_shared<Drawing::GPUContext>();
        Drawing::GPUContextOptions options;
        auto handler = std::make_shared<MemoryHandler>();
        auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        auto size = glesVersion ? strlen(glesVersion) : 0;
        handler->ConfigureContext(&options, glesVersion, size);

        if (!gpuContext->BuildFromGL(options)) {
            RS_LOGE("nullptr gpuContext is null");
            return nullptr;
        }
        return gpuContext;
    }
#endif

#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        auto gpuContext = std::make_shared<Drawing::GPUContext>();
        Drawing::GPUContextOptions options;
        auto handler = std::make_shared<MemoryHandler>();
        std::string vulkanVersion = RsVulkanContext::GetSingleton().GetVulkanVersion();
        auto size = vulkanVersion.size();
        handler->ConfigureContext(&options, vulkanVersion.c_str(), size);
        if (!gpuContext->BuildFromVK(RsVulkanContext::GetSingleton().GetGrVkBackendContext(), options)) {
            RS_LOGE("nullptr gpuContext is null");
            return nullptr;
        }
        return gpuContext;
    }
#endif
    return nullptr;
}

void RSSubThread::ResetGrContext()
{
    RS_TRACE_NAME_FMT("subthread ResetGrContext release resource");
    if (grContext_ == nullptr) {
        return;
    }
    grContext_->FlushAndSubmit(true);
    grContext_->FreeGpuResources();
}

void RSSubThread::ThreadSafetyReleaseTexture()
{
    grContext_->FreeGpuResources();
}

void RSSubThread::ReleaseSurface()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (tmpSurfaces_.size() > 0) {
        auto tmp = tmpSurfaces_.front();
        tmpSurfaces_.pop();
        tmp = nullptr;
    }
}

void RSSubThread::AddToReleaseQueue(std::shared_ptr<Drawing::Surface>&& surface)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tmpSurfaces_.push(std::move(surface));
}

MemoryGraphic RSSubThread::CountSubMem(int pid)
{
    MemoryGraphic memoryGraphic;
    PostSyncTask([&pid, &memoryGraphic, this]() {
        memoryGraphic = MemoryManager::CountPidMemory(pid, grContext_.get());
    });
    return memoryGraphic;
}
}
