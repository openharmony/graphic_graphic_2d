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

#ifndef RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_SUB_THREAD_H
#define RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_SUB_THREAD_H

#include <cstdint>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "include/core/SkSurface.h"
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrDirectContext.h"
#else
#include "include/gpu/GrDirectContext.h"
#endif
#ifdef RS_ENABLE_VK
#ifdef USE_M133_SKIA
#include "include/gpu/vk/VulkanBackendContext.h"
#else
#include "include/gpu/vk/GrVkBackendContext.h"
#endif
#endif
#include "pipeline/parallel_render/rs_render_task.h"
#include "render_context/render_context.h"
#include "event_handler.h"

namespace OHOS::Rosen {
class RSSubThread {
public:
    RSSubThread(RenderContext* context, uint32_t threadIndex) : threadIndex_(threadIndex), renderContext_(context) {}
    ~RSSubThread();

    pid_t Start();
    void PostTask(const std::function<void()>& task, const std::string& name = std::string());
    void PostSyncTask(const std::function<void()>& task);
    void RemoveTask(const std::string& name);
    void DrawableCache(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable);
    void ReleaseSurface();
    void ReleaseCacheSurfaceOnly(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable);
    void AddToReleaseQueue(std::shared_ptr<Drawing::Surface>&& surface);
    void ResetGrContext();
    void ThreadSafetyReleaseTexture();
    void DumpMem(DfxString& log);
    MemoryGraphic CountSubMem(int pid);
    float GetAppGpuMemoryInMB();
    unsigned int GetDoingCacheProcessNum()
    {
        return doingCacheProcessNum_.load();
    }
    inline void DoingCacheProcessNumInc()
    {
        doingCacheProcessNum_++;
    }
    void DrawableCacheWithSkImage(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable);
    std::shared_ptr<Drawing::GPUContext> GetGrContext() const
    {
        return grContext_;
    }
    void UpdateGpuMemoryStatistics();
    std::unordered_map<pid_t, size_t> GetGpuMemoryOfPid()
    {
        std::lock_guard<std::mutex> lock(memMutex_);
        return gpuMemoryOfPid_;
    }
    void ErasePidOfGpuMemory(pid_t pid)
    {
        std::lock_guard<std::mutex> lock(memMutex_);
        gpuMemoryOfPid_.erase(pid);
    }

private:
    void CreateShareEglContext();
    void DestroyShareEglContext();
    std::shared_ptr<Drawing::GPUContext> CreateShareGrContext();
    void SetHighContrastIfEnabled(RSPaintFilterCanvas& canvas);
    NodeId GetSubAppNodeId(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable,
        RSSurfaceRenderParams* surfaceParams);
    bool CheckValid(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable);

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    uint32_t threadIndex_ = UNI_RENDER_THREAD_INDEX;
    RenderContext *renderContext_ = nullptr;
#ifdef RS_ENABLE_GL
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
#endif
    std::shared_ptr<Drawing::GPUContext> grContext_ = nullptr;
    std::mutex mutex_;
    std::queue<std::shared_ptr<Drawing::Surface>> tmpSurfaces_;
    std::atomic<unsigned int> doingCacheProcessNum_ = 0;
    std::unordered_map<pid_t, size_t> gpuMemoryOfPid_;
    std::mutex memMutex_;
};
}
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_SUB_THREAD_H