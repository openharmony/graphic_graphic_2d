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

#include <mutex>
#include <set>

#include "buffer_extra_data_impl.h"
#include "buffer_log.h"
#include "buffer_manager.h"
#include "buffer_utils.h"
#include "sync_fence.h"

namespace OHOS {
BufferQueueProducer::BufferQueueProducer(sptr<BufferQueue>& bufferQueue)
{
    bufferQueue_ = bufferQueue;
    if (bufferQueue_ != nullptr) {
        bufferQueue_->GetName(name_);
    }
    BLOGNI("ctor");

    memberFuncMap_[BUFFER_PRODUCER_REQUEST_BUFFER] = &BufferQueueProducer::RequestBufferRemote;
    memberFuncMap_[BUFFER_PRODUCER_CANCEL_BUFFER] = &BufferQueueProducer::CancelBufferRemote;
    memberFuncMap_[BUFFER_PRODUCER_FLUSH_BUFFER] = &BufferQueueProducer::FlushBufferRemote;
    memberFuncMap_[BUFFER_PRODUCER_ATTACH_BUFFER] = &BufferQueueProducer::AttachBufferRemote;
    memberFuncMap_[BUFFER_PRODUCER_DETACH_BUFFER] = &BufferQueueProducer::DetachBufferRemote;
    memberFuncMap_[BUFFER_PRODUCER_GET_QUEUE_SIZE] = &BufferQueueProducer::GetQueueSizeRemote;
    memberFuncMap_[BUFFER_PRODUCER_SET_QUEUE_SIZE] = &BufferQueueProducer::SetQueueSizeRemote;
    memberFuncMap_[BUFFER_PRODUCER_GET_NAME] = &BufferQueueProducer::GetNameRemote;
    memberFuncMap_[BUFFER_PRODUCER_GET_DEFAULT_WIDTH] = &BufferQueueProducer::GetDefaultWidthRemote;
    memberFuncMap_[BUFFER_PRODUCER_GET_DEFAULT_HEIGHT] = &BufferQueueProducer::GetDefaultHeightRemote;
    memberFuncMap_[BUFFER_PRODUCER_GET_DEFAULT_USAGE] = &BufferQueueProducer::GetDefaultUsageRemote;
    memberFuncMap_[BUFFER_PRODUCER_GET_UNIQUE_ID] = &BufferQueueProducer::GetUniqueIdRemote;
    memberFuncMap_[BUFFER_PRODUCER_CLEAN_CACHE] = &BufferQueueProducer::CleanCacheRemote;
    memberFuncMap_[BUFFER_PRODUCER_REGISTER_RELEASE_LISTENER] = &BufferQueueProducer::RegisterReleaseListenerRemote;
    memberFuncMap_[BUFFER_PRODUCER_SET_TRANSFORM] = &BufferQueueProducer::SetTransformRemote;
    memberFuncMap_[BUFFER_PRODUCER_IS_SUPPORTED_ALLOC] = &BufferQueueProducer::IsSupportedAllocRemote;
    memberFuncMap_[BUFFER_PRODUCER_GET_NAMEANDUNIQUEDID] = &BufferQueueProducer::GetNameAndUniqueIdRemote;
    memberFuncMap_[BUFFER_PRODUCER_DISCONNECT] = &BufferQueueProducer::DisconnectRemote;
}

BufferQueueProducer::~BufferQueueProducer()
{
    BLOGNI("dtor");
}

GSError BufferQueueProducer::CheckConnectLocked()
{
    if (connectedPid_ == 0) {
        BLOGNI("this BufferQueue has no connections");
        return GSERROR_OK;
    }

    if (connectedPid_ != GetCallingPid()) {
        BLOGNW("this BufferQueue has been connected by :%{public}d, you can not disconnect", connectedPid_);
        return GSERROR_INVALID_OPERATING;
    }

    return GSERROR_OK;
}

int BufferQueueProducer::OnRemoteRequest(uint32_t code, MessageParcel &arguments,
                                         MessageParcel &reply, MessageOption &option)
{
    auto it = memberFuncMap_.find(code);
    if (it == memberFuncMap_.end()) {
        BLOGN_FAILURE("cannot process %{public}u", code);
        return 0;
    }

    if (it->second == nullptr) {
        BLOGN_FAILURE("memberFuncMap_[%{public}u] is nullptr", code);
        return 0;
    }

    auto remoteDescriptor = arguments.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    auto ret = (this->*(it->second))(arguments, reply, option);
    return ret;
}

int32_t BufferQueueProducer::RequestBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    RequestBufferReturnValue retval;
    sptr<BufferExtraData> bedataimpl = new BufferExtraDataImpl;
    BufferRequestConfig config = {};

    ReadRequestConfig(arguments, config);

    GSError sret = RequestBuffer(config, bedataimpl, retval);

    reply.WriteInt32(sret);
    if (sret == GSERROR_OK) {
        WriteSurfaceBufferImpl(reply, retval.sequence, retval.buffer);
        bedataimpl->WriteToParcel(reply);
        retval.fence->WriteToMessageParcel(reply);
        reply.WriteInt32Vector(retval.deletingBuffers);
    }
    return 0;
}

int BufferQueueProducer::CancelBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    int32_t sequence;
    sptr<BufferExtraData> bedataimpl = new BufferExtraDataImpl;

    sequence = arguments.ReadInt32();
    bedataimpl->ReadFromParcel(arguments);

    GSError sret = CancelBuffer(sequence, bedataimpl);
    reply.WriteInt32(sret);
    return 0;
}

int BufferQueueProducer::FlushBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
    int32_t sequence;
    BufferFlushConfig config;
    sptr<BufferExtraData> bedataimpl = new BufferExtraDataImpl;

    sequence = arguments.ReadInt32();
    bedataimpl->ReadFromParcel(arguments);

    fence->ReadFromMessageParcel(arguments);
    ReadFlushConfig(arguments, config);

    GSError sret = FlushBuffer(sequence, bedataimpl, fence, config);

    reply.WriteInt32(sret);
    return 0;
}

int32_t BufferQueueProducer::AttachBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    BLOGNE("BufferQueueProducer::AttachBufferRemote not support remote");
    return 0;
}

int32_t BufferQueueProducer::DetachBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    BLOGNE("BufferQueueProducer::DetachBufferRemote not support remote");
    return 0;
}

int BufferQueueProducer::GetQueueSizeRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    reply.WriteInt32(GetQueueSize());
    return 0;
}

int BufferQueueProducer::SetQueueSizeRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    int32_t queueSize = arguments.ReadInt32();
    GSError sret = SetQueueSize(queueSize);
    reply.WriteInt32(sret);
    return 0;
}

int BufferQueueProducer::GetNameRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    std::string name;
    auto sret = bufferQueue_->GetName(name);
    reply.WriteInt32(sret);
    if (sret == GSERROR_OK) {
        reply.WriteString(name);
    }
    return 0;
}

int BufferQueueProducer::GetNameAndUniqueIdRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    std::string name = "not init";
    uint64_t uniqueId = 0;
    auto ret = GetNameAndUniqueId(name, uniqueId);
    reply.WriteInt32(ret);
    if (ret == GSERROR_OK) {
        reply.WriteString(name);
        reply.WriteUint64(uniqueId);
    }
    return 0;
}

int BufferQueueProducer::GetDefaultWidthRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    reply.WriteInt32(GetDefaultWidth());
    return 0;
}

int BufferQueueProducer::GetDefaultHeightRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    reply.WriteInt32(GetDefaultHeight());
    return 0;
}

int BufferQueueProducer::GetDefaultUsageRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    reply.WriteUint32(GetDefaultUsage());
    return 0;
}

int BufferQueueProducer::GetUniqueIdRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    reply.WriteUint64(GetUniqueId());
    return 0;
}

int BufferQueueProducer::CleanCacheRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    reply.WriteInt32(CleanCache());
    return 0;
}

int32_t BufferQueueProducer::RegisterReleaseListenerRemote(MessageParcel &arguments,
    MessageParcel &reply, MessageOption &option)
{
    BLOGNE("BufferQueueProducer::RegisterReleaseListenerRemote not support remote");
    return 0;
}

int BufferQueueProducer::SetTransformRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    TransformType transform = static_cast<TransformType>(arguments.ReadUint32());
    GSError sret = SetTransform(transform);
    reply.WriteInt32(sret);
    return 0;
}

int BufferQueueProducer::IsSupportedAllocRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    std::vector<VerifyAllocInfo> infos;
    ReadVerifyAllocInfo(arguments, infos);

    std::vector<bool> supporteds;
    GSError sret = IsSupportedAlloc(infos, supporteds);
    reply.WriteInt32(sret);
    if (sret == GSERROR_OK) {
        reply.WriteBoolVector(supporteds);
    }

    return 0;
}

int BufferQueueProducer::DisconnectRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    GSError sret = Disconnect();
    reply.WriteInt32(sret);
    return 0;
}

GSError BufferQueueProducer::RequestBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
                                           RequestBufferReturnValue &retval)
{
    static std::map<int32_t, wptr<SurfaceBuffer>> cache;
    static std::map<pid_t, std::set<int32_t>> sendeds;
    static std::mutex mutex;
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (connectedPid_ != 0 && connectedPid_ != GetCallingPid()) {
            BLOGNW("this BufferQueue has been connected by :%{public}d", connectedPid_);
            return GSERROR_INVALID_OPERATING;
        }
        connectedPid_ = GetCallingPid();
    }

    auto sret = bufferQueue_->RequestBuffer(config, bedata, retval);

    std::lock_guard<std::mutex> lock(mutex);
    auto callingPid = GetCallingPid();
    auto &sended = sendeds[callingPid];
    if (sret == GSERROR_OK) {
        if (retval.buffer != nullptr) {
            cache[retval.sequence] = retval.buffer;
            sended.insert(retval.sequence);
        } else if (callingPid == getpid()) {
            // for BufferQueue not first
            // A local call always returns a non-null pointer
            retval.buffer = cache[retval.sequence].promote();
        } else if (sended.find(retval.sequence) == sended.end()) {
            // The first remote call from a different process returns a non-null pointer
            retval.buffer = cache[retval.sequence].promote();
            sended.insert(retval.sequence);
        }
    } else {
        BLOGNI("BufferQueue::RequestBuffer failed with %{public}s", GSErrorStr(sret).c_str());
    }

    for (const auto &buffer : retval.deletingBuffers) {
        cache.erase(buffer);
        sended.erase(buffer);
    }
    return sret;
}

GSError BufferQueueProducer::CancelBuffer(int32_t sequence, const sptr<BufferExtraData> &bedata)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->CancelBuffer(sequence, bedata);
}

GSError BufferQueueProducer::FlushBuffer(int32_t sequence, const sptr<BufferExtraData> &bedata,
                                         const sptr<SyncFence>& fence, BufferFlushConfig &config)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->FlushBuffer(sequence, bedata, fence, config);
}

GSError BufferQueueProducer::AttachBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->AttachBuffer(buffer);
}

GSError BufferQueueProducer::DetachBuffer(sptr<SurfaceBuffer>& buffer)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->DetachBuffer(buffer);
}

uint32_t BufferQueueProducer::GetQueueSize()
{
    if (bufferQueue_ == nullptr) {
        return 0;
    }
    return bufferQueue_->GetQueueSize();
}

GSError BufferQueueProducer::SetQueueSize(uint32_t queueSize)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->SetQueueSize(queueSize);
}

GSError BufferQueueProducer::GetName(std::string &name)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->GetName(name);
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

uint32_t BufferQueueProducer::GetDefaultUsage()
{
    if (bufferQueue_ == nullptr) {
        return 0;
    }
    return bufferQueue_->GetDefaultUsage();
}

uint64_t BufferQueueProducer::GetUniqueId()
{
    if (bufferQueue_ == nullptr) {
        return 0;
    }
    return bufferQueue_->GetUniqueId();
}

GSError BufferQueueProducer::CleanCache()
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto ret = CheckConnectLocked();
        if (ret != GSERROR_OK) {
            return ret;
        }
    }

    return bufferQueue_->CleanCache();
}

GSError BufferQueueProducer::RegisterReleaseListener(OnReleaseFunc func)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->RegisterReleaseListener(func);
}

GSError BufferQueueProducer::SetTransform(TransformType transform)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->SetTransform(transform);
}

GSError BufferQueueProducer::IsSupportedAlloc(const std::vector<VerifyAllocInfo> &infos,
                                              std::vector<bool> &supporteds)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    return bufferQueue_->IsSupportedAlloc(infos, supporteds);
}

GSError BufferQueueProducer::GetNameAndUniqueId(std::string& name, uint64_t& uniqueId)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    uniqueId = GetUniqueId();
    return GetName(name);
}

GSError BufferQueueProducer::Disconnect()
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto ret = CheckConnectLocked();
        if (ret != GSERROR_OK) {
            return ret;
        }
        connectedPid_ = 0;
    }
    return bufferQueue_->CleanCache();
}
}; // namespace OHOS
