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
            BLOG_FAILURE("SendRequest return %{public}d", ret);                 \
            return SURFACE_ERROR_BINDER_ERROR;                                  \
        }                                                                       \
    } while (0)

#define SEND_REQUEST_WITH_SEQ(COMMAND, arguments, reply, option, sequence)      \
    do {                                                                        \
        int32_t ret = Remote()->SendRequest(COMMAND, arguments, reply, option); \
        if (ret != ERR_NONE) {                                                  \
            BLOG_FAILURE_ID(sequence, "SendRequest return %{public}d", ret);    \
            return SURFACE_ERROR_BINDER_ERROR;                                  \
        }                                                                       \
    } while (0)

#define CHECK_RETVAL_WITH_SEQ(reply, sequence)                          \
    do {                                                                \
        int32_t ret = reply.ReadInt32();                                \
        if (ret != SURFACE_ERROR_OK) {                                  \
            BLOG_FAILURE_ID(sequence, "Remote return %{public}d", ret); \
            return (SurfaceError)ret;                                   \
        }                                                               \
    } while (0)

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "BufferClientProducer" };
}

BufferClientProducer::BufferClientProducer(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IBufferProducer>(impl)
{
    BLOGFD("");
}

BufferClientProducer::~BufferClientProducer()
{
    BLOGFD("");
}

SurfaceError BufferClientProducer::RequestBuffer(int32_t& sequence, sptr<SurfaceBuffer>& buffer,
                                                 int32_t& fence, BufferRequestConfig& config,
                                                 std::vector<int32_t>& deletingBuffers)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGFE);

    WriteRequestConfig(arguments, config);

    SEND_REQUEST_WITH_SEQ(BUFFER_PRODUCER_REQUEST_BUFFER, arguments, reply, option, sequence);
    CHECK_RETVAL_WITH_SEQ(reply, sequence);

    ReadFence(reply, fence);
    reply.ReadInt32Vector(&deletingBuffers);

    sptr<SurfaceBufferImpl> bufferImpl;
    ReadSurfaceBufferImpl(reply, sequence, bufferImpl);
    buffer = bufferImpl;

    return SURFACE_ERROR_OK;
}

SurfaceError BufferClientProducer::CancelBuffer(int32_t sequence)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGFE);

    arguments.WriteInt32(sequence);

    SEND_REQUEST_WITH_SEQ(BUFFER_PRODUCER_CANCEL_BUFFER, arguments, reply, option, sequence);
    CHECK_RETVAL_WITH_SEQ(reply, sequence);

    return SURFACE_ERROR_OK;
}

SurfaceError BufferClientProducer::FlushBuffer(int32_t sequence,
                             int32_t fence, BufferFlushConfig& config)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGFE);

    arguments.WriteInt32(sequence);
    WriteFence(arguments, fence);
    WriteFlushConfig(arguments, config);

    SEND_REQUEST_WITH_SEQ(BUFFER_PRODUCER_FLUSH_BUFFER, arguments, reply, option, sequence);
    CHECK_RETVAL_WITH_SEQ(reply, sequence);

    return SURFACE_ERROR_OK;
}

uint32_t     BufferClientProducer::GetQueueSize()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGFE);

    SEND_REQUEST(BUFFER_PRODUCER_GET_QUEUE_SIZE, arguments, reply, option);

    return reply.ReadUint32();
}

SurfaceError BufferClientProducer::SetQueueSize(uint32_t queueSize)
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGFE);

    arguments.WriteInt32(queueSize);

    SEND_REQUEST(BUFFER_PRODUCER_SET_QUEUE_SIZE, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != SURFACE_ERROR_OK) {
        BLOG_FAILURE("Remote return %{public}d", ret);
        return (SurfaceError)ret;
    }

    return SURFACE_ERROR_OK;
}

int32_t BufferClientProducer::GetDefaultWidth()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGFE);

    SEND_REQUEST(BUFFER_PRODUCER_GET_DEFAULT_WIDTH, arguments, reply, option);

    return reply.ReadInt32();
}

int32_t BufferClientProducer::GetDefaultHeight()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGFE);

    SEND_REQUEST(BUFFER_PRODUCER_GET_DEFAULT_HEIGHT, arguments, reply, option);

    return reply.ReadInt32();
}

SurfaceError BufferClientProducer::CleanCache()
{
    DEFINE_MESSAGE_VARIABLES(arguments, reply, option, BLOGFE);

    SEND_REQUEST(BUFFER_PRODUCER_CLEAN_CACHE, arguments, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != SURFACE_ERROR_OK) {
        BLOG_FAILURE("Remote return %{public}d", ret);
        return (SurfaceError)ret;
    }

    return SURFACE_ERROR_OK;
}
}; // namespace OHOS
