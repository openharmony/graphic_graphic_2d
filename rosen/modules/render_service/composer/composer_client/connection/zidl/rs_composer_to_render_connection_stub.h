/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_COMPOSER_CLIENT_CONNECTION_ZIDL_RS_COMPOSER_TO_RENDER_CONNECTION_STUB_H
#define RENDER_SERVICE_COMPOSER_CLIENT_CONNECTION_ZIDL_RS_COMPOSER_TO_RENDER_CONNECTION_STUB_H

#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>
#include <unordered_set>
#include "irs_composer_to_render_connection.h"

namespace OHOS {
namespace Rosen {
class RSComposerToRenderConnectionStub : public IRemoteStub<IRSComposerToRenderConnection> {
public:
    RSComposerToRenderConnectionStub() = default;
    virtual ~RSComposerToRenderConnectionStub() noexcept = default;

    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

private:
    int32_t ReleaseLayerBuffersStub(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t NotifyLppLayerToRenderStub(MessageParcel& data, MessageParcel& reply, MessageOption& option);
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_COMPOSER_CLIENT_CONNECTION_ZIDL_RS_COMPOSER_TO_RENDER_CONNECTION_STUB_H