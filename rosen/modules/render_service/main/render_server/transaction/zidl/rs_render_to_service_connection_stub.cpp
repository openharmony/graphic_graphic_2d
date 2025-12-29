/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "gfx/dump/rs_dump_manager.h"
#include "rs_render_to_service_connection_stub.h"

#include "rs_irender_to_service_connection_ipc_interface_code.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderToServiceConnectionStub"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAX_SCREEN_ID_COUNT = 64;
}

int RSRenderToServiceConnectionStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int ret = ERR_NONE;
    if (auto interfaceToken = data.ReadInterfaceToken();
        interfaceToken != RSIRenderToServiceConnection::GetDescriptor()) {
        RS_LOGE("%{public}s: Read interfaceToken failed.", __func__);
        return ERR_INVALID_STATE;
    }
    switch (code) {
        case static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::REPLY_DUMP_RESULT_TO_SERVICE): {
            int32_t size = 0;
            if (!data.ReadInt32(size)) {
                RS_LOGE("%{public}s::REPLY_DUMP_RESULT_TO_SERVICE ReadInt32 failed.", __func__);
                return ERR_INVALID_STATE;
            }
            char *dataPtr = RSDumpManager::GetInstance().ReadAshmemDataFromParcel(data, size);
            std::string dumpString = dataPtr != nullptr ? 
                std::string(static_cast<const char*>(dataPtr), size) : "";
            ReplyDumpResultToService(dumpString);
            break;
        }
        case static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::NOTIFY_PROCESS_FRAME_RATE): {
            uint64_t timestamp = 0;
            uint64_t vsyncId = 0;
            if (!data.ReadUint64(timestamp) || !data.ReadUint64(vsyncId)) {
                RS_LOGE("%{public}s::NOTIFY_PROCESS_FRAME_RATE ReadTimestamp or ReadVsyncId failed.", __func__);
                return ERR_INVALID_STATE;
            }
            sptr<HgmProcessToServiceInfo> processToServiceInfo = data.ReadParcelable<HgmProcessToServiceInfo>();
            if (processToServiceInfo == nullptr) {
                RS_LOGE("%{public}s::NOTIFY_PROCESS_FRAME_RATE ReadParcelable failed.", __func__);
                return ERR_INVALID_STATE;
            }
            uint32_t size = 0;
            if (!data.ReadUint32(size) || size > MAX_SCREEN_ID_COUNT) {
                RS_LOGE("%{public}s::NOTIFY_PROCESS_FRAME_RATE ReadSize failed.", __func__);
                return ERR_INVALID_STATE;
            }
            auto serviceToProcessInfo = NotifyRpHgmFrameRate(timestamp, vsyncId, processToServiceInfo);
            if (serviceToProcessInfo == nullptr || !reply.WriteParcelable(serviceToProcessInfo.GetRefPtr())) {
                RS_LOGE("%{public}s::NOTIFY_PROCESS_FRAME_RATE WriteParcelable failed.", __func__);
                return ERR_INVALID_STATE;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderToServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCH_FINISHED): {
            ScreenId id = 0;
            if (!data.ReadUint64(id)) {
                RS_LOGE("%{public}s::NOTIFY_SCREEN_SWITCH_FINISHED ReadUint64 failed.", __func__);
                return ERR_INVALID_STATE;
            }
            NotifyScreenSwitchFinished(id);
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ret;
}

} // namespace Rosen
} // namespace OHOS