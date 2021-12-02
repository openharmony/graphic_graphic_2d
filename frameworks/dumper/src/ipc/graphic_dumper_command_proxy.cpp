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

#include "ipc/graphic_dumper_command_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "graphic_dumper_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperCommandProxy" };
}

GraphicDumperCommandProxy::GraphicDumperCommandProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IGraphicDumperCommand>(impl)
{
}

GSError GraphicDumperCommandProxy::GetConfig(const std::string &k, std::string &v)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;
    GDLOGFE("%{public}s -> %{public}s", k.c_str(), v.c_str());

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        GDLOGE("write interface token failed");
    }

    arg.WriteString(k);
    arg.WriteString(v);
    int result = Remote()->SendRequest(IGRAPHIC_DUMPER_COMMAND_GET_CONFIG, arg, ret, opt);
    if (result) {
        GDLOG_ERROR_API(result, SendRequest);
        return GSERROR_API_FAILED;
    }
    GSError retCode = static_cast<GSError>(ret.ReadInt32());
    if (retCode == GSERROR_OK) {
        v = ret.ReadString();
        GDLOGFE("%{public}s -> %{public}s", k.c_str(), v.c_str());
    }
    return retCode;
}

GSError GraphicDumperCommandProxy::SetConfig(const std::string &k, const std::string &v)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;
    GDLOGFE("%{public}s -> %{public}s", k.c_str(), v.c_str());

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        GDLOGE("write interface token failed");
    }

    arg.WriteString(k);
    arg.WriteString(v);
    int result = Remote()->SendRequest(IGRAPHIC_DUMPER_COMMAND_SET_CONFIG, arg, ret, opt);
    if (result) {
        GDLOG_ERROR_API(result, SendRequest);
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}

GSError GraphicDumperCommandProxy::Dump(const std::string &tag)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;
    GDLOGFE("%{public}s", tag.c_str());

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        GDLOGE("write interface token failed");
    }

    arg.WriteString(tag);
    int result = Remote()->SendRequest(IGRAPHIC_DUMPER_COMMAND_DUMP, arg, ret, opt);
    if (result) {
        GDLOG_ERROR_API(result, SendRequest);
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}

GSError GraphicDumperCommandProxy::GetLog(const std::string &tag, std::string &log)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;
    GDLOGFE("%{public}s -> %{public}s", tag.c_str(), log.c_str());

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        GDLOGE("write interface token failed");
    }

    arg.WriteString(tag);
    int result = Remote()->SendRequest(IGRAPHIC_DUMPER_COMMAND_GET_LOG, arg, ret, opt);
    if (result) {
        GDLOG_ERROR_API(result, SendRequest);
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}

GSError GraphicDumperCommandProxy::AddInfoListener(const std::string &tag, sptr<IGraphicDumperInfoListener> &listener)
{
    if (listener == nullptr) {
        GDLOG_FAILURE_NO(GSERROR_INVALID_ARGUMENTS);
        return GSERROR_INVALID_ARGUMENTS;
    }
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        GDLOGE("write interface token failed");
    }
    arg.WriteString(tag);
    arg.WriteRemoteObject(listener->AsObject());

    int result = Remote()->SendRequest(IGRAPHIC_DUMPER_COMMAND_ADD_INFO_LISTENER, arg, ret, opt);
    if (result) {
        GDLOG_ERROR_API(result, SendRequest);
        return GSERROR_BINDER;
    }

    GSError err = (GSError)ret.ReadInt32();
    if (err != GSERROR_OK) {
        GDLOG_FAILURE_NO(err);
    }
    return err;
}
} // namespace OHOS
