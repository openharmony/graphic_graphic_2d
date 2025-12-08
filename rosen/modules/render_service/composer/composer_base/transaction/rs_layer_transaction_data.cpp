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

#include "rs_layer_transaction_data.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

std::vector<std::shared_ptr<RSLayer>> RSLayerTransactionData::GetRSLayers()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return std::move(layersVec_);
}

void RSLayerTransactionData::AddLayer(const std::shared_ptr<RSLayer>& rsLayer)
{
    std::unique_lock<std::mutex> lock(mutex_);
    layersVec_.emplace_back(rsLayer);
}

void RSLayerTransactionData::ClearAllLayers()
{
    std::unique_lock<std::mutex> lock(mutex_);
    layersVec_.clear();
}

void RSLayerTransactionData::TransferData(std::unique_ptr<RSLayerTransactionData>& transactionData)
{
    std::unique_lock<std::mutex> lock(mutex_);
    layersVec_ = transactionData->GetRSLayers();
    RS_TRACE_NAME_FMT("RSLayerTransactionData::TransferData layerSize:%d", layersVec_.size());
}
} // namespace Rosen
} // namespace OHOS