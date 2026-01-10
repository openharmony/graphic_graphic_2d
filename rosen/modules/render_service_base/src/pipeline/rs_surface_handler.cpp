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
#include "metadata_helper.h"
#endif
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

GPUCacheCleanupCallback RSSurfaceHandler::s_gpuCacheCleanupCallback = nullptr;
std::mutex RSSurfaceHandler::s_gpuCacheCleanupCallbackMutex_;

RSSurfaceHandler::~RSSurfaceHandler() noexcept
{
#ifdef RS_ENABLE_GPU
    // Ensure buffers currently held by this handler are included in cleanup.
#ifndef ROSEN_CROSS_PLATFORM
    std::set<uint64_t> bufferIds;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (buffer_.buffer != nullptr) {
            bufferIds.insert(buffer_.buffer->GetBufferId());
        }
        if (preBuffer_.buffer != nullptr) {
            bufferIds.insert(preBuffer_.buffer->GetBufferId());
        }
        if (holdBuffer_ && holdBuffer_->buffer != nullptr) {
            bufferIds.insert(holdBuffer_->buffer->GetBufferId());
        }
    }
    AddGPUCacheToCleanupSet(bufferIds);
#endif

    // Flush any pending GPU cache cleanup on destruction.
    FlushGPUCacheCleanup();
#endif
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
    UpdateBuffer(buffer.buffer, buffer.acquireFence, buffer.damageRect, buffer.timestamp, buffer.bufferOwnerCount_);
    SetCurrentFrameBufferConsumed();
    RS_LOGD_IF(DEBUG_PIPELINE,
        "RsDebug surfaceHandler(id: %{public}" PRIu64 ") buffer update, "
        "buffer[timestamp:%{public}" PRId64 ", seq:%{public}" PRIu32 "]",
        GetNodeId(), buffer.timestamp, buffer.buffer->GetSeqNum());
}

void RSSurfaceHandler::UpdateBuffer(
    const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence, const Rect& damage,
    const int64_t timestamp, std::shared_ptr<BufferOwnerCount> bufferOwnerCount)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (preBuffer_.buffer != nullptr) {
        bool isCached = consumer_->IsCached(preBuffer_.buffer->GetSeqNum());
        preBuffer_.Reset(isCached ? !RSSystemProperties::GetVKImageUseEnabled() : true);
    }
    preBuffer_ = buffer_;
    buffer_.buffer = buffer;
    buffer_.bufferOwnerCount_ = bufferOwnerCount;
    if (buffer != nullptr) {
        buffer_.seqNum = buffer->GetBufferId();
        buffer_.bufferOwnerCount_->seqNum_ = buffer_.seqNum;
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
#endif
}
}
