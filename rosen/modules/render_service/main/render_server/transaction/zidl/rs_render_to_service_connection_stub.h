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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_ZIDL_RS_RENDER_TO_SERVICE_CONNECTION_STUB_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_ZIDL_RS_RENDER_TO_SERVICE_CONNECTION_STUB_H

#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>

#include "rs_irender_to_service_connection.h"

namespace OHOS {
namespace Rosen {
class RSRenderToServiceConnectionStub : public IRemoteStub<RSIRenderToServiceConnection> {
public:
    RSRenderToServiceConnectionStub() = default;
    virtual ~RSRenderToServiceConnectionStub() noexcept = default;

    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option);
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_ZIDL_RS_RENDER_TO_SERVICE_CONNECTION_STUB_H