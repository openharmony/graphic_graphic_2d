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

#include "buffer_client_producer.h"

#include <iremote_stub.h>
#include "buffer_log.h"
#include "buffer_utils.h"
#include "sync_fence.h"
#include "message_option.h"
#include "securec.h"

#define DEFINE_MESSAGE_VARIABLES(arg, ret, opt, LOGE) \
    MessageOption opt;                                \
    MessageParcel arg;                                \
    MessageParcel ret;                                \
    if (!arg.WriteInterfaceToken(GetDescriptor())) {  \
        LOGE("write interface token failed");         \
    }

#define SEND_REQUEST(COMMAND, arguments, reply, option)                         \
    do {                                                                        \
        int32_t ret = Remote()->SendRequest(COMMAND, arguments, reply, option); \
        if (ret != ERR_NONE) {                                                  \
            BLOGN_FAILURE("SendRequest return %{public}d", ret);                 \
            return GSERROR_BINDER;                                  \
        }                                                                       \
    } while (0)

#define SEND_REQUEST_WITH_SEQ(COMMAND, arguments, reply, option, sequence)      \
    do {                                                                        \
        int32_t ret = Remote()->SendRequest(COMMAND, arguments, reply, option); \
        if (ret != ERR_NONE) {                                                  \
            BLOGN_FAILURE_ID(sequence, "SendRequest return %{public}d", ret);    \
            return GSERROR_BINDER;                                  \
        }                                                                       \
    } while (0)

#define CHECK_RETVAL_WITH_SEQ(reply, sequence)                          \
    do {                                                                \
        int32_t ret = reply.ReadInt32();                                \
        if (ret != GSERROR_OK) {                                  \
            BLOGN_FAILURE_ID(sequence, "Remote return %{public}d", ret); \
            return (GSError)ret;                                   \
        }                                                               \
    } while (0)

namespace OHOS {
namespace {
    int32_t g_CancelBufferConsecutiveFailedCount  = 0;
    constexpr int32_t MAX_COUNT = 2;
}
BufferClientProducer::BufferClientProducer(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IBufferProducer>(impl)
{
}

BufferClientProducer::~BufferClientProducer()
{
}

GSError BufferClientProducer::RequestBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
                                            RequestBufferReturnValue &retval)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    WriteRequestConfig(arguments, config);

    SEND_REQUEST(BUFFER_PRODUCER_REQUEST_BUFFER, arguments, reply, option);
    int32_t retCode = reply.ReadInt32();
    if (retCode != GSERROR_OK) {
        BLOGND("Remote return %{public}d", retCode);
        return (GSError)retCode;
    }

    GSError ret = ReadSurfaceBufferImpl(reply, retval.sequence, retval.buffer);
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Read surface buffer impl failed, return %{public}d", ret);
        return ret;
    }
    if (retval.buffer != nullptr) {
        retval.buffer->SetBufferRequestConfig(config);
    }

    ret = bedata->ReadFromParcel(reply);
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Read extra data from parcel failed, return %{public}d", ret);
        return ret;
    }
    retval.fence = SyncFence::ReadFromMessageParcel(reply);
    reply.ReadInt32Vector(&retval.deletingBuffers);

    return GSERROR_OK;
}

GSError BufferClientProducer::GetLastFlushedBuffer(sptr<SurfaceBuffer>& buffer,
    sptr<SyncFence>& fence, float matrix[16])
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    SEND_REQUEST(BUFFER_PRODUCER_GET_LAST_FLUSHED_BUFFER, arguments, reply, option);
    int32_t retCode = reply.ReadInt32();
    if (retCode != GSERROR_OK) {
        BLOGND("Remote return %{public}d", retCode);
        return (GSError)retCode;
    }
    uint32_t sequence;
    GSError ret = ReadSurfaceBufferImpl(reply, sequence, buffer);
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Read surface buffer impl failed, return %{public}d", ret);
        return ret;
    }
    ret = buffer->ReadBufferRequestConfig(reply);
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("ReadBufferRequestConfig failed, return %{public}d", ret);
        return ret;
    }

    fence = SyncFence::ReadFromMessageParcel(reply);
    std::vector<float> readMatrixVector;
    reply.ReadFloatVector(&readMatrixVector);
    if (memcpy_s(matrix, readMatrixVector.size() * sizeof(float),
        readMatrixVector.data(), readMatrixVector.size() * sizeof(float)) != EOK) {
        BLOGN_FAILURE("memcpy_s fail");
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}

GSError BufferClientProducer::CancelBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    arguments.WriteUint32(sequence);
    bedata->WriteToParcel(arguments);

    SEND_REQUEST_WITH_SEQ(BUFFER_PRODUCER_CANCEL_BUFFER, arguments, reply, option, sequence);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        g_CancelBufferConsecutiveFailedCount++;
        if (g_CancelBufferConsecutiveFailedCount < MAX_COUNT) {
            BLOGN_FAILURE_ID(sequence, "Remote return %{public}d", ret);
        }
        return (GSError)ret;
    }
    g_CancelBufferConsecutiveFailedCount = 0;
    return GSERROR_OK;
}

GSError BufferClientProducer::FlushBuffer(uint32_t sequence, const sptr<BufferExtraData> &bedata,
                                          const sptr<SyncFence>& fence, BufferFlushConfigWithDamages &config)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    arguments.WriteUint32(sequence);
    bedata->WriteToParcel(arguments);
    fence->WriteToMessageParcel(arguments);
    WriteFlushConfig(arguments, config);

    SEND_REQUEST_WITH_SEQ(BUFFER_PRODUCER_FLUSH_BUFFER, arguments, reply, option, sequence);
    CHECK_RETVAL_WITH_SEQ(reply, sequence);

    return GSERROR_OK;
}

GSError BufferClientProducer::AttachBufferToQueue(sptr<SurfaceBuffer>& buffer)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);
    int32_t sequence = buffer->GetSeqNum();
    WriteSurfaceBufferImpl(arguments, sequence, buffer);
    auto ret = buffer->WriteBufferRequestConfig(arguments);
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("WriteBufferRequestConfig failed, return %{public}d", ret);
        return ret;
    }
    SEND_REQUEST_WITH_SEQ(BUFFER_PRODUCER_ATTACH_BUFFER_TO_QUEUE, arguments, reply, option, sequence);
    CHECK_RETVAL_WITH_SEQ(reply, sequence);
    return GSERROR_OK;
}

GSError BufferClientProducer::DetachBufferFromQueue(sptr<SurfaceBuffer>& buffer)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);
    int32_t sequence = buffer->GetSeqNum();
    WriteSurfaceBufferImpl(arguments, sequence, buffer);
    SEND_REQUEST_WITH_SEQ(BUFFER_PRODUCER_DETACH_BUFFER_FROM_QUEUE, arguments, reply, option, sequence);
    CHECK_RETVAL_WITH_SEQ(reply, sequence);
    return GSERROR_OK;
}

GSError BufferClientProducer::AttachBuffer(sptr<SurfaceBuffer>& buffer)
{
    return GSERROR_NOT_SUPPORT;
}

GSError BufferClientProducer::AttachBuffer(sptr<SurfaceBuffer>& buffer, int32_t timeOut)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);
    uint32_t sequence = buffer->GetSeqNum();
    WriteSurfaceBufferImpl(arguments, sequence, buffer);
    arguments.WriteInt32(timeOut);
    SEND_REQUEST_WITH_SEQ(BUFFER_PRODUCER_ATTACH_BUFFER, arguments, reply, option, sequence);
    CHECK_RETVAL_WITH_SEQ(reply, sequence);
    return GSERROR_OK;
}

GSError BufferClientProducer::DetachBuffer(sptr<SurfaceBuffer>& buffer)
{
    return GSERROR_NOT_SUPPORT;
}

GSError BufferClientProducer::RegisterReleaseListener(sptr<IProducerListener> listener)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    arguments.WriteRemoteObject(listener->AsObject());

    SEND_REQUEST(BUFFER_PRODUCER_REGISTER_RELEASE_LISTENER, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return (GSError)ret;
    }
    return GSERROR_OK;
}

GSError BufferClientProducer::UnRegisterReleaseListener()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);
    SEND_REQUEST(BUFFER_PRODUCER_UNREGISTER_RELEASE_LISTENER, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return (GSError)ret;
    }
    return GSERROR_OK;
}

uint32_t BufferClientProducer::GetQueueSize()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    SEND_REQUEST(BUFFER_PRODUCER_GET_QUEUE_SIZE, arguments, reply, option);

    return reply.ReadUint32();
}

GSError BufferClientProducer::SetQueueSize(uint32_t queueSize)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    arguments.WriteInt32(queueSize);

    SEND_REQUEST(BUFFER_PRODUCER_SET_QUEUE_SIZE, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return (GSError)ret;
    }

    return GSERROR_OK;
}

GSError BufferClientProducer::GetName(std::string &name)
{
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (name_ != "not init") {
            name = name_;
            return GSERROR_OK;
        }
    }
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    SEND_REQUEST(BUFFER_PRODUCER_GET_NAME, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return static_cast<GSError>(ret);
    }
    if (reply.ReadString(name) == false) {
        BLOGN_FAILURE("reply.ReadString return false");
        return GSERROR_BINDER;
    }
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        name_ = name;
    }
    return static_cast<GSError>(ret);
}

uint64_t BufferClientProducer::GetUniqueId()
{
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (uniqueId_ != 0) {
            return uniqueId_;
        }
    }
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);
    SEND_REQUEST(BUFFER_PRODUCER_GET_UNIQUE_ID, arguments, reply, option);
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        uniqueId_ = reply.ReadUint64();
    }
    return uniqueId_;
}

GSError BufferClientProducer::GetNameAndUniqueId(std::string& name, uint64_t& uniqueId)
{
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (uniqueId_ != 0 && name_ != "not init") {
            uniqueId = uniqueId_;
            name = name_;
            return GSERROR_OK;
        }
    }
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    SEND_REQUEST(BUFFER_PRODUCER_GET_NAMEANDUNIQUEDID, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return static_cast<GSError>(ret);
    }
    if (reply.ReadString(name) == false) {
        BLOGN_FAILURE("reply.ReadString return false");
        return GSERROR_BINDER;
    }

    uniqueId = reply.ReadUint64();
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        name_ = name;
        uniqueId_ = uniqueId;
    }
    return static_cast<GSError>(ret);
}

int32_t BufferClientProducer::GetDefaultWidth()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    SEND_REQUEST(BUFFER_PRODUCER_GET_DEFAULT_WIDTH, arguments, reply, option);

    return reply.ReadInt32();
}

int32_t BufferClientProducer::GetDefaultHeight()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    SEND_REQUEST(BUFFER_PRODUCER_GET_DEFAULT_HEIGHT, arguments, reply, option);

    return reply.ReadInt32();
}

uint32_t BufferClientProducer::GetDefaultUsage()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    SEND_REQUEST(BUFFER_PRODUCER_GET_DEFAULT_USAGE, arguments, reply, option);

    return reply.ReadUint32();
}

GSError BufferClientProducer::CleanCache()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    SEND_REQUEST(BUFFER_PRODUCER_CLEAN_CACHE, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return (GSError)ret;
    }

    return GSERROR_OK;
}

GSError BufferClientProducer::GoBackground()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    SEND_REQUEST(BUFFER_PRODUCER_GO_BACKGROUND, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return (GSError)ret;
    }

    return GSERROR_OK;
}

GSError BufferClientProducer::SetTransform(GraphicTransformType transform)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    arguments.WriteUint32(static_cast<uint32_t>(transform));

    SEND_REQUEST(BUFFER_PRODUCER_SET_TRANSFORM, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return (GSError)ret;
    }

    return GSERROR_OK;
}

GSError BufferClientProducer::IsSupportedAlloc(const std::vector<BufferVerifyAllocInfo> &infos,
                                               std::vector<bool> &supporteds)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    WriteVerifyAllocInfo(arguments, infos);

    SEND_REQUEST(BUFFER_PRODUCER_IS_SUPPORTED_ALLOC, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return static_cast<GSError>(ret);
    }

    if (reply.ReadBoolVector(&supporteds) == false) {
        BLOGN_FAILURE("reply.ReadBoolVector return false");
        return GSERROR_BINDER;
    }

    return static_cast<GSError>(ret);
}

GSError BufferClientProducer::Disconnect()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    SEND_REQUEST(BUFFER_PRODUCER_DISCONNECT, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return static_cast<GSError>(ret);
    }
    return GSERROR_OK;
}

GSError BufferClientProducer::SetScalingMode(uint32_t sequence, ScalingMode scalingMode)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);
    arguments.WriteUint32(sequence);
    arguments.WriteInt32(static_cast<int32_t>(scalingMode));
    SEND_REQUEST(BUFFER_PRODUCER_SET_SCALING_MODE, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return (GSError)ret;
    }

    return GSERROR_OK;
}

GSError BufferClientProducer::SetMetaData(uint32_t sequence, const std::vector<GraphicHDRMetaData> &metaData)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);
    arguments.WriteUint32(sequence);
    WriteHDRMetaData(arguments, metaData);
    SEND_REQUEST(BUFFER_PRODUCER_SET_METADATA, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return (GSError)ret;
    }

    return GSERROR_OK;
}

GSError BufferClientProducer::SetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey key,
                                             const std::vector<uint8_t> &metaData)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);
    arguments.WriteUint32(sequence);
    arguments.WriteUint32(static_cast<uint32_t>(key));
    WriteHDRMetaDataSet(arguments, metaData);
    SEND_REQUEST(BUFFER_PRODUCER_SET_METADATASET, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return (GSError)ret;
    }

    return GSERROR_OK;
}

GSError BufferClientProducer::SetTunnelHandle(const GraphicExtDataHandle *handle)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);
    if (handle == nullptr) {
        arguments.WriteBool(false);
    } else {
        arguments.WriteBool(true);
        WriteExtDataHandle(arguments, handle);
    }
    SEND_REQUEST(BUFFER_PRODUCER_SET_TUNNEL_HANDLE, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return (GSError)ret;
    }
    return GSERROR_OK;
}

GSError BufferClientProducer::GetPresentTimestamp(uint32_t sequence, GraphicPresentTimestampType type, int64_t &time)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);
    arguments.WriteUint32(sequence);
    arguments.WriteUint32(static_cast<uint32_t>(type));
    SEND_REQUEST(BUFFER_PRODUCER_GET_PRESENT_TIMESTAMP, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return static_cast<GSError>(ret);
    }
    time = reply.ReadInt64();
    return static_cast<GSError>(ret);
}

sptr<NativeSurface> BufferClientProducer::GetNativeSurface()
{
    BLOGND("BufferClientProducer::GetNativeSurface not support.");
    return nullptr;
}

GSError BufferClientProducer::SendDeathRecipientObject()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);
    token_ = new IRemoteStub<IBufferProducerToken>();
    arguments.WriteRemoteObject(token_->AsObject());
    SEND_REQUEST(BUFFER_PRODUCER_REGISTER_DEATH_RECIPIENT, arguments, reply, option);

    int32_t ret = reply.ReadInt32();
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", ret);
        return static_cast<GSError>(ret);
    }
    return GSERROR_OK;
}

GSError BufferClientProducer::GetTransform(GraphicTransformType &transform)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);
    SEND_REQUEST(BUFFER_PRODUCER_GET_TRANSFORM, arguments, reply, option);

    auto ret = static_cast<GSError>(reply.ReadInt32());
    if (ret != GSERROR_OK) {
        BLOGN_FAILURE("Remote return %{public}d", static_cast<int>(ret));
        return ret;
    }
    transform = static_cast<GraphicTransformType>(reply.ReadUint32());
    return GSERROR_OK;
}
}; // namespace OHOS
