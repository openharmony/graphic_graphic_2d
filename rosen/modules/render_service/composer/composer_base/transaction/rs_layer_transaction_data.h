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

#ifndef RENDER_SERVICE_COMPOSER_BASE_TRANSACTION_RS_LAYER_TRANSACTION_DATA_H
#define RENDER_SERVICE_COMPOSER_BASE_TRANSACTION_RS_LAYER_TRANSACTION_DATA_H

#include <memory>
#include <mutex>
#include <vector>
#include "common/rs_macros.h"
#include "message_parcel.h"
#include "params/rs_render_params.h"
#include "rs_layer.h"

namespace OHOS::Rosen {
class RSLayerContext;

class RSB_EXPORT RSLayerTransactionData {
public:
    RSLayerTransactionData() = default;
    RSLayerTransactionData(const RSLayerTransactionData&) = delete;
    RSLayerTransactionData& operator=(const RSLayerTransactionData&) = delete;
    ~RSLayerTransactionData() = default;

    std::vector<std::shared_ptr<RSLayer>> GetRSLayers();
    void AddLayer(const std::shared_ptr<RSLayer>& rsLayer);
    void ClearAllLayers();
    void TransferData(std::unique_ptr<RSLayerTransactionData>& transactionData);

private:
    std::mutex mutex_;
    std::vector<std::shared_ptr<RSLayer>> layersVec_;
    friend class RSLayerTransactionHandler;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_BASE_TRANSACTION_RS_LAYER_TRANSACTION_DATA_H