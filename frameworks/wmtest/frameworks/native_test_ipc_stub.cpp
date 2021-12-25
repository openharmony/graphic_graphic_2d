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

#include "native_test_ipc_stub.h"

#include <gslogger.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("NativeTestIpcStub");
} // namespace

int NativeTestIpcStub::OnRemoteRequest(uint32_t code, MessageParcel &arguments,
                                       MessageParcel &reply, MessageOption &option)
{
    auto remoteDescriptor = arguments.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        GSLOG2HI(ERROR) << "descriptor is invalid";
        return ERR_INVALID_STATE;
    }

    std::string message = "";
    int32_t sequence = 0;
    sptr<IRemoteObject> remoteObject = nullptr;
    sptr<INativeTestIpc> ipc = nullptr;

    switch (code) {
        case INATIVE_TEST_IPC_SEND_MESSAGE:
            sequence = arguments.ReadInt32();
            message = arguments.ReadString();
            if (arguments.ReadBool()) {
                remoteObject = arguments.ReadRemoteObject();
            }

            reply.WriteInt32(SendMessage(sequence, message, remoteObject));
            break;
        case INATIVE_TEST_IPC_REGISTER:
            sequence = arguments.ReadInt32();
            remoteObject = arguments.ReadRemoteObject();
            if (remoteObject == nullptr) {
                GSLOG2HI(ERROR) << "remoteObject is nullptr";
                reply.WriteInt32(GSERROR_INVALID_ARGUMENTS);
                break;
            }

            ipc = iface_cast<INativeTestIpc>(remoteObject);
            reply.WriteInt32(Register(sequence, ipc));
            break;
        case INATIVE_TEST_IPC_ON_MESSAGE:
            sequence = arguments.ReadInt32();
            message = arguments.ReadString();
            if (arguments.ReadBool()) {
                remoteObject = arguments.ReadRemoteObject();
            }

            reply.WriteInt32(OnMessage(sequence, message, remoteObject));
            break;
        default:
            GSLOG2HI(ERROR) << "code " << code << " cannot process";
            return ERR_INVALID_STATE;
    }
    return ERR_NONE;
}
} // namespace OHOS
