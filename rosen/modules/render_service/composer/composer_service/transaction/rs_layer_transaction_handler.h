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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_RS_LAYER_TRANSACTION_HANDLER_H
#define RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_RS_LAYER_TRANSACTION_HANDLER_H

#include <event_handler.h>
#include <memory>
#include <queue>
#include <stack>
#include "rs_layer_parcel.h"
#include "rs_layer_transaction_data.h"
#include "rs_render_to_composer_connection_proxy.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSLayerTransactionHandler final {
public:
    RSLayerTransactionHandler() = default;
    ~RSLayerTransactionHandler() = default;

    void SetRSComposerConnectionProxy(const sptr<IRSRenderToComposerConnection>& rsComposerConnection);
    void AddRSLayerParcel(RSLayerId layerId, std::shared_ptr<RSLayerParcel>& layerParcel);
    bool CommitRSLayerTransaction(ComposerInfo& composerInfo,
        uint64_t timestamp = 0, const std::string& abilityName = "");
    bool IsEmpty() const;

private:
    RSLayerTransactionHandler(const RSLayerTransactionHandler&) = delete;
    RSLayerTransactionHandler(const RSLayerTransactionHandler&&) = delete;
    RSLayerTransactionHandler& operator=(const RSLayerTransactionHandler&) = delete;
    RSLayerTransactionHandler& operator=(const RSLayerTransactionHandler&&) = delete;

    std::unique_ptr<RSLayerTransactionData> rsLayerTransactionData_ = std::make_unique<RSLayerTransactionData>();
    sptr<IRSRenderToComposerConnection> rsComposerConnection_;
    uint64_t timestamp_ = 0;
};

} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_RS_LAYER_TRANSACTION_HANDLER_H