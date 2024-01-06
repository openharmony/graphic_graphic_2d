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

namespace OHOS {
BufferQueueConsumer::BufferQueueConsumer(sptr<BufferQueue>& bufferQueue)
{
    bufferQueue_ = bufferQueue;
    if (bufferQueue_ != nullptr) {
        bufferQueue_->GetName(name_);
    }
}

BufferQueueConsumer::~BufferQueueConsumer()
{
}

GSError BufferQueueConsumer::AcquireBuffer(sptr<SurfaceBuffer>& buffer, sptr<SyncFence>& fence,
    int64_t &timestamp, std::vector<Rect> &damages)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->AcquireBuffer(buffer, fence, timestamp, damages);
}

GSError BufferQueueConsumer::ReleaseBuffer(sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& fence)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->ReleaseBuffer(buffer, fence);
}

GSError BufferQueueConsumer::AttachBuffer(sptr<SurfaceBuffer>& buffer)
{
    int32_t timeOut = 0;
    return AttachBuffer(buffer, timeOut);
}

GSError BufferQueueConsumer::AttachBuffer(sptr<SurfaceBuffer>& buffer, int32_t timeOut)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->AttachBuffer(buffer, timeOut);
}

GSError BufferQueueConsumer::DetachBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->DetachBuffer(buffer);
}

GSError BufferQueueConsumer::RegisterSurfaceDelegator(sptr<IRemoteObject> client, sptr<Surface> cSurface)
{
    if (bufferQueue_ == nullptr) {
        BLOGFE("RegisterSurfaceDelegator failed for nullptr bufferqueue.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->RegisterSurfaceDelegator(client, cSurface);
}

bool BufferQueueConsumer::QueryIfBufferAvailable()
{
    if (bufferQueue_ == nullptr) {
        return false;
    }
    return bufferQueue_->QueryIfBufferAvailable();
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

GSError BufferQueueConsumer::RegisterDeleteBufferListener(OnDeleteBufferFunc func, bool isForUniRedraw)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->RegisterDeleteBufferListener(func, isForUniRedraw);
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

GraphicTransformType BufferQueueConsumer::GetTransform() const
{
    if (bufferQueue_ == nullptr) {
        return GraphicTransformType::GRAPHIC_ROTATE_BUTT;
    }
    return bufferQueue_->GetTransform();
}

GSError BufferQueueConsumer::GetScalingMode(uint32_t sequence, ScalingMode &scalingMode) const
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->GetScalingMode(sequence, scalingMode);
}

GSError BufferQueueConsumer::QueryMetaDataType(uint32_t sequence, HDRMetaDataType &type) const
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->QueryMetaDataType(sequence, type);
}

GSError BufferQueueConsumer::GetMetaData(uint32_t sequence, std::vector<GraphicHDRMetaData> &metaData) const
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->GetMetaData(sequence, metaData);
}

GSError BufferQueueConsumer::GetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey &key,
                                            std::vector<uint8_t> &metaData) const
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->GetMetaDataSet(sequence, key, metaData);
}

sptr<SurfaceTunnelHandle> BufferQueueConsumer::GetTunnelHandle() const
{
    if (bufferQueue_ == nullptr) {
        return nullptr;
    }
    return bufferQueue_->GetTunnelHandle();
}

GSError BufferQueueConsumer::SetPresentTimestamp(uint32_t sequence, const GraphicPresentTimestamp &timestamp)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->SetPresentTimestamp(sequence, timestamp);
}

bool BufferQueueConsumer::GetStatus() const
{
    return bufferQueue_->GetStatus();
}

void BufferQueueConsumer::SetStatus(bool status)
{
    bufferQueue_->SetStatus(status);
}

GSError BufferQueueConsumer::OnConsumerDied()
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->OnConsumerDied();
}

GSError BufferQueueConsumer::GoBackground()
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->GoBackground();
}
} // namespace OHOS
