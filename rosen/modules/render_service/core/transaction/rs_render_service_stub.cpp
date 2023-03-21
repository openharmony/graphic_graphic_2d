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

#include "rs_render_service_stub.h"

#include <iremote_proxy.h>

namespace OHOS {
namespace Rosen {
class RSConnectionTokenProxy : public IRemoteProxy<RSIConnectionToken> {
public:
    explicit RSConnectionTokenProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<RSIConnectionToken>(impl) {}
    virtual ~RSConnectionTokenProxy() noexcept = default;

private:
    static inline BrokerDelegator<RSConnectionTokenProxy> delegator_;
};

int RSRenderServiceStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int ret = ERR_NONE;
    switch (code) {
        case CREATE_CONNECTION: {
            auto interfaceToken = data.ReadInterfaceToken();
            if (interfaceToken != RSIRenderService::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            auto remoteObj = data.ReadRemoteObject();
            if (remoteObj == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }

            if (!remoteObj->IsProxyObject()) {
                ret = ERR_UNKNOWN_OBJECT;
                break;
            }

            auto token = iface_cast<RSIConnectionToken>(remoteObj);
            auto newConn = CreateConnection(token);
            reply.WriteRemoteObject(newConn->AsObject());
            break;
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }

    return ret;
}
} // namespace Rosen
} // namespace OHOS
