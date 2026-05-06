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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_RS_RENDER_CONNECT_PARCEL_INFO_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_RS_RENDER_CONNECT_PARCEL_INFO_H

#include "iremote_object.h"
#include "message_parcel.h"

#include "render_process/transaction/ipc_persistence/rs_ipc_persistence_manager.h"
#include "screen_manager/rs_screen_property.h"

namespace OHOS {
namespace Rosen {
struct ReplyToRenderInfo : public Parcelable {
public:
    ReplyToRenderInfo() = default;
    ReplyToRenderInfo(const sptr<IRemoteObject>& composerConnection, const sptr<RSScreenProperty>& rsScreenProperty,
        const sptr<IRemoteObject>& vsyncConn, const std::shared_ptr<IpcPersistenceTypeToDataMap>& replayData)
        : composeConnection_(composerConnection), rsScreenProperty_(rsScreenProperty), vsyncConn_(vsyncConn),
          replayData_(replayData) {}
    ~ReplyToRenderInfo() noexcept override = default;

    bool Marshalling(Parcel& data) const override;
    [[nodiscard]] static ReplyToRenderInfo* Unmarshalling(Parcel& data);

    sptr<IRemoteObject> composeConnection_ = nullptr;
    sptr<RSScreenProperty> rsScreenProperty_ = nullptr;
    sptr<IRemoteObject> vsyncConn_ = nullptr;
    std::shared_ptr<IpcPersistenceTypeToDataMap> replayData_ = nullptr;
};

struct ConnectToServiceInfo : public Parcelable {
public:
    ConnectToServiceInfo() = default;
    ConnectToServiceInfo(const sptr<IRemoteObject>& composerToRenderConnection, const sptr<IRemoteObject>& vsyncToken)
        : composerToRenderConnection_(composerToRenderConnection), vsyncToken_(vsyncToken) {}
    ~ConnectToServiceInfo() noexcept override = default;

    bool Marshalling(Parcel& data) const override;
    [[nodiscard]] static ConnectToServiceInfo* Unmarshalling(Parcel& data);

    sptr<IRemoteObject> composerToRenderConnection_ = nullptr;
    sptr<IRemoteObject> vsyncToken_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_RS_RENDER_CONNECT_PARCEL_INFO_H