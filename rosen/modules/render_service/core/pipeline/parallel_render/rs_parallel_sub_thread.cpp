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

#define EGL_EGLEXT_PROTOTYPES
#include "rs_parallel_sub_thread.h"
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include "GLES3/gl3.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkRect.h"
#include "EGL/egl.h"
#include "rs_trace.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "rs_node_cost_manager.h"
#include "rs_parallel_render_manager.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "rs_parallel_render_ext.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_engine.h"

namespace OHOS {
namespace Rosen {
RSParallelSubThread::RSParallelSubThread(int threadIndex)
    : threadIndex_(threadIndex), subThread_(nullptr), renderType_(ParallelRenderType::DRAW_IMAGE) {}
#ifdef NEW_RENDER_CONTEXT
RSParallelSubThread::RSParallelSubThread(std::shared_ptr<RenderContextBase> context,
    ParallelRenderType renderType, int threadIndex) : threadIndex_(threadIndex), subThread_(nullptr),
    renderContext_(context), renderType_(renderType) {}
#else
RSParallelSubThread::RSParallelSubThread(RenderContext *context, ParallelRenderType renderType, int threadIndex)
    : threadIndex_(threadIndex), subThread_(nullptr), renderContext_(context), renderType_(renderType) {}
#endif
RSParallelSubThread::~RSParallelSubThread()
{
    if (renderContext_ != nullptr) {
#ifdef NEW_RENDER_CONTEXT
        auto frame = renderContext_->GetRSRenderSurfaceFrame();
        EGLDisplay eglDisplay = frame->eglState->eglDisplay;
        eglDestroyContext(eglDisplay, eglShareContext_);
        eglShareContext_ = EGL_NO_CONTEXT;
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
#else
        eglDestroyContext(renderContext_->GetEGLDisplay(), eglShareContext_);
        eglShareContext_ = EGL_NO_CONTEXT;
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
#endif
    }
    texture_ = nullptr;
    canvas_ = nullptr;
#ifndef USE_ROSEN_DRAWING
    skSurface_ = nullptr;
#else
    surface_ = nullptr;
#endif
    RS_LOGI("~RSParallelSubThread():%{public}d", threadIndex_);
}

void RSParallelSubThread::MainLoopHandlePrepareTask()
{
    RS_TRACE_BEGIN("SubThreadCostPrepare[" + std::to_string(threadIndex_) + "]");
    StartPrepare();
    Prepare();
    RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
    RS_TRACE_END();
}

void RSParallelSubThread::MainLoop()
{
    InitSubThread();
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        CreateShareEglContext();
    }
#endif
    while (true) {
        WaitTaskSync();
        if (RSParallelRenderManager::Instance()->GetParallelRenderingStatus() == ParallelStatus::OFF) {
            return;
        }
        // parallel rendering will be enable when the windows number is greater than 50
        RSParallelRenderManager::Instance()->CommitSurfaceNum(50);
        switch (RSParallelRenderManager::Instance()->GetTaskType()) {
            case TaskType::PREPARE_TASK: {
                MainLoopHandlePrepareTask();
                break;
            }
            case TaskType::CALC_COST_TASK: {
                RS_TRACE_BEGIN("SubThreadCalcCost[" + std::to_string(threadIndex_) + "]");
                CalcCost();
                RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
                RS_TRACE_END();
                break;
            }
            case TaskType::PROCESS_TASK: {
                RS_TRACE_BEGIN("SubThreadCostProcess[" + std::to_string(threadIndex_) + "]");
                StartRender();
                Render();
                ParallelStatus status = RSParallelRenderManager::Instance()->GetParallelRenderingStatus();
                if (status == ParallelStatus::FIRSTFLUSH || status == ParallelStatus::WAITFIRSTFLUSH) {
                    RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
                }
                RS_TRACE_END();
                Flush();
                break;
            }
            case TaskType::COMPOSITION_TASK: {
                StartComposition();
                Composition();
                RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
                break;
            }
            default: {
                break;
            }
        }
    }
}

void RSParallelSubThread::StartSubThread()
{
    subThread_ = new std::thread(&RSParallelSubThread::MainLoop, this);
}

void RSParallelSubThread::WaitTaskSync()
{
    RSParallelRenderManager::Instance()->SubMainThreadWait(threadIndex_);
}

void RSParallelSubThread::InitSubThread()
{
    pthread_setname_np(pthread_self(), "SubMainThread");
    struct sched_param param = {0};
    // sched_priority interval: [1, 99]; higher number refers to a higher thread priority,
    // set 2 as experience reference.
    param.sched_priority = 2;
    (void)sched_setscheduler(0, SCHED_FIFO, &param);
}

void RSParallelSubThread::CreateShareEglContext()
{
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
        return;
    }
    if (renderContext_ == nullptr) {
        RS_LOGE("renderContext_ is nullptr");
        return;
    }
#ifdef NEW_RENDER_CONTEXT
    eglShareContext_ = renderContext_->CreateContext(true);
#else
    eglShareContext_ = renderContext_->CreateShareContext();
#endif
    if (eglShareContext_ == EGL_NO_CONTEXT) {
        RS_LOGE("eglShareContext_ is EGL_NO_CONTEXT");
        return;
    }
    if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
#ifdef NEW_RENDER_CONTEXT
        auto frame = renderContext_->GetRSRenderSurfaceFrame();
        EGLDisplay eglDisplay = frame->eglState->eglDisplay;
        if (!eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_)) {
#else
        if (!eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_)) {
#endif
            RS_LOGE("eglMakeCurrent failed");
            return;
        }
    }
#endif
}

void RSParallelSubThread::StartPrepare()
{
    InitUniVisitor();
}

void RSParallelSubThread::InitUniVisitor()
{
    RSUniRenderVisitor *uniVisitor = RSParallelRenderManager::Instance()->GetUniVisitor();
    if (uniVisitor == nullptr) {
        RS_LOGE("uniVisitor is nullptr");
        return;
    }
    visitor_ = std::make_shared<RSUniRenderVisitor>(*uniVisitor);
}

void RSParallelSubThread::Prepare()
{
    if (threadTask_ == nullptr) {
        RS_LOGE("threadTask is nullptr");
        return;
    }
    while (threadTask_->GetTaskSize() > 0) {
        RSParallelRenderManager::Instance()->StartTiming(threadIndex_);
        auto task = threadTask_->GetNextRenderTask();
        if (!task || (task->GetIdx() == 0)) {
            RS_LOGE("surfaceNode is nullptr");
            continue;
        }
        auto node = task->GetNode();
        if (!node) {
            RS_LOGE("surfaceNode is nullptr");
            continue;
        }
        node->Prepare(visitor_);
        RSParallelRenderManager::Instance()->StopTimingAndSetRenderTaskCost(
            threadIndex_, task->GetIdx(), TaskType::PREPARE_TASK);
    }
}

void RSParallelSubThread::CalcCost()
{
    if (threadTask_ == nullptr) {
        RS_LOGE("CalcCost thread task is null");
        return;
    }
    RSUniRenderVisitor *uniVisitor = RSParallelRenderManager::Instance()->GetUniVisitor();
    if (uniVisitor == nullptr) {
        RS_LOGE("CalcCost visitor is null");
        return;
    }
    std::shared_ptr<RSNodeCostManager> manager = std::make_shared<RSNodeCostManager>(
        RSParallelRenderManager::Instance()->IsDoAnimate(),
        RSParallelRenderManager::Instance()->IsOpDropped(),
        RSParallelRenderManager::Instance()->IsSecurityDisplay());

    while (threadTask_->GetTaskSize() > 0) {
        RSParallelRenderManager::Instance()->StartTiming(threadIndex_);
        auto task = threadTask_->GetNextRenderTask();
        if (task == nullptr || task->GetIdx() == 0) {
            RS_LOGI("CalcCost task is invalid");
            continue;
        }
        auto node = task->GetNode();
        if (node == nullptr) {
            RS_LOGI("CalcCost node is null");
            continue;
        }
        auto surfaceNodePtr = node->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (surfaceNodePtr == nullptr) {
            RS_LOGI("CalcCost surface node is null");
            continue;
        }
        manager->CalcNodeCost(*surfaceNodePtr);
        surfaceNodePtr->SetNodeCost(manager->GetDirtyNodeCost());
        RSParallelRenderManager::Instance()->StopTimingAndSetRenderTaskCost(
            threadIndex_, task->GetIdx(), TaskType::CALC_COST_TASK);
    }
}

void RSParallelSubThread::StartRender()
{
    CreateResource();
}

void RSParallelSubThread::Render()
{
    if (threadTask_ == nullptr) {
        RS_LOGE("threadTask is nullptr");
        return;
    }
    auto physicalDisplayNode = std::static_pointer_cast<RSDisplayRenderNode>(threadTask_->GetNode());
    auto physicalGeoPtr = (
        physicalDisplayNode->GetRenderProperties().GetBoundsGeometry());
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        if (canvas_ == nullptr) {
            RS_LOGE("Canvas is nullptr");
            return;
        }
#ifndef USE_ROSEN_DRAWING
        int saveCount = canvas_->save();
        if (RSMainThread::Instance()->GetRenderEngine()) {
            canvas_->SetHighContrast(RSMainThread::Instance()->GetRenderEngine()->IsHighContrastEnabled());
        }
        if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
            canvas_->clear(SK_ColorTRANSPARENT);
        }
        canvas_->save();
        if (physicalGeoPtr != nullptr) {
            canvas_->concat(physicalGeoPtr->GetMatrix());
            canvas_->SetCacheType(RSSystemProperties::GetCacheEnabledForRotation() ?
                RSPaintFilterCanvas::CacheType::ENABLED : RSPaintFilterCanvas::CacheType::DISABLED);
        }
#else
        auto saveCount = canvas_->Save();
        if (RSMainThread::Instance()->GetRenderEngine()) {
            canvas_->SetHighContrast(RSMainThread::Instance()->GetRenderEngine()->IsHighContrastEnabled());
        }
        if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
            canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        }
        canvas_->Save();
        if (physicalGeoPtr != nullptr) {
            canvas_->ConcatMatrix(physicalGeoPtr->GetMatrix());
            canvas_->SetCacheType(RSSystemProperties::GetCacheEnabledForRotation() ?
                RSPaintFilterCanvas::CacheType::ENABLED : RSPaintFilterCanvas::CacheType::DISABLED);
        }
#endif
        while (threadTask_->GetTaskSize() > 0) {
            RSParallelRenderManager::Instance()->StartTiming(threadIndex_);
            auto task = threadTask_->GetNextRenderTask();
            if (!task || (task->GetIdx() == 0)) {
                RS_LOGE("renderTask is nullptr");
                continue;
            }
            auto node = task->GetNode();
            if (!node) {
                RS_LOGE("surfaceNode is nullptr");
                continue;
            }
            node->Process(visitor_);
            RSParallelRenderManager::Instance()->StopTimingAndSetRenderTaskCost(
                threadIndex_, task->GetIdx(), TaskType::PROCESS_TASK);
        }
#ifndef USE_ROSEN_DRAWING
        canvas_->restoreToCount(saveCount);
#else
        canvas_->RestoreToCount(saveCount);
#endif
    }
#endif

#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        if (!displayNode_) {
            RS_LOGE("RSParallelSubThread::Render displayNode_ nullptr");
            return;
        }
        displayNode_->SetScreenId(physicalDisplayNode->GetScreenId());
        displayNode_->SetDisplayOffset(
            physicalDisplayNode->GetDisplayOffsetX(), physicalDisplayNode->GetDisplayOffsetY());
        displayNode_->SetForceSoftComposite(physicalDisplayNode->IsForceSoftComposite());
        auto geoPtr = (
            displayNode_->GetRenderProperties().GetBoundsGeometry());
        if (physicalGeoPtr && geoPtr) {
            *geoPtr = *physicalGeoPtr;
            geoPtr->UpdateByMatrixFromSelf();
        }
        while (threadTask_->GetTaskSize() > 0) {
            auto task = threadTask_->GetNextRenderTask();
            if (!task || (task->GetIdx() == 0)) {
                RS_LOGE("renderTask is nullptr");
                continue;
            }
            auto node = task->GetNode();
            if (!node) {
                RS_LOGE("surfaceNode is nullptr");
                continue;
            }
            displayNode_->AddCrossParentChild(node);
        }
        displayNode_->Process(visitor_);
        for (auto& child : *displayNode_->GetChildren()) {
            displayNode_->RemoveCrossParentChild(child, physicalDisplayNode);
        }
    }
#endif
}

#ifdef USE_ROSEN_DRAWING
bool RSParallelSubThread::FlushForRosenDrawing()
{
    if (drCanvas_ == nullptr) {
        RS_LOGE("in Flush(), drCanvas is nullptr");
        return false;
    }
    if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
        RS_TRACE_BEGIN("Flush");
        if (drContext_) {
            drContext_->FlushAndSubmit(false);
        }
        RS_TRACE_END();
        RS_TRACE_BEGIN("Create Fence");
#ifdef NEW_RENDER_CONTEXT
        if (renderContext_ == nullptr) {
            RS_LOGE("renderContext_ is nullptr");
            return false;
        }
        auto frame = renderContext_->GetRSRenderSurfaceFrame();
        EGLDisplay eglDisplay = frame->eglState->eglDisplay;
        eglSync_ = eglCreateSyncKHR(eglDisplay, EGL_SYNC_FENCE_KHR, nullptr);
#else
        eglSync_ = eglCreateSyncKHR(renderContext_->GetEGLDisplay(), EGL_SYNC_FENCE_KHR, nullptr);
#endif
        RS_TRACE_END();
        texture_ = surface_->GetImageSnapshot();
        drCanvas_->Discard();
    }
    return true;
}
#endif

void RSParallelSubThread::Flush()
{
    threadTask_ = nullptr;
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
#ifndef USE_ROSEN_DRAWING
        if (skCanvas_ == nullptr) {
            RS_LOGE("in Flush(), skCanvas is nullptr");
            return;
        }
        if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
            RS_TRACE_BEGIN("Flush");
            if (grContext_) {
                grContext_->flushAndSubmit(false);
            }
            RS_TRACE_END();
            RS_TRACE_BEGIN("Create Fence");
#ifdef NEW_RENDER_CONTEXT
            auto frame = renderContext_->GetRSRenderSurfaceFrame();
            EGLDisplay eglDisplay = frame->eglState->eglDisplay;
            eglSync_ = eglCreateSyncKHR(eglDisplay, EGL_SYNC_FENCE_KHR, nullptr);
#else
            eglSync_ = eglCreateSyncKHR(renderContext_->GetEGLDisplay(), EGL_SYNC_FENCE_KHR, nullptr);
#endif
            RS_TRACE_END();
            texture_ = skSurface_->makeImageSnapshot();
            skCanvas_->discard();
        }
#else
        if (!FlushForRosenDrawing()) {
            return;
        }
#endif
    }
#endif
    // FIRSTFLUSH or WAITFIRSTFLUSH
    ParallelStatus parallelStatus = RSParallelRenderManager::Instance()->GetParallelRenderingStatus();
    if (parallelStatus == ParallelStatus::FIRSTFLUSH || parallelStatus == ParallelStatus::WAITFIRSTFLUSH) {
        RSParallelRenderManager::Instance()->ReadySubThreadNumIncrement();
    } else {
        RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
    }
}

bool RSParallelSubThread::WaitReleaseFence()
{
    if (eglSync_ != EGL_NO_SYNC_KHR) {
#ifdef NEW_RENDER_CONTEXT
        if (renderContext_ == nullptr) {
            RS_LOGE("renderContext_ is nullptr");
            return false;
        }
        auto frame = renderContext_->GetRSRenderSurfaceFrame();
        EGLDisplay eglDisplay = frame->eglState->eglDisplay;
        EGLint ret = eglWaitSyncKHR(eglDisplay, eglSync_, 0);
#else
        EGLint ret = eglWaitSyncKHR(renderContext_->GetEGLDisplay(), eglSync_, 0);
#endif
        if (ret == EGL_FALSE) {
            ROSEN_LOGE("eglClientWaitSyncKHR error 0x%{public}x", eglGetError());
            return false;
        } else if (ret == EGL_TIMEOUT_EXPIRED_KHR) {
            ROSEN_LOGE("create eglClientWaitSyncKHR timeout");
            return false;
        }
#ifdef NEW_RENDER_CONTEXT
        eglDestroySyncKHR(eglDisplay, eglSync_);
#else
        eglDestroySyncKHR(renderContext_->GetEGLDisplay(), eglSync_);
#endif
    }
    eglSync_ = EGL_NO_SYNC_KHR;
    return true;
}

void RSParallelSubThread::CreateResource()
{
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        int width, height;
        RSParallelRenderManager::Instance()->GetFrameSize(width, height);
        if (width != surfaceWidth_ || height != surfaceHeight_) {
            RS_LOGE("CreateResource %{public}d, new size [%{public}d, %{public}d], old size [%{public}d, %{public}d]",
                threadIndex_, width, height, surfaceWidth_, surfaceHeight_);
            surfaceWidth_ = width;
            surfaceHeight_ = height;
#ifndef USE_ROSEN_DRAWING
            AcquireSubSkSurface(surfaceWidth_, surfaceHeight_);
            if (skSurface_ == nullptr) {
                RS_LOGE("in CreateResource, skSurface is nullptr");
                return;
            }
            skCanvas_ = skSurface_->getCanvas();
            canvas_ = std::make_shared<RSPaintFilterCanvas>(skCanvas_);
            canvas_->SetIsParallelCanvas(true);
#else
            AcquireSubDrawingSurface(surfaceWidth_, surfaceHeight_);
            if (surface_ == nullptr) {
                RS_LOGE("in CreateResource, surface_ is nullptr");
                return;
            }
            drCanvas_ = surface_->GetCanvas().get();
            canvas_ = std::make_shared<RSPaintFilterCanvas>(drCanvas_);
            canvas_->SetIsParallelCanvas(true);
#endif
        }
        visitor_ = std::make_shared<RSUniRenderVisitor>(canvas_, threadIndex_);
    }
#endif

#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        displayNode_ = RSParallelRenderManager::Instance()->GetParallelDisplayNode(threadIndex_);
        if (!displayNode_) {
            RS_LOGE("RSParallelSubThread::CreateResource displayNode_ nullptr");
            return;
        }
        visitor_ = std::make_shared<RSUniRenderVisitor>(nullptr, threadIndex_);
        visitor_->SetRenderFrame(
            RSParallelRenderManager::Instance()->GetParallelFrame(threadIndex_));
    }
#endif
    visitor_->CopyPropertyForParallelVisitor(
        RSParallelRenderManager::Instance()->GetUniVisitor());
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
sk_sp<GrDirectContext> RSParallelSubThread::CreateShareGrContext()
#else
sk_sp<GrContext> RSParallelSubThread::CreateShareGrContext()
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
    // fix svg antialiasing bug
    options.fGpuPathRenderers &= ~GpuPathRenderers::kAtlas;
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
#else
std::shared_ptr<Drawing::GPUContext> RSParallelSubThread::CreateShareGPUContext()
{
    auto drGPUContext = std::make_shared<Drawing::GPUContext>();
    Drawing::GPUContextOptions options;
    drGPUContext->BuildFromGL(options);
    return drGPUContext;
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSParallelSubThread::AcquireSubSkSurface(int width, int height)
{
    if (grContext_ == nullptr) {
        grContext_ = CreateShareGrContext();
    }

    if (grContext_ == nullptr) {
        RS_LOGE("Share GrContext is not ready!!!");
        return;
    }

    auto surfaceInfo = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    skSurface_ = SkSurface::MakeRenderTarget(grContext_.get(), SkBudgeted::kYes, surfaceInfo);
    if (skSurface_ == nullptr) {
        RS_LOGE("skSurface is not ready!!!");
        return;
    }
}
#else
void RSParallelSubThread::AcquireSubDrawingSurface(int width, int height)
{
    if (drContext_ == nullptr) {
        drContext_ = CreateShareGPUContext();
    }

    if (drContext_ == nullptr) {
        RS_LOGE("Share GrContext is not ready!!!");
        return;
    }

    Drawing::ImageInfo surfaceInfo =
        Drawing::ImageInfo{width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL};
    surface_ = Drawing::Surface::MakeRenderTarget(drContext_.get(), true, surfaceInfo);
    if (surface_ == nullptr) {
        RS_LOGE("surface is not ready!!!");
        return;
    }
}
#endif

void RSParallelSubThread::StartComposition()
{
    if (processorRenderEngine_ == nullptr) {
        processorRenderEngine_ = std::make_shared<RSUniRenderEngine>();
        processorRenderEngine_->Init();
        auto context = processorRenderEngine_->GetRenderContext();
        context->SetAndMakeCurrentShareContex(eglShareContext_);
    }
    compositionVisitor_ = std::make_shared<RSUniRenderVisitor>();
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    if (parallelRenderManager->GetUniParallelCompositionVisitor() != nullptr) {
        compositionVisitor_->CopyVisitorInfos(parallelRenderManager->GetUniParallelCompositionVisitor());
    } else {
        compositionVisitor_ = nullptr;
    }
}

void RSParallelSubThread::Composition()
{
    if (compositionTask_ == nullptr || compositionTask_->GetIdx() == 0) {
        RS_LOGE("compositionTask is nullptr or displayNodeId is 0");
        return;
    }

    auto node = compositionTask_->GetNode();
    if (node == nullptr || compositionVisitor_ == nullptr) {
        RS_LOGE("displayNode or visitor is nullptr.");
        return;
    }

    compositionVisitor_->SetProcessorRenderEngine(processorRenderEngine_);
    node->Process(compositionVisitor_);
    compositionVisitor_ = nullptr;
    compositionTask_ = nullptr;
}

EGLContext RSParallelSubThread::GetSharedContext() const
{
    return eglShareContext_;
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkSurface> RSParallelSubThread::GetSkSurface() const
{
    return skSurface_;
}
#else
std::shared_ptr<Drawing::Surface> RSParallelSubThread::GetDrawingSurface() const
{
    return surface_;
}
#endif

void RSParallelSubThread::SetSuperTask(std::unique_ptr<RSSuperRenderTask> superRenderTask)
{
    threadTask_ = std::move(superRenderTask);
}

void RSParallelSubThread::SetCompositionTask(std::unique_ptr<RSCompositionTask> compositionTask)
{
    compositionTask_ = std::move(compositionTask);
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImage> RSParallelSubThread::GetTexture() const
#else
std::shared_ptr<Drawing::Image> RSParallelSubThread::GetTexture() const
#endif
{
    return texture_;
}
} // namespace Rosen
} // namespace OHOS