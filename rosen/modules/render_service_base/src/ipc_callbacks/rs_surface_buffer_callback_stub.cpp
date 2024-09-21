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

#include "rs_surface_buffer_callback_stub.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
int RSSurfaceBufferCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (!securityManager_.IsInterfaceCodeAccessible(code)) {
        RS_LOGE("RSSurfaceBufferCallbackStub::OnRemoteRequest no permission to access codeID=%{public}u.", code);
        return ERR_INVALID_STATE;
    }

    auto token = data.ReadInterfaceToken();
    if (token != RSISurfaceBufferCallback::GetDescriptor()) {
        ROSEN_LOGE("RSSurfaceBufferCallbackStub: token ERR_INVALID_STATE");
        return ERR_INVALID_STATE;
    }
    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSISurfaceBufferCallbackInterfaceCode::ON_FINISH): {
            uint64_t uid = {};
            auto readRet = data.ReadUint64(uid);
            std::vector<uint32_t> surfaceBufferIds;
            readRet = readRet && data.ReadUInt32Vector(&surfaceBufferIds);
            if (!readRet) {
                ROSEN_LOGE("RSSurfaceBufferCallbackStub Read Remote Data ERROR");
            }
            OnFinish(uid, surfaceBufferIds);
            break;
        }
        default: {
            ROSEN_LOGE("RSSurfaceBufferCallbackStub: Unhandled enumeration value");
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }

    return ret;
}

const RSInterfaceCodeSecurityManager RSSurfaceBufferCallbackStub::securityManager_ = \
    RSInterfaceCodeSecurityManager::CreateInstance<RSISurfaceBufferCallbackInterfaceCodeAccessVerifier>();
} // namespace Rosen
} // namespace OHOS
