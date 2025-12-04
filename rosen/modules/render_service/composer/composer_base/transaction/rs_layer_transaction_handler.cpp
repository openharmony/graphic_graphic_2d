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
#include "rs_render_to_composer_connection_proxy.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
void RSLayerTransactionHandler::SetRSComposerConnectionProxy(const sptr<IRSRenderToComposerConnection>& rsComposerConnection)
{
    if (rsComposerConnection == nullptr) {
        RS_LOGE("SetRsComposerConnectionProxy connection is nullptr");
        return;
    }
    rsComposerConnection_ = rsComposerConnection;
}

void RSLayerTransactionHandler::AddRsLayerParcel(std::shared_ptr<RSLayerParcel>& layerParcel, RSLayerId layerId)
{
    if (rsLayerTransactionData_ == nullptr || layerParcel == nullptr) {
        RS_LOGE("RSLayerTransactionHandler::AddRsLayerParcel param is nullptr");
        return;
    }
    rsLayerTransactionData_->AddRsLayerParcel(layerParcel, layerId);
}

void RSLayerTransactionHandler::CheckScreenInfoIsChanged(ScreenInfo& screenInfo)
{
    if (memcmp(&screenInfo_, &screenInfo, sizeof(ScreenInfo)) != 0) {
        rsLayerTransactionData_->SetIsScreenInfoChanged(true);
        rsLayerTransactionData_->SetScreenInfo(screenInfo);
        screenInfo_ = screenInfo;
    }
}

void RSLayerTransactionHandler::CommitRsLayerTransaction(CommitLayerInfo& commitLayerInfo, uint64_t timestamp, const std::string& abilityName)
{
    timestamp_ = std::max(timestamp_, timestamp);
    if (rsComposerConnection_ == nullptr || rsLayerTransactionData_->IsEmpty()) {
        RS_LOGE("RSLayerTransactionHandler::CommitRsLayerTransaction param is nullptr");
        return;
    }
    rsLayerTransactionData_->timestamp_ = timestamp;
    CheckScreenInfoIsChanged(commitLayerInfo.screenInfo);
    rsLayerTransactionData_->SetPipelineParam(commitLayerInfo.pipelineParam);
    rsComposerConnection_->CommitLayers(rsLayerTransactionData_);
    rsLayerTransactionData_ = std::make_unique<RSLayerTransactionData>();
}

bool RSLayerTransactionHandler::IsEmpty() const
{
    bool isEmpty = true;
    if (rsLayerTransactionData_) {
        isEmpty &= rsLayerTransactionData_->IsEmpty();
    }
    return isEmpty;
}
} // namespace Rosen
} // namespace OHOS