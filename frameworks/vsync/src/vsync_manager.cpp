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

#include "vsync_callback_death_recipient.h"
#include "vsync_callback_proxy.h"
#include "vsync_log.h"

#define REMOTE_RETURN(reply, vsync_error) \
    reply.WriteInt32(vsync_error);        \
    if (vsync_error != VSYNC_ERROR_OK) {  \
        VLOG_FAILURE_NO(vsync_error);     \
    }                                     \
    break

namespace OHOS {
namespace Vsync {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncManager" };
}

int32_t VsyncManager::OnRemoteRequest(uint32_t code, MessageParcel &data,
                                      MessageParcel &reply, MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        VLOG_FAILURE("descriptor is invalid");
        return ERR_INVALID_STATE;
    }

    switch (code) {
        case IVSYNC_MANAGER_LISTEN_VSYNC: {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                REMOTE_RETURN(reply, VSYNC_ERROR_NULLPTR);
            }

            auto cb = iface_cast<IVsyncCallback>(remoteObject);

            VsyncError ret = ListenVsync(cb);

            REMOTE_RETURN(reply, ret);
        } break;
        case IVSYNC_MANAGER_GET_VSYNC_FREQUENCY: {
            uint32_t freq = 0;
            VsyncError ret = GetVsyncFrequency(freq);
            reply.WriteInt32(ret);
            reply.WriteUint32(freq);
        } break;
        default: {
            VLOG_FAILURE("code %{public}d cannot process", code);
            return 1;
        } break;
    }
    return 0;
}

VsyncError VsyncManager::ListenVsync(sptr<IVsyncCallback>& cb)
{
    if (cb == nullptr) {
        VLOG_FAILURE_NO(VSYNC_ERROR_NULLPTR);
        return VSYNC_ERROR_NULLPTR;
    }
    VLOGI("add callbacks %{public}d", GetCallingPid());

    sptr<IRemoteObject::DeathRecipient> deathRecipient = new VsyncCallbackDeathRecipient(this);
    if (cb->AsObject()->AddDeathRecipient(deathRecipient) == false) {
        VLOGW("Failed to add death recipient");
    }

    std::lock_guard<std::mutex> lock(callbacksMutex_);
    callbacks_.push_back(cb);
    return VSYNC_ERROR_OK;
}

VsyncError VsyncManager::GetVsyncFrequency(uint32_t& freq)
{
    constexpr uint32_t defaultVsyncFrequency = 60;
    freq = defaultVsyncFrequency;
    return VSYNC_ERROR_OK;
}

void VsyncManager::Callback(int64_t timestamp)
{
    VLOGI("call callback");
    std::lock_guard<std::mutex> lock(callbacksMutex_);

    std::list<sptr<IVsyncCallback>> okcbs;
    for (const auto& cb : callbacks_) {
        if (cb->OnVsync(timestamp) != VSYNC_ERROR_BINDER_ERROR) {
            okcbs.push_back(cb);
        }
    }
    callbacks_ = okcbs;
}
} // namespace Vsync
} // namespace OHOS
