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

#include "buffer_log.h"
#include "buffer_manager.h"
#include "buffer_utils.h"

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
BufferClientProducer::BufferClientProducer(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IBufferProducer>(impl)
{
    BLOGNI("ctor");
}

BufferClientProducer::~BufferClientProducer()
{
    BLOGNI("dtor");
}

GSError BufferClientProducer::RequestBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
                                            RequestBufferReturnValue &retval)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    WriteRequestConfig(arguments, config);

    SEND_REQUEST(BUFFER_PRODUCER_REQUEST_BUFFER, arguments, reply, option);
    CHECK_RETVAL_WITH_SEQ(reply, retval.sequence);

    ReadSurfaceBufferImpl(reply, retval.sequence, retval.buffer);
    bedata->ReadFromParcel(reply);
    ReadFence(reply, retval.fence);
    reply.ReadInt32Vector(&retval.deletingBuffers);
    return GSERROR_OK;
}

GSError BufferClientProducer::CancelBuffer(int32_t sequence, const sptr<BufferExtraData> &bedata)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    arguments.WriteInt32(sequence);
    bedata->WriteToParcel(arguments);

    SEND_REQUEST_WITH_SEQ(BUFFER_PRODUCER_CANCEL_BUFFER, arguments, reply, option, sequence);
    CHECK_RETVAL_WITH_SEQ(reply, sequence);

    return GSERROR_OK;
}

GSError BufferClientProducer::FlushBuffer(int32_t sequence, const sptr<BufferExtraData> &bedata,
                                          int32_t fence, BufferFlushConfig &config)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGE);

    arguments.WriteInt32(sequence);
    bedata->WriteToParcel(arguments);
    WriteFence(arguments, fence);
    WriteFlushConfig(arguments, config);

    SEND_REQUEST_WITH_SEQ(BUFFER_PRODUCER_FLUSH_BUFFER, arguments, reply, option, sequence);
    CHECK_RETVAL_WITH_SEQ(reply, sequence);

    return GSERROR_OK;
}

GSError BufferClientProducer::AttachBuffer(sptr<SurfaceBuffer>& buffer)
{
    return GSERROR_NOT_SUPPORT;
}

GSError BufferClientProducer::DetachBuffer(sptr<SurfaceBuffer>& buffer)
{
    return GSERROR_NOT_SUPPORT;
}

GSError BufferClientProducer::RegisterReleaseListener(OnReleaseFunc func)
{
    return GSERROR_NOT_SUPPORT;
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

GSError BufferClientProducer::SetTransform(TransformType transform)
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

GSError BufferClientProducer::IsSupportedAlloc(const std::vector<VerifyAllocInfo> &infos,
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
        return (GSError)ret;
    }

    return GSERROR_OK;
}
}; // namespace OHOS
