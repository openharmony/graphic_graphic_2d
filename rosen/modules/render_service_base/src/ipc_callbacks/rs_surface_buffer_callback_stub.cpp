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
    auto token = data.ReadInterfaceToken();
    if (token != RSISurfaceBufferCallback::GetDescriptor()) {
        ROSEN_LOGE("RSSurfaceBufferCallbackStub: token ERR_INVALID_STATE");
        return ERR_INVALID_STATE;
    }
    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSISurfaceBufferCallbackInterfaceCode::ON_FINISH): {
            FinishCallbackRet ret;
            auto readRet = data.ReadUint64(ret.uid);
            readRet = readRet && data.ReadUInt32Vector(&ret.surfaceBufferIds);
            readRet = readRet && data.ReadUInt8Vector(&ret.isRenderedFlags);
            readRet = readRet && data.ReadBool(ret.isUniRender);
#ifdef ROSEN_OHOS
            uint64_t releaseFenceVecSize = {};
            constexpr uint64_t MAX_RELEASE_FENCE_VEC_SIZE = 32;
            readRet = readRet && data.ReadUint64(releaseFenceVecSize);
            if (releaseFenceVecSize > MAX_RELEASE_FENCE_VEC_SIZE) {
                ROSEN_LOGE("RSSurfaceBufferCallbackStub releaseFenceVecSize exceeds limits");
                return ERR_INVALID_DATA;
            }
            ret.releaseFences.reserve(releaseFenceVecSize);
            for (uint64_t idx = 0; idx < releaseFenceVecSize; ++idx) {
                auto fence = SyncFence::ReadFromMessageParcel(data);
                ret.releaseFences.emplace_back(fence);
                readRet = readRet && (!!fence);
            }
#endif
            if (!readRet) {
                ROSEN_LOGE("RSSurfaceBufferCallbackStub Read Remote Data ERROR");
            }
            OnFinish(ret);
            break;
        }
        case static_cast<uint32_t>(RSISurfaceBufferCallbackInterfaceCode::ON_AFTER_ACQUIRE_BUFFER) : {
            AfterAcquireBufferRet ret;
            auto readRet = data.ReadUint64(ret.uid);
            readRet = readRet && data.ReadBool(ret.isUniRender);
            if (!readRet) {
                ROSEN_LOGE("RSSurfaceBufferCallbackStub Read Remote Data ERROR");
            }
            OnAfterAcquireBuffer(ret);
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
} // namespace Rosen
} // namespace OHOS
