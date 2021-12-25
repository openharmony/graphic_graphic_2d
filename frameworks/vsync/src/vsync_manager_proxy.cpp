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

#include "vsync_manager_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "return_value_tester.h"
#include "vsync_log.h"

namespace OHOS {
namespace Vsync {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncManagerProxy" };
}

VsyncManagerProxy::VsyncManagerProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IVsyncManager>(impl)
{
}

GSError VsyncManagerProxy::ListenVsync(sptr<IVsyncCallback>& cb)
{
    if (cb == nullptr) {
        VLOG_FAILURE_NO(GSERROR_INVALID_ARGUMENTS);
        return GSERROR_INVALID_ARGUMENTS;
    }

    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;
    auto reval = arg.WriteInterfaceToken(GetDescriptor());
    if (!ReturnValueTester::Get<bool>(reval)) {
        VLOGE("write interface token failed");
        return GSERROR_INVALID_ARGUMENTS;
    }

    arg.WriteRemoteObject(cb->AsObject());
    int result = Remote()->SendRequest(IVSYNC_MANAGER_LISTEN_VSYNC, arg, ret, opt);
    if (ReturnValueTester::Get<int>(result)) {
        VLOG_ERROR_API(result, SendRequest);
        return GSERROR_BINDER;
    }

    int res = ret.ReadInt32();
    GSError err = (GSError)ReturnValueTester::Get<int>(res);
    if (err != GSERROR_OK) {
        VLOG_FAILURE_NO(err);
        return err;
    }

    return GSERROR_OK;
}

GSError VsyncManagerProxy::RemoveVsync(sptr<IVsyncCallback>& cb)
{
    if (cb == nullptr) {
        VLOG_FAILURE_NO(GSERROR_INVALID_ARGUMENTS);
        return GSERROR_INVALID_ARGUMENTS;
    }

    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;
    auto reval = arg.WriteInterfaceToken(GetDescriptor());
    if (!ReturnValueTester::Get<bool>(reval)) {
        VLOGE("write interface token failed");
        return GSERROR_INVALID_ARGUMENTS;
    }

    arg.WriteRemoteObject(cb->AsObject());
    int result = Remote()->SendRequest(IVSYNC_MANAGER_REMOVE_VSYNC, arg, ret, opt);
    if (ReturnValueTester::Get<int>(result)) {
        VLOG_ERROR_API(result, SendRequest);
        return GSERROR_BINDER;
    }

    int res = ret.ReadInt32();
    GSError err = (GSError)ReturnValueTester::Get<int>(res);
    if (err != GSERROR_OK) {
        VLOG_FAILURE_NO(err);
        return err;
    }

    return GSERROR_OK;
}

GSError VsyncManagerProxy::GetVsyncFrequency(uint32_t &freq)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;

    auto reval = arg.WriteInterfaceToken(GetDescriptor());
    if (!ReturnValueTester::Get<bool>(reval)) {
        VLOGE("write interface token failed");
        return GSERROR_INVALID_ARGUMENTS;
    }

    int32_t result = Remote()->SendRequest(IVSYNC_MANAGER_GET_VSYNC_FREQUENCY, arg, ret, opt);
    if (ReturnValueTester::Get<int>(result)) {
        VLOG_ERROR_API(result, SendRequest);
        return GSERROR_BINDER;
    }

    int res = ret.ReadInt32();
    GSError err = (GSError)ReturnValueTester::Get<int>(res);
    if (err != GSERROR_OK) {
        VLOG_FAILURE_NO(err);
        return err;
    }

    freq = ret.ReadUint32();
    return GSERROR_OK;
}
} // namespace Vsync
} // namespace OHOS
