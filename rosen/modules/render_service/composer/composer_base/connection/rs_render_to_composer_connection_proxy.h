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
#ifndef RENDER_SERVICE_COMPOSER_BASE_CONNECTION_RS_RENDER_TO_COMPOSER_CONNECTION_PROXY_H
#define RENDER_SERVICE_COMPOSER_BASE_CONNECTION_RS_RENDER_TO_COMPOSER_CONNECTION_PROXY_H

#include <iremote_proxy.h>
#include "irs_render_to_composer_connection.h"
#include "sandbox_utils.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSRenderToComposerConnectionProxy : public IRemoteProxy<IRSRenderToComposerConnection> {
public:
    explicit RSRenderToComposerConnectionProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSRenderToComposerConnectionProxy() = default;

    void CommitLayers(std::unique_ptr<RSLayerTransactionData>& transactionData) override;
    void ClearFrameBuffers() override;
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId) override;
    void ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds) override;
    void SetScreenBacklight(uint32_t level) override;
private:
    bool FillParcelWithTransactionData(std::unique_ptr<RSLayerTransactionData>& transactionData,
        std::shared_ptr<MessageParcel>& data);
    RSComposerError SendLayers(std::vector<std::shared_ptr<MessageParcel>>& parcels);
    RSComposerError SendRequest(uint32_t command, MessageParcel &arg, MessageParcel &reply, MessageOption &opt);

    pid_t pid_ = GetRealPid();
    uint32_t transactionDataIndex_ = 0;
    static inline BrokerDelegator<RSRenderToComposerConnectionProxy> delegator_;

};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_BASE_CONNECTION_RS_RENDER_TO_COMPOSER_CONNECTION_PROXY_H