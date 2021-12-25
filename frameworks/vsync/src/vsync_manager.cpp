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

#include <scoped_bytrace.h>

#include "vsync_callback_death_recipient.h"
#include "vsync_callback_proxy.h"
#include "vsync_log.h"

#define REMOTE_RETURN(reply, vsync_error) \
    reply.WriteInt32(vsync_error);        \
    if (vsync_error != GSERROR_OK) {  \
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
                REMOTE_RETURN(reply, GSERROR_INVALID_ARGUMENTS);
            }

            auto cb = iface_cast<IVsyncCallback>(remoteObject);

            GSError ret = ListenVsync(cb);

            REMOTE_RETURN(reply, ret);
            break;
        }
        case IVSYNC_MANAGER_REMOVE_VSYNC: {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                REMOTE_RETURN(reply, GSERROR_INVALID_ARGUMENTS);
            }

            auto cb = iface_cast<IVsyncCallback>(remoteObject);
            auto ret = RemoveVsync(cb);
            REMOTE_RETURN(reply, ret);
            break;
        }
        case IVSYNC_MANAGER_GET_VSYNC_FREQUENCY: {
            uint32_t freq = 0;
            GSError ret = GetVsyncFrequency(freq);
            reply.WriteInt32(ret);
            reply.WriteUint32(freq);
            break;
        }
        default: {
            VLOG_FAILURE("code %{public}d cannot process", code);
            return 1;
        }
    }
    return 0;
}

GSError VsyncManager::ListenVsync(sptr<IVsyncCallback>& cb)
{
    if (cb == nullptr) {
        VLOG_FAILURE_NO(GSERROR_INVALID_ARGUMENTS);
        return GSERROR_INVALID_ARGUMENTS;
    }
    VLOGI("add callbacks %{public}d", GetCallingPid());

    sptr<IRemoteObject::DeathRecipient> deathRecipient = new VsyncCallbackDeathRecipient(this);
    if (cb->AsObject()->AddDeathRecipient(deathRecipient) == false) {
        VLOGW("Failed to add death recipient");
    }

    ScopedBytrace bytrace(__func__);
    std::lock_guard<std::mutex> lock(callbacksMutex_);
    callbacks_.push_back(cb);
    return GSERROR_OK;
}

GSError VsyncManager::RemoveVsync(sptr<IVsyncCallback>& callback)
{
    ScopedBytrace bytrace(__func__);
    std::lock_guard<std::mutex> lock(callbacksMutex_);
    for (auto it = callbacks_.begin(); it != callbacks_.end(); it++) {
        if (*it == callback) {
            callbacks_.erase(it);
            break;
        }
    }

    return GSERROR_OK;
}

GSError VsyncManager::GetVsyncFrequency(uint32_t &freq)
{
    constexpr uint32_t defaultVsyncFrequency = 60;
    freq = defaultVsyncFrequency;
    return GSERROR_OK;
}

void VsyncManager::Callback(int64_t timestamp)
{
    using sptrIVsyncCallback = sptr<IVsyncCallback>;
    std::list<sptrIVsyncCallback> okcbs, calling;
    {
        ScopedBytrace bytrace("callback 1");
        std::lock_guard<std::mutex> lock(callbacksMutex_);
        calling = callbacks_;
        callbacks_.clear();
    }

    {
        ScopedBytrace bytrace("callback 2");
        for (const auto &cb : calling) {
            if (cb->OnVsync(timestamp) != GSERROR_BINDER) {
                okcbs.push_back(cb);
            }
        }
    }

    {
        ScopedBytrace bytrace("callback 3");
        std::lock_guard<std::mutex> lock(callbacksMutex_);
        callbacks_.insert(callbacks_.begin(), okcbs.begin(), okcbs.end());
    }
}
} // namespace Vsync
} // namespace OHOS
