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

#include "pipeline/rs_cold_start_thread.h"

#ifdef RS_ENABLE_GL
#include <EGL/egl.h>
#include "include/gpu/GrContext.h"
#endif

#include "include/core/SkCanvas.h"
#include "include/core/SkPicture.h"
#include "include/core/SkSurface.h"
#include "include/core/SkImageInfo.h"

#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSColdStartThread::RSColdStartThread(std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode)
    : surfaceNode_(surfaceRenderNode)
{
#ifdef RS_ENABLE_GL
    thread_ = std::make_unique<std::thread>(&RSColdStartThread::Run, this, eglGetCurrentContext());
#else
    thread_ = std::make_unique<std::thread>(&RSColdStartThread::Run, this);
#endif
}

RSColdStartThread::~RSColdStartThread()
{
    if (isRunning_) {
        Stop();
    }
}

void RSColdStartThread::Stop()
{
    RS_TRACE_NAME_FMT("RSColdStartThread::Stop");
#ifdef RS_ENABLE_GL
    if (handler_ != nullptr) {
        handler_->PostSyncTask([this]() {
            RS_TRACE_NAME_FMT("RSColdStartThread abandonContext");
            for (auto grContext : grContexts_) {
                if (grContext != nullptr) {
                    grContext->abandonContext();
                }
            }
        }, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
#endif
    if (runner_ != nullptr) {
        runner_->Stop();
    }
    if (thread_ != nullptr && thread_->joinable()) {
        thread_->detach();
    }
    isRunning_ = false;
}

#ifdef RS_ENABLE_GL
void RSColdStartThread::Run(EGLContext context)
#else
void RSColdStartThread::Run()
#endif
{
    RS_TRACE_NAME_FMT("RSColdStartThread::Run");
#ifdef RS_ENABLE_GL
    context_ = RSSharedContext::MakeSharedGLContext(context);
    if (context_ != nullptr) {
        context_->MakeCurrent();
    }
#endif
    isRunning_ = true;
    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    if (runner_ != nullptr) {
        runner_->Run();
    }
}

void RSColdStartThread::PostPlayBackTask(sk_sp<SkPicture> picture, float width, float height)
{
    if (handler_ == nullptr) {
        RS_LOGE("RSColdStartThread::PostPlayBackTask failed, handler_ is nullptr");
        return;
    }
    auto task = [picture = picture, width = width, height = height, this]() {
#ifdef RS_ENABLE_GL
        if (context_ == nullptr) {
            RS_LOGE("RSColdStartThread::PostPlayBackTask context_ is nullptr");
            return;
        }
#endif
        auto node = surfaceNode_.lock();
        if (!node) {
            RS_LOGE("RSColdStartThread::PostPlayBackTask surfaceNode is nullptr");
            return;
        }
        if (surface_ == nullptr) {
#ifdef RS_ENABLE_GL
            SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
            auto grContext = context_->MakeGrContext();
            grContexts_.emplace_back(grContext);
            surface_ = SkSurface::MakeRenderTarget(grContext.get(), SkBudgeted::kYes, info);
#else
            surface_ = SkSurface::MakeRasterN32Premul(width, height);
#endif
        }
        if (surface_ == nullptr || surface_->getCanvas() == nullptr) {
            RS_LOGE("RSColdStartThread::PostPlayBackTask make SkSurface failed");
            return;
        }
        RS_LOGD("RSColdStartThread::PostPlayBackTask drawPicture");
        RS_TRACE_NAME_FMT("RSColdStartThread drawPicture");
        auto canvas = surface_->getCanvas();
        canvas->clear(SK_ColorTRANSPARENT);
        canvas->drawPicture(picture);
        if (node->GetCacheSurface() == nullptr) {
            node->NotifyUIBufferAvailable();
        }
        auto cachedSurface = node->GetCacheSurface();
        std::swap(cachedSurface, surface_);
        node->SetCacheSurface(cachedSurface);
    };
    handler_->RemoveAllEvents();
    handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}
} // namespace Rosen
} // namespace OHOS
