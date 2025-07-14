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

#include "vsync_connection_stub.h"
#include <unistd.h>
#include "graphic_common.h"
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "vsync_log.h"

namespace OHOS {
namespace Rosen {
namespace {
    const std::string RSS_PROCESS_NAME = "resource_schedule_service";
}

int32_t VSyncConnectionStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    switch (code) {
        case IVSYNC_CONNECTION_REQUEST_NEXT_VSYNC: {
            RequestNextVSync();
            break;
        }
        case IVSYNC_CONNECTION_GET_RECEIVE_FD: {
            int32_t fd = -1;
            int32_t ret = GetReceiveFd(fd);
            if (!reply.WriteInt32(ret)) {
                VLOGE("IVSYNC_CONNECTION_GET_RECEIVE_FD Write ret failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            if (ret != VSYNC_ERROR_OK) {
                // check add log
                return ret;
            }
            if (!reply.WriteFileDescriptor(fd)) {
                VLOGE("IVSYNC_CONNECTION_GET_RECEIVE_FD Write fd failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            break;
        }
        case IVSYNC_CONNECTION_SET_RATE: {
            int32_t rate{0};
            if (!data.ReadInt32(rate)) {
                VLOGE("IVSYNC_CONNECTION_SET_RATE Read rate failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            int32_t ret = SetVSyncRate(rate);
            if (!reply.WriteInt32(ret)) {
                VLOGE("IVSYNC_CONNECTION_SET_RATE Write ret failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            if (ret != VSYNC_ERROR_OK) {
                // check add log
                return ret;
            }
            break;
        }
        case IVSYNC_CONNECTION_DESTROY: {
            int32_t ret = Destroy();
            if (!reply.WriteInt32(ret)) {
                VLOGE("IVSYNC_CONNECTION_DESTROY Write ret failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            return ret;
        }
        case IVSYNC_CONNECTION_SET_UI_DVSYNC_SWITCH: {
            bool dvsyncOn{false};
            if (!data.ReadBool(dvsyncOn)) {
                VLOGE("IVSYNC_CONNECTION_SET_UI_DVSYNC_SWITCH Read dvsyncOn failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            int32_t ret = SetUiDvsyncSwitch(dvsyncOn);
            if (!reply.WriteInt32(ret)) {
                VLOGE("IVSYNC_CONNECTION_SET_UI_DVSYNC_SWITCH Write ret failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            return ret;
        }
        case IVSYNC_CONNECTION_SET_UI_DVSYNC_CONFIG: {
            if (!CheckCallingPermission()) {
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            int32_t bufferCount{0};
            if (!data.ReadInt32(bufferCount)) {
                VLOGE("IVSYNC_CONNECTION_SET_UI_DVSYNC_CONFIG Read bufferCount failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            bool delayEnable{false};
            if (!data.ReadBool(delayEnable)) {
                VLOGE("IVSYNC_CONNECTION_SET_UI_DVSYNC_CONFIG Read delayEnable failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            bool nativeDelayEnable{false};
            if (!data.ReadBool(nativeDelayEnable)) {
                VLOGE("IVSYNC_CONNECTION_SET_UI_DVSYNC_CONFIG Read nativeDelayEnable failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            int32_t ret = SetUiDvsyncConfig(bufferCount, delayEnable, nativeDelayEnable);
            if (!reply.WriteInt32(ret)) {
                VLOGE("IVSYNC_CONNECTION_SET_UI_DVSYNC_CONFIG Write ret failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            return ret;
        }
        case IVSYNC_CONNECTION_SET_NATIVE_DVSYNC_SWITCH: {
            bool dvsyncOn{false};
            if (!data.ReadBool(dvsyncOn)) {
                VLOGE("IVSYNC_CONNECTION_SET_NATIVE_DVSYNC_SWITCH Read dvsyncOn failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            int32_t ret = SetNativeDVSyncSwitch(dvsyncOn);
            if (!reply.WriteInt32(ret)) {
                VLOGE("IVSYNC_CONNECTION_SET_NATIVE_DVSYNC_SWITCH Write ret failed");
                return VSYNC_ERROR_INVALID_ARGUMENTS;
            }
            return ret;
        }
        default: {
            // check add log
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return 0;
}

bool VSyncConnectionStub::CheckCallingPermission()
{
    Security::AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    Security::AccessToken::AccessTokenID rssToken =
        Security::AccessToken::AccessTokenKit::GetNativeTokenId(RSS_PROCESS_NAME);
    if (tokenId != rssToken) {
        VLOGE("CheckPermissionFailed, calling process illegal");
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
