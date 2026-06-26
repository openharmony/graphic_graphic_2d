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

#ifndef RS_DELEGATE_COMPOSITE_CALLBACK_PROXY_H
#define RS_DELEGATE_COMPOSITE_CALLBACK_PROXY_H

#include <iremote_proxy.h>

#include "ipc_callbacks/rs_delegate_composite_callback.h"
#include "ipc_callbacks/rs_delegate_composite_callback_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSWebProxyComposerCallbackProxy : public IRemoteProxy<RSISurfaceTransactionListener> {
public:
    explicit RSWebProxyComposerCallbackProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<RSISurfaceTransactionListener>(impl) {};
    GSError OnComplete(uint64_t timestamp, uint64_t srcId, std::queue<uint64_t> &seqNums);
private:
    static inline BrokerDelegator<RSWebProxyComposerCallbackProxy> delegator_;
};

class SurfaceNodeBufferReleaseCallbackProxy : public IRemoteProxy<RSISurfaceNodeBufferReleaseCallback> {
public:
    explicit SurfaceNodeBufferReleaseCallbackProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<RSISurfaceNodeBufferReleaseCallback>(impl) {};
    GSError OnBufferComplete(std::queue<OnCompletedRet> &queue);
private:
    static inline BrokerDelegator<SurfaceNodeBufferReleaseCallbackProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_DELEGATE_COMPOSITE_CALLBACK_PROXY_H
