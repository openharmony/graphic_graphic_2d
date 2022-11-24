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
#ifdef ROSEN_OHOS
#include <sys/prctl.h>
#endif
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include "include/core/SkImageInfo.h"

#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
static const std::string THREAD_NAME = "ColdStartThread";

static void SystemCallSetThreadName(const std::string& name)
{
#ifdef ROSEN_OHOS
    if (prctl(PR_SET_NAME, name.c_str()) < 0) {
        return;
    }
#endif
}

RSColdStartThread::RSColdStartThread(std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode, NodeId surfaceNodeId)
    : surfaceNode_(surfaceRenderNode), surfaceNodeId_(surfaceNodeId)
{
#ifdef RS_ENABLE_GL
    thread_ = std::make_unique<std::thread>(&RSColdStartThread::Run, this, eglGetCurrentContext());
#else
    thread_ = std::make_unique<std::thread>(&RSColdStartThread::Run, this);
#endif
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait_for(lock, std::chrono::milliseconds(10), [this]() { // wait for 10ms max
        return isRunning_.load();
    });
}

RSColdStartThread::~RSColdStartThread()
{
    if (isRunning_.load()) {
        Stop();
    }
}

void RSColdStartThread::Stop()
{
    if (!isRunning_.load()) {
        return;
    }
    isRunning_.store(false);
    RS_TRACE_NAME_FMT("RSColdStartThread::Stop");
    if (handler_ != nullptr) {
        handler_->PostTask([this]() {
            RS_TRACE_NAME_FMT("RSColdStartThread abandonContext"); // abandonContext here to avoid crash
            for (auto& resource : resourceVector_) {
                auto grContext = resource.grContext;
                if (grContext != nullptr) {
                    grContext->abandonContext();
                }
            }
            resourceVector_.clear();
            currentResource_.grContext = nullptr;
            currentResource_.skSurface = nullptr;
#ifdef RS_ENABLE_GL
            context_ = nullptr;
#endif
            RSMainThread::Instance()->PostTask([this]() {
                RS_TRACE_NAME_FMT("RSColdStartThread runner stop");
                if (runner_ != nullptr) {
                    runner_->Stop();
                }
                if (thread_ != nullptr && thread_->joinable()) {
                    thread_->detach();
                }
                RSColdStartManager::Instance().DestroyColdStartThread(surfaceNodeId_);
            });
        }, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

#ifdef RS_ENABLE_GL
void RSColdStartThread::Run(EGLContext context)
#else
void RSColdStartThread::Run()
#endif
{
    RS_LOGD("RSColdStartThread::Run");
    RS_TRACE_NAME_FMT("RSColdStartThread::Run");
    SystemCallSetThreadName(THREAD_NAME);
#ifdef RS_ENABLE_GL
    context_ = RSSharedContext::MakeSharedGLContext(context);
    if (context_ != nullptr) {
        context_->MakeCurrent();
    }
#endif
    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        isRunning_.store(true);
        cv_.notify_one();
    }
    if (runner_ != nullptr) {
        runner_->Run();
    }
}

void RSColdStartThread::PostPlayBackTask(std::shared_ptr<DrawCmdList> drawCmdList, float width, float height)
{
    if (handler_ == nullptr) {
        RS_LOGE("RSColdStartThread::PostPlayBackTask failed, handler_ is nullptr");
        return;
    }
    auto task = [drawCmdList = drawCmdList, width = width, height = height, this]() {
#ifdef RS_ENABLE_GL
        if (context_ == nullptr) {
            RS_LOGE("RSColdStartThread::PostPlayBackTask context_ is nullptr");
            return;
        }
#endif
        if (drawCmdList == nullptr) {
            RS_LOGE("RSColdStartThread::PostPlayBackTask drawCmdList is nullptr");
            return;
        }
        auto node = surfaceNode_.lock();
        if (!node) {
            RS_LOGE("RSColdStartThread::PostPlayBackTask surfaceNode is nullptr");
            return;
        }
        if (currentResource_.grContext == nullptr || currentResource_.skSurface == nullptr) {
            sk_sp<SkSurface> surface;
            sk_sp<GrContext> grContext;
#ifdef RS_ENABLE_GL
            SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
            grContext = context_->MakeGrContext();
            surface = SkSurface::MakeRenderTarget(grContext.get(), SkBudgeted::kYes, info);
#else
            surface = SkSurface::MakeRasterN32Premul(width, height);
#endif
            if (surface == nullptr || surface->getCanvas() == nullptr) {
                RS_LOGE("RSColdStartThread::PostPlayBackTask make SkSurface failed");
                return;
            }
            currentResource_ = { grContext, surface };
            resourceVector_.emplace_back(currentResource_);
        }

        RS_LOGD("RSColdStartThread::PostPlayBackTask drawCmdList Playback");
        RS_TRACE_NAME_FMT("RSColdStartThread Playback");
        auto canvas = currentResource_.skSurface->getCanvas();
        canvas->clear(SK_ColorTRANSPARENT);
        drawCmdList->Playback(*canvas);
        if (node->GetCachedResource().skSurface == nullptr) {
            node->NotifyUIBufferAvailable();
        }
        RSMainThread::Instance()->PostSyncTask([this]() {
            auto node = surfaceNode_.lock();
            if (!node) {
                RS_LOGE("RSColdStartThread PostSyncTask surfaceNode is nullptr");
                return;
            }
            auto cachedResource = node->GetCachedResource();
            node->SetCachedResource(currentResource_);
            currentResource_ = cachedResource;
        });
    };
    handler_->RemoveAllEvents();
    handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

RSColdStartManager& RSColdStartManager::Instance()
{
    static RSColdStartManager instance;
    return instance;
}

void RSColdStartManager::PostPlayBackTask(NodeId id, std::shared_ptr<DrawCmdList> drawCmdList,
    float width, float height)
{
    if (coldStartThreadMap_.count(id) != 0 && coldStartThreadMap_[id] != nullptr) {
        coldStartThreadMap_[id]->PostPlayBackTask(drawCmdList, width, height);
    }
}

bool RSColdStartManager::IsColdStartThreadRunning(NodeId id)
{
    return coldStartThreadMap_.count(id) != 0 && coldStartThreadMap_[id] != nullptr;
}

void RSColdStartManager::StartColdStartThreadIfNeed(std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    if (surfaceNode == nullptr) {
        RS_LOGE("RSColdStartManager::StartColdStartThreadIfNeed surfaceNode is nullptr");
        return;
    }
    auto id = surfaceNode->GetId();
    if (coldStartThreadMap_.count(id) == 0) {
        coldStartThreadMap_[id] = std::make_unique<RSColdStartThread>(surfaceNode, id);
    }
}

void RSColdStartManager::StopColdStartThread(NodeId id)
{
    if (coldStartThreadMap_.count(id) != 0 && coldStartThreadMap_[id] != nullptr) {
        coldStartThreadMap_[id]->Stop();
    }
}

void RSColdStartManager::DestroyColdStartThread(NodeId id)
{
    coldStartThreadMap_.erase(id);
}

void RSColdStartManager::CheckColdStartMap(const RSRenderNodeMap& nodeMap)
{
    for (auto& elem : coldStartThreadMap_) {
        auto node = nodeMap.GetRenderNode<RSSurfaceRenderNode>(elem.first);
        if (!node && elem.second) {
            RS_LOGD("RSColdStartManager::CheckColdStartMap need stop");
            elem.second->Stop();
        }
    }
    RS_LOGD("RSColdStartManager::CheckColdStartMap size:%zu", coldStartThreadMap_.size());
}
} // namespace Rosen
} // namespace OHOS
