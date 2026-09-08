/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RS_DELEGATE_COMPOSITE_CALLBACK_H
#define RS_DELEGATE_COMPOSITE_CALLBACK_H

#include "iremote_broker.h"
#include <iremote_stub.h>
#include <iremote_proxy.h>
#include "graphic_common.h"

#include "rs_transaction_callback.h"

namespace OHOS {
namespace Rosen {
class RSISurfaceTransactionListener : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.surfaceTransactionListener");
    RSISurfaceTransactionListener() = default;
    virtual ~RSISurfaceTransactionListener() noexcept = default;
    virtual GSError OnComplete(uint64_t timestamp, uint64_t srcId, std::queue<uint64_t>& seqNums) = 0;
};

class RSISurfaceNodeBufferReleaseCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.surfaceNodeBufferReleaseCallback");
    RSISurfaceNodeBufferReleaseCallback() = default;
    virtual ~RSISurfaceNodeBufferReleaseCallback() noexcept = default;
    virtual GSError OnBufferComplete(std::queue<OnCompletedRet>& queue) = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_DELEGATE_COMPOSITE_CALLBACK_H
