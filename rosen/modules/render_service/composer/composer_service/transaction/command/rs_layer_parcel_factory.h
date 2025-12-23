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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_LAYER_PARCEL_FACTORY_H
#define RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_LAYER_PARCEL_FACTORY_H

#include <unordered_map>
#include "common/rs_macros.h"
#include "message_parcel.h"

namespace OHOS {
namespace Rosen {
class RSLayerParcel;

using UnmarshallingFunc = RSLayerParcel* (*)(OHOS::MessageParcel& parcel);

class RSB_EXPORT RSLayerParcelFactory {
public:
    static RSLayerParcelFactory& Instance();

    void Register(uint16_t type, UnmarshallingFunc func);
    UnmarshallingFunc GetUnmarshallingFunc(uint16_t type);

private:
    RSLayerParcelFactory() = default;
    ~RSLayerParcelFactory() = default;

    std::unordered_map<uint32_t, UnmarshallingFunc> unmarshallingFuncLUT_;
};

// helper class for automatically registry
template<uint16_t rsLayerParacelType, UnmarshallingFunc func>
class RSLayerParcelRegister {
public:
    RSLayerParcelRegister()
    {
        RSLayerParcelFactory::Instance().Register(rsLayerParacelType, func);
    }
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_LAYER_PARCEL_FACTORY_H
