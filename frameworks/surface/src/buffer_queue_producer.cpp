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
    memberFuncMap_[BUFFER_PRODUCER_SET_SCALING_MODE] = &BufferQueueProducer::SetScalingModeRemote;
    memberFuncMap_[BUFFER_PRODUCER_SET_METADATA] = &BufferQueueProducer::SetMetaDataRemote;
    memberFuncMap_[BUFFER_PRODUCER_SET_METADATASET] = &BufferQueueProducer::SetMetaDataSetRemote;
    memberFuncMap_[BUFFER_PRODUCER_SET_TUNNEL_HANDLE] = &BufferQueueProducer::SetTunnelHandleRemote;
    memberFuncMap_[BUFFER_PRODUCER_GO_BACKGROUND] = &BufferQueueProducer::GoBackgroundRemote;
    memberFuncMap_[BUFFER_PRODUCER_GET_PRESENT_TIMESTAMP] = &BufferQueueProducer::GetPresentTimestampRemote;
}

BufferQueueProducer::~BufferQueueProducer()
{
}

GSError BufferQueueProducer::CheckConnectLocked()
{
    if (connectedPid_ == 0) {
        BLOGNI("this BufferQueue has no connections");
        return GSERROR_INVALID_OPERATING;
    }

    if (connectedPid_ != GetCallingPid()) {
        BLOGNW("this BufferQueue has been connected by :%{public}d, you can not disconnect", connectedPid_);
        return GSERROR_INVALID_OPERATING;
    }

    return GSERROR_OK;
}

int32_t BufferQueueProducer::OnRemoteRequest(uint32_t code, MessageParcel &arguments,
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

int32_t BufferQueueProducer::CancelBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    uint32_t sequence;
    sptr<BufferExtraData> bedataimpl = new BufferExtraDataImpl;

    sequence = arguments.ReadUint32();
    bedataimpl->ReadFromParcel(arguments);

    GSError sret = CancelBuffer(sequence, bedataimpl);
    reply.WriteInt32(sret);
    return 0;
}

int32_t BufferQueueProducer::FlushBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    uint32_t sequence;
    BufferFlushConfig config;
    sptr<BufferExtraData> bedataimpl = new BufferExtraDataImpl;

    sequence = arguments.ReadUint32();
    bedataimpl->ReadFromParcel(arguments);

    sptr<SyncFence> fence = SyncFence::ReadFromMessageParcel(arguments);
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

int32_t BufferQueueProducer::GetQueueSizeRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    reply.WriteInt32(GetQueueSize());
    return 0;
}

int32_t BufferQueueProducer::SetQueueSizeRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    int32_t queueSize = arguments.ReadInt32();
    GSError sret = SetQueueSize(queueSize);
    reply.WriteInt32(sret);
    return 0;
}

int32_t BufferQueueProducer::GetNameRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    std::string name;
    auto sret = bufferQueue_->GetName(name);
    reply.WriteInt32(sret);
    if (sret == GSERROR_OK) {
        reply.WriteString(name);
    }
    return 0;
}

int32_t BufferQueueProducer::GetNameAndUniqueIdRemote(MessageParcel &arguments, MessageParcel &reply,
                                                      MessageOption &option)
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

int32_t BufferQueueProducer::GetDefaultWidthRemote(MessageParcel &arguments, MessageParcel &reply,
                                                   MessageOption &option)
{
    reply.WriteInt32(GetDefaultWidth());
    return 0;
}

int32_t BufferQueueProducer::GetDefaultHeightRemote(MessageParcel &arguments, MessageParcel &reply,
                                                    MessageOption &option)
{
    reply.WriteInt32(GetDefaultHeight());
    return 0;
}

int32_t BufferQueueProducer::GetDefaultUsageRemote(MessageParcel &arguments, MessageParcel &reply,
                                                   MessageOption &option)
{
    reply.WriteUint32(GetDefaultUsage());
    return 0;
}

int32_t BufferQueueProducer::GetUniqueIdRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    reply.WriteUint64(GetUniqueId());
    return 0;
}

int32_t BufferQueueProducer::CleanCacheRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    reply.WriteInt32(CleanCache());
    return 0;
}

int32_t BufferQueueProducer::GoBackgroundRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    reply.WriteInt32(GoBackground());
    return 0;
}

int32_t BufferQueueProducer::RegisterReleaseListenerRemote(MessageParcel &arguments,
    MessageParcel &reply, MessageOption &option)
{
    BLOGNE("BufferQueueProducer::RegisterReleaseListenerRemote not support remote");
    return 0;
}

int32_t BufferQueueProducer::SetTransformRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    GraphicTransformType transform = static_cast<GraphicTransformType>(arguments.ReadUint32());
    GSError sret = SetTransform(transform);
    reply.WriteInt32(sret);
    return 0;
}

int32_t BufferQueueProducer::IsSupportedAllocRemote(MessageParcel &arguments, MessageParcel &reply,
                                                    MessageOption &option)
{
    std::vector<BufferVerifyAllocInfo> infos;
    ReadVerifyAllocInfo(arguments, infos);

    std::vector<bool> supporteds;
    GSError sret = IsSupportedAlloc(infos, supporteds);
    reply.WriteInt32(sret);
    if (sret == GSERROR_OK) {
        reply.WriteBoolVector(supporteds);
    }

    return 0;
}

int32_t BufferQueueProducer::DisconnectRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    GSError sret = Disconnect();
    reply.WriteInt32(sret);
    return 0;
}

int32_t BufferQueueProducer::SetScalingModeRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    uint32_t sequence = arguments.ReadUint32();
    ScalingMode scalingMode = static_cast<ScalingMode>(arguments.ReadInt32());
    GSError sret = SetScalingMode(sequence, scalingMode);
    reply.WriteInt32(sret);
    return 0;
}

int32_t BufferQueueProducer::SetMetaDataRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    uint32_t sequence = arguments.ReadUint32();
    std::vector<GraphicHDRMetaData> metaData;
    ReadHDRMetaData(arguments, metaData);
    GSError sret = SetMetaData(sequence, metaData);
    reply.WriteInt32(sret);
    return 0;
}

int32_t BufferQueueProducer::SetMetaDataSetRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option)
{
    uint32_t sequence = arguments.ReadUint32();
    GraphicHDRMetadataKey key = static_cast<GraphicHDRMetadataKey>(arguments.ReadUint32());
    std::vector<uint8_t> metaData;
    ReadHDRMetaDataSet(arguments, metaData);
    GSError sret = SetMetaDataSet(sequence, key, metaData);
    reply.WriteInt32(sret);
    return 0;
}

int32_t BufferQueueProducer::SetTunnelHandleRemote(MessageParcel &arguments, MessageParcel &reply,
                                                   MessageOption &option)
{
    sptr<SurfaceTunnelHandle> handle = nullptr;
    if (arguments.ReadBool()) {
        handle = new SurfaceTunnelHandle();
        ReadExtDataHandle(arguments, handle);
    }
    GSError sret = SetTunnelHandle(handle);
    reply.WriteInt32(sret);
    return 0;
}

int32_t BufferQueueProducer::GetPresentTimestampRemote(MessageParcel &arguments, MessageParcel &reply,
                                                       MessageOption &option)
{
    uint32_t sequence = arguments.ReadUint32();
    GraphicPresentTimestampType type = static_cast<GraphicPresentTimestampType>(arguments.ReadUint32());
    int64_t time = 0;
    GSError sret = GetPresentTimestamp(sequence, type, time);
    reply.WriteInt32(sret);
    if (sret == GSERROR_OK) {
        reply.WriteInt64(time);
    }
    return 0;
}

GSError BufferQueueProducer::RequestBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
                                           RequestBufferReturnValue &retval)
{
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

    return bufferQueue_->RequestBuffer(config, bedata, retval);
}

GSError BufferQueueProducer::CancelBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->CancelBuffer(sequence, bedata);
}

GSError BufferQueueProducer::FlushBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata,
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

GSError BufferQueueProducer::GoBackground()
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
    return bufferQueue_->SetProducerCacheCleanFlagLocked(true);
}

GSError BufferQueueProducer::RegisterReleaseListener(OnReleaseFunc func)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->RegisterReleaseListener(func);
}

GSError BufferQueueProducer::SetTransform(GraphicTransformType transform)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->SetTransform(transform);
}

GSError BufferQueueProducer::IsSupportedAlloc(const std::vector<BufferVerifyAllocInfo> &infos,
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
    return bufferQueue_->GoBackground();
}

GSError BufferQueueProducer::SetScalingMode(uint32_t sequence, ScalingMode scalingMode)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->SetScalingMode(sequence, scalingMode);
}

GSError BufferQueueProducer::SetMetaData(uint32_t sequence, const std::vector<GraphicHDRMetaData> &metaData)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    return bufferQueue_->SetMetaData(sequence, metaData);
}

GSError BufferQueueProducer::SetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey key,
                                            const std::vector<uint8_t> &metaData)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    return bufferQueue_->SetMetaDataSet(sequence, key, metaData);
}

GSError BufferQueueProducer::SetTunnelHandle(const sptr<SurfaceTunnelHandle> &handle)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->SetTunnelHandle(handle);
}

GSError BufferQueueProducer::SetTunnelHandle(const GraphicExtDataHandle *handle)
{
    sptr<SurfaceTunnelHandle> tunnelHandle = new SurfaceTunnelHandle();
    if (tunnelHandle->SetHandle(handle) != GSERROR_OK) {
        return GSERROR_INVALID_OPERATING;
    }
    return bufferQueue_->SetTunnelHandle(tunnelHandle);
}

GSError BufferQueueProducer::GetPresentTimestamp(uint32_t sequence, GraphicPresentTimestampType type, int64_t &time)
{
    if (bufferQueue_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return bufferQueue_->GetPresentTimestamp(sequence, type, time);
}

bool BufferQueueProducer::GetStatus() const
{
    return bufferQueue_->GetStatus();
}

void BufferQueueProducer::SetStatus(bool status)
{
    bufferQueue_->SetStatus(status);
}

sptr<NativeSurface> BufferQueueProducer::GetNativeSurface()
{
    BLOGND("BufferQueueProducer::GetNativeSurface not support.");
    return nullptr;
}
}; // namespace OHOS
