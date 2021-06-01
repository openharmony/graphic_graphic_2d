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

#include "vsync_manager.h"
#include "vsync_callback_proxy.h"
#include "vsync_log.h"

#define REMOTE_RETURN(reply, vsync_error) \
    reply.WriteInt32(vsync_error);        \
    if (vsync_error != VSYNC_ERROR_OK) {  \
        VLOG_FAILURE_NO(vsync_error);     \
    }                                     \
    break

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncManager" };
}

int32_t VsyncManager::OnRemoteRequest(uint32_t code, MessageParcel &data,
                                      MessageParcel &reply, MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    switch (code) {
        case IVSYNC_MANAGER_LISTEN_NEXT_VSYNC: {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                REMOTE_RETURN(reply, VSYNC_ERROR_NULLPTR);
            }

            auto cb = iface_cast<IVsyncCallback>(remoteObject);

            VsyncError ret = ListenNextVsync(cb);

            REMOTE_RETURN(reply, ret);
        } break;
        default: {
            VLOG_FAILURE("code %{public}d cannot process", code);
            return 1;
        } break;
    }
    return 0;
}

VsyncError VsyncManager::ListenNextVsync(sptr<IVsyncCallback>& cb)
{
    if (cb == nullptr) {
        VLOG_FAILURE_NO(VSYNC_ERROR_NULLPTR);
        return VSYNC_ERROR_NULLPTR;
    }

    std::unique_lock<std::mutex> lockGuard(callbacks_mutex);

    callbacks.push_back(cb);
    condition_.notify_all();
    return VSYNC_ERROR_OK;
}

void VsyncManager::Callback(int64_t timestamp)
{
    std::unique_lock<std::mutex> lockGuard(callbacks_mutex);

    for (auto it = callbacks.begin(); it != callbacks.end(); it++) {
        (*it)->OnVsync(timestamp);
    }
    callbacks.clear();
}

void VsyncManager::CheckVsyncRequest()
{
    std::unique_lock<std::mutex> lockGuard(callbacks_mutex);
    while (callbacks.empty()) {
        condition_.wait(lockGuard);
    }
}

void VsyncManager::StopCheck()
{
    callbacks.push_back(nullptr);
    condition_.notify_all();
}
} // namespace OHOS
