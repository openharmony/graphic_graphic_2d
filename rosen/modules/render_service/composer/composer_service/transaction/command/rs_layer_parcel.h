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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_LAYER_RS_LAYER_PARCEL_H
#define RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_LAYER_RS_LAYER_PARCEL_H

#include "message_parcel.h"
#include "rs_layer_common_def.h"

namespace OHOS {
namespace Rosen {
class RSRenderComposerContext;

class RSLayerParcel {
public:
    RSLayerParcel() = default;
    RSLayerParcel(const RSLayerParcel&) = delete;
    RSLayerParcel(const RSLayerParcel&&) = delete;
    RSLayerParcel& operator=(const RSLayerParcel&) = delete;
    RSLayerParcel& operator=(const RSLayerParcel&&) = delete;
    virtual ~RSLayerParcel() noexcept = default;

    virtual uint16_t GetRSLayerParcelType() const = 0;
    virtual RSLayerId GetRSLayerId() const = 0;
    virtual bool Marshalling(OHOS::MessageParcel& parcel) const = 0;
    virtual void ApplyRSLayerCmd(std::shared_ptr<RSRenderComposerContext> context) = 0;

private:
    size_t indexVerifier_ = 0;

    friend class RSLayerTransactionData;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_LAYER_RS_LAYER_PARCEL_H
