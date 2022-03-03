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
BufferQueueConsumer::BufferQueueConsumer(sptr<BufferQueue>& bufferQueue)
{
    bufferQueue_ = bufferQueue;
    if (bufferQueue_ != nullptr) {
        bufferQueue_->GetName(name_);
    }
    BLOGNI("ctor");
}

BufferQueueConsumer::~BufferQueueConsumer()
{
    BLOGNI("dtor");
}

GSError BufferQueueConsumer::AcquireBuffer(sptr<SurfaceBufferImpl>& buffer, int32_t &fence,
    int64_t &timestamp, Rect &damage)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->AcquireBuffer(buffer, fence, timestamp, damage);
}

GSError BufferQueueConsumer::ReleaseBuffer(sptr<SurfaceBufferImpl>& buffer, int32_t fence)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->ReleaseBuffer(buffer, fence);
}

GSError BufferQueueConsumer::AttachBuffer(sptr<SurfaceBufferImpl>& buffer)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->AttachBuffer(buffer);
}

GSError BufferQueueConsumer::DetachBuffer(sptr<SurfaceBufferImpl>& buffer)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->DetachBuffer(buffer);
}

GSError BufferQueueConsumer::RegisterConsumerListener(sptr<IBufferConsumerListener>& listener)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->RegisterConsumerListener(listener);
}

GSError BufferQueueConsumer::RegisterConsumerListener(IBufferConsumerListenerClazz *listener)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->RegisterConsumerListener(listener);
}

GSError BufferQueueConsumer::RegisterReleaseListener(OnReleaseFunc func)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->RegisterReleaseListener(func);
}
GSError BufferQueueConsumer::UnregisterConsumerListener()
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->UnregisterConsumerListener();
}

GSError BufferQueueConsumer::SetDefaultWidthAndHeight(int32_t width, int32_t height)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->SetDefaultWidthAndHeight(width, height);
}

GSError BufferQueueConsumer::SetDefaultUsage(uint32_t usage)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->SetDefaultUsage(usage);
}

void BufferQueueConsumer::Dump(std::string &result) const
{
    if (bufferQueue_ == nullptr) {
        return;
    }
    return bufferQueue_->Dump(result);
}
} // namespace OHOS
