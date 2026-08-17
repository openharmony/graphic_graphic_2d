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

#ifndef RENDER_SERVICE_BASE_FEATURE_INHERITED_PROPERTY_I_INHERITED_PROPERTY_H
#define RENDER_SERVICE_BASE_FEATURE_INHERITED_PROPERTY_I_INHERITED_PROPERTY_H

#include <cstdint>

namespace OHOS {
namespace Rosen {

// Extensible property type discriminator, append new entries at the end.
enum class InheritedPropertyType : uint32_t {
    NONE = 0,
    CONTRAST_COLOR_SCHEME,
};

// Unified base class for inheritable node properties (e.g. ColorPicker-like attributes).
// RSInheritedPropertyManager manages different property types through this interface with
// identical parameter and return types.
class IInheritedProperty {
public:
    virtual ~IInheritedProperty() = default;

    // Property type discriminator, used for safe downcasting (see RSInheritedPropertyManager::GetAs).
    virtual InheritedPropertyType GetType() const = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_FEATURE_INHERITED_PROPERTY_I_INHERITED_PROPERTY_H
