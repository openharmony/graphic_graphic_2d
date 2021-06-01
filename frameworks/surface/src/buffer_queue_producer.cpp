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

#include "buffer_queue_producer.h"

#include "buffer_log.h"
#include "buffer_manager.h"
#include "buffer_utils.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "BufferQueueProducer" };
}

BufferQueueProducer::BufferQueueProducer(sptr<BufferQueue>& bufferQueue)
{
    BLOGFD("");
    bufferQueue_ = bufferQueue;

    memberFuncMap_[BUFFER_PRODUCER_REQUEST_BUFFER] = &BufferQueueProducer::RequestBufferInner;
    memberFuncMap_[BUFFER_PRODUCER_CANCEL_BUFFER] = &BufferQueueProducer::CancelBufferInner;
    memberFuncMap_[BUFFER_PRODUCER_FLUSH_BUFFER] = &BufferQueueProducer::FlushBufferInner;
    memberFuncMap_[BUFFER_PRODUCER_GET_QUEUE_SIZE] = &BufferQueueProducer::GetQueueSizeInner;
    memberFuncMap_[BUFFER_PRODUCER_SET_QUEUE_SIZE] = &BufferQueueProducer::SetQueueSizeInner;
    memberFuncMap_[BUFFER_PRODUCER_GET_DEFAULT_WIDTH] = &BufferQueueProducer::GetDefaultWidthInner;
    memberFuncMap_[BUFFER_PRODUCER_GET_DEFAULT_HEIGHT] = &BufferQueueProducer::GetDefaultHeightInner;
    memberFuncMap_[BUFFER_PRODUCER_CLEAN_CACHE] = &BufferQueueProducer::CleanCacheInner;
}

BufferQueueProducer::~BufferQueueProducer()
{
    BLOGFD("");
}

int BufferQueueProducer::OnRemoteRequest(uint32_t code, MessageParcel& arguments,
                                         MessageParcel& reply, MessageOption& option)
{
    auto it = memberFuncMap_.find(code);
    if (it == memberFuncMap_.end()) {
        BLOG_FAILURE("cannot process %{public}u", code);
        return 0;
    }

    if (it->second == nullptr) {
        BLOG_FAILURE("memberFuncMap_[%{public}u] is nullptr", code);
        return 0;
    }

    auto remoteDescriptor = arguments.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    return (this->*(it->second))(arguments, reply, option);
}

int BufferQueueProducer::RequestBufferInner(MessageParcel& arguments, MessageParcel& reply, MessageOption& option)
{
    int32_t sequence;
    sptr<SurfaceBuffer> buffer;
    int32_t fence = -1;
    BufferRequestConfig config;
    std::vector<int32_t> deletingBuffers;

    ReadRequestConfig(arguments, config);

    SurfaceError retval = RequestBuffer(sequence, buffer, fence, config, deletingBuffers);

    reply.WriteInt32(retval);
    if (retval == SURFACE_ERROR_OK) {
        sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(buffer);

        WriteFence(reply, fence);
        reply.WriteInt32Vector(deletingBuffers);
        WriteSurfaceBufferImpl(reply, sequence, bufferImpl);
    }
    return 0;
}

int BufferQueueProducer::CancelBufferInner(MessageParcel& arguments, MessageParcel& reply, MessageOption& option)
{
    int32_t sequence = arguments.ReadInt32();
    SurfaceError retval = CancelBuffer(sequence);
    reply.WriteInt32(retval);
    return 0;
}

int BufferQueueProducer::FlushBufferInner(MessageParcel& arguments, MessageParcel& reply, MessageOption& option)
{
    int32_t fence;
    int32_t sequence;
    BufferFlushConfig config;

    sequence = arguments.ReadInt32();
    ReadFence(arguments, fence);
    ReadFlushConfig(arguments, config);

    SurfaceError retval = FlushBuffer(sequence, fence, config);

    reply.WriteInt32(retval);
    return 0;
}

int BufferQueueProducer::GetQueueSizeInner(MessageParcel& arguments, MessageParcel& reply, MessageOption& option)
{
    reply.WriteInt32(GetQueueSize());
    return 0;
}

int BufferQueueProducer::SetQueueSizeInner(MessageParcel& arguments, MessageParcel& reply, MessageOption& option)
{
    int32_t queueSize = arguments.ReadInt32();
    SurfaceError retval = SetQueueSize(queueSize);
    reply.WriteInt32(retval);
    return 0;
}

int BufferQueueProducer::GetDefaultWidthInner(MessageParcel& arguments, MessageParcel& reply, MessageOption& option)
{
    reply.WriteInt32(GetDefaultWidth());
    return 0;
}

int BufferQueueProducer::GetDefaultHeightInner(MessageParcel& arguments, MessageParcel& reply, MessageOption& option)
{
    reply.WriteInt32(GetDefaultHeight());
    return 0;
}

int BufferQueueProducer::CleanCacheInner(MessageParcel& arguments, MessageParcel& reply, MessageOption& option)
{
    reply.WriteInt32(CleanCache());
    return 0;
}

SurfaceError BufferQueueProducer::RequestBuffer(int32_t& sequence, sptr<SurfaceBuffer>& buffer,
    int32_t& fence, BufferRequestConfig& config, std::vector<int32_t>& deletingBuffers)
{
    if (bufferQueue_ == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }
    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(buffer);
    SurfaceError ret = bufferQueue_->RequestBuffer(sequence, bufferImpl, fence, config, deletingBuffers);
    buffer = bufferImpl;
    return ret;
}

SurfaceError BufferQueueProducer::CancelBuffer(int32_t sequence)
{
    if (bufferQueue_ == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }
    return bufferQueue_->CancelBuffer(sequence);
}

SurfaceError BufferQueueProducer::FlushBuffer(int32_t sequence,
                                              int32_t fence, BufferFlushConfig& config)
{
    if (bufferQueue_ == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }
    return bufferQueue_->FlushBuffer(sequence, fence, config);
}

uint32_t BufferQueueProducer::GetQueueSize()
{
    if (bufferQueue_ == nullptr) {
        return 0;
    }
    return bufferQueue_->GetQueueSize();
}

SurfaceError BufferQueueProducer::SetQueueSize(uint32_t queueSize)
{
    if (bufferQueue_ == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }
    return bufferQueue_->SetQueueSize(queueSize);
}

int32_t BufferQueueProducer::GetDefaultWidth()
{
    if (bufferQueue_ == nullptr) {
        return 0;
    }
    return bufferQueue_->GetDefaultWidth();
}

int32_t BufferQueueProducer::GetDefaultHeight()
{
    if (bufferQueue_ == nullptr) {
        return 0;
    }
    return bufferQueue_->GetDefaultHeight();
}

SurfaceError BufferQueueProducer::CleanCache()
{
    if (bufferQueue_ == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }
    return bufferQueue_->CleanCache();
}
}; // namespace OHOS
