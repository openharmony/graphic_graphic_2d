/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "buffer_queue_consumer.h"

#include "buffer_log.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "BufferQueueConsumer" };
}

BufferQueueConsumer::BufferQueueConsumer(sptr<BufferQueue>& bufferQueue)
{
    BLOGFD("");
    bufferQueue_ = bufferQueue;
}

BufferQueueConsumer::~BufferQueueConsumer()
{
    BLOGFD("");
}

SurfaceError BufferQueueConsumer::AcquireBuffer(sptr<SurfaceBufferImpl>& buffer, int32_t& fence,
                                                int64_t& timestamp, Rect& damage)
{
    if (bufferQueue_ == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }
    return bufferQueue_->AcquireBuffer(buffer, fence, timestamp, damage);
}

SurfaceError BufferQueueConsumer::ReleaseBuffer(sptr<SurfaceBufferImpl>& buffer, int32_t fence)
{
    if (bufferQueue_ == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }
    return bufferQueue_->ReleaseBuffer(buffer, fence);
}

SurfaceError BufferQueueConsumer::RegisterConsumerListener(sptr<IBufferConsumerListener>& listener)
{
    if (bufferQueue_ == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }
    return bufferQueue_->RegisterConsumerListener(listener);
}

SurfaceError BufferQueueConsumer::UnregisterConsumerListener()
{
    if (bufferQueue_ == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }
    return bufferQueue_->UnregisterConsumerListener();
}

SurfaceError BufferQueueConsumer::SetDefaultWidthAndHeight(int32_t width, int32_t height)
{
    if (bufferQueue_ == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }
    return bufferQueue_->SetDefaultWidthAndHeight(width, height);
}
} // namespace OHOS
