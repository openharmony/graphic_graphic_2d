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

#ifndef INTERFACES_INNERKITS_SURFACE_IBUFFER_PRODUCER_H
#define INTERFACES_INNERKITS_SURFACE_IBUFFER_PRODUCER_H

#include <string>
#include <vector>

#include <iremote_broker.h>

#include "buffer_extra_data.h"
#include "surface_buffer.h"
#include "surface_type.h"

namespace OHOS {
class SyncFence;
class IBufferProducer : public IRemoteBroker {
public:
    struct RequestBufferReturnValue {
        int32_t sequence;
        sptr<SurfaceBuffer> buffer;
        sptr<SyncFence> fence;
        std::vector<int32_t> deletingBuffers;
    };
    virtual GSError RequestBuffer(const BufferRequestConfig &config, sptr<BufferExtraData> &bedata,
                                       RequestBufferReturnValue &retval) = 0;

    virtual GSError CancelBuffer(int32_t sequence, const sptr<BufferExtraData> &bedata) = 0;

    virtual GSError FlushBuffer(int32_t sequence, const sptr<BufferExtraData> &bedata,
                                     const sptr<SyncFence>& fence, BufferFlushConfig &config) = 0;

    virtual GSError AttachBuffer(sptr<SurfaceBuffer>& buffer) = 0;
    virtual GSError DetachBuffer(sptr<SurfaceBuffer>& buffer) = 0;

    virtual uint32_t GetQueueSize() = 0;
    virtual GSError SetQueueSize(uint32_t queueSize) = 0;

    virtual GSError GetName(std::string &name) = 0;
    virtual uint64_t GetUniqueId() = 0;
    virtual GSError GetNameAndUniqueId(std::string& name, uint64_t& uniqueId) = 0;

    virtual int32_t GetDefaultWidth() = 0;
    virtual int32_t GetDefaultHeight() = 0;
    virtual uint32_t GetDefaultUsage() = 0;

    virtual GSError CleanCache() = 0;

    virtual GSError RegisterReleaseListener(OnReleaseFunc func) = 0;

    virtual GSError SetTransform(TransformType transform) = 0;

    virtual GSError IsSupportedAlloc(const std::vector<VerifyAllocInfo> &infos,
                                     std::vector<bool> &supporteds) = 0;
    
    virtual GSError Disconnect() = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"surf.IBufferProducer");

protected:
    enum {
        BUFFER_PRODUCER_REQUEST_BUFFER = 0,
        BUFFER_PRODUCER_CANCEL_BUFFER = 1,
        BUFFER_PRODUCER_FLUSH_BUFFER = 2,
        BUFFER_PRODUCER_GET_QUEUE_SIZE = 3,
        BUFFER_PRODUCER_SET_QUEUE_SIZE = 4,
        BUFFER_PRODUCER_GET_NAME = 5,
        BUFFER_PRODUCER_GET_DEFAULT_WIDTH = 6,
        BUFFER_PRODUCER_GET_DEFAULT_HEIGHT = 7,
        BUFFER_PRODUCER_GET_DEFAULT_USAGE = 8,
        BUFFER_PRODUCER_CLEAN_CACHE = 9,
        BUFFER_PRODUCER_ATTACH_BUFFER = 10,
        BUFFER_PRODUCER_DETACH_BUFFER = 11,
        BUFFER_PRODUCER_REGISTER_RELEASE_LISTENER = 12,
        BUFFER_PRODUCER_GET_UNIQUE_ID = 13,
        BUFFER_PRODUCER_SET_TRANSFORM = 14,
        BUFFER_PRODUCER_IS_SUPPORTED_ALLOC = 15,
        BUFFER_PRODUCER_GET_NAMEANDUNIQUEDID = 16,
        BUFFER_PRODUCER_DISCONNECT = 17,
    };
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_IBUFFER_PRODUCER_H
