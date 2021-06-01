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

#include <message_option.h>
#include <message_parcel.h>

#include "vsync_log.h"
#include "vsync_manager_proxy.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncManagerProxy" };
}

VsyncManagerProxy::VsyncManagerProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IVsyncManager>(impl)
{
    VLOGI("DEBUG VsyncManagerProxy");
}

VsyncManagerProxy::~VsyncManagerProxy()
{
    VLOGI("DEBUG ~VsyncManagerProxy");
}

VsyncError VsyncManagerProxy::ListenNextVsync(sptr<IVsyncCallback>& cb)
{
    if (cb == nullptr) {
        VLOG_FAILURE_NO(VSYNC_ERROR_NULLPTR);
        return VSYNC_ERROR_NULLPTR;
    }

    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        VLOGE("write interface token failed");
    }

    arg.WriteRemoteObject(cb->AsObject());

    int result = Remote()->SendRequest(IVSYNC_MANAGER_LISTEN_NEXT_VSYNC, arg, ret, opt);
    if (result) {
        VLOG_ERROR_API(result, SendRequest);
        return VSYNC_ERROR_BINDER_ERROR;
    }

    VsyncError err = (VsyncError)ret.ReadInt32();
    if (err != VSYNC_ERROR_OK) {
        VLOG_FAILURE_NO(err);
    }

    return err;
}
} // namespace OHOS
