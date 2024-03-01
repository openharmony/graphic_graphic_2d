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

#include <mutex>
#define EGL_EGLEXT_PROTOTYPES
#include "rs_filter_sub_thread.h"

#include <string>

#include "GLES3/gl3.h"
#include "include/core/SkCanvas.h"
#include "memory/rs_memory_manager.h"
#include "memory/rs_tag_tracker.h"
#include "rs_trace.h"

#include "common/rs_optional_trace.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_visitor.h"
#ifdef RES_SCHED_ENABLE
#include "res_sched_client.h"
#include "res_type.h"
#endif

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif // RS_ENABLE_VK

namespace OHOS::Rosen {
namespace {
#ifdef RES_SCHED_ENABLE
const uint32_t RS_SUB_QOS_LEVEL = 7;
constexpr const char* RS_BUNDLE_NAME = "render_service";
#endif
} // namespace
RSFilterSubThread::~RSFilterSubThread()
{
    RS_LOGI("~RSSubThread():%{public}d", threadIndex_);
    PostTask([this]() { DestroyShareEglContext(); });
}

void RSFilterSubThread::StartColorPicker()
{
    RS_LOGI("RSColorPickerSubThread::Start():%{public}d", threadIndex_);
    std::string name = "RSColorPickerSubThread" + std::to_string(threadIndex_);
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
    RSColorPickerCacheTask::postColorPickerTask = [this](std::weak_ptr<RSColorPickerCacheTask> task) {
        auto colorPickerTask = task.lock();
        if (RSMainThread::Instance()->GetNoNeedToPostTask()) {
            colorPickerTask->SetStatus(CacheProcessStatus::WAITING);
            return;
        }
        PostTask([this, task]() { ColorPickerRenderCache(task); });
    };
#ifdef IS_OHOS
    RSColorPickerCacheTask::saveImgAndSurToRelease =
        [this](std::shared_ptr<Drawing::Image>&& cacheImage, std::shared_ptr<Drawing::Surface>&& cacheSurface,
            std::shared_ptr<OHOS::AppExecFwk::EventHandler> initHandler,
            std::weak_ptr<std::atomic<bool>> waitRelease,
            std::weak_ptr<std::mutex> grBackendTextureMutex) {
        SaveAndReleaseCacheResource(std::move(cacheImage), std::move(cacheSurface),
            initHandler, waitRelease, grBackendTextureMutex);
    };
#endif
}

void RSFilterSubThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

#ifdef IS_OHOS
void RSFilterSubThread::AddToReleaseQueue(std::shared_ptr<Drawing::Image>&& cacheImage,
    std::shared_ptr<Drawing::Surface>&& cacheSurface,
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> initHandler)
{
    if (initHandler != nullptr) {
        auto runner = initHandler->GetEventRunner();
        std::string handlerName = runner->GetRunnerThreadName();
        handlerMap_[handlerName] = initHandler;
        if (tmpImageResources_.find(handlerName) != tmpImageResources_.end()) {
            tmpImageResources_[handlerName].push(std::move(cacheImage));
        } else {
            std::queue<std::shared_ptr<Drawing::Image>> imageQueue;
            imageQueue.push(std::move(cacheImage));
            tmpImageResources_[handlerName] = imageQueue;
        }
    }
    tmpSurfaceResources_.push(std::move(cacheSurface));
}

void RSFilterSubThread::PreAddToReleaseQueue(std::shared_ptr<Drawing::Image>&& cacheImage,
    std::shared_ptr<Drawing::Surface>&& cacheSurface,
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> initHandler,
    std::weak_ptr<std::mutex> grBackendTextureMutex)
{
    ROSEN_LOGD("RSFilterSubThread::AddToReleaseQueue");
    std::lock_guard<std::mutex> lock(mutex_);
    auto backendTextureMutex = grBackendTextureMutex.lock();
    if (backendTextureMutex != nullptr) {
        std::unique_lock<std::mutex> lock(*backendTextureMutex);
        AddToReleaseQueue(std::move(cacheImage), std::move(cacheSurface), initHandler);
    } else {
        AddToReleaseQueue(std::move(cacheImage), std::move(cacheSurface), initHandler);
    }
}

void RSFilterSubThread::ResetWaitRelease(std::weak_ptr<std::atomic<bool>> waitRelease)
{
    auto wait = waitRelease.lock();
    if (wait != nullptr) {
        wait->store(false);
    }
}

void RSFilterSubThread::ReleaseSurface(std::weak_ptr<std::atomic<bool>> waitRelease)
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (tmpSurfaceResources_.size() > 0) {
        auto tmp = tmpSurfaceResources_.front();
        tmpSurfaceResources_.pop();
        if (tmp == nullptr) {
            ResetWaitRelease(waitRelease);
            return;
        }
        tmp = nullptr;
    }
    ResetWaitRelease(waitRelease);
}

void RSFilterSubThread::ReleaseImage(std::queue<std::shared_ptr<Drawing::Image>>& queue,
    std::weak_ptr<std::atomic<bool>> waitRelease)
{
    while (queue.size() > 0) {
        auto tmp = queue.front();
        queue.pop();
        if (tmp == nullptr) {
            ResetWaitRelease(waitRelease);
            return;
        }
        tmp.reset();
    }
    PostTask([this, waitRelease]() { ReleaseSurface(waitRelease); });
}

void RSFilterSubThread::PreReleaseImage(std::queue<std::shared_ptr<Drawing::Image>>& queue,
    std::weak_ptr<std::atomic<bool>> waitRelease,
    std::weak_ptr<std::mutex> grBackendTextureMutex)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto backendTextureMutex = grBackendTextureMutex.lock();
    if (backendTextureMutex != nullptr) {
        std::unique_lock<std::mutex> lock(*backendTextureMutex);
        ReleaseImage(queue, waitRelease);
    } else {
        ReleaseImage(queue, waitRelease);
    }
}

void RSFilterSubThread::ReleaseImageAndSurfaces(std::weak_ptr<std::atomic<bool>> waitRelease,
    std::weak_ptr<std::mutex> grBackendTextureMutex)
{
    ROSEN_LOGD("ReleaseImageAndSurfaces tmpImageResources_.size %{public}d",
        static_cast<int>(tmpImageResources_.size()));
    for (auto& item : tmpImageResources_) {
        auto& initHandler = handlerMap_[item.first];
        auto& imageQueue = item.second;
        if (imageQueue.size() != 0) {
            initHandler->PostTask(
                [this, &imageQueue, waitRelease, grBackendTextureMutex]() {
                    PreReleaseImage(imageQueue, waitRelease, grBackendTextureMutex);
                }, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        }
    }
}

void RSFilterSubThread::SaveAndReleaseCacheResource(std::shared_ptr<Drawing::Image>&& cacheImage,
    std::shared_ptr<Drawing::Surface>&& cacheSurface,
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> initHandler,
    std::weak_ptr<std::atomic<bool>> waitRelease,
    std::weak_ptr<std::mutex> grBackendTextureMutex)
{
    PreAddToReleaseQueue(std::move(cacheImage), std::move(cacheSurface), initHandler, grBackendTextureMutex);
    ReleaseImageAndSurfaces(waitRelease, grBackendTextureMutex);
}
#endif

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

void RSFilterSubThread::DestroyShareEglContext()
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

void RSFilterSubThread::ColorPickerRenderCache(std::weak_ptr<RSColorPickerCacheTask> colorPickerTask)
{
    RS_TRACE_NAME("ColorPickerRenderCache");
    auto task = colorPickerTask.lock();
    if (!task) {
        RS_LOGE("Color picker task is null");
        return;
    }
    if (grContext_ == nullptr) {
        grContext_ = CreateShareGrContext();
    }
    if (grContext_ == nullptr) {
        RS_LOGE("Color picker grContext is null");
        return;
    }
    if (!task->InitSurface(grContext_.get())) {
        RS_LOGE("Color picker initSurface failed");
        return;
    }
    if (!task->Render()) {
        RS_LOGE("Color picker render failed");
    }
    RSMainThread::Instance()->RequestNextVSync();
    RSMainThread::Instance()->SetColorPickerForceRequestVsync(true);
}

std::shared_ptr<Drawing::GPUContext> RSFilterSubThread::CreateShareGrContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        CreateShareEglContext();
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
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
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

void RSFilterSubThread::ResetGrContext()
{
    RS_TRACE_NAME("ResetGrContext release resource");
    if (grContext_ == nullptr) {
        return;
    }
    grContext_->FlushAndSubmit(true);
    grContext_->FreeGpuResources();
}
} // namespace OHOS::Rosen
