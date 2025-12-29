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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_ZIDL_RS_IRENDER_TO_SERVICE_CONNECTION_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_ZIDL_RS_IRENDER_TO_SERVICE_CONNECTION_H

#include "platform/ohos/transaction/rs_irender_connection_token.h"
#include "feature/hyper_graphic_manager/hgm_info_parcel.h"

namespace OHOS {
namespace Rosen {

class RSIRenderToServiceConnection : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.RenderToServiceConnection");

    RSIRenderToServiceConnection() = default;
    virtual ~RSIRenderToServiceConnection() noexcept = default;

    // Dfx
    virtual void ReplyDumpResultToService(std::string& dumpString) = 0;

    // Hgm
    virtual sptr<HgmServiceToProcessInfo> NotifyRpHgmFrameRate(uint64_t timestamp, uint64_t vsyncId,
        const sptr<HgmProcessToServiceInfo>& processToServiceInfo) = 0;

    // Screen Manager
    virtual void NotifyScreenSwitchFinished(ScreenId screenId) = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_ZIDL_RS_IRENDER_TO_SERVICE_CONNECTION_H