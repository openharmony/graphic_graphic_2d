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

#include "include/core/SkCanvas.h"
#include "include/core/SkPicture.h"
#include "include/core/SkSurface.h"
#include "include/core/SkImageInfo.h"
#include "include/gpu/GrContext.h"

#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSColdStartThread::RSColdStartThread(std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode) :
    surfaceNode_(surfaceRenderNode)
{
    Start();
}

RSColdStartThread::~RSColdStartThread()
{
    if (isRunning_) {
        Stop();
    }
}

void RSColdStartThread::Start()
{
    if (thread_ == nullptr) {
        thread_ = std::make_unique<std::thread>(&RSColdStartThread::Run, this);
    }
}

void RSColdStartThread::Stop()
{
    if (handler_ != nullptr) {
        handler_->PostTask([surfaceNode = surfaceNode_]() {
            auto node = surfaceNode.lock();
            if (node != nullptr) {
                node->ClearCacheSurface();
            }
        }, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
    if (runner_ != nullptr) {
        runner_->Stop();
    }
    if (thread_ != nullptr && thread_->joinable()) {
        thread_->detach();
        thread_ = nullptr;
    }
    isRunning_ = false;
}

void RSColdStartThread::Run()
{
    isRunning_ = true;
    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    if (runner_ != nullptr) {
        runner_->Run();
    }
}

void RSColdStartThread::PostPlayBackTask(GrContext* context, sk_sp<SkPicture> picture, float width, float height)
{
    if (handler_ == nullptr) {
        RS_LOGE("RSColdStartThread::PostPlayBackTask failed, handler_ is nullptr");
    }
    auto task = [picture = picture, context = context,
        width = width, height = height, surfaceNode = surfaceNode_]() {
        auto node = surfaceNode.lock();
        if (!node) {
            RS_LOGE("RSColdStartThread::PostPlayBackTask surfaceNode is nullptr");
            return;
        }
        sk_sp<SkSurface> surface = nullptr;
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
        SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
        surface = SkSurface::MakeRenderTarget(context, SkBudgeted::kYes, info);
#else
        surface = SkSurface::MakeRasterN32Premul(width, height);
#endif
        if (surface == nullptr || surface->getCanvas() == nullptr) {
            RS_LOGE("RSColdStartThread::PostPlayBackTask make SkSurface failed");
            return;
        }
        surface->getCanvas()->drawPicture(picture);
        node->SetCacheSurface(surface);
        if (node->GetCacheSurface() == nullptr) {
            node->NotifyUIBufferAvailable();
        }
    };
    handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}
} // namespace Rosen
} // namespace OHOS
