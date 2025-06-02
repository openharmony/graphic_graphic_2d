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

#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "GLES3/gl3.h"
#include "include/core/SkCanvas.h"

#include "memory/rs_memory_graphic.h"
#include "memory/rs_memory_manager.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/main_thread/rs_uni_render_visitor.h"
#include "rs_trace.h"
#include "utils/graphic_coretrace.h"

#ifdef RES_SCHED_ENABLE
#include "qos.h"
#endif

namespace OHOS::Rosen {
RSSubThread::~RSSubThread()
{
    RS_LOGI("~RSSubThread():%{public}d", threadIndex_);
    PostSyncTask([this]() {
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
        auto ret = OHOS::QOS::SetThreadQos(OHOS::QOS::QosLevel::QOS_USER_INTERACTIVE);
        RS_LOGI("RSSubThread%{public}d: SetThreadQos retcode = %{public}d", threadIndex_, ret);
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
    std::vector<std::pair<NodeId, std::string>> nodeTags;
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([&nodeTags](const std::shared_ptr<RSSurfaceRenderNode> node) {
        if (node == nullptr) {
            return;
        }
        std::string name = node->GetName() + " " + std::to_string(node->GetId());
        nodeTags.push_back({node->GetId(), name});
    });
    PostSyncTask([&log, &nodeTags, this]() {
        MemoryManager::DumpDrawingGpuMemory(log, grContext_.get(), nodeTags);
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

void RSSubThread::DrawableCache(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable)
{
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::RS_RSSUBTHREAD_DRAWABLECACHE);
    if (grContext_ == nullptr) {
        grContext_ = CreateShareGrContext();
        if (grContext_ == nullptr) {
            RS_LOGE("RSSubThread::RenderCache DrawableCache grContext is null");
            return;
        }
    }
    if (!nodeDrawable) {
        return;
    }

    NodeId nodeId = nodeDrawable->GetId();
    auto& rsSubThreadCache = nodeDrawable->GetRsSubThreadCache();
    rsSubThreadCache.SetSubThreadSkip(false);

    RS_TRACE_NAME_FMT("RSSubThread::DrawableCache [%s] id:[%" PRIu64 "]", nodeDrawable->GetName().c_str(), nodeId);
    RSTagTracker tagTracker(grContext_, nodeId, RSTagTracker::TAGTYPE::TAG_SUB_THREAD, nodeDrawable->GetName());

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(nodeDrawable->GetRenderParams().get());
    if (UNLIKELY(!surfaceParams)) {
        return;
    }
    // set cur firstlevel node in subThread
    RSUiFirstProcessStateCheckerHelper stateCheckerHelper(
        surfaceParams->GetFirstLevelNodeId(), surfaceParams->GetUifirstRootNodeId(), surfaceParams->GetId());
    rsSubThreadCache.SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);
    if (rsSubThreadCache.HasCachedTexture() &&
        rsSubThreadCache.GetTaskFrameCount() != RSUniRenderThread::Instance().GetFrameCount()) {
        RS_TRACE_NAME_FMT("subthread skip id:%llu", nodeId);
        RS_LOGI("uifirst subthread skip id:%{public}" PRIu64 " name:%{public}s postFrame:%{public}" PRIu64
            " curFrame:%{public}" PRIu64, nodeId, nodeDrawable->GetName().c_str(), rsSubThreadCache.GetTaskFrameCount(),
            RSUniRenderThread::Instance().GetFrameCount());
        rsSubThreadCache.SetCacheSurfaceProcessedStatus(CacheProcessStatus::SKIPPED);
        rsSubThreadCache.SetSubThreadSkip(true);
        rsSubThreadCache.ProcessSurfaceSkipCount();
        doingCacheProcessNum_--;
        RSSubThreadManager::Instance()->NodeTaskNotify(nodeId);
        RSUifirstManager::Instance().AddProcessSkippedNode(nodeId);
        RSMainThread::Instance()->RequestNextVSync("subthreadSkipped");
        return;
    }
    DrawableCacheWithSkImage(nodeDrawable);

    RSMainThread::Instance()->PostTask([]() {
        RSMainThread::Instance()->SetIsCachedSurfaceUpdated(true);
    });

    rsSubThreadCache.SetCacheSurfaceProcessedStatus(CacheProcessStatus::DONE);
    rsSubThreadCache.SetCacheSurfaceNeedUpdated(true);
    rsSubThreadCache.ResetSurfaceSkipCount();

    RSSubThreadManager::Instance()->NodeTaskNotify(nodeId);

    RSMainThread::Instance()->RequestNextVSync("subthreadDone");

    // mark nodedrawable can release
    RSUifirstManager::Instance().AddProcessDoneNode(nodeId);
    doingCacheProcessNum_--;
    UpdateGpuMemoryStatistics();
}

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
        MemoryManager::SetGpuMemoryLimit(gpuContext.get());
        return gpuContext;
    }
#endif
    return nullptr;
}

void RSSubThread::DrawableCacheWithSkImage(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable)
{
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSUBTHREAD_DRAWABLECACHEWITHSKIMAGE);
    if (!nodeDrawable) {
        RS_LOGE("RSSubThread::DrawableCacheWithSkImage nodeDrawable is nullptr");
        return;
    }
    auto& rsSubThreadCache = nodeDrawable->GetRsSubThreadCache();
    auto cacheSurface = rsSubThreadCache.GetCacheSurface(threadIndex_);
    bool isHdrSurface = false;
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(nodeDrawable->GetRenderParams().get());
    if (surfaceParams != nullptr) {
        isHdrSurface = surfaceParams->GetHDRPresent();
    }
    bool isScRGBEnable = RSSystemParameters::IsNeedScRGBForP3(rsSubThreadCache.GetTargetColorGamut()) &&
        RSUifirstManager::Instance().GetUiFirstSwitch();
    bool isNeedFP16 = isHdrSurface || isScRGBEnable;
    bool bufferFormatNeedUpdate = rsSubThreadCache.BufferFormatNeedUpdate(cacheSurface, isNeedFP16);
    if (!cacheSurface || rsSubThreadCache.NeedInitCacheSurface(surfaceParams) || bufferFormatNeedUpdate) {
        DrawableV2::RsSubThreadCache::ClearCacheSurfaceFunc func = &RSUniRenderUtil::ClearNodeCacheSurface;
        rsSubThreadCache.InitCacheSurface(grContext_.get(), nodeDrawable, func, threadIndex_, isNeedFP16);
        cacheSurface = rsSubThreadCache.GetCacheSurface(threadIndex_);
    }

    if (!cacheSurface) {
        RS_LOGE("RSSubThread::DrawableCacheWithSkImage cacheSurface is nullptr");
        return;
    }

    auto rscanvas = std::make_shared<RSPaintFilterCanvas>(cacheSurface.get());
    if (rscanvas == nullptr) {
        RS_LOGE("RSSubThread::DrawableCacheWithSkImage canvas is nullptr");
        return;
    }
    SetHighContrastIfEnabled(*rscanvas);
    rscanvas->SetIsParallelCanvas(true);
    rscanvas->SetDisableFilterCache(true);
    rscanvas->SetParallelThreadIdx(threadIndex_);
    rscanvas->SetScreenId(rsSubThreadCache.GetScreenId());
    rscanvas->SetTargetColorGamut(rsSubThreadCache.GetTargetColorGamut());
    rscanvas->SetHdrOn(rsSubThreadCache.GetHDRPresent());
    rscanvas->SetIsDrawingCache(true);
    rscanvas->Save();
    rsSubThreadCache.SubDraw(nodeDrawable.get(), *rscanvas);
    rscanvas->Restore();
    bool optFenceWait = (RSUifirstManager::Instance().GetUiFirstType() == UiFirstCcmType::MULTI &&
        !rsSubThreadCache.IsHighPostPriority()) ? false : true;
    RSUniRenderUtil::OptimizedFlushAndSubmit(cacheSurface, grContext_.get(), optFenceWait);
    rsSubThreadCache.UpdateCacheSurfaceInfo(nodeDrawable);
    rsSubThreadCache.UpdateBackendTexture();
    rsSubThreadCache.SetCacheBehindWindowData(rscanvas->GetCacheBehindWindowData());

    // uifirst_debug dump img, run following commands to grant permissions before dump, otherwise dump maybe fail:
    // 1. hdc shell mount -o rw,remount /
    // 2. hdc shell setenforce 0 # close selinux temporarily
    // 3. hdc shell chmod 0777 /data
    RSBaseRenderUtil::WriteCacheImageRenderNodeToPng(cacheSurface, nodeDrawable->GetName());
}

void RSSubThread::ResetGrContext()
{
    RS_TRACE_NAME_FMT("subthread ResetGrContext release resource");
    if (grContext_ == nullptr) {
        return;
    }
    grContext_->FlushAndSubmit(true);
    grContext_->FreeGpuResources();
    UpdateGpuMemoryStatistics();
}

void RSSubThread::ThreadSafetyReleaseTexture()
{
    if (grContext_ == nullptr) {
        return;
    }
    grContext_->FreeGpuResources();
    UpdateGpuMemoryStatistics();
}

void RSSubThread::ReleaseSurface()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (tmpSurfaces_.size() > 0) {
        auto tmp = tmpSurfaces_.front();
        tmpSurfaces_.pop();
        tmp = nullptr;
    }
    UpdateGpuMemoryStatistics();
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

void RSSubThread::ReleaseCacheSurfaceOnly(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable)
{
    if (!nodeDrawable) {
        return;
    }
    const auto& param = nodeDrawable->GetRenderParams();
    if (!param) {
        return;
    }
    NodeId nodeId = nodeDrawable->GetId();
    RS_TRACE_NAME_FMT("ReleaseCacheSurfaceOnly id:" PRIu64, nodeId);
    RS_LOGI("ReleaseCacheSurfaceOnly id:%{public}" PRIu64, nodeId);
    nodeDrawable->GetRsSubThreadCache().ClearCacheSurfaceOnly();
    UpdateGpuMemoryStatistics();
}

void RSSubThread::SetHighContrastIfEnabled(RSPaintFilterCanvas& canvas)
{
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (renderEngine) {
        canvas.SetHighContrast(renderEngine->IsHighContrastEnabled());
    }
}

void RSSubThread::UpdateGpuMemoryStatistics()
{
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return;
    }
    if (grContext_ == nullptr) {
        return;
    }
    std::unordered_map<pid_t, size_t> gpuMemOfPid;
    grContext_->GetUpdatedMemoryMap(gpuMemOfPid);
    std::lock_guard<std::mutex> lock(memMutex_);
    for (auto& [pid, size] : gpuMemOfPid) {
        gpuMemoryOfPid_[pid] = size;
    }
}
}
