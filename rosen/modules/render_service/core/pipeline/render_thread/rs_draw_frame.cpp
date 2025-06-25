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

#include "rs_draw_frame.h"

#include <hitrace_meter.h>
#include <parameters.h>

#include "rs_trace.h"

#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "gfx/performance/rs_perfmonitor_reporter.h"
#include "memory/rs_memory_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_render_node_gc.h"
#include "render/rs_filter_cache_manager.h"
#include "rs_frame_report.h"
#include "rs_uni_render_thread.h"

#include "rs_profiler.h"
#include "render/rs_high_performance_visual_engine.h"

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

bool RSDrawFrame::debugTraceEnabled_ =
    std::atoi((OHOS::system::GetParameter("persist.sys.graphic.openDebugTrace", "0")).c_str()) != 0;

void RSDrawFrame::RenderFrame()
{
    HitracePerfScoped perfTrace(RSDrawFrame::debugTraceEnabled_, HITRACE_TAG_GRAPHIC_AGP, "OnRenderFramePerfCount");
    RS_TRACE_NAME_FMT("RenderFrame");
    // The destructor of GPUCompositonCacheGuard, a memory release check will be performed
    RSMainThread::GPUCompositonCacheGuard guard;
    RsFrameReport::GetInstance().ReportSchedEvent(FrameSchedEvent::RS_UNI_RENDER_START, {});
    JankStatsRenderFrameStart();
    unirenderInstance_.IncreaseFrameCount();
    RSUifirstManager::Instance().ProcessSubDoneNode();
    Sync();
    const bool doJankStats = IsUniRenderAndOnVsync();
    JankStatsRenderFrameAfterSync(doJankStats);
    RSMainThread::Instance()->ProcessUiCaptureTasks();
    RSUifirstManager::Instance().PostUifistSubTasks();
    UnblockMainThread();
    RsFrameReport::GetInstance().UnblockMainThread();
    Render();
    RsFrameReport::GetInstance().ReportSchedEvent(FrameSchedEvent::RS_RENDER_END, {});
    ReleaseSelfDrawingNodeBuffer();
    NotifyClearGpuCache();
    RSMainThread::Instance()->CallbackDrawContextStatusToWMS(true);
    RSRenderNodeGC::Instance().ReleaseDrawableMemory();
    if (RSSystemProperties::GetPurgeBetweenFramesEnabled()) {
        unirenderInstance_.PurgeCacheBetweenFrames();
    }
    unirenderInstance_.MemoryManagementBetweenFrames();
    unirenderInstance_.PurgeShaderCacheAfterAnimate();
    MemoryManager::MemoryOverCheck(unirenderInstance_.GetRenderEngine()->GetRenderContext()->GetDrGPUContext());
    JankStatsRenderFrameEnd(doJankStats);
    RSPerfMonitorReporter::GetInstance().ReportAtRsFrameEnd();
    RsFrameReport::GetInstance().RenderEnd();
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
    RsFrameReport& fr = RsFrameReport::GetInstance();
    if (fr.GetEnable()) {
        fr.SendCommandsStart();
        fr.RenderEnd();
    }
 
    uint32_t renderFrameNumber = RS_PROFILER_GET_FRAME_NUMBER();
    switch (rsParallelType_) {
        case RsParallelType::RS_PARALLEL_TYPE_SYNC: { // wait until render finish in render thread
            unirenderInstance_.PostSyncTask([this, renderFrameNumber]() {
                unirenderInstance_.SetMainLooping(true);
                RS_PROFILER_ON_PARALLEL_RENDER_BEGIN();
                RenderFrame();
                unirenderInstance_.RunImageReleaseTask();
                RS_PROFILER_ON_PARALLEL_RENDER_END(renderFrameNumber);
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
            unirenderInstance_.PostTask([this, renderFrameNumber]() {
                unirenderInstance_.SetMainLooping(true);
                RS_PROFILER_ON_PARALLEL_RENDER_BEGIN();
                RSMainThread::Instance()->GetRSVsyncRateReduceManager().FrameDurationBegin();
                RenderFrame();
                unirenderInstance_.RunImageReleaseTask();
                RSMainThread::Instance()->GetRSVsyncRateReduceManager().FrameDurationEnd();
                RS_PROFILER_ON_PARALLEL_RENDER_END(renderFrameNumber);
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

bool RSDrawFrame::CheckCanvasSkipSync(std::shared_ptr<RSRenderNode> node)
{
    if (node->GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        auto nodeDrawable =
            std::static_pointer_cast<DrawableV2::RSCanvasDrawingRenderNodeDrawable>(node->GetRenderDrawable());
        if (nodeDrawable == nullptr) {
            return true;
        }
        if (nodeDrawable->IsNeedDraw() && nodeDrawable->GetDrawSkipType() == DrawableV2::DrawSkipType::NONE) {
            stagingSyncCanvasDrawingNodes_.emplace(node->GetId(), node);
            return false;
        }
    }
    return true;
}

void RSDrawFrame::Sync()
{
    RS_TRACE_NAME_FMT("Sync");
    RSMainThread::Instance()->GetContext().GetGlobalRootRenderNode()->Sync();

    auto& pendingSyncNodes = RSMainThread::Instance()->GetContext().pendingSyncNodes_;
    for (auto [id, weakPtr] : stagingSyncCanvasDrawingNodes_) {
        pendingSyncNodes.emplace(id, weakPtr);
    }
    stagingSyncCanvasDrawingNodes_.clear();
    for (auto& [id, weakPtr] : pendingSyncNodes) {
        if (auto node = weakPtr.lock()) {
            if (!CheckCanvasSkipSync(node)) {
                node->SkipSync();
                continue;
            }
            if (!RSUifirstManager::Instance().CollectSkipSyncNode(node)) {
                node->Sync();
            } else {
                node->SkipSync();
            }
        }
    }
    pendingSyncNodes.clear();
    HveFilter::GetHveFilter().ClearSurfaceNodeInfo();

    unirenderInstance_.Sync(std::move(stagingRenderThreadParams_));
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
    RS_TRACE_NAME_FMT("Render vsyncId: %" PRIu64 "", unirenderInstance_.GetVsyncId());
    unirenderInstance_.Render();
}

void RSDrawFrame::JankStatsRenderFrameStart()
{
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
    RSJankStats::GetInstance().SetAccumulatedBufferCount(RSBaseRenderUtil::GetAccumulatedBufferCount());
    unirenderInstance_.UpdateDisplayNodeScreenId();
}

void RSDrawFrame::JankStatsRenderFrameEnd(bool doJankStats)
{
    if (!doJankStats) {
        unirenderInstance_.SetDiscardJankFrames(false);
        return;
    }
    const auto& renderThreadParams = unirenderInstance_.GetRSRenderThreadParams();
    if (renderThreadParams == nullptr) {
        return;
    }
    RSJankStats::GetInstance().SetOnVsyncStartTime(
        renderThreadParams->GetOnVsyncStartTime(),
        renderThreadParams->GetOnVsyncStartTimeSteady(),
        renderThreadParams->GetOnVsyncStartTimeSteadyFloat());
    RSJankStats::GetInstance().SetImplicitAnimationEnd(renderThreadParams->GetImplicitAnimationEnd());
    RSJankStats::GetInstance().SetEndTime(
        unirenderInstance_.GetSkipJankAnimatorFrame(),
        unirenderInstance_.GetDiscardJankFrames() || renderThreadParams->GetDiscardJankFrames(),
        unirenderInstance_.GetDynamicRefreshRate());
    unirenderInstance_.SetDiscardJankFrames(false);
}
} // namespace Rosen
} // namespace OHOS
