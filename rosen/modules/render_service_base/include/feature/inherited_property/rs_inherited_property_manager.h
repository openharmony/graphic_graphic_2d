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

#ifndef RENDER_SERVICE_BASE_FEATURE_INHERITED_PROPERTY_RS_INHERITED_PROPERTY_MANAGER_H
#define RENDER_SERVICE_BASE_FEATURE_INHERITED_PROPERTY_RS_INHERITED_PROPERTY_MANAGER_H

#include <memory>
#include <unordered_map>
#include <unistd.h>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "feature/inherited_property/i_inherited_property.h"

namespace OHOS {
namespace Rosen {

// Manages inheritable node properties (see IInheritedProperty) keyed by NodeId.
// Held as a member of RSContext; thread affinity follows its owner (RS main thread),
// so no internal locking is provided.
class RSB_EXPORT RSInheritedPropertyManager {
public:
    RSInheritedPropertyManager() = default;
    ~RSInheritedPropertyManager() = default;
    RSInheritedPropertyManager(const RSInheritedPropertyManager&) = delete;
    RSInheritedPropertyManager& operator=(const RSInheritedPropertyManager&) = delete;

    // Stores the property under its GetType(); an existing property of the same node
    // and type is overwritten. Null property or InheritedPropertyType::NONE is ignored.
    void Store(NodeId nodeId, const std::shared_ptr<IInheritedProperty>& property);

    // Returns the property of the given node and type, nullptr when absent.
    std::shared_ptr<IInheritedProperty> Get(NodeId nodeId, InheritedPropertyType type) const;

    // Type-safe read: downcasts after verifying GetType(), returns nullptr on mismatch.
    template<typename T>
    std::shared_ptr<T> GetAs(NodeId nodeId, InheritedPropertyType type) const
    {
        auto property = Get(nodeId, type);
        if (property == nullptr || property->GetType() != type) {
            return nullptr;
        }
        return std::static_pointer_cast<T>(property);
    }

    // Clears all inherited properties of the given node.
    void Clear(NodeId nodeId);

    // Clears inherited properties of all nodes belonging to the given pid.
    void ClearByPid(pid_t pid);

private:
    using PropertyMap = std::unordered_map<InheritedPropertyType, std::shared_ptr<IInheritedProperty>>;
    std::unordered_map<NodeId, PropertyMap> properties_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_FEATURE_INHERITED_PROPERTY_RS_INHERITED_PROPERTY_MANAGER_H
