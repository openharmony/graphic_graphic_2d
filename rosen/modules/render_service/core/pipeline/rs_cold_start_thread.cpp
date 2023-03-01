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
static const std::string PLAYBACK_TASK_NAME = "PlaybackTask";

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
    RS_LOGD("RSColdStartThread surfaceNodeId:%" PRIu64 "", surfaceNodeId);
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
    RS_LOGD("~RSColdStartThread");
    if (isRunning_.load()) {
        Stop();
    }
}

void RSColdStartThread::Stop()
{
    if (!isRunning_.load() || handler_ == nullptr) {
        return;
    }
    if (!handler_->IsIdle()) {
        RS_LOGD("RSColdStartThread::Stop handler not idle, delay stop");
        RSMainThread::Instance()->RequestNextVSync();
        return;
    }

    RS_LOGD("RSColdStartThread::Stop");
    RS_TRACE_NAME_FMT("RSColdStartThread::Stop");
    isRunning_.store(false);
    handler_->PostSyncTask([this]() {
        RS_TRACE_NAME_FMT("RSColdStartThread abandonContext"); // abandonContext here to avoid crash
        RS_LOGD("RSColdStartThread releaseResourcesAndAbandonContext");
        {
            std::lock_guard<std::mutex> lock(imageMutex_);
            while (!images_.empty()) {
                images_.pop();
            }
        }
        if (grContext_ != nullptr) {
            grContext_->releaseResourcesAndAbandonContext();
            grContext_ = nullptr;
        }
        skSurface_ = nullptr;
#ifdef RS_ENABLE_GL
        context_ = nullptr;
#endif
    }, AppExecFwk::EventQueue::Priority::IMMEDIATE);

    RS_TRACE_NAME_FMT("RSColdStartThread runner stop");
    RS_LOGD("RSColdStartThread runner stop");
    if (runner_ != nullptr) {
        runner_->Stop();
    }
    if (thread_ != nullptr && thread_->joinable()) {
        thread_->detach();
    }
    RSMainThread::Instance()->PostTask([id = surfaceNodeId_]() {
        RS_LOGD("RSMainThread DestroyColdStartThread id:%" PRIu64 "", id);
        RSColdStartManager::Instance().DestroyColdStartThread(id);
    });
}

void RSColdStartThread::PostTask(std::function<void()> task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
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
#ifdef RS_ENABLE_GL
        if (grContext_ == nullptr) {
            grContext_ = context_->MakeGrContext();
        }
        SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
        skSurface_ = SkSurface::MakeRenderTarget(grContext_.get(), SkBudgeted::kYes, info);
#else
        skSurface_ = SkSurface::MakeRasterN32Premul(width, height);
#endif
        if (skSurface_ == nullptr || skSurface_->getCanvas() == nullptr) {
            RS_LOGE("RSColdStartThread::PostPlayBackTask make SkSurface failed");
            return;
        }

        RS_LOGD("RSColdStartThread::PostPlayBackTask drawCmdList Playback");
        RS_TRACE_NAME_FMT("RSColdStartThread Playback");
        auto canvas = skSurface_->getCanvas();
        canvas->clear(SK_ColorTRANSPARENT);
        drawCmdList->Playback(*canvas);

        RS_TRACE_BEGIN("flush");
        skSurface_->flush();
#ifdef RS_ENABLE_GL
        glFinish();
#endif
        sk_sp<SkImage> image = skSurface_->makeImageSnapshot();
        RS_TRACE_END();

        if (node->GetCachedImage() == nullptr) {
            node->NotifyUIBufferAvailable();
        }
        {
            std::lock_guard<std::mutex> lock(imageMutex_);
            images_.push(image);
        }
        RSMainThread::Instance()->PostTask([this]() {
            auto node = surfaceNode_.lock();
            if (!node) {
                RS_LOGE("RSColdStartThread PostSyncTask surfaceNode is nullptr");
                return;
            }
            {
                RS_LOGD("RSMainThread SetCachedImage");
                std::lock_guard<std::mutex> lock(imageMutex_);
                if (!images_.empty()) {
                    node->SetCachedImage(images_.back());
                }
            }
            RSMainThread::Instance()->RequestNextVSync();
        });
    };
    if (handler_->IsIdle()) {
        handler_->PostTask(task, PLAYBACK_TASK_NAME, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
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
        RS_LOGD("RSColdStartManager::StartColdStartThread id:%" PRIu64 "", id);
        coldStartThreadMap_[id] = std::make_unique<RSColdStartThread>(surfaceNode, id);
    }
}

void RSColdStartManager::StopColdStartThread(NodeId id)
{
    if (coldStartThreadMap_.count(id) != 0 && coldStartThreadMap_[id] != nullptr) {
        RS_LOGD("RSColdStartManager::StopColdStartThread id:%" PRIu64 "", id);
        coldStartThreadMap_[id]->Stop();
    }
}

void RSColdStartManager::DestroyColdStartThread(NodeId id)
{
    RS_LOGD("RSColdStartManager::DestroyColdStartThread id:%" PRIu64 "", id);
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
