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

#ifndef CONSUMER_SURFACE_DELEGATOR_H
#define CONSUMER_SURFACE_DELEGATOR_H

#include "transact_surface_delegator_stub.h"

namespace OHOS {
class BufferQueue;
class ConsumerSurfaceDelegator : public TransactSurfaceDelegatorStub {
public:
    static sptr<ConsumerSurfaceDelegator> Create();
    ~ConsumerSurfaceDelegator() = default;
    GSError DequeueBuffer(const BufferRequestConfig& config, sptr<BufferExtraData>& bedata,
                          struct IBufferProducer::RequestBufferReturnValue& retval);
    GSError QueueBuffer(sptr<SurfaceBuffer>& buffer, int32_t fenceFd);
    GSError ReleaseBuffer(int slot, int releaseFenceFd);
    GSError CancelBuffer(sptr<SurfaceBuffer>& buffer);
    GSError DetachBuffer(sptr<SurfaceBuffer>& buffer);
    bool SetBufferQueue(BufferQueue* bufferQueue);
    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
private:
    ConsumerSurfaceDelegator() = default;
    std::map<int32_t, sptr<SurfaceBuffer>> slotBufferMap_;
    BufferQueue* bufferQueue_ = nullptr;
};
} // namespace OHOS
#endif // CONSUMER_SURFACE_DELEGATOR_H
