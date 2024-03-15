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
#include "producer_surface_delegator.h"
#include "buffer_log.h"
#include "sync_fence.h"

namespace OHOS {
GSError ProducerSurfaceDelegator::DequeueBuffer(int32_t slot, sptr<SurfaceBuffer> buffer)
{
    return GSERROR_OK;
}

GSError ProducerSurfaceDelegator::QueueBuffer(int32_t slot, int32_t acquireFence)
{
    return GSERROR_OK;
}

GSError ProducerSurfaceDelegator::ReleaseBuffer(const sptr<SurfaceBuffer> &buffer, const sptr<SyncFence> &fence)
{
    return GSERROR_OK;
}

GSError ProducerSurfaceDelegator::ClearBufferSlot(int32_t slot)
{
    (void)slot;
    return GSERROR_OK;
}

GSError ProducerSurfaceDelegator::ClearAllBuffers()
{
    return GSERROR_OK;
}

void ProducerSurfaceDelegator::AddBufferLocked(const sptr<SurfaceBuffer>& buffer, int32_t slot)
{
    (void)buffer;
    (void)slot;
}

sptr<SurfaceBuffer> ProducerSurfaceDelegator::GetBufferLocked(int32_t slot)
{
    (void)slot;
    return nullptr;
}

int32_t ProducerSurfaceDelegator::GetSlotLocked(const sptr<SurfaceBuffer>& buffer)
{
    (void)buffer;
    return 0;
}

GSError ProducerSurfaceDelegator::CancelBuffer(int32_t slot, int32_t fenceFd)
{
    return GSERROR_OK;
}

GSError ProducerSurfaceDelegator::DetachBuffer(int32_t slot)
{
    return GSERROR_OK;
}

int ProducerSurfaceDelegator::OnDequeueBuffer(MessageParcel &data, MessageParcel &reply)
{
    return ERR_NONE;
}

int ProducerSurfaceDelegator::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    return ERR_NONE;
}
} // namespace OHOS
