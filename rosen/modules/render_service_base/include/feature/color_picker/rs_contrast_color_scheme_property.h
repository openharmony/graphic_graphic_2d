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

#ifndef RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_CONTRAST_COLOR_SCHEME_PROPERTY_H
#define RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_CONTRAST_COLOR_SCHEME_PROPERTY_H

#include "feature/inherited_property/i_inherited_property.h"
#include "property/rs_color_picker_def.h"

namespace OHOS {
namespace Rosen {

class RSContrastColorSchemeProperty : public IInheritedProperty {
public:
    RSContrastColorSchemeProperty() = default;
    explicit RSContrastColorSchemeProperty(ContrastColorScheme value) : value_(value) {}
    ~RSContrastColorSchemeProperty() override = default;

    InheritedPropertyType GetType() const override
    {
        return InheritedPropertyType::CONTRAST_COLOR_SCHEME;
    }

    ContrastColorScheme GetValue() const
    {
        return value_;
    }

    void SetValue(ContrastColorScheme value)
    {
        value_ = value;
    }

private:
    ContrastColorScheme value_ = ContrastColorScheme::INVALID;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_CONTRAST_COLOR_SCHEME_PROPERTY_H