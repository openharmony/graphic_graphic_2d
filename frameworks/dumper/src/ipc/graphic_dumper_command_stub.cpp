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

#include "ipc/graphic_dumper_command_stub.h"

#include "graphic_dumper_hilog.h"
#include "graphic_dumper_server.h"
#include "ipc/igraphic_dumper_command.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperCommandStub" };
}

int32_t GraphicDumperCommandStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return 1;
    }
    switch (code) {
        case IGRAPHIC_DUMPER_COMMAND_GET_CONFIG: {
            std::string k = data.ReadString();
            std::string v = data.ReadString();
            GSError ret = GetConfig(k, v);
            reply.WriteInt32(ret);
            reply.WriteString(v);
            break;
        }

        case IGRAPHIC_DUMPER_COMMAND_SET_CONFIG: {
            std::string k = data.ReadString();
            std::string v = data.ReadString();
            GSError ret = SetConfig(k, v);
            reply.WriteInt32(ret);
            break;
        }

        case IGRAPHIC_DUMPER_COMMAND_DUMP: {
            std::string v = data.ReadString();
            GSError ret = Dump(v);
            reply.WriteInt32(ret);
            break;
        }

        case IGRAPHIC_DUMPER_COMMAND_GET_LOG: {
            std::string tag = data.ReadString();
            std::string log = "";
            GSError ret = GetLog(tag, log);
            reply.WriteInt32(ret);
            break;
        }
        case IGRAPHIC_DUMPER_COMMAND_ADD_INFO_LISTENER: {
            auto tag = data.ReadString();
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                reply.WriteInt32(GSERROR_INVALID_ARGUMENTS);
                GDLOG_FAILURE_NO(GSERROR_INVALID_ARGUMENTS);
                break;
            }

            auto l = iface_cast<IGraphicDumperInfoListener>(remoteObject);
            GSError ret = AddInfoListener(tag, l);
            reply.WriteInt32(ret);
            if (ret != GSERROR_OK) {
                GDLOG_FAILURE_NO(ret);
            }
            break;
        }
        default: {
            return 1;
            break;
        }
    }
    return 0;
}

GSError GraphicDumperCommandStub::GetConfig(const std::string &k, std::string &v)
{
    GDLOGFE("%{public}s -> %{public}s", k.c_str(), v.c_str());
    GraphicDumperServer::GetInstance()->GetConfig(k, v);
    GDLOGFE("%{public}s -> %{public}s", k.c_str(), v.c_str());
    return GSERROR_OK;
}

GSError GraphicDumperCommandStub::SetConfig(const std::string &k, const std::string &v)
{
    GDLOGFE("%{public}s -> %{public}s", k.c_str(), v.c_str());
    GraphicDumperServer::GetInstance()->SetConfig(k, v);
    return GSERROR_OK;
}

GSError GraphicDumperCommandStub::Dump(const std::string &tag)
{
    GDLOGFE("%{public}s", tag.c_str());
    GraphicDumperServer::GetInstance()->Dump(tag);
    return GSERROR_OK;
}

GSError GraphicDumperCommandStub::GetLog(const std::string &tag, std::string &log)
{
    GDLOGFE("%{public}s -> %{public}s", tag.c_str(), log.c_str());
    GraphicDumperServer::GetInstance()->GetLog(tag, log);
    return GSERROR_OK;
}

GSError GraphicDumperCommandStub::AddInfoListener(const std::string &tag, sptr<IGraphicDumperInfoListener> &listener)
{
    GDLOGFE("");
    if (listener == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    return GraphicDumperServer::GetInstance()->AddInfoListener(listener);
}
} // namespace OHOS
