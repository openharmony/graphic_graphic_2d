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

#include "ipc/graphic_dumper_service_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "graphic_dumper_hilog.h"


namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperServiceProxy" };
}

GraphicDumperServiceProxy::GraphicDumperServiceProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IGraphicDumperService>(impl)
{
}

GSError GraphicDumperServiceProxy::AddClientListener(const std::string &tag,
                                                     sptr<IGraphicDumperClientListener> &listener)
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

    int result = Remote()->SendRequest(IGRAPHIC_DUMPER_SERVICE_ADD_CLIENT_LISTENER, arg, ret, opt);
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

GSError GraphicDumperServiceProxy::SendInfo(const std::string &tag, const std::string &info)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;

    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        GDLOGE("write interface token failed");
    }
    arg.WriteString(tag);
    arg.WriteString(info);
    GDLOGE("SendInfo SendRequest !!!!!");
    int result = Remote()->SendRequest(IGRAPHIC_DUMPER_SERVICE_SEND_INFO, arg, ret, opt);
    if (result) {
        GDLOG_ERROR_API(result, SendRequest);
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
}
} // namespace OHOS
