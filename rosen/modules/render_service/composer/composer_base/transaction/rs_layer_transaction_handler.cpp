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

#include "rs_layer_transaction_handler.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
void RSLayerTransactionHandler::SetRSComposerConnection(const sptr<RSRenderToComposerConnection>& rsComposerConnection)
{
    if (rsComposerConnection == nullptr) {
        RS_LOGE("SetRsComposerConnectionProxy connection is nullptr");
        return;
    }
    rsComposerConnection_ = rsComposerConnection;
}

void RSLayerTransactionHandler::CommitRSLayerTransaction()
{
    if (rsComposerConnection_ == nullptr) {
        RS_LOGE("RSLayerTransactionHandler::CommitRSLayerTransaction param is nullptr");
        return;
    }
    auto rsLayerTransactionData = std::make_unique<RSLayerTransactionData>();
    rsLayerTransactionData->TransferData(rsLayerTransactionData_);
    rsComposerConnection_->CommitLayers(rsLayerTransactionData);
}

void RSLayerTransactionHandler::AddLayer(const std::shared_ptr<RSLayer>& rsLayer)
{
    if (rsLayerTransactionData_ == nullptr) {
        RS_LOGE("RSLayerTransactionHandler::AddLayer param is nullptr");
        return;
    }
    rsLayerTransactionData_->AddLayer(rsLayer);
}

void RSLayerTransactionHandler::ClearAllLayers()
{
    if (rsLayerTransactionData_ == nullptr) {
        RS_LOGE("RSLayerTransactionHandler::ClearAllLayers param is nullptr");
        return;
    }
    rsLayerTransactionData_->ClearAllLayers();
}
} // namespace Rosen
} // namespace OHOS