
/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_base_surface_util.h"

#include <parameters.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <unordered_set>

#include "pipeline/rs_surface_handler.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/rs_jank_stats.h"
#include "rs_frame_rate_vote.h"
#include "rs_trace.h"
#include "rs_uni_render_thread.h"
#include "system/rs_system_parameters.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string SCREENNODE = "ScreenNode";
}

GSError RSBaseSurfaceUtil::DropFrameProcess(RSSurfaceHandler& surfaceHandler, uint64_t presentWhen)
{
    auto availableBufferCnt = surfaceHandler.GetAvailableBufferCount();
    const auto surfaceConsumer = surfaceHandler.GetConsumer();
    if (surfaceConsumer == nullptr) {
        RS_LOGE("RsDebug RSBaseSurfaceUtil::DropFrameProcess (node: %{public}" PRIu64 "): surfaceConsumer is null!",
            surfaceHandler.GetNodeId());
        return OHOS::GSERROR_NO_CONSUMER;
    }

    // maxDirtyListSize should minus one buffer used for displaying, and another one that has just been acquried.
    int32_t maxDirtyListSize = static_cast<int32_t>(surfaceConsumer->GetQueueSize()) - 1 - 1;
    // maxDirtyListSize > 1 means QueueSize >3 too
    if (maxDirtyListSize > 1 && availableBufferCnt >= maxDirtyListSize) {
        if (IsTagEnabled(HITRACE_TAG_GRAPHIC_AGP)) {
            RS_TRACE_NAME("DropFrame");
        }
        IConsumerSurface::AcquireBufferReturnValue returnValue;
        returnValue.fence = SyncFence::InvalidFence();
        int32_t ret = surfaceConsumer->AcquireBuffer(returnValue, static_cast<int64_t>(presentWhen), false);
        if (ret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGW("RSBaseSurfaceUtil::DropFrameProcess(node: %{public}" PRIu64 "): AcquireBuffer failed("
                " ret: %{public}d), do nothing ", surfaceHandler.GetNodeId(), ret);
            return OHOS::GSERROR_NO_BUFFER;
        }

        ret = surfaceConsumer->ReleaseBuffer(returnValue.buffer, returnValue.fence);
        if (ret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGW("RSBaseSurfaceUtil::DropFrameProcess(node: %{public}" PRIu64
                    "): ReleaseBuffer failed(ret: %{public}d), Acquire done ",
                surfaceHandler.GetNodeId(), ret);
        }
        surfaceHandler.SetAvailableBufferCount(static_cast<int32_t>(surfaceConsumer->GetAvailableBufferCount()));
        RS_LOGD("RsDebug RSBaseSurfaceUtil::DropFrameProcess (node: %{public}" PRIu64 "), drop one frame",
            surfaceHandler.GetNodeId());
    }

    return OHOS::GSERROR_OK;
}

Rect RSBaseSurfaceUtil::MergeBufferDamages(const std::vector<Rect>& damages)
{
    RectI damage;
    std::for_each(damages.begin(), damages.end(), [&damage](const Rect& damageRect) {
        damage = damage.JoinRect(RectI(damageRect.x, damageRect.y, damageRect.w, damageRect.h));
    });
    return {damage.left_, damage.top_, damage.width_, damage.height_};
}

void RSBaseSurfaceUtil::MergeBufferDamages(Rect& surfaceDamage, const std::vector<Rect>& damages)
{
    RectI damage;
    std::for_each(damages.begin(), damages.end(), [&damage](const Rect& damageRect) {
        damage = damage.JoinRect(RectI(damageRect.x, damageRect.y, damageRect.w, damageRect.h));
    });
    surfaceDamage = { damage.left_, damage.top_, damage.width_, damage.height_ };
}

CM_INLINE bool RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(RSSurfaceHandler& surfaceHandler,
    uint64_t presentWhen, const DropFrameConfig& dropFrameConfig, uint64_t parentNodeId, bool dropFrameByScreenFrozen)
{
    if (surfaceHandler.GetAvailableBufferCount() <= 0) {
        return true;
    }
    const auto& consumer = surfaceHandler.GetConsumer();
    if (consumer == nullptr) {
        RS_LOGE("Consume and update buffer fail for consumer is nullptr");
        return false;
    }

    bool acqiureWithPTSEnable =
        RSUniRenderJudgement::IsUniRender() && RSSystemParameters::GetControlBufferConsumeEnabled();
    if (dropFrameConfig.ShouldDrop() && acqiureWithPTSEnable) {
        RS_LOGD("RsDebug RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(node: %{public}" PRIu64
            "), set drop frame level=%{public}d", surfaceHandler.GetNodeId(), dropFrameConfig.level);
    }

    // check presentWhen conversion validation range
    bool presentWhenValid = presentWhen <= static_cast<uint64_t>(INT64_MAX);
    uint64_t acquireTimeStamp = presentWhen;
    if (!presentWhenValid || !acqiureWithPTSEnable) {
        acquireTimeStamp = CONSUME_DIRECTLY;
        RS_LOGD("RSBaseSurfaceUtil::ConsumeAndUpdateBuffer ignore presentWhen "\
            "[acqiureWithPTSEnable:%{public}d, presentWhenValid:%{public}d]", acqiureWithPTSEnable, presentWhenValid);
    }

    std::shared_ptr<RSSurfaceHandler::SurfaceBufferEntry> surfaceBuffer;
    if (surfaceHandler.GetHoldBuffer() == nullptr) {
        IConsumerSurface::AcquireBufferReturnValue returnValue;
        int32_t ret = consumer->AcquireBuffer(returnValue, static_cast<int64_t>(acquireTimeStamp), false);
        if (returnValue.buffer == nullptr || ret != SURFACE_ERROR_OK) {
            auto holdReturnValue = surfaceHandler.GetHoldReturnValue();
            if (LIKELY(!dropFrameByScreenFrozen) && UNLIKELY(holdReturnValue)) {
                returnValue.buffer = holdReturnValue->buffer;
                returnValue.fence = holdReturnValue->fence;
                returnValue.timestamp = holdReturnValue->timestamp;
                returnValue.damages = holdReturnValue->damages;
                surfaceHandler.ResetHoldReturnValue();
            } else {
                RS_LOGD_IF(DEBUG_PIPELINE, "RsDebug surfaceHandler(id: %{public}" PRIu64 ") "
                    "AcquireBuffer failed(ret: %{public}d)!", surfaceHandler.GetNodeId(), ret);
                surfaceBuffer = nullptr;
                return false;
            }
        }
        auto holdReturnValue = surfaceHandler.GetHoldReturnValue();
        if (UNLIKELY(holdReturnValue)) {
            consumer->ReleaseBuffer(holdReturnValue->buffer, holdReturnValue->fence);
            surfaceHandler.ResetHoldReturnValue();
        }
        if (UNLIKELY(dropFrameByScreenFrozen)) {
            surfaceHandler.SetHoldReturnValue(returnValue);
            return false;
        }
        surfaceBuffer = std::make_shared<RSSurfaceHandler::SurfaceBufferEntry>();
        surfaceBuffer->buffer = returnValue.buffer;
        surfaceBuffer->acquireFence = returnValue.fence;
        surfaceBuffer->timestamp = returnValue.timestamp;
        RS_OPTIONAL_TRACE_NAME_FMT("RSBaseSurfaceUtil::ConsumeAndUpdateBuffer bufferId %" PRIu64,
            surfaceBuffer->buffer ? surfaceBuffer->buffer->GetBufferId() : 0);
        surfaceBuffer->RegisterReleaseBufferListener([](uint64_t bufferId) {
            RSUniRenderThread::Instance().ReleaseBufferById(bufferId);
        });
        RSUniRenderThread::Instance().AddPendingReleaseBuffer(consumer, surfaceBuffer->buffer,
                                                              SyncFence::InvalidFence());
        RS_LOGD_IF(DEBUG_PIPELINE,
            "RsDebug surfaceHandler(id: %{public}" PRIu64 ") AcquireBuffer success, acquireTimeStamp = "
            "%{public}" PRIu64 ", buffer timestamp = %{public}" PRId64 ", seq = %{public}" PRIu32 ".",
            surfaceHandler.GetNodeId(), acquireTimeStamp, surfaceBuffer->timestamp, surfaceBuffer->buffer->GetSeqNum());
        if (IsTagEnabled(HITRACE_TAG_GRAPHIC_AGP)) {
            RS_TRACE_NAME_FMT("RsDebug surfaceHandler(id: %" PRIu64 ") AcquireBuffer success, parentNodeId = %"
                PRIu64 ", acquireTimeStamp = %" PRIu64 ", buffer timestamp = %" PRId64 ", seq = %" PRIu32 ".",
                surfaceHandler.GetNodeId(), parentNodeId, acquireTimeStamp, surfaceBuffer->timestamp,
                surfaceBuffer->buffer->GetSeqNum());
        }
        // The damages of buffer will be merged here, only single damage is supported so far
        MergeBufferDamages(surfaceBuffer->damageRect, returnValue.damages);
        if (surfaceBuffer->damageRect.h <= 0 || surfaceBuffer->damageRect.w <= 0) {
            RS_LOGW("RsDebug surfaceHandler(id: %{public}" PRIu64 ") buffer damage is invalid",
                surfaceHandler.GetNodeId());
        }
        // Flip damage because the rect is specified relative to the bottom-left of the surface in gl,
        // but the damages is specified relative to the top-left in rs.
        // The damages in vk is also transformed to the same as gl now.
        // [planning]: Unify the damage's coordinate systems of vk and gl.
        surfaceBuffer->damageRect.y =
            surfaceBuffer->buffer->GetHeight() - surfaceBuffer->damageRect.y - surfaceBuffer->damageRect.h;
        if (consumer->IsBufferHold()) {
            surfaceHandler.SetHoldBuffer(surfaceBuffer);
            surfaceBuffer = nullptr;
            RS_LOGW("RsDebug surfaceHandler(id: %{public}" PRIu64 ") set hold buffer",
                surfaceHandler.GetNodeId());
            return true;
        }
    }
    if (consumer->IsBufferHold()) {
        surfaceBuffer = surfaceHandler.GetHoldBuffer();
        surfaceHandler.SetHoldBuffer(nullptr);
        consumer->SetBufferHold(false);
        RS_LOGW("RsDebug surfaceHandler(id: %{public}" PRIu64 ") consume hold buffer", surfaceHandler.GetNodeId());
    }
    if (surfaceBuffer == nullptr || surfaceBuffer->buffer == nullptr) {
        RS_LOGE("RsDebug surfaceHandler(id: %{public}" PRIu64 ") no buffer to consume", surfaceHandler.GetNodeId());
        return false;
    }
    RSJankStats::GetInstance().AvcodecVideoCollect(consumer->GetUniqueId(), surfaceBuffer->buffer->GetSeqNum());
    surfaceHandler.ConsumeAndUpdateBuffer(*surfaceBuffer);
    if (consumer->GetName() != SCREENNODE) {
        DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(surfaceHandler.GetNodeId(),
            consumer->GetSurfaceSourceType(), surfaceBuffer->buffer);
    }
    OHSurfaceSource sourceType =  consumer->GetSurfaceSourceType();
    surfaceHandler.SetSourceType(static_cast<uint32_t>(sourceType));
    if (sourceType == OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO) {
        RS_TRACE_NAME_FMT("lpp node: %" PRIu64 "", surfaceHandler.GetNodeId());
    }
    surfaceBuffer = nullptr;
    surfaceHandler.SetAvailableBufferCount(static_cast<int32_t>(consumer->GetAvailableBufferCount()));
    // should drop frame after acquire buffer to avoid drop key frame
    DropFrameProcess(surfaceHandler, acquireTimeStamp);
    return true;
}

} // namespace Rosen
} // namespace OHOS
