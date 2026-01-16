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

#undef LOG_TAG
#define LOG_TAG "RSLayerTransactionHandler"
namespace OHOS {
namespace Rosen {
void RSLayerTransactionHandler::SetRSComposerConnectionProxy(const sptr<IRSRenderToComposerConnection>& rsComposerConnection)
{
    if (rsComposerConnection == nullptr) {
        RS_LOGE("%{public}s connection is nullptr", __func__);
        return;
    }
    rsComposerConnection_ = rsComposerConnection;
}

void RSLayerTransactionHandler::AddRSLayerParcel(std::shared_ptr<RSLayerParcel>& layerParcel, RSLayerId layerId)
{
    if (rsLayerTransactionData_ == nullptr || layerParcel == nullptr) {
        RS_LOGE("%{public}s param is nullptr", __func__);
        return;
    }
    rsLayerTransactionData_->AddRSLayerParcel(layerParcel, layerId);
}

bool RSLayerTransactionHandler::CommitRSLayerTransaction(ComposerInfo& composerInfo, uint64_t timestamp, const std::string& abilityName)
{
    timestamp_ = std::max(timestamp_, timestamp);
    if (rsComposerConnection_ == nullptr || rsLayerTransactionData_->IsEmpty()) {
        RS_LOGE("%{public}s param is nullptr", __func__);
        return false;
    }
    rsLayerTransactionData_->timestamp_ = timestamp;
    rsLayerTransactionData_->SetComposerInfo(composerInfo);
    if (!rsComposerConnection_->CommitLayers(rsLayerTransactionData_)) {
        RS_LOGE("%{public}s CommitLayers failed", __func__);
        return false;
    }
    rsLayerTransactionData_ = std::make_unique<RSLayerTransactionData>();
    return true;
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