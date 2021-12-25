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

#include "native_test_ipc_proxy.h"

#include <gslogger.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("NativeTestIpcProxy");
} // namespace

NativeTestIpcProxy::NativeTestIpcProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<INativeTestIpc>(impl)
{
}

GSError NativeTestIpcProxy::SendMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;
    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        GSLOG2HI(ERROR) << "write interface token failed";
        return GSERROR_BINDER;
    }

    arg.WriteInt32(sequence);
    arg.WriteString(message);
    arg.WriteBool(robj != nullptr);
    if (robj != nullptr) {
        arg.WriteRemoteObject(robj);
    }

    auto res = Remote()->SendRequest(INATIVE_TEST_IPC_SEND_MESSAGE, arg, ret, opt);
    if (res != ERR_NONE) {
        GSLOG2HI(ERROR) << "SendRequest return " << res;
        return GSERROR_BINDER;
    }

    return static_cast<enum GSError>(ret.ReadInt32());
}

GSError NativeTestIpcProxy::Register(int32_t sequence, sptr<INativeTestIpc> &ipc)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;
    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        GSLOG2HI(ERROR) << "write interface token failed";
        return GSERROR_BINDER;
    }

    arg.WriteInt32(sequence);
    arg.WriteRemoteObject(ipc->AsObject());
    auto res = Remote()->SendRequest(INATIVE_TEST_IPC_REGISTER, arg, ret, opt);
    if (res != ERR_NONE) {
        GSLOG2HI(ERROR) << "SendRequest return " << res;
        return GSERROR_BINDER;
    }

    return static_cast<enum GSError>(ret.ReadInt32());
}

GSError NativeTestIpcProxy::OnMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;
    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        GSLOG2HI(ERROR) << "write interface token failed";
        return GSERROR_BINDER;
    }

    arg.WriteInt32(sequence);
    arg.WriteString(message);
    arg.WriteBool(robj != nullptr);
    if (robj != nullptr) {
        arg.WriteRemoteObject(robj);
    }

    auto res = Remote()->SendRequest(INATIVE_TEST_IPC_ON_MESSAGE, arg, ret, opt);
    if (res != ERR_NONE) {
        GSLOG2HI(ERROR) << "SendRequest return " << res;
        return GSERROR_BINDER;
    }

    return static_cast<enum GSError>(ret.ReadInt32());
}
} // namespace OHOS
