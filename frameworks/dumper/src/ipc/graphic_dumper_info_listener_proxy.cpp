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

#include "ipc/graphic_dumper_info_listener_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "graphic_dumper_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperInfoListenerProxy" };
}

GraphicDumperInfoListenerProxy::GraphicDumperInfoListenerProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IGraphicDumperInfoListener>(impl)
{
}

void GraphicDumperInfoListenerProxy::OnInfoComing(const std::string &info)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;
    if (!arg.WriteInterfaceToken(GetDescriptor())) {
        GDLOGE("write interface token failed");
    }

    arg.WriteString(info);
    int result = Remote()->SendRequest(IGRAPHIC_DUMPER_INFO_LISTENER_ON_INFO_COMING, arg, ret, opt);
    if (result) {
        GDLOG_ERROR_API(result, SendRequest);
        return;
    }
    return;
}
} // namespace OHOS
