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
#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_MANAGER_MAP_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_MANAGER_MAP_H

#include <mutex>
#include <unordered_map>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSModifierManager;

class RS_EXPORT RSModifierManagerMap final {
public:
    ~RSModifierManagerMap() = default;
    static std::shared_ptr<RSModifierManagerMap>& Instance();

    const std::shared_ptr<RSModifierManager>& GetModifierManager(const int32_t id);

private:
    RSModifierManagerMap() = default;
    RSModifierManagerMap(const RSModifierManagerMap&) = delete;
    RSModifierManagerMap(const RSModifierManagerMap&&) = delete;
    RSModifierManagerMap& operator=(const RSModifierManagerMap&) = delete;
    RSModifierManagerMap& operator=(const RSModifierManagerMap&&) = delete;

    std::mutex mutex_;
    std::unordered_map<int32_t, std::shared_ptr<RSModifierManager>> managerMap_;
    static std::shared_ptr<RSModifierManagerMap> instance_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_MANAGER_MAP_H
