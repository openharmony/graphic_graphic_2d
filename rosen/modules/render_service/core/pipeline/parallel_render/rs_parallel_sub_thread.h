/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_SUB_THREAD_H
#define RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_SUB_THREAD_H

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <thread>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "include/core/SkImage.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "pipeline/parallel_render/rs_render_task.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#ifdef NEW_RENDER_CONTEXT
#include "render_context_base.h"
#include "include/gpu/gl/GrGLInterface.h"
#else
#include "render_context/render_context.h"
#endif
#include "pipeline/rs_base_render_engine.h"

namespace OHOS {
namespace Rosen {
class RSUniRenderVisitor;
class RSDisplayRenderNode;
enum class ParallelRenderType;

class RSParallelSubThread {
public:
    explicit RSParallelSubThread(int threadIndex);
#ifdef NEW_RENDER_CONTEXT
    RSParallelSubThread(std::shared_ptr<RenderContextBase> context, ParallelRenderType renderType,
        int threadIndex);
#else
    RSParallelSubThread(RenderContext *context, ParallelRenderType renderType, int threadIndex);
#endif
    ~RSParallelSubThread();

    void StartSubThread();
    void WaitTaskSync();
    void SetMainVisitor(RSUniRenderVisitor *mainVisitor);
    bool GetRenderFinish();
    void SetSuperTask(std::unique_ptr<RSSuperRenderTask> superRenderTask);
    void SetCompositionTask(std::unique_ptr<RSCompositionTask> compositionTask);
    EGLContext GetSharedContext() const;
    std::shared_ptr<Drawing::Surface> GetDrawingSurface() const;
    std::shared_ptr<Drawing::Image> GetTexture() const;
    bool WaitReleaseFence();
    std::shared_ptr<RSUniRenderVisitor> GetUniVisitor() const
    {
        return visitor_;
    }
    void WaitSubMainThreadEnd()
    {
        subThread_->join();
    }

private:
    void MainLoop();
    void MainLoopHandlePrepareTask();
    void StartRender();
    void InitSubThread();
    void Render();
    void InitUniVisitor();
    void Flush();
    void CreateResource();
    void CreatePbufferSurface();
    void CreateShareEglContext();
    void StartPrepare();
    void Prepare();
    void CalcCost();
    void StartComposition();
    void Composition();
    std::shared_ptr<Drawing::GPUContext> CreateShareGPUContext();
    void AcquireSubDrawingSurface(int width, int height);
    bool FlushForRosenDrawing();

    uint32_t threadIndex_;
    int surfaceWidth_ = 0;
    int surfaceHeight_ = 0;
    std::shared_ptr<Drawing::GPUContext> drContext_ = nullptr;
    std::shared_ptr<Drawing::Surface> surface_ = nullptr;
    Drawing::Canvas *drCanvas_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> canvas_ = nullptr;
    std::shared_ptr<RSUniRenderVisitor> visitor_;
    std::shared_ptr<RSUniRenderVisitor> compositionVisitor_ = nullptr;
    std::shared_ptr<RSBaseRenderEngine> processorRenderEngine_ = nullptr;
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
    EGLSurface eglPSurface_ = EGL_NO_SURFACE;
    std::function<void()> mainLoop_;
    std::thread *subThread_;
    std::condition_variable cvFlush_;
    std::mutex flushMutex_;
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RenderContextBase> renderContext_ = nullptr;
#else
    RenderContext *renderContext_ = nullptr;
#endif
    std::unique_ptr<RSSuperRenderTask> threadTask_;
    std::unique_ptr<RSSuperRenderTask> cacheThreadTask_;
    std::unique_ptr<RSCompositionTask> compositionTask_ = nullptr;

    RSUniRenderVisitor *mainVisitor_ = nullptr;
    ParallelRenderType renderType_;
    std::shared_ptr<Drawing::Image> texture_;
    EGLSyncKHR eglSync_ = EGL_NO_SYNC_KHR;

    // Use for Vulkan
    std::shared_ptr<RSDisplayRenderNode> displayNode_ = nullptr;
    std::unique_ptr<RSRenderFrame> renderFrame_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_SUB_THREAD_H