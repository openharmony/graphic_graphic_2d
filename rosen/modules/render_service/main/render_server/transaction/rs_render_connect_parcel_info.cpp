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

#include "rs_render_connect_parcel_info.h"

#include "render_process/transaction/ipc_persistence/rs_ipc_persistence_manager.h"

namespace OHOS {
namespace Rosen {
bool ReplyToRenderInfo::Marshalling(Parcel& data) const
{
    MessageParcel* message = static_cast<MessageParcel*>(&data);
    if (!message->WriteRemoteObject(composeConnection_)) {
        return false;
    }
    if (!data.WriteParcelable(rsScreenProperty_.GetRefPtr())) {
        return false;
    }
    if (!message->WriteRemoteObject(vsyncConn_)) {
        return false;
    }
    return RSIpcPersistenceManager::Marshalling(data, persistenceData_);
}

ReplyToRenderInfo* ReplyToRenderInfo::Unmarshalling(Parcel& data)
{
    auto result = std::make_unique<ReplyToRenderInfo>();
    MessageParcel* message = static_cast<MessageParcel*>(&data);
    result->composeConnection_ = message->ReadRemoteObject();
    if (!result->composeConnection_) {
        return nullptr;
    }
    result->rsScreenProperty_ = sptr<RSScreenProperty>(data.ReadParcelable<RSScreenProperty>());
    if (!result->rsScreenProperty_) {
        return nullptr;
    }
    result->vsyncConn_ = message->ReadRemoteObject();
    if (!result->vsyncConn_) {
        return nullptr;
    }
    auto persistenceData = RSIpcPersistenceManager::Unmarshalling(data);
    if (!persistenceData) {
        return nullptr;
    }
    result->persistenceData_ = std::move(*persistenceData);
    return result.release();
}

bool ConnectToServiceInfo::Marshalling(Parcel& data) const
{
    MessageParcel* message = static_cast<MessageParcel*>(&data);
    if (!message->WriteRemoteObject(composerToRenderConnection_)) {
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
    result->composerToRenderConnection_ = message->ReadRemoteObject();
    if (!result->composerToRenderConnection_) {
        return nullptr;
    }
    result->vsyncToken_ = message->ReadRemoteObject();
    if (!result->vsyncToken_) {
        return nullptr;
    }
    return result.release();
}
} // namespace Rosen
} // namespace OHOS
