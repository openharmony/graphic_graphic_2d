/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "rs_trace.h"

#include "common/rs_special_layer_manager.h"
#include "platform/common/rs_log.h"
#include "set"

namespace OHOS {
namespace Rosen {
bool RSSpecialLayerManager::Set(uint32_t type, bool set)
{
    if (Find(type) == set) {
        return false;
    }
    if (set) {
        specialLayerType_ |= type;
    } else {
        specialLayerType_ &= ~type;
    }
    return true;
}

bool RSSpecialLayerManager::Find(uint32_t type) const
{
    return specialLayerType_ & type;
}

uint32_t RSSpecialLayerManager::Get() const
{
    return specialLayerType_;
}

void RSSpecialLayerManager::AddIds(uint32_t type, NodeId id)
{
    uint32_t isType = type & IS_GENERAL_SPECIAL;
    uint32_t currentType = SpecialLayerType::SECURITY;
    while (isType != 0) {
        auto IsSpecial = isType & 1;
        if (IsSpecial) {
            specialLayerIds_[currentType].insert(id);
            specialLayerType_ |= (currentType << SPECIAL_TYPE_NUM);
        }
        isType >>= 1;
        currentType <<= 1;
    }
}

void RSSpecialLayerManager::RemoveIds(uint32_t type, NodeId id)
{
    uint32_t isType = type & IS_GENERAL_SPECIAL;
    uint32_t currentType = SpecialLayerType::SECURITY;
    while (isType != 0) {
        auto IsSpecial = isType & 1;
        if (IsSpecial) {
            specialLayerIds_[currentType].erase(id);
            if (specialLayerIds_[currentType].empty()) {
                specialLayerType_ &= ~(currentType << SPECIAL_TYPE_NUM);
            }
        }
        isType >>= 1;
        currentType <<= 1;
    }
}
} // namespace Rosen
} // namespace OHOS