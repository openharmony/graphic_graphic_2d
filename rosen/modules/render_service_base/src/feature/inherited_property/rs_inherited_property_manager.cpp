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

#include "feature/inherited_property/rs_inherited_property_manager.h"

namespace OHOS {
namespace Rosen {

void RSInheritedPropertyManager::Store(NodeId nodeId, const std::shared_ptr<IInheritedProperty>& property)
{
    if (property == nullptr || property->GetType() == InheritedPropertyType::NONE) {
        return;
    }
    properties_[nodeId][property->GetType()] = property;
}

std::shared_ptr<IInheritedProperty> RSInheritedPropertyManager::Get(NodeId nodeId, InheritedPropertyType type) const
{
    auto nodeIt = properties_.find(nodeId);
    if (nodeIt == properties_.end()) {
        return nullptr;
    }
    auto propertyIt = nodeIt->second.find(type);
    if (propertyIt == nodeIt->second.end()) {
        return nullptr;
    }
    return propertyIt->second;
}

void RSInheritedPropertyManager::Clear(NodeId nodeId)
{
    properties_.erase(nodeId);
}

void RSInheritedPropertyManager::Clear(NodeId nodeId, InheritedPropertyType type)
{
    auto nodeIt = properties_.find(nodeId);
    if (nodeIt == properties_.end()) {
        return;
    }
    nodeIt->second.erase(type);
    if (nodeIt->second.empty()) {
        properties_.erase(nodeIt);
    }
}

void RSInheritedPropertyManager::ClearByPid(pid_t pid)
{
    for (auto it = properties_.begin(); it != properties_.end();) {
        if (ExtractPid(it->first) == pid) {
            it = properties_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace Rosen
} // namespace OHOS
