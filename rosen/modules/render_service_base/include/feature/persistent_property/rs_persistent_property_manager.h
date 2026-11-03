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

#ifndef RENDER_SERVICE_BASE_FEATURE_INHERITED_PROPERTY_RS_PERSISTENT_PROPERTY_MANAGER_H
#define RENDER_SERVICE_BASE_FEATURE_INHERITED_PROPERTY_RS_PERSISTENT_PROPERTY_MANAGER_H

#include <memory>
#include <unordered_map>
#include <unistd.h>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "feature/persistent_property/i_persistent_property.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSPersistentPropertyManager {
public:
    RSPersistentPropertyManager() = default;
    ~RSPersistentPropertyManager() = default;
    RSPersistentPropertyManager(const RSPersistentPropertyManager&) = delete;
    RSPersistentPropertyManager& operator=(const RSPersistentPropertyManager&) = delete;

    void Store(NodeId nodeId, const std::shared_ptr<IPersistentProperty>& property);

    std::shared_ptr<IPersistentProperty> Get(NodeId nodeId, PersistentPropertyType type) const;

    template<typename T>
    std::shared_ptr<T> GetAs(NodeId nodeId, PersistentPropertyType type) const
    {
        auto property = Get(nodeId, type);
        if (property == nullptr || property->GetType() != type) {
            return nullptr;
        }
        return std::static_pointer_cast<T>(property);
    }

    void Clear(NodeId nodeId);

    void Clear(NodeId nodeId, PersistentPropertyType type);

    void ClearByPid(pid_t pid);

private:
    using PropertyMap = std::unordered_map<PersistentPropertyType, std::shared_ptr<IPersistentProperty>>;
    std::unordered_map<NodeId, PropertyMap> properties_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_FEATURE_INHERITED_PROPERTY_RS_PERSISTENT_PROPERTY_MANAGER_H
