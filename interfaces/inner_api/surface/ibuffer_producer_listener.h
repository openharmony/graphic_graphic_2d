/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNERKITS_SURFACE_IBUFFER_PRODUCER_LISTENER_H
#define INTERFACES_INNERKITS_SURFACE_IBUFFER_PRODUCER_LISTENER_H

#include "iremote_broker.h"
#include "surface_buffer.h"

namespace OHOS {
class IProducerListener : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"surf.IProducerListener");
    IProducerListener() = default;
    virtual ~IProducerListener() noexcept = default;
    virtual GSError OnBufferReleased() = 0;
    virtual GSError OnBufferReleasedWithFence(const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& fence) = 0;
    enum {
        ON_BUFFER_RELEASED = 0,
        ON_BUFFER_RELEASED_WITH_FENCE = 1,
    };
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_IBUFFER_PRODUCER_LISTENER_H
