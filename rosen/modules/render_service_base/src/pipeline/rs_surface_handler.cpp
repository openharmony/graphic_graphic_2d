/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_surface_handler.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "feature/buffer_reclaim/rs_buffer_reclaim.h"
#include "metadata_helper.h"
#endif
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
#ifndef ROSEN_CROSS_PLATFORM
const uint32_t MIN_RENDER_FRAMES_AFTER_CLEAN_CACHE = 4;
#endif
RSSurfaceHandler::~RSSurfaceHandler() noexcept
{
}
#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceHandler::SetConsumer(sptr<IConsumerSurface> consumer)
{
    consumer_ = consumer;
}
#endif

void RSSurfaceHandler::IncreaseAvailableBuffer()
{
    bufferAvailableCount_++;
}

void RSSurfaceHandler::ReduceAvailableBuffer()
{
    --bufferAvailableCount_;
}

void RSSurfaceHandler::SetGlobalZOrder(float globalZOrder)
{
    globalZOrder_ = globalZOrder;
}

float RSSurfaceHandler::GetGlobalZOrder() const
{
    return globalZOrder_;
}

#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceHandler::ConsumeAndUpdateBuffer(SurfaceBufferEntry buffer)
{
    ConsumeAndUpdateBufferInner(buffer);
}

void RSSurfaceHandler::ConsumeAndUpdateBufferInner(SurfaceBufferEntry& buffer)
{
    if (!buffer.buffer) {
        return;
    }
    using namespace OHOS::HDI::Display::Graphic::Common::V1_0;
    BufferHandleMetaRegion crop;
    if (MetadataHelper::GetCropRectMetadata(buffer.buffer, crop) == GSERROR_OK) {
        buffer.buffer->SetCropMetadata({crop.left, crop.top, crop.width, crop.height});
    }
    UpdateBuffer(buffer.buffer, buffer.acquireFence, buffer.damageRect, buffer.timestamp);
    SetCurrentFrameBufferConsumed();
    RS_LOGD_IF(DEBUG_PIPELINE,
        "RsDebug surfaceHandler(id: %{public}" PRIu64 ") buffer update, "
        "buffer[timestamp:%{public}" PRId64 ", seq:%{public}" PRIu32 "]",
        GetNodeId(), buffer.timestamp, buffer.buffer->GetSeqNum());
}

void RSSurfaceHandler::UpdateBuffer(
    const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence, const Rect& damage, const int64_t timestamp)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (preBuffer_.buffer != nullptr) {
        bool isCached = consumer_->IsCached(preBuffer_.buffer->GetSeqNum());
        preBuffer_.Reset(isCached ? !RSSystemProperties::GetVKImageUseEnabled() : true);
    }
    ResetLastBufferInfo();
    preBuffer_ = buffer_;
    buffer_.buffer = buffer;
    if (buffer != nullptr) {
        buffer_.seqNum = buffer->GetBufferId();
        buffer_.buffer->ClearBufferDeletedFlag(BufferDeletedFlag::DELETED_FROM_RS);
    }
    buffer_.acquireFence = acquireFence;
    buffer_.damageRect = damage;
    buffer_.timestamp = timestamp;

    if (preBuffer_.buffer == nullptr) {
        return;
    }
    bufferSizeChanged_ =
        buffer->GetWidth() != preBuffer_.buffer->GetWidth() || buffer->GetHeight() != preBuffer_.buffer->GetHeight();
}

void RSSurfaceHandler::SetHoldReturnValue(const IConsumerSurface::AcquireBufferReturnValue& returnValue)
{
    holdReturnValue_ = std::make_shared<IConsumerSurface::AcquireBufferReturnValue>();
    holdReturnValue_->buffer = returnValue.buffer;
    holdReturnValue_->fence = returnValue.fence;
    holdReturnValue_->timestamp = returnValue.timestamp;
    holdReturnValue_->damages = returnValue.damages;
}

bool RSSurfaceHandler::ReclaimLastBufferPrepare()
{
    ++lastBufferReclaimNum_;
    return (lastBufferReclaimNum_ >= MIN_RENDER_FRAMES_AFTER_CLEAN_CACHE);
}

bool RSSurfaceHandler::ReclaimLastBufferProcess()
{
    if (!RSBufferReclaim::GetInstance().CheckBufferReclaim()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    sptr<SurfaceBuffer> buffer = buffer_.buffer;
    bool ret = false;
    if ((lastBufferId_ != 0) && (buffer != nullptr) && (buffer->GetBufferId() == lastBufferId_)) {
        auto startTime = std::chrono::steady_clock::now();
        {
            RS_TRACE_NAME_FMT("[LastBufferReclaim]TryReclaim-begin: bufId: %" PRIu64 ""
                ", bufSeqNum: %u, w: %d, h: %d, fmt: %d, hasReclaim: %d", buffer->GetBufferId(), buffer->GetSeqNum(),
                buffer->GetWidth(), buffer->GetHeight(), buffer->GetFormat(), buffer->IsReclaimed());
            ret = RSBufferReclaim::GetInstance().DoBufferReclaim(buffer);
            RS_TRACE_NAME_FMT("[LastBufferReclaim]TryReclaim-end, ret: %d", ret);
        }
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        RS_LOGI("[LastBufferReclaim]TryReclaim: bufId: %{public}" PRIu64 ""
            ", bufSeqNum: %{public}u, w: %{public}d, h: %{public}d, fmt: %{public}d, cost %{public}" PRIu64 " ms",
            buffer->GetBufferId(), buffer->GetSeqNum(), buffer->GetWidth(), buffer->GetHeight(), buffer->GetFormat(),
            static_cast<uint64_t>(duration.count()));
        lastBufferReclaimNum_ = 0;
        lastBufferId_ = 0;
    }
    return ret;
}

void RSSurfaceHandler::TryResumeLastBuffer()
{
    std::lock_guard<std::mutex> lock(mutex_);
    sptr<SurfaceBuffer> buffer = buffer_.buffer;
    if (buffer && buffer->IsReclaimed()) {
        auto startTime = std::chrono::steady_clock::now();
        {
            RS_TRACE_NAME_FMT("[LastBufferReclaim]TryResumeLastBuffer-begin: bufId: %" PRIu64 ""
                ", bufSeqNum: %u, w: %d, h: %d, fmt: %d", buffer->GetBufferId(),
                buffer->GetSeqNum(), buffer->GetWidth(), buffer->GetHeight(), buffer->GetFormat());
            bool ret = RSBufferReclaim::GetInstance().DoBufferResume(buffer);
            RS_TRACE_NAME_FMT("[LastBufferReclaim]TryResumeLastBuffer-end, ret: %d", ret);
        }
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        RS_LOGI("[LastBufferReclaim]TryResumeLastBuffer: bufId: %{public}" PRIu64 ""
            ", bufSeqNum: %{public}u, w: %{public}d, h: %{public}d, fmt: %{public}d, cost %{public}" PRIu64 " ms",
            buffer->GetBufferId(), buffer->GetSeqNum(), buffer->GetWidth(), buffer->GetHeight(), buffer->GetFormat(),
            static_cast<uint64_t>(duration.count()));
    }
}
#endif
}
}
