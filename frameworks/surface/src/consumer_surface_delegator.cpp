/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <message_option.h>
#include <message_parcel.h>
#include "consumer_surface_delegator.h"
#include "buffer_log.h"
#include "sync_fence.h"

namespace OHOS {
sptr<ConsumerSurfaceDelegator> ConsumerSurfaceDelegator::Create()
{
    return sptr<ConsumerSurfaceDelegator>(new ConsumerSurfaceDelegator());
}

GSError ConsumerSurfaceDelegator::DequeueBuffer(const BufferRequestConfig& config, sptr<BufferExtraData>& bedata,
                                                struct IBufferProducer::RequestBufferReturnValue& retval)
{
    return GSERROR_OK;
}

GSError ConsumerSurfaceDelegator::QueueBuffer(sptr<SurfaceBuffer>& buffer, int32_t fenceFd)
{
    return GSERROR_OK;
}

GSError ConsumerSurfaceDelegator::ReleaseBuffer(int slot, int releaseFenceFd)
{
    return GSERROR_OK;
}

GSError ConsumerSurfaceDelegator::CancelBuffer(sptr<SurfaceBuffer>& buffer)
{
    return GSERROR_OK;
}

GSError ConsumerSurfaceDelegator::DetachBuffer(sptr<SurfaceBuffer>& buffer)
{
    return GSERROR_OK;
}

bool ConsumerSurfaceDelegator::SetBufferQueue(BufferQueue* bufferQueue)
{
    bufferQueue_ = bufferQueue;
    return true;
}

int ConsumerSurfaceDelegator::OnRemoteRequest(uint32_t code,
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    return ERR_NONE;
}
} // namespace OHOS