/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_COMPOSER_BASE_CONNECTION_RS_RENDER_TO_COMPOSER_CONNECTION_STUB_H
#define RENDER_SERVICE_COMPOSER_BASE_CONNECTION_RS_RENDER_TO_COMPOSER_CONNECTION_STUB_H

#include <iremote_stub.h>
#include "message_parcel.h"
#include "message_option.h"
#include "irs_render_to_composer_connection.h"

namespace OHOS::Rosen {
class RSRenderToComposerConnectionStub : public IRemoteStub<IRSRenderToComposerConnection> {
public:
    virtual int32_t OnRemoteRequest(uint32_t code, OHOS::MessageParcel& data,
        OHOS::MessageParcel& reply, MessageOption& option) override;

private:
    std::set<uint64_t> ParseClearRedrawCacheBufferIds(OHOS::MessageParcel& parcel);
    std::unique_ptr<RSLayerTransactionData> ParseTransactionData(OHOS::MessageParcel& parcel);
    int32_t GetCleanLayerBufferSurfaceId(OHOS::MessageParcel& parcel, uint64_t& surfaceId);
    int32_t GetBacklightLevel(OHOS::MessageParcel& parcel, uint32_t& level);
    pid_t lastSendingPid_ = 0;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_BASE_CONNECTION_RS_RENDER_TO_COMPOSER_CONNECTION_STUB_H