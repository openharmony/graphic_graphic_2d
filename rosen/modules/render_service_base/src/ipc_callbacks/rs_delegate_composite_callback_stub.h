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

#ifndef RS_DELEGATE_COMPOSITE_CALLBACK_STUB_H
#define RS_DELEGATE_COMPOSITE_CALLBACK_STUB_H

#include <iremote_stub.h>
#include "ipc_callbacks/rs_delegate_composite_callback.h"
#include "ipc_callbacks/rs_delegate_composite_callback_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSWebProxyComposerCallbackStub : public IRemoteStub<RSISurfaceTransactionListener>
{
public:
    int32_t OnRemoteRequest(uint32_t code, MessageParcel& arguments,
        MessageParcel& reply, MessageOption& option) override;
    GSError OnComplete(uint64_t timestamp, uint64_t srcId, std::queue<uint64_t> &seqNums) override;

    using OnCompleteCallback = std::function<void(uint64_t timestamp,
        uint64_t srcId, const std::queue<uint64_t>& seqNums)>;
    void RegisterOnCompleteCallBack(OnCompleteCallback cb);
    void UnRegisterOnCompleteCallBack();

private:
    std::mutex mutex_;
    OnCompleteCallback onCompleteCallback_ = nullptr;
};

class RSB_EXPORT SurfaceNodeBufferReleaseCallbackStub : public IRemoteStub<RSISurfaceNodeBufferReleaseCallback>
{
public:
    using OnCompleteCallback = std::function<void(std::queue<OnCompletedRet>& queue)>;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel& arguments,
        MessageParcel& reply, MessageOption& option) override;
    void RegisterReleaseBufferCallBack(OnCompleteCallback cb);
    void UnRegisterReleaseBufferCallBack();
    GSError OnBufferComplete(std::queue<OnCompletedRet>& queue) override;
private:
    std::mutex mutex_;
    OnCompleteCallback onCompleteCallback_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_DELEGATE_COMPOSITE_CALLBACK_STUB_H
