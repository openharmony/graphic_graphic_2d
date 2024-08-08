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
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSSurfaceHandler::~RSSurfaceHandler() noexcept
{
#ifndef ROSEN_CROSS_PLATFORM
    ClearBufferCache();
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
void RSSurfaceHandler::ReleaseBuffer(SurfaceBufferEntry& buffer)
{
    auto consumer = GetConsumer();
    if (consumer != nullptr && buffer.buffer != nullptr) {
        auto ret = consumer->ReleaseBuffer(buffer.buffer, SyncFence::INVALID_FENCE);
        if (ret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGD("RsDebug surfaceHandler(id: %{public}" PRIu64 ") ReleaseBuffer failed(ret: %{public}d)!",
                GetNodeId(), ret);
        } else {
            RS_LOGD("RsDebug surfaceHandler(id: %{public}" PRIu64 ") ReleaseBuffer success(ret: %{public}d)!",
                GetNodeId(), ret);
        }
    }
    buffer.Reset();
}

void RSSurfaceHandler::ConsumeAndUpdateBuffer(SurfaceBufferEntry buffer)
{
    if (!buffer.buffer) {
        return;
    }
    SetBuffer(buffer.buffer, buffer.acquireFence, buffer.damageRect, buffer.timestamp);
    SetBufferSizeChanged(buffer.buffer);
    SetCurrentFrameBufferConsumed();
    RS_LOGD("RsDebug surfaceHandler(id: %{public}" PRIu64 ") buffer update, "\
        "buffer[timestamp:%{public}" PRId64 ", seq:%{public}" PRIu32 "]",
        GetNodeId(), buffer.timestamp, buffer.buffer->GetSeqNum());
}

void RSSurfaceHandler::CacheBuffer(const SurfaceBufferEntry& buffer, const std::string& surfaceName)
{
    std::lock_guard<std::mutex> lock(mutex_);
    uint64_t bufferTimestamp = static_cast<uint64_t>(buffer.timestamp);
    auto found = bufferCache_.find(bufferTimestamp);
    if (found != bufferCache_.end()) {
        ReleaseBuffer(found->second);
    }
    bufferCache_[bufferTimestamp] = buffer;
    // attention: don't delete or change follow trace
    std::string traceDescription = "RSSurfaceHandler buffer cache " + surfaceName + "[" + std::to_string(id_) + "]";
    RS_TRACE_INT(traceDescription, static_cast<int>(bufferCache_.size()));
}

bool RSSurfaceHandler::HasBufferCache() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return bufferCache_.size() != 0;
}

RSSurfaceHandler::SurfaceBufferEntry RSSurfaceHandler::GetBufferFromCache(
    uint64_t vsyncTimestamp, const std::string& surfaceName)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RSSurfaceHandler::SurfaceBufferEntry buffer;
    for (auto iter = bufferCache_.begin(); iter != bufferCache_.end();) {
        if (iter->first < vsyncTimestamp) {
            ReleaseBuffer(buffer);
            buffer = iter->second;
            iter = bufferCache_.erase(iter);
        } else {
            break;
        }
    }
    if (buffer.buffer != nullptr) {
        RS_TRACE_NAME_FMT("surfaceHandler(id: %" PRIu64 ") get buffer success, "
            "buffer[timestamp:%" PRId64 ", seq:%" PRIu32 "]",
            GetNodeId(), buffer.timestamp, buffer.buffer->GetSeqNum());
        // attention: don't delete or change follow trace
        std::string traceDescription =
            "RSSurfaceHandler buffer cache " + surfaceName + "[" + std::to_string(id_) + "]";
        RS_TRACE_INT(traceDescription, static_cast<int>(bufferCache_.size()));
    }
    return buffer;
}

void RSSurfaceHandler::ClearBufferCache()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (!bufferCache_.empty()) {
        ReleaseBuffer(bufferCache_.begin()->second);
        bufferCache_.erase(bufferCache_.begin());
    }
}
#endif
}
}
