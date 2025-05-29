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
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
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
    preBuffer_.Reset(!RSSystemProperties::GetVKImageUseEnabled());
    preBuffer_ = buffer_;
    buffer_.buffer = buffer;
    if (buffer != nullptr) {
        buffer_.seqNum = buffer->GetSeqNum();
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
