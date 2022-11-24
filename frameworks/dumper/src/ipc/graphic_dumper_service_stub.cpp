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

#include "ipc/graphic_dumper_service_stub.h"

#include "graphic_dumper_hilog.h"
#include "graphic_dumper_server.h"

#define REMOTE_RETURN(reply, gs_error)  \
    do {                                \
        (reply).WriteInt32(gs_error);   \
        if ((gs_error) != GSERROR_OK) { \
            GDLOG_FAILURE_NO(gs_error); \
        }                               \
    } while (0);                        \
    break

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperServiceStub" };
}

int32_t GraphicDumperServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return 1;
    }

    switch (code) {
        case IGRAPHIC_DUMPER_SERVICE_ADD_CLIENT_LISTENER: {
            auto tag = data.ReadString();
            GDLOGFE("%{public}s", tag.c_str());
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                reply.WriteInt32(GSERROR_INVALID_ARGUMENTS);
                GDLOG_FAILURE_NO(GSERROR_INVALID_ARGUMENTS);
                break;
            }
            auto l = iface_cast<IGraphicDumperClientListener>(remoteObject);
            GSError ret = AddClientListener(tag, l);
            reply.WriteInt32(ret);
            if (ret != GSERROR_OK) {
                GDLOG_FAILURE_NO(ret);
            }
            break;
        }
        case IGRAPHIC_DUMPER_SERVICE_SEND_INFO: {
            std::string tag = data.ReadString();
            std::string info = data.ReadString();
            GSError ret = SendInfo(tag, info);
            reply.WriteInt32(ret);
            if (ret != GSERROR_OK) {
                GDLOG_FAILURE_NO(ret);
            }
            break;
        }
        default: {
            GDLOGFE("code %{public}d cannot process", code);
            return 1;
        }
    }
    return 0;
}
GSError GraphicDumperServiceStub::AddClientListener(const std::string &tag,
    sptr<IGraphicDumperClientListener> &listener)
{
    if (listener == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return GraphicDumperServer::GetInstance()->AddConfigListener(tag, listener);
}
GSError GraphicDumperServiceStub::SendInfo(const std::string &tag, const std::string &info)
{
    return GraphicDumperServer::GetInstance()->InfoHandle(tag, info);
}
} // namespace OHOS
