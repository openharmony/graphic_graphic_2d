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

#include "pipeline/rs_draw_frame.h"

#include "rs_trace.h"

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_uifirst_manager.h"
#include "pipeline/rs_uni_render_thread.h"
#include "property/rs_filter_cache_manager.h"
#include "rs_frame_report.h"

namespace OHOS {
namespace Rosen {
RSDrawFrame::RSDrawFrame()
    : unirenderInstance_(RSUniRenderThread::Instance()), rsParallelType_(RSSystemParameters::GetRsParallelType())
{}

RSDrawFrame::~RSDrawFrame() noexcept {}

void RSDrawFrame::SetRenderThreadParams(std::unique_ptr<RSRenderThreadParams>& stagingRenderThreadParams)
{
    stagingRenderThreadParams_ = std::move(stagingRenderThreadParams);
}

void RSDrawFrame::RenderFrame()
{
    RS_TRACE_NAME_FMT("RenderFrame");
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().RSRenderStart();
    }
    JankStatsRenderFrameStart();
    RSUifirstManager::Instance().ProcessSubDoneNode();
    Sync();
    const bool doJankStats = IsUniRenderAndOnVsync();
    JankStatsRenderFrameAfterSync(doJankStats);
    RSUifirstManager::Instance().PostUifistSubTasks();
    UnblockMainThread();
    Render();
    ReleaseSelfDrawingNodeBuffer();
    NotifyClearGpuCache();
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().RSRenderEnd();
    }
    JankStatsRenderFrameEnd(doJankStats);
    RSRenderNodeGC::Instance().ReleaseDrawableMemory();
}

void RSDrawFrame::NotifyClearGpuCache()
{
    if (RSFilterCacheManager::GetFilterInvalid()) {
        unirenderInstance_.ClearMemoryCache(ClearMemoryMoment::FILTER_INVALID, true);
        RSFilterCacheManager::SetFilterInvalid(false);
    }
}

void RSDrawFrame::ReleaseSelfDrawingNodeBuffer()
{
    unirenderInstance_.ReleaseSelfDrawingNodeBuffer();
}

void RSDrawFrame::PostAndWait()
{
    RS_TRACE_NAME_FMT("PostAndWait, parallel type %d", static_cast<int>(rsParallelType_));
    switch (rsParallelType_) {
        case RsParallelType::RS_PARALLEL_TYPE_SYNC: { // wait until render finish in render thread
            unirenderInstance_.PostSyncTask([this]() {
                unirenderInstance_.SetMainLooping(true);
                RenderFrame();
                unirenderInstance_.RunImageReleaseTask();
                unirenderInstance_.SetMainLooping(false);
            });
            break;
        }
        case RsParallelType::RS_PARALLEL_TYPE_SINGLE_THREAD: { // render in main thread
            RenderFrame();
            unirenderInstance_.RunImageReleaseTask();
            break;
        }
        case RsParallelType::RS_PARALLEL_TYPE_ASYNC: // wait until sync finish in render thread
        default: {
            std::unique_lock<std::mutex> frameLock(frameMutex_);
            canUnblockMainThread = false;
            unirenderInstance_.PostTask([this]() {
                unirenderInstance_.SetMainLooping(true);
                RenderFrame();
                unirenderInstance_.RunImageReleaseTask();
                unirenderInstance_.SetMainLooping(false);
            });

            frameCV_.wait(frameLock, [this] { return canUnblockMainThread; });
        }
    }
}

void RSDrawFrame::PostDirectCompositionJankStats(const JankDurationParams& rsParams)
{
    RS_TRACE_NAME_FMT("PostDirectCompositionJankStats, parallel type %d", static_cast<int>(rsParallelType_));
    switch (rsParallelType_) {
        case RsParallelType::RS_PARALLEL_TYPE_SYNC: // wait until render finish in render thread
        case RsParallelType::RS_PARALLEL_TYPE_SINGLE_THREAD: // render in main thread
        case RsParallelType::RS_PARALLEL_TYPE_ASYNC: // wait until sync finish in render thread
        default: {
            bool isReportTaskDelayed = unirenderInstance_.IsMainLooping();
            auto task = [rsParams, isReportTaskDelayed]() -> void {
                RSJankStats::GetInstance().HandleDirectComposition(rsParams, isReportTaskDelayed);
            };
            unirenderInstance_.PostTask(task);
        }
    }
}

void RSDrawFrame::Sync()
{
    RS_TRACE_NAME_FMT("Sync");
    RSMainThread::Instance()->GetContext().GetGlobalRootRenderNode()->Sync();

    auto& pendingSyncNodes = RSMainThread::Instance()->GetContext().pendingSyncNodes_;
    for (auto& [id, weakPtr] : pendingSyncNodes) {
        if (auto node = weakPtr.lock()) {
            if (!RSUifirstManager::Instance().CollectSkipSyncNode(node)) {
                node->Sync();
            } else {
                node->SkipSync();
            }
        }
    }
    pendingSyncNodes.clear();

    unirenderInstance_.Sync(stagingRenderThreadParams_);
}

void RSDrawFrame::UnblockMainThread()
{
    RS_TRACE_NAME_FMT("UnlockMainThread");
    std::unique_lock<std::mutex> frameLock(frameMutex_);
    if (!canUnblockMainThread) {
        canUnblockMainThread = true;
        frameCV_.notify_all();
    }
}

void RSDrawFrame::Render()
{
    unirenderInstance_.Render();
}

void RSDrawFrame::JankStatsRenderFrameStart()
{
    unirenderInstance_.SetDiscardJankFrames(false);
    unirenderInstance_.SetSkipJankAnimatorFrame(false);
}

bool RSDrawFrame::IsUniRenderAndOnVsync() const
{
    const auto& renderThreadParams = unirenderInstance_.GetRSRenderThreadParams();
    if (!renderThreadParams) {
        return false;
    }
    return renderThreadParams->IsUniRenderAndOnVsync();
}

void RSDrawFrame::JankStatsRenderFrameAfterSync(bool doJankStats)
{
    if (!doJankStats) {
        return;
    }
    RSJankStats::GetInstance().SetStartTime();
    unirenderInstance_.UpdateDisplayNodeScreenId();
}

void RSDrawFrame::JankStatsRenderFrameEnd(bool doJankStats)
{
    if (!doJankStats) {
        return;
    }
    RSJankStats::GetInstance().SetOnVsyncStartTime(
        unirenderInstance_.GetRSRenderThreadParams()->GetOnVsyncStartTime(),
        unirenderInstance_.GetRSRenderThreadParams()->GetOnVsyncStartTimeSteady());
    RSJankStats::GetInstance().SetEndTime(unirenderInstance_.GetSkipJankAnimatorFrame(),
        unirenderInstance_.GetDiscardJankFrames(), unirenderInstance_.GetDynamicRefreshRate());
}
} // namespace Rosen
} // namespace OHOS
