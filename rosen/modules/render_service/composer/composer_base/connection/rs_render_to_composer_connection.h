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
#ifndef RENDER_SERVICE_COMPOSER_BASE_CONNECTION_RS_RENDER_TO_COMPOSER_CONNECTION_H
#define RENDER_SERVICE_COMPOSER_BASE_CONNECTION_RS_RENDER_TO_COMPOSER_CONNECTION_H

#include "rs_layer_transaction_data.h"
#include "rs_render_composer_agent.h"

namespace OHOS::Rosen {
class RSRenderToComposerConnection : public RefBase {
public:
    RSRenderToComposerConnection(const std::string& name, uint64_t screenId,
        std::shared_ptr<RSRenderComposerAgent> rsRenderComposerAgent);
    ~RSRenderToComposerConnection() noexcept override = default;

    void CommitLayers(std::unique_ptr<RSLayerTransactionData>& transactionData);

private:
    uint64_t screenId_ = 0;
    std::shared_ptr<RSRenderComposerAgent> rsRenderComposerAgent_;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_BASE_CONNECTION_RS_RENDER_TO_COMPOSER_CONNECTION_H