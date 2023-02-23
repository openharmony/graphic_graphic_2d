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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ANIMATION_MANAGER_MAP_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ANIMATION_MANAGER_MAP_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSUIAnimationManager;

class RS_EXPORT RSAnimationManagerMap final {
public:
    ~RSAnimationManagerMap() = default;
    static std::shared_ptr<RSAnimationManagerMap>& Instance();

    const std::shared_ptr<RSUIAnimationManager>& GetAnimationManager(const int32_t id);

private:
    RSAnimationManagerMap() = default;
    RSAnimationManagerMap(const RSAnimationManagerMap&) = delete;
    RSAnimationManagerMap(const RSAnimationManagerMap&&) = delete;
    RSAnimationManagerMap& operator=(const RSAnimationManagerMap&) = delete;
    RSAnimationManagerMap& operator=(const RSAnimationManagerMap&&) = delete;

    std::mutex mutex_;
    std::unordered_map<int32_t, std::shared_ptr<RSUIAnimationManager>> managerMap_;
    static std::shared_ptr<RSAnimationManagerMap> instance_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_ANIMATION_MANAGER_MAP_H
