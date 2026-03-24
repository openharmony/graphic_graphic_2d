/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "platform/ohos/transaction/rs_render_connect_parcel_info.h"

namespace OHOS {
namespace Rosen {
bool ReplyToRenderInfo::Marshalling(Parcel& data) const
{
    MessageParcel* message = static_cast<MessageParcel*>(&data);
    if (!message->WriteRemoteObject(serviceConnection_)) {
        return false;
    }
    if (!message->WriteRemoteObject(composeConnection_)) {
        return false;
    }
    if (!data.WriteParcelable(rsScreenProperty_.GetRefPtr())) {
        return false;
    }
    if (!message->WriteRemoteObject(vsyncConn_)) {
        return false;
    }
    return true;
}

ReplyToRenderInfo* ReplyToRenderInfo::Unmarshalling(Parcel& data)
{
    auto result = std::make_unique<ReplyToRenderInfo>();
    MessageParcel* message = static_cast<MessageParcel*>(&data);
    result->serviceConnection_ = message->ReadRemoteObject();
    if (result->serviceConnection_ == nullptr) {
        return nullptr;
    }
    result->composeConnection_ = message->ReadRemoteObject();
    if (result->composeConnection_ == nullptr) {
        return nullptr;
    }
    result->rsScreenProperty_ = sptr<RSScreenProperty>(data.ReadParcelable<RSScreenProperty>());
    if (result->rsScreenProperty_ == nullptr) {
        return nullptr;
    }
    result->vsyncConn_ = message->ReadRemoteObject();
    if (result->vsyncConn_ == nullptr) {
        return nullptr;
    }
    return result.release();
}

bool ConnectToServiceInfo::Marshalling(Parcel& data) const
{
    MessageParcel* message = static_cast<MessageParcel*>(&data);
    if (!message->WriteRemoteObject(serviceToRenderConnection_)) {
        return false;
    }
    if (!message->WriteRemoteObject(composerToRenderConnection_)) {
        return false;
    }
    if (!message->WriteRemoteObject(connectToRenderConnection_)) {
        return false;
    }
    if (!message->WriteRemoteObject(vsyncToken_)) {
        return false;
    }
    return true;
}

ConnectToServiceInfo* ConnectToServiceInfo::Unmarshalling(Parcel& data)
{
    auto result = std::make_unique<ConnectToServiceInfo>();
    MessageParcel* message = static_cast<MessageParcel*>(&data);
    result->serviceToRenderConnection_ = message->ReadRemoteObject();
    if (result->serviceToRenderConnection_ == nullptr) {
        return nullptr;
    }
    result->composerToRenderConnection_ = message->ReadRemoteObject();
    if (result->composerToRenderConnection_ == nullptr) {
        return nullptr;
    }
    result->connectToRenderConnection_ = message->ReadRemoteObject();
    if (result->connectToRenderConnection_ == nullptr) {
        return nullptr;
    }
    result->vsyncToken_ = message->ReadRemoteObject();
    if (result->vsyncToken_ == nullptr) {
        return nullptr;
    }
    return result.release();
}
} // namespace Rosen
} // namespace OHOS