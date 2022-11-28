/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "modifier/rs_modifier_manager_map.h"

#include "modifier/rs_modifier_manager.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSModifierManagerMap> RSModifierManagerMap::instance_ =
    std::shared_ptr<RSModifierManagerMap>(new RSModifierManagerMap());

std::shared_ptr<RSModifierManagerMap>& RSModifierManagerMap::Instance()
{
    return instance_;
}

const std::shared_ptr<RSModifierManager>& RSModifierManagerMap::GetModifierManager(const int32_t id)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto& manager = managerMap_[id];
    if (manager == nullptr) {
        manager = std::make_shared<RSModifierManager>();
    }
    return manager;
}
} // namespace Rosen
} // namespace OHOS
