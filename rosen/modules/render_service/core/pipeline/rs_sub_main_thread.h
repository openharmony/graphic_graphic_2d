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
#ifdef RS_ENABLE_GL
#ifndef RS_SUB_MAIN_THREAD_H
#define RS_SUB_MAIN_THREAD_H
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include <EGL/egl.h>
#include "render_context/render_context.h"
#include "rs_render_task_manager.h"

namespace OHOS {
namespace Rosen {
class RSSubMainThread {
public:
    RSSubMainThread();
    virtual ~RSSubMainThread();

    static RSSubMainThread& Instance();

    void InitTaskManager()
    {
        tastManager_.InitTaskManager();
    }

    void SetRenderContext(RenderContext* context)
    {
        renderContext_  = context;
    }

    RenderContext* GetRenderContext()
    {
        return renderContext_;
    }

    uint32_t GetShareContexNum() const
    {
        return eglShareCtxNum;
    }

    EGLContext GetShareEglContex(int32_t index) const
    {
        if ((index >= 0) && (index < eglShareContext.size())) {
            return eglShareContext[index];
        }
        return EGL_NO_CONTEXT;
    }

    void SetFrameWH(int32_t width, int32_t height);
    void CreateEglShareContext();
    void InitializeSubRenderThread(uint32_t subThreadNum);
    void InitializeSubContext(RenderContext* context);
    void StartSubThread();
    void SubMainThread(uint32_t index);
    EGLSurface CreateShareContextPSurface(int32_t width, int32_t height);
    GrContext* CreateShareGrContext(uint32_t index);
    GrContext* CreateShareGrContext(uint32_t index, int32_t width, int32_t height);
    SkSurface* AcquireShareSkSurface(uint32_t index, int32_t width, int32_t height);
    SkSurface* AcquireShareRsSurface(uint32_t index, int32_t width, int32_t height);

    void ManagerCallBackRegister();
    void GetSubContextFuncRegister();
    void CanvasPrepareFuncRegister();
    void SubDrawFuncRegister();
    void FlushAndSubmitRegister();
    bool EnableParallerRendering();
    RSRenderTaskManager tastManager_;
    void ProcessFrameStartFlag();
    void ProcessFrameEndFlag();

private:
    struct ThreadResource {
        int32_t surfaceWidth = 0;
        int32_t surfaceHeight = 0;
        SkSurface* skSurface = nullptr;
        SkCanvas* skCanvas = nullptr;
        RSPaintFilterCanvas* canvas = nullptr;
        std::shared_ptr<RSNodeVisitor> visitor;
        timespec timeStart;
        timespec timeEnd;
        float costing = 0.f;
    };

    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLContext eglContext_ = EGL_NO_CONTEXT;
    EGLConfig config_ = nullptr;
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
    std::mutex mutex_;
    mutable std::condition_variable cvCtx_;
    mutable std::condition_variable cvFrameWH_;
    std::mutex cmdMutex_;
    RenderContext* renderContext_ = nullptr;
    uint32_t eglShareCtxNum = 0;
    std::vector<std::thread*> subRSThreads_;
    std::vector<EGLContext> eglShareContext;
    std::vector<EGLSurface> eglPSurface;
    std::vector<ThreadResource> resources;
    int32_t frameWidth_ = 0;
    int32_t frameHeight_ = 0;
    int32_t surfaceWidth_ = 0;
    int32_t surfaceHeight_ = 0;
    using CreateEglShareContextFunc = EGLContext(*)(EGLDisplay, EGLConfig, EGLContext, const EGLint *);
    std::queue<uint64_t> processFrameTimes_;
    uint64_t processFrameStartTime_;
    uint64_t processFrameTotalTime_;
    uint64_t processFrameAvgTime_;
};
}
}
#endif
#endif
