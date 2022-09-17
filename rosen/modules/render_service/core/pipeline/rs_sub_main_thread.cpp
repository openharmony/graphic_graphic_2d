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
#include "rs_sub_main_thread.h"
#include <ctime>
#include <pthread.h>
#include <string>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <thread>
#include "rs_trace.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "platform/common/rs_log.h"
namespace OHOS {
namespace Rosen {
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;
const int STENCIL_BUFFER_SIZE = 8;
constexpr int SUB_RENDER_CORE_LEVEL = 400;
constexpr uint64_t FRAME_COUNT = 100;
constexpr uint64_t FRAME_PROCESS_TIME = 16;
RSSubMainThread::RSSubMainThread()
    : eglDisplay_(EGL_NO_DISPLAY),
      eglContext_(EGL_NO_CONTEXT),
      config_(nullptr),
      renderContext_(nullptr),
      eglShareCtxNum(0),
      frameWidth_(0),
      frameHeight_(0),
      surfaceWidth_(0),
      surfaceHeight_(0),
      processFrameStartTime_(0),
      processFrameTotalTime_(0),
      processFrameAvgTime_(0)
{
}

RSSubMainThread::~RSSubMainThread()
{
    eglDisplay_ = EGL_NO_DISPLAY;
    eglContext_ = EGL_NO_CONTEXT;
    config_ = nullptr;
    renderContext_ = nullptr;
    eglShareCtxNum = 0;
    frameWidth_ = 0;
    frameHeight_ = 0;
    surfaceWidth_ = 0;
    surfaceHeight_ = 0;
}

RSSubMainThread& RSSubMainThread::Instance()
{
    static RSSubMainThread subRenderThread;
    return subRenderThread;
}

void RSSubMainThread::ProcessFrameStartFlag()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    processFrameStartTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

void RSSubMainThread::ProcessFrameEndFlag()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    auto costing = std::chrono::duration_cast<std::chrono::milliseconds>(now).count() - 
        processFrameStartTime_;
    if (processFrameTimes_.size() == FRAME_COUNT) {
        processFrameTimes_.pop();
    }
    processFrameTimes_.push(costing);
    processFrameTotalTime_ += costing;
    processFrameAvgTime_ = processFrameTotalTime_ / static_cast<uint64_t>(processFrameTimes_.size());
}

bool RSSubMainThread::EnableParallerRendering()
{
    return RSInnovation::GetParallelRenderingEnabled() && (eglShareCtxNum > 0) && 
        tastManager_.EnableParallerRendering() && (processFrameAvgTime_ > FRAME_PROCESS_TIME);
}

void RSSubMainThread::SetFrameWH(int32_t width, int32_t height)
{
    std::unique_lock<std::mutex> lock(mutex_);
    frameWidth_ = width;
    frameHeight_ = height;
    cvFrameWH_.notify_all();
}

void RSSubMainThread::CreateEglShareContext()
{
    if (renderContext_ == nullptr || eglContext_ == EGL_NO_CONTEXT) {
        RS_LOGE("EGL Context has not initialized");
        return;
    }

    if (eglShareCtxNum <= 0) {
        RS_LOGE("EGL context has no share context");
        return;
    }

    std::vector<EGLSurface>(eglShareCtxNum, EGL_NO_SURFACE).swap(eglPSurface);
    static const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE };
    for (uint32_t i = 0; i < eglShareCtxNum; ++i) {
        if (RSInnovation::_s_createEglShareContext) {
            auto CreateEglShareContextFuncSo = (CreateEglShareContextFunc)RSInnovation::_s_createEglShareContext;
            auto eglShareCtx = (*CreateEglShareContextFuncSo)(eglDisplay_, config_, eglContext_, context_attribs);
            if (eglShareCtx == EGL_NO_CONTEXT) {
                RS_LOGE("Failed to create egl share context %{public}x", eglGetError());
                return;
            }
            RS_LOGI("Create EGL share[%{public}d] context addr : %{public}p \n", i, eglShareCtx);
            eglShareContext.push_back(eglShareCtx);
        }
    }
    std::unique_lock<std::mutex> lock(mutex_);
    cvCtx_.notify_all();
    RS_LOGI("Create EGL share context successfully\n");
}

void RSSubMainThread::InitializeSubRenderThread(uint32_t subThreadNum)
{
    if (!RSInnovation::GetParallelRenderingEnabled()) return;
    eglShareCtxNum = subThreadNum;
    std::vector<std::thread*>(eglShareCtxNum, nullptr).swap(subRSThreads_);
    tastManager_.InitTaskManager();
    tastManager_.SetSubRenderThreadNum(subThreadNum);
}

void RSSubMainThread::InitializeSubContext(RenderContext* context)
{
    if (!RSInnovation::GetParallelRenderingEnabled()) return;
    renderContext_ = context;
    eglDisplay_ = renderContext_->GetEGLDisplay();
    config_ = renderContext_->GetEGLConfig();
    eglContext_ = renderContext_->GetEGLContext();
    colorSpace_ = renderContext_->GetColorSpace();
    CreateEglShareContext();
}

void RSSubMainThread::StartSubThread()
{
    if (!RSInnovation::GetParallelRenderingEnabled()) return;
    ManagerCallBackRegister();
    for (uint32_t i = 0; i < eglShareCtxNum; ++i) {
        if (subRSThreads_[i] == nullptr) {
            resources.emplace_back();
            subRSThreads_[i] = new std::thread(&RSSubMainThread::SubMainThread, this, i);
        }
    }
}

void RSSubMainThread::ManagerCallBackRegister()
{
    GetSubContextFuncRegister();
    CanvasPrepareFuncRegister();
    SubDrawFuncRegister();
    FlushAndSubmitRegister();
}

void RSSubMainThread::GetSubContextFuncRegister()
{
    auto getSubContext = [this](uint32_t index) {
        std::unique_lock<std::mutex> lock(mutex_);
        cvCtx_.wait(lock, [this, index] { return GetShareEglContex(index) != EGL_NO_CONTEXT;});
        cvFrameWH_.wait(lock, [this] { return frameWidth_ != 0 && frameHeight_ != 0;});
        lock.unlock();
        pthread_setname_np(pthread_self(), "SubMainThread");
        // Thread priority number interval: [-20, 20]; lower number refers to a higher thread priority.
        setpriority(PRIO_PROCESS, 0, -8);
        struct sched_param param = {0};
        param.sched_priority = 2;
        (void)sched_setscheduler(0, SCHED_FIFO, &param);
    };
    if (RSInnovation::_s_getSubContextRegister) {
        using GetSubContextFunc = void (*)(std::function<void(uint32_t)>);
        auto GetSubContextFuncRegister = (GetSubContextFunc)RSInnovation::_s_getSubContextRegister;
        (*GetSubContextFuncRegister)(getSubContext);
    }
}

void RSSubMainThread::CanvasPrepareFuncRegister()
{
    auto canvasPrepare = [this](uint32_t i) {
        if (resources[i].surfaceWidth != frameWidth_ || resources[i].surfaceHeight != frameHeight_) {
            resources[i].surfaceWidth = frameWidth_;
            resources[i].surfaceHeight = frameHeight_;
            if (resources[i].skSurface) {
                delete resources[i].skSurface;
                resources[i].skSurface = nullptr;
            }
            if (resources[i].skCanvas) {
                delete resources[i].skCanvas;
                resources[i].skCanvas = nullptr;
            }
            if (resources[i].canvas) {
                delete resources[i].canvas;
                resources[i].canvas = nullptr;
            }
            resources[i].skSurface = AcquireShareSkSurface(i, resources[i].surfaceWidth, resources[i].surfaceHeight);
            resources[i].skCanvas = resources[i].skSurface->getCanvas();
            resources[i].canvas = new RSPaintFilterCanvas(resources[i].skSurface->getCanvas());
            resources[i].visitor = std::make_shared<RSUniRenderVisitor>(resources[i].canvas);
        }
    };
    if (RSInnovation::_s_canvasPrepareRegister) {
        using CanvasPrepareFunc = void (*)(std::function<void(uint32_t)>);
        auto canvasPrepareFuncRegister = (CanvasPrepareFunc)RSInnovation::_s_canvasPrepareRegister;
        (*canvasPrepareFuncRegister)(canvasPrepare);
    }
}

void RSSubMainThread::SubDrawFuncRegister()
{
    auto subDraw = [this](uint32_t i) -> int32_t {
        if (RSInnovation::_s_setCoreLevel) {
            using SetCoreLevelFunc = void(*)(int);
            auto SetCoreLevel = (SetCoreLevelFunc)RSInnovation::_s_setCoreLevel;
            (*SetCoreLevel)(SUB_RENDER_CORE_LEVEL);
        }
        auto task = tastManager_.GetTask(i);
        auto threadTrace = "LoadBalance" + std::to_string(i);
        RS_TRACE_BEGIN(threadTrace);
        int32_t taskIdx = task->GetSuperTaskIdx();
        resources[i].canvas->clear(SK_ColorTRANSPARENT);
        while (task->GetTaskSize() > 0) {
            RS_TRACE_BEGIN("LoadCosting");
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &resources[i].timeStart);
            auto node = task->GetSurfaceNode();
            if (node != nullptr) {
                node->Process(resources[i].visitor);
                clock_gettime(CLOCK_THREAD_CPUTIME_ID, &resources[i].timeEnd);
                resources[i].costing = (resources[i].timeEnd.tv_sec * 1000.0f + resources[i].timeEnd.tv_nsec * 1e-6) -
                            (resources[i].timeStart.tv_sec * 1000.0f + resources[i].timeStart.tv_nsec * 1e-6);
                tastManager_.SetSubThreadRenderLoad(i, node->GetId(), resources[i].costing);
            }
            RS_TRACE_END();
        }
        return taskIdx;
    };
    if (RSInnovation::_s_subDrawRegister) {
        using SubDrawFunc = void (*)(std::function<int32_t(uint32_t)>);
        auto subDrawFuncRegister = (SubDrawFunc)RSInnovation::_s_subDrawRegister;
        (*subDrawFuncRegister)(subDraw);
    }
}

void RSSubMainThread::FlushAndSubmitRegister()
{
    auto subDraw = [this](uint32_t i, int32_t taskIdx) {
        RS_TRACE_BEGIN("SubThreadFlush");
        resources[i].skSurface->flush();
        RS_TRACE_END();
        tastManager_.CreateTaskFence(taskIdx);
        sk_sp<SkImage> texture = resources[i].skSurface->makeImageSnapshot();
        tastManager_.SaveTexture(taskIdx, texture);
        resources[i].skCanvas->discard();
        RS_TRACE_END();
    };
    if (RSInnovation::_s_flushAndSubmitFuncRegister) {
        using FlushAndSubmitFunc = void (*)(std::function<void(uint32_t, int32_t)>);
        auto flushAndSubmitFuncRegister = (FlushAndSubmitFunc)RSInnovation::_s_flushAndSubmitFuncRegister;
        (*flushAndSubmitFuncRegister)(subDraw);
    }
}

void RSSubMainThread::SubMainThread(uint32_t index)
{
    if (RSInnovation::_s_subMainThread) {
        using SubMainThreadFunc = void(*)(uint32_t);
        auto subThread = (SubMainThreadFunc)RSInnovation::_s_subMainThread;
        (*subThread)(index);
    }
}

EGLSurface RSSubMainThread::CreateShareContextPSurface(int32_t width, int32_t height)
{
    if (RSInnovation::_s_createShareContextPSurface) {
        using CreateShareContextPSurfaceFunc = EGLSurface(*)(int32_t, int32_t, EGLDisplay, EGLConfig);
        auto CreateShareContextPSurfaceFuncSo =
            (CreateShareContextPSurfaceFunc)RSInnovation::_s_createShareContextPSurface;
        return (*CreateShareContextPSurfaceFuncSo)(width, height, eglDisplay_, config_);
    } else {
        return nullptr;
    }
}

GrContext* RSSubMainThread::CreateShareGrContext(uint32_t index)
{
    if (RSInnovation::_s_createSharedContext) {
        using CreateShareGrContextFunc = bool (*)(uint32_t, uint32_t, std::vector<EGLContext>&, EGLDisplay);
        auto CreateSharedContext = (CreateShareGrContextFunc)RSInnovation::_s_createSharedContext;

        if (!(*CreateSharedContext)(index, eglShareCtxNum, eglShareContext, eglDisplay_)) {
            return nullptr;
        }
    } else {
        return nullptr;
    }
    const GrGLInterface *grGlInterface = GrGLCreateNativeInterface();
    sk_sp<const GrGLInterface> glInterface(grGlInterface);
    if (glInterface.get() == nullptr) {
        RS_LOGE("CreateShareGrContext failed");
        return nullptr;
    }
    
    GrContextOptions options = { };
    options.fGpuPathRenderers = GpuPathRenderers::kAll & ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;
    
    sk_sp<GrContext> grContext = GrContext::MakeGL(std::move(glInterface), options);
    if (grContext == nullptr) {
        RS_LOGE("nullptr grContext is null");
        return nullptr;
    }

    RS_LOGI("Create grContext successfully!!!, grContext:[%{public}p], index:[%{public}d]", grContext.get(), index);
    return grContext.release();
}

GrContext* RSSubMainThread::CreateShareGrContext(uint32_t index, int32_t width, int32_t height)
{
    if ((index >= eglShareCtxNum) || (index < 0) || (width <= 0) || (height <= 0)) {
        RS_LOGE("ShareContext index :[%d] is out of  eglShareCtxNum :[%d]", index, eglShareCtxNum);
        return nullptr;
    }

    EGLContext shareCtx = eglShareContext[index];
    if (shareCtx == EGL_NO_CONTEXT) {
        RS_LOGE("egl share context :[%{public}d} not create!\n", index);
        return nullptr;
    }
    RS_LOGI("egl share context:[%{public}p]", shareCtx);

    EGLDisplay pSurface = CreateShareContextPSurface(width, height);
    if (pSurface == EGL_NO_SURFACE) {
        RS_LOGE("Failed to create pSurface!!! %{public}x", eglGetError());
        return nullptr;
    }
    RS_LOGI("egl pbuffer Surface:[%{public}p]", pSurface);
    
    eglPSurface[index] = pSurface;
    if (!eglMakeCurrent(eglDisplay_, pSurface, pSurface, shareCtx)) {
        RS_LOGE("Failed to make current on pSurface %{public}p, error is %{public}x", pSurface, eglGetError());
        return nullptr;
    }
    
    const GrGLInterface *grGlInterface = GrGLCreateNativeInterface();
    sk_sp<const GrGLInterface> glInterface(grGlInterface);
    if (glInterface.get() == nullptr) {
        RS_LOGE("CreateShareGrContext failed");
        return nullptr;
    }
    
    GrContextOptions options = { };
    options.fGpuPathRenderers = GpuPathRenderers::kAll & ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;
    
    sk_sp<GrContext> grContext = GrContext::MakeGL(std::move(glInterface), options);
    if (grContext == nullptr) {
        RS_LOGE("nullptr grContext is null");
        return nullptr;
    }

    RS_LOGI("Create grContext successfully!!!, grContext:[%{public}p], index:[%{public}d]", grContext.get(), index);
    return grContext.release();
}

SkSurface* RSSubMainThread::AcquireShareSkSurface(uint32_t index, int32_t width, int32_t height)
{
    auto grContext = CreateShareGrContext(index);
    if (grContext == nullptr) {
        RS_LOGE("Share GrContext is not ready!!!");
        return nullptr;
    }
    
    auto surfaceInfo = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    auto skSurface = SkSurface::MakeRenderTarget(grContext, SkBudgeted::kYes, surfaceInfo);
    if (skSurface == nullptr) {
        RS_LOGE("skSurface is nullptr");
        return nullptr;
    }
    
    RS_LOGE("AcquireShareSkSurface successfully!!! (%{public}p)", skSurface->getCanvas());
    return skSurface.release();
}

SkSurface* RSSubMainThread::AcquireShareRsSurface(uint32_t index, int32_t width, int32_t height)
{
    auto grContext = CreateShareGrContext(index, width, height);
    if (grContext == nullptr) {
        RS_LOGE("Share GrContext is not ready!!!");
        return nullptr;
    }

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType = kRGBA_8888_SkColorType;

    GrBackendRenderTarget backendRenderTarget(width, height, 0, STENCIL_BUFFER_SIZE, framebufferInfo);
    sk_sp<SkColorSpace> skColorSpace{nullptr};
    if (static_cast<int>(colorSpace_) == static_cast<int>(COLOR_GAMUT_DISPLAY_P3)) {
        skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDCIP3);
    } else if (static_cast<int>(colorSpace_) == static_cast<int>(COLOR_GAMUT_ADOBE_RGB)) {
        skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kAdobeRGB);
    } else if (static_cast<int>(colorSpace_) == static_cast<int>(COLOR_GAMUT_BT2020)) {
        skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kRec2020);
    }

    SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;
    auto skSurface = SkSurface::MakeFromBackendRenderTarget(
        grContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, skColorSpace, &surfaceProps);
    if (skSurface == nullptr) {
        RS_LOGE("skSurface is nullptr");
        return nullptr;
    }

    RS_LOGI("AcquireShareRsSurface successfully!!! (%{public}p)", skSurface->getCanvas());
    return skSurface.release();
}
} // namespace Rosen
} // namespace OHOS
#endif