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

#include "rs_layer_parcel_factory.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
inline uint32_t MakeKey(uint16_t commandType)
{
    return ((uint32_t)commandType);
}

RSLayerParcelFactory& RSLayerParcelFactory::Instance()
{
    static RSLayerParcelFactory instance;
    return instance;
}

void RSLayerParcelFactory::Register(uint16_t type, UnmarshallingFunc func)
{
    auto result = unmarshallingFuncLUT_.try_emplace(MakeKey(type), func);
    if (!result.second) {
        ROSEN_LOGD("RSLayerParcelFactory Register faile, type:%{public}d", type);
    }
}

UnmarshallingFunc RSLayerParcelFactory::GetUnmarshallingFunc(uint16_t type)
{
    auto it = unmarshallingFuncLUT_.find(MakeKey(type));
    if (it == unmarshallingFuncLUT_.end()) {
        ROSEN_LOGE("RSLayerParcelFactory GetUnmarshallingFunc type:%{public}d not found", type);
        return nullptr;
    }
    return it->second;
}
} // namespace Rosen
} // namespace OHOS
